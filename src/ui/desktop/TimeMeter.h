#pragma once

#include <JuceHeader.h>

#include "model/Project.h"

using namespace std;
using namespace juce;

namespace trackman {

class TimeMeter : public Component {
  public:
    TimeMeter(Project &project);
    ~TimeMeter() override = default;

    //==============================================================================
    // Component
    void paint(Graphics &g) override;

  private:
    Project &project;

    void drawTicksInSeconds(Graphics &g, const Rectangle<int> &bounds) const;
    void drawTicksInMeasures(Graphics &g, const Rectangle<int> &bounds) const;
    void drawStartMarker(Graphics &g) const;
};

} // namespace trackman
