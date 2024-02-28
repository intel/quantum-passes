//===- QuantumFLEQGraph.h -------------------------------------*- C++ -*-===//
//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
/// \file "QuantumFLEQGraph.h"
///
/// This file defines the class FLEQGraph and related classes. The FLEQGraph
/// represents the dependency of calls which feed into a FLEQ evaluation call.
/// This is designed based on the clang CallGraph and can be used in conjuction
/// with any LLVM tools which require a specialization of GraphTraits.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_FLEQ_GRAPH_H
#define LLVM_FLEQ_GRAPH_H

#include "IntelQuantumPasses/QuantumAnalysis/QuantumRefs.h"
#include "IntelQuantumPasses/QuantumUtils/QuantumFLEQUtils.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/GraphTraits.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SetVector.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/IR/Value.h"
#include <iostream>
#include <iterator>
#include <list>
#include <map>
#include <set>

namespace llvm {
namespace fleq {

class FLEQNode {

public:
  using FLEQCallRecord = FLEQNode *;

private:
  Value *function_call_;
  NodeType type_ = kNULL;

  // List of function call dependencies (limited to FLEQ and QExpr calls)
  SmallVector<FLEQCallRecord, 5> dependency_list_;
  SmallVector<FLEQCallRecord, 5> parent_list_;

  // Added for testing
  // List of dependencies in Value*
  SmallVector<Value *, 5> dependency_list_val;

public:
  FLEQNode() = delete;
  FLEQNode(Value *call) : function_call_(call) { assert(call != nullptr); }
  FLEQNode(Value *call, NodeType ty) : function_call_(call), type_(ty) {
    assert(call != nullptr);
  }
  ~FLEQNode() = default;

  friend bool operator==(const FLEQNode &node1, const FLEQNode &node2) {
    return node1.function_call_ == node2.function_call_;
  }

  operator Value &() const { return *function_call_; }

  static bool classof(Value *V) { return true; }

  using iterator = SmallVectorImpl<FLEQCallRecord>::iterator;
  using const_iterator = SmallVectorImpl<FLEQCallRecord>::const_iterator;

  /// Iterators through all the callees/children of the node.
  iterator begin() { return dependency_list_.begin(); }
  iterator end() { return dependency_list_.end(); }
  const_iterator begin() const { return dependency_list_.begin(); }
  const_iterator end() const { return dependency_list_.end(); }

  bool empty() const { return dependency_list_.empty(); }
  unsigned size() const { return dependency_list_.size(); }
  unsigned getListSize() const { return dependency_list_.size(); }

  void addCallee(FLEQNode *Nd) { dependency_list_.push_back(Nd); }

  void swapChildren() {
    if (dependency_list_.size() == 2) {
      FLEQCallRecord temp = dependency_list_[0];
      dependency_list_[0] = dependency_list_[1];
      dependency_list_[1] = temp;
    }
  }

  void dump() const;

  const SmallVector<FLEQCallRecord, 5> &getDependencyList() const {
    return dependency_list_;
  }
  const SmallVector<FLEQCallRecord, 5> &getParentList() const {
    return parent_list_;
  }
  Value *getFunctionCall() const { return function_call_; }
  NodeType getNodeType() const { return type_; }
  inline bool hasUniqueParent() { return parent_list_.size() == 1; }
  inline bool hasNoParent() { return parent_list_.size() == 0; }
  inline bool hasUniqueChild() { return dependency_list_.size() == 1; }

  friend raw_ostream &operator<<(raw_ostream &stream, const FLEQNode &FN) {
    stream << aqcc::getPrettyName(FN.function_call_);
    return stream;
  }

  friend class FLEQGraph;
};

class FLEQGraph {

  friend class FLEQNode;

  // Needed to add typename in the line below, otheriwse get ISO C++ error...
  using FLEQFunctionMapTy = DenseMap<Value *, std::unique_ptr<FLEQNode>>;

  FLEQFunctionMapTy FLEQFunctionMap;

  FLEQNode *Root;

public:
  FLEQGraph() = delete;

  FLEQGraph(Value *val) {
    Root = getOrInsertNode(val);
    addToFLEQGraph(val);
  }
  ~FLEQGraph() = default;

  /// Lookup the node for the given declaration. If none found, insert one into
  /// the graph.
  FLEQNode *getOrInsertNode(Value *);

  /// Recursively add a node and its dependencies
  /// Returns the new inserted node
  FLEQNode *addToFLEQGraph(Value *val);

  /// Lookup the node for the given declaration.
  FLEQNode *getNode(const Value *) const;

  /// Replace the passed node with a new node build from Value and
  /// add dependencies to graph.
  FLEQNode *replaceNodeWith(FLEQNode *, Value *);

  /// set nodes as dead as well as all dead-dependent nodes as dead
  void setNodesAsDead(const SmallVector<FLEQNode *, 5> &);

  /// Iterator defs...
  using iterator = FLEQFunctionMapTy::iterator;
  using const_iterator = FLEQFunctionMapTy::const_iterator;

  iterator begin() { return FLEQFunctionMap.begin(); }
  iterator end() { return FLEQFunctionMap.end(); }
  const_iterator begin() const { return FLEQFunctionMap.begin(); }
  const_iterator end() const { return FLEQFunctionMap.end(); }

  // get Graph size
  unsigned size() const { return FLEQFunctionMap.size(); }

  // get Root
  FLEQNode *getRoot() const { return Root; }

  /// Iterators through all the nodes of the graph that have no parent. These
  /// are the unreachable nodes, which are either unused or are due to us
  /// failing to add a call edge due to the analysis imprecision.
  using nodes_iterator = SetVector<FLEQNode *>::iterator;
  using const_nodes_iterator = SetVector<FLEQNode *>::const_iterator;

  void dump() const;

  friend raw_ostream &operator<<(raw_ostream &stream, const FLEQGraph &FG) {
    for (auto &pair : FG.FLEQFunctionMap) {
      stream << *pair.second << "\n";
    }
    return stream;
  }
};

} // namespace fleq

// Graph traits...
template <> struct GraphTraits<fleq::FLEQNode *> {
  using NodeType = fleq::FLEQNode;
  using NodeRef = fleq::FLEQNode *;
  using ChildIteratorType = NodeType::iterator;

  static NodeType *getEntryNode(fleq::FLEQNode *FGN) { return FGN; }
  static ChildIteratorType child_begin(NodeType *N) { return N->begin(); }
  static ChildIteratorType child_end(NodeType *N) { return N->end(); }
};

template <> struct GraphTraits<const fleq::FLEQNode *> {
  using NodeType = const fleq::FLEQNode;
  using NodeRef = const fleq::FLEQNode *;
  using ChildIteratorType = NodeType::const_iterator;

  static NodeType *getEntryNode(const fleq::FLEQNode *FGN) { return FGN; }
  static ChildIteratorType child_begin(NodeType *N) { return N->begin(); }
  static ChildIteratorType child_end(NodeType *N) { return N->end(); }
};

template <>
struct GraphTraits<fleq::FLEQGraph *> : public GraphTraits<fleq::FLEQNode *> {
  static NodeType *getEntryNode(fleq::FLEQGraph *FG) {
    return FG->getRoot(); // Start at the external node!
  }

  static fleq::FLEQNode *
  FGGetValue(fleq::FLEQGraph::const_iterator::value_type &P) {
    return P.second.get();
  }

  // nodes_iterator/begin/end - Allow iteration over all nodes in the graph
  using nodes_iterator =
      mapped_iterator<fleq::FLEQGraph::iterator, decltype(&FGGetValue)>;

  static nodes_iterator nodes_begin(fleq::FLEQGraph *FG) {
    return nodes_iterator(FG->begin(), &FGGetValue);
  }

  static nodes_iterator nodes_end(fleq::FLEQGraph *FG) {
    return nodes_iterator(FG->end(), &FGGetValue);
  }

  static unsigned size(fleq::FLEQGraph *FG) { return FG->size(); }
};

template <>
struct GraphTraits<const fleq::FLEQGraph *>
    : public GraphTraits<const fleq::FLEQNode *> {
  static NodeType *getEntryNode(const fleq::FLEQGraph *FGN) {
    return FGN->getRoot();
  }

  static fleq::FLEQNode *
  FGGetValue(fleq::FLEQGraph::const_iterator::value_type &P) {
    return P.second.get();
  }

  // nodes_iterator/begin/end - Allow iteration over all nodes in the graph
  using nodes_iterator =
      mapped_iterator<fleq::FLEQGraph::const_iterator, decltype(&FGGetValue)>;

  static nodes_iterator nodes_begin(const fleq::FLEQGraph *FG) {
    return nodes_iterator(FG->begin(), &FGGetValue);
  }

  static nodes_iterator nodes_end(const fleq::FLEQGraph *FG) {
    return nodes_iterator(FG->end(), &FGGetValue);
  }

  static unsigned size(const fleq::FLEQGraph *FG) { return FG->size(); }
};

} // namespace llvm

#endif // LLVM_FLEQ_GRAPH_H
