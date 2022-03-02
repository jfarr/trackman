#pragma once

#include "JuceHeader.h"

#include "ChildWindow.h"
#include "DesktopController.h"
#include "controls/AudioPlayer.h"
#include "controls/mixer/MixerComponent.h"
#include "controls/tracks/TrackListPanel.h"

class DesktopComponent : public juce::Component, public juce::ApplicationCommandTarget, public juce::MenuBarModel {
  public:
    //==============================================================================
    /** A list of the commands that this menu responds to. */
    enum CommandIDs { newTrack = 1, newAudioPlayer, editUndo };

    //==============================================================================
    DesktopComponent(juce::DocumentWindow *parentWindow);
    ~DesktopComponent() override;

    //==============================================================================
    // Component
    void paint(juce::Graphics &g) override;
    void resized() override;

    //==============================================================================

    juce::StringArray getMenuBarNames() override { return {"new", "edit"}; }

    juce::PopupMenu getMenuForIndex(int menuIndex, const juce::String & /*menuName*/) override {
        juce::PopupMenu menu;

        if (menuIndex == 0) {
            menu.addCommandItem(&commandManager, CommandIDs::newTrack);
            menu.addCommandItem(&commandManager, CommandIDs::newAudioPlayer);
        } else if (menuIndex == 1) {
            menu.addCommandItem(&commandManager, CommandIDs::editUndo);
        }

        return menu;
    }

    void menuItemSelected(int /*menuItemID*/, int /*topLevelMenuIndex*/) override {}

    //==============================================================================
    // The following methods implement the ApplicationCommandTarget interface,
    // allowing this window to publish a set of actions it can perform, and
    // which can be mapped onto menus, keypresses, etc.
    ApplicationCommandTarget *getNextCommandTarget() override { return nullptr; }

    void getAllCommands(juce::Array<juce::CommandID> &c) override {
        juce::Array<juce::CommandID> commands{CommandIDs::newTrack, CommandIDs::newAudioPlayer, CommandIDs::editUndo};
        c.addArray(commands);
    }

    void getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo &result) override {
        switch (commandID) {
        case CommandIDs::newTrack:
            result.setInfo("track", "Create a new track", "Menu", 0);
            result.addDefaultKeypress('t', juce::ModifierKeys::shiftModifier);
            break;
        case CommandIDs::newAudioPlayer:
            result.setInfo("audioplayer", "Create a new audioplayer component", "Menu", 0);
            result.addDefaultKeypress('p', juce::ModifierKeys::shiftModifier);
            break;
        case CommandIDs::editUndo:
            result.setInfo(
                (desktopController.getLastCommandName() == "" ? "undo"
                                                              : "undo " + desktopController.getLastCommandName()),
                "Undo the last edit", "Menu", 0);
            result.addDefaultKeypress('z', juce::ModifierKeys::commandModifier);
            result.setActive(desktopController.canUndo());
            break;
        default:
            break;
        }
    }
    bool perform(const InvocationInfo &info) override {
        switch (info.commandID) {
        case CommandIDs::newTrack:
            desktopController.addNewTrack();
            break;
        case CommandIDs::newAudioPlayer:
            createChildWindow("audioplayer", new AudioPlayer(formatManager));
            break;
        case CommandIDs::editUndo:
            desktopController.undoLast();
            break;
        default:
            return false;
        }

        return true;
    }

  private:
    juce::AudioFormatManager formatManager;
    MixerComponent mixerComponent;
    TrackListPanel trackListPanel;

    DesktopController desktopController;
    juce::Viewport trackListViewport;
    juce::ApplicationCommandManager commandManager;
    juce::MenuBarComponent menuBar;

    // Because the windows delete themselves, we'll use the
    // Component::SafePointer class to point to them, which automatically
    // becomes null when the component that it points to is deleted.
    juce::Array<Component::SafePointer<Component>> windows;

    void createChildWindow(const juce::String &name, juce::Component *component);
    void closeAllWindows();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DesktopComponent)
};
