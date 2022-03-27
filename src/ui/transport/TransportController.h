#pragma once

#include <JuceHeader.h>

#include "TransportPanel.h"

namespace trackman {

class DesktopController;

class TransportController {
  public:
    TransportController(DesktopController &desktopController);
    ~TransportController() = default;

    TransportPanel &getTransportPanel() { return transportPanel; }

    void selectionChanged(Track *track);

    void update();

  private:
    DesktopController &desktopController;
    TransportPanel transportPanel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TransportController)
};

} // namespace trackman
