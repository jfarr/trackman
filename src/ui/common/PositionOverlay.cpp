#include "PositionOverlay.h"

PositionOverlay::PositionOverlay(juce::AudioTransportSource &transportSource, Project *project) : transportSource(transportSource), project(project) {
    if (project != nullptr) {
        setInterceptsMouseClicks(false, true);
    }
    startTimer(20);
}

void PositionOverlay::paint(juce::Graphics &g) {
    g.setColour(juce::Colour{0xff282828});
    if (project == nullptr) {
        auto duration = (float)transportSource.getLengthInSeconds();
        if (duration > 0.0) {
            auto audioPosition = (float)transportSource.getCurrentPosition();
            auto drawPosition = (audioPosition / duration) * (float)getWidth();
            g.drawLine(drawPosition, 0.0f, drawPosition, (float)getHeight(), 1.0f);
        } else {
            g.drawLine(0.0f, 0.0f, 0.0f, (float)getHeight(), 1.0f);
        }
    } else {
        auto leftPanelWidth = 25;
        auto laneWidth = getParentComponent()->getWidth() - leftPanelWidth;
        auto duration = laneWidth / project->getHorizontalScale();
        if (duration > 0.0) {
            auto audioPosition = (float)transportSource.getCurrentPosition();
            auto drawPosition = (audioPosition / duration) * laneWidth;
            g.drawLine(drawPosition, 0.0f, drawPosition, (float)getHeight(), 1.0f);
        } else {
            g.drawLine(0.0f, 0.0f, 0.0f, (float)getHeight(), 1.0f);
        }
    }
}

void PositionOverlay::mouseDown(const juce::MouseEvent &event) {
    auto duration = transportSource.getLengthInSeconds();

    if (duration > 0.0) {
        auto clickPosition = event.position.x;
        auto audioPosition = (clickPosition / (float)getWidth()) * duration;

        transportSource.setPosition(audioPosition);
    }
    Component::mouseDown(event);
}

void PositionOverlay::timerCallback() { repaint(); }
