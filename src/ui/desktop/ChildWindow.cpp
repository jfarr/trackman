#include "ChildWindow.h"

namespace trackman {

ChildWindow::ChildWindow(const String &name, Component *component)
    : DocumentWindow(name,
          Desktop::getInstance().getDefaultLookAndFeel().findColour(ResizableWindow::backgroundColourId), closeButton,
          false) {
    Rectangle<int> area(0, 0, 300, 200);
    setBounds(area);
    setResizable(true, false);
    setUsingNativeTitleBar(false);
    setContentOwned(component, true);
}

void ChildWindow::closeButtonPressed() { delete this; }

} // namespace trackman
