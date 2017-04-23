ARDUINO_ROOT := /usr/local/arduino/
LIBS = $(ESP_LIBS)/ESP8266WiFi \
			 $(ESP_LIBS)/ESP8266WebServer \
			 $(ESP_LIBS)/ESP8266mDNS \
			 $(ARDUINO_ROOT)/libraries/TinyXML \
			 $(ARDUINO_ROOT)/libraries/FastLED \
			 $(ARDUINO_ROOT)/libraries/Time
BUILD_EXTRA_FLAGS = -Wall
