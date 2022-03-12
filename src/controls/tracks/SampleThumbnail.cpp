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
    if (!dragging) {
        dragging = true;
        auto *container = juce::DragAndDropContainer::findParentDragContainerFor(this);
        if (container != nullptr) {
            container->startDragging("clip", this, scaledDragImage);
        }
    } else {
        auto bounds = thumbnail.getScreenBounds();
        auto x = event.getScreenX();
        auto newWidth = std::max(x - bounds.getX(), 1);
        thumbnail.setBounds(thumbnail.getBounds().withWidth(newWidth));
    }
}

void StretchHandle::mouseDown(const juce::MouseEvent &event) { thumbnail.mouseDown(event); }

void StretchHandle::mouseUp(const juce::MouseEvent &event) { dragging = false; }

SampleThumbnail::SampleThumbnail(Project &project, Track &track, Sample &sample, juce::AudioTransportSource &transport,
    juce::AudioFormatManager &formatManager)
    : project(project), track(track), sample(sample), transport(transport), thumbnailCache(5),
      thumbnail(512, formatManager, thumbnailCache), stretchHandle(*this) {
    thumbnail.setSource(new juce::FileInputSource(sample.getFile()));
    createControls();
    setSize(200, 81);
}

void SampleThumbnail::createControls() {
    filenameLabel.setText(sample.getFile().getFileName(), juce::dontSendNotification);
    thumbnail.setSource(new juce::FileInputSource(sample.getFile()));
    addAndMakeVisible(filenameLabel);
    addAndMakeVisible(stretchHandle);
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

void SampleThumbnail::paintOverlay(juce::Graphics &g) {
    auto duration = (float)transport.getLengthInSeconds();

    if (duration > 0.0) {
        auto audioPosition = (float)transport.getCurrentPosition();
        if (audioPosition >= sample.getStartPos() && audioPosition <= sample.getEndPos()) {
            auto samplePos = audioPosition - sample.getStartPos();
            auto drawPosition = samplePos * project.getHorizontalScale();
            g.setColour(juce::Colour{0xff282828});
            g.drawLine(drawPosition, 0.0f, drawPosition, (float)getHeight(), 1.0f);
        }
    }
}

void SampleThumbnail::resized() {
    auto area = getLocalBounds();
    auto labelHeight = 18;
    auto margin = 2;
    auto handleSize = 12;
    stretchHandle.setBounds(getWidth() - handleSize, 0, handleSize, handleSize);
    filenameLabel.setBounds(area.removeFromTop(labelHeight).reduced(margin));
    if (sample.getSource() == nullptr) {
        missingFileLabel.centreWithSize(40, 15);
    }
}

void SampleThumbnail::mouseDown(const juce::MouseEvent &event) {
    Component::mouseDown(event);
    notifySampleSelected(track, sample);
}

void SampleThumbnail::mouseDrag(const juce::MouseEvent &event) {
    auto *container = juce::DragAndDropContainer::findParentDragContainerFor(this);
    auto pos = juce::Point<int>(-event.getPosition().getX(), -event.getPosition().getY());
    if (container != nullptr) {
        container->startDragging("clip", this, *getImage(), false, &pos);
    }
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
