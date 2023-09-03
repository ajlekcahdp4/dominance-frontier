// ----------------------------------------------------------------------------
//   "THE BEER-WARE LICENSE" (Revision 42):
//   <alex.rom23@mail.ru> wrote this file. As long as you retain this
//   notice you can do whatever you want with this stuff. If we meet some day,
//   and you think this stuff is worth it, you can buy me a beer in return.
// ----------------------------------------------------------------------------

#include "Dominators.h"

#include <llvm/ADT/BreadthFirstIterator.h>
#include <llvm/ADT/PostOrderIterator.h>
#include <llvm/ADT/STLExtras.h>

#include <ranges>
#include <string_view>

namespace lqvm {
size_t ComputeHash(const NodetoDominatorsTy &M) {
  size_t Hash = 0;
  for (auto &&[NodePtr, Doms] : M) {
    for (auto *Dom : Doms)
      Hash += std::hash<const Node *>()(Dom);
  }
  return Hash;
}

void ComputeDominatorsIteration(const GraphTy<Node> &G, NodetoDominatorsTy &M) {
  std::vector<const Node *> AllNodesSet;
  for (auto &&Nd : G)
    AllNodesSet.push_back(&Nd);
  for (auto &&Nd : G) {
    std::vector<const Node *> Doms = AllNodesSet;
    for (auto *Parent : Nd.Parents)
      if (M.contains(Parent))
        Doms = utils::OrderedIntersection(Doms, M.at(Parent));
    Doms.push_back(&Nd);
    if (Nd.Val != G.getEntryNodeVal())
      M[&Nd] = std::move(Doms);
  }
}

NodetoDominatorsTy ComputeDominators(const GraphTy<Node> &G) {
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

std::map<const Node *, const Node *> ComputeIDom(const GraphTy<Node> &G) {
  std::map<const Node *, const Node *> IDom;
  for (auto &&Nd : G)
    IDom[&Nd] = nullptr;
  IDom[&G.front()] = &G.front();
  bool Changed = true;
  while (Changed) {
    Changed = false;
		auto PO = postOrder(G);
		auto RPO = std::views::reverse(PO);
    auto PickParent = [RPO](const auto *Nd) -> const Node * {
      for (const auto *Parent : Nd->Parents) {
        if (utils::GetIndexIn(Parent, RPO) < utils::GetIndexIn(Nd, RPO))
          return Parent;
      }
      return nullptr;
    };
    for (auto *CNode : utils::drop_begin(RPO)) {
      const auto *NewIdom = PickParent(CNode);
      for (auto *Parent : CNode->Parents) {
        if (Parent != NewIdom && IDom[Parent]) {
          NewIdom = IntersectNodes(Parent, NewIdom, PO, IDom);
        }
      }
      if (IDom[CNode] != NewIdom) {
        IDom[CNode] = NewIdom;
        Changed = true;
      }
    }
  }
  IDom[&G.front()] = nullptr;
  return IDom;
}

GraphTy<Node> BuildDomTree(const GraphTy<Node> &G) {
  auto IDom = ComputeIDom(G);
  GraphTy<Node> T;
  T.reserve(G.size());
  for (auto [Nd, Dom] : IDom) {
    auto IsCurNode = [Nd](const auto &Pair) { return Pair.second == Nd; };
    auto *TreeNode = T.getOrInsertNode(Nd->Val);
    auto Found = std::ranges::find_if(IDom, IsCurNode);
    while (Found != IDom.end()) {
      if (Found->first->Val != G.getEntryNodeVal())
        TreeNode->push_back(T.getOrInsertNode(Found->first->Val));
      Found = std::find_if(std::next(Found), IDom.end(), IsCurNode);
    }
  }
  return T;
}

GraphTy<DJNode> ComputeDJ(const GraphTy<Node> &G) {
  auto IDom = ComputeIDom(G);
  GraphTy<DJNode> DJ;
  DJ.reserve(G.size());
  for (auto &&Nd : G)
    DJ.emplace_back(Nd.Val);
  for (auto [Nd, Dom] : IDom)
    if (Nd->Val != G.getEntryNodeVal())
      DJ.getOrInsertNode(Dom->Val)->adoptChild(DJ.getOrInsertNode(Nd->Val));
  assert(DJ.size() == G.size());
  for (auto &Nd : G) {
    if (Nd.Parents.size() > 1 && Nd.Val != G.getEntryNodeVal())
      for (auto *Parent : Nd.Parents) {
        auto &Vec = *DJ.getOrInsertNode(Parent->Val);
        if (std::ranges::find_if(Vec, [&Nd, &DJ](const auto &Pair) {
              return Pair.first == DJ.getOrInsertNode(Nd.Val);
            }) == Vec.end())
          DJ.getOrInsertNode(Parent->Val)
              ->addBastard(DJ.getOrInsertNode(Nd.Val));
      }
  }
  return DJ;
}

GraphTy<Node> BuildDF(const GraphTy<Node> &G) {
  auto DJ = ComputeDJ(G);
  GraphTy<Node> DF;
  for (auto &&Nd : G)
    DF.getOrInsertNode(Nd.Val);
  auto HasBastard = [](const DJNode &Nd) {
    return std::any_of(Nd.begin(), Nd.end(),
                       [](const auto &Pair) { return !Pair.second; });
  };
  auto BastardOwners = DJ | std::views::filter(HasBastard);
  for (auto &&BO : BastardOwners) {
    for (auto [Child, IsTrueBorn] : BO) {
      if (!IsTrueBorn) {
        auto NCAPath = findPathToNCA(&BO, Child);
        for (const auto *Nd : utils::drop_end(NCAPath)) {
          // check duplicate
          auto *DFNd = DF.getOrInsertNode(Nd->Val);
          auto *DFChild = DF.getOrInsertNode(Child->Val);
          if (!utils::is_contained(*DFNd, DFChild))
            DFNd->adoptChild(DFChild);
        }
      }
    }
  }
  return DF;
}

GraphTy<Node> BuildIDF(const GraphTy<Node> &G) {
  auto IDF = BuildDF(G);
  for (auto &&CurrNode : IDF) {
    auto BFS = llvm::breadth_first(&CurrNode);
    for (auto DesIt = std::next(BFS.begin()), End = BFS.end(); DesIt != End;
         ++DesIt) {
      if (!utils::is_contained(CurrNode, *DesIt))
        CurrNode.adoptChild(*DesIt);
    }
  }
  return IDF;
}

} // namespace lqvm
