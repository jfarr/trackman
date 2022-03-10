#pragma once

#include <JuceHeader.h>

#include "SampleListener.h"
#include "controls/common/ThumbnailComponent.h"
#include "model/Sample.h"

class StretchHandle : public juce::Component {
  public:
    StretchHandle(juce::Component &thumbnail)
        : thumbnail(thumbnail), dragImage(juce::Image::ARGB, 1, 1, true), scaledDragImage(dragImage) {}
    ~StretchHandle() {}

    //==============================================================================
    // Component
    void paint(juce::Graphics &g) override;
    void mouseDrag(const juce::MouseEvent &event) override;
    void mouseUp(const juce::MouseEvent &event) override;

  private:
    juce::Component &thumbnail;
    juce::Image dragImage;
    juce::ScaledImage scaledDragImage;
    bool dragging = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StretchHandle)
};

class SampleThumbnail : public juce::Component {
  public:
    SampleThumbnail(
        Track &track, Sample &sample, juce::AudioTransportSource &transport, juce::AudioFormatManager &formatManager);
    ~SampleThumbnail() {}

    Sample &getSample() { return sample; }

    void addListener(SampleListener *listener);
    void removeListener(SampleListener *listener);

    //==============================================================================
    // Component
    void paint(juce::Graphics &g) override;
    void resized() override;
    void mouseDrag(const juce::MouseEvent &event) override;
    void mouseDown(const juce::MouseEvent &event) override;

  private:
    Track &track;
    Sample &sample;
    juce::AudioTransportSource &transport;
    juce::Label filenameLabel;
    juce::Label missingFileLabel;
    StretchHandle stretchHandle;

    // TODO: make thumbnailCache a singleton
    juce::AudioThumbnailCache thumbnailCache;
    juce::AudioThumbnail thumbnail;
    std::unique_ptr<juce::Image> image;
    std::unique_ptr<juce::ScaledImage> scaledImage;
    float scale = 75;
    std::list<SampleListener *> sampleListeners;

    void createControls();
    void paintWithoutOverlay(juce::Graphics &g);
    void paintOverlay(juce::Graphics &g);
    juce::ScaledImage *getImage();

    void notifySampleSelected(Track &track, Sample &selected);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SampleThumbnail)
};
