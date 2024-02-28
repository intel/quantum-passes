//===- RewriteQExpr.cpp ---------------------------------------------------===//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
//
// Clang plugin for QExprs, which analyzes each Call Instruction that returns
// a QExpr value, and analyzes the AST of the BinaryOperator, UnaryOperators
// and makes sure that each expression evaluates to a QExpr value.  It also
// rewrites each operator (*, +, !, -, ~) to a built-in function for QExpr in
// the AST.
//
//===----------------------------------------------------------------------===//

#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/Sema/Sema.h"
#include "clang/Sema/SemaDiagnostic.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/DenseSet.h"
#include "llvm/Support/PrettyStackTrace.h"
#include "llvm/Support/Signals.h"
#include "llvm/Support/raw_ostream.h"
#include <map>
#include <vector>

using namespace clang;

namespace {

static const char *IntelMsg =
    "\nPlease report bugs to  " BUG_REPORT_URL ".\n\n";

static void ErrorHandler(void *) { llvm::errs() << llvm::getBugReportMsg(); }

class QExprRewriterVisitor : public RecursiveASTVisitor<QExprRewriterVisitor> {
private:
  DiagnosticsEngine *Diags = nullptr;
  unsigned BadOp = -1;
  unsigned BadQExprArg = -1;
  unsigned BadQExprCall = -1;
  unsigned DoubleReturns = -1;
  unsigned NumReturns = 0;
  bool ReturnErrorReported = false;

public:
  ASTContext *Ctx = nullptr;
  bool ErrorFound = false;
  std::map<std::string, FunctionDecl *> NameToFunctionDecl;
  bool FunctionReturnQExpr = false;

  void SetUpDiags(DiagnosticsEngine &DiagsIn) {
    Diags = &DiagsIn;
    BadOp = Diags->getCustomDiagID(DiagnosticsEngine::Error,
                                   "invalid operator for QExpr operations");
    BadQExprCall = Diags->getCustomDiagID(DiagnosticsEngine::Error,
                                          "function must return a QExpr");
    BadQExprArg = Diags->getCustomDiagID(DiagnosticsEngine::Error,
                                         "value is not a QExpr expression");
    DoubleReturns = Diags->getCustomDiagID(
        DiagnosticsEngine::Error,
        "QExpr functions must only include one return statement.");
  }

  void resetNumReturns() {
    NumReturns = 0;
    ReturnErrorReported = false;
  }

  unsigned getNumReturns() { return NumReturns; }

  void reportReturnDiag(FunctionDecl *FD) {
    Diags->Report(FD->getBeginLoc(), DoubleReturns);
  }

  /// Create a function call for the function name and arguments provided
  /// from the provided operator. This is fetched from the functions
  /// previously found that return a QExpr value.
  /// \param FunctionName - the fully qualified name of the function.
  /// \param Arguments - the operands involved in the expression.
  /// \returns A CallExpr to the function that performs the given
  /// operation.
  CallExpr *GetFunctionRefForOperator(std::string &FunctionName,
                                      std::vector<Expr *> &Arguments) {
    // Check whether we have a matching function declaration for the function
    // name provided.
    auto It = NameToFunctionDecl.find(FunctionName);
    if (It == NameToFunctionDecl.end())
      return nullptr;

    FunctionDecl *callOperatorDecl = It->second;
    // Create a DeclRefExpr to the function declaration, which is needed
    // for the function call itself.
    DeclRefExpr *callOperatorDeclRef = DeclRefExpr::Create(
        /* Ctx =*/*Ctx,
        /* QualifierLoc =*/NestedNameSpecifierLoc(),
        /* TemplateKWLoc =*/SourceLocation(), callOperatorDecl,
        /* RefersToEnclosingVariableOrCapture=*/false,
        /* NameLoc =*/SourceLocation(),
        /* T =*/callOperatorDecl->getType(),
        /* VK =*/VK_LValue);
    QualType pToFunc = Ctx->getPointerType(callOperatorDecl->getType());
    // This is required to make a valid call to the function.
    ImplicitCastExpr *ICE = ImplicitCastExpr::Create(
        *Ctx, pToFunc, CK_FunctionToPointerDecay, callOperatorDeclRef, nullptr,
        VK_PRValue, FPOptionsOverride());
    const auto *FT = callOperatorDecl->getType()->castAs<FunctionType>();

    // Create the arguments, updating values to be reference types as needed,
    // as well as casting integer literals and boolean values.
    std::vector<Expr *> TemporaryArguments;
    for (Expr *E : Arguments) {
      if (isa<IntegerLiteral>(E)) {
        ImplicitCastExpr *IntegerICE = ImplicitCastExpr::Create(
            *Ctx, Ctx->UnsignedIntTy, CK_IntegralCast, E, nullptr, VK_PRValue,
            FPOptionsOverride());
        TemporaryArguments.push_back(IntegerICE);
        continue;
      } else if (E->getType()->isReferenceType()) {
        auto MTE = new (Ctx) MaterializeTemporaryExpr(E->getType(), E, true);
        TemporaryArguments.push_back(MTE);
        continue;
      } else if (FunctionName == "invert" && isa<ImplicitCastExpr>(E)) {
        ImplicitCastExpr *ArgICE = dyn_cast<ImplicitCastExpr>(E);
        TemporaryArguments.push_back(ArgICE->getSubExpr());
        continue;
      }
      TemporaryArguments.push_back(E);
    }

    // Create the actual call instruction here.
    CallExpr *CE = CallExpr::Create(*Ctx, ICE, TemporaryArguments,
                                    FT->getCallResultType(*Ctx), VK_PRValue,
                                    SourceLocation(), FPOptionsOverride());
    return CE;
  }

  /// Look at a given expression and determine whether it can return a
  /// QExpr value, and rewrite binary operators as necessary/
  ///
  /// \param ArgE - The expression to be analyzed, usually the argument of
  /// a function call or the return value.
  /// \param TopLevel - set to true when the expression is a call and is
  /// the first call to this function. If it is not, it needs to return
  /// a QExpr to retain correctness.
  /// \returns An (potentially rewritten) pointer to an expression
  /// that is equivalent to \p ArgE.
  Expr *AnalyzeExpr(Expr *ArgE, bool TopLevel = false) {
    Expr *E;
    // If we have a cast or reference type, we get the subtype as needed.
    if (MaterializeTemporaryExpr *MTE =
            dyn_cast<MaterializeTemporaryExpr>(ArgE)) {
      E = MTE->getSubExpr();
    } else if (FullExpr *FE = dyn_cast<FullExpr>(ArgE)) {
      E = FE->getSubExpr();
    } else if (ImplicitCastExpr *ICE = dyn_cast<ImplicitCastExpr>(ArgE)) {
      E = ICE->getSubExpr();
    } else
      E = ArgE;

    if (BinaryOperator *BO = dyn_cast<BinaryOperator>(E)) {
      FullSourceLoc FullLocation = Ctx->getFullLoc(BO->getOperatorLoc());
      // Check each side of the binary operator, this returns a nullptr if it
      // failed.

      Expr *RevisedLHS = AnalyzeExpr(BO->getLHS());
      Expr *RevisedRHS = nullptr;
      if (BO->getOpcode() != BO_Xor) {
        RevisedRHS = AnalyzeExpr(BO->getRHS());
      } else {
        RevisedRHS = BO->getRHS();
      }

      if (ErrorFound)
        return nullptr;

      // Iterate over the valid opcodes, reversing the arguments as needed
      // for the particular function. Mark the name of the function we want
      // to replace the opcode with.
      CallExpr *CE = nullptr;
      std::vector<Expr *> Arguments = {RevisedLHS, RevisedRHS};
      std::string FunctionName;
      switch (BO->getOpcode()) {
      case BO_Mul:
        // A * B --> join(B, A)
        Arguments[0] = RevisedRHS;
        Arguments[1] = RevisedLHS;
        FunctionName = "qexpr::join";
        break;
      case BO_Add:
        // A + B --> join(A, B)
        FunctionName = "qexpr::join";
        break;
      case BO_Xor:
        Arguments[0] = RevisedRHS;
        Arguments[1] = RevisedLHS;
        FunctionName = "qexpr::power";
        break;
      case BO_Shr:
        // A >> B --> bind(B, A)
        Arguments[0] = RevisedRHS;
        Arguments[1] = RevisedLHS;
        FunctionName = "qexpr::bind";
        break;
      case BO_Shl:
        // A << B --> bind(A, B)
        FunctionName = "qexpr::bind";
        break;
      default:
        break;
      }

      if (FunctionName != "")
        CE = GetFunctionRefForOperator(FunctionName, Arguments);

      if (!CE) {
        Diags->Report(BO->getOperatorLoc(), BadOp);
        ErrorFound = true;
        return nullptr;
      }

      // If the original function was a reference type, make sure we replace it.
      if (MaterializeTemporaryExpr *OldMTE =
              dyn_cast<MaterializeTemporaryExpr>(ArgE)) {
        auto MTE = new (Ctx) MaterializeTemporaryExpr(
            CE->getType(), CE, OldMTE->isBoundToLvalueReference());
        return MTE;
      } else if (FullExpr *FE = dyn_cast<FullExpr>(ArgE)) {
        FE->setSubExpr(CE);
        return FE;
      }

      return CE;

    } else if (UnaryOperator *UO = dyn_cast<UnaryOperator>(E)) {
      FullSourceLoc FullLocation = Ctx->getFullLoc(UO->getOperatorLoc());
      Expr *RevisedSubExpr = AnalyzeExpr(UO->getSubExpr());

      if (ErrorFound)
        return nullptr;

      // Iterate over the valid opcodes, reversing the arguments as needed
      // for the particular function. Mark the name of the function we want
      // to replace the opcode with.
      CallExpr *CE = nullptr;
      std::vector<Expr *> Arguments = {RevisedSubExpr};
      std::string FunctionName;
      bool LogicalNot = false;
      switch (UO->getOpcode()) {
      case UO_LNot: {
        // In the case of the logical not operator, we need
        // to remove the implicit cast from integer to boolean
        // since the placement function will be looking for
        // an integer.
        LogicalNot = true;
        if (ImplicitCastExpr *ICE =
                dyn_cast<ImplicitCastExpr>(RevisedSubExpr)) {
          if (ICE->getCastKind() == CK_IntegralToBoolean)
            Arguments[0] = ICE->getSubExpr();
        }
      }
      case UO_Minus:
      case UO_Not:
        FunctionName = "qexpr::invert";
        break;
      default:
        break;
      }

      if (FunctionName != "")
        CE = GetFunctionRefForOperator(FunctionName, Arguments);

      if (!CE) {
        Diags->Report(UO->getOperatorLoc(), BadOp);
        ErrorFound = true;
        return nullptr;
      }

      // If the original function was a reference type, make sure we replace it.
      if (MaterializeTemporaryExpr *OldMTE =
              dyn_cast<MaterializeTemporaryExpr>(ArgE)) {
        auto MTE = new (Ctx) MaterializeTemporaryExpr(
            CE->getType(), CE, OldMTE->isBoundToLvalueReference());
        return MTE;
      } else if (FullExpr *FE = dyn_cast<FullExpr>(ArgE)) {
        FE->setSubExpr(CE);
        return FE;
      }

      return CE;
    } else if (ParenExpr *PE = dyn_cast<ParenExpr>(E)) {
      // Parentheses, we just need to check the inside and process it
      // separately, and return it.
      Expr *E = PE->getSubExpr();
      Expr *NewSub = AnalyzeExpr(E);
      if (!NewSub)
        return nullptr;
      PE->setSubExpr(NewSub);
    } else if (ConditionalOperator *CO = dyn_cast<ConditionalOperator>(E)) {
      FullSourceLoc FullLocation = Ctx->getFullLoc(CO->getBeginLoc());
      // Check each side of the binary operator, this returns a nullptr if it
      // failed.

      Expr *Condition = CO->getCond();
      Expr *RevisedTrue = AnalyzeExpr(CO->getTrueExpr());
      Expr *RevisedFalse = AnalyzeExpr(CO->getFalseExpr());
      if (ErrorFound)
        return nullptr;

      // Iterate over the valid opcodes, reversing the arguments as needed
      // for the particular function. Mark the name of the function we want
      // to replace the opcode with.
      CallExpr *CE = nullptr;
      std::vector<Expr *> Arguments = {Condition, RevisedTrue, RevisedFalse};
      std::string FunctionName = "qexpr::cIf";

      if (FunctionName != "")
        CE = GetFunctionRefForOperator(FunctionName, Arguments);

      // If the original function was a reference type, make sure we replace it.
      if (MaterializeTemporaryExpr *OldMTE =
              dyn_cast<MaterializeTemporaryExpr>(ArgE)) {
        auto MTE = new (Ctx) MaterializeTemporaryExpr(
            CE->getType(), CE, OldMTE->isBoundToLvalueReference());
        return MTE;
      } else if (FullExpr *FE = dyn_cast<FullExpr>(ArgE)) {
        FE->setSubExpr(CE);
        return FE;
      }

      return CE;
    } else if (CallExpr *CE = dyn_cast<CallExpr>(E)) {
      // For each call instruction, we either stop our recursion, or process the
      // arguments if this is the first time the function has been called.

      Expr *BaseCallee = CE->getCallee();
      FunctionDecl *FD = CE->getDirectCallee();
      std::vector<const Type *> ParamTypes;
      const Type *ReturnType = nullptr;
      // This handles the case of whether the call is indirect, or couldn't be
      // resolved.
      if (!FD) {
        Decl *CalleeDecl = CE->getCalleeDecl();
        if (!CalleeDecl) {
          return ArgE;
        }
        // Look up the function if it was unresolved., if we have found the name
        // previously, we can grab the function declaration from there.
        // Otherwise, return.
        if (UnresolvedLookupExpr *URE =
                dyn_cast<UnresolvedLookupExpr>(BaseCallee)) {
          auto It = NameToFunctionDecl.find(
              URE->getName().getAsIdentifierInfo()->getName().str());
          if (It == NameToFunctionDecl.end()) {
            return ArgE;
          }
          FD = It->second;
        } else if (ParmVarDecl *PVD = dyn_cast<ParmVarDecl>(CalleeDecl)) {
          // If it's because this is an indirect call and the function was
          // passed in, analyze the type and get the return type.
          const PointerType *PointerTemp =
              dyn_cast<PointerType>(PVD->getType().getTypePtr());
          if (!PointerTemp)
            return ArgE;
          const ParenType *ParenTemp =
              dyn_cast<ParenType>(PointerTemp->getPointeeType().getTypePtr());
          const FunctionProtoType *FPT = dyn_cast<FunctionProtoType>(
              ParenTemp->getInnerType().getTypePtr());
          for (QualType Param : FPT->param_types())
            ParamTypes.push_back(Param.getTypePtr());
          ReturnType = FPT->getReturnType().getTypePtr();
        } else
          return ArgE;
      }

      // If it is valid, get the return type and the parameter types.
      if (FD) {
        ReturnType = FD->getReturnType().getTypePtr();
        for (ParmVarDecl *Param : FD->parameters())
          ParamTypes.push_back(Param->getOriginalType().getTypePtr());
      }

      if (!TopLevel) {
        // If this is not the first call, it needs to return a QExpr.

        // If it does, continue, otherwise we need to mark it as an error and
        // return an error message.

        if (const ElaboratedType *ET = dyn_cast<ElaboratedType>(ReturnType)) {
          QualType Temp = ET->getNamedType();
          ReturnType = Temp.getTypePtr();
        }
        const TypedefType *TDT = dyn_cast<TypedefType>(ReturnType);
        if (TDT) {
          TypedefNameDecl *TDNM = TDT->getDecl();
          StringRef Name = TDNM->getName();
          if (Name != "QExpr")
            ErrorFound = true;
        } else {
          ErrorFound = true;
        }

        if (ErrorFound) {
          Diags->Report(ArgE->getExprLoc(), BadQExprCall);
          return nullptr;
        }
        return ArgE;
      }

      // This handles the case of an function dereference.  We simply start our
      // iteration from a different point in this case.
      unsigned IsACXXOperator = isa<CXXOperatorCallExpr>(CE) ? 1 : 0;

      // Iterate over the different arguments.

      for (unsigned CEIdx = IsACXXOperator, FDIdx = 0, Edx = CE->getNumArgs();
           CEIdx < Edx; ++CEIdx, ++FDIdx) {
        // This should only happen if the called function has variadic
        // parameters.
        if (FDIdx >= ParamTypes.size())
          break;

        const Type *PT = ParamTypes[FDIdx];
        if (const ReferenceType *RT = dyn_cast<ReferenceType>(PT))
          PT = RT->getPointeeType().getTypePtr();

        if (const ElaboratedType *ET = dyn_cast<ElaboratedType>(PT)) {
          QualType Temp = ET->getNamedType();
          PT = Temp.getTypePtr();
        }
        const TypedefType *PTDT = dyn_cast<TypedefType>(PT);
        if (!PTDT) {
          continue;
        }

        // If the parameter is a QExpr, we analyze it to make sure everything
        // the operators are correct and are rewritten.
        TypedefNameDecl *PTDNM = PTDT->getDecl();
        StringRef Name = PTDNM->getName();
        if (Name != "QExpr")
          continue;
        Expr *Arg = CE->getArg(CEIdx);
        Expr *NewArg = AnalyzeExpr(Arg);
        if (!NewArg)
          continue;
        // If the argument was changed, we replace it.
        CE->setArg(CEIdx, NewArg);
      }
    } else {
      const Type *T = ArgE->getType().getTypePtr();
      if (const ElaboratedType *ET = dyn_cast<ElaboratedType>(T)) {
        QualType Temp = ET->getNamedType();
        T = Temp.getTypePtr();
      }
      const TypedefType *TDT = dyn_cast<TypedefType>(T);
      if (TDT) {
        TypedefNameDecl *TDNM = TDT->getDecl();
        StringRef Name = TDNM->getName();
        if (Name != "QExpr")
          ErrorFound = true;
      } else {
        ErrorFound = true;
      }
      if (ErrorFound)
        Diags->Report(ArgE->getExprLoc(), BadQExprArg);
    }

    return !ErrorFound ? ArgE : nullptr;
  }

  /// Analyzing the top level statments, checking for CallExprs and return
  /// statements to ensure that they match QExprs and rewriting the overloaded
  /// functions.
  ///
  /// \param S - The statement we are currently analyzing.
  /// \returns true if everything resolves correctly.
  bool VisitStmt(Stmt *S) {

    // If we have a call expression, analyze it, and mark it as
    // the first call.
    bool OldErrorFound = ErrorFound;
    ErrorFound = false;
    if (isa<CallExpr>(S)) {
      AnalyzeExpr(dyn_cast<Expr>(S), true);
    }

    // If we have a return statement, we check if the current
    // function is a QExpr.  Then check the return value expression.
    if (ReturnStmt *RS = dyn_cast<ReturnStmt>(S)) {
      if (FunctionReturnQExpr) {
        NumReturns += 1;
        bool OldErrorFound = ErrorFound;
        ErrorFound = false;
        Expr *NewRetVal = AnalyzeExpr(RS->getRetValue());
        if (NewRetVal)
          RS->setRetValue(NewRetVal);
      }
    }

    ErrorFound = OldErrorFound || ErrorFound;
    // The return value indicates whether we want the visitation to proceed.
    // Return false to stop the traversal of the AST.
    return true;
  }
};

// ASTVisitor that iterates over the declarations to find all the relevant
// function declarations.
class FunctionDeclFinderVisitor
    : public RecursiveASTVisitor<FunctionDeclFinderVisitor> {
public:
  std::map<std::string, FunctionDecl *> *NameToFunctionDecl;
  std::vector<FunctionDecl *> *FunctionDecls;
  llvm::DenseSet<FunctionDecl *> *QExprFunctionDecls;

  bool VisitDecl(Decl *D) {

    if (FunctionDecl *FD = dyn_cast<FunctionDecl>(D)) {
      // Add the function declaration to the list where the
      // body needs to be analyzed.
      FunctionDecls->push_back(FD);
      // Stop if the function declaration doesn't have a name.

      // If the return type is a QExpr, add it to the mapping
      // of functions we need to keep track of.
      QualType RetType = FD->getReturnType();
      const Type *T = RetType.getTypePtr();
      if (const ElaboratedType *ET = dyn_cast<ElaboratedType>(T)) {
        RetType = ET->getNamedType();
        T = RetType.getTypePtr();
      }
      const TypedefType *TDT = dyn_cast<TypedefType>(T);
      if (!TDT) {
        return true;
      }
      TypedefNameDecl *TDNM = TDT->getDecl();
      if (TDNM->getNameAsString() == "QExpr") {
        NameToFunctionDecl->insert(
            std::make_pair(FD->getQualifiedNameAsString(), FD));
        if (FD->getDeclName().isIdentifier()) {
          NameToFunctionDecl->insert(std::make_pair(FD->getName(), FD));
        }
        QExprFunctionDecls->insert(FD);
      }
    }

    return true;
  }
};

// AST Consumer that actually rewrites the AST operators to AST calls.
class QExprRewriteConsumer : public ASTConsumer {
  CompilerInstance &Instance;
  std::set<std::string> ParsedTemplates;
  std::vector<FunctionDecl *> FunctionDecls;
  llvm::DenseSet<FunctionDecl *> QExprFunctionDecls;

public:
  QExprRewriteConsumer(CompilerInstance &Instance,
                       std::set<std::string> ParsedTemplates)
      : Instance(Instance), ParsedTemplates(ParsedTemplates) {
    Visitor.Ctx = &Instance.getASTContext();
    Visitor.SetUpDiags(Instance.getDiagnostics());
    FDVisitor.FunctionDecls = &FunctionDecls;
    FDVisitor.NameToFunctionDecl = &Visitor.NameToFunctionDecl;
    FDVisitor.QExprFunctionDecls = &QExprFunctionDecls;
  }

  bool HandleTopLevelDecl(DeclGroupRef DG) override {
    for (DeclGroupRef::iterator i = DG.begin(), e = DG.end(); i != e; ++i) {
      Decl *D = *i;
      FDVisitor.TraverseDecl(D);
    }

    // For the function declarations, analyze the declaration and its body.
    for (FunctionDecl *FD : FunctionDecls) {
      Visitor.FunctionReturnQExpr =
          QExprFunctionDecls.find(FD) != QExprFunctionDecls.end();
      Visitor.resetNumReturns();
      Visitor.TraverseDecl(FD);
      if (Visitor.getNumReturns() > 1) {
        Visitor.reportReturnDiag(FD);
      }
      Visitor.FunctionReturnQExpr = false;
    }

    FunctionDecls.clear();

    return true;
  }

private:
  QExprRewriterVisitor Visitor;
  FunctionDeclFinderVisitor FDVisitor;
};

class QExprRewriteAction : public PluginASTAction {
  std::set<std::string> ParsedTemplates;

protected:
  std::unique_ptr<ASTConsumer>
  CreateASTConsumer(CompilerInstance &CI, llvm::StringRef FileName) override {
    llvm::setBugReportMsg(IntelMsg);
    llvm::sys::AddSignalHandler(ErrorHandler, nullptr);
    CI_ = &CI;

    FileName_ = FileName.str();

    return std::make_unique<QExprRewriteConsumer>(CI, ParsedTemplates);
  }

  PluginASTAction::ActionType getActionType() override {
    return AddBeforeMainAction;
  }

  bool ParseArgs(const CompilerInstance &CI,
                 const std::vector<std::string> &args) override {
    return true;
  }

private:
  clang::CompilerInstance *CI_ = nullptr;

  std::string FileName_;
  bool FileRewriteError_ = false;
};

} // namespace

static FrontendPluginRegistry::Add<QExprRewriteAction> X("rewrite-qexpr",
                                                         "Rewrite QExpr");
