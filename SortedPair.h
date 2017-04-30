#include <algorithm>

template<typename T> struct SortedPair {
  T first;
  T second;
  SortedPair() = default;
  SortedPair(const T& t1, const T& t2) : first(std::min(first, second)), second(std::max(second, first)) {};

  void push(const T& t) {
    const auto smallest = std::min(t, first);
    const auto secondSmallest = std::min(t, second);
    first = smallest;
    second = secondSmallest;
  }
};
