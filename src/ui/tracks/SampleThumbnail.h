#pragma once

#include <JuceHeader.h>

#include "model/Project.h"
#include "model/Sample.h"
#include "ui/common/ThumbnailComponent.h"

using namespace std;
using namespace juce;

namespace trackman {

class SampleThumbnail;

class StretchHandle : public Component {
  public:
    StretchHandle(SampleThumbnail &thumbnail)
        : thumbnail(thumbnail), dragImage(Image::ARGB, 1, 1, true), scaledDragImage(dragImage) {}
    ~StretchHandle() {}

    SampleThumbnail &getThumbnail() { return thumbnail; }

    //==============================================================================
    // Component
    void paint(Graphics &g) override;
    void mouseDrag(const MouseEvent &event) override;
    void mouseDown(const MouseEvent &event) override;
    void mouseUp(const MouseEvent &event) override;

  private:
    SampleThumbnail &thumbnail;
    Image dragImage;
    ScaledImage scaledDragImage;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StretchHandle)
};

class SampleThumbnail : public Component {
  public:
    SampleThumbnail(Project &project, Track &track, Sample &sample, AudioFormatManager &formatManager);
    ~SampleThumbnail() {}

    Track &getTrack() { return track; }
    Sample &getSample() { return sample; }

    function<void(Track &, Sample &)> onSelected = nullptr;
    function<void(SampleThumbnail &, int, int)> onMouseDragged = nullptr;

    //==============================================================================
    // Component
    void paint(Graphics &g) override;
    void resized() override;
    void mouseDown(const MouseEvent &event) override;
    void mouseUp(const MouseEvent &event) override;
    void mouseDrag(const MouseEvent &event) override;

  private:
    Project &project;
    Track &track;
    Sample &sample;

    Label filenameLabel;
    Label missingFileLabel;
    StretchHandle stretchHandle;
    bool dragging = false;
    int xPos = 0;
    Image dragImage;
    ScaledImage scaledDragImage;

    // TODO: make thumbnailCache a singleton
    AudioThumbnailCache thumbnailCache;
    AudioThumbnail thumbnail;

    void createControls();

    void notifySelected(Track &track, Sample &selected) const;
    void notifyMouseDragged(SampleThumbnail &thumbnail, int x, int screenY) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SampleThumbnail)
};

}
