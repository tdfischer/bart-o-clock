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
  const auto stat = m_currentUpdate->stop.updatePredictions();
  if (stat == Stop::Continue) {
    return;
  }

  if (stat == Stop::Updated) {
    for(TransitDot* dot : m_currentUpdate->renderers) {
      if (dot) {
        dot->setPredictions(m_currentUpdate->stop, m_currentUpdate->stop.predictions[0], m_currentUpdate->stop.predictions[1]);
      }
    }
  }
  m_currentUpdate++;
}
