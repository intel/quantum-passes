//===- quantum-optimization-driver.cpp --------------------------*- C++ -*-===//
//
//===----------------------------------------------------------------------===//
//
// Copyright (C) 2024 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
/// \file "quantum-optimization-driver.cpp"
///
/// This tool run custom pass pipelines to run LLVM optimizations on quantum
/// kernels to compile quantum programs.
///
//===----------------------------------------------------------------------===//

#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/AsmParser/Parser.h"
#include "llvm/Bitcode/BitcodeWriterPass.h"
#include "llvm/CodeGen/CommandFlags.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/Config/llvm-config.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/DebugInfo.h"
#include "llvm/IR/Dominators.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LLVMRemarkStreamer.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/LegacyPassNameParser.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/ModuleSummaryIndex.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IR/Verifier.h"
#include "llvm/IRPrinter/IRPrintingPasses.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/InitializePasses.h"
#include "llvm/LinkAllIR.h"
#include "llvm/LinkAllPasses.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Passes/StandardInstrumentations.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/InitLLVM.h"
#include "llvm/Support/PGOOptions.h"
#include "llvm/Support/PluginLoader.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/SystemUtils.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/ToolOutputFile.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/TargetParser/Host.h"
#include "llvm/TargetParser/SubtargetFeature.h"
#include "llvm/TargetParser/Triple.h"

#include "IntelQuantumPasses/QuantumAnalysis/QuantumInitPlacement.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumIterator.h"
#include "IntelQuantumPasses/QuantumCompile/IntelQuantumPlugin.h"
#include "IntelQuantumPasses/QuantumCompile/QuantumPassPipeline.h"
#include "IntelQuantumPasses/QuantumCompile/SimpleQbitPlacement.h"

#include <algorithm>
#include <memory>
#include <optional>

using namespace llvm;
using namespace aqcc;

static codegen::RegisterCodeGenFlags CFG;

static cl::opt<std::string> InputFilename(cl::Positional,
                                          cl::desc("<input bitcode file>"),
                                          cl::init("-"),
                                          cl::value_desc("filename"));

static cl::opt<std::string> OutputFilename("o",
                                           cl::desc("Override output filename"),
                                           cl::value_desc("filename"));

static cl::list<std::string>
    PassPlugins("load-pass-plugin",
                cl::desc("Load passes from plugin library"));

static cl::opt<bool> VerbosePrinting("verbose",
                                     cl::desc("data layout string to use"),
                                     cl::value_desc("layout-string"),
                                     cl::init(false));

static cl::opt<bool> CircuitPrinting("circuit-print",
                                     cl::desc("data layout string to use"),
                                     cl::value_desc("layout-string"),
                                     cl::init(false));

static cl::opt<int> CheckStage("s", cl::desc("<stage>"), cl::init(-1));

static cl::opt<bool> NoVerify("disable-verify",
                              cl::desc("Do not run the verifier"), cl::Hidden);

static cl::opt<bool> OutputAssembly("S",
                                    cl::desc("Write output as LLVM assembly"));

static cl::opt<std::string>
    StartStageName("start-stage", cl::desc("Section of compilation to start"),
                   cl::init("begin"));

static cl::opt<std::string>
    EndStageName("end-stage", cl::desc("Section of compilation to start"),
                 cl::init("end"));

static cl::opt<bool> UseSynth("use-synth",
                              cl::desc("Flag to use synthesis pass"),
                              cl::init(false));

static cl::opt<bool> UseSerial("serialize-kernels",
                               cl::desc("Flag to use serialize-kernels pass"),
                               cl::init(false));

static cl::list<std::string> CustomPassInput(
    "custom-passes",
    cl::desc("List of passes during a particular stage. Templated as "
             "<stage-name>:pass1[,pass2,..,passN]"),
    cl::ZeroOrMore);

extern cl::opt<bool> CanonicalScheduling;

extern cl::opt<std::string> QDType;

namespace llvm {

extern cl::opt<std::string> PlacementMethod;

}

void addUserPasses(PassBuilder &PB, ModulePassManager &MPM,
                   std::map<std::string, std::string> CustomPassMap,
                   std::string Stage) {
  std::map<std::string, std::string>::iterator It = CustomPassMap.find(Stage);
  if (It == CustomPassMap.end())
    return;
  auto Err = PB.parsePassPipeline(MPM, It->second);
  if (Err) {
    errs() << "Could not parse -" << It->second
           << " as a pipeline: " << toString(std::move(Err))
           << " will be skipping it\n";
    return;
  }
}

unsigned StageToNum(std::map<std::string, unsigned> &StageToNumMap,
                    std::string &StageName) {
  std::map<std::string, unsigned>::iterator It = StageToNumMap.find(StageName);
  if (It == StageToNumMap.end())
    return -1;
  return It->second;
}

int main(int argc, const char *argv[]) {
  InitLLVM X(argc, argv);

  // Enable debug stream buffering.
  EnableDebugBuffering = true;

  InitializeAllTargets();
  InitializeAllTargetMCs();
  InitializeAllAsmPrinters();
  InitializeAllAsmParsers();

  SmallVector<PassPlugin, 1> PluginList;
  PassPlugins.setCallback([&](const std::string &PluginPath) {
    auto Plugin = PassPlugin::Load(PluginPath);
    if (!Plugin)
      report_fatal_error(Plugin.takeError(), /*gen_crash_diag=*/false);
    PluginList.emplace_back(Plugin.get());
  });

  // Register the Target and CPU printer for --version.
  cl::AddExtraVersionPrinter(sys::printDefaultTargetAndDetectedCPU);

  cl::ParseCommandLineOptions(argc, argv,
                              "Intel Quantum Compilation Pass Driver\n");

  LLVMContext CurrContext;
  SMDiagnostic Err;

  // Load the input module
  std::unique_ptr<Module> M;
  M = parseIRFile(InputFilename, Err, CurrContext);

  if (!M) {
    Err.print(argv[0], errs());
    return 1;
  }

  // Immediately run the verifier to catch any problems before starting up the
  // pass pipelines.  Otherwise we can crash on broken code during
  // doInitialization().
  if (!NoVerify && verifyModule(*M, &errs())) {
    errs() << argv[0] << ": " << InputFilename
           << ": error: input module is broken!\n";
    return 1;
  }

  // Figure out what stream we are supposed to write to...
  std::unique_ptr<ToolOutputFile> Out;
  std::unique_ptr<ToolOutputFile> ThinLinkOut;
  // Default to standard output.
  if (OutputFilename.empty())
    OutputFilename = "-";

  std::error_code EC;
  sys::fs::OpenFlags Flags =
      OutputAssembly ? sys::fs::OF_TextWithCRLF : sys::fs::OF_None;
  Out.reset(new ToolOutputFile(OutputFilename, EC, Flags));
  if (EC) {
    errs() << EC.message() << '\n';
    return 1;
  }

  Triple ModuleTriple(M->getTargetTriple());
  std::string CPUStr, FeaturesStr;
  std::unique_ptr<TargetMachine> TM;
  const TargetOptions Options =
      codegen::InitTargetOptionsFromCodeGenFlags(ModuleTriple);
  if (ModuleTriple.getArch()) {
    CPUStr = codegen::getCPUStr();
    FeaturesStr = codegen::getFeaturesStr();
    std::string Error;
    const Target *TheTarget =
        TargetRegistry::lookupTarget(ModuleTriple.getTriple(), Error);
    if (!TheTarget) {
      errs() << argv[0] << ": ERROR: failed to create target machine for '"
             << ModuleTriple.str() << "': " << Error << "\n";
      return 1;
    }
    TM =
        std::move(std::unique_ptr<TargetMachine>(TheTarget->createTargetMachine(
            ModuleTriple.getTriple(), CPUStr, FeaturesStr, Options,
            codegen::getExplicitRelocModel())));
  } else if (ModuleTriple.getArchName() != "unknown" &&
             ModuleTriple.getArchName() != "") {
    errs() << argv[0] << ": unrecognized architecture '"
           << ModuleTriple.getArchName() << "' provided.\n";
    return 1;
  }

  LoopAnalysisManager LAM;
  FunctionAnalysisManager FAM;
  CGSCCAnalysisManager CGAM;
  ModuleAnalysisManager MAM;

  PassInstrumentationCallbacks PIC;
  unsigned UnresolvedKernels = 0;
  std::set<const Function *> ValidatedFuncs;
  bool CanCheck = false;

  /// Pass call back for loop passes to ensure that we only operate on loops
  /// that contain function calls.s
  PIC.registerShouldRunOptionalPassCallback(
      [&UnresolvedKernels, &ValidatedFuncs, &CanCheck](StringRef P,
                                                       llvm::Any IR) {
        if (const Loop **L = any_cast<const Loop *>(&IR)) {
          for (BasicBlock *BB : (*L)->getBlocks()) {
            QBBIter QIt(*BB);
            if (!QIt.isEnd())
              return true;
          }
          return false;
        }
        return true;
      });

  PrintPassOptions PrintPassOpts;
  bool VerifyEachPass = false;
  PrintPassOpts.Verbose = false;
  PrintPassOpts.SkipAnalyses = true;
  StandardInstrumentations SI(M->getContext(), false, VerifyEachPass,
                              PrintPassOpts);
  SI.registerCallbacks(PIC, &MAM);

  PipelineTuningOptions PTO;
  PTO.LoopUnrolling = true;
  PTO.UnifiedLTO = false;

  std::optional<PGOOptions> P = std::nullopt;
  PassBuilder PB(&*TM, PTO, P, &PIC);

  registerIntelQuantumPassesCallBacks(PB);

  // For any loaded plugins, let them register pass builder callbacks.
  for (auto &PassPlugin : PluginList)
    PassPlugin.registerPassBuilderCallbacks(PB);

  ArrayRef<std::function<void(llvm::PassBuilder &)>> PassBuilderCallbacks;

  // Load any explicitly specified plugins.
  for (auto &PassCallback : PassBuilderCallbacks)
    PassCallback(PB);

#define HANDLE_EXTENSION(Ext)                                                  \
  get##Ext##PluginInfo().RegisterPassBuilderCallbacks(PB);
#include "llvm/Support/Extension.def"

  PB.registerModuleAnalyses(MAM);
  PB.registerCGSCCAnalyses(CGAM);
  PB.registerFunctionAnalyses(FAM);
  PB.registerLoopAnalyses(LAM);
  PB.crossRegisterProxies(LAM, FAM, CGAM, MAM);

  std::vector<std::string> Stages = {
      "begin", "intrinsics", "flatten",  "unroll", "validate", "synthesize",
      "lower", "schedule",   "separate", "split",  "end",
  };
  std::map<std::string, unsigned> StageToNumMap;
  unsigned Idx = 0;
  for (std::string &S : Stages)
    StageToNumMap.insert(std::make_pair(S, Idx++));
  unsigned StartStage = StageToNum(StageToNumMap, StartStageName);
  unsigned EndStage = StageToNum(StageToNumMap, EndStageName);

  ModulePassManager PreUnrollingMPM;
  ModulePassManager PreMPM;
  ModulePassManager PostMPM;
  FunctionPassManager FPM;

  if (StartStage <= StageToNumMap["intrinsics"] &&
      EndStage >= StageToNumMap["intrinsics"])
    addQuantumAttributePasses(PreUnrollingMPM);
  if (StartStage <= StageToNumMap["flatten"] &&
      EndStage >= StageToNumMap["flatten"])
    addFlattenQuantumKernelPasses(PreUnrollingMPM);
  if (StartStage <= StageToNumMap["unroll"] &&
      EndStage >= StageToNumMap["unroll"]) {
    addQuantumModulePasses(PreUnrollingMPM);
    addPreQuantumLoopUnrollingModulePasses(PB, PreMPM);
    addPostQuantumLoopUnrollingModulePasses(PB, PostMPM);
    addQuantumLoopUnrollingFunctionPasses(PB, FPM, PTO, P);
  }

  std::map<std::string, std::string> CustomPassMap;
  for (std::string CustomPassList : CustomPassInput) {
    std::string::size_type ColonLoc = CustomPassList.find(":");
    if (ColonLoc == std::string::npos)
      continue;
    std::string StageName = CustomPassList.substr(0, ColonLoc);
    std::string PassList = CustomPassList.substr(ColonLoc + 1);
    CustomPassMap.insert(std::make_pair(StageName, PassList));
  }

  ModulePassManager PostLoopMPM;
  if (EndStage >= StageToNumMap["validate"])
    addQuantumModulePasses(PostLoopMPM);
  if (StartStage <= StageToNumMap["validate"] &&
      EndStage >= StageToNumMap["validate"]) {
    addUserPasses(PB, PostLoopMPM, CustomPassMap, "precond");
    addValidatePasses(PostLoopMPM, VerbosePrinting, CircuitPrinting);
  }
  if (!VerbosePrinting)
    PostLoopMPM.addPass(SimpleQbitPlacementPass());
  if (StartStage <= StageToNumMap["lower"] &&
      EndStage >= StageToNumMap["lower"]) {
    if (UseSynth) {
      addUserPasses(PB, PostLoopMPM, CustomPassMap, "presynth");
      addSynthesisPasses(PostLoopMPM, VerbosePrinting, CircuitPrinting);
      addUserPasses(PB, PostLoopMPM, CustomPassMap, "prelower");
      addLoweringPasses(PostLoopMPM, "LD", false, true, VerbosePrinting,
                        CircuitPrinting);
    } else {
      addUserPasses(PB, PostLoopMPM, CustomPassMap, "prelower");
      addLoweringPasses(PostLoopMPM, "LD", false, false, VerbosePrinting,
                        CircuitPrinting);
    }
  }
  if (PlacementMethod != "none")
    PostLoopMPM.addPass(QuantumInitPlacementPass());
  else
    PostLoopMPM.addPass(SimpleQbitPlacementPass());
  if (StartStage <= StageToNumMap["schedule"] &&
      EndStage >= StageToNumMap["schedule"]) {
    addUserPasses(PB, PostLoopMPM, CustomPassMap, "preschedule");
    addSchedulingPasses(PostLoopMPM, "LD", false, UseSerial, VerbosePrinting,
                        CircuitPrinting);
  }
  if (StartStage <= StageToNumMap["separate"] &&
      EndStage >= StageToNumMap["separate"]) {
    addUserPasses(PB, PostLoopMPM, CustomPassMap, "presplit");
    addSeparationPasses(PostLoopMPM, VerbosePrinting, CircuitPrinting);
  }
  if (StartStage <= StageToNumMap["split"] &&
      EndStage >= StageToNumMap["split"])
    addSplitPasses(PostLoopMPM);
  PostLoopMPM.addPass(VerifierPass());

  PostLoopMPM.addPass(PrintModulePass(Out->os(), "", false, false));

  PreUnrollingMPM.run(*M, MAM);
  if (EndStage >= StageToNumMap["unroll"])
    runFunctionPassesQuantumKernels(PreMPM, PostMPM, FPM, *M, MAM, FAM);
  PostLoopMPM.run(*M, MAM);

  Out->keep();
}
