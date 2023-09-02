// ----------------------------------------------------------------------------
//   "THE BEER-WARE LICENSE" (Revision 42):
//   <alex.rom23@mail.ru> wrote this file. As long as you retain this
//   notice you can do whatever you want with this stuff. If we meet some day,
//   and you think this stuff is worth it, you can buy me a beer in return.
// ----------------------------------------------------------------------------
#include "ReducibleGraph.h"

#include <llvm/Support/CommandLine.h>

#include <iostream>

namespace lqvm {
using namespace llvm;
cl::OptionCategory Options("domfront options");
static cl::opt<unsigned long long> Seed("seed", cl::desc("Random seed"),
                                        cl::cat(Options), cl::init(0));
} // namespace lqvm

using namespace lqvm;
int main(int Argc, char **Argv) {
  cl::ParseCommandLineOptions(Argc, Argv, "");
  ReducibleGraphBuilder GB(100, Seed);
  auto G = GB.generate();
  G.dumpDot(std::cout);
}