#pragma once
#include "Transit.h"
#include <FastLED.h>
#include <Time.h>
#include <vector>

struct Sprite {
  Sprite(CRGB* leds) : m_leds(leds) {}
  CRGB* m_leds;
};

struct TransitDot : public Sprite {
  TransitDot(CRGB* leds) : Sprite(leds) {}

  Prediction m_nextTime;
  uint8_t m_pulseFrame = 0;
  bool validPredictions = false;
  const Stop* m_currentStop;

  void setNextTime(const Stop& stop, Prediction t) {
    if (m_currentStop == &stop) {
      m_nextTime = t;
    } else {
      if (t < m_nextTime || !validPredictions) {
        m_currentStop = &stop;
        m_nextTime = t;
      }
    }
    validPredictions = m_nextTime >= now();
  }

  uint8_t colorForDuration(int minutesInFuture) const {
    if (minutesInFuture < 5) {
      return 0;
    }
    if (minutesInFuture < 10) {
      return 30;
    }
    if (minutesInFuture < 15) {
      return 70;
    }
    return 100;
  }
  
  void animate(time_t currentTime, bool pulse) {
    if (validPredictions) {
      m_leds[0] = CHSV(colorForDuration(minutesRemaining(currentTime)), 255, scale8(pulse ? 255 : 100, lerp8by8(200, 255, sin8(pulse ? m_pulseFrame : 0))));
      m_pulseFrame += 1;
    } else {
      m_leds[0] = CHSV(210, 128, 50);
    }
  }

  uint8_t minutesRemaining(time_t currentTime) const {
    if (currentTime >= m_nextTime) {
      return 0;
    } else {
      return std::min(255l, (m_nextTime - currentTime)/60);
    }
  }
  
  void animateTimer(CRGB* timerLED, time_t currentTime) {
    const int minutesPerDot[] = {
      0, 1, 2, 3, 5, 10, 15, 30
    };
    static unsigned int fadeShift = 0;
    fadeShift += 1;
    for(int i = 0; i < 8; i++) {
      const uint8_t fade = lerp8by8(230, 255, (i / 8.0) * 255 + fadeShift);
      if (minutesRemaining(currentTime) >= minutesPerDot[i] && validPredictions) {
        timerLED[i] = CHSV(colorForDuration(minutesPerDot[i]), 255, fade);
      } else {
        timerLED[i] = CHSV(colorForDuration(minutesPerDot[i]), 128, scale8(50, fade));
      }
    }
  }
};

