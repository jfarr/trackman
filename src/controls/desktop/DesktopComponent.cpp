#include "DesktopComponent.h"
#include "common/listutil.h"

DesktopComponent::DesktopComponent(juce::DocumentWindow *parentWindow, juce::AudioFormatManager &formatManager)
    : formatManager(formatManager), desktopController(*parentWindow, *this, deviceManager, formatManager),
      trackListViewport(desktopController.getTrackListController().getViewport()),
      mixerPanel(desktopController.getMixerController().getMixerPanel()), mixer(desktopController.getMixer()),
      timeMeter(desktopController.getProject()), horizontalScaleButtonPanel(false), verticalScaleButtonPanel(true) {

    setAudioChannels(0, 2);

    addListener(&desktopController);
    verticalScaleButtonPanel.addListener(&desktopController);
    horizontalScaleButtonPanel.addListener(&desktopController);

    trackListViewport.getHorizontalScrollBar().setColour(juce::ScrollBar::thumbColourId, juce::Colours::dimgrey);
    trackListViewport.getVerticalScrollBar().setColour(juce::ScrollBar::thumbColourId, juce::Colours::dimgrey);
    trackListViewport.getHorizontalScrollBar().setAutoHide(false);
    trackListViewport.getVerticalScrollBar().setAutoHide(false);
    addAndMakeVisible(timeMeter);
    addAndMakeVisible(trackListViewport);
    addAndMakeVisible(mixerPanel);
    addAndMakeVisible(verticalScaleButtonPanel);
    addAndMakeVisible(horizontalScaleButtonPanel);

    setApplicationCommandManagerToWatch(&commandManager);
    commandManager.registerAllCommandsForTarget(this);

    // this ensures that commands invoked on the application are correctly
    // forwarded to this component
    commandManager.setFirstCommandTarget(this);

    // this lets the command manager use keypresses that arrive in our window to
    // send out commands
    addKeyListener(commandManager.getKeyMappings());
    setWantsKeyboardFocus(true);

    parentWindow->setMenuBar(this);
    addAndMakeVisible(menuBar);

    setSize(800, 600);

#if JUCE_MAC
    MenuBarModel::setMacMainMenu(this);
#endif
}

DesktopComponent::~DesktopComponent() {
    shutdownAudio();
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

//==============================================================================
void DesktopComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate) {
    mixer.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void DesktopComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) {
    mixer.getNextAudioBlock(bufferToFill);
}

void DesktopComponent::releaseResources() { mixer.releaseResources(); }

//==============================================================================
void DesktopComponent::paint(juce::Graphics &g) {
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void DesktopComponent::resized() {
    auto area = getLocalBounds();
    auto topStripWidth = 20;
    auto scaleButtonPanelWidth = 24;
    auto scaleButtonPanelHeight = 12;
    auto scrollBarWidth = trackListViewport.getScrollBarThickness();
    timeMeter.setBounds(area.removeFromTop(topStripWidth));
    timeMeter.repaint();
    verticalScaleButtonPanel.setBounds(juce::Rectangle<int>(area.getWidth() - (scaleButtonPanelWidth + scrollBarWidth),
        area.getY(), scaleButtonPanelWidth, scaleButtonPanelHeight));
    mixerPanel.setBounds(area.removeFromBottom(mixerPanel.getPreferredHeight()));
    horizontalScaleButtonPanel.setBounds(
        juce::Rectangle<int>(0, area.getHeight(), scaleButtonPanelWidth, scaleButtonPanelHeight));
    trackListViewport.setBounds(area);
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
