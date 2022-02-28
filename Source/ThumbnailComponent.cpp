#include "ThumbnailComponent.h"

ThumbnailComponent::ThumbnailComponent(juce::AudioFormatManager &formatManager)
    : thumbnailCache(5), thumbnail(512, formatManager, thumbnailCache) {
    thumbnail.addChangeListener(this);
}

ThumbnailComponent::~ThumbnailComponent() {}

void ThumbnailComponent::setSource(const juce::File &file) { thumbnail.setSource(new juce::FileInputSource(file)); }

void ThumbnailComponent::paint(juce::Graphics &g) {
    juce::Rectangle<int> thumbnailBounds = getLocalBounds();
    if (thumbnail.getNumChannels() == 0)
        paintIfNoFileLoaded(g, thumbnailBounds);
    else
        paintIfFileLoaded(g, thumbnailBounds);
}

void ThumbnailComponent::paintIfNoFileLoaded(juce::Graphics &g, const juce::Rectangle<int> &thumbnailBounds) {
    g.setColour(juce::Colours::darkgrey);
    g.fillRect(thumbnailBounds);
    g.setColour(juce::Colours::white);
    g.drawFittedText("No File Loaded", thumbnailBounds, juce::Justification::centred, 1);
}

void ThumbnailComponent::paintIfFileLoaded(juce::Graphics &g, const juce::Rectangle<int> &thumbnailBounds) {
    g.setColour(juce::Colours::darkgrey);
    g.fillRect(thumbnailBounds);

    g.setColour(juce::Colours::green);

    thumbnail.drawChannels(g, thumbnailBounds,
        0.0,                        // start time
        thumbnail.getTotalLength(), // end time
        1.0f);                      // vertical zoom
}

void ThumbnailComponent::changeListenerCallback(juce::ChangeBroadcaster *source) {
    if (source == &thumbnail)
        thumbnailChanged();
}

void ThumbnailComponent::thumbnailChanged() { repaint(); }

//==============================================================================

PositionOverlay::PositionOverlay(juce::AudioTransportSource &transportSource) : transportSource(transportSource) {
    startTimer(20);
}

void PositionOverlay::paint(juce::Graphics &g) {
    auto duration = (float)transportSource.getLengthInSeconds();

    if (duration > 0.0) {
        auto audioPosition = (float)transportSource.getCurrentPosition();
        auto drawPosition = (audioPosition / duration) * (float)getWidth();

        g.setColour(juce::Colours::yellowgreen);
        g.drawLine(drawPosition, 0.0f, drawPosition, (float)getHeight(), 2.0f);
    }
}

void PositionOverlay::mouseDown(const juce::MouseEvent &event) {
    auto duration = transportSource.getLengthInSeconds();

    if (duration > 0.0) {
        auto clickPosition = event.position.x;
        auto audioPosition = (clickPosition / (float)getWidth()) * duration;

        transportSource.setPosition(audioPosition);
    }
}

void PositionOverlay::timerCallback() { repaint(); }
