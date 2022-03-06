#pragma once

#include <JuceHeader.h>

#include "model/Sample.h"
#include "controls/common/ThumbnailComponent.h"

class SampleThumbnail : public juce::Component {
  public:
    SampleThumbnail(Sample &sample, juce::AudioTransportSource &transport, juce::AudioFormatManager &formatManager);
    ~SampleThumbnail() {}

    Sample &getSample() { return sample; }

    //==============================================================================
    // Component
    void paint(juce::Graphics &g) override;
    void resized() override;

  private:
    Sample &sample;
    juce::AudioTransportSource &transport;
    juce::Label filenameLabel;
    // TODO: make thumbnailCache a singleton
    juce::AudioThumbnailCache thumbnailCache;
    juce::AudioThumbnail thumbnail;
    float scale = 75;

    void createControls();
};
