#include "DesktopComponent.h"

DesktopComponent::DesktopComponent(juce::DocumentWindow *parentWindow, juce::AudioFormatManager &formatManager)
    : formatManager(formatManager), desktopController(*parentWindow, formatManager),
      trackListViewport(desktopController.getTrackListController().getViewport()),
      mixerPanel(desktopController.getMixerController().getMixerPanel()) {
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

bool DesktopComponent::isInterestedInFileDrag (const juce::StringArray& files) {
    return true;
}

void DesktopComponent::fileDragEnter (const juce::StringArray& files, int x, int y) {
    std::cout << "drag enter: " << files.joinIntoString(",").toStdString() << "\n";
}

void DesktopComponent::fileDragMove (const juce::StringArray& files, int x, int y) {

}

void DesktopComponent::fileDragExit (const juce::StringArray& files) {
    std::cout << "drag exit: " << files.joinIntoString(",").toStdString() << "\n";
}

void DesktopComponent::filesDropped (const juce::StringArray& files, int x, int y) {
    std::cout << "files dropped: " << files.joinIntoString(",").toStdString() << "\n";
}
