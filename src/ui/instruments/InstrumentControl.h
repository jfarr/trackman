#pragma once

#include "model/Track.h"
#include <JuceHeader.h>

namespace trackman {

class InstrumentControl : public juce::Component {
  public:
    InstrumentControl(Track &track);
    ~InstrumentControl() override = default;

    int getPreferredHeight() const { return preferredHeight; }

    void update();

    //==============================================================================
    // Component
    void paint(juce::Graphics &g) override;
    void resized() override;

  private:
    const int preferredHeight = 75;

    Track &track;
    juce::TextButton selectButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(InstrumentControl)
};

} // namespace trackman
