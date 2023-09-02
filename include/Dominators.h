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

  void addParent(DJNode *Parent) { Parents.insert(Parent); }
};

GraphTy<DJNode> ComputeDJ(const GraphTy<Node> &G);

void DumpDJ(const GraphTy<DJNode> &DJ, std::ostream &OS);

void DumpDomTree(const GraphTy<Node> &DomTree, std::ostream &OS);
} // namespace lqvm