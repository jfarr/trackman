#include "DesktopComponent.h"

DesktopComponent::DesktopComponent(juce::DocumentWindow *parentWindow)
{
    addAndMakeVisible(mixerComponent);

    setApplicationCommandManagerToWatch(&commandManager);
    commandManager.registerAllCommandsForTarget(this);

    // this ensures that commands invoked on the application are correctly
    // forwarded to this component
    commandManager.setFirstCommandTarget(this);

    // this lets the command manager use keypresses that arrive in our window to send out commands
    addKeyListener(commandManager.getKeyMappings());
    setWantsKeyboardFocus(true);

    parentWindow->setMenuBar(this);
    addAndMakeVisible(menuBar);

    formatManager.registerBasicFormats();

    setSize(800, 600);
}

DesktopComponent::~DesktopComponent()
{
    closeAllWindows();

#if JUCE_MAC
    juce::MenuBarModel::setMacMainMenu(nullptr);
#endif
    commandManager.setFirstCommandTarget(nullptr);
}

void DesktopComponent::createChildWindow(const juce::String& name, juce::Component* component)
{
    auto* window = new ChildWindow(name, component);
    windows.add(window);
    addAndMakeVisible(window);
}

void DesktopComponent::closeAllWindows()
{
    for (auto& window : windows)
        window.deleteAndZero();

    windows.clear();
}
void DesktopComponent::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void DesktopComponent::resized()
{
    auto area = getLocalBounds();
    mixerComponent.setBounds(area.removeFromBottom(mixerComponent.getHeight()));
}
