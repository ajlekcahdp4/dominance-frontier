// ----------------------------------------------------------------------------
//   "THE BEER-WARE LICENSE" (Revision 42):
//   <alex.rom23@mail.ru> wrote this file. As long as you retain this
//   notice you can do whatever you want with this stuff. If we meet some day,
//   and you think this stuff is worth it, you can buy me a beer in return.
// ----------------------------------------------------------------------------
#pragma once

#include "Utils.h"

#include <algorithm>
#include <cassert>
#include <functional>
#include <ostream>
#include <queue>
#include <random>
#include <ranges>
#include <set>
#include <string_view>
#include <unordered_map>

// LQVM - Low Quality Virtual Machine
namespace lqvm {

struct Node final : private std::vector<Node *> {
  std::set<Node *> Parents;
  using ValueTy = unsigned;
  ValueTy Val;
  Node(ValueTy Value) : vector(), Val(Value) {}
  Node() : Val{} {}

  using vector::back;
  using vector::begin;
  using vector::cbegin;
  using vector::cend;
  using vector::end;
  using vector::front;
  using vector::push_back;
  using vector::operator[];
  using vector::erase;
  using vector::size;

  using typename vector::const_iterator;
  using typename vector::iterator;
  using typename vector::value_type;

  void adoptChild(Node *Child) {
    assert(std::ranges::find(*this, Child) == end() &&
           "Attempt to duplicate child.");
    push_back(Child);
    Child->addParent(this);
  }

  void addParent(Node *Parent) { Parents.insert(Parent); }

  void abandonChild(Node *Child) {
    assert(std::ranges::find(*this, Child) != end() &&
           "Attempt to remove non-existent child.");
    erase(std::remove(begin(), end(), Child), end());
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

template <typename NodeTy> struct GraphTy final : private std::vector<NodeTy> {
private:
  using BaseTy = std::vector<NodeTy>;

public:
  using BaseTy::back;
  using BaseTy::begin;
  using BaseTy::cbegin;
  using BaseTy::cend;
  using BaseTy::emplace_back;
  using BaseTy::end;
  using BaseTy::front;
  using BaseTy::push_back;
  using BaseTy::operator[];
  using BaseTy::erase;
  using BaseTy::reserve;
  using BaseTy::size;

  using typename BaseTy::const_iterator;
  using typename BaseTy::iterator;
  using typename BaseTy::value_type;

  using ValueTy = typename NodeTy::ValueTy;

  GraphTy() = default;
  GraphTy(const GraphTy &) = delete;
  GraphTy &operator=(const GraphTy &) = delete;
  GraphTy(GraphTy &&) = default;
  GraphTy &operator=(GraphTy &&) = default;

  NodeTy *getOrInsertNode(ValueTy Val) {
    auto Found = std::ranges::find_if(
        *this, [Val](const NodeTy &Node) { return Node.Val == Val; });
    if (Found == end()) {
      BaseTy::emplace_back(Val);
      return &back();
    }
    return std::addressof(*Found);
  }

  unsigned getIndex(ValueTy Val) const {
    auto Found = std::ranges::find_if(
        *this, [Val](const NodeTy &Nd) { return Nd.Val == Val; });
    assert(Found != end() &&
           "Cannot found vertex for which index for requested.");
    return std::distance(begin(), Found);
  }

  ValueTy getEntryNodeVal() const { return 0; }

  void dumpDot(std::ostream &OS, std::string_view Title) const;
};

template <typename NodeTy>
void GraphTy<NodeTy>::dumpDot(std::ostream &OS, std::string_view Title) const {
  OS << "digraph cluster_1 {\n";
  OS << "label=\"" << Title << "\";\n";
  auto DeclareNodesAndEdges = [&OS](const NodeTy &Nd) {
    Nd.dumpSelf(OS);
    Nd.dumpChildrenEdges(OS);
  };
  std::for_each(begin(), end(), DeclareNodesAndEdges);
  OS << "\t}\n";
}

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
        for (auto &&Child : *Nd) {
          New->adoptChild(Child);
          Nd->abandonChild(Child);
        }
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
        for (auto &&Child : *Nd) {
          New->adoptChild(Child);
          Nd->abandonChild(Child);
        }
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
    if (Nd->size() < 2 && std::ranges::find(*Nd, Nd) == Nd->end())
      Nd->adoptChild(Nd);
  }
  Node *insertNode() {
    Graph.emplace_back(Graph.size());
    return &Graph.back();
  }

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
      if (utils::is_contained(Nd, &Nd))
        Nd.abandonChild(&Nd);
    }
  }

  GraphTy<Node> generate() {
    generateImpl();
    return std::move(Graph);
  }
};

template <typename NodeTy>
class PostOrder final : private std::vector<const NodeTy *> {
  using BaseTy = std::vector<const NodeTy *>;

public:
  explicit PostOrder(const GraphTy<NodeTy> &G) {
    enum class ColorTy { E_WHITE, E_GRAY, E_BLACK };
    std::unordered_map<const NodeTy *, ColorTy> Nodes;
    std::ranges::transform(
        G, std::inserter(Nodes, Nodes.begin()),
        [](const auto &Nd) { return std::make_pair(&Nd, ColorTy::E_WHITE); });
    std::function<void(const NodeTy *)> DFSVisit;
    DFSVisit = [&Nodes, &G, this, &DFSVisit](const NodeTy *Start) {
      Nodes.at(Start) = ColorTy::E_GRAY;
      for (const auto *Child : *Start) {
        if (Nodes.at(Child) == ColorTy::E_WHITE)
          DFSVisit(Child);
      }
      Nodes.at(Start) = ColorTy::E_BLACK;
      BaseTy::push_back(Start);
    };
    DFSVisit(&G.front());
  }

  auto begin() const { return BaseTy::cbegin(); }

  auto end() const { return BaseTy::cend(); }
};

template <typename NodeTy>
class BreadthFirst final : private std::vector<NodeTy *> {
  using BaseTy = std::vector<NodeTy *>;

public:
  explicit BreadthFirst(NodeTy *Start) {
    enum class ColorTy { E_WHITE, E_GRAY, E_BLACK };
    std::unordered_map<unsigned, ColorTy> Nodes;
    Nodes[Start->Val] = ColorTy::E_GRAY;
    std::queue<NodeTy *> Q;
    Q.push(Start);
    while (!Q.empty()) {
      auto *Curr = Q.front();
      BaseTy::push_back(Curr);
      Q.pop();
      Nodes.emplace(Curr->Val, ColorTy::E_WHITE);
      for (auto *Child : *Curr) {
        auto &ChildColor =
            Nodes.emplace(Child->Val, ColorTy::E_WHITE).first->second;
        if (ChildColor == ColorTy::E_WHITE) {
          ChildColor = ColorTy::E_GRAY;
          Q.push(Child);
        }
      }
      Nodes.at(Curr->Val) = ColorTy::E_BLACK;
    }
  }

  auto begin() const { return BaseTy::cbegin(); }

  auto end() const { return BaseTy::cend(); }
};

} // namespace lqvm
