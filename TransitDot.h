#pragma once
#include "Transit.h"
#include <FastLED.h>
#include <Time.h>
#include <vector>
#include <map>
#include "SortedPair.h"

struct Sprite {
  Sprite(CRGB* leds) : m_leds(leds) {}
  CRGB* m_leds;
};

struct TransitDot : public Sprite {
  TransitDot(CRGB* leds) : Sprite(leds) {}

  uint8_t m_pulseFrame = 0;
  bool validPredictions = false;
  std::map<Stop, std::tuple<Prediction, Prediction>> m_predictions;

  void setPredictions(const Stop& stop, const Prediction& p1, const Prediction& p2) {
    m_predictions[stop] = std::make_tuple(p1, p2);
    const auto best = minutesRemaining(now());
    validPredictions = best.first != Minutes{};
    Serial.println(Minutes{p1.timeRemaining(now())}.value);
    Serial.println(Minutes{p2.timeRemaining(now())}.value);
    Serial.println(Minutes{best.first}.value);
    Serial.println(Minutes{best.second}.value);
  }

  uint8_t colorForDuration(const Minutes& minutesInFuture) const {
    if (minutesInFuture < Minutes{5}) {
      return 0;
    }
    if (minutesInFuture < Minutes{10}) {
      return 30;
    }
    if (minutesInFuture < Minutes{15}) {
      return 70;
    }
    return 100;
  }
  
  void animate(time_t currentTime, bool pulse) {
    if (validPredictions) {
      m_leds[0] = CHSV(colorForDuration(minutesRemaining(currentTime).first), 255, scale8(pulse ? 255 : 100, lerp8by8(200, 255, sin8(pulse ? m_pulseFrame : 0))));
      m_pulseFrame += 1;
    } else {
      m_leds[0] = CHSV(210, 128, 50);
    }
  }

  SortedPair<Minutes> minutesRemaining(time_t currentTime) const {
    SortedPair<Minutes> ret{Hours{3}, Hours{3}};
    for(const auto& p : m_predictions) {
      ret.push(std::get<0>(p.second).timeRemaining(currentTime));
      ret.push(std::get<1>(p.second).timeRemaining(currentTime));
    }
    return ret;
  }

  void animateTimer(CRGB* timerLED, time_t currentTime) {
    const Minutes minutesPerDot[] = {
      Minutes{0}, Minutes{1}, Minutes{2}, Minutes{3}, Minutes{5}, Minutes{10}, Minutes{15}, Minutes{30}
    };
    static unsigned int fadeShift = 0;
    const auto remainingTime = minutesRemaining(currentTime);
    fadeShift += 1;
    for(int i = 0; i < 8; i++) {
      const uint8_t fade = lerp8by8(230, 255, (i / 8.0) * 255 + fadeShift);
      if (remainingTime.first >= minutesPerDot[i] && validPredictions) {
        timerLED[i] = CHSV(colorForDuration(minutesPerDot[i]), 255, fade);
      } else if (remainingTime.second >= minutesPerDot[i] && validPredictions) {
        timerLED[i] = CHSV(colorForDuration(minutesPerDot[i]), 255, scale8(80, fade));
      } else {
        timerLED[i] = CHSV(colorForDuration(minutesPerDot[i]), 128, scale8(50, fade));
      }
    }
  }
};

