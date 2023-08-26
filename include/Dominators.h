// ----------------------------------------------------------------------------
//   "THE BEER-WARE LICENSE" (Revision 42):
//   <alex.rom23@mail.ru> wrote this file. As long as you retain this
//   notice you can do whatever you want with this stuff. If we meet some day,
//   and you think this stuff is worth it, you can buy me a beer in return.
// ----------------------------------------------------------------------------
#pragma once

#include "ReducibleGraph.h"
#include "Utils.h"

#include <map>
#include <string_view>
#include <vector>
namespace lqvm {
using NodetoDominatorsTy = std::map<const Node *, std::vector<const Node *>>;
NodetoDominatorsTy ComputeDominators(const GraphTy<Node> &G);

std::map<const Node *, const Node *> ComputeIDom(const GraphTy<Node> &G);

GraphTy<Node> BuildDomTree(const GraphTy<Node> &G);

struct DJNode final
    : std::vector<std::pair<DJNode *, bool>> { // bool = true for trueborn
                                               // children
  std::set<DJNode *> Parents;
  using ValueTy = unsigned;
  ValueTy Val;
  DJNode(ValueTy Value) : vector(), Val(Value) {}

  bool isBastardOf(const DJNode *Parent) const {
    auto Found = llvm::find_if(
        *Parent, [this](const auto &Pair) { return Pair.first == this; });
    if (Found == Parent->end())
      return false;
    return !Found->second;
  }

  void addBastard(DJNode *Child) {
    assert(std::find_if(begin(), end(),
                        [Child](const auto &Pair) {
                          return Pair.first == Child;
                        }) == end() &&
           "Attempt to duplicate bastard.");
    emplace_back(Child, false);
  }

  void adoptChild(DJNode *Child) {
    assert(std::find_if(begin(), end(),
                        [Child](const auto &Pair) {
                          return Pair.first == Child;
                        }) == end() &&
           "Attempt to duplicate child.");
    emplace_back(Child, true);
    Child->addParent(this);
  }

  void dumpSelf(std::ostream &OS) const {
    OS << "vert_" << Val;
    OS << "[shape=square, label=\"" << Val << "\"];\n";
  }

  void dumpChildrenEdges(std::ostream &OS) const {
    for (const auto &Child : *this) {
      std::string_view Style = Child.second ? "solid" : "dotted";
      OS << "vert_" << Val << " -> vert_" << Child.first->Val << " [style=\""
         << Style
         << "\"]"
            ";\n";
    }
  }

  void addParent(DJNode *Parent) { Parents.insert(Parent); }
};

template <typename NodeTy>
std::vector<const NodeTy *> pathUp(const NodeTy *From) {
  std::vector<const NodeTy *> Path;
  const auto *CurrNode = From;
  Path.push_back(CurrNode);
  while (true) {
    auto Found = llvm::find_if(CurrNode->Parents, [CurrNode](const auto *P) {
      return !CurrNode->isBastardOf(P); // A.K.A. trueborn
    });
    if (Found == CurrNode->Parents.end())
      break;
    CurrNode = *Found;
    Path.push_back(CurrNode);
  }
  return Path;
}

template <typename NodeTy>
std::vector<const NodeTy *> findPathToNCA(const NodeTy *From,
                                          const NodeTy *To) {
  auto FromPath = pathUp(From);
  auto ToPath = pathUp(To);
  auto MatchIt = utils::first_match(FromPath, ToPath);
  assert(MatchIt != FromPath.end());
  std::vector<const NodeTy *> Res;
  std::copy(FromPath.cbegin(), std::next(MatchIt), std::back_inserter(Res));
  return Res;
}

GraphTy<DJNode> ComputeDJ(const GraphTy<Node> &G);

GraphTy<Node> BuildDF(const GraphTy<Node> &G);

GraphTy<Node> BuildIDF(const GraphTy<Node> &G);
} // namespace lqvm