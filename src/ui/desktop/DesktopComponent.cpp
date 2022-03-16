#include "DesktopComponent.h"
#include "DesktopController.h"
#include "common/listutil.h"
#include "ui/MainWindow.h"

DesktopComponent::DesktopComponent(DesktopController &desktopController)
    : desktopController(desktopController), timeMeter(desktopController.getProject()),
      horizontalScaleButtonPanel(false), verticalScaleButtonPanel(true) {

    setSize(initialWidth, initialHeight);

    addListener(&desktopController);
    verticalScaleButtonPanel.addListener(&desktopController);
    horizontalScaleButtonPanel.addListener(&desktopController);

    addAndMakeVisible(timeMeter);
//    addAndMakeVisible(instrumentsPanel);
    addAndMakeVisible(desktopController.getTrackListController().getViewport());
    addAndMakeVisible(desktopController.getMixerController().getMixerPanel());
    addAndMakeVisible(desktopController.getInstrumentsController().getInstrumentsPanel());
    addAndMakeVisible(verticalScaleButtonPanel);
    addAndMakeVisible(horizontalScaleButtonPanel);

    setApplicationCommandManagerToWatch(&commandManager);
    commandManager.registerAllCommandsForTarget(this);
    commandManager.setFirstCommandTarget(this);
    addKeyListener(commandManager.getKeyMappings());
    setWantsKeyboardFocus(true);

#if JUCE_MAC
    MenuBarModel::setMacMainMenu(this);
#else
    parentWindow->setMenuBar(this);
#endif
}

DesktopComponent::~DesktopComponent() {
    closeAllWindows();

    removeListener(&desktopController);

#if JUCE_MAC
    juce::MenuBarModel::setMacMainMenu(nullptr);
#endif
    commandManager.setFirstCommandTarget(nullptr);
}

void DesktopComponent::visibleAreaChanged(const juce::Rectangle<int> &newVisibleArea) {
    timeMeter.setBounds(timeMeter.getBounds().withLeft(-newVisibleArea.getX() + leftPanelWidth));
}

void DesktopComponent::createChildWindow(const juce::String &name, juce::Component *component) {
    auto *window = new ChildWindow(name, component);
    windows.add(window);
    addAndMakeVisible(window);
}

void DesktopComponent::closeAllWindows() {
    for (auto &window : windows)
        window.deleteAndZero();

    windows.clear();
}

//==============================================================================
juce::StringArray DesktopComponent::getMenuBarNames() { return {"File", "Edit", "New", "Track"}; }

juce::PopupMenu DesktopComponent::getMenuForIndex(int menuIndex, const juce::String & /*menuName*/) {
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

//==============================================================================
void DesktopComponent::getAllCommands(juce::Array<juce::CommandID> &c) {
    juce::Array<juce::CommandID> commands{CommandIDs::openProject, CommandIDs::saveProject, CommandIDs::saveProjectAs,
        CommandIDs::exportProject, CommandIDs::editUndo, CommandIDs::newTrack, CommandIDs::newAudioPlayer,
        CommandIDs::deleteTrackSelection};
    c.addArray(commands);
}

void DesktopComponent::getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo &result) {
    switch (commandID) {
    case CommandIDs::openProject:
        result.setInfo("Open Project...", "Open a saved project", "Menu", 0);
        result.addDefaultKeypress('o', juce::ModifierKeys::commandModifier);
        break;
    case CommandIDs::saveProject:
        result.setInfo("Save Project", "Save the current project", "Menu", 0);
        result.addDefaultKeypress('s', juce::ModifierKeys::commandModifier);
        break;
    case CommandIDs::saveProjectAs:
        result.setInfo("Save Project As...", "Save the current project as a new file", "Menu", 0);
        result.addDefaultKeypress('s', juce::ModifierKeys::commandModifier | juce::ModifierKeys::altModifier);
        break;
    case CommandIDs::exportProject:
        result.setInfo("Export Project...", "Export the current project as an audio file", "Menu", 0);
        result.addDefaultKeypress('e', juce::ModifierKeys::commandModifier | juce::ModifierKeys::shiftModifier);
        break;
    case CommandIDs::editUndo:
        result.setInfo(
            (desktopController.getLastCommandName() == "" ? "Undo" : "Undo " + desktopController.getLastCommandName()),
            "Undo the last edit", "Menu", 0);
        result.addDefaultKeypress('z', juce::ModifierKeys::commandModifier);
        result.setActive(desktopController.canUndo());
        break;
    case CommandIDs::newTrack:
        result.setInfo("Track", "Create a new track", "Menu", 0);
        result.addDefaultKeypress('t', juce::ModifierKeys::commandModifier);
        break;
    case CommandIDs::newAudioPlayer:
        result.setInfo("Audio Player", "Create a new audioplayer component", "Menu", 0);
        result.addDefaultKeypress('p', juce::ModifierKeys::commandModifier);
        break;
    case CommandIDs::deleteTrackSelection:
        result.setInfo("Delete " + desktopController.getSelectionType(),
            "Delete the selected " + desktopController.getSelectionType(), "Menu", 0);
        result.addDefaultKeypress(juce::KeyPress::backspaceKey, juce::ModifierKeys::noModifiers);
        result.setActive(desktopController.hasSelection());
        break;
    default:
        break;
    }
}

bool DesktopComponent::perform(const InvocationInfo &info) {
    switch (info.commandID) {
    case CommandIDs::openProject:
        desktopController.openProject();
        break;
    case CommandIDs::saveProject:
        desktopController.saveProject();
        break;
    case CommandIDs::saveProjectAs:
        desktopController.saveProjectAs();
        break;
    case CommandIDs::exportProject:
        desktopController.exportProject();
        break;
    case CommandIDs::editUndo:
        desktopController.undoLast();
        break;
    case CommandIDs::newTrack:
        desktopController.addNewTrack();
        break;
    case CommandIDs::newAudioPlayer:
        createChildWindow("audioplayer",
            new AudioPlayer(desktopController.getMainWindow().getMainAudioComponent().getFormatManager()));
        break;
    case CommandIDs::deleteTrackSelection:
        desktopController.deleteSelected();
        break;
    default:
        return false;
    }
    return true;
}

//==============================================================================
void DesktopComponent::paint(juce::Graphics &g) {
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void DesktopComponent::resized() {
    auto scrollBarWidth = desktopController.getTrackListController().getViewport().getScrollBarThickness();
    auto area = getLocalBounds();
    auto &mixerPanel = desktopController.getMixerController().getMixerPanel();
    mixerPanel.setBounds(area.removeFromBottom(mixerPanel.getPreferredHeight()));
    verticalScaleButtonPanel.setBounds(juce::Rectangle<int>(area.getWidth() - (scaleButtonWidth + scrollBarWidth),
        area.getY() + topStripHeight, scaleButtonWidth, scaleButtonWidth * 2));
    horizontalScaleButtonPanel.setBounds(
        juce::Rectangle<int>(0, area.getHeight() - scaleButtonWidth, scaleButtonWidth * 2, scaleButtonWidth));
    auto &instrumentsPanel = desktopController.getInstrumentsController().getInstrumentsPanel();
    instrumentsPanel.setBounds(area.removeFromLeft(leftPanelWidth));
    timeMeter.setBounds(area.removeFromTop(topStripHeight));
    timeMeter.repaint();
    desktopController.getTrackListController().getViewport().setBounds(area);
    desktopController.resize();
}

//==============================================================================
bool DesktopComponent::isInterestedInFileDrag(const juce::StringArray &files) { return true; }

void DesktopComponent::fileDragEnter(const juce::StringArray &files, int x, int y) { notifyFileDragEnter(files, x, y); }

void DesktopComponent::fileDragMove(const juce::StringArray &files, int x, int y) { notifyFileDragMove(files, x, y); }

void DesktopComponent::fileDragExit(const juce::StringArray &files) { notifyFileDragExit(files); }

void DesktopComponent::filesDropped(const juce::StringArray &files, int x, int y) { notifyFilesDropped(files, x, y); }

//==============================================================================
void DesktopComponent::addListener(FileDragDropTarget *listener) {
    if (!listContains(listeners, listener)) {
        listeners.push_front(listener);
    }
}

void DesktopComponent::removeListener(FileDragDropTarget *listener) { listeners.remove(listener); }

void DesktopComponent::notifyFileDragEnter(const juce::StringArray &files, int x, int y) {
    for (FileDragDropTarget *listener : listeners) {
        listener->fileDragEnter(files, x, y);
    }
}

void DesktopComponent::notifyFileDragMove(const juce::StringArray &files, int x, int y) {
    for (FileDragDropTarget *listener : listeners) {
        listener->fileDragMove(files, x, y);
    }
}

void DesktopComponent::notifyFileDragExit(const juce::StringArray &files) {
    for (FileDragDropTarget *listener : listeners) {
        listener->fileDragExit(files);
    }
}

void DesktopComponent::notifyFilesDropped(const juce::StringArray &files, int x, int y) {
    for (FileDragDropTarget *listener : listeners) {
        listener->filesDropped(files, x, y);
    }
}
