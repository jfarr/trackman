#pragma once

#include "InstrumentControl.h"
#include "ui/desktop/TrackListListener.h"
#include <JuceHeader.h>

using namespace juce;

namespace trackman {

class InstrumentsController;

class InstrumentController : public MouseListener {
  public:
    explicit InstrumentController(InstrumentsController &instrumentsController, Track &track);
    ~InstrumentController() override = default;

    InstrumentControl &getInstrumentControl() { return instrumentControl; }

    void update();
    void repaint();

    void addListener(TrackListListener *listener);
    void removeListener(TrackListListener *listener);

    //==============================================================================
    // MouseListener
    void mouseDown(const MouseEvent &event) override;

  private:
    Track &track;
    InstrumentControl instrumentControl;
    list<TrackListListener *> trackListListeners;

    void notifySelectionChanged();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(InstrumentController)
};

}
