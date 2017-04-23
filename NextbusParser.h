#pragma once
#include "XMLParser.h"

class NextbusParser : public XMLParser {
  public:
    NextbusParser(const char* agency, const char* route, const char* stopID);

    std::tuple<uint32_t, uint32_t> results() const override;

  protected:
    void xml_callback(uint8_t statusFlags, char* tagName,
      uint16_t tagNameLen, char* data, uint16_t dataLen) override;
    bool makeRequest(WiFiClient& client) override;
    bool afterUpdate() override;
      
  private:
    TinyXML m_xml;
    uint8_t m_buffer[150];
    uint32_t m_seconds[2];
    const char* m_agency;
    const char* m_route;
    const char* m_stopID;
};