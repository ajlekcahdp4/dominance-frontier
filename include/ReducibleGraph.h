// ----------------------------------------------------------------------------
//   "THE BEER-WARE LICENSE" (Revision 42):
//   <alex.rom23@mail.ru> wrote this file. As long as you retain this
//   notice you can do whatever you want with this stuff. If we meet some day,
//   and you think this stuff is worth it, you can buy me a beer in return.
// ----------------------------------------------------------------------------
#pragma once

#include <algorithm>
#include <cassert>
#include <ostream>
#include <random>

// LQVM - Low Quality Virtual Machine
namespace lqvm {

struct Node final : std::vector<Node *> {
  unsigned Val;
  Node(unsigned Val) : vector(), Val(Val) {}
};
struct GraphTy : public std::vector<Node> {
  void dumpDot(std::ostream &OS) const;
};

class ReducibleGraphBuilder final {
  GraphTy Graph;
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
      Nd->push_back(New);
      break;
    }
    case 1: {
      switch (getUniformRandom(0, 2)) {
      case 0: {
        auto *Old = Nd->back();
        Nd->pop_back();
        Nd->push_back(New);
        New->push_back(Old);
        break;
      }
      case 1: {
        New->push_back(Nd->back());
        Nd->push_back(New);
        break;
      }
      case 2: {
        Nd->push_back(New);
        break;
      }
      };
      break;
    }
    case 2: {
      switch (getUniformRandom(0, 2)) {
      case 0: {
        std::copy(Nd->begin(), Nd->end(), std::back_inserter(*New));
        Nd->erase(Nd->begin(), Nd->end());
        Nd->push_back(New);
        break;
      }
      case 1: {
        auto *Old = Nd->back();
        Nd->pop_back();
        Nd->push_back(New);
        New->push_back(Old);
        break;
      }
      case 2: {
        std::copy(Nd->begin(), Nd->end(), std::back_inserter(*New));
        Nd->erase(Nd->begin(), Nd->end());
        Nd->push_back(New);
        Nd->push_back(New->back());
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
      Nd->push_back(Nd);
  }
  Node *insertNode() {
    Graph.emplace_back(Graph.size());
    return &Graph.back();
  }

  void dumpDot(std::ostream &OS) const;

  void generateImpl() {
    insertNode();
    for (int I = 0; I < MaxSz; ++I) {
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
    std::for_each(Graph.begin(), Graph.end(), [](Node &Nd) {
      Nd.erase(std::remove(Nd.begin(), Nd.end(), &Nd), Nd.end());
    });
  }

  GraphTy generate() {
    generateImpl();
    return std::move(Graph);
  }
};
} // namespace lqvm