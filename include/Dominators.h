// ----------------------------------------------------------------------------
//   "THE BEER-WARE LICENSE" (Revision 42):
//   <alex.rom23@mail.ru> wrote this file. As long as you retain this
//   notice you can do whatever you want with this stuff. If we meet some day,
//   and you think this stuff is worth it, you can buy me a beer in return.
// ----------------------------------------------------------------------------
#pragma once

#include "ReducibleGraph.h"
#include "Utils.h"

#include <llvm/ADT/DenseMap.h>
#include <llvm/ADT/STLExtras.h>
#include <llvm/ADT/SetOperations.h>
#include <llvm/ADT/SetVector.h>

#include <functional>
#include <iostream>
#include <optional>
#include <unordered_map>
#include <vector>
namespace lqvm {

using NodetoDominatorsTy =
    std::unordered_map<const Node *, std::vector<const Node *>>;
inline size_t ComputeHash(const NodetoDominatorsTy &M) {
  size_t Hash = 0;
  for (auto &&[NodePtr, Doms] : M) {
    for (auto *Dom : Doms)
      Hash += std::hash<const Node *>()(Dom);
  }
  return Hash;
}

inline void ComputeDominatorsIteration(const GraphTy &G,
                                       NodetoDominatorsTy &M) {
  std::vector<const Node *> AllNodesSet;
  for (auto &Nd : G)
    AllNodesSet.push_back(&Nd);
  for (auto &&Nd : G) {
    std::vector<const Node *> Doms = AllNodesSet;
    for (auto *Parent : Nd.Parents)
      if (M.contains(Parent))
        Doms = OrderedIntersection(Doms, M[Parent]);
    Doms.push_back(&Nd);
    if (!Nd.Parents.empty())
      M[&Nd] = std::move(Doms);
  }
}

inline NodetoDominatorsTy ComputeDominators(const GraphTy &G) {
  NodetoDominatorsTy NodeToDominators;
  NodeToDominators[&G.front()] = std::vector<const Node *>{&G.front()};
  auto NewHash = ComputeHash(NodeToDominators);
  auto OldHash = NewHash;
  do {
    OldHash = NewHash;
    ComputeDominatorsIteration(G, NodeToDominators);
    NewHash = ComputeHash(NodeToDominators);
  } while (NewHash != OldHash);
  return NodeToDominators;
}

inline std::vector<std::pair<const Node *, const Node *>>
ComputeIDom(const GraphTy &G) {
  auto NodeToDoms = ComputeDominators(G);
  std::vector<std::pair<const Node *, const Node *>> Res;
  for (auto &&[NodePtr, Doms] : NodeToDoms)
    Res.emplace_back(NodePtr, *std::prev(Doms.end(), 2));
  return Res;
}

} // namespace lqvm