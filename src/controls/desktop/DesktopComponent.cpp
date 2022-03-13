#include "DesktopComponent.h"
#include "DesktopController.h"
#include "common/listutil.h"
#include "controls/MainWindow.h"

DesktopComponent::DesktopComponent(DesktopController &desktopController)
    : desktopController(desktopController) /*, timeMeter(desktopController.getProject()),
      horizontalScaleButtonPanel(false), verticalScaleButtonPanel(true)*/ {
    //    : formatManager(formatManager), timeMeter(desktopController.getProject()),
    //      desktopController(*parentWindow, *this, deviceManager, formatManager),
    //      trackListViewport(desktopController.getTrackListController().getViewport()),
    //      mixerPanel(desktopController.getMixerController().getMixerPanel()), mixer(desktopController.getMixer()),
    //      horizontalScaleButtonPanel(false), verticalScaleButtonPanel(true) {

    setSize(800, 600);

    //    setAudioChannels(0, 2);

    addListener(&desktopController);

    addAndMakeVisible(desktopController.getMixerController().getMixerPanel());
//    verticalScaleButtonPanel.addListener(&desktopController);
//    horizontalScaleButtonPanel.addListener(&desktopController);
//
//    addAndMakeVisible(timeMeter);
//    addAndMakeVisible(verticalScaleButtonPanel);
//    addAndMakeVisible(horizontalScaleButtonPanel);

    //    auto &trackListViewport = desktopController.getTrackListController().getViewport();
    //    auto &mixerPanel = desktopController.getMixerController().getMixerPanel();
    //    addAndMakeVisible(trackListViewport);
    //    addAndMakeVisible(mixerPanel);

    setApplicationCommandManagerToWatch(&commandManager);
    commandManager.registerAllCommandsForTarget(this);

    // this ensures that commands invoked on the application are correctly
    // forwarded to this component
    commandManager.setFirstCommandTarget(this);

    // this lets the command manager use keypresses that arrive in our window to
    // send out commands
    addKeyListener(commandManager.getKeyMappings());
    setWantsKeyboardFocus(true);

//    desktopController.getMainWindow().setMenuBar(this);
//    addAndMakeVisible(menuBar);

#if JUCE_MAC
    MenuBarModel::setMacMainMenu(this);
#else
    parentWindow->setMenuBar(this);
    addAndMakeVisible(menuBar);
#endif
//    initialized = true;
}

DesktopComponent::~DesktopComponent() {
    //    shutdownAudio();
    closeAllWindows();

    removeListener(&desktopController);

#if JUCE_MAC
    juce::MenuBarModel::setMacMainMenu(nullptr);
#endif
    commandManager.setFirstCommandTarget(nullptr);
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

void DesktopComponent::visibleAreaChanged(const juce::Rectangle<int> &newVisibleArea) {
    DBG("DesktopComponent::visibleAreaChanged: " << newVisibleArea.getX() << "," << newVisibleArea.getY());
//    timeMeter.setBounds(timeMeter.getBounds().withLeft(-newVisibleArea.getX()));
}


void DesktopComponent::getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo &result) {
    switch (commandID) {
    case CommandIDs::openProject:
        result.setInfo("open project...", "Open a saved project", "Menu", 0);
        result.addDefaultKeypress('o', juce::ModifierKeys::commandModifier);
        break;
    case CommandIDs::saveProject:
        result.setInfo("save project", "Save the current project", "Menu", 0);
        result.addDefaultKeypress('s', juce::ModifierKeys::commandModifier);
        break;
    case CommandIDs::saveProjectAs:
        result.setInfo("save project as...", "Save the current project as a new file", "Menu", 0);
        result.addDefaultKeypress('s', juce::ModifierKeys::commandModifier | juce::ModifierKeys::altModifier);
        break;
    case CommandIDs::exportProject:
        result.setInfo("export project...", "Export the current project as an audio file", "Menu", 0);
        result.addDefaultKeypress('e', juce::ModifierKeys::commandModifier | juce::ModifierKeys::shiftModifier);
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
        result.addDefaultKeypress('t', juce::ModifierKeys::commandModifier);
        break;
    case CommandIDs::newAudioPlayer:
        result.setInfo("audioplayer", "Create a new audioplayer component", "Menu", 0);
        result.addDefaultKeypress('p', juce::ModifierKeys::commandModifier);
        break;
    case CommandIDs::deleteTrackSelection:
        result.setInfo("delete " + desktopController.getSelectionType(),
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
        createChildWindow("audioplayer", new AudioPlayer(desktopController.getMainWindow().getMainAudioComponent().getFormatManager()));
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
// void DesktopComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate) {
//    desktopController.getMixer().prepareToPlay(samplesPerBlockExpected, sampleRate);
//}
//
// void DesktopComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) {
//    desktopController.getMixer().getNextAudioBlock(bufferToFill);
//}
//
// void DesktopComponent::releaseResources() { desktopController.getMixer().releaseResources(); }

//==============================================================================
void DesktopComponent::paint(juce::Graphics &g) {
//    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    g.fillAll(juce::Colours::green);
}

void DesktopComponent::resized() {
//    if (!initialized) {
//        return;
//    }
    auto topStripHeight = 15;
    auto scaleButtonWidth = 12;
    //    auto scrollBarWidth = desktopController.getTrackListController().getViewport().getScrollBarThickness();
    auto scrollBarWidth = 15;
    auto area = getLocalBounds();
    area.removeFromTop(topStripHeight);
//    timeMeter.setBounds(area.removeFromTop(topStripHeight));
//    verticalScaleButtonPanel.setBounds(juce::Rectangle<int>(
//        area.getWidth() - (scaleButtonWidth + scrollBarWidth), area.getY(), scaleButtonWidth, scaleButtonWidth * 2));
    //    auto &mixerPanel = desktopController.getMixerController().getMixerPanel();
    //    mixerPanel.setBounds(area.removeFromBottom(mixerPanel.getPreferredHeight()));
    //    horizontalScaleButtonPanel.setBounds(
    //        juce::Rectangle<int>(0, area.getHeight() - (scaleButtonWidth + scrollBarWidth) + topStripHeight,
    //            scaleButtonWidth * 2, scaleButtonWidth));
    //    desktopController.getTrackListController().getViewport().setBounds(area);
//    desktopController.resize();
    auto &mixerPanel = desktopController.getMixerController().getMixerPanel();
    mixerPanel.setBounds(area.removeFromBottom(mixerPanel.getPreferredHeight()));
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
