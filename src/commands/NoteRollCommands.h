#pragma once

#include "Command.h"
#include "model/Track.h"
#include "ui/desktop/DesktopController.h"

using namespace juce;

namespace trackman {

class AddNoteRollCommand : public Command {
  public:
    AddNoteRollCommand(DesktopController &desktopController, Track *track, NoteRoll *noteRoll)
        : Command("Record MIDI"), desktopController(desktopController), track(track), noteRoll(noteRoll) {}
    ~AddNoteRollCommand() override = default;

    void execute() override {}
    void undo() override { desktopController.deleteNoteRoll(*track, noteRoll); }

  private:
    DesktopController &desktopController;
    Track *track;
    NoteRoll *noteRoll = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AddNoteRollCommand)
};

class DeleteNoteRollCommand : public Command {
  public:
    DeleteNoteRollCommand(DesktopController &controller, Track &track, NoteRoll &noteRoll)
        : Command("Delete Note Roll"), controller(controller), track(track), noteRoll(noteRoll) {}
    ~DeleteNoteRollCommand() override = default;

    void execute() override { controller.deleteNoteRoll(track, &noteRoll); }
    void undo() override { controller.undeleteNoteRoll(track, &noteRoll); }

  private:
    DesktopController &controller;
    Track &track;
    NoteRoll &noteRoll;
};

class MoveNoteRollCommand : public Command {
  public:
    MoveNoteRollCommand(DesktopController &controller, NoteRoll &noteRoll, Track &fromTrack, Track *toTrack,
        double prevPos, double newPos)
        : Command("Move Note Roll"), controller(controller), noteRoll(noteRoll), fromTrack(fromTrack), toTrack(toTrack),
          prevPos(prevPos), newPos(newPos) {}
    ~MoveNoteRollCommand() override = default;

    void execute() override {
        if (toTrack == nullptr) {
            toTrack = controller.addTrack();
            newTrack = true;
        }
        controller.moveNoteRoll(noteRoll, fromTrack, *toTrack, newPos);
    }

    void undo() override {
        controller.moveNoteRoll(noteRoll, *toTrack, fromTrack, prevPos);
        if (newTrack) {
            controller.deleteTrack(toTrack, true);
        }
    }

  private:
    DesktopController &controller;
    NoteRoll &noteRoll;
    Track &fromTrack;
    Track *toTrack;
    bool newTrack = false;
    double prevPos;
    double newPos;
};

} // namespace trackman
