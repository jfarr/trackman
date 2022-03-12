#pragma once

#include "TrackScaleListener.h"
#include <JuceHeader.h>

class ScaleButtonPanel : public juce::Component {
  public:
    ScaleButtonPanel();
    ~ScaleButtonPanel() override = default;

    void addListener(TrackScaleListener *listener);
    void removeListener(TrackScaleListener *listener);

    //==============================================================================
    // Component
    void paint(juce::Graphics &g) override;
    void resized() override;

  private:
    juce::TextButton minusButton;
    juce::TextButton plusButton;
    std::list<TrackScaleListener *> listeners;

    void minusButtonClicked();
    void plusButtonClicked();

    void notifyScaleIncreased();
    void notifyScaleDecreased();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ScaleButtonPanel)
};
