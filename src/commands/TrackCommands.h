#pragma once

#include <JuceHeader.h>

#include <utility>

#include "Command.h"
#include "model/Track.h"
#include "ui/tracks/TrackListController.h"

class RenameTrackCommand : public Command {
  public:
    RenameTrackCommand(trackman::DesktopController &desktopController, Track &track, juce::String newName)
        : Command("Rename Track"), desktopController(desktopController), track(track), newName(std::move(newName)),
          prevName(track.getName()) {}
    ~RenameTrackCommand() override = default;

    void execute() override { desktopController.renameTrack(track, newName); }
    void undo() override { desktopController.renameTrack(track, prevName); }

  private:
    trackman::DesktopController &desktopController;
    Track &track;
    juce::String newName;
    juce::String prevName;
};

class AddSampleCommand : public Command {
  public:
    AddSampleCommand(trackman::DesktopController &desktopController, Track *track, juce::File file, int pos)
        : Command("Add Sample"), desktopController(desktopController), track(track), file(std::move(file)), pos(pos) {}
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
    trackman::DesktopController &desktopController;
    Track *track;
    Track *newTrack = nullptr;
    juce::File file;
    int pos;
    Sample *sample = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AddSampleCommand)
};

class DeleteSampleCommand : public Command {
  public:
    DeleteSampleCommand(trackman::DesktopController &controller, Track &track, Sample &sample)
        : Command("Delete Sample"), controller(controller), track(track), sample(sample) {}
    ~DeleteSampleCommand() override = default;

    void execute() override { controller.deleteSample(track, &sample); }
    void undo() override { controller.undeleteSample(track, &sample); }

  private:
    trackman::DesktopController &controller;
    Track &track;
    Sample &sample;
};

class MoveSampleCommand : public Command {
  public:
    MoveSampleCommand(trackman::DesktopController &controller, Sample &sample, Track &fromTrack, Track *toTrack,
        double prevPos, double newPos)
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
    trackman::DesktopController &controller;
    Sample &sample;
    Track &fromTrack;
    Track *toTrack;
    bool newTrack = false;
    double prevPos;
    double newPos;
};

class ResizeSampleCommand : public Command {
  public:
    ResizeSampleCommand(trackman::TrackListController &controller, Sample &sample, double prevLen, double newLen)
        : Command("Resize Sample"), controller(controller), sample(sample), prevLen(prevLen), newLen(newLen) {}
    ~ResizeSampleCommand() override = default;

    void execute() override { controller.resizeSample(sample, newLen); }
    void undo() override { controller.resizeSample(sample, prevLen); }

  private:
    trackman::TrackListController &controller;
    Sample &sample;
    double prevLen;
    double newLen;
};
