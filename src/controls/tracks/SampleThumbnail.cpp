#include "SampleThumbnail.h"

SampleThumbnail::SampleThumbnail(
    Sample &sample, juce::AudioTransportSource &transport, juce::AudioFormatManager &formatManager)
    : sample(sample), transport(transport), thumbnailCache(5), thumbnail(512, formatManager, thumbnailCache) {
    thumbnail.setSource(new juce::FileInputSource(sample.getFile()));
    createControls();
    setSize(200, 85);
}

void SampleThumbnail::createControls() {
    filenameLabel.setText(sample.getFile().getFileName(), juce::dontSendNotification);
    thumbnail.setSource(new juce::FileInputSource(sample.getFile()));
    addAndMakeVisible(filenameLabel);
//    addAndMakeVisible(thumbnail);
}

void SampleThumbnail::paint(juce::Graphics &g) {
    auto area = getLocalBounds();
    auto margin = 3;

    g.fillAll(juce::Colours::dimgrey);
    g.setColour(juce::Colours::grey);
    g.drawRect(0, 0, getWidth(), getHeight());

    auto thumbnailBounds = area.reduced(margin);

    g.setColour(juce::Colours::darkgrey);
    g.fillRect(thumbnailBounds);

    g.setColour(juce::Colours::green);

    thumbnail.drawChannels(g, thumbnailBounds,
        0.0,                        // start time
        thumbnail.getTotalLength(), // end time
        1.0f);                      // vertical zoom

    auto duration = (float)transport.getLengthInSeconds();

    if (duration > 0.0) {
        auto audioPosition = (float)transport.getCurrentPosition();
        if (audioPosition >= sample.getStartPos() && audioPosition <= sample.getEndPos()) {
            auto samplePos = audioPosition - sample.getStartPos();
            auto drawPosition = samplePos * scale;
            g.setColour(juce::Colours::yellowgreen);
            g.drawLine(drawPosition, 0.0f, drawPosition, (float)getHeight(), 2.0f);
        }
    }
}

void SampleThumbnail::resized() {
    auto area = getLocalBounds();
    auto labelHeight = 18;
    auto margin = 2;
    filenameLabel.setBounds(area.removeFromTop(labelHeight).reduced(margin));
//    thumbnail.setBounds(area.reduced(margin));
}
