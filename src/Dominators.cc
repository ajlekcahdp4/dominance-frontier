// ----------------------------------------------------------------------------
//   "THE BEER-WARE LICENSE" (Revision 42):
//   <alex.rom23@mail.ru> wrote this file. As long as you retain this
//   notice you can do whatever you want with this stuff. If we meet some day,
//   and you think this stuff is worth it, you can buy me a beer in return.
// ----------------------------------------------------------------------------

#include "Dominators.h"

#include <llvm/ADT/PostOrderIterator.h>
#include <llvm/ADT/STLExtras.h>

namespace lqvm {
size_t ComputeHash(const NodetoDominatorsTy &M) {
  size_t Hash = 0;
  for (auto &&[NodePtr, Doms] : M) {
    for (auto *Dom : Doms)
      Hash += std::hash<const Node *>()(Dom);
  }
  return Hash;
}

void ComputeDominatorsIteration(const GraphTy &G, NodetoDominatorsTy &M) {
  std::vector<const Node *> AllNodesSet;
  for (auto &Nd : G)
    AllNodesSet.push_back(&Nd);
  for (auto &&Nd : G) {
    std::vector<const Node *> Doms = AllNodesSet;
    for (auto *Parent : Nd.Parents)
      if (M.contains(Parent))
        Doms = utils::OrderedIntersection(Doms, M[Parent]);
    Doms.push_back(&Nd);
    if (!Nd.Parents.empty())
      M[&Nd] = std::move(Doms);
  }
}

NodetoDominatorsTy ComputeDominators(const GraphTy &G) {
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

const Node *IntersectNodes(const Node *First, const Node *Second, auto PO,
                           const std::map<const Node *, const Node *> &IDoms) {
  auto *Finger1 = First;
  auto *Finger2 = Second;
  while (Finger1 != Finger2) {
    auto Idx1 = utils::GetIndexIn(Finger1, PO);
    auto Idx2 = utils::GetIndexIn(Finger2, PO);
    while (Idx1 < Idx2) {
      Finger1 = IDoms.at(Finger1);
      Idx1 = utils::GetIndexIn(Finger1, PO);
    }
    while (Idx2 < Idx1) {
      Finger2 = IDoms.at(Finger2);
      Idx2 = utils::GetIndexIn(Finger2, PO);
    }
  }
  return Finger1;
}

std::map<const Node *, const Node *> ComputeIDom(const GraphTy &G) {
  std::map<const Node *, const Node *> IDom;
  for (auto &&Nd : G)
    IDom[&Nd] = nullptr;
  IDom[&G.front()] = &G.front();
  bool Changed = true;
  while (Changed) {
    Changed = false;
    auto RPO = llvm::ReversePostOrderTraversal(&G.front());
    auto PickParent = [RPO](const auto *Nd) -> const Node * {
      for (const auto *Parent : Nd->Parents) {
        if (utils::GetIndexIn(Parent, RPO) < utils::GetIndexIn(Nd, RPO))
          return Parent;
      }
      return nullptr;
    };
    for (auto *CNode : llvm::drop_begin(RPO)) {
      const auto *NewIdom = PickParent(CNode);
      for (auto *Parent : CNode->Parents) {
        if (Parent != NewIdom && IDom[Parent]) {
          auto PO = llvm::post_order(&G.front());
          NewIdom = IntersectNodes(Parent, NewIdom, PO, IDom);
        }
      }
      if (IDom[CNode] != NewIdom) {
        IDom[CNode] = NewIdom;
        Changed = true;
      }
    }
  }
  return IDom;
}

} // namespace lqvm