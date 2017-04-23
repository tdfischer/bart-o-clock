#pragma once
#include <vector>
#include <cstdint>

struct WifiConnection {
  const char* ssid;
  const char* password;
};

class WifiManager {
  public:
    WifiManager(std::initializer_list<WifiConnection> connections);
    void update();
    void reconnect();
  private:
    void nextSSID();
    std::vector<WifiConnection> m_connections;
    std::vector<WifiConnection>::iterator m_current;
    uint32_t m_retry;
};
