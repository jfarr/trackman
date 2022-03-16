#pragma once

#include <JuceHeader.h>
#include "InstrumentControl.h"

class InstrumentController {
  public:
    InstrumentController() = default;
    ~InstrumentController() = default;

    InstrumentControl &getInstrumentControl() { return instrumentControl; }

    void update();
    void repaint();

  private:
    InstrumentControl instrumentControl;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(InstrumentController)
};
