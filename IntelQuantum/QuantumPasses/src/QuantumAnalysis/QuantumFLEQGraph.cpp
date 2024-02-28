//===- QuantumFLEQGraph.cpp -------------------------------------*- C++ -*-===//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
/// \file "QuantumFLEQGraph.cpp"
///
/// This file implements the methods from "QuantumFLEQGraph.h"
///
//===----------------------------------------------------------------------===//

#include "IntelQuantumPasses/QuantumAnalysis/QuantumFLEQGraph.h"
#include "llvm/Support/raw_ostream.h"

namespace llvm {
namespace fleq {

FLEQNode *FLEQGraph::getNode(const Value *F) const {
  FLEQFunctionMapTy::const_iterator I = FLEQFunctionMap.find(F);
  if (I == FLEQFunctionMap.end())
    return nullptr;
  return I->second.get();
}

FLEQNode *FLEQGraph::getOrInsertNode(Value *F) {

  assert(F != nullptr);

  FLEQNode *FN = getNode(F);
  if (FN) {
    return FN;
  }

  FLEQFunctionMap.insert(std::make_pair(
      F, std::make_unique<FLEQNode>(F, characterizeNodeType(F))));
  return FLEQFunctionMap[F].get();
}

FLEQNode *FLEQGraph::addToFLEQGraph(Value *val) {
  assert(val != nullptr);
  FLEQNode *newnode = getOrInsertNode(val);

  if (newnode->getListSize() == 0) { // needed to stop double populating list
    SmallVector<Value *, 5> lst1 =
        getFLEQDependencies(val, newnode->getNodeType());
    SmallVector<Value *, 5>::iterator itr1;

    for (itr1 = lst1.begin(); itr1 != lst1.end(); itr1++) {
      newnode->addCallee(getOrInsertNode(*itr1));
      FLEQNode *child = addToFLEQGraph(*itr1);
      (child->parent_list_).push_back(newnode);
    }
  }

  return newnode;
}

FLEQNode *FLEQGraph::replaceNodeWith(FLEQNode *FN, Value *val) {

  FLEQNode *newnode = addToFLEQGraph(val);

  // go through FNs deps to see if new node is in there
  auto dep_old = (newnode->parent_list_).begin();
  auto end = (newnode->parent_list_).end();
  for (; dep_old != end; ++dep_old) {
    if (*dep_old == FN)
      break;
  }

  // go through FNs parent nodes and replace with new node in deps
  for (auto parent : FN->parent_list_) {
    if (parent == newnode)
      continue;
    auto &deps = parent->dependency_list_;
    for (int i = 0; i < deps.size(); i++) {
      if (deps[i] == FN) {
        deps[i] = newnode;
        if (dep_old != end) {
          *dep_old = parent;
          dep_old = end;
        } else {
          (newnode->parent_list_).push_back(parent);
        }
      }
    }
  }

  // leave FN in graph but detach it from root
  (FN->parent_list_).clear();

  // Deaden old node and all dead children
  setNodesAsDead({FN});

  return newnode;
}

void FLEQGraph::setNodesAsDead(const SmallVector<FLEQNode *, 5> &nodes) {

  if (nodes.size() == 0)
    return;

  SmallVector<FLEQNode *, 5> pass_fwd;

  for (auto node : nodes) {
    bool is_node_dead = true;
    for (auto parent : node->parent_list_) {
      is_node_dead = is_node_dead && (parent->type_ == kDead);
    }

    if (!is_node_dead)
      continue;

    node->type_ = kDead;
    for (auto child : *node)
      pass_fwd.push_back(child);
  }

  return setNodesAsDead(pass_fwd);
}

} // namespace fleq
} // namespace llvm