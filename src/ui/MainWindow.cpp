#include "MainWindow.h"
#include "MainAudioComponent.h"

using namespace juce;

namespace trackman {

MainWindow::MainWindow(String name)
    : DocumentWindow(name,
          Desktop::getInstance().getDefaultLookAndFeel().findColour(ResizableWindow::backgroundColourId),
          DocumentWindow::allButtons),
      mainAudioComponent(*new MainAudioComponent(*this)) {
    setUsingNativeTitleBar(true);
    auto display = Desktop::getInstance().getDisplays().getPrimaryDisplay();
    if (display != nullptr) {
        auto r = display->userArea;
        setResizeLimits(250, 250, r.getWidth(), r.getHeight());
    }
    setContentOwned(&mainAudioComponent, true);

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
    JUCEApplication::getInstance()->systemRequestedQuit();
}

} // namespace trackman
