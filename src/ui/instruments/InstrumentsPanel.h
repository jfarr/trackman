#pragma once

#include <JuceHeader.h>
#include "InstrumentControl.h"

class InstrumentsPanel : public juce::Component {
  public:
    InstrumentsPanel() = default;
    ~InstrumentsPanel() override = default;

    void addTrack(InstrumentControl *lane) { instruments.push_back(lane); }
    void clear() { instruments.clear(); }
    void update();

    //==============================================================================
    // Component
    void paint(juce::Graphics &g) override;
    void resized() override;

  private:
    const int panelWidth = 25;
    const int  topStripHeight = 15;

    std::list<InstrumentControl *> instruments;

    void resize();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(InstrumentsPanel)
};
