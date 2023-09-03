// ----------------------------------------------------------------------------
//   "THE BEER-WARE LICENSE" (Revision 42):
//   <alex.rom23@mail.ru> wrote this file. As long as you retain this
//   notice you can do whatever you want with this stuff. If we meet some day,
//   and you think this stuff is worth it, you can buy me a beer in return.
// ----------------------------------------------------------------------------
#include "Dominators.h"
#include "ReducibleGraph.h"
#include "driver.h"

#include <boost/program_options.hpp>
#include <boost/program_options/option.hpp>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace po = boost::program_options;

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
	po::options_description Desc ("Allowed Options");

	int Seed = 0;
	unsigned GenIterations = 0;
	std::string DumpCFG="", DumpDomTree="", DumpDJ="", DumpDF="", DumpIDF="";

	Desc.add_options()
		("help,h", "Produce help message")
		("seed", po::value<int>(&Seed)->default_value(0), "Random Seed")
		("gen-iterations", po::value<unsigned>(&GenIterations)->default_value(10), "Iteration number for CFG generation")
		("dump-cfg", po::value<std::string>(&DumpCFG), "Dump CFG to .dot file")
		("generate-cfg", "Forces to generate CFG, not take it as an input")
		("print-idom", "Print Immediate dominators")
		("dump-dom-tree", po::value<std::string>(&DumpDomTree), "Dump Dom tree to .dot file")
		("dump-dj", po::value<std::string>(&DumpDJ), "Dump DJ graph to .dot file")
		("dump-df", po::value<std::string>(&DumpDF), "Dump Dominance frontier to .dot file")
		("dump-idf", po::value<std::string>(&DumpIDF), "Dump iterated dominance frontier to .dot file")
		;
	po::variables_map VM;
	po::store(po::command_line_parser(Argc, Argv).options(Desc).run(), VM);
	po::notify(VM);
	GraphTy<Node> G;
	if (VM.count("help")) {
		std::cout << Desc << std::endl;
		return EXIT_SUCCESS;
	}

	if (VM.count("generate-cfg")) {
    ReducibleGraphBuilder GB(GenIterations, Seed);
    G = GB.generate();
  } else
    G = parse_cfg();
  if (VM.count("dump-cfg")) {
    std::ofstream DotFile(DumpCFG);
    if (DotFile.is_open())
      G.dumpDot(DotFile, "Control Flow Graph");
    else {
      std::cerr << "Unable to open file";
      return EXIT_FAILURE;
    }
  }
#if 0
	if ()
    PrintDominators(ComputeDominators(G), std::cout);
#endif
	if (VM.count("print-idom"))
    printIDom(ComputeIDom(G), std::cout);
  if (VM.count("dump-dom-tree")) {
    std::ofstream DotFile(DumpDomTree);
    if (DotFile.is_open()) {
      auto DomTree = BuildDomTree(G);
      DomTree.dumpDot(DotFile, "Dom Tree");
    }
    else {
      std::cerr << "Unable to open file";
      return EXIT_FAILURE;
    }
  }
  if (VM.count("dump-dj")) {
    std::ofstream DotFile(DumpDJ);
    if (DotFile.is_open()) {
      auto DJ = ComputeDJ(G);
      DJ.dumpDot(DotFile, "DJ graph");
    } else {
      std::cerr << "Unable to open file";
      return EXIT_FAILURE;
    }
  }
  if (VM.count("dump-df")) {
    std::ofstream DotFile(DumpDF);
    if (DotFile.is_open()) {
      auto DF = BuildDF(G);
      DF.dumpDot(DotFile, "Dominance Frontier");
    } else {
      std::cerr << "Unable to open file";
      return EXIT_FAILURE;
    }
  }
  if (VM.count("dump-idf")) {
    std::ofstream DotFile(DumpIDF);
    if (DotFile.is_open()) {
      auto IDF = BuildIDF(G);
      IDF.dumpDot(DotFile, "Iterated Dominance Frontier");
    } else {
      std::cerr << "Unable to open file";
      return EXIT_FAILURE;
    }
  }
}
