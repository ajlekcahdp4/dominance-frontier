// ----------------------------------------------------------------------------
//   "THE BEER-WARE LICENSE" (Revision 42):
//   <alex.rom23@mail.ru> wrote this file. As long as you retain this
//   notice you can do whatever you want with this stuff. If we meet some day,
//   and you think this stuff is worth it, you can buy me a beer in return.
// ----------------------------------------------------------------------------
#pragma once

#include <algorithm>
#include <cassert>
#include <ranges>
#include <vector>

namespace lqvm {
namespace utils {
template <typename Cont>
Cont orderedIntersection(const Cont &First, const Cont &Second) {
  Cont Res;
  for (auto &&Entry : First)
    if (std::ranges::find(Second, Entry) != Second.end())
      Res.push_back(Entry);
  return Res;
}

template <typename Range, typename NodeTy>
inline size_t getIndexIn(const NodeTy *Nd, Range Order) {
  auto Found = std::find(Order.begin(), Order.end(), Nd);
  assert(Found != Order.end());
  return std::distance(Order.begin(), Found);
}

template <typename ContTy>
inline auto first_match(const ContTy &Cont1, const ContTy &Cont2)
    -> decltype(Cont1.cbegin()) {
  auto FirstIt = Cont1.cbegin();
  auto FirstEnd = Cont1.cend();
  while (FirstIt != FirstEnd) {
    auto FoundIt = std::ranges::find(Cont2, *FirstIt);
    if (FoundIt != Cont2.cend())
      break;
    ++FirstIt;
  }
  return FirstIt;
}

template <std::ranges::input_range RangeTy> auto drop_begin(RangeTy R) {
  return R | std::views::drop(1);
}

template <std::ranges::forward_range RangeTy> auto drop_end(const RangeTy &R) {
  return R | std::views::take(std::ranges::size(R) - 1);
}

template <std::ranges::forward_range RangeTy>
bool is_contained(RangeTy R, const typename RangeTy::value_type &Val) {
  return std::ranges::find(R, Val) != R.end();
}

} // namespace utils
} // namespace lqvm
