#include "DesktopComponent.h"
#include "MainComponent.h"

DesktopComponent::DesktopComponent(juce::DocumentWindow *parentWindow)
{
    menuBar.reset(new juce::MenuBarComponent(this));
    addAndMakeVisible(menuBar.get());
    setApplicationCommandManagerToWatch(&commandManager);
    commandManager.registerAllCommandsForTarget(this);

    // this ensures that commands invoked on the application are correctly
    // forwarded to this component
    commandManager.setFirstCommandTarget(this);

    // this lets the command manager use keypresses that arrive in our window to send out commands
    addKeyListener(commandManager.getKeyMappings());

    addChildComponent(menuHeader);
    addAndMakeVisible(outerCommandTarget);
    addAndMakeVisible(sidePanel);
 
    setWantsKeyboardFocus(true);
    parentWindow->setMenuBarComponent(menuBar.get());

    setSize(450, 300);

    auto* window = new ChildWindow("child", new MainComponent());
    windows.add(window);

    juce::Rectangle<int> area(0, 0, 300, 200);

    window->setBounds(area);
    //window->setResizable(true, false);
    window->setUsingNativeTitleBar(false);

    addAndMakeVisible(window);
}

DesktopComponent::~DesktopComponent()
{
    closeAllWindows();
    menuBar.release();

#if JUCE_MAC
    juce::MenuBarModel::setMacMainMenu(nullptr);
#endif
    commandManager.setFirstCommandTarget(nullptr);
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
}
