// ----------------------------------------------------------------------------
//   "THE BEER-WARE LICENSE" (Revision 42):
//   <alex.rom23@mail.ru> wrote this file. As long as you retain this
//   notice you can do whatever you want with this stuff. If we meet some day,
//   and you think this stuff is worth it, you can buy me a beer in return.
// ----------------------------------------------------------------------------
#pragma once

#include "ReducibleGraph.h"
#include "Utils.h"

#include <map>
#include <vector>
namespace lqvm {
using NodetoDominatorsTy = std::map<const Node *, std::vector<const Node *>>;
NodetoDominatorsTy ComputeDominators(const GraphTy &G);

std::map<const Node *, const Node *> ComputeIDom(const GraphTy &G);
} // namespace lqvm