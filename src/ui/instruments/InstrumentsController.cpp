#include "InstrumentsController.h"
#include "ui/desktop/DesktopController.h"

InstrumentsController::InstrumentsController(DesktopController &desktopController)
    : desktopController(desktopController), project(desktopController.getProject()) {
    instrumentsPanel.resized();
}

InstrumentsController::~InstrumentsController() {}

void InstrumentsController::update() {
    instruments.clear();
    instrumentsPanel.clear();
    project.getTrackList().eachTrack([this](Track &track) {
        auto instrument = new InstrumentController(*this, track);
        instruments.push_back(std::unique_ptr<InstrumentController>(instrument));
        instrumentsPanel.addTrack(&instrument->getInstrumentControl());
        instrument->update();
    });
    instrumentsPanel.update();
}

void InstrumentsController::repaint() {
    for (auto &instrument : instruments) {
        instrument->repaint();
    }
}

void InstrumentsController::selectionChanged(Track *track) {
    desktopController.selectionChanged(track);
}
