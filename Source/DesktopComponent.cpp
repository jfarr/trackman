#include "DesktopComponent.h"

DesktopComponent::DesktopComponent(juce::DocumentWindow *parentWindow) : mixerComponent(formatManager) {
    addAndMakeVisible(mixerComponent);

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

    formatManager.registerBasicFormats();

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

    for (auto &track : tracks)
        delete track;
    tracks.clear();
}

void DesktopComponent::addNewTrack() {
    juce::String name = juce::String("Track ") + juce::String::formatted(juce::String("%d"), tracks.size() + 1);
    Track *newTrack = new Track(name);
    tracks.add(newTrack);
    mixerComponent.addTrack(*newTrack);
    addAndMakeVisible(newTrack->getTrackLaneControl());
    resized();
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
    mixerComponent.setBounds(area.removeFromBottom(mixerComponent.getHeight()));
    for (auto &track : tracks)
        track->getTrackLaneControl().setBounds(area.removeFromTop(track->getTrackLaneControl().getHeight()));
}
