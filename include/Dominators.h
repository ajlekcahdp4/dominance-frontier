// ----------------------------------------------------------------------------
//   "THE BEER-WARE LICENSE" (Revision 42):
//   <alex.rom23@mail.ru> wrote this file. As long as you retain this
//   notice you can do whatever you want with this stuff. If we meet some day,
//   and you think this stuff is worth it, you can buy me a beer in return.
// ----------------------------------------------------------------------------
#pragma once

#include "ReducibleGraph.h"

#include <llvm/ADT/DenseMap.h>
#include <llvm/ADT/STLExtras.h>
#include <llvm/ADT/SetOperations.h>

#include <functional>
#include <iostream>
#include <optional>
#include <unordered_map>
#include <vector>
namespace lqvm {

using NodetoDominatorsTy =
    std::unordered_map<const Node *, std::optional<std::set<const Node *>>>;
inline size_t ComputeHash(const NodetoDominatorsTy &M) {
  size_t Hash = 0;
  for (auto &&[NodePtr, Doms] : M) {
    if (Doms.has_value())
      for (auto *Dom : Doms.value())
        Hash += std::hash<const Node *>()(Dom);
  }
  return Hash;
}

inline void ComputeDominatorsIteration(const GraphTy &G,
                                       NodetoDominatorsTy &M) {
  std::set<const Node *> AllNodesSet;
  for (auto &Nd : G) {
    AllNodesSet.insert(&Nd);
  }
  for (auto &&Nd : G) {
    std::set<const Node *> Doms = AllNodesSet;
    for (auto *Parent : Nd.Parents) {
      if (M[Parent]) {
        llvm::set_intersect(Doms, M[Parent].value());
      }
    }
    Doms.insert(&Nd);
    if (!Nd.Parents.empty())
      M[&Nd] = std::move(Doms);
  }
}

inline NodetoDominatorsTy ComputeDominators(const GraphTy &G) {
  NodetoDominatorsTy NodeToDominators;
  NodeToDominators[&G.front()] = std::set<const Node *>{&G.front()};
  for (auto &&Node : llvm::drop_begin(G))
    NodeToDominators.insert(std::make_pair(&Node, std::nullopt));

  auto NewHash = ComputeHash(NodeToDominators);
  auto OldHash = NewHash;
  do {
    OldHash = NewHash;
    ComputeDominatorsIteration(G, NodeToDominators);
    NewHash = ComputeHash(NodeToDominators);
  } while (NewHash != OldHash);
  return NodeToDominators;
}

} // namespace lqvm