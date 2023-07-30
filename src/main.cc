// ----------------------------------------------------------------------------
//   "THE BEER-WARE LICENSE" (Revision 42):
//   <alex.rom23@mail.ru> wrote this file. As long as you retain this
//   notice you can do whatever you want with this stuff. If we meet some day,
//   and you think this stuff is worth it, you can buy me a beer in return.
// ----------------------------------------------------------------------------
#include "Dominators.h"
#include "ReducibleGraph.h"

#include <llvm/Support/CommandLine.h>

#include <iostream>

namespace lqvm {
using namespace llvm;
cl::OptionCategory Options("domfront options");
static cl::opt<unsigned long long> Seed("seed", cl::desc("Random seed"),
                                        cl::cat(Options), cl::init(0));

static cl::opt<bool> DumpCFG("dump-cfg", cl::desc("Dump CFG"), cl::cat(Options),
                             cl::init(false));
} // namespace lqvm

using namespace lqvm;
int main(int Argc, char **Argv) {
  cl::ParseCommandLineOptions(Argc, Argv, "");
  ReducibleGraphBuilder GB(10, Seed);
  auto G = GB.generate();
  if (DumpCFG)
    G.dumpDot(std::cout);
  auto Dominators = ComputeDominators(G);
  for (auto &&[NodePtr, Doms] : Dominators) {
    std::cerr << NodePtr->Val << ": ";
    if (Doms) {
      for (auto *Dom : Doms.value())
        std::cerr << Dom->Val << ", ";
      std::cerr << std::endl;
    }
  }
}