#include "Transit.h"
#include "NextbusParser.h"
#include "BartParser.h"
#include "XMLStreamer.h"

Stop::Stop(const char* _agency, const char* _route, const char* _stopID):
    agency(_agency),
    route(_route),
    stopID(_stopID),
    m_parser(determineParser())
{
}

std::shared_ptr<XMLParser>
Stop::determineParser()
{
  if (strcmp(agency, "bart") == 0) {
    return std::make_shared<BartParser>(route, stopID);
  } else if (strlen(agency) > 0) {
    return std::make_shared<NextbusParser>(agency, route, stopID);
  } else {
    return nullptr;
  }
}

// Returns true if new predictions are available
bool
Stop::updatePredictions()
{
  if (m_backoff.throttled()) {
    return false;
  }

  if (m_parser->state() == XMLParser::Ready) {
    Serial.print("Updating predictions for ");
    Serial.println(route);
  }

  m_parser->update();
  if (m_parser->state() == XMLParser::Parsing) {
    return false;
  } else if (m_parser->state() == XMLParser::Error) {
    Serial.println("Update error.");
    m_parser->reset();
    m_backoff.failure();
    return false;
  } else if (m_parser->state() == XMLParser::Success) {
    Serial.println("Update complete!");
    std::tie(predictions[0], predictions[1]) = m_parser->results();
    m_parser->reset();
  }
  if (!predictions[0].valid()) {
    m_backoff.failure();
    Serial.println("No next bus.");
    return false;
  }
  Serial.print("Updated time for ");
  Serial.print(route);
  Serial.print(": ");
  Serial.print(Minutes{predictions[0].timeRemaining()}.value);
  Serial.println(" minutes");
  m_backoff.success();
  return true;
}
