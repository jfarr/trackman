#pragma once

#include <JuceHeader.h>

using namespace std;
using namespace juce;

namespace trackman {

class ThumbnailComponent : public Component, public ChangeListener {
  public:
    ThumbnailComponent(AudioFormatManager &formatManager);
    ~ThumbnailComponent();

    void setSource(const File &file);

    //==============================================================================
    // Component
    void paint(Graphics &g) override;

    //==============================================================================
    // ChangeListener
    void changeListenerCallback(ChangeBroadcaster *source) override;

  private:
    AudioThumbnailCache thumbnailCache;
    AudioThumbnail thumbnail;

    void paintIfNoFileLoaded(Graphics &g, const Rectangle<int> &thumbnailBounds);
    void paintIfFileLoaded(Graphics &g, const Rectangle<int> &thumbnailBounds);
    void thumbnailChanged();
};

} // namespace trackman
