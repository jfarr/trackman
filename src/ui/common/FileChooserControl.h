#pragma once

#include <JuceHeader.h>

using namespace std;
using namespace juce;

namespace trackman {

class FileListener {
  public:
    virtual void fileChosen(File file) = 0;
};

class FileChooserControl : public Component {
  public:
    FileChooserControl(File file = File{});
    ~FileChooserControl();

    void addListener(FileListener *listener);
    void removeListener(FileListener *listener);

    //==============================================================================
    // Component
    void paint(Graphics &g) override;
    void resized() override;

    //==============================================================================

    void openButtonClicked();

  private:
    File selectedFile;
    list<FileListener *> listeners;

    void createControls();
    void notifyFileChosen(File file);

    unique_ptr<FileChooser> chooser;
    TextButton openButton;
    Label selectedFileLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FileChooserControl)
};

} // namespace trackman
