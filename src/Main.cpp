#include <JuceHeader.h>

#include "controls/MainWindow.h"

class TrackmanApplication : public juce::JUCEApplication {
  public:
    TrackmanApplication() {}

    const juce::String getApplicationName() override { return ProjectInfo::projectName; }
    const juce::String getApplicationVersion() override { return ProjectInfo::versionString; }
    bool moreThanOneInstanceAllowed() override { return true; }

    //==============================================================================
    void initialise(const juce::String & /*commandLine*/) override {
        // This method is where you should put your application's initialisation
        // code..

        formatManager.registerBasicFormats();

        mainWindow.reset(new MainWindow(getApplicationName(), formatManager));
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
        quit();
    }

    void anotherInstanceStarted(const juce::String & /*commandLine*/) override {
        // When another instance of the app is launched while this one is
        // running, this method is invoked, and the commandLine parameter tells
        // you what the other instance's command-line arguments were.
    }

  private:
    juce::AudioFormatManager formatManager;
    std::unique_ptr<MainWindow> mainWindow;
};

START_JUCE_APPLICATION(TrackmanApplication)
