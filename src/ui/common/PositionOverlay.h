#pragma once

#include <JuceHeader.h>

using namespace juce;

namespace trackman {

class PositionOverlay : public Component, private Timer {
  public:
    explicit PositionOverlay(AudioTransportSource &transportSource);

    void paint(Graphics &g) override;

  private:
    void timerCallback() override;

    AudioTransportSource &transportSource;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PositionOverlay)
};

} // namespace trackman
