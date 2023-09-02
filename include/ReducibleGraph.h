// ----------------------------------------------------------------------------
//   "THE BEER-WARE LICENSE" (Revision 42):
//   <alex.rom23@mail.ru> wrote this file. As long as you retain this
//   notice you can do whatever you want with this stuff. If we meet some day,
//   and you think this stuff is worth it, you can buy me a beer in return.
// ----------------------------------------------------------------------------
#pragma once

#include <llvm/ADT/GraphTraits.h>
#include <llvm/ADT/STLExtras.h>

#include <algorithm>
#include <cassert>
#include <ostream>
#include <random>
#include <set>

// LQVM - Low Quality Virtual Machine
namespace lqvm {

struct Node final : std::vector<Node *> {
  std::set<Node *> Parents;
  using ValueTy = unsigned;
  ValueTy Val;
  Node(ValueTy Value) : vector(), Val(Value) {}

  void adoptChild(Node *Child) {
    assert(std::find(begin(), end(), Child) == end() &&
           "Attempt to duplicate child.");
    push_back(Child);
    Child->addParent(this);
  }

  void addParent(Node *Parent) { Parents.insert(Parent); }

  void abandonChild(Node *Child) {
    assert(std::find(begin(), end(), Child) != end() &&
           "Attempt to remove non-existent child.");
    llvm::erase_value(*this, Child);
    Child->removeParent(this);
  }

  void dumpSelf(std::ostream &OS) const {
    OS << "vert_" << Val;
    OS << "[shape=square, label=\"" << Val << "\"];\n";
  }

  void dumpChildrenEdges(std::ostream &OS) const {
    for (auto *Child : *this)
      OS << "vert_" << Val << " -> "
         << "vert_" << Child->Val << ";\n";
  }

  void removeParent(Node *Parent) { Parents.erase(Parent); }
};

template <typename NodeTy> struct GraphTy final : public std::vector<NodeTy> {
private:
  using BaseTy = std::vector<NodeTy>;

public:
  using BaseTy::back;
  using BaseTy::begin;
  using BaseTy::end;
  using BaseTy::front;
  using BaseTy::operator[];
  void dumpDot(std::ostream &OS) const;

  NodeTy *GetOrInsertNode(typename NodeTy::ValueTy Val) {
    auto Found = llvm::find_if(
        *this, [Val](const NodeTy &Node) { return Node.Val == Val; });
    if (Found == end()) {
      BaseTy::emplace_back(Val);
      return &back();
    }
    return std::addressof(*Found);
  }
};

class ReducibleGraphBuilder final {
  GraphTy<Node> Graph;
  unsigned MaxSz;
  std::mt19937 RandEngine;

  unsigned getUniformRandom(unsigned Lower, unsigned Upper) {
    std::uniform_int_distribution<decltype(RandEngine)::result_type> Dist(
        Lower, Upper);
    return Dist(RandEngine);
  }

  Node *addNode(Node *Nd) {
    auto *New = insertNode();
    auto Size = Nd->size();
    switch (Size) {
    case 0: {
      Nd->adoptChild(New);
      break;
    }
    case 1: {
      switch (getUniformRandom(0, 2)) {
      case 0: {
        auto *Old = Nd->back();
        Nd->abandonChild(Old);
        Nd->adoptChild(New);
        New->adoptChild(Old);
        break;
      }
      case 1: {
        New->adoptChild(Nd->back());
        Nd->adoptChild(New);
        break;
      }
      case 2: {
        Nd->adoptChild(New);
        break;
      }
      };
      break;
    }
    case 2: {
      switch (getUniformRandom(0, 2)) {
      case 0: {
        llvm::for_each(*Nd, [New](Node *Child) { New->adoptChild(Child); });
        llvm::for_each(*Nd, [Nd](Node *Child) { Nd->abandonChild(Child); });
        Nd->adoptChild(New);
        break;
      }
      case 1: {
        auto *Old = Nd->back();
        Nd->abandonChild(Old);
        Nd->adoptChild(New);
        New->adoptChild(Old);
        break;
      }
      case 2: {

        llvm::for_each(*Nd, [New](Node *Child) { New->adoptChild(Child); });
        llvm::for_each(*Nd, [Nd](Node *Child) { Nd->abandonChild(Child); });
        Nd->adoptChild(New);
        Nd->adoptChild(New->back());
        break;
      }
      }
      break;
    }
    default:
      assert(0 && "Node can't have more than 2 successors.");
    };
    return New;
  }

public:
  ReducibleGraphBuilder(unsigned Sz, unsigned long long Seed)
      : MaxSz(Sz), RandEngine(Seed) {
    Graph.reserve(MaxSz);
  }

  void addSelfLoop(Node *Nd) {
    if (Nd->size() < 2 && std::find(Nd->begin(), Nd->end(), Nd) == Nd->end())
      Nd->adoptChild(Nd);
  }
  Node *insertNode() {
    Graph.emplace_back(Graph.size());
    return &Graph.back();
  }

  void dumpDot(std::ostream &OS) const;

  void generateImpl() {
    insertNode();
    for (unsigned I = 0; I < MaxSz; ++I) {
      auto N = getUniformRandom(0, Graph.size() - 1);
      auto Op = getUniformRandom(0, 1);
      switch (Op) {
      case 0:
        addNode(&Graph[N]);
        break;
      case 1:
        addSelfLoop(&Graph[N]);
        break;
      };
    }
    for (auto &&Nd : Graph) {
      if (llvm::is_contained(Nd, &Nd))
        Nd.abandonChild(&Nd);
    }
  }

  GraphTy<Node> generate() {
    generateImpl();
    return std::move(Graph);
  }
};

template <typename NodeTy>
void GraphTy<NodeTy>::dumpDot(std::ostream &OS) const {
  OS << "digraph cluster_1 {\n";
  auto DeclareNodesAndEdges = [&OS](const NodeTy &Nd) {
    Nd.dumpSelf(OS);
    Nd.dumpChildrenEdges(OS);
  };
  std::for_each(begin(), end(), DeclareNodesAndEdges);
  OS << "\t}\n";
}

} // namespace lqvm

namespace llvm {

template <> class GraphTraits<const lqvm::Node *> {
public:
  using NodeRef = const lqvm::Node *;
  using ChildIteratorType = lqvm::Node::const_iterator;
  static NodeRef getEntryNode(const lqvm::Node *N) { return N; }
  static ChildIteratorType child_begin(NodeRef N) { return N->cbegin(); }
  static ChildIteratorType child_end(NodeRef N) { return N->cend(); }
};
} // namespace llvm
