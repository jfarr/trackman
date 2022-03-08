#include "SampleThumbnail.h"
#include "common/listutil.h"

SampleThumbnail::SampleThumbnail(
    Track &track, Sample &sample, juce::AudioTransportSource &transport, juce::AudioFormatManager &formatManager)
    : track(track), sample(sample), transport(transport), thumbnailCache(5),
      thumbnail(512, formatManager, thumbnailCache) {
    thumbnail.setSource(new juce::FileInputSource(sample.getFile()));
    createControls();
    setSize(200, 81);
}

void SampleThumbnail::createControls() {
    filenameLabel.setText(sample.getFile().getFileName(), juce::dontSendNotification);
    thumbnail.setSource(new juce::FileInputSource(sample.getFile()));
    addAndMakeVisible(filenameLabel);
    if (sample.getSource() == nullptr) {
        missingFileLabel.setText("no file", juce::dontSendNotification);
        missingFileLabel.setFont(missingFileLabel.getFont().withHeight(10));
        missingFileLabel.setColour(juce::Label::textColourId, juce::Colours::grey);
        addAndMakeVisible(missingFileLabel);
    }
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

    auto alpha = 0.8f;
    auto bgcolor = track.isSelected() && sample.isSelected() ? juce::Colours::lightgrey : juce::Colours::dimgrey;
    g.fillAll(bgcolor.withAlpha(alpha));
    g.setColour(juce::Colours::grey);
    g.drawRect(0, 0, getWidth(), getHeight());

    auto thumbnailBounds = area.reduced(margin);
    g.setColour(juce::Colours::darkgrey.withAlpha(alpha));
    g.fillRect(thumbnailBounds);
    g.setColour(juce::Colours::green.withAlpha(alpha));
    thumbnail.drawChannels(g, thumbnailBounds, 0.0, thumbnail.getTotalLength(), 1.0f);
}

void SampleThumbnail::paintOverlay(juce::Graphics &g) {
    auto duration = (float)transport.getLengthInSeconds();

    if (duration > 0.0) {
        auto audioPosition = (float)transport.getCurrentPosition();
        if (audioPosition >= sample.getStartPos() && audioPosition <= sample.getEndPos()) {
            auto samplePos = audioPosition - sample.getStartPos();
            auto drawPosition = samplePos * scale;
            g.setColour(juce::Colour{0xff282828});
            g.drawLine(drawPosition, 0.0f, drawPosition, (float)getHeight(), 1.0f);
        }
    }
}

void SampleThumbnail::resized() {
    auto area = getLocalBounds();
    auto labelHeight = 18;
    auto margin = 2;
    filenameLabel.setBounds(area.removeFromTop(labelHeight).reduced(margin));
    if (sample.getSource() == nullptr) {
        missingFileLabel.centreWithSize(40, 15);
    }
}

void SampleThumbnail::mouseDrag(const juce::MouseEvent &event) {
    auto *container = juce::DragAndDropContainer::findParentDragContainerFor(this);
    auto pos = juce::Point<int>(-event.getPosition().getX(), -event.getPosition().getY());
    if (container != nullptr) {
        container->startDragging("clip", this, *getImage(), false, &pos);
    }
}

void SampleThumbnail::mouseDown(const juce::MouseEvent &event) {
    Component::mouseDown(event);
    notifySampleSelected(track, sample);
}

void SampleThumbnail::addListener(SampleListener *listener) {
    if (!listContains(sampleListeners, listener)) {
        sampleListeners.push_front(listener);
    }
}

void SampleThumbnail::removeListener(SampleListener *listener) { sampleListeners.remove(listener); }

void SampleThumbnail::notifySampleSelected(Track &track, Sample &selected) {
    for (SampleListener *listener : sampleListeners) {
        listener->sampleSelected(track, selected);
    }
}
