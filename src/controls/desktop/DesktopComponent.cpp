#include "DesktopComponent.h"
#include "common/listutil.h"

DesktopComponent::DesktopComponent(juce::DocumentWindow *parentWindow, juce::AudioFormatManager &formatManager)
    : formatManager(formatManager), desktopController(*parentWindow, formatManager),
      trackListViewport(desktopController.getTrackListController().getViewport()),
      mixerPanel(desktopController.getMixerController().getMixerPanel()) {

    addListener(&desktopController);

    addAndMakeVisible(trackListViewport);
    addAndMakeVisible(mixerPanel);

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
    closeAllWindows();

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
void DesktopComponent::paint(juce::Graphics &g) {
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void DesktopComponent::resized() {
    auto area = getLocalBounds();
    mixerPanel.setBounds(area.removeFromBottom(mixerPanel.getHeight()));
    trackListViewport.setBounds(area);
    desktopController.resize();
}

bool DesktopComponent::isInterestedInFileDrag(const juce::StringArray &files) { return true; }

void DesktopComponent::fileDragEnter(const juce::StringArray &files, int x, int y) {
    notifyFileDragEnter(files, x, y);
}

void DesktopComponent::fileDragMove(const juce::StringArray &files, int x, int y) {
    notifyFileDragMove(files, x, y);
}

void DesktopComponent::fileDragExit(const juce::StringArray &files) {
    notifyFileDragExit(files);
}

void DesktopComponent::filesDropped(const juce::StringArray &files, int x, int y) {
    notifyFilesDropped(files, x, y);
}

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
