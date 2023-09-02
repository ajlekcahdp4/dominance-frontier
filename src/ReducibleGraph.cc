// ----------------------------------------------------------------------------
//   "THE BEER-WARE LICENSE" (Revision 42):
//   <alex.rom23@mail.ru> wrote this file. As long as you retain this
//   notice you can do whatever you want with this stuff. If we meet some day,
//   and you think this stuff is worth it, you can buy me a beer in return.
// ----------------------------------------------------------------------------

#include "ReducibleGraph.h"
#include <algorithm>
namespace lqvm {

void GraphTy::dumpDot(std::ostream &OS) const {
  OS << "digraph cluster_1 {\n";
  auto DeclareNodesAndEdges = [&OS](const Node &Nd) {
    OS << "vert_" << Nd.Val;
    OS << "\t\t"
       << "[shape=circle, label=\"" << Nd.Val << "\"];\n";
    for (auto *Child : Nd) {
      OS << "vert_" << Nd.Val << " -> "
         << "vert_" << Child->Val << ";\n";
    }
  };
  std::for_each(begin(), end(), DeclareNodesAndEdges);
  OS << "\t}\n";
}
} // namespace lqvm