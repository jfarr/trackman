#pragma once

#include <JuceHeader.h>
#include "InstrumentControl.h"

class InstrumentController {
  public:
    InstrumentController(Track &track) : track(track), instrumentControl(track) {}
    ~InstrumentController() = default;

    InstrumentControl &getInstrumentControl() { return instrumentControl; }

    void update();
    void repaint();

  private:
    Track &track;
    InstrumentControl instrumentControl;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(InstrumentController)
};
