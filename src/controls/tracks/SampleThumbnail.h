#pragma once

#include <JuceHeader.h>

#include "controls/common/ThumbnailComponent.h"
#include "model/Sample.h"

class SampleThumbnail : public juce::Component {
  public:
    SampleThumbnail(Sample &sample, juce::AudioTransportSource &transport, juce::AudioFormatManager &formatManager);
    ~SampleThumbnail() {}

    Sample &getSample() { return sample; }

    //==============================================================================
    // Component
    void paint(juce::Graphics &g) override;
    void resized() override;
    void mouseDrag(const juce::MouseEvent &event) override;

  private:
    Sample &sample;
    juce::AudioTransportSource &transport;
    juce::Label filenameLabel;
    // TODO: make thumbnailCache a singleton
    juce::AudioThumbnailCache thumbnailCache;
    juce::AudioThumbnail thumbnail;
    std::unique_ptr<juce::Image> image;
    std::unique_ptr<juce::ScaledImage> scaledImage;
    float scale = 75;

    void createControls();
    void paintWithoutOverlay(juce::Graphics &g);
    void paintOverlay(juce::Graphics &g);
    juce::ScaledImage *getImage();
};
