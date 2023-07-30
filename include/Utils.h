// ----------------------------------------------------------------------------
//   "THE BEER-WARE LICENSE" (Revision 42):
//   <alex.rom23@mail.ru> wrote this file. As long as you retain this
//   notice you can do whatever you want with this stuff. If we meet some day,
//   and you think this stuff is worth it, you can buy me a beer in return.
// ----------------------------------------------------------------------------
#pragma once

#include <algorithm>
#include <vector>

namespace lqvm {
template <typename Cont>
Cont OrderedIntersection(const Cont &First, const Cont &Second) {
  Cont Res;
  for (auto &&Entry : First)
    if (std::find(Second.begin(), Second.end(), Entry) != Second.end())
      Res.push_back(Entry);
  return Res;
}
} // namespace lqvm