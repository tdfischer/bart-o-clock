#include "XMLStreamer.h"
#include <ESP8266WiFi.h>
#include <TinyXML.h>

XMLStreamer::XMLStreamer(WiFiClient& client, TinyXML& xml) :
  m_client(client),
  m_xml(xml),
  m_lastRead(0)
{}

XMLStreamer::~XMLStreamer()
{
}

void
XMLStreamer::reset()
{
  m_state = Ready;
  m_lastRead = 0;
}

XMLStreamer::State
XMLStreamer::stream()
{
  const int READ_TIMEOUT = 15;
  bool timedOut = false;
  int c;
  if (m_state != Ready) {
    return m_state;
  }
  if (m_lastRead == 0) m_lastRead = millis();
  if(m_client.available()) {
    c = m_client.read();
    //Serial.print((char)c);
    m_xml.processChar(c);
    m_lastRead = millis(); // Reset timeout clock
    m_state = m_client.connected() ? Ready : Success;
  } else if((millis() - m_lastRead) >= (READ_TIMEOUT * 1000)) {
    Serial.println("---Timeout---");
    m_state = Error;
  }
  return m_state;
}

XMLStreamer::State XMLStreamer::state() const
{
  return m_state;
}
