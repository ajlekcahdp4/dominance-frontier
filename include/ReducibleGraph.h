// ----------------------------------------------------------------------------
//   "THE BEER-WARE LICENSE" (Revision 42):
//   <alex.rom23@mail.ru> wrote this file. As long as you retain this
//   notice you can do whatever you want with this stuff. If we meet some day,
//   and you think this stuff is worth it, you can buy me a beer in return.
// ----------------------------------------------------------------------------
#pragma once

#include "../graphs/include/graphs/directed_graph.hpp"
#include <ostream>

// LQVM - Low Quality Virtual Machine
namespace lqvm {

class ReducibleGraphBuilder final {
  using GraphTy = graphs::basic_directed_graph<unsigned, void, void>;
  GraphTy Graph;

  void addSelfLoop(GraphTy::value_type Nd) { Graph.insert(Nd, Nd); }

public:
  void dumpDot(std::ostream &OS) const;

  void generateImpl() {
    Graph.insert(0);
    addSelfLoop(0);
  }

  GraphTy generate() {
    generateImpl();
    return std::move(Graph);
  }
};
} // namespace lqvm