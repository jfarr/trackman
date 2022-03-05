#pragma once

#include <JuceHeader.h>

#include "model/Sample.h"
#include "controls/common/ThumbnailComponent.h"

class SampleThumbnail : public juce::Component {
  public:
    SampleThumbnail(Sample &sample, juce::AudioFormatManager &formatManager);
    ~SampleThumbnail() {}

    Sample &getSample() { return sample; }

    //==============================================================================
    // Component
    void paint(juce::Graphics &g) override;
    void resized() override;

  private:
    Sample &sample;
    juce::Label filenameLabel;
    ThumbnailComponent thumbnail;

    void createControls();
};
