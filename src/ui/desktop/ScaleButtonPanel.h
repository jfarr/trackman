#pragma once

#include <JuceHeader.h>

#include "TrackScaleListener.h"

using namespace std;
using namespace juce;

namespace trackman {

class ScaleButtonPanel : public Component {
  public:
    ScaleButtonPanel(bool vertical);
    ~ScaleButtonPanel() override = default;

    void addListener(TrackScaleListener *listener);
    void removeListener(TrackScaleListener *listener);

    //==============================================================================
    // Component
    void paint(Graphics &g) override;
    void resized() override;

  private:
    TextButton minusButton;
    TextButton plusButton;
    bool vertical;
    list<TrackScaleListener *> listeners;

    void minusButtonClicked();
    void plusButtonClicked();

    void notifyScaleIncreased();
    void notifyScaleDecreased();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ScaleButtonPanel)
};

} // namespace trackman
