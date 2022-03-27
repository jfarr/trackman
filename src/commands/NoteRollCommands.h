#pragma once

#include "Command.h"
#include "model/Track.h"
#include "ui/desktop/DesktopController.h"

using namespace juce;

namespace trackman {

class AddNoteRollCommand : public Command {
  public:
    AddNoteRollCommand(DesktopController &desktopController, Track *track, int pos)
        : Command("Add Sample"), desktopController(desktopController), track(track), pos(pos) {}
    ~AddNoteRollCommand() override = default;

    void execute() override {
//        if (track == nullptr) {
//            newTrack = desktopController.addTrack();
//            track = newTrack;
//        }
//        sample = desktopController.addSample(*track, file, pos);
    }
    void undo() override {
//        desktopController.deleteSample(*track, sample);
//        if (newTrack != nullptr) {
//            desktopController.deleteTrack(newTrack, true);
//        }
    }

  private:
    DesktopController &desktopController;
    Track *track;
    Track *newTrack = nullptr;
    int pos;
    NoteRoll *noteRoll = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AddNoteRollCommand)
};

}
