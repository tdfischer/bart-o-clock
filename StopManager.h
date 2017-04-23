#pragma once
#include <vector>
#include "Transit.h"

class TransitDot;

class StopManager {
  struct Registration {
    Stop stop;
    std::vector<TransitDot*> renderers;
  };

  public:
    StopManager();
    void addStop(Stop stop, TransitDot* renderer);
    void update();
    void render();

  private:
    std::vector<Registration> m_registrations;
    std::vector<Registration>::iterator m_currentUpdate;
};

