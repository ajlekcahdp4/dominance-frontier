// ----------------------------------------------------------------------------
//   "THE BEER-WARE LICENSE" (Revision 42):
//   <alex.rom23@mail.ru> wrote this file. As long as you retain this
//   notice you can do whatever you want with this stuff. If we meet some day,
//   and you think this stuff is worth it, you can buy me a beer in return.
// ----------------------------------------------------------------------------
#include "ReducibleGraph.h"
#include <iostream>

int main() {
  lqvm::ReducibleGraphBuilder GB(100);
  auto G = GB.generate();
  G.dumpDot(std::cout);
}