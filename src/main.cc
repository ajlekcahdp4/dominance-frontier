// ----------------------------------------------------------------------------
//   "THE BEER-WARE LICENSE" (Revision 42):
//   <alex.rom23@mail.ru> wrote this file. As long as you retain this
//   notice you can do whatever you want with this stuff. If we meet some day,
//   and you think this stuff is worth it, you can buy me a beer in return.
// ----------------------------------------------------------------------------
#include "Dominators.h"
#include "ReducibleGraph.h"

#include <llvm/Support/CommandLine.h>

#include <fstream>
#include <iostream>
#include <string>

namespace lqvm {
using namespace llvm;
cl::OptionCategory Options("domfront options");
static cl::opt<unsigned long long> Seed("seed", cl::desc("Random seed"),
                                        cl::cat(Options), cl::init(0));
static cl::opt<unsigned long long>
    GenIterations("gen-iterations",
                  cl::desc("Iteration number for CFG generation"),
                  cl::cat(Options), cl::init(10));
static cl::opt<std::string> DumpCFG("dump-cfg",
                                    cl::desc("Dump CFG to dot file"),
                                    cl::value_desc("filename"),
                                    cl::cat(Options));
static cl::opt<bool> PrintDominatorsOpt("print-dominators",
                                        cl::desc("Print Dominators"),
                                        cl::cat(Options), cl::init(false));

static cl::opt<bool> PrintIDomOpt("print-idom",
                                  cl::desc("Print Immediate Dominators"),
                                  cl::cat(Options), cl::init(false));
static cl::opt<std::string>
    DumpDomTreeOpt("dump-dom-tree", cl::desc("Dump Dom Tree to dot file"),
                   cl::value_desc("filename"), cl::cat(Options));
static cl::opt<std::string> DumpDJOpt("dump-dj",
                                      cl::desc("Dump DJ graph to dot file"),
                                      cl::value_desc("filename"),
                                      cl::cat(Options));
static cl::opt<std::string> DumpDFOpt("dump-df",
                                      cl::desc("Dump DF graph to dot file"),
                                      cl::value_desc("filename"),
                                      cl::cat(Options));
static cl::opt<std::string> DumpIDFOpt("dump-idf",
                                       cl::desc("Dump IDF graph to dot file"),
                                       cl::value_desc("filename"),
                                       cl::cat(Options));

void PrintDominators(const NodetoDominatorsTy &Dominators, std::ostream &OS) {
  for (auto &&[NodePtr, Doms] : Dominators) {
    OS << NodePtr->Val << ": ";
    for (auto *Dom : Doms) {
      OS << Dom->Val;
      if (Dom != *std::prev(Doms.end()))
        OS << ", ";
    }
    OS << std::endl;
  }
}

void PrintIDom(const std::map<const Node *, const Node *> M, std::ostream &OS) {
  for (auto [Nd, Dom] : M)
    OS << Nd->Val << ": " << Dom->Val << "\n";
}
} // namespace lqvm

using namespace lqvm;
int main(int Argc, char **Argv) {
  cl::ParseCommandLineOptions(Argc, Argv, "");
  ReducibleGraphBuilder GB(GenIterations, Seed);
  auto G = GB.generate();
  if (DumpCFG.getNumOccurrences()) {
    std::ofstream DotFile(DumpCFG);
    if (DotFile.is_open())
      G.dumpDot(DotFile, "Control Flow Graph");
    else {
      std::cerr << "Unable to open file";
      return EXIT_FAILURE;
    }
  }
  if (PrintDominatorsOpt)
    PrintDominators(ComputeDominators(G), std::cout);
  if (PrintIDomOpt)
    PrintIDom(ComputeIDom(G), std::cout);
  if (DumpDomTreeOpt.getNumOccurrences()) {
    std::ofstream DotFile(DumpDomTreeOpt);
    if (DotFile.is_open())
      DumpDomTree(BuildDomTree(G), DotFile);
    else {
      std::cerr << "Unable to open file";
      return EXIT_FAILURE;
    }
  }
  if (DumpDJOpt.getNumOccurrences()) {
    std::ofstream DotFile(DumpDJOpt);
    if (DotFile.is_open())
      DumpDJ(ComputeDJ(G), DotFile);
    else {
      std::cerr << "Unable to open file";
      return EXIT_FAILURE;
    }
  }
  if (DumpDFOpt.getNumOccurrences()) {
    std::ofstream DotFile(DumpDFOpt);
    if (DotFile.is_open()) {
      auto DF = BuildDF(G);
      DF.dumpDot(DotFile, "Dominance Frontier");
    } else {
      std::cerr << "Unable to open file";
      return EXIT_FAILURE;
    }
  }
  if (DumpIDFOpt.getNumOccurrences()) {
    std::ofstream DotFile(DumpIDFOpt);
    if (DotFile.is_open()) {
      auto IDF = BuildIDF(G);
      IDF.dumpDot(DotFile, "Iterated Dominance Frontier");
    } else {
      std::cerr << "Unable to open file";
      return EXIT_FAILURE;
    }
  }
}