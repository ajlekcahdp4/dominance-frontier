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
static cl::opt<unsigned long long>
    GenIterations("gen-iterations",
                  cl::desc("Iteration number for CFG generation"),
                  cl::cat(Options), cl::init(10));
static cl::opt<bool> DumpCFG("dump-cfg", cl::desc("Dump CFG"), cl::cat(Options),
                             cl::init(false));
static cl::opt<bool> PrintDominators("print-dominators",
                                     cl::desc("Print Dominators"),
                                     cl::cat(Options), cl::init(false));

static void Print(const NodetoDominatorsTy &Dominators) {
  for (auto &&[NodePtr, Doms] : Dominators) {
    std::cout << NodePtr->Val << ": ";
    if (Doms) {
      for (auto *Dom : Doms.value())
        std::cout << Dom->Val << ", ";
      std::cout << std::endl;
    }
  }
}
} // namespace lqvm

using namespace lqvm;
int main(int Argc, char **Argv) {
  cl::ParseCommandLineOptions(Argc, Argv, "");
  ReducibleGraphBuilder GB(10, Seed);
  auto G = GB.generate();
  if (DumpCFG)
    G.dumpDot(std::cout);
  if (PrintDominators)
    Print(ComputeDominators(G));
}