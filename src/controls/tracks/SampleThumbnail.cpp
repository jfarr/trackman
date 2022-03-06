#include "SampleThumbnail.h"

SampleThumbnail::SampleThumbnail(
    Sample &sample, juce::AudioTransportSource &transport, juce::AudioFormatManager &formatManager)
    : sample(sample), transport(transport), thumbnailCache(5), thumbnail(512, formatManager, thumbnailCache) {
    thumbnail.setSource(new juce::FileInputSource(sample.getFile()));
    createControls();
    setSize(200, 85);
}

void SampleThumbnail::createControls() {
    filenameLabel.setText(sample.getFile().getFileName(), juce::dontSendNotification);
    thumbnail.setSource(new juce::FileInputSource(sample.getFile()));
    addAndMakeVisible(filenameLabel);
}

juce::ScaledImage *SampleThumbnail::getImage() {
    image = std::make_unique<juce::Image>(juce::Image::ARGB, getWidth(), getHeight(), false);
    juce::Graphics g(*image);
    paintWithoutOverlay(g);
    scaledImage = std::make_unique<juce::ScaledImage>(*image);
    return scaledImage.get();
}

void SampleThumbnail::paint(juce::Graphics &g) {
    paintWithoutOverlay(g);
    paintOverlay(g);
}

void SampleThumbnail::paintWithoutOverlay(juce::Graphics &g) {
    auto area = getLocalBounds();
    auto margin = 3;

    g.fillAll(juce::Colours::dimgrey);
    g.setColour(juce::Colours::grey);
    g.drawRect(0, 0, getWidth(), getHeight());

    auto thumbnailBounds = area.reduced(margin);
    g.setColour(juce::Colours::darkgrey);
    g.fillRect(thumbnailBounds);
    g.setColour(juce::Colours::green);
    thumbnail.drawChannels(g, thumbnailBounds, 0.0, thumbnail.getTotalLength(), 1.0f);
}

void SampleThumbnail::paintOverlay(juce::Graphics &g) {
    auto duration = (float)transport.getLengthInSeconds();

    if (duration > 0.0) {
        auto audioPosition = (float)transport.getCurrentPosition();
        if (audioPosition >= sample.getStartPos() && audioPosition <= sample.getEndPos()) {
            auto samplePos = audioPosition - sample.getStartPos();
            auto drawPosition = samplePos * scale;
            g.setColour(juce::Colours::yellowgreen);
            g.drawLine(drawPosition, 0.0f, drawPosition, (float)getHeight(), 2.0f);
        }
    }
}

void SampleThumbnail::resized() {
    auto area = getLocalBounds();
    auto labelHeight = 18;
    auto margin = 2;
    filenameLabel.setBounds(area.removeFromTop(labelHeight).reduced(margin));
}

void SampleThumbnail::mouseDrag(const juce::MouseEvent &event) {
    auto *container = juce::DragAndDropContainer::findParentDragContainerFor(this);
    auto pos = juce::Point<int>(-event.getPosition().getX(), -event.getPosition().getY());
    if (container != nullptr) {
        container->startDragging("clip", this, *getImage(), false, &pos);
    }
}
