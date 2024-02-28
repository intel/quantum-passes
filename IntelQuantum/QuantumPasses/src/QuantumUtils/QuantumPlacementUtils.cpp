//===-- QuantumPlacementUtils.cpp
//------------------------------------------====//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
//
// This file implements a set of common utility functions for the quantum
// placement.
//
//===----------------------------------------------------------------------===//

#include <IntelQuantumPasses/QuantumUtils/QuantumPlacementUtils.h>
// For LLVM_DEBUG
#include "llvm/Support/Debug.h"
#include <algorithm>
#include <optional>
#include <random>

namespace llvm {
namespace aqcc {

void ApplicationGraph::addNodeToApplicationGraph(Node_T qubit) {
  std::pair<Node_T, Adjacency_T> new_qubit{qubit, {}};
  graph.insert(new_qubit);
}

void ApplicationGraph::addEdgeToApplicationGraph(Node_T qubit_1,
                                                 Node_T qubit_2) {
  addNodeToApplicationGraph(qubit_1);
  addNodeToApplicationGraph(qubit_2);
  graph[qubit_1].insert(qubit_2);
  graph[qubit_2].insert(qubit_1);
}

// Constructs application graph
// Creates an edge between any qubit with a 2 qubit gate
// 3-qubit gates are handled via a 3-clique as they will be lowered
// to 2-qubit gates eventually
void ApplicationGraph::addQBBToGraph(llvm::BasicBlock &QBB, QuantumModule &QM) {
  QBBIter qbb_iter(QBB);
  for (qbb_iter.gotoBegin(); !qbb_iter.isEnd(); ++qbb_iter) {
    auto qrefs = qbb_iter.getQubitOperands();
    assert(qrefs.size() > 0);
    assert(qrefs.size() < 4);
    if (qrefs.size() == 1) {
      addNodeToApplicationGraph(
          QM.qubit_maps[&QBB]->getIdFromProgQbitRef(qrefs[0]));
    } else if (qrefs.size() == 2) {
      addEdgeToApplicationGraph(
          QM.qubit_maps[&QBB]->getIdFromProgQbitRef(qrefs[0]),
          QM.qubit_maps[&QBB]->getIdFromProgQbitRef(qrefs[1]));
    } else if (qrefs.size() == 3) {
      addEdgeToApplicationGraph(
          QM.qubit_maps[&QBB]->getIdFromProgQbitRef(qrefs[0]),
          QM.qubit_maps[&QBB]->getIdFromProgQbitRef(qrefs[1]));
      addEdgeToApplicationGraph(
          QM.qubit_maps[&QBB]->getIdFromProgQbitRef(qrefs[1]),
          QM.qubit_maps[&QBB]->getIdFromProgQbitRef(qrefs[2]));
      addEdgeToApplicationGraph(
          QM.qubit_maps[&QBB]->getIdFromProgQbitRef(qrefs[0]),
          QM.qubit_maps[&QBB]->getIdFromProgQbitRef(qrefs[2]));
    }
  }
}

void ApplicationGraph::constructApplicationGraph(QuantumModule &QM) {
  for (auto &QK : QM) {
    for (auto &QBB : QK) {
      addQBBToGraph(QBB, QM);
    }
  }
}

int findBestDenseSet(QuantumModule &QM, unsigned prog_qubits,
                     std::set<unsigned> &best_subset) {

  if (best_subset.size() >= prog_qubits)
    return EXIT_SUCCESS;

  unsigned num_vertices = QM.getNumVertices();
  std::vector<unsigned> vec_indices = QM.getVecIndices();
  std::vector<unsigned> vec_targets = QM.getVecTargets();

  // getting the coordination number of each node is not free, so we will cache
  // them as we compute them
  std::vector<int> coord_value(num_vertices + 1, -1);
  coord_value[num_vertices] = 16; // set to a max value

  // if we have a non-empty best subset, we will use its boundary as the roots,
  // otherwise we use all vertices as our roots
  std::vector<unsigned> roots;
  if (best_subset.size() == 0) {
    for (size_t i = 0; i < num_vertices; i++)
      roots.push_back(i);
  } else {
    unsigned pos = 0;
    for (unsigned vertex : best_subset) {
      for (unsigned pos = vec_indices[vertex]; pos < vec_indices[vertex + 1];
           pos++) {
        unsigned target = vec_targets[pos];
        auto it = best_subset.find(target);
        if (it == best_subset.end())
          roots.push_back(target);
      }
    }
  }

  unsigned max_connectivity = 0;

  /*For faster convergence, We can calculate the max_connectivity for our
    nearest-neighbour topologies as follows: Every qubit in a nearest neighbour
    topology will have at most (8 edges). Thus, max connectivity of the desired
    subset can be 8*prog_qubits
  */
  unsigned max_possible_connectivity = 8 * prog_qubits;

  // 1. Perform BFS starting at every qubit in the topology. Thus BFS
  // implementation is
  // based on DirectedGraph::BreadthFirstSearch
  std::vector<unsigned> outcome;
  for (unsigned root : roots) {
    std::vector<unsigned> vec_markers(num_vertices, 0);
    std::vector<unsigned> bfs_list = {static_cast<unsigned int>(root)};

    vec_markers[root] = 1;
    for (unsigned best : best_subset)
      vec_markers[best] = 1;
    unsigned source, target;
    unsigned iteration = 1;

    std::vector<unsigned> new_found_vertices = bfs_list;
    std::vector<unsigned> sources_this_iteration;
    int num_left = prog_qubits - best_subset.size() - 1;

    while (new_found_vertices.size() > 0 && num_left > 0) {
      sources_this_iteration.resize(new_found_vertices.size());
      std::copy(new_found_vertices.begin(), new_found_vertices.end(),
                sources_this_iteration.begin());

      new_found_vertices.clear();
      iteration++;

      for (unsigned j = 0; j < sources_this_iteration.size(); j++) {
        source = sources_this_iteration[j];
        for (unsigned index = vec_indices[source];
             index < vec_indices[source + 1]; index++) {
          target = vec_targets[index];
          // check if vertex was already visited
          if (vec_markers[target] > 0) {
            // We allow initially marked to be skipped once
            // to avoid expansion around this root from getting
            // trapped between qubits in best_subset
            if (vec_markers[target] == 1) {
              vec_markers[target] = iteration;
              new_found_vertices.push_back(target);
            }
            continue;
          }
          // newly found vertex
          bfs_list.push_back(target);
          num_left--;
          vec_markers[target] = iteration;
          new_found_vertices.push_back(target);
        }
      }
    }

    // 2. Calculate the subset's approximate connectivity by calculating the sum
    // of edges for the subset qubits. The sum will also contain the edges
    // connecting the subset to other qubits.
    if (bfs_list.size() == 0)
      continue;

    unsigned connectivity = 0;

    // In some cases, the previous step may have more than needed elements
    // in which we remove the least connected elements.
    int num_to_rm = bfs_list.size() - prog_qubits + best_subset.size();
    num_to_rm = num_to_rm > 0 ? num_to_rm : 0;
    std::vector<unsigned> to_rm;
    std::vector<unsigned> connected_edges;
    for (size_t i = 0; i < bfs_list.size(); i++) {
      if (coord_value[bfs_list[i]] < 0) {
        QM.getVecEdgesConnectedToVertex(bfs_list[i], connected_edges);
        coord_value[bfs_list[i]] = connected_edges.size();
      }
      if (to_rm.size() < num_to_rm)
        to_rm.push_back(i);
      else {
        std::optional<unsigned> replacement;
        for (size_t j = 0; j < to_rm.size(); j++) {
          if (coord_value[bfs_list[i]] < coord_value[bfs_list[to_rm[j]]]) {
            if (replacement.has_value()) {
              if (coord_value[bfs_list[to_rm[j]]] <
                  coord_value[bfs_list[replacement.value()]]) {
                replacement = j;
              }
            } else {
              replacement = j;
            }
          }
        }
        if (replacement.has_value())
          to_rm[replacement.value()] = i;
      }
      connectivity += coord_value[bfs_list[i]];
    }

    for (auto rm : to_rm) {
      connectivity -= coord_value[bfs_list[rm]];
    }

    // 3. keep subgraph with max connectivity
    if (connectivity > max_connectivity) {
      max_connectivity = connectivity;
      for (auto rm : to_rm)
        bfs_list[rm] = num_vertices;
      std::swap(outcome, bfs_list);
    }

    // if max reached, break
    if (max_connectivity == max_possible_connectivity)
      break;

    // TODO:
    // 0.If N equally connected edges, use single and 2-qubit operations info
    // (noise, durations, etc) to pick the best qubits
    // 2. Use the power of adjacency-matrix as a measure of connectivity
    // 3. Take into account the program's connectivity as well
  }
  best_subset.insert(outcome.begin(), outcome.end());
  best_subset.erase(num_vertices);
  // 4. Re-order based on vertix indices (Can be changed)
  // Quicksort of best_subset:
  // quicksort(best_subset, 0, best_subset.size()-1);

  return EXIT_SUCCESS;
}

#define DEBUG_TYPE "localSearch"

// Precomputes value heuristic based on length of shortest path between 2 qubits
// in qubit connectivity graph
void LocalSearch::precomputeSwapHeuristic(
    std::vector<unsigned> &physical_qubit_subset,
    std::unordered_map<unsigned, unsigned> &fixed_placement,
    QuantumModule &QM) {
  int num_placement_qubits = physical_qubit_subset.size();
  unsigned connected_pairs_index = 0;
  for (int i = 0; i < num_placement_qubits; ++i) {
    for (int j = i + 1; j < num_placement_qubits; ++j) {
      unsigned qubit_0 = physical_qubit_subset[i];
      unsigned qubit_1 = physical_qubit_subset[j];
      if (qubit_1 < qubit_0) {
        std::swap(qubit_0, qubit_1);
      }
      unsigned swap_heuristic =
          QM.getDistanceBetweenQubits(qubit_0, qubit_1) - 1;
      std::pair<unsigned, unsigned> possible_pair = {qubit_0, qubit_1};
      LLVM_DEBUG(dbgs() << "Swap distance between " << qubit_0 << " " << qubit_1
                        << ": " << swap_heuristic << '\n');
      if (swap_heuristic > 0) {
        heuristic_cost[possible_pair] = swap_heuristic;
      } else {
        connected_pairs.push_back(possible_pair);
        qubit_pair_to_index_map[possible_pair] = connected_pairs_index++;
        LLVM_DEBUG(dbgs() << "Swap index " << qubit_0 << " " << qubit_1 << ": "
                          << qubit_pair_to_index_map[possible_pair] << '\n');
      }
    }

    // For fixed qubits, we inform the heuristic of the fixed qubit, but do not
    // add it as a possible edge to swap.
    // Here we calculate h_ij(qubit to place, fixed qubit)
    // Note: we ignore the cost of h_ij(fixed qubit, fixed qubit) as this
    // will always be the same.
    for (auto &fixed_qubit : fixed_placement) {
      unsigned qubit_0 = physical_qubit_subset[i];
      unsigned qubit_1 = fixed_qubit.second;
      if (qubit_1 < qubit_0) {
        std::swap(qubit_0, qubit_1);
      }
      unsigned swap_heuristic =
          QM.getDistanceBetweenQubits(qubit_0, qubit_1) - 1;
      std::pair<unsigned, unsigned> possible_pair = {qubit_0, qubit_1};
      LLVM_DEBUG(dbgs() << "Swap distance between " << qubit_0 << " " << qubit_1
                        << ": " << swap_heuristic << '\n');

      if (swap_heuristic > 0) {
        heuristic_cost[possible_pair] = swap_heuristic;
      }
    }
  }
}

// Calculates the value heuristic for a given placement
double LocalSearch::valueHeuristic(
    std::unordered_map<unsigned, unsigned> &current_placement,
    ApplicationGraph &application_graph) {
  double heuristic_value = 0;
  for (auto &node_pair : application_graph.graph) {
    for (auto &qubit : node_pair.second) {
      unsigned qid_1 = current_placement[node_pair.first];
      unsigned qid_2 = current_placement[qubit];
      // Avoid double counting
      if (qid_1 < qid_2) {
        std::pair<unsigned, unsigned> qubit_pair = {qid_1, qid_2};
        auto it = heuristic_cost.find(qubit_pair);
        if (it != heuristic_cost.end())
          heuristic_value += it->second;
      }
    }
  }
  return heuristic_value;
}

// Given the current placement this method calculates the difference between the
// current value heuristic and the value heuristic if we swap the given qubits.
// Since the only changes to the heuristic are on the neighbors in the
// application graph, this is much faster then computing the entire heuristic
double LocalSearch::valueHeuristicDifferential(
    std::unordered_map<unsigned, unsigned> &current_placement, unsigned qubit_1,
    unsigned qubit_2, ApplicationGraph &application_graph) {
  // By construction qubit 1 connected to qubit 2
  double differential_cost = 0;

  auto program_qubit_1_iterator = phys_to_program_map.find(qubit_1);
  if (program_qubit_1_iterator != phys_to_program_map.end()) {
    auto program_qubit_1_neighbors =
        application_graph.graph[program_qubit_1_iterator->second];
    for (auto &vertex : program_qubit_1_neighbors) {
      unsigned placement_vertex = current_placement[vertex];
      std::pair<unsigned, unsigned> qubit_pair =
          std::make_pair(qubit_1, placement_vertex);
      if (qubit_pair.second < qubit_pair.first)
        std::swap(qubit_pair.first, qubit_pair.second);
      auto it = heuristic_cost.find(qubit_pair);
      if (it != heuristic_cost.end()) {
        differential_cost -= it->second;
      }

      qubit_pair = std::make_pair(qubit_2, placement_vertex);
      if (qubit_pair.second < qubit_pair.first)
        std::swap(qubit_pair.first, qubit_pair.second);
      it = heuristic_cost.find(qubit_pair);
      if (it != heuristic_cost.end()) {
        differential_cost += it->second;
      }
    }
  }

  auto program_qubit_2_iterator = phys_to_program_map.find(qubit_2);
  if (program_qubit_2_iterator != phys_to_program_map.end()) {
    auto program_qubit_2_neighbors =
        application_graph.graph[program_qubit_2_iterator->second];
    for (auto &vertex : program_qubit_2_neighbors) {
      unsigned placement_vertex = current_placement[vertex];
      std::pair<unsigned, unsigned> qubit_pair =
          std::make_pair(qubit_2, placement_vertex);
      if (qubit_pair.second < qubit_pair.first)
        std::swap(qubit_pair.first, qubit_pair.second);
      auto it = heuristic_cost.find(qubit_pair);
      if (it != heuristic_cost.end()) {
        differential_cost -= it->second;
      }

      qubit_pair = std::make_pair(qubit_1, placement_vertex);
      if (qubit_pair.second < qubit_pair.first)
        std::swap(qubit_pair.first, qubit_pair.second);
      it = heuristic_cost.find(qubit_pair);
      if (it != heuristic_cost.end()) {
        differential_cost += it->second;
      }
    }
  }

  return differential_cost;
}

void LocalSearch::swapQubits(
    std::unordered_map<unsigned, unsigned> &current_placement, unsigned qubit_1,
    unsigned qubit_2) {
  // Updates current placement and phys_to_program_map
  // If swap contains phys qubits that have no corresponding program qubit,
  // we remove the entry in the phys_to_program map if it exists for the
  // swapped qubit

  std::optional<unsigned> program_qubit_1;
  std::optional<unsigned> program_qubit_2;

  auto program_qubit_1_iterator = phys_to_program_map.find(qubit_1);
  if (program_qubit_1_iterator != phys_to_program_map.end()) {
    program_qubit_1 = program_qubit_1_iterator->second;
  }

  auto program_qubit_2_iterator = phys_to_program_map.find(qubit_2);
  if (program_qubit_2_iterator != phys_to_program_map.end()) {
    program_qubit_2 = program_qubit_2_iterator->second;
  }

  if (program_qubit_1.has_value()) {
    current_placement[program_qubit_1.value()] = qubit_2;
    phys_to_program_map[qubit_2] = program_qubit_1.value();
  } else {
    phys_to_program_map.erase(qubit_2);
  }

  if (program_qubit_2.has_value()) {
    current_placement[program_qubit_2.value()] = qubit_1;
    phys_to_program_map[qubit_1] = program_qubit_2.value();
  } else {
    phys_to_program_map.erase(qubit_1);
  }
}

void LocalSearch::initializePhysToProgramMap(
    std::unordered_map<unsigned, unsigned> &current_placement) {
  phys_to_program_map.clear();
  for (auto &placement_pair : current_placement) {
    phys_to_program_map[placement_pair.second] = placement_pair.first;
  }
}

void LocalSearch::updateSwapChecks(
    std::unordered_map<unsigned, unsigned> &current_placement,
    std::vector<unsigned> &swap_checks, unsigned phys_qubit_1,
    unsigned phys_qubit_2,
    std::unordered_map<unsigned, double> &placement_cost_differential,
    QuantumModule &QM, ApplicationGraph &application_graph) {
  swap_checks.clear();

  std::unordered_set<unsigned> neighbors_qubit_1;
  std::unordered_set<unsigned> neighbors_qubit_2;
  std::unordered_set<unsigned> swap_qubits_neighbors;

  auto program_qubit_1_iterator = phys_to_program_map.find(phys_qubit_1);
  if (program_qubit_1_iterator != phys_to_program_map.end()) {
    neighbors_qubit_1 =
        application_graph.graph[program_qubit_1_iterator->second];
    swap_qubits_neighbors.insert(program_qubit_1_iterator->second);
  }

  auto program_qubit_2_iterator = phys_to_program_map.find(phys_qubit_2);
  if (program_qubit_2_iterator != phys_to_program_map.end()) {
    neighbors_qubit_2 =
        application_graph.graph[program_qubit_2_iterator->second];
    swap_qubits_neighbors.insert(program_qubit_2_iterator->second);
  }

  std::set_union(
      neighbors_qubit_1.begin(), neighbors_qubit_1.end(),
      neighbors_qubit_2.begin(), neighbors_qubit_2.end(),
      std::inserter(swap_qubits_neighbors, std::begin(swap_qubits_neighbors)));

  // Map program qubits + neighbors to physical qubits
  // There's an edge case where phys_qubit_1 and phys_qubit_2 do not both
  // correspond to program qubits. We manually insert them both to make
  // sure they are in the mapped_phys_qubits set.
  std::unordered_set<unsigned> mapped_phys_qubits;
  std::transform(
      swap_qubits_neighbors.begin(), swap_qubits_neighbors.end(),
      std::inserter(mapped_phys_qubits, std::begin(mapped_phys_qubits)),
      [&current_placement](auto &val) { return current_placement[val]; });
  mapped_phys_qubits.insert(phys_qubit_1);
  mapped_phys_qubits.insert(phys_qubit_2);

  std::unordered_set<unsigned> inserted_edges;

  for (auto &phys_qubit : mapped_phys_qubits) {
    std::vector<unsigned> connected_qubits;
    QM.getQubitsConnectedToQubit(phys_qubit, connected_qubits);
    for (auto &connected_qubit : connected_qubits) {
      std::pair<unsigned, unsigned> qubit_pair =
          std::make_pair(phys_qubit, connected_qubit);
      if (qubit_pair.second < qubit_pair.first)
        std::swap(qubit_pair.first, qubit_pair.second);
      // Only look at edges that are being swapped.
      if (qubit_pair_to_index_map.find(qubit_pair) ==
          qubit_pair_to_index_map.end())
        continue;
      unsigned pair_index = qubit_pair_to_index_map[qubit_pair];
      // If both phys qubits do not correspond to any program qubits, then
      // remove these from the possible swaps to consider!
      if (phys_to_program_map.find(qubit_pair.first) ==
              phys_to_program_map.end() &&
          phys_to_program_map.find(qubit_pair.second) ==
              phys_to_program_map.end()) {
        placement_cost_differential.erase(pair_index);
        continue;
      }
      auto it = inserted_edges.find(pair_index);
      if (it == inserted_edges.end()) {
        swap_checks.push_back(pair_index);
        inserted_edges.insert(pair_index);
      }
    }
  }

  // Remove swap if it exists (we know how to calculate it already!)
  std::pair<unsigned, unsigned> qubit_pair =
      std::make_pair(phys_qubit_1, phys_qubit_2);
  auto it = std::find(swap_checks.begin(), swap_checks.end(),
                      qubit_pair_to_index_map[qubit_pair]);
  if (it != swap_checks.end())
    swap_checks.erase(it);
}

void LocalSearch::initializeSwapsToCheck(
    std::vector<unsigned> &swaps_to_check,
    std::unordered_map<unsigned, unsigned> &placement, QuantumModule &QM) {
  std::unordered_set<unsigned> swaps_added;
  for (auto &program_qubit : placement) {
    std::vector<unsigned> connected_qubits;
    QM.getQubitsConnectedToQubit(program_qubit.second, connected_qubits);
    for (auto &neighbor : connected_qubits) {
      std::pair<unsigned, unsigned> qubit_pair =
          std::make_pair(neighbor, program_qubit.second);
      if (qubit_pair.second < qubit_pair.first)
        std::swap(qubit_pair.first, qubit_pair.second);
      // Only look at edges that are being swapped.
      auto index_it = qubit_pair_to_index_map.find(qubit_pair);
      if (index_it == qubit_pair_to_index_map.end())
        continue;

      if (swaps_added.find(index_it->second) == swaps_added.end()) {
        swaps_to_check.push_back(index_it->second);
        swaps_added.insert(index_it->second);
      }
    }
  }
}

std::unordered_map<unsigned, unsigned>
LocalSearch::localSearch(std::vector<unsigned> &program_qubit_subset,
                         std::vector<unsigned> &physical_qubit_subset,
                         std::unordered_map<unsigned, unsigned> &fixed_subset,
                         std::vector<unsigned> &starting_qubit_phys_subset,
                         ApplicationGraph &application_graph,
                         QuantumModule &QM) {
  // We allow program_qubit_subset.size() < physical_qubit_subset.size()
  // In this case, there are more physical qubits than program qubits,
  // and let local search consider these in the search, but will only
  // place program_qubit_subset.size() of them in the final placement
  // that it comes up with.
  if (program_qubit_subset.size() > physical_qubit_subset.size())
    displayErrorAndExit(
        "LocalSearch",
        "Tried to place " + std::to_string(program_qubit_subset.size()) +
            " program qubits onto " +
            std::to_string(physical_qubit_subset.size()) + " physical qubits.");
  std::vector<unsigned> fixed_qubits;
  std::transform(fixed_subset.begin(), fixed_subset.end(),
                 std::back_inserter(fixed_qubits),
                 [](auto &pair) { return pair.second; });
  std::vector<unsigned> intersection;
  std::set_intersection(
      fixed_qubits.begin(), fixed_qubits.end(), physical_qubit_subset.begin(),
      physical_qubit_subset.end(), std::back_inserter(intersection));

  if (intersection.size() > 0)
    displayErrorAndExit("LocalSearch",
                        "Tried to place a qubit onto an already fixed qubit.");

  std::vector<unsigned> difference;
  std::set_difference(
      starting_qubit_phys_subset.begin(), starting_qubit_phys_subset.end(),
      physical_qubit_subset.begin(), physical_qubit_subset.end(),
      std::back_inserter(difference));
  if (!difference.empty())
    displayErrorAndExit("LocalSearch",
                        "Tried to use invalid starting qubit set.");

  int num_placement_qubits = physical_qubit_subset.size();
  precomputeSwapHeuristic(physical_qubit_subset, fixed_subset, QM);

  // Use deterministic seed!!
  int seed = 0;
  std::default_random_engine rng(seed);
  size_t num_program_qubits = program_qubit_subset.size();
  std::unordered_map<unsigned, unsigned> best_placement = fixed_subset;
  for (size_t index = 0; index < num_program_qubits; ++index) {
    best_placement[program_qubit_subset[index]] = physical_qubit_subset[index];
  }

  // If there are no pairs to swap, just return a placement
  if (connected_pairs.empty())
    return best_placement;

  // If there are no qubits to place, just return a placement
  if (num_program_qubits == 0)
    return best_placement;

  double best_placement_cost =
      valueHeuristic(best_placement, application_graph);

  std::bernoulli_distribution random_distribution(0.5);

  // Each reset starts from a random qubit placement
  for (int reset = 0; reset < num_resets; ++reset) {
    LLVM_DEBUG(dbgs() << "Running iteration " << reset << '\n');
    std::unordered_map<unsigned, unsigned> current_placement = fixed_subset;
    std::vector<unsigned> starting_subset = starting_qubit_phys_subset;
    unsigned num_starting_qubit_subset = starting_qubit_phys_subset.size();

    std::vector<unsigned> shuffled_placement;
    std::set_difference(
        physical_qubit_subset.begin(), physical_qubit_subset.end(),
        starting_qubit_phys_subset.begin(), starting_qubit_phys_subset.end(),
        std::back_inserter(shuffled_placement));
    std::shuffle(shuffled_placement.begin(), shuffled_placement.end(), rng);

    for (size_t index = 0;
         index < num_program_qubits - num_starting_qubit_subset; ++index) {
      starting_subset.push_back(shuffled_placement[index]);
    }

    std::shuffle(starting_subset.begin(), starting_subset.end(), rng);

    for (size_t index = 0; index < num_program_qubits; ++index) {
      current_placement[program_qubit_subset[index]] = starting_subset[index];
    }

    initializePhysToProgramMap(current_placement);

    double current_placement_cost =
        valueHeuristic(current_placement, application_graph);
    // swaps_to_check[i] corresponds to index into connected_pairs vector
    std::vector<unsigned> swaps_to_check;
    // Initialize swaps to check
    initializeSwapsToCheck(swaps_to_check, current_placement, QM);
    // Each iteration swaps an edge on the qubit connectivity graph to obtain
    // a new placement.

    std::unordered_map<unsigned, double> placement_cost_differential;

    for (int iteration = 0; iteration < num_iterations; ++iteration) {
      bool use_random_swap = random_distribution(rng);

      // Compute heuristic cost for each pair of connected qubits
      // Calculate heuristic inplace instead of copying placement!
      for (auto &swap_index : swaps_to_check) {
        std::pair<unsigned, unsigned> swap = connected_pairs[swap_index];
        double cost_differential = valueHeuristicDifferential(
            current_placement, swap.first, swap.second, application_graph);

        double cost = current_placement_cost + cost_differential;
        // Avoid edge cases later on so we can assume placement_cost[index] > 0
        // If cost is 0, we are done
        if (cost == 0) {
          swapQubits(current_placement, swap.first, swap.second);
          assert(valueHeuristic(current_placement, application_graph) == 0);
          return current_placement;
        }
        placement_cost_differential[swap_index] = cost_differential;
      }

      // Random probability distribution is half completely random and half
      // linear based on heuristic differential
      if (use_random_swap) {
        // Sort heuristic cost by the total cost
        std::vector<std::pair<unsigned, double>> heuristic_costs{
            placement_cost_differential.begin(),
            placement_cost_differential.end()};
        std::sort(heuristic_costs.begin(), heuristic_costs.end(),
                  [&current_placement_cost](
                      const std::pair<unsigned, double> pair_1,
                      const std::pair<unsigned, double> pair_2) {
                    double cost = current_placement_cost + pair_1.second;
                    double compared_cost =
                        current_placement_cost + pair_2.second;
                    return (1.0 / cost) < (1.0 / compared_cost);
                  });

        unsigned num_possible_swaps = heuristic_costs.size();
        std::vector<double> swap_probability(num_possible_swaps);
        for (unsigned i = 0; i < num_possible_swaps; ++i) {
          std::pair<unsigned, unsigned> qubit_pair =
              connected_pairs[heuristic_costs[i].first];
          swap_probability[i] =
              (1.0 / (2.0 * num_possible_swaps)) +
              ((double)(i + 1)) /
                  ((double)(num_possible_swaps * (num_possible_swaps + 1)));
        }

        std::discrete_distribution<unsigned> swap_distribution(
            swap_probability.begin(), swap_probability.end());
        unsigned rng_index = swap_distribution(rng);
        unsigned swap_index = heuristic_costs[rng_index].first;
        std::pair<unsigned, unsigned> qubits_to_swap =
            connected_pairs[swap_index];
        swapQubits(current_placement, qubits_to_swap.first,
                   qubits_to_swap.second);
        current_placement_cost += placement_cost_differential[swap_index];
        updateSwapChecks(current_placement, swaps_to_check,
                         qubits_to_swap.first, qubits_to_swap.second,
                         placement_cost_differential, QM, application_graph);
        placement_cost_differential[swap_index] =
            -placement_cost_differential[swap_index];
      } else {
        auto swap_minimum_it =
            std::min_element(placement_cost_differential.begin(),
                             placement_cost_differential.end(),
                             [](const auto &pair_1, const auto &pair_2) {
                               return pair_1.second < pair_2.second;
                             });
        std::pair<unsigned, unsigned> qubits_to_swap =
            connected_pairs[swap_minimum_it->first];
        swapQubits(current_placement, qubits_to_swap.first,
                   qubits_to_swap.second);
        current_placement_cost += swap_minimum_it->second;
        updateSwapChecks(current_placement, swaps_to_check,
                         qubits_to_swap.first, qubits_to_swap.second,
                         placement_cost_differential, QM, application_graph);
        placement_cost_differential[swap_minimum_it->first] =
            -swap_minimum_it->second;
      }

      LLVM_DEBUG(dbgs() << "Iteration: " << iteration << " "
                        << current_placement_cost << '\n');
      if (best_placement_cost > current_placement_cost) {
        best_placement = current_placement;
        best_placement_cost = current_placement_cost;
      }
    }
    LLVM_DEBUG(dbgs() << "Best cost for reset " << reset << " "
                      << best_placement_cost << '\n');
  }

  // Maybe best to have this assert somewhere to make sure the differential
  // logic is valid
  assert(valueHeuristic(best_placement, application_graph) ==
         best_placement_cost);
  return best_placement;
}

void LocalSearch::localSearchReset() {
  phys_to_program_map.clear();
  heuristic_cost.clear();
  connected_pairs.clear();
  qubit_pair_to_index_map.clear();
}

void LocalSearch::setNumIterations(size_t num_iterations) {
  this->num_iterations = num_iterations;
}

void LocalSearch::setNumResets(size_t num_resets) {
  this->num_resets = num_resets;
}

LocalSearch::LocalSearch() {
  // Set some reasonable defaults
  num_resets = 1;
  num_iterations = 100000;
}

#undef DEBUG_TYPE

} // namespace aqcc
} // namespace llvm