#include "SampleThumbnail.h"
#include "common/listutil.h"

namespace trackman {

SampleThumbnail::SampleThumbnail(Project &project, Track &track, Sample &sample, AudioFormatManager &formatManager)
    : project(project), track(track), sample(sample), thumbnailCache(5), thumbnail(512, formatManager, thumbnailCache),
      stretchHandle(*this), dragImage(Image::ARGB, 1, 1, true), scaledDragImage(dragImage) {
    thumbnail.setSource(new FileInputSource(sample.getFile()));
    createControls();
    setSize(200, 81);
}

void SampleThumbnail::createControls() {
    filenameLabel.setText(sample.getFile().getFileName(), dontSendNotification);
    filenameLabel.setFont(missingFileLabel.getFont().withHeight(10));
    thumbnail.setSource(new FileInputSource(sample.getFile()));
    addAndMakeVisible(filenameLabel);
    addAndMakeVisible(stretchHandle);
    if (!sample.isLoaded()) {
        missingFileLabel.setText("no file", dontSendNotification);
        missingFileLabel.setFont(missingFileLabel.getFont().withHeight(10));
        missingFileLabel.setColour(Label::textColourId, Colours::grey);
        addAndMakeVisible(missingFileLabel);
    }
}

void SampleThumbnail::paint(Graphics &g) {
    auto area = getLocalBounds();
    auto margin = 2;

    auto bgcolor = track.isSelected() && sample.isSelected() ? Colours::lightgrey : Colours::dimgrey;
    g.fillAll(bgcolor);
    g.setColour(Colours::grey);
    g.drawRect(0, 0, getWidth(), getHeight());

    auto thumbnailBounds = area.reduced(margin);
    g.setColour(Colours::dimgrey);
    g.fillRect(thumbnailBounds);
    g.setColour(Colours::limegreen);
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

void SampleThumbnail::mouseDown(const MouseEvent &event) {
    Component::mouseDown(event);
    notifySelected(track, sample);
    if (!dragging) {
        dragging = true;
        xPos = getPosition().getX();
    }
}

void SampleThumbnail::mouseUp(const MouseEvent &event) { dragging = false; }

void SampleThumbnail::mouseDrag(const MouseEvent &event) {
    auto *container = DragAndDropContainer::findParentDragContainerFor(this);
    if (container != nullptr) {
        container->startDragging("clip", this, scaledDragImage);
    }
    auto d = event.getDistanceFromDragStartX();
    notifyMouseDragged(*this, xPos + d, event.getScreenY());
}

void SampleThumbnail::notifySelected(Track &track, Sample &selected) const {
    if (onSelected != nullptr) {
        onSelected(track, selected);
    }
}

void SampleThumbnail::notifyMouseDragged(SampleThumbnail &thumbnail, int x, int screenY) const {
    if (onMouseDragged != nullptr) {
        onMouseDragged(thumbnail, x, screenY);
    }
}

void StretchHandle::paint(Graphics &g) {
    Path path;
    auto w = getWidth();
    auto h = getHeight();
    path.addTriangle(Point<float>(0, 0), Point<float>(w, 0), Point<float>(w, h));
    g.setColour(Colours::grey.withAlpha(0.8f));
    g.fillPath(path);
}

void StretchHandle::mouseDrag(const MouseEvent &event) {
    auto *container = DragAndDropContainer::findParentDragContainerFor(this);
    if (container != nullptr) {
        container->startDragging("sample", this, scaledDragImage);
    }
    auto bounds = thumbnail.getScreenBounds();
    auto x = event.getScreenX();
    auto newWidth = max(x - bounds.getX(), 1);
    thumbnail.setBounds(thumbnail.getBounds().withWidth(newWidth));
}

void StretchHandle::mouseDown(const MouseEvent &event) { thumbnail.mouseDown(event); }

void StretchHandle::mouseUp(const MouseEvent &event) {}

} // namespace trackman
