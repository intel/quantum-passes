//===- QuantumCircuit.h ---------------------------------------------====//
//
// Copyright (C) 2024 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
//
// This file is the header file for the QuantumCircuit and QuantumCircuitOps
// classes.
//
//===----------------------------------------------------------------------===//

#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SmallSet.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/IR/BasicBlock.h"

#include "IntelQuantumPasses/QuantumAnalysis/QuantumModule.h"
#include "IntelQuantumPasses/QuantumAnalysis/QuantumRefs.h"

#ifndef QUANTUM_CIRCUIT_H
#define QUANTUM_CIRCUIT_H

namespace llvm {
namespace aqcc {

class QuantumCircuit;
class QuantumModule;
class QuantumKernel;
class QuantumOp;

/// Different quantum circuits.
/// AllQuantum - There are only quantum operations contained
/// in this circuit, usually at the basic block level.
/// NoQuantum - This circuit contains blocks/operations
/// that contain no quantum operations.
/// Combined - This circuit has a mix of blocks/operations
/// that use quantum and classical elements.
enum CircuitKind { None, AllQuantum, NoQuantum, Combined };

/// Collection of different operations that can be included
/// in a Quantum Circuit.
enum OpKind {
  GeneralOp,
  Start,
  End,
  Quantum,
  ControlledByQuantum,
  ControlledByClassical,
  Classical,
  NewCircuitMarker
};

/// Specialized comapare function for comparing QbitRefs to one another.
/// First checks if they are equivalent references.  If they are, use
/// the overloaded less than comparison.
struct QubitCompare {
  bool operator()(const QbitRef &lhs, const QbitRef &rhs) const {
    if (lhs.isEqual(rhs) == 1)
      return false;
    return lhs < rhs;
  }
};

/// Parent class for describing an operation in a quantum circuit.
/// Represented by a node in the overall QuantumCircuit graph.
///
/// Each operation represents a single operation. However,
/// an operation can be a function call to another quantum
/// kernel.
///
/// The edges between the operations are defined by shared
/// qubits used in the two operations.
class QuantumCircuitOp {
private:
  std::string Name = "";

protected:
  /// The QuantumModule the operation belongs to.
  QuantumModule *QM = nullptr;

  /// Attribute of the parent circuit.
  QuantumCircuit *Parent = nullptr;

  /// Kind of operation being described by this node.
  OpKind Kind = GeneralOp;

  /// Unique ID for this operation.
  unsigned GateId;

  /// Marks whether this is an operation, or a bookkeeping
  /// node.
  bool SentryNode = true;

  /// The predecessor operations, either defined explicitly, or
  /// by whether there is a shared qubit between the predecessor
  /// node and the current node.
  std::set<QuantumCircuitOp *> Predecessors;

  /// The successor operations, either defined explicitly, or
  /// by whether there is a shared qubit between the successor
  /// node and the current node.
  std::set<QuantumCircuitOp *> Successors;

  /// Mapping from Qubits to QuantumOperations, denoting the edge relationships
  /// in the graph that are built on usage of the same qubit.  When there is no
  /// entry for a given qubit, it means that the qubit is not used by the
  /// current operation.
  std::map<QbitRef, QuantumCircuitOp *, QubitCompare> SuccessorDependency;

  /// Mapping from QuantumOperations to Qubits, denoting the edge relationships
  /// in the graph that are built on usage of the same set of qubits.  If there
  /// is no entry for an operation, or the set is empty, it does not mean there
  /// is no edge. Rather, it means that any dependency is not built on a qubit
  /// dependency between the operations.
  std::map<QuantumCircuitOp *, std::set<QbitRef, QubitCompare>>
      SuccessorDependence;

public:
  QuantumCircuitOp(QuantumCircuit &QC);
  QuantumCircuitOp();

  virtual ~QuantumCircuitOp() = default;

  /// Accepts two QuantumCircuitOps, determines the shared qubits between the
  /// two operations and manage the dependency maps appropriately.
  static void resolveDependence(QuantumCircuitOp &Pred, QuantumCircuitOp &Succ);

  /// Accepts two QuantumCircuitOps and a QbitRef, if there is a dependency
  /// based on the given qubit, it is removed from the maps.
  static void unlinkDependence(QuantumCircuitOp &Pred, QuantumCircuitOp &Succ,
                               QbitRef &QR);

  /// Accepts two QuantumCircuitOps, if Pred and Succ do not have any qubit
  /// dependencies, they are removed as predecessor and successors from one
  /// another.
  static void unlinkOperationsIfNoDependence(QuantumCircuitOp &Pred,
                                             QuantumCircuitOp &Succ);

  /// Removes the qubit depedencies and predecessor/successor relationship
  /// between Pred and Succ.
  static void unlinkOperations(QuantumCircuitOp &Pred, QuantumCircuitOp &Succ);

  /// Add a qubit dependency between the given predecessor and successor.
  static void createDependence(QuantumCircuitOp &Pred, QuantumCircuitOp &Succ,
                               const QbitRef &QR);

  /// Check whether there are current operations that are successors of this
  /// operation dependent on QbitRef QR.
  bool hasDependence(QbitRef &QR);

  /// Return the operation that is a successor of this
  /// operation dependent on QbitRef QR. Returns nullptr if there is no such
  /// operation.
  QuantumCircuitOp *getDependence(QbitRef &QR);

  /// Return the qubits that QCO is dependent on for this operation. Returns an
  /// empty set if there are none.
  std::set<QbitRef, QubitCompare> getQubitDependence(QuantumCircuitOp *QCO);

  void addPredecessors(std::set<QuantumCircuitOp *> &Ins);
  void addSuccessors(std::set<QuantumCircuitOp *> &Outs);

  /// Add to the set of predecessors, this helper function
  /// does not perform any sort of ordering checks.
  void addPredecessor(QuantumCircuitOp *In);

  /// Add to the set of successors, this helper function
  /// does not perform any sort of ordering checks.
  void addSuccessor(QuantumCircuitOp *Out);

  /// Remove In from the set of predecessors, this helper function
  /// does not perform any sort of ordering checks.
  void removePredecessor(QuantumCircuitOp *In);

  /// Remove out from the set of successors, this helper function
  /// does not perform any sort of ordering checks.
  void removeSuccessor(QuantumCircuitOp *Out);

  /// Get a reference to the set of predecessors of this node.
  std::set<QuantumCircuitOp *> &getPredecessors() { return Predecessors; };

  /// Get a reference to the set of successors from this node.
  std::set<QuantumCircuitOp *> &getSuccessors() { return Successors; };

  /// Check whether node is included in the predecessor set.
  bool isSuccessorTo(QuantumCircuitOp *QCO) {
    return Predecessors.find(QCO) != Predecessors.end();
  };

  /// Check whether node is included in the successor set.
  bool isPredecessorTo(QuantumCircuitOp *QCO) {
    return Successors.find(QCO) != Successors.end();
  };

  /// Each circuit operation has a unique ID value.  Comparing these
  /// does not represent any equivalency between operations.
  unsigned getGateID() { return GateId; }

  /// The the kind of operation contained.  This will help
  /// when handling different operations.
  OpKind getKind() { return Kind; }

  /// Get the parent circuit this operation belongs to.
  /// An operation will only ever belong to a single QuantumCircuit.
  /// If "inserted" it will be either be removed or copied from the
  /// previous parent.
  QuantumCircuit *getParentCircuit() { return Parent; }

  void setParentCircuit(QuantumCircuit *QC);

  /// Getters and setters for the QuantumModule.
  void setQuantumModule(QuantumModule &TheQM) { QM = &TheQM; }
  QuantumModule *getQuantumModule() { return QM; }

  /// Virtual function to be overloaded by the subclasses for printing
  /// the different operations.
  virtual std::string printPretty();

  friend std::ostream &operator<<(std::ostream &os,
                                  QuantumCircuitOp const &QCO) {
    return os;
  }

  /// Overloaded operators to compare the QuantumCircuitOps based
  /// on their ID value.
  bool operator<(QuantumCircuitOp &Other);
  bool operator>(QuantumCircuitOp &Other);
  bool operator==(QuantumCircuitOp &Other);
};

enum OpParamTy { QUBIT, QUBITPTR, GATEPARAM, DOUBLE, INT, OTHER };

/// Helper Struct that lets user define a vector of differently typed
/// parameters. Specifically helpful when adding a user defined function to the
/// graph.
struct QuantumOpParam {
  /// Used to denote which "mode" the object should be used in.

  QbitRef QR;
  ParaRef PR;
  Value *OtherValue;
  double OtherDouble;
  unsigned OtherInt;
  bool isArg = false;
  Type *T = nullptr;
  Argument *Arg = nullptr;
  OpParamTy ParamType;

  QuantumOpParam(OpParamTy PT, QbitRef &QR) {
    ParamType = PT;
    this->QR = QR;
  }

  QuantumOpParam(OpParamTy PT, ParaRef &PR) {
    ParamType = PT;
    this->PR = PR;
  }

  QuantumOpParam(OpParamTy PT, Value *V) {
    ParamType = PT;
    this->OtherValue = V;
  }

  QuantumOpParam(OpParamTy PT, double DoubleVal) {
    ParamType = PT;
    this->OtherDouble = DoubleVal;
  }

  QuantumOpParam(OpParamTy PT, int IntVal) {
    ParamType = PT;
    this->OtherInt = IntVal;
  }

  QuantumOpParam(OpParamTy PT, Type *GivenT = nullptr) {
    ParamType = PT;
    T = GivenT;
    this->isArg = true;
  }

  Value *getValue(LLVMContext &C);
  Value *getValue(QuantumCircuit *QC);
  Value *getValue(QuantumOp *QC);

  Type *getType(LLVMContext &C);
};

/// A subclass representing quantum operations in the circuit
/// object.
class QuantumOp : public QuantumCircuitOp {
private:
  /// Vector with ordering describing the qubit operands of the operation.
  /// It is a pair as the second value describes the operand location
  /// in the original instruction.
  SmallVector<std::pair<QbitRef, unsigned>, 2> Qubits;

  /// Vector with ordering describing the constant parameters of the operation.
  /// usually for the parameters of quantum operations. It is a pair as the
  /// second value describes the operand location in the original instruction.
  SmallVector<std::pair<ParaRef, unsigned>, 2> ResolvedClassicalParameters;

  /// Vector with ordering describing the variable parameters of the operation.
  /// usually for the parameters of a function call. It is a pair as the
  /// second value describes the operand location in the original instruction.
  SmallVector<std::pair<Value *, unsigned>, 2> OtherParameters;

  /// Most quantum operations are function calls in the IR.  This represents the
  /// called value for reference.
  Value *CalledVal = nullptr;

  /// This is the instruction of the entire operation captured byt this node.
  Value *OriginalValue = nullptr;

  /// Helper boolean for whether this is a custom operation, or one builtin to
  /// the Intel Quantum Passes.
  bool IsBuiltin = false;

  /// Flag for marking whether the quantum operation needs to update the
  /// marked parameters after moving or copying the operation.
  bool ParamsChanged = false;

public:
  QuantumOp(Value *V);
  QuantumOp(Value *V, QuantumCircuit &QC);
  QuantumOp(Value *V, std::vector<QbitRef> &Qubits, QuantumCircuit &QC);
  QuantumOp(Value *V, std::vector<QbitRef> &Qubits,
            std::vector<ParaRef> &Params, QuantumCircuit &QC);
  QuantumOp(Value *V, std::vector<QbitRef> &Qubits,
            std::vector<ParaRef> &Params);
  QuantumOp(Value *V, std::vector<std::pair<QbitRef, unsigned>> &Qubits,
            std::vector<std::pair<ParaRef, unsigned>> &ConstantParams,
            std::vector<std::pair<Value *, unsigned>> &OtherParams);
  QuantumOp(Value *V, std::vector<std::pair<QbitRef, unsigned>> &Qubits,
            std::vector<std::pair<ParaRef, unsigned>> &ConstantParams,
            std::vector<std::pair<Value *, unsigned>> &OtherParams,
            QuantumCircuit &QC);
  static QuantumOp *Create(Function *F, QuantumModule &QM,
                           std::vector<QbitRef> &Qubits,
                           std::vector<ParaRef> &Params,
                           QuantumCircuitOp *After = nullptr,
                           std::string Name = "");
  static QuantumOp *Create(Function *F, QuantumModule &QM,
                           std::vector<QuantumOpParam> &OperationParams,
                           QuantumCircuitOp *After = nullptr,
                           std::string Name = "");
  static QuantumOp *Create(int GateID, QuantumModule &QM,
                           std::vector<QbitRef> &Qubits,
                           std::vector<ParaRef> &Params,
                           QuantumCircuitOp *After = nullptr,
                           std::string Name = "");
  static QuantumOp *Create(std::string GateName, QuantumModule &QM,
                           std::vector<QbitRef> &Qubits,
                           std::vector<ParaRef> &Params,
                           QuantumCircuitOp *After = nullptr);
  static QuantumOp *Create(std::string GateName, QuantumModule &QM,
                           std::vector<QbitRef> &Qubits,
                           std::vector<ParaRef> &Params,
                           std::vector<double> &OtherParams,
                           QuantumCircuitOp *After = nullptr);
  static QuantumOp *Create(int GateID, QuantumModule &QM,
                           std::vector<QbitRef> &Qubits,
                           std::vector<ParaRef> &Params,
                           QuantumCircuit &InsertAtEnd);
  static QuantumOp *Create(std::string GateName, QuantumModule &QM,
                           std::vector<QbitRef> &Qubits,
                           std::vector<ParaRef> &Params,
                           QuantumCircuit &InsertAtEnd);
  static QuantumOp *Create(std::string GateName, QuantumModule &QM,
                           std::vector<QuantumOpParam> &OperationParams,
                           QuantumCircuit &InsertAtEnd);
  static QuantumOp *Create(std::string GateName, QuantumModule &QM,
                           std::vector<QuantumOpParam> &OperationParams,
                           QuantumCircuitOp *After = nullptr);

  ~QuantumOp();

  /// Helper function to ge the original instruction.
  Value *getBaseValue() { return OriginalValue; }

  /// Helper function to get the called function.
  Value *getCalledValue() { return CalledVal; }

  /// Delete the given operation from the QuantumCircuit.  Ensures that the
  /// qubit dependencies are maintained.  If there is a manual dependency that
  /// is not specified from qubit relationships, every successor to the deleted
  /// operation is directly connected to the predecessors of the deleted
  /// operation.  This also removes the gate from the IR form of the
  /// circuit and erases it from memory as well.
  bool deleteGate();

  /// Removes the given operation from the QuantumCircuit.  Ensures that the
  /// qubit dependencies are maintained.  If there is a manual dependency that
  /// is not specified from qubit relationships, every successor to the deleted
  /// operation is directly connected to the predecessors of the deleted
  /// operation.  This also removes the gate from the IR representation of the
  /// circuit, but does not remove it from memory.
  bool removeGate();

  /// Move the gate after the After operation, in the parent
  /// QuantumCircuit that After belongs to. This change will also be reflected
  /// in the IR, and operations will be cloned as necessary to handle the
  /// parameters of the operation. This may include adding new local qubit
  /// allocations to the ParentCircuit of the after operation.
  bool moveGate(QuantumCircuitOp &After);

  /// Move the gate ToMove to the end of QuantumCircuit QC. If no circuit is
  /// specified, the gate will be moved to the end of the current parent
  /// circuit. This change will also be reflected in the IR, and operations will
  /// be cloned as necessary to handle the parameters of the operation. This may
  /// include adding new local qubit allocations to the ParentCircuit of the
  /// After operation.
  bool moveGate(QuantumCircuit *QC);

  /// Move the gate after the After operation, in the parent
  /// QuantumCircuit that After belongs to. This change will also be reflected
  /// in the IR, and operations will be cloned as necessary to handle the
  /// parameters of the operation. This may include adding new local qubit
  /// allocations to the ParentCircuit of the after operation. ArgMapping
  /// is a mapping of an integer, representing an argument to the parent
  /// function to a QuantumOpParam. If the QuantumOp is moved between
  /// circuits, the parameters may rely on arguments of the original parent
  /// circuit. When moving the QuantumOp, the values in ArgMapping will be used
  /// to replace any circuit argument used. If it is not found, the operation
  /// will fail.
  bool moveGate(QuantumCircuitOp &After,
                std::map<unsigned, QuantumOpParam> &ArgMapping);

  /// Move the gate to the end of QuantumCircuit QC. If no circuit is
  /// specified, the gate will be moved to the end of the current parent
  /// circuit. This change will also be reflected in the IR, and operations will
  /// be cloned as necessary to handle the parameters of the operation. This may
  /// include adding new local qubit allocations to the ParentCircuit of the
  /// After operation. ArgMapping is a mapping of an integer, representing an
  /// argument to the parent function to a QuantumOpParam. If the QuantumOp is
  /// moved between circuits, the parameters may rely on arguments of the
  /// original parent circuit. When moving the QuantumOp, the values in
  /// ArgMapping will be used to replace any circuit argument used. If it is not
  /// found, the operation will fail.
  bool moveGate(QuantumCircuit *QC,
                std::map<unsigned, QuantumOpParam> &ArgMapping);

  /// Copy the gate ToMove after the After operation, in the parent
  /// QuantumCircuit that After belongs to. This change will also be reflected
  /// in the IR, and operations will be cloned as necessary to handle the
  /// parameters of the operation. This may include adding new local qubit
  /// allocations to the ParentCircuit of the after operation.
  QuantumOp *copyGate(QuantumCircuitOp &After);

  /// Copy the gate ToMove to the end of QuantumCircuit QC. If no circuit is
  /// specified, the gate will be moved to the end of the current parent
  /// circuit. This change will also be reflected in the IR, and operations will
  /// be cloned as necessary to handle the parameters of the operation. This may
  /// include adding new local qubit allocations to the ParentCircuit of the
  /// after operation.
  QuantumOp *copyGate(QuantumCircuit *QC);

  /// Copy the gate after the After operation, in the parent
  /// QuantumCircuit that After belongs to. This change will also be reflected
  /// in the IR, and operations will be cloned as necessary to handle the
  /// parameters of the operation. This may include adding new local qubit
  /// allocations to the ParentCircuit of the after operation. ArgMapping
  /// is a mapping of an integer, representing an argument to the parent
  /// function to a QuantumOpParam. If the QuantumOp is copied between
  /// circuits, the parameters may rely on arguments of the original parent
  /// circuit. When moving the QuantumOp, the values in ArgMapping will be used
  /// to replace any circuit argument used. If it is not found, the operation
  /// will fail.
  QuantumOp *copyGate(QuantumCircuitOp &After,
                      std::map<unsigned, QuantumOpParam> &ArgMapping);

  /// Copy the gate to the end of QuantumCircuit QC. If no circuit is
  /// specified, the gate will be moved to the end of the current parent
  /// circuit. This change will also be reflected in the IR, and operations will
  /// be cloned as necessary to handle the parameters of the operation. This may
  /// include adding new local qubit allocations to the ParentCircuit of the
  /// after operation. ArgMapping is a mapping of an integer, representing an
  /// argument to the parent function to a QuantumOpParam. If the QuantumOp is
  /// copied between circuits, the parameters may rely on arguments of the
  /// original parent circuit. When moving the QuantumOp, the values in
  /// ArgMapping will be used to replace any circuit argument used. If it is not
  /// found, the operation will fail.
  QuantumOp *copyGate(QuantumCircuit *QC,
                      std::map<unsigned, QuantumOpParam> &ArgMapping);

  /// Copy and return the gate, but do not insert it into any circuit.
  QuantumOp *copyGate();

  /// Helper function to get a reference list to the qubits in the operations.
  SmallVector<std::pair<QbitRef, unsigned>, 2> &getQubits() { return Qubits; }

  /// Helper function to get a reference list to the resolved classical
  /// parameters in the operations. Used for gates.
  SmallVector<std::pair<ParaRef, unsigned>, 2> &getResolvedParams() {
    return ResolvedClassicalParameters;
  }

  /// Helper function to get a reference list to the other parameters in the
  /// operations. Used for kernel calls.
  SmallVector<std::pair<Value *, unsigned>, 2> &getOtherParams() {
    return OtherParameters;
  }

  /// Overloaded function for printing a human readable version of the
  /// operation.
  std::string printPretty();

  /// Helper function to check the base call instruction and create a QbitRef
  /// from the given argument location.  If it does not exist, the QbitRef is
  /// null.
  QbitRef getQbitRefFromValue(CallBase *CB, unsigned Idx);

  /// Helper function to check the base call instruction and create a ParaRef
  /// from the given argument location. If a valid ParaRef cannot be created,
  /// the returned ParaRef is null.
  ParaRef getParaRefFromValue(CallBase *CB, unsigned Idx);

  /// Mark the flag that internal parameters need to be updated.
  void setParamsNeedUpdating() { ParamsChanged = true; };

  /// Construct the parameter vectors from the original instruction.
  void allParametersFromValue();

  ///
  std::string getOpName();
};

/// A subclass representing the start of a quantum circuit.
/// It contains no qubits, but has edges as if it uses all
/// qubits to maintain an ordering between circuits.
class StartingOp : public QuantumCircuitOp {
public:
  StartingOp(QuantumCircuit &P) : QuantumCircuitOp(P) {
    Kind = OpKind::Start;
    SentryNode = true;
  }

  std::string printPretty();
};
/// A subclass representing the end of a quantum circuit.
/// It contains no qubits, but has edges as if it uses all
/// qubits to maintain an ordering between circuits.
class EndingOp : public QuantumCircuitOp {
public:
  EndingOp(QuantumCircuit &P) : QuantumCircuitOp(P) {
    Kind = OpKind::End;
    SentryNode = true;
  }

  std::string printPretty();
};

/// A helper struct to represent the QuantumCircuit in a "time-slice"
/// rather than a single operation at a time.
struct QuantumCircuitSlice {
  /// The operations contained within the time slice.
  std::vector<QuantumCircuitOp *> Ops;

  QuantumCircuitSlice() {}

  /// Copy Constructor
  QuantumCircuitSlice(QuantumCircuitSlice &&) = default;

  /// Copy Constructor
  QuantumCircuitSlice(QuantumCircuitSlice &Other) {
    for (QuantumCircuitOp *QCO : Other.Ops)
      Ops.push_back(QCO);
  }

  /// Two time slices are equal if they contain the same operations.
  bool operator==(const QuantumCircuitSlice &Other) const {
    if (Ops.size() != Other.Ops.size())
      return false;
    return Ops == Other.Ops;
  }
};

class QuantumCircuit {
private:
  /// A vector containing the different quantum circuits. Does
  /// not guarantee any particular ordering.
  std::vector<QuantumCircuit *> CircuitList;

  /// A map to help quickly identify the internal QbitRef value
  /// of a given IR value.
  DenseMap<Value *, QbitRef> ValueToQbitRef;

  /// A map to help quickly identify the internal ParaRef value
  /// of a given IR value.
  DenseMap<Value *, ParaRef> ValueToParaRef;

  /// The qubits used in the circuit.  Uses the specialized
  /// comparison function for QbitRefs in order to accurately
  /// create a non-repeating set.
  std::set<QbitRef, QubitCompare> Qubits;

  /// The starting operation for the circuit.
  StartingOp *StartOp = nullptr;
  /// The ending operation for the circuit.
  EndingOp *EndOp = nullptr;

  /// The value the circuit is derived from. If this is
  /// nullptr this means that it is a subcircuit (a kernel
  /// that does not contain all basic blocks), or a set of blocks
  /// belonging to no kernel.
  Value *BaseValue = nullptr;

  /// The mix of quantum and classical operations contained in this circuit.
  CircuitKind Kind = None;

  /// The number of quantum operations contained in the circuit. Does not
  /// contain start and end operations.
  unsigned NumberGates = 0;

  /// The unique circuit id.
  unsigned CircuitId = -1;

  /// Static operations to assign unique IDs to circuits.
  static unsigned GateIdCounter;
  static unsigned CircuitIdCounter;

  /// List of operations that has all qubits as an input, but is not
  /// explicit as an operand.
  std::vector<QuantumOp *> BlockingOps;

  std::vector<QuantumOpParam> ArgumentParams;

  /// The QuantumModule the circuit is included in.
  QuantumModule *QM;

  QuantumCircuit *ParentCircuit = nullptr;

  static std::set<QbitRef, QubitCompare> GlobalQubits;

  QuantumCircuit(QuantumModule &QM, BasicBlock &BB);
  QuantumCircuit(QuantumModule &QM, QuantumKernel &QK);

  static void collectModuleGlobalQubits(QuantumModule &QM);

public:
  static QuantumCircuit *Create(QuantumModule &QM, BasicBlock &BB) {
    return new QuantumCircuit(QM, BB);
  }
  static QuantumCircuit *Create(QuantumModule &QM, QuantumKernel &QK) {
    return new QuantumCircuit(QM, QK);
  }

  static QuantumCircuit *Create(QuantumModule &QM, std::string Name = "",
                                bool IsParent = false);
  static QuantumCircuit *Create(QuantumModule &QM,
                                std::vector<QuantumOpParam> &Args,
                                std::string Name = "");

  ~QuantumCircuit();

  /// Create a new allocation of qubits that is local to this circuit only.
  bool createLocalQubitRegister(unsigned Size,
                                std::vector<QbitRef> &LocalQubits,
                                std::string Name = "");

  /// Create a new allocation of qubits that any quantum circuit can use.
  bool createGlobalQubitRegister(unsigned Size,
                                 std::vector<QbitRef> &GlobalQubits,
                                 std::string Name = "");

  /// Create a QbitRef to qubit reference QR at index Val.
  /// the register must be larger than Val.
  QbitRef getQubitInRegister(QbitRef &QR, unsigned Val);

  bool addQubit(const QbitRef &QR);

  /// Append the given quantum operation to the end of the circuit, just before
  /// the EndOp of the circuit, also creates appropriate dependency
  /// relationships between the predecessor operations to the EndOp as well.
  /// This also adds the gate to the IR representation of the circuit.
  bool appendGate(QuantumOp *QCO);

  /// Append the given quantum operation to the end of the circuit, just before
  /// the EndOp of the circuit, also creates appropriate dependency
  /// relationships between the predecessor operations to the EndOp as well.
  /// This also adds the gate to the IR representation of the circuit.
  bool appendGate(QuantumOp *QCO, std::map<unsigned, QuantumOpParam> &Mapping);

  /// Add the gate to the quantum circuit after the given operation.  This will
  /// maintain qubit relationships, but will add an explicit relationship
  /// between After and QCO that does not depend on a particular qubit.
  bool insertGate(QuantumOp *QCO, QuantumCircuitOp *After);

  /// Add the gate to the quantum circuit after the given operation.  This will
  /// maintain qubit relationships, but will add an explicit relationship
  /// between After and QCO that does not depend on a particular qubit.
  bool insertGate(QuantumOp *QCO, QuantumCircuitOp *After,
                  std::map<unsigned, QuantumOpParam> &Mapping);

  /// Add the circuit NewQC to the quantum circuit after the given sub-circuit
  /// After. After must be inside the parent circuit being inserted into. The
  /// Mapping replaces any applicable values that were dependent on
  /// arguments of the QuantumCircuit represented by the previous parent
  /// circuit.  If After is a nullptr, the circuit is inserted at the end of the
  /// parent circuit.  If NewQC is a circuit representing a kernel, the QBBs
  /// will be moved and NewQC will be empty. If NewQC is a block, it must not
  /// have a parent and will be moved in its entirety into the new parent
  /// circuit.
  bool insertCircuit(QuantumCircuit *NewQC, QuantumCircuit *After,
                     std::map<unsigned, QuantumOpParam> &Mapping);

  /// Add the circuit to the quantum circuit after the given sub-circuit.
  /// Follows the same rules as the previously defined insert circuit.
  bool insertCircuit(QuantumCircuit *NewQC, QuantumCircuit *After);

  /// Adds the circuit to parent circuit after operation After. This is done
  /// by splitting the QBB and Quantum Circuit that After is contained in.
  /// We insert NewQC after the basic block After is in.
  bool insertCircuit(QuantumCircuit *NewQC, QuantumOp &After,
                     std::map<unsigned, QuantumOpParam> &Mapping);

  /// Adds the circuit to parent circuit after operation After.
  bool insertCircuit(QuantumCircuit *NewQC, QuantumOp &After);

  /// Add the circuit to the end of the quantum circuit.
  /// Follows the same rules as the previously defined insert circuit.
  bool insertCircuit(QuantumCircuit *NewQC);

  /// Create a copy of the quantum circuit by cloning the quantum kernel or
  /// QBB and creating a new circuit with the new value.
  QuantumCircuit *copyCircuit(std::string Name = "");

  /// Perform the same insertion as above, but copy the circuit first, creating
  /// a new value and all new objects.
  bool copyAndInsertCircuit(QuantumCircuit *ToCopy, QuantumCircuit *After,
                            std::map<unsigned, QuantumOpParam> &Mapping);

  /// Perform the same insertion as above, but copy the circuit first, creating
  /// a new value and all new objects.
  bool copyAndInsertCircuit(QuantumCircuit *ToCopy, QuantumCircuit *After);

  /// Perform the same insertion as above, but copy the circuit first, creating
  /// a new value and all new objects.
  bool copyAndInsertCircuit(QuantumCircuit *ToCopy, QuantumOp &After,
                            std::map<unsigned, QuantumOpParam> &Mapping);

  /// Perform the same insertion as above, but copy the circuit first, creating
  /// a new value and all new objects.
  bool copyAndInsertCircuit(QuantumCircuit *ToCopy, QuantumOp &After);

  /// Perform the same insertion as above, but copy the circuit first, creating
  /// a new value and all new objects.
  bool copyAndInsertCircuit(QuantumCircuit *ToCopy);

  /// Fill the given vector with the circuit operations in a defined
  /// topological order. Order is defined by operationIDs and the graph
  /// structure.
  void iterateGates(std::vector<QuantumCircuitOp *> &Ops);

  /// Fill the given vector with the circuit operations in a defined
  /// topological order. Order is defined by operationIDs and the graph
  /// structure. However, this implementation limits the
  /// iteration to only return operations on specific qubits.
  void iterateGatesLimitedQubits(std::vector<QuantumCircuitOp *> &Ops,
                                 std::set<QbitRef, QubitCompare> &LimitQubits);

  /// Fill the given vector with the circuit slices in a defined
  /// topological order. Order is defined by operationIDs and the graph
  /// structure.  Rather than returning a list of operations, the operations
  /// are grouped in each "frontier" of operations. Useful for determining the
  /// depth of a given circuit.
  void iterateGatesTimeSlices(std::vector<QuantumCircuitSlice> &Slice);

  /// Fill the given vector with the circuit slices in a defined
  /// topological order. Order is defined by operationIDs and the graph
  /// structure.  Rather than returning a list of operations, the operations
  /// are grouped in each "frontier" of operations. Useful for determining the
  /// depth of a given circuit. However, this implementation limits the
  /// iteration to only return operations on specific qubits.
  void iterateGatesTimeSlicesLimitedQubits(
      std::vector<QuantumCircuitSlice> &Slice,
      std::set<QbitRef, QubitCompare> &LimitQubits);

  /// Recursively determine the number of gates contained in the circuit.
  unsigned getNumberGates() {
    unsigned RecursiveGates = 0;
    for (QuantumCircuit *QC : CircuitList)
      RecursiveGates += QC->getNumberGates();
    return NumberGates + RecursiveGates;
  };

  void reduceGates() { NumberGates -= 1; };

  void addToGates() { NumberGates += 1; };

  /// Get a reference to the circuits included in the parent circuit.
  std::vector<QuantumCircuit *> &getCircuitList() { return CircuitList; };

  /// Returns true if the circuit was derived from an IR Function.
  bool isAFunction() { return isa<Function>(BaseValue); }

  /// Returns true if the circuit was derived from an IR BasicBlock.
  bool isABasicBlock() { return isa<BasicBlock>(BaseValue); }

  /// Get a new GateID value for a newly created quantum gate.
  static unsigned getGateID() { return GateIdCounter++; }

  /// Get the value the circuit was built from.
  Value *getBaseValue() { return BaseValue; }

  /// Get the parent circuit that points to this sub circuit, if it
  /// exists.
  QuantumCircuit *getParentCircuit() { return ParentCircuit; }

  /// Get the enum valued describing the circuit.
  CircuitKind getKind() { return Kind; }

  /// Get the starting sentry value of the circuit.
  StartingOp *getStartOp() { return StartOp; }

  /// Get the ending sentry value of the circuit.
  EndingOp *getEndOp() { return EndOp; }

  QuantumModule *getQuantumModule() { return QM; }

  /// Check whether operation A comes before operation B in the graph.
  /// If there is a loop, this is true for both cases.
  bool isAncestorTo(QuantumCircuitOp *A, QuantumCircuitOp *B);

  std::set<QbitRef, QubitCompare> &getQubits() { return Qubits; }

  std::set<QbitRef, QubitCompare> &getGlobalQubits();

  static std::set<QbitRef, QubitCompare> &getGlobalQubits(QuantumModule &QM);

  std::vector<QuantumOpParam> &getOpParams() { return ArgumentParams; }

  /// If the value exists, get the name of the base value the
  /// circuit is derived from. Otherwise, create a name
  /// derived from the circuit's ID value.
  std::string getName() {
    if (BaseValue)
      return BaseValue->getName().str();

    return "Circuit ID " + std::to_string(CircuitId);
  }

  /// A helper iterator class for the QuantumCircuit.
  struct QuantumCircuitIterator {
  private:
    /// Pointer to the operation the iterator is currently
    /// at.
    QuantumCircuitOp *Op = nullptr;

    /// The circuit the iterator is looping over.
    QuantumCircuit *Circuit = nullptr;

    /// The operations the iterator has already checked.
    std::set<QuantumCircuitOp *> Visited;

    /// The current list of operations the iterator is
    /// checking.
    std::vector<QuantumCircuitOp *> Frontier;

    /// Boolean if we only want to contain operations involving
    /// specific qubits.
    bool OnlyCertainQubits = false;

    /// The list of qubits that should be used in an operation.
    std::set<QbitRef, QubitCompare> LimitedQubits;

    /// Move to the next operation.
    void advance();

    /// In the case of starting from a specific operation, iterate
    /// until we are on the particular operation.
    void pseudoAddVisited();

  public:
    /// Derefence to get the operation.
    QuantumCircuitOp *operator*() { return Op; }

    /// Prefix increment.
    QuantumCircuitIterator &operator++() {
      advance();
      return *this;
    }

    /// Postfix increment.
    QuantumCircuitIterator operator++(int I) {
      QuantumCircuitIterator It(*this);
      advance();
      return It;
    }

    /// Overloaded equality, equal if pointing at the same operation.
    bool operator==(const QuantumCircuitIterator &Other) const {
      return Op == Other.Op;
    }
    bool operator!=(const QuantumCircuitIterator &Other) const {
      return !(Op == Other.Op);
    }

    QuantumCircuitIterator(QuantumCircuitOp *StartingOp);
    QuantumCircuitIterator(QuantumCircuitOp *StartingOp,
                           std::set<QbitRef, QubitCompare> &LimitQubits);
  };

  typedef QuantumCircuitIterator iterator;
  /// Start at the initial start sentry operations.
  iterator begin() { return iterator(StartOp); }

  /// Start at the initial start sentry operations, but limit to specific
  /// qubits.
  iterator begin(std::set<QbitRef, QubitCompare> &LimitQubits) {
    return iterator(StartOp, LimitQubits);
  }
  iterator end() { return iterator(nullptr); }

  /// Helper iterator for iterating over time-slices.
  struct QuantumCircuitSliceIterator {
  private:
    // This could be a shared pointer instead.
    /// The current time slice iterator is pointing to.
    QuantumCircuitSlice Slice;

    /// The QuantumCircuit we are iterating over.
    QuantumCircuit *Circuit = nullptr;

    /// The starting operation of the circuit.
    QuantumCircuitOp *StartingOp = nullptr;

    /// The operations already checked.
    std::set<QuantumCircuitOp *> Visited;

    /// The frontier of operations we are looking at.
    std::vector<QuantumCircuitOp *> Frontier;

    /// Boolean if we only want to contain operations involving
    /// specific qubits.
    bool OnlyCertainQubits = false;

    /// The list of qubits that should be used in an operation.
    std::set<QbitRef, QubitCompare> LimitedQubits;

    /// A check as to whether iteration has started and can add
    /// to the time slice.
    bool StartProcessed = false;

    /// Create the next set of currently executable operations
    /// based on the frontier.
    void advance();

    /// In the case of starting from a specific operation, iterate
    /// until we are on the particular operation.
    void pseudoAddVisited();

  public:
    QuantumCircuitSlice operator*() { return Slice; }

    QuantumCircuitSliceIterator &operator++() {
      advance();
      return *this;
    }

    QuantumCircuitSliceIterator operator++(int I) {
      QuantumCircuitSliceIterator It(*this);
      advance();
      return It;
    }

    bool operator==(const QuantumCircuitSliceIterator &Other) const {
      // Use for all here
      return Slice == Other.Slice;
    }
    bool operator!=(const QuantumCircuitSliceIterator &Other) const {
      // Use for any here
      return !(Slice == Other.Slice);
    }

    QuantumCircuitSliceIterator(QuantumCircuitOp *StartingOp);
    QuantumCircuitSliceIterator(QuantumCircuitOp *StartingOp,
                                std::set<QbitRef, QubitCompare> &LimitQubits);
  };

  typedef QuantumCircuitSliceIterator slice_iterator;
  slice_iterator begin_slice() { return slice_iterator(StartOp); }
  slice_iterator begin_slice(std::set<QbitRef, QubitCompare> &LimitQubits) {
    return slice_iterator(StartOp, LimitQubits);
  }
  slice_iterator end_slice() { return slice_iterator(nullptr); }

  /// Helper function to check the call instruction and get a QbitRef
  /// to the argument value.  If it does not exists, the QbitRef is null.
  QbitRef getQbitRefFromValue(CallBase *CB, unsigned Idx);

  /// Helper function to check the call instruction and get a ParaRef
  /// to the argument value.  If it does not ParaRef, the QbitRef is null.
  ParaRef getParaRefFromValue(CallBase *CB, unsigned Idx);
};

} // namespace aqcc
} // namespace llvm

#endif