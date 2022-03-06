#include "PositionOverlay.h"

PositionOverlay::PositionOverlay(juce::AudioTransportSource &transportSource) : transportSource(transportSource) {
    startTimer(20);
}

void PositionOverlay::paint(juce::Graphics &g) {
    auto duration = (float)transportSource.getLengthInSeconds();

    if (duration > 0.0) {
        auto audioPosition = (float)transportSource.getCurrentPosition();
        auto drawPosition = (audioPosition / duration) * (float)getWidth();

        g.setColour(juce::Colour{0xff282828});
        g.drawLine(drawPosition, 0.0f, drawPosition, (float)getHeight(), 1.0f);
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
