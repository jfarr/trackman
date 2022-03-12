#include "ScaleButtonPanel.h"
#include "common/listutil.h"

ScaleButtonPanel::ScaleButtonPanel() {
    minusButton.setButtonText("-");
    minusButton.setConnectedEdges(juce::Button::ConnectedOnRight);
    minusButton.onClick = [this]() { minusButtonClicked(); };
    plusButton.setButtonText("+");
    plusButton.setConnectedEdges(juce::Button::ConnectedOnLeft);
    plusButton.onClick = [this]() { plusButtonClicked(); };
    addAndMakeVisible(minusButton);
    addAndMakeVisible(plusButton);
}

void ScaleButtonPanel::paint(juce::Graphics &g) {
    g.fillAll(juce::Colours::grey.withAlpha(0.5f));
    g.setColour(juce::Colours::lightgrey.withAlpha(0.2f));
    g.drawRect(0, -1, getWidth(), getHeight() + 1);
}

void ScaleButtonPanel::resized() {
    auto area = getLocalBounds();
    auto buttonWidth = 12;
    minusButton.setBounds(area.removeFromLeft(buttonWidth));
    plusButton.setBounds(area.removeFromLeft(buttonWidth));
}

void ScaleButtonPanel::minusButtonClicked() { notifyScaleDecreased(); }

void ScaleButtonPanel::plusButtonClicked() { notifyScaleIncreased(); }

void ScaleButtonPanel::addListener(TrackScaleListener *listener) {
    if (!listContains(listeners, listener)) {
        listeners.push_front(listener);
    }
}

void ScaleButtonPanel::removeListener(TrackScaleListener *listener) { listeners.remove(listener); }

void ScaleButtonPanel::notifyScaleIncreased() {
    for (TrackScaleListener *listener : listeners) {
        listener->scaleIncreased();
    }
}

void ScaleButtonPanel::notifyScaleDecreased() {
    for (TrackScaleListener *listener : listeners) {
        listener->scaleDecreased();
    }
}
