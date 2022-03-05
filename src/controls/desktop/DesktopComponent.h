#pragma once

#include "JuceHeader.h"

#include "ChildWindow.h"
#include "DesktopController.h"
#include "controls/AudioPlayer.h"
#include "controls/mixer/MixerPanel.h"
#include "controls/tracks/TrackListPanel.h"
#include "model/Mixer.h"

class DesktopComponent : public juce::Component, public juce::ApplicationCommandTarget, public juce::MenuBarModel {
  public:
    //==============================================================================
    /** A list of the commands that this menu responds to. */
    enum CommandIDs { saveProject = 1, saveProjectAs, editUndo, newTrack, newAudioPlayer, deleteTrack };

    //==============================================================================
    DesktopComponent(juce::DocumentWindow *parentWindow, juce::AudioFormatManager &formatManager);
    ~DesktopComponent() override;

    //==============================================================================
    // Component
    void paint(juce::Graphics &g) override;
    void resized() override;

    //==============================================================================
    // MenuBarModel
    juce::StringArray getMenuBarNames() override { return {"file", "edit", "new", "track"}; }

    juce::PopupMenu getMenuForIndex(int menuIndex, const juce::String & /*menuName*/) override {
        juce::PopupMenu menu;

        if (menuIndex == 0) {
            menu.addCommandItem(&commandManager, CommandIDs::saveProject);
            menu.addCommandItem(&commandManager, CommandIDs::saveProjectAs);
        } else if (menuIndex == 1) {
            menu.addCommandItem(&commandManager, CommandIDs::editUndo);
        } else if (menuIndex == 2) {
            menu.addCommandItem(&commandManager, CommandIDs::newTrack);
            menu.addCommandItem(&commandManager, CommandIDs::newAudioPlayer);
        } else if (menuIndex == 3) {
            menu.addCommandItem(&commandManager, CommandIDs::deleteTrack);
        }

        return menu;
    }

    void menuItemSelected(int /*menuItemID*/, int /*topLevelMenuIndex*/) override {}

    //==============================================================================
    // ApplicationCommandTarget
    ApplicationCommandTarget *getNextCommandTarget() override { return nullptr; }

    void getAllCommands(juce::Array<juce::CommandID> &c) override {
        juce::Array<juce::CommandID> commands{CommandIDs::saveProject, CommandIDs::saveProjectAs, CommandIDs::editUndo,
            CommandIDs::newTrack, CommandIDs::newAudioPlayer, CommandIDs::deleteTrack};
        c.addArray(commands);
    }

    void getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo &result) override {
        switch (commandID) {
        case CommandIDs::saveProject:
            result.setInfo("save project", "Save the current project", "Menu", 0);
            result.addDefaultKeypress('s', juce::ModifierKeys::commandModifier);
            break;
        case CommandIDs::saveProjectAs:
            result.setInfo("save project as...", "Save the current project as a new file", "Menu", 0);
            result.addDefaultKeypress('s', juce::ModifierKeys::commandModifier | juce::ModifierKeys::altModifier);
            break;
        case CommandIDs::editUndo:
            result.setInfo(
                (desktopController.getLastCommandName() == "" ? "undo"
                                                              : "undo " + desktopController.getLastCommandName()),
                "Undo the last edit", "Menu", 0);
            result.addDefaultKeypress('z', juce::ModifierKeys::commandModifier);
            result.setActive(desktopController.canUndo());
            break;
        case CommandIDs::newTrack:
            result.setInfo("track", "Create a new track", "Menu", 0);
            result.addDefaultKeypress('t', juce::ModifierKeys::shiftModifier);
            break;
        case CommandIDs::newAudioPlayer:
            result.setInfo("audioplayer", "Create a new audioplayer component", "Menu", 0);
            result.addDefaultKeypress('p', juce::ModifierKeys::shiftModifier);
            break;
        case CommandIDs::deleteTrack:
            result.setInfo("delete", "Delete the selected track", "Menu", 0);
            result.addDefaultKeypress(juce::KeyPress::backspaceKey, juce::ModifierKeys::noModifiers);
            result.addDefaultKeypress(juce::KeyPress::deleteKey, juce::ModifierKeys::noModifiers);
            break;
        default:
            break;
        }
    }

    bool perform(const InvocationInfo &info) override {
        switch (info.commandID) {
        case CommandIDs::saveProject:
            desktopController.saveProject();
            break;
        case CommandIDs::saveProjectAs:
            desktopController.saveProjectAs();
            break;
        case CommandIDs::editUndo:
            desktopController.undoLast();
            break;
        case CommandIDs::newTrack:
            desktopController.addNewTrack();
            break;
        case CommandIDs::newAudioPlayer:
            createChildWindow("audioplayer", new AudioPlayer(formatManager));
            break;
        case CommandIDs::deleteTrack:
            desktopController.deleteSelectedTrack();
            break;
        default:
            return false;
        }
        return true;
    }

  private:
    DesktopController desktopController;
    juce::Component &trackListViewport;
    juce::Component &mixerPanel;

    juce::ApplicationCommandManager commandManager;
    juce::MenuBarComponent menuBar;

    // Because the windows delete themselves, we'll use the
    // Component::SafePointer class to point to them, which automatically
    // becomes null when the component that it points to is deleted.
    juce::Array<Component::SafePointer<Component>> windows;

    juce::AudioFormatManager &formatManager;

    void createChildWindow(const juce::String &name, juce::Component *component);
    void closeAllWindows();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DesktopComponent)
};
