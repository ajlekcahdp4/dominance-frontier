// ----------------------------------------------------------------------------
//   "THE BEER-WARE LICENSE" (Revision 42):
//   <alex.rom23@mail.ru> wrote this file. As long as you retain this
//   notice you can do whatever you want with this stuff. If we meet some day,
//   and you think this stuff is worth it, you can buy me a beer in return.
// ----------------------------------------------------------------------------

#include "ReducibleGraph.h"

namespace lqvm {

void ReducibleGraphBuilder::dumpDot(std::ostream &OS) const {

  OS << "digraph cluster_1 {\n";

  auto DeclareNodesAndEdges = [&OS](const GraphTy::node_type &Nd) {
    OS << "vert_" << Nd.value.key;
    OS << "\t\t"
       << "[shape=circle, label=\"" << Nd.value.key << "\"];\n";
    for (auto &&Child : Nd) {
      OS << "vert_" << Nd.value.key << " -> "
         << "vert_" << Child.key << ";\n";
    }
  };
  graphs::breadth_first_search(Graph, 0, DeclareNodesAndEdges);
  OS << "\t}\n";
}
} // namespace lqvm