#pragma once
#include <cstdint>

class WiFiClient;
class TinyXML;

class XMLStreamer {
  public:
    enum State {
      Success,
      Ready,
      Error
    };
    XMLStreamer(WiFiClient& client, TinyXML& xml);
    ~XMLStreamer();
    State stream();
    State state() const;
    void reset();
  private:
    TinyXML& m_xml;
    WiFiClient& m_client;
    State m_state;
    uint32_t m_lastRead;
};
