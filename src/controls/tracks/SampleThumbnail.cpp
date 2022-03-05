#include "SampleThumbnail.h"

SampleThumbnail::SampleThumbnail(Sample &sample) : sample(sample) {
    setSize(200, 85);
}

void SampleThumbnail::paint(juce::Graphics &g) {
    g.fillAll(juce::Colours::dimgrey);
    g.setColour(juce::Colours::grey);
    g.drawRect(0, 0, getWidth(), getHeight());
}

void SampleThumbnail::resized() {

}
