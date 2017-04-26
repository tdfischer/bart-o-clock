#include "NextbusParser.h"
#include <WiFiClient.h>
#include "Transit.h"
#include "XMLParser.h"

NextbusParser::NextbusParser(const char* agency, const char* route, const char* stopID) :
  XMLParser(),
  m_seconds({0, 0}),
  m_agency(agency),
  m_route(route),
  m_stopID(stopID)
{
}

void
NextbusParser::makeRequest(WiFiClient& client)
{
  Serial.print("Downloading nextbus data for ");
  Serial.println(m_route);
  client.print("GET /service/publicXMLFeed?command=predictions&a=");
  client.print(m_agency);
  client.print("&r=");
  client.print(m_route);
  client.print("&s=");
  client.print(m_stopID);
  client.print(" HTTP/1.1\r\nHost: ");
  client.print(hostname());
  client.print("\r\nConnection: Close\r\n\r\n");
}

std::tuple<uint32_t, uint32_t>
NextbusParser::results() const {
  return std::make_tuple(m_seconds[0]/60, m_seconds[1]/60);
}

bool
NextbusParser::afterUpdate()
{
  if (m_seconds[0] == 0) {
    Serial.println("No vehicle found :(");
    return false;
  } else {
    Serial.println("---beep beep---");
    return true;
  }
}

void
NextbusParser::xml_callback(uint8_t statusFlags, char* tagName,
  uint16_t tagNameLen, char* data, uint16_t dataLen) {
  constexpr uint64_t MIN_TIME = 5;
  if((statusFlags & STATUS_ATTR_TEXT) && !strcasecmp(tagName, "seconds")) {
    uint32_t t = atoi(data); // Prediction in seconds (0 if gibberish)
    if(t >= (MIN_TIME * 60)) {        // Above our "too soon" threshold?
      if(!m_seconds[0]) {               //  No predictions yet?
        m_seconds[0] = t;               //   Save in slot 0, done
      } else {                        //  Else 1-2 existing predictions...
        if(t <= m_seconds[0]) {         // New time sooner than slot 0?
          m_seconds[1] = m_seconds[0];    //  Move 0 up to 1 (old 1 discarded)
          m_seconds[0] = t;             //  Store new time in 0
        } else if(!m_seconds[1] ||      // Slot 1 empty?
                 (t <= m_seconds[1])) { // Or new time sooner than 1?
          m_seconds[1] = t;             //  Store new time in slot 1
        }                             // Else discard
        if(m_seconds[0] == m_seconds[1]) m_seconds[1] = 0; // If equal, delete 1
      }
    }
  }
}
