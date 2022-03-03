#include "MainWindow.h"

MainWindow::MainWindow(juce::String name, juce::AudioFormatManager &formatManager)
    : DocumentWindow(name,
          juce::Desktop::getInstance().getDefaultLookAndFeel().findColour(
              juce::ResizableWindow::backgroundColourId),
          DocumentWindow::allButtons) {
    setUsingNativeTitleBar(true);
    setContentOwned(new DesktopComponent(this, formatManager), true);

#if JUCE_IOS || JUCE_ANDROID
    setFullScreen(true);
#else
    setResizable(true, true);
    centreWithSize(getWidth(), getHeight());
#endif

    setVisible(true);
}

void MainWindow::closeButtonPressed() {
    // This is called when the user tries to close this window. Here,
    // we'll just ask the app to quit when this happens, but you can
    // change this to do whatever you need.
    juce::JUCEApplication::getInstance()->systemRequestedQuit();
}
