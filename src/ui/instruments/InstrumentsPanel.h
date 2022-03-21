#pragma once

#include <JuceHeader.h>

#include "InstrumentControl.h"
#include "model/Project.h"

using namespace juce;

namespace trackman {

class InstrumentsPanel : public Component {
  public:
    InstrumentsPanel(Project &project) : project(project) {}
    ~InstrumentsPanel() override = default;

    void addTrack(InstrumentControl *lane) { instruments.push_back(lane); }
    void clear() { instruments.clear(); }
    void update();

    //==============================================================================
    // Component
    void paint(Graphics &g) override;
    void resized() override;

  private:
    const int panelWidth = 30;
    const int topStripHeight = 30;

    Project &project;
    list<InstrumentControl *> instruments;

    void resize();
    double getPaneHeight();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(InstrumentsPanel)
};

}
