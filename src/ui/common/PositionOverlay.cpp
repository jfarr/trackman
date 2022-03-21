#include "PositionOverlay.h"

namespace trackman {

PositionOverlay::PositionOverlay(juce::AudioTransportSource &transportSource) : transportSource(transportSource) {
    setInterceptsMouseClicks(false, false);
    startTimer(20);
}

void PositionOverlay::paint(juce::Graphics &g) {
    g.setColour(juce::Colour{0xff282828});
    auto duration = (float)transportSource.getLengthInSeconds();
    auto audioPosition = (float)transportSource.getCurrentPosition();
    auto drawPosition = duration == 0 ? 0 : (audioPosition / duration) * (float)getWidth();
    g.drawLine(drawPosition, 0.0f, drawPosition, (float)getHeight(), 1.0f);
}

void PositionOverlay::timerCallback() { repaint(); }

} // namespace trackman
