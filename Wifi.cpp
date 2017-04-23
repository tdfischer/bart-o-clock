#include "Wifi.h"
#include <ESP8266WiFi.h>

WifiManager::WifiManager(std::initializer_list<WifiConnection> connections) :
  m_connections(connections), m_current(m_connections.begin()) {};

void
WifiManager::update() {
  if (WiFi.status() != WL_CONNECTED) {
    reconnect();
  }
}

void
WifiManager::reconnect()
{
  m_retry++;
  if (m_retry >= 3) {
    nextSSID();
    m_retry = 0;
  }
  Serial.print("Connecting to wifi ");
  Serial.print(m_current->ssid);
  WiFi.begin(m_current->ssid, m_current->password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
}

void
WifiManager::nextSSID()
{
  m_current++;
  if (m_current == m_connections.end()) {
    m_current = m_connections.begin();
  }
}
