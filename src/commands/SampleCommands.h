#pragma once

#include <JuceHeader.h>
#include <utility>

#include "Command.h"
#include "model/Track.h"
#include "ui/tracks/TrackListController.h"

using namespace juce;

namespace trackman {

class AddSampleCommand : public Command {
  public:
    AddSampleCommand(DesktopController &desktopController, Track *track, File file, int pos)
        : Command("Add Sample"), desktopController(desktopController), track(track), file(move(file)), pos(pos) {}
    ~AddSampleCommand() override = default;

    void execute() override {
        if (track == nullptr) {
            newTrack = desktopController.addTrack();
            track = newTrack;
        }
        sample = desktopController.addSample(*track, file, pos);
    }
    void undo() override {
        desktopController.deleteSample(*track, sample);
        if (newTrack != nullptr) {
            desktopController.deleteTrack(newTrack, true);
        }
    }

  private:
    DesktopController &desktopController;
    Track *track;
    Track *newTrack = nullptr;
    File file;
    int pos;
    Sample *sample = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AddSampleCommand)
};

class DeleteSampleCommand : public Command {
  public:
    DeleteSampleCommand(DesktopController &controller, Track &track, Sample &sample)
        : Command("Delete Sample"), controller(controller), track(track), sample(sample) {}
    ~DeleteSampleCommand() override = default;

    void execute() override { controller.deleteSample(track, &sample); }
    void undo() override { controller.undeleteSample(track, &sample); }

  private:
    DesktopController &controller;
    Track &track;
    Sample &sample;
};

class MoveSampleCommand : public Command {
  public:
    MoveSampleCommand(
        DesktopController &controller, Sample &sample, Track &fromTrack, Track *toTrack, double prevPos, double newPos)
        : Command("Move Sample"), controller(controller), sample(sample), fromTrack(fromTrack), toTrack(toTrack),
          prevPos(prevPos), newPos(newPos) {}
    ~MoveSampleCommand() override = default;

    void execute() override {
        if (toTrack == nullptr) {
            toTrack = controller.addTrack();
            newTrack = true;
        }
        controller.moveSample(sample, fromTrack, *toTrack, newPos);
    }

    void undo() override {
        controller.moveSample(sample, *toTrack, fromTrack, prevPos);
        if (newTrack) {
            controller.deleteTrack(toTrack, true);
        }
    }

  private:
    DesktopController &controller;
    Sample &sample;
    Track &fromTrack;
    Track *toTrack;
    bool newTrack = false;
    double prevPos;
    double newPos;
};

class ResizeSampleCommand : public Command {
  public:
    ResizeSampleCommand(TrackListController &controller, Sample &sample, double prevLen, double newLen)
        : Command("Resize Sample"), controller(controller), sample(sample), prevLen(prevLen), newLen(newLen) {}
    ~ResizeSampleCommand() override = default;

    void execute() override { controller.resizeSample(sample, newLen); }
    void undo() override { controller.resizeSample(sample, prevLen); }

  private:
    TrackListController &controller;
    Sample &sample;
    double prevLen;
    double newLen;
};

} // namespace trackman
