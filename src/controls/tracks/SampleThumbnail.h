#pragma once

#include <JuceHeader.h>

#include "model/Sample.h"

class SampleThumbnail : public juce::Component {
  public:
    SampleThumbnail(Sample &sample);
    ~SampleThumbnail() {}

    Sample &getSample() { return sample; }

    //==============================================================================
    // Component
    void paint(juce::Graphics &g) override;
    void resized() override;

  private:
    Sample &sample;
};
