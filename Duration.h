#pragma once
#include <Arduino.h>
#undef max
#undef min
#include <cstdint>
#include <type_traits>
#include <limits>

template<int Period> struct Duration {
  uint32_t value;
  static constexpr uint32_t period = Period;

  constexpr Duration() : value(0) {}
  Duration(const Duration&) = default;
  template<typename Integer,
           typename = typename std::enable_if<std::is_integral<Integer>::value>::type>
  constexpr explicit Duration(const Integer& r) : value(r) {}
  template<int Period2> Duration(const Duration<Period2>& d) : value((d.value * Period2) / Period) {}

  Duration& operator=(const Duration& d) = default;

  bool operator<(const Duration& d) const {
    return value < d.value;
  }
  bool operator>(const Duration& d) const {
    return value > d.value;
  }
  bool operator<=(const Duration& d) const {
    return value <= d.value;
  }
  bool operator>=(const Duration& d) const {
    return value >= d.value;
  }

  bool operator==(const Duration& d) const {
    return value == d.value;
  }

  bool operator!=(const Duration& d) const {
    return value != d.value;
  }
};

typedef Duration<1> Seconds;
typedef Duration<60> Minutes;
typedef Duration<60 * 60> Hours;
