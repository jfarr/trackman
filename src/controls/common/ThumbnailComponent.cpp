#include "ThumbnailComponent.h"

ThumbnailComponent::ThumbnailComponent(juce::AudioFormatManager &formatManager)
    : thumbnailCache(5), thumbnail(512, formatManager, thumbnailCache) {
    thumbnail.addChangeListener(this);
}

ThumbnailComponent::~ThumbnailComponent() {}

void ThumbnailComponent::setSource(const juce::File &file) { thumbnail.setSource(new juce::FileInputSource(file)); }

void ThumbnailComponent::paint(juce::Graphics &g) {
    juce::Rectangle<int> thumbnailBounds = getLocalBounds();
    if (thumbnail.getNumChannels() == 0) {
        paintIfNoFileLoaded(g, thumbnailBounds);
    } else {
        paintIfFileLoaded(g, thumbnailBounds);
    }
}

void ThumbnailComponent::paintIfNoFileLoaded(juce::Graphics &g, const juce::Rectangle<int> &thumbnailBounds) {
    g.setColour(juce::Colours::dimgrey);
    g.fillRect(thumbnailBounds);
    g.setColour(juce::Colours::white);
    g.drawFittedText("No File Loaded", thumbnailBounds, juce::Justification::centred, 1);
}

void ThumbnailComponent::paintIfFileLoaded(juce::Graphics &g, const juce::Rectangle<int> &thumbnailBounds) {
    g.setColour(juce::Colours::dimgrey);
    g.fillRect(thumbnailBounds);
    g.setColour(juce::Colours::limegreen);
    thumbnail.drawChannels(g, thumbnailBounds, 0.0, thumbnail.getTotalLength(), 1.0f);
}

void ThumbnailComponent::changeListenerCallback(juce::ChangeBroadcaster *source) {
    if (source == &thumbnail) {
        thumbnailChanged();
    }
}

void ThumbnailComponent::thumbnailChanged() { repaint(); }
