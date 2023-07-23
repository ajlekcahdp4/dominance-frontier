// ----------------------------------------------------------------------------
//   "THE BEER-WARE LICENSE" (Revision 42):
//   <alex.rom23@mail.ru> wrote this file. As long as you retain this
//   notice you can do whatever you want with this stuff. If we meet some day,
//   and you think this stuff is worth it, you can buy me a beer in return.
// ----------------------------------------------------------------------------
#pragma once

#include "graphs/directed_graph.hpp"
#include <algorithm>

// LQVM - Low Quality Virtual Machine
namespace lqvm {

class reducible_graph_builder final {
  graphs::directed_graph graph;

  void generate_impl() {}

public:
  graphs::directed_graph generate() {
    generate_impl();
    return std::move(graph);
  }
};
} // namespace lqvm