#pragma once

#include <JuceHeader.h>

#include "ChildWindow.h"
#include "FileDragDropTarget.h"
#include "ScaleButtonPanel.h"
#include "model/Mixer.h"
#include "ui/AudioPlayer.h"
#include "ui/common/TimeMeter.h"
#include "ui/mixer/MixerPanel.h"
#include "ui/tracks/TrackListPanel.h"
#include "ui/tracks/TrackListViewport.h"

class DesktopController;

class DesktopComponent : public juce::Component,
                         public juce::ApplicationCommandTarget,
                         public juce::MenuBarModel,
                         public juce::FileDragAndDropTarget {
  public:
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

    DesktopComponent(DesktopController &desktopController);
    ~DesktopComponent() override;

    void visibleAreaChanged(const juce::Rectangle<int> &newVisibleArea);

    void addListener(FileDragDropTarget *listener);
    void removeListener(FileDragDropTarget *listener);

    //==============================================================================
    // Component
    void paint(juce::Graphics &g) override;
    void resized() override;

    //==============================================================================
    // FileDragAndDropTarget
    bool isInterestedInFileDrag(const juce::StringArray &files) override;
    void fileDragEnter(const juce::StringArray &files, int x, int y) override;
    void fileDragMove(const juce::StringArray &files, int x, int y) override;
    void fileDragExit(const juce::StringArray &files) override;
    void filesDropped(const juce::StringArray &files, int x, int y) override;

    //==============================================================================
    // MenuBarModel
    juce::StringArray getMenuBarNames() override;
    juce::PopupMenu getMenuForIndex(int menuIndex, const juce::String & /*menuName*/) override;
    void menuItemSelected(int /*menuItemID*/, int /*topLevelMenuIndex*/) override {}

    //==============================================================================
    // ApplicationCommandTarget
    ApplicationCommandTarget *getNextCommandTarget() override { return nullptr; }
    void getAllCommands(juce::Array<juce::CommandID> &c) override;
    void getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo &result) override;
    bool perform(const InvocationInfo &info) override;

  private:
    const int initialWidth = 800;
    const int initialHeight = 600;

    DesktopController &desktopController;

    TimeMeter timeMeter;
    ScaleButtonPanel horizontalScaleButtonPanel;
    ScaleButtonPanel verticalScaleButtonPanel;
    std::list<FileDragDropTarget *> listeners;

    juce::ApplicationCommandManager commandManager;
    juce::MenuBarComponent menuBar;

    juce::Array<Component::SafePointer<Component>> windows;

    void createChildWindow(const juce::String &name, juce::Component *component);
    void closeAllWindows();

    void notifyFileDragEnter(const juce::StringArray &files, int x, int y);
    void notifyFileDragMove(const juce::StringArray &files, int x, int y);
    void notifyFileDragExit(const juce::StringArray &files);
    void notifyFilesDropped(const juce::StringArray &files, int x, int y);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DesktopComponent)
};
