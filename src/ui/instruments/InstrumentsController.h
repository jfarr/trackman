#pragma once

#include <JuceHeader.h>

#include "InstrumentController.h"
#include "InstrumentsPanel.h"
#include "model/Project.h"

class DesktopController;

class InstrumentsController {
  public:
    InstrumentsController(DesktopController &desktopController);
    ~InstrumentsController();

    InstrumentsPanel& getInstrumentsPanel() { return instrumentsPanel; }

    void update();
    void repaint();

  private:
    DesktopController &desktopController;
    Project &project;

    InstrumentsPanel instrumentsPanel;
    std::list<std::unique_ptr<InstrumentController>> instruments;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(InstrumentsController)
};
