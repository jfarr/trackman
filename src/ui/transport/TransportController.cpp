#include "TransportController.h"
#include "ui/desktop/DesktopController.h"

namespace trackman {

TransportController::TransportController(DesktopController &desktopController)
    : desktopController(desktopController), transportPanel(desktopController) {}

void TransportController::selectionChanged(Track *track) {
    transportPanel.selectionChanged(track);
}

} // namespace trackman
