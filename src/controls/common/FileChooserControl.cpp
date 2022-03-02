#include "FileChooserControl.h"

#include "common/listutil.h"

FileChooserControl::FileChooserControl(juce::File file) {
    selectedFile = file;
    createControls();
}

FileChooserControl::~FileChooserControl() {}

void FileChooserControl::createControls() {
    addAndMakeVisible(&openButton);
    openButton.setButtonText("...");
    openButton.onClick = [this] { openButtonClicked(); };

    addAndMakeVisible(&selectedFileLabel);
}

void FileChooserControl::paint(juce::Graphics &g) {
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void FileChooserControl::resized() {
    auto area = getLocalBounds();

    auto buttonWidth = 30;
    auto margin = 2;
    openButton.setBounds(area.removeFromLeft(buttonWidth).reduced(margin));
    selectedFileLabel.setBounds(area.reduced(margin));
}

void FileChooserControl::openButtonClicked() {
    chooser = std::make_unique<juce::FileChooser>("Select a file to play...", selectedFile, "*.wav");
    auto chooserFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;

    chooser->launchAsync(chooserFlags, [this](const juce::FileChooser &fc) {
        auto file = fc.getResult();

        if (file != juce::File{}) {
            selectedFileLabel.setText(file.getFileName(), juce::dontSendNotification);
            notifyFileChosen(file);
        }
    });
}

void FileChooserControl::notifyFileChosen(juce::File file) {
    for (FileListener *listener : listeners) {
        listener->fileChosen(file);
    }
}

void FileChooserControl::addListener(FileListener *listener) {
    if (!listContains(listener, listeners)) {
        listeners.push_front(listener);
    }
}

void FileChooserControl::removeListener(FileListener *listener) { listeners.remove(listener); }
