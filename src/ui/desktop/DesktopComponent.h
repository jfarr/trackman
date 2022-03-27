#pragma once

#include <JuceHeader.h>

#include "ChildWindow.h"
#include "FileDragDropTarget.h"
#include "ScaleButtonPanel.h"
#include "TimeMeter.h"
#include "model/Mixer.h"
#include "ui/AudioPlayer.h"
#include "ui/instruments/InstrumentsPanel.h"
#include "ui/mixer/MixerPanel.h"
#include "ui/tracks/TrackListPanel.h"
#include "ui/tracks/TrackListViewport.h"

using namespace std;
using namespace juce;

namespace trackman {

class DesktopController;

class Spacer : public Component {
  public:
    Spacer() = default;
    ~Spacer() = default;

    void paint(Graphics &g) override {
        g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
    }

  private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Spacer)
};

class DesktopComponent : public Component,
                         public ApplicationCommandTarget,
                         public MenuBarModel,
                         public FileDragAndDropTarget {
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
        newMidiKeyboard,
        deleteTrackSelection
    };

    DesktopComponent(DesktopController &desktopController);
    ~DesktopComponent() override;

    TimeMeter & getTimeMeter() { return timeMeter; }

    void visibleAreaChanged(const Rectangle<int> &newVisibleArea);

    void createChildWindow(const String &name, Component *component);

    void addListener(FileDragDropTarget *listener);
    void removeListener(FileDragDropTarget *listener);

    //==============================================================================
    // Component
    void paint(Graphics &g) override;
    void resized() override;

    //==============================================================================
    // FileDragAndDropTarget
    bool isInterestedInFileDrag(const StringArray &files) override;
    void fileDragEnter(const StringArray &files, int x, int y) override;
    void fileDragMove(const StringArray &files, int x, int y) override;
    void fileDragExit(const StringArray &files) override;
    void filesDropped(const StringArray &files, int x, int y) override;

    //==============================================================================
    // MenuBarModel
    StringArray getMenuBarNames() override;
    PopupMenu getMenuForIndex(int menuIndex, const String & /*menuName*/) override;
    void menuItemSelected(int /*menuItemID*/, int /*topLevelMenuIndex*/) override {}

    //==============================================================================
    // ApplicationCommandTarget
    ApplicationCommandTarget *getNextCommandTarget() override { return nullptr; }
    void getAllCommands(Array<CommandID> &c) override;
    void getCommandInfo(CommandID commandID, ApplicationCommandInfo &result) override;
    bool perform(const InvocationInfo &info) override;

  private:
    const int initialWidth = 800;
    const int initialHeight = 600;
    const int leftPanelWidth = 30;
    const int panelWidth = 25;
    const int topStripHeight = 30;
    const int scaleButtonWidth = 12;

    DesktopController &desktopController;

    TimeMeter timeMeter;
    ScaleButtonPanel horizontalScaleButtonPanel;
    ScaleButtonPanel verticalScaleButtonPanel;
    Spacer spacer;

    list<FileDragDropTarget *> listeners;

    ApplicationCommandManager commandManager;
    MenuBarComponent menuBar;

    Array<Component::SafePointer<Component>> windows;

    void closeAllWindows();

    void notifyFileDragEnter(const StringArray &files, int x, int y);
    void notifyFileDragMove(const StringArray &files, int x, int y);
    void notifyFileDragExit(const StringArray &files);
    void notifyFilesDropped(const StringArray &files, int x, int y);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DesktopComponent)
};

} // namespace trackman
