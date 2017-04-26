#pragma once
#include <Arduino.h>
#undef max
#undef min
#include <cstdint>
#include <type_traits>
#include <limits>

template<int _ticksPerUnit> struct Duration {
  uint32_t value;
  static constexpr uint32_t ticksPerUnit = _ticksPerUnit;

  template<typename Integer,
           typename = typename std::enable_if<std::is_integral<Integer>::value>::type>
   explicit constexpr Duration(Integer v) : value(v) {}

  template<typename OtherDuration,
           typename = typename std::enable_if<std::is_convertible<OtherDuration, Duration>::value>::type>
  Duration(const OtherDuration& v) {
    *this = v;
  }

  Duration() : value(0) {}

  template<typename OtherDuration> bool operator<(const OtherDuration& d) {
    return value < Duration{d}.value;
  }
  template<typename OtherDuration> bool operator<=(const OtherDuration& d) {
    return value <= Duration{d}.value;
  }

  template<typename OtherDuration> bool operator==(const OtherDuration& d) {
    return value == Duration{d}.value;
  }

  operator bool() const {
    return (bool)value;
  }

  template<typename OtherDuration> bool operator!=(const OtherDuration& d) {
    return value != Duration{d}.value;
  }

  template<typename OtherDuration> operator OtherDuration() const {
    return (value * ticksPerUnit) / OtherDuration::ticksPerUnit;
  }

  template<typename OtherDuration,
           typename = typename std::enable_if<std::is_convertible<OtherDuration, Duration>::value>::type> Duration& operator=(const OtherDuration& d) {
    value = (d.value * OtherDuration::ticksPerUnit) / ticksPerUnit;
    return *this;
  }
};

typedef Duration<1> Seconds;
typedef Duration<60> Minutes;
typedef Duration<60 * 60> Hours;
