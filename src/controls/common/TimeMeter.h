#pragma once

#include <JuceHeader.h>

class TimeMeter : public juce::Component {
  public:
    TimeMeter() {}
    ~TimeMeter() {}

    //==============================================================================
    // Component
    void paint(juce::Graphics &g) override;

  private:
    float scale = 75;
};
