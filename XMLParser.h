#pragma once
#include <ESP8266WiFi.h>
#include <TinyXML.h>
#include <tuple>
#include "XMLStreamer.h"

class XMLParser {
  public:
    enum State {
      Ready,
      Parsing,
      Success,
      Error
    };
    XMLParser();

    State update();
    State state() const;
    void reset();
    virtual std::tuple<uint32_t, uint32_t> results() const = 0;
  protected:
    virtual void xml_callback(uint8_t statusFlags, char* tagName,
      uint16_t tagNameLen, char* data, uint16_t dataLen) = 0;
    virtual bool makeRequest(WiFiClient& client) = 0;
    virtual void beforeUpdate() {}
    virtual bool afterUpdate() {return true;}
      
  private:
    State m_state;
    TinyXML m_xml;
    WiFiClient m_client;
    XMLStreamer m_streamer;
    uint8_t m_buffer[150];
    static XMLParser* s_parsingInstance;
    static void xml_callback_curry(uint8_t statusFlags, char* tagName,
      uint16_t tagNameLen, char* data, uint16_t dataLen);
};
