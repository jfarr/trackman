#pragma once

#include <JuceHeader.h>

#include "ChildWindow.h"
#include "FileDragDropTarget.h"
#include "ScaleButtonPanel.h"
#include "controls/AudioPlayer.h"
#include "controls/common/TimeMeter.h"
#include "controls/mixer/MixerPanel.h"
#include "controls/tracks/TrackListPanel.h"
#include "controls/tracks/TrackListViewport.h"
#include "model/Mixer.h"

class DesktopController;

class DesktopComponent : public juce::Component,
                         public juce::ApplicationCommandTarget,
                         public juce::MenuBarModel,
                         public juce::FileDragAndDropTarget {
  public:
    //==============================================================================
    /** A list of the commands that this menu responds to. */
    enum CommandIDs {
        openProject = 1,
        saveProject,
        saveProjectAs,
        exportProject,
        editUndo,
        newTrack,
        newAudioPlayer,
        deleteTrackSelection
    };

    //==============================================================================
    //    DesktopComponent(juce::DocumentWindow *parentWindow, juce::AudioFormatManager &formatManager);
    DesktopComponent(DesktopController &desktopController);
    ~DesktopComponent() override;

    //    DesktopController &getDesktopController() { return desktopController; }
    //    bool isDirty() const { return desktopController.isDirty(); };
    void visibleAreaChanged(const juce::Rectangle<int> &newVisibleArea);

    void addListener(FileDragDropTarget *listener);
    void removeListener(FileDragDropTarget *listener);

    //==============================================================================
    // Component
    void paint(juce::Graphics &g) override;
    void resized() override;

    //==============================================================================
    // AudioAppComponent
    //    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    //    void getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) override;
    //    void releaseResources() override;

    //==============================================================================
    // FileDragAndDropTarget
    bool isInterestedInFileDrag(const juce::StringArray &files) override;
    void fileDragEnter(const juce::StringArray &files, int x, int y) override;
    void fileDragMove(const juce::StringArray &files, int x, int y) override;
    void fileDragExit(const juce::StringArray &files) override;
    void filesDropped(const juce::StringArray &files, int x, int y) override;

    //==============================================================================
    // MenuBarModel
    juce::StringArray getMenuBarNames() override { return {"file", "edit", "new", "track"}; }

    juce::PopupMenu getMenuForIndex(int menuIndex, const juce::String & /*menuName*/) override {
        juce::PopupMenu menu;

        if (menuIndex == 0) {
            menu.addCommandItem(&commandManager, CommandIDs::openProject);
            menu.addCommandItem(&commandManager, CommandIDs::saveProject);
            menu.addCommandItem(&commandManager, CommandIDs::saveProjectAs);
            menu.addSeparator();
            menu.addCommandItem(&commandManager, CommandIDs::exportProject);
        } else if (menuIndex == 1) {
            menu.addCommandItem(&commandManager, CommandIDs::editUndo);
        } else if (menuIndex == 2) {
            menu.addCommandItem(&commandManager, CommandIDs::newTrack);
            menu.addCommandItem(&commandManager, CommandIDs::newAudioPlayer);
        } else if (menuIndex == 3) {
            menu.addCommandItem(&commandManager, CommandIDs::deleteTrackSelection);
        }

        return menu;
    }

    void menuItemSelected(int /*menuItemID*/, int /*topLevelMenuIndex*/) override {}

    //==============================================================================
    // ApplicationCommandTarget
    ApplicationCommandTarget *getNextCommandTarget() override { return nullptr; }

    void getAllCommands(juce::Array<juce::CommandID> &c) override {
        juce::Array<juce::CommandID> commands{CommandIDs::openProject, CommandIDs::saveProject,
            CommandIDs::saveProjectAs, CommandIDs::exportProject, CommandIDs::editUndo, CommandIDs::newTrack,
            CommandIDs::newAudioPlayer, CommandIDs::deleteTrackSelection};
        c.addArray(commands);
    }

    void getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo &result) override;
    bool perform(const InvocationInfo &info) override;

  private:
    DesktopController &desktopController;
    //    TrackListViewport &trackListViewport;
    //    MixerPanel &mixerPanel;
    //    Mixer &mixer;
    TimeMeter timeMeter;
    ScaleButtonPanel horizontalScaleButtonPanel;
    ScaleButtonPanel verticalScaleButtonPanel;
    std::list<FileDragDropTarget *> listeners;
    bool initialized = false;

    juce::ApplicationCommandManager commandManager;
    juce::MenuBarComponent menuBar;

    // Because the windows delete themselves, we'll use the
    // Component::SafePointer class to point to them, which automatically
    // becomes null when the component that it points to is deleted.
    juce::Array<Component::SafePointer<Component>> windows;

    //    juce::AudioFormatManager &formatManager;

    void createChildWindow(const juce::String &name, juce::Component *component);
    void closeAllWindows();

    void notifyFileDragEnter(const juce::StringArray &files, int x, int y);
    void notifyFileDragMove(const juce::StringArray &files, int x, int y);
    void notifyFileDragExit(const juce::StringArray &files);
    void notifyFilesDropped(const juce::StringArray &files, int x, int y);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DesktopComponent)
};
