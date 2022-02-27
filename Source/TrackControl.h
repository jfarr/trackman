#pragma once

#include <JuceHeader.h>
#include "DecibelSlider.h"
#include "FileChooserControl.h"


class SourceListener
{
public:
    virtual void onSourceSet(juce::AudioFormatReaderSource& source) = 0;
};

class TrackControl : public juce::Component
{
public:
    TrackControl(juce::String trackName);
    ~TrackControl();

    void setListener(class FileListener* listener) { this->listener = listener;  }

    //==============================================================================
    // Component
    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    juce::String trackName;
    DecibelSlider decibelSlider;
    juce::TextButton muteButton;
    juce::Label channelLabel;
    float level;
    bool muted;

    void createControls();
    void muteButtonClicked();

    std::unique_ptr<juce::FileChooser> chooser;
    juce::TextButton openButton;
    void openButtonClicked();
    FileListener* listener;
};
