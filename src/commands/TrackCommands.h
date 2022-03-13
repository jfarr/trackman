#pragma once

#include <JuceHeader.h>

#include "Command.h"
#include "controls/tracks/TrackListController.h"
#include "model/Track.h"

class RenameTrackCommand : public Command {
  public:
    RenameTrackCommand(DesktopController &desktopController, Track &track, juce::String newName)
        : Command("rename track"), desktopController(desktopController), track(track), newName(newName),
          prevName(track.getName()) {}
    ~RenameTrackCommand() {}

    void execute() override { desktopController.renameTrack(track, newName); }
    void undo() override { desktopController.renameTrack(track, prevName); }

  private:
    DesktopController &desktopController;
    Track &track;
    juce::String newName;
    juce::String prevName;
};

class AddSampleCommand : public Command {
  public:
    AddSampleCommand(DesktopController &desktopController, Track *track, const juce::File &file, int pos)
        : Command("add sample"), desktopController(desktopController), track(track), file(file), pos(pos) {}
    ~AddSampleCommand() override {}

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
    Track *newTrack;
    juce::File file;
    int pos;
    Sample *sample;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AddSampleCommand)
};

class DeleteSampleCommand : public Command {
  public:
    DeleteSampleCommand(TrackListController &controller, Track &track, Sample &sample)
        : Command("delete sample"), controller(controller), track(track), sample(sample) {}
    ~DeleteSampleCommand() {}

    void execute() override { controller.deleteSample(track, &sample); }
    void undo() override { controller.undeleteSample(track, &sample); }

  private:
    TrackListController &controller;
    Track &track;
    Sample &sample;
};

class MoveSampleCommand : public Command {
  public:
    MoveSampleCommand(DesktopController &controller, Sample &sample, Track &fromTrack, Track *toTrack, double prevPos,
        double newPos)
        : Command("move sample"), controller(controller), sample(sample), fromTrack(fromTrack), toTrack(toTrack),
          prevPos(prevPos), newPos(newPos) {}
    ~MoveSampleCommand() {}

    void execute() override {
        if (toTrack == nullptr) {
            toTrack = controller.addTrack();
            newTrack = true;
        }
        controller.getTrackListController().moveSample(sample, fromTrack, *toTrack, newPos);
    }

    void undo() override {
        controller.getTrackListController().moveSample(sample, *toTrack, fromTrack, prevPos);
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
        : Command("resize sample"), controller(controller), sample(sample), prevLen(prevLen), newLen(newLen) {}
    ~ResizeSampleCommand() {}

    void execute() override { controller.resizeSample(sample, newLen); }
    void undo() override { controller.resizeSample(sample, prevLen); }

  private:
    TrackListController &controller;
    Sample &sample;
    double prevLen;
    double newLen;
};
