#include "SampleThumbnail.h"

SampleThumbnail::SampleThumbnail(Sample &sample, juce::AudioFormatManager &formatManager) : sample(sample), thumbnail(formatManager) {
    createControls();
    setSize(200, 85);
}

void SampleThumbnail::createControls() {
    filenameLabel.setText(sample.getFile().getFileName(), juce::dontSendNotification);
    thumbnail.setSource(sample.getFile());
    addAndMakeVisible(filenameLabel);
    addAndMakeVisible(thumbnail);
}

void SampleThumbnail::paint(juce::Graphics &g) {
    g.fillAll(juce::Colours::dimgrey);
    g.setColour(juce::Colours::grey);
    g.drawRect(0, 0, getWidth(), getHeight());
}

void SampleThumbnail::resized() {
    auto area = getLocalBounds();
    auto labelHeight = 18;
    auto margin = 2;
    filenameLabel.setBounds(area.removeFromTop(labelHeight).reduced(margin));
    thumbnail.setBounds(area.reduced(margin));
}
