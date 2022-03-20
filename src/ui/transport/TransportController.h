#pragma once

#include <JuceHeader.h>

#include "TransportPanel.h"

class DesktopController;

namespace trackman {

class TransportController {
  public:
    TransportController(DesktopController &desktopController);
    ~TransportController() = default;

  private:
    DesktopController &desktopController;
    TransportPanel transportPanel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TransportController)
};

} // namespace trackman
