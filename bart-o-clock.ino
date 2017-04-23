#include <FastLED.h>
#include <Time.h>
#include <vector>

#include <ESP8266WiFi.h>
#include "Transit.h"
#include "TransitDot.h"
#include "StopManager.h"
#include "Wifi.h"

#define NUM_LEDS 13
CRGB leds[NUM_LEDS];

TransitDot dots[] {
  TransitDot{&leds[0]},
  TransitDot{&leds[1]},
  TransitDot{&leds[2]},
  TransitDot{&leds[3]},
  TransitDot{&leds[4]},
};

WifiManager wireless{
  {"The Frequency", "thepasswordkenneth"},
  {"Noisebridge", 0}
};

StopManager stops;

void setup() {
  Serial.begin(115200);
  setTime(0);
  FastLED.addLeds<NEOPIXEL, 2>(leds, NUM_LEDS);
  for(int i = 0; i < NUM_LEDS;i++) {
    leds[i] = CHSV(0, 0, 0);
  }
  FastLED.setBrightness(128);
  FastLED.show();

  Serial.println("Graphics initialized");
  for(int h = 0; h < 255; h++) {
    for(int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CHSV(h, 255, 255);
    }
    FastLED.show();
    delay(10);
  }

  wireless.update();
  stops.addStop(Stop{"actransit", "72", "1030020"}, &dots[0]);
  stops.addStop(Stop{"actransit", "72M", "1030020"}, &dots[0]);
  stops.addStop(Stop{"actransit", "72R", "1030020"}, &dots[0]);
  stops.addStop(Stop{"actransit", "51A", "1006450"}, &dots[1]);
  stops.addStop(Stop{"bart", "RICH", "12th"}, &dots[2]);
  stops.addStop(Stop{"bart", "PITT", "12th"}, &dots[3]);
  stops.addStop(Stop{"bart", "SFIA", "12th"}, &dots[4]);
  Serial.println("Data initialized");
}

TransitDot* graphedDot = nullptr;
bool switchedDot = false;

void loop() {
  time_t loopTime = now();
  int smallestTime = 32760;
  for(TransitDot& dot : dots) {
    if (dot.validPredictions) {
      const int dotMinutes = dot.minutesRemaining(loopTime);
      if (dotMinutes < smallestTime) {
        graphedDot = &dot;
        smallestTime = dotMinutes;
      }
    }
  }
  for(TransitDot& dot : dots) {
    dot.animate(loopTime, &dot == graphedDot);
  }
  if (graphedDot) {
    graphedDot->animateTimer(&leds[5], loopTime);
  }
  FastLED.show();
  wireless.update();
  stops.update();
}
