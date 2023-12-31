// ----------------------------------------------------------------------------
//   "THE BEER-WARE LICENSE" (Revision 42):
//   <alex.rom23@mail.ru> wrote this file. As long as you retain this
//   notice you can do whatever you want with this stuff. If we meet some day,
//   and you think this stuff is worth it, you can buy me a beer in return.
// ----------------------------------------------------------------------------

#pragma once

#include "ReducibleGraph.h"
#include "scanner.h"

#include "graph_parser.h"

namespace lqvm {

class Driver {
private:
  scanner Scan;
  parser Parser;

  friend class scanner;

public:
  GraphTy<Node> G;

  Driver() : Scan{}, Parser{Scan, *this} {}
  void parse() { Parser.parse(); }
  void switchInputStream(std::istream *Is) { Scan.switch_streams(Is, nullptr); }
};
} // namespace lqvm