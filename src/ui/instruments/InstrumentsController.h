#pragma once

#include <JuceHeader.h>

#include "InstrumentController.h"
#include "InstrumentsPanel.h"
#include "model/Project.h"

using namespace std;

namespace trackman {

class DesktopController;

class InstrumentsController : public TrackListListener {
  public:
    InstrumentsController(DesktopController &desktopController);
    ~InstrumentsController();

    InstrumentsPanel &getInstrumentsPanel() { return instrumentsPanel; }

    void update();
    void repaint();

    //==============================================================================
    // TrackListListener
    void selectionChanged(Track *track) override;

  private:
    DesktopController &desktopController;
    Project &project;

    InstrumentsPanel instrumentsPanel;
    list<unique_ptr<InstrumentController>> instruments;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(InstrumentsController)
};

} // namespace trackman
