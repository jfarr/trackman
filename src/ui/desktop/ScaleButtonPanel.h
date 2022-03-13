#pragma once

#include <JuceHeader.h>

#include "TrackScaleListener.h"

class ScaleButtonPanel : public juce::Component {
  public:
    ScaleButtonPanel(bool vertical);
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
    bool vertical;
    std::list<TrackScaleListener *> listeners;

    void minusButtonClicked();
    void plusButtonClicked();

    void notifyScaleIncreased();
    void notifyScaleDecreased();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ScaleButtonPanel)
};
