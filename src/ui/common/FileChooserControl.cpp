#include "FileChooserControl.h"

#include "common/listutil.h"

namespace trackman {

FileChooserControl::FileChooserControl(File file) {
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

void FileChooserControl::paint(Graphics &g) {
    g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
}

void FileChooserControl::resized() {
    auto area = getLocalBounds();

    auto buttonWidth = 30;
    auto margin = 2;
    openButton.setBounds(area.removeFromLeft(buttonWidth).reduced(margin));
    selectedFileLabel.setBounds(area.reduced(margin));
}

void FileChooserControl::openButtonClicked() {
    chooser = make_unique<FileChooser>("Select a file to play...", selectedFile, "*.wav", true);
    auto chooserFlags = FileBrowserComponent::openMode | FileBrowserComponent::canSelectFiles;

    chooser->launchAsync(chooserFlags, [this](const FileChooser &fc) {
        auto file = fc.getResult();

        if (file != File{}) {
            selectedFileLabel.setText(file.getFileName(), dontSendNotification);
            notifyFileChosen(file);
        }
    });
}

void FileChooserControl::notifyFileChosen(File file) {
    for (FileListener *listener : listeners) {
        listener->fileChosen(file);
    }
}

void FileChooserControl::addListener(FileListener *listener) {
    if (!listContains(listeners, listener)) {
        listeners.push_front(listener);
    }
}

void FileChooserControl::removeListener(FileListener *listener) { listeners.remove(listener); }

} // namespace trackman
