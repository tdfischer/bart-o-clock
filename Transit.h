#pragma once
#include <cstdint>
#include "Duration.h"
#include <Time.h>
#include <algorithm>
#include <memory>
#include <string.h>
#include <Arduino.h>

class XMLParser;

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
    return std::min((uint32_t)maxBackoffSeconds * millisPerSeconds, std::max(minDelaySeconds * millisPerSeconds, (backoffSeconds * millisPerSeconds) * multiplier));
  }
};

struct Prediction {
  time_t timestamp;

  Prediction() : timestamp(0) {}
  explicit Prediction(const Seconds& s) : timestamp(now() + s.value) {}

  bool valid() const {
    return timestamp != 0;
  }

  Prediction& operator=(const time_t v) {
    timestamp = v;
    return *this;
  }

  Seconds timeRemaining(time_t ref) const {
    return Seconds{timestamp - ref};
  }

  bool operator==(const Prediction& p) const {
    return timestamp == p.timestamp;
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

  bool operator<(const Stop& other) const {
    return strcmp(stopID, other.stopID) < 0;
  }

  enum UpdateResult {
    Continue,
    Updated,
    NoUpdate
  };
    
  Prediction predictions[2];    // Most recent predictions from server
  UpdateResult updatePredictions();
  private:
    std::shared_ptr<XMLParser> m_parser;
    std::shared_ptr<XMLParser> determineParser();
    Backoff m_backoff;
};

