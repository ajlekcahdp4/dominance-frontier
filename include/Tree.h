// ----------------------------------------------------------------------------
//   "THE BEER-WARE LICENSE" (Revision 42):
//   <alex.rom23@mail.ru> wrote this file. As long as you retain this
//   notice you can do whatever you want with this stuff. If we meet some day,
//   and you think this stuff is worth it, you can buy me a beer in return.
// ----------------------------------------------------------------------------
#pragma once

#include <llvm/ADT/STLExtras.h>
#include <vector>

namespace lqvm {
struct TreeNode {
  using ValueTy = unsigned;
  ValueTy Val = 0;
  std::vector<TreeNode *> Children;
  TreeNode(ValueTy Value) : Val(Value) {}
};

struct Tree {
  std::vector<TreeNode> Nodes;

  Tree(unsigned MaxSz) { Nodes.reserve(MaxSz); }

  TreeNode *GetOrInsertNode(TreeNode::ValueTy Val) {
    auto Found = llvm::find_if(
        Nodes, [Val](const TreeNode &Node) { return Node.Val == Val; });
    if (Found == Nodes.end()) {
      Nodes.emplace_back(Val);
      return &Nodes.back();
    }
    return std::addressof(*Found);
  }
};
} // namespace lqvm