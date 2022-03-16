#include "SampleThumbnail.h"
#include "common/listutil.h"

void StretchHandle::paint(juce::Graphics &g) {
    juce::Path path;
    auto w = getWidth();
    auto h = getHeight();
    path.addTriangle(juce::Point<float>(0, 0), juce::Point<float>(w, 0), juce::Point<float>(w, h));
    g.setColour(juce::Colours::grey.withAlpha(0.8f));
    g.fillPath(path);
}

void StretchHandle::mouseDrag(const juce::MouseEvent &event) {
    auto *container = juce::DragAndDropContainer::findParentDragContainerFor(this);
    if (container != nullptr) {
        container->startDragging("clip", this, scaledDragImage);
    }
    auto bounds = thumbnail.getScreenBounds();
    auto x = event.getScreenX();
    auto newWidth = std::max(x - bounds.getX(), 1);
    thumbnail.setBounds(thumbnail.getBounds().withWidth(newWidth));
}

void StretchHandle::mouseDown(const juce::MouseEvent &event) { thumbnail.mouseDown(event); }

void StretchHandle::mouseUp(const juce::MouseEvent &event) {}

SampleThumbnail::SampleThumbnail(
    Project &project, Track &track, Sample &sample, juce::AudioFormatManager &formatManager)
    : project(project), track(track), sample(sample), thumbnailCache(5), thumbnail(512, formatManager, thumbnailCache),
      stretchHandle(*this), dragImage(juce::Image::ARGB, 1, 1, true), scaledDragImage(dragImage) {
    thumbnail.setSource(new juce::FileInputSource(sample.getFile()));
    createControls();
    setSize(200, 81);
}

void SampleThumbnail::createControls() {
    filenameLabel.setText(sample.getFile().getFileName(), juce::dontSendNotification);
    filenameLabel.setFont(missingFileLabel.getFont().withHeight(10));
    thumbnail.setSource(new juce::FileInputSource(sample.getFile()));
    addAndMakeVisible(filenameLabel);
    addAndMakeVisible(stretchHandle);
    if (!sample.isLoaded()) {
        missingFileLabel.setText("no file", juce::dontSendNotification);
        missingFileLabel.setFont(missingFileLabel.getFont().withHeight(10));
        missingFileLabel.setColour(juce::Label::textColourId, juce::Colours::grey);
        addAndMakeVisible(missingFileLabel);
    }
}

void SampleThumbnail::paint(juce::Graphics &g) {
    auto area = getLocalBounds();
    auto margin = 3;

    auto bgcolor = track.isSelected() && sample.isSelected() ? juce::Colours::lightgrey : juce::Colours::dimgrey;
    g.fillAll(bgcolor);
    g.setColour(juce::Colours::grey);
    g.drawRect(0, 0, getWidth(), getHeight());

    auto thumbnailBounds = area.reduced(margin);
    g.setColour(juce::Colours::dimgrey);
    g.fillRect(thumbnailBounds);
    g.setColour(juce::Colours::limegreen);
    thumbnail.drawChannels(g, thumbnailBounds, 0.0, thumbnail.getTotalLength(), 1.0f);
}

void SampleThumbnail::resized() {
    auto area = getLocalBounds();
    auto labelHeight = 15;
    auto margin = 2;
    auto handleSize = 12;
    stretchHandle.setBounds(getWidth() - handleSize, 0, handleSize, handleSize);
    filenameLabel.setBounds(area.removeFromTop(labelHeight).reduced(margin));
    if (!sample.isLoaded()) {
        missingFileLabel.centreWithSize(40, 15);
    }
}

void SampleThumbnail::mouseDown(const juce::MouseEvent &event) {
    Component::mouseDown(event);
    notifySampleSelected(track, sample);
    if (!dragging) {
        dragging = true;
        xPos = getPosition().getX();
    }
}

void SampleThumbnail::mouseUp(const juce::MouseEvent &event) { dragging = false; }

void SampleThumbnail::mouseDrag(const juce::MouseEvent &event) {
    auto *container = juce::DragAndDropContainer::findParentDragContainerFor(this);
    if (container != nullptr) {
        container->startDragging("clip", this, scaledDragImage);
    }
    auto d = event.getDistanceFromDragStartX();
    notifyMouseDragged(*this, xPos + d, event.getScreenY());
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

void SampleThumbnail::notifyMouseDragged(SampleThumbnail &thumbnail, int x, int y) {
    for (SampleListener *listener : sampleListeners) {
        listener->mouseDragged(thumbnail, x, y);
    }
}
