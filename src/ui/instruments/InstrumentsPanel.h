#pragma once

#include <JuceHeader.h>

#include "InstrumentControl.h"
#include "model/Project.h"

using namespace juce;

namespace trackman {

class InstrumentsPanel : public AudioAppComponent {
  public:
    InstrumentsPanel(Project &project);
    ~InstrumentsPanel() override;

    void addTrack(InstrumentControl *lane) { instruments.push_back(lane); }
    void clear() { instruments.clear(); }
    void update();

    //==============================================================================
    // Component
    void paint(Graphics &g) override;
    void resized() override;

    //==============================================================================
    // AudioAppComponent
    void prepareToPlay(int blockSize, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock(const AudioSourceChannelInfo &bufferToFill) override;

  private:
    const int panelWidth = 30;
    const int topStripHeight = 30;

    Project &project;
    InstrumentPlayer &instrumentPlayer;
    list<InstrumentControl *> instruments;

    void resize();
    double getPaneHeight();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(InstrumentsPanel)
};

} // namespace trackman
