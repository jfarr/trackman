#include "ThumbnailComponent.h"

namespace trackman {

ThumbnailComponent::ThumbnailComponent(AudioFormatManager &formatManager)
    : thumbnailCache(5), thumbnail(512, formatManager, thumbnailCache) {
    thumbnail.addChangeListener(this);
}

ThumbnailComponent::~ThumbnailComponent() {}

void ThumbnailComponent::setSource(const File &file) { thumbnail.setSource(new FileInputSource(file)); }

void ThumbnailComponent::paint(Graphics &g) {
    Rectangle<int> thumbnailBounds = getLocalBounds();
    if (thumbnail.getNumChannels() == 0) {
        paintIfNoFileLoaded(g, thumbnailBounds);
    } else {
        paintIfFileLoaded(g, thumbnailBounds);
    }
}

void ThumbnailComponent::paintIfNoFileLoaded(Graphics &g, const Rectangle<int> &thumbnailBounds) {
    g.setColour(Colours::dimgrey);
    g.fillRect(thumbnailBounds);
    g.setColour(Colours::white);
    g.drawFittedText("No File Loaded", thumbnailBounds, Justification::centred, 1);
}

void ThumbnailComponent::paintIfFileLoaded(Graphics &g, const Rectangle<int> &thumbnailBounds) {
    g.setColour(Colours::dimgrey);
    g.fillRect(thumbnailBounds);
    g.setColour(Colours::limegreen);
    thumbnail.drawChannels(g, thumbnailBounds, 0.0, thumbnail.getTotalLength(), 1.0f);
}

void ThumbnailComponent::changeListenerCallback(ChangeBroadcaster *source) {
    if (source == &thumbnail) {
        thumbnailChanged();
    }
}

void ThumbnailComponent::thumbnailChanged() { repaint(); }

} // namespace trackman
