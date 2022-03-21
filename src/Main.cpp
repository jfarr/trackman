#include <JuceHeader.h>

#include <memory>

#include "ui/MainWindow.h"

using namespace std;
using namespace juce;

namespace trackman {

class TrackmanApplication : public JUCEApplication {
  public:
    TrackmanApplication() {}

    const String getApplicationName() override { return ProjectInfo::projectName; }
    const String getApplicationVersion() override { return ProjectInfo::versionString; }
    bool moreThanOneInstanceAllowed() override { return true; }

    //==============================================================================
    void initialise(const String & /*commandLine*/) override {
        // This method is where you should put your application's initialisation
        // code..
        mainWindow = make_unique<MainWindow>(getApplicationName());

        // Bring the main window to the front.
        mainWindow->setAlwaysOnTop(true);
        mainWindow->setWantsKeyboardFocus(true);
        mainWindow->setAlwaysOnTop(false);
    }

    void shutdown() override {
        // Add your application's shutdown code here..

        mainWindow = nullptr; // (deletes our window)
    }

    //==============================================================================
    void systemRequestedQuit() override {
        // This is called when the app is being asked to quit: you can ignore
        // this request and let the app carry on running, or call quit() to
        // allow the app to close.
        if (mainWindow == nullptr || !mainWindow->getMainAudioComponent().getDesktopController().isDirty()) {
            quit();
        } else {
            NativeMessageBox::showYesNoCancelBox(MessageBoxIconType::QuestionIcon, "", "Save project before closing?",
                &mainWindow->getMainAudioComponent().getDesktopController().getDesktopComponent(),
                ModalCallbackFunction::create([this](int result) {
                    if (result == 2) {
                        quit();
                    } else if (result == 1) {
                        mainWindow->getMainAudioComponent().getDesktopController().saveProject([](bool saved) {
                            if (saved) {
                                quit();
                            }
                        });
                    }
                }));
        }
    }

    void anotherInstanceStarted(const String & /*commandLine*/) override {
        // When another instance of the app is launched while this one is
        // running, this method is invoked, and the commandLine parameter tells
        // you what the other instance's command-line arguments were.
    }

  private:
    unique_ptr<MainWindow> mainWindow;
};

START_JUCE_APPLICATION(TrackmanApplication)

} // namespace trackman
