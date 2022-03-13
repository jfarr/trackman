#pragma once

#include <JuceHeader.h>

#include "SampleListener.h"
#include "model/Project.h"
#include "model/Sample.h"
#include "ui/common/ThumbnailComponent.h"

class SampleThumbnail;

class StretchHandle : public juce::Component {
  public:
    StretchHandle(SampleThumbnail &thumbnail)
        : thumbnail(thumbnail), dragImage(juce::Image::ARGB, 1, 1, true), scaledDragImage(dragImage) {}
    ~StretchHandle() {}

    SampleThumbnail &getThumbnail() { return thumbnail; }

    //==============================================================================
    // Component
    void paint(juce::Graphics &g) override;
    void mouseDrag(const juce::MouseEvent &event) override;
    void mouseDown(const juce::MouseEvent &event) override;
    void mouseUp(const juce::MouseEvent &event) override;

  private:
    SampleThumbnail &thumbnail;
    juce::Image dragImage;
    juce::ScaledImage scaledDragImage;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StretchHandle)
};

class SampleThumbnail : public juce::Component {
  public:
    SampleThumbnail(Project &project, Track &track, Sample &sample, juce::AudioTransportSource &transport,
        juce::AudioFormatManager &formatManager);
    ~SampleThumbnail() {}

    Track &getTrack() { return track; }
    Sample &getSample() { return sample; }

    void addListener(SampleListener *listener);
    void removeListener(SampleListener *listener);

    //==============================================================================
    // Component
    void paint(juce::Graphics &g) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent &event) override;
    void mouseUp(const juce::MouseEvent &event) override;
    void mouseDrag(const juce::MouseEvent &event) override;

  private:
    Project &project;
    Track &track;
    Sample &sample;
    juce::AudioTransportSource &transport;

    juce::Label filenameLabel;
    juce::Label missingFileLabel;
    StretchHandle stretchHandle;
    bool dragging = false;
    int xPos = 0;
    juce::Image dragImage;
    juce::ScaledImage scaledDragImage;

    // TODO: make thumbnailCache a singleton
    juce::AudioThumbnailCache thumbnailCache;
    juce::AudioThumbnail thumbnail;
    std::unique_ptr<juce::Image> image;
    std::unique_ptr<juce::ScaledImage> scaledImage;
    std::list<SampleListener *> sampleListeners;

    void createControls();
    void paintWithoutOverlay(juce::Graphics &g);
    void paintOverlay(juce::Graphics &g);

    void notifySampleSelected(Track &track, Sample &selected);
    void notifyMouseDragged(SampleThumbnail &thumbnail, int x, int y);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SampleThumbnail)
};
