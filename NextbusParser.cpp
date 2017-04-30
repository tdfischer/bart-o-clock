#include "NextbusParser.h"
#include <WiFiClient.h>
#include "Transit.h"
#include "XMLParser.h"

NextbusParser::NextbusParser(const char* agency, const char* route, const char* stopID) :
  XMLParser(),
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

std::tuple<Prediction, Prediction>
NextbusParser::results() const {
  return std::make_tuple(Prediction{m_seconds[0]}, Prediction{m_seconds[1]});
}

bool
NextbusParser::afterUpdate()
{
  if (m_seconds[0] == Seconds{}) {
    Serial.println("No vehicle found :(");
    return false;
  } else {
    Serial.print("---beep beep---  ");
    Serial.println(Minutes{m_seconds[0]}.value);
    return true;
  }
}

void
NextbusParser::beforeUpdate()
{
  m_seconds[0] = Seconds{};
  m_seconds[1] = Seconds{};
}

void
NextbusParser::xml_callback(uint8_t statusFlags, char* tagName,
  uint16_t tagNameLen, char* data, uint16_t dataLen) {
  if((statusFlags & STATUS_ATTR_TEXT) && !strcasecmp(tagName, "seconds")) {
    Seconds t{atoi(data)}; // Prediction in seconds (0 if gibberish)
    if(m_seconds[0] == Seconds{}) {               //  No predictions yet?
      m_seconds[0] = t;               //   Save in slot 0, done
    } else {                        //  Else 1-2 existing predictions...
      m_seconds[1] = t;
    }
  }
}
