#pragma once
#include <cstdint>
#include "NextbusParser.h"
#include "BartParser.h"

using std::min;
using std::max;

struct Backoff {
  uint32_t multiplier = 1;
  uint32_t nextMillis = 0;
  static constexpr uint32_t millisPerSeconds = 1000;
  static constexpr uint32_t maxBackoffSeconds = 60 * 15;
  static constexpr uint32_t minDelaySeconds = 60;
  static constexpr uint32_t backoffSeconds = 1;

  void success() {multiplier = 1;nextMillis = millis() + getDelay();}
  void failure() {multiplier *= 2;nextMillis = millis() + getDelay();}
  bool throttled() const { return millis() < nextMillis; }
  uint32_t getDelay() const {
    return min((uint32_t)maxBackoffSeconds * millisPerSeconds, max(minDelaySeconds * millisPerSeconds, (backoffSeconds * millisPerSeconds) * multiplier));
  }
};

struct Stop {
  const char       *agency;        // Get these using routefinder.py script
  const char       *route;
  const char       *stopID;
  Stop(const char* _agency, const char* _route, const char* _stopID);
  Stop() : agency(nullptr), route(nullptr), stopID(nullptr) {};

  bool operator==(const Stop& other) const {
    return strcmp(stopID, other.stopID) == 0 && strcmp(route, other.route) == 0;
  }
    
  uint32_t          minutes[2];    // Most recent predictions from server
  bool updatePredictions();
  private:
    std::shared_ptr<XMLParser> m_parser;
    std::shared_ptr<XMLParser> determineParser();
    Backoff m_backoff;
};

