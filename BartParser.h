#pragma once
#include "XMLParser.h"

class BartParser : public XMLParser {
  public:
    enum BartLine {
      Richmond,
      Pittsburg,
      WarmSprings,
      SanFrancisco,
      Unknown
    };
    BartParser(const char* destination, const char* station);

    std::tuple<Prediction, Prediction> results() const override;

    static BartLine strToBartLine(const char* abbrev);

  protected:
    void xml_callback(uint8_t statusFlags, char* tagName,
      uint16_t tagNameLen, char* data, uint16_t dataLen) override;
    void makeRequest(WiFiClient& client) override;
    void beforeUpdate() override;
    const char* hostname() const override {
      return "api.bart.gov";
    }

  private:
    const char* m_destination;
    const char* m_station;
    BartLine m_currentLine;
    Prediction m_predictions[5];
    Minutes m_parsedMinutes;
};
