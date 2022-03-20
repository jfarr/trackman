#include "TransportController.h"
#include "ui/desktop/DesktopController.h"

namespace trackman {

TransportController::TransportController(DesktopController &desktopController)
    : desktopController(desktopController), transportPanel(desktopController) {}

} // namespace trackman
