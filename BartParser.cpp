#include "BartParser.h"

BartParser::BartParser(const char* destination, const char* station) :
  XMLParser(),
  m_destination(destination),
  m_station(station)
{
}

void
BartParser::makeRequest(WiFiClient& client)
{
  Serial.println("Downloading BART data");
  client.print("GET /api/etd.aspx?cmd=etd&orig=");
  client.print(m_station);
  client.print("&key=ZVZJ-PMTI-9J6T-DWE9");
  client.print(" HTTP/1.1\r\nHost: ");
  client.print(hostname());
  client.print("\r\nConnection: Close\r\n\r\n");
}

std::tuple<Prediction, Prediction>
BartParser::results() const
{
  return std::make_tuple(m_predictions[(int)strToBartLine(m_destination)], Prediction{});
}

void
BartParser::xml_callback(uint8_t statusFlags, char* tagName,
  uint16_t tagNameLen, char* data, uint16_t dataLen) {
  if((statusFlags & STATUS_TAG_TEXT) && !strcasecmp(tagName, "/root/station/etd/estimate/minutes")) {
    Minutes t{atoi(data)};
    m_parsedMinutes = t;
    Serial.println("Found minutes...");
  } else if((statusFlags & STATUS_TAG_TEXT) && !strcasecmp(tagName, "/root/station/etd/abbreviation")) {
    m_currentLine = strToBartLine(data);
    Serial.println("Found station...");
  }
  if (m_currentLine != BartLine::Unknown && m_parsedMinutes != 0) {
    Serial.print("Got BART update for ");
    Serial.print((int)m_currentLine);
    Serial.print(": ");
    Serial.println(m_parsedMinutes.value);
    m_predictions[(int)m_currentLine] = std::min(m_predictions[(int)m_currentLine], Prediction{m_parsedMinutes});
    m_parsedMinutes = Minutes{0};
    m_currentLine = BartLine::Unknown;
  }
}

void
BartParser::beforeUpdate()
{
  m_currentLine = BartLine::Unknown;
  m_parsedMinutes = Minutes{0};
  m_predictions[strToBartLine(m_destination)] = 3000;
}

BartParser::BartLine
BartParser::strToBartLine(const char* abbrev)
{
  if (strcasecmp(abbrev, "RICH")) {
    return BartLine::Richmond;
  } else if (strcasecmp(abbrev, "PITT")) {
    return BartLine::Pittsburg;
  } else if (strcasecmp(abbrev, "DALY") || strcasecmp(abbrev, "MLBR") || strcasecmp(abbrev, "SFIA")) {
    return BartLine::SanFrancisco;
  }
}
