#include "DesktopComponent.h"

DesktopComponent::DesktopComponent(juce::DocumentWindow *parentWindow, juce::AudioFormatManager &formatManager)
    : formatManager(formatManager), mixerPanel(desktopController.getTrackList(), mixer),
      desktopController(formatManager, mixer, mixerPanel, trackListPanel),
      trackListPanel(desktopController.getTrackList()) {
    addAndMakeVisible(&trackListViewport);
    trackListViewport.setSize(800, 350);
    trackListViewport.setScrollBarsShown(true, true);
    trackListViewport.setViewedComponent(&trackListPanel, false);
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
}
