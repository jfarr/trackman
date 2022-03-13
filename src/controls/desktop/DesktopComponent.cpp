#include "DesktopComponent.h"
#include "common/listutil.h"

DesktopComponent::DesktopComponent(DesktopController &desktopController)
    : desktopController(desktopController), timeMeter(desktopController.getProject()),
      horizontalScaleButtonPanel(false), verticalScaleButtonPanel(true) {
    //    : formatManager(formatManager), timeMeter(desktopController.getProject()),
    //      desktopController(*parentWindow, *this, deviceManager, formatManager),
    //      trackListViewport(desktopController.getTrackListController().getViewport()),
    //      mixerPanel(desktopController.getMixerController().getMixerPanel()), mixer(desktopController.getMixer()),
    //      horizontalScaleButtonPanel(false), verticalScaleButtonPanel(true) {

    setSize(800, 600);

//    setAudioChannels(0, 2);

    addListener(&desktopController);
    verticalScaleButtonPanel.addListener(&desktopController);
    horizontalScaleButtonPanel.addListener(&desktopController);

    addAndMakeVisible(timeMeter);
    addAndMakeVisible(verticalScaleButtonPanel);
    addAndMakeVisible(horizontalScaleButtonPanel);

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

#if JUCE_MAC
    MenuBarModel::setMacMainMenu(this);
#else
    parentWindow->setMenuBar(this);
    addAndMakeVisible(menuBar);
#endif
    initialized = true;
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
    timeMeter.setBounds(timeMeter.getBounds().withLeft(-newVisibleArea.getX()));
}

//==============================================================================
void DesktopComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate) {
    desktopController.getMixer().prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void DesktopComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) {
    desktopController.getMixer().getNextAudioBlock(bufferToFill);
}

void DesktopComponent::releaseResources() { desktopController.getMixer().releaseResources(); }

//==============================================================================
void DesktopComponent::paint(juce::Graphics &g) {
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void DesktopComponent::resized() {
    if (!initialized) {
        return;
    }
    auto topStripHeight = 15;
    auto scaleButtonWidth = 12;
    auto scrollBarWidth = desktopController.getTrackListController().getViewport().getScrollBarThickness();
    auto area = getLocalBounds();
    area.removeFromTop(topStripHeight);
    timeMeter.setBounds(area.removeFromTop(topStripHeight));
    verticalScaleButtonPanel.setBounds(juce::Rectangle<int>(
        area.getWidth() - (scaleButtonWidth + scrollBarWidth), area.getY(), scaleButtonWidth, scaleButtonWidth * 2));
    auto &mixerPanel = desktopController.getMixerController().getMixerPanel();
    mixerPanel.setBounds(area.removeFromBottom(mixerPanel.getPreferredHeight()));
    horizontalScaleButtonPanel.setBounds(
        juce::Rectangle<int>(0, area.getHeight() - (scaleButtonWidth + scrollBarWidth) + topStripHeight,
            scaleButtonWidth * 2, scaleButtonWidth));
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
