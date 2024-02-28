//===- QuantumPlacementUtils.h - Utility functions useful for Qubit Placement
//---===//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
//
// This file contains the classes and methods needed to do qubit placements from
// program qubits to physical qubits.
//
//===----------------------------------------------------------------===//

#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "llvm/IR/BasicBlock.h"

#include "IntelQuantumPasses/QuantumAnalysis/QuantumModule.h"
#include "IntelQuantumPasses/QuantumUtils/QuantumGeneralUtils.h"

#ifndef LLVM_QUANTUM_PLACEMENT_UTILS_H
#define LLVM_QUANTUM_PLACEMENT_UTILS_H

namespace llvm {

// OK for current QuISA
// Come up with a better way to combine a hash for future ISA
struct QubitPairHash {
  std::size_t
  operator()(const std::pair<unsigned, unsigned> &qubit_pair) const {
    return std::hash<unsigned>()(256 * qubit_pair.first + qubit_pair.second);
  }
};

struct QubitPairEq {
  bool operator()(const std::pair<unsigned, unsigned> &qubit_pair_1,
                  const std::pair<unsigned, unsigned> &qubit_pair_2) const {
    return qubit_pair_1 == qubit_pair_2;
  }
};

namespace aqcc {

/*
A heuristic based method to find a size (prog_qubits) subset of the platform's
physical qubits with higher connectivity. It performs BFS search on the
physical qubits to find the subsets, and then uses the config-json's Machine
class to calculate the connectivity based on the number of edges.
*/
int findBestDenseSet(QuantumModule &QM, unsigned prog_qubits,
                     std::set<unsigned> &best_subset);
struct ApplicationGraph {
  typedef unsigned Node_T;
  typedef std::unordered_set<Node_T> Adjacency_T;
  typedef std::unordered_map<Node_T, Adjacency_T> Graph_T;
  Graph_T graph;
  // Adds node to application graph
  void addNodeToApplicationGraph(Node_T qubit);
  // Adds edge to application graph
  void addEdgeToApplicationGraph(Node_T qubit_1, Node_T qubit_2);

  // Adds QBB to graph
  void addQBBToGraph(llvm::BasicBlock &QBB, QuantumModule &QM);
  // Creates application graph from all QBBs
  void constructApplicationGraph(QuantumModule &QM);
};

class LocalSearch {
private:
  // Number of iterations for each reset of local search
  size_t num_iterations;
  // How many times to "reset" to random placement and try again
  size_t num_resets;

public:
  // Default constructor for local search
  LocalSearch();

  // Computes h_ij for for each pair of physical qubits that are used
  void precomputeSwapHeuristic(std::vector<unsigned> &,
                               std::unordered_map<unsigned, unsigned> &,
                               QuantumModule &QM);
  // Computes value heuristic based on precomputation of h_ij
  double valueHeuristic(std::unordered_map<unsigned, unsigned> &,
                        ApplicationGraph &application_graph);
  // Computes value heuristic differential based on precomputation of h_ij
  double valueHeuristicDifferential(std::unordered_map<unsigned, unsigned> &,
                                    unsigned, unsigned,
                                    ApplicationGraph &application_graph);
  // Swaps qubits and updates current_placement and phys_to_program map
  void swapQubits(std::unordered_map<unsigned, unsigned> &, unsigned, unsigned);
  // Creates initial phys_to_program map
  void initializePhysToProgramMap(std::unordered_map<unsigned, unsigned> &);
  // Creates initial vector indicating which heuristic differentials need to
  // be recalculated. Initially this is all possible swaps containing at
  // least one program qubit.
  void initializeSwapsToCheck(std::vector<unsigned> &swaps_to_check,
                              std::unordered_map<unsigned, unsigned> &placement,
                              QuantumModule &QM);
  // Calculates which edges for which you have to recalculate h_ij based on
  // swapping given qubits After swapping the 2 phys qubits, this method
  // calculates which edges for which the value heuristic differential changes
  // If swapping qubits q1, q2, then for any 2 qubits q3, q4 that are neighbors
  // in the qubit connectivity graph if the intersection of (N(q3) U N(q4)) and
  // (N(q1) U N(q2)) is empty, where N(q) is the neigbors of q and q itself in
  // the application graph, then the value heuristic differential of swapping q3
  // and q4 does not change. This method computes N(q1) U N(q2), and adds the
  // edge of any neighbor in the qubit connectivity graph as a possible edge to
  // search.
  void updateSwapChecks(
      std::unordered_map<unsigned, unsigned> &current_placement,
      std::vector<unsigned> &swap_checks, unsigned phys_qubit_1,
      unsigned phys_qubit_2,
      std::unordered_map<unsigned, double> &placement_cost_differential,
      QuantumModule &QM, ApplicationGraph &application_graph);
  // Physical to Program qubit map
  std::unordered_map<unsigned, unsigned> phys_to_program_map;
  // For each pair (i,j) of physical qubits not in G_p, maps (i,j) -> h_ij
  std::unordered_map<std::pair<unsigned, unsigned>, double, QubitPairHash,
                     QubitPairEq>
      heuristic_cost;
  // Vector of pairs (i,j) of physical qubits in G_p
  std::vector<std::pair<unsigned, unsigned>> connected_pairs;
  // Maps qubit pair (i,j) in G_p to index into connected_pairs
  std::unordered_map<std::pair<unsigned, unsigned>, unsigned, QubitPairHash,
                     QubitPairEq>
      qubit_pair_to_index_map;

  // Resets class variables to be able to run local search again
  void localSearchReset();
  void setNumIterations(size_t);
  void setNumResets(size_t);
  // Do the local search!
  // Assumes we have already picked N fixed phys qubits to place the N program
  // qubits on.
  // Starts out at a random placement, half the time does the swap
  // that reduces the value heuristic the most, half the time does a
  // random swap ("noise move"). This helps get out of local minima

  // Assumption - anything in the application graph is eithier a fixed qubit or
  // in the physical_qubit_subset
  //
  std::unordered_map<unsigned, unsigned>
  localSearch(std::vector<unsigned> &program_qubit_subset,
              std::vector<unsigned> &physical_qubit_subset,
              std::unordered_map<unsigned, unsigned> &fixed_subset,
              std::vector<unsigned> &starting_qubit_phys_subset,
              ApplicationGraph &application_graph, QuantumModule &QM);
};

} // namespace aqcc
} // namespace llvm

#endif