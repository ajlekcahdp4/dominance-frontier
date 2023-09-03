// ----------------------------------------------------------------------------
//   "THE BEER-WARE LICENSE" (Revision 42):
//   <alex.rom23@mail.ru> wrote this file. As long as you retain this
//   notice you can do whatever you want with this stuff. If we meet some day,
//   and you think this stuff is worth it, you can buy me a beer in return.
// ----------------------------------------------------------------------------
#include "Dominators.h"
#include "ReducibleGraph.h"
#include "driver.h"

#include <popl/popl.hpp>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace lqvm {

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

void printIDom(const std::map<const Node *, const Node *> M, std::ostream &OS) {
  for (auto [Nd, Dom] : M)
    OS << Nd->Val << ": " << (Dom ? std::to_string(Dom->Val) : "-") << "\n";
}

GraphTy<Node> parse_cfg() {
  std::noskipws(std::cin);
  std::string Input{std::istreambuf_iterator<char>{std::cin},
                    std::istreambuf_iterator<char>{}};
  Driver DRV{};
  std::istringstream ISS{Input};
  DRV.switchInputStream(&ISS);
  DRV.parse();
  return std::move(DRV.G);
}
} // namespace lqvm

using namespace lqvm;
int main(int Argc, char **Argv) {
	popl::OptionParser OP("Allowed Options");

	int Seed = 1;
	unsigned GenIterations;

	auto Help = OP.add<popl::Switch>("h", "help", "Print help message");
	OP.add<popl::Implicit<int>>("s", "seed", "Random seed", 1, &Seed);
	OP.add<popl::Implicit<unsigned>>("n", "gen-iterations", "Number of iterations for CFG generation", 10, &GenIterations);
	auto GenerateCFG = OP.add<popl::Switch>("", "generate-cfg", "Forces to generate CFG, not take it as an input");
	auto PrintIDom = OP.add<popl::Switch>("", "print-idom", "Print immediate dominators");
	auto DumpCFG = OP.add<popl::Value<std::string>>("", "dump-cfg", "Dump CFG to .dot file");
	auto DumpDomTree = OP.add<popl::Value<std::string>>("", "dump-dom-tree", "Dump dominators tree to .dot file");
	auto DumpDJ = OP.add<popl::Value<std::string>>("", "dump-dj", "Dump DJ graph to .dot file");
	auto DumpDF = OP.add<popl::Value<std::string>>("", "dump-df", "Dump dominance frontier to .dot file");
	auto DumpIDF = OP.add<popl::Value<std::string>>("", "dump-idf", "Dump iterated dominance frontier to .dot file");
	OP.parse(Argc, Argv);

	if (Help->is_set())
	{
		std::cout << OP.help()<<std::endl;
		return EXIT_SUCCESS;
	}


	GraphTy<Node> G;
  if (GenerateCFG->is_set()) {
    ReducibleGraphBuilder GB(GenIterations, Seed);
    G = GB.generate();
  } else
    G = parse_cfg();
  if (DumpCFG->is_set()) {
    std::ofstream DotFile(DumpCFG->value());
    if (DotFile.is_open())
      G.dumpDot(DotFile, "Control Flow Graph");
    else {
      std::cerr << "Unable to open file";
      return EXIT_FAILURE;
    }
  }
#if 0	
  if (PrintDominatorsOpt)
    PrintDominators(ComputeDominators(G), std::cout);
#endif
  if (PrintIDom->is_set())
    printIDom(ComputeIDom(G), std::cout);
  if (DumpDomTree->is_set()) {
    std::ofstream DotFile(DumpDomTree->value());
    if (DotFile.is_open()) {
      auto DomTree = BuildDomTree(G);
      DomTree.dumpDot(DotFile, "Dom Tree");
    }

    else {
      std::cerr << "Unable to open file";
      return EXIT_FAILURE;
    }
  }
  if (DumpDJ->is_set()) {
    std::ofstream DotFile(DumpDJ->value());
    if (DotFile.is_open()) {
      auto DJ = ComputeDJ(G);
      DJ.dumpDot(DotFile, "DJ graph");
    } else {
      std::cerr << "Unable to open file";
      return EXIT_FAILURE;
    }
  }
  if (DumpDF->is_set()) {
    std::ofstream DotFile(DumpDF->value());
    if (DotFile.is_open()) {
      auto DF = BuildDF(G);
      DF.dumpDot(DotFile, "Dominance Frontier");
    } else {
      std::cerr << "Unable to open file";
      return EXIT_FAILURE;
    }
  }
  if (DumpIDF->is_set()) {
    std::ofstream DotFile(DumpIDF->value());
    if (DotFile.is_open()) {
      auto IDF = BuildIDF(G);
      IDF.dumpDot(DotFile, "Iterated Dominance Frontier");
    } else {
      std::cerr << "Unable to open file";
      return EXIT_FAILURE;
    }
  }
}
