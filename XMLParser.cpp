#include "XMLParser.h"

XMLParser* XMLParser::s_parsingInstance = nullptr;

XMLParser::XMLParser() :
  m_streamer(m_client, m_xml),
  m_state(Ready)
{
  m_xml.init(m_buffer, sizeof(m_buffer), &XMLParser::xml_callback_curry);
}

void
XMLParser::reset()
{
  m_state = Ready;
  Serial.println("Client stopped");
}

XMLParser::State
XMLParser::state() const
{
  return m_state;
}

void
XMLParser::updateDNS()
{
  IPAddress hostIP;
  if (WiFi.hostByName(hostname(), hostIP)) {
    m_hostnameIP = hostIP;
  } else {
    Serial.println("Could not resolve DNS!");
  }
}

XMLParser::State
XMLParser::update()
{
  if (m_state == Ready) {
    Serial.println("Ready -> Parsing");
    beforeUpdate();
    bool ret = false;
    m_client.stop();
    m_xml.reset();
    m_streamer.reset();
    if (!m_hostnameIP) {
      updateDNS();
    }
    if (m_client.connect(m_hostnameIP, 80)) {
      makeRequest(m_client);
      Serial.println("Request ready!");
      m_state = Parsing;
    } else {
      Serial.println("Could not start request!");
      m_state = Error;
      updateDNS();
    }
  }
  if (m_state == Parsing) {
    s_parsingInstance = this;
    m_streamer.stream();
    s_parsingInstance = nullptr;
    switch (m_streamer.state()) {
      case XMLStreamer::Success:
        m_state = afterUpdate() ? Success : Error;
        Serial.println("Parsing -> Success?");
        break;
      case XMLStreamer::Error:
        m_state = Error;
        Serial.println("Parsing -> Error");
        break;
    }
  }
  return m_state;
}

void
XMLParser::xml_callback_curry(uint8_t statusFlags, char* tagName,
  uint16_t tagNameLen, char* data, uint16_t dataLen) {
  s_parsingInstance->xml_callback(statusFlags, tagName, tagNameLen, data, dataLen);
}
