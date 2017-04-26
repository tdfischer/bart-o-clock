#include "StopManager.h"
#include <algorithm>
#include "Transit.h"
#include "TransitDot.h"

StopManager::StopManager() {}

void
StopManager::addStop(Stop stop, TransitDot* renderer)
{
    auto idx = std::find_if(std::begin(m_registrations), std::end(m_registrations), [stop](Registration& r) {
        return r.stop == stop;
      });
    if (idx == m_registrations.end()) {
      idx = m_registrations.emplace(std::begin(m_registrations), Registration{stop, {}});
    }
    idx->renderers.push_back(renderer);
    m_currentUpdate = m_registrations.begin();
}

void
StopManager::update()
{
  if (m_currentUpdate == m_registrations.end()) {
    m_currentUpdate = m_registrations.begin();
  }
  if (m_currentUpdate->stop.updatePredictions()) {
    Prediction nearestBus;
    nearestBus = std::min(m_currentUpdate->stop.predictions[0], nearestBus);
    for(TransitDot* dot : m_currentUpdate->renderers) {
      if (dot) {
        dot->setNextTime(m_currentUpdate->stop, nearestBus);
      }
    }
    m_currentUpdate++;
  }
}
