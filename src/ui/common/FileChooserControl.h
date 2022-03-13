#pragma once

#include <JuceHeader.h>

class FileListener {
  public:
    virtual void fileChosen(juce::File file) = 0;
};

class FileChooserControl : public juce::Component {
  public:
    FileChooserControl(juce::File file = juce::File{});
    ~FileChooserControl();

    void addListener(FileListener *listener);
    void removeListener(FileListener *listener);

    //==============================================================================
    // Component
    void paint(juce::Graphics &g) override;
    void resized() override;

    //==============================================================================

    void openButtonClicked();

  private:
    juce::File selectedFile;
    std::list<FileListener *> listeners;

    void createControls();
    void notifyFileChosen(juce::File file);

    std::unique_ptr<juce::FileChooser> chooser;
    juce::TextButton openButton;
    juce::Label selectedFileLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FileChooserControl)
};
