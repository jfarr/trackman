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
    AddSampleCommand(DesktopController &desktopController, Track *track, juce::File file, int pos)
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
    MoveSampleCommand(TrackListController &controller, Sample &sample, double prevPos, double newPos)
        : Command("move sample"), controller(controller), sample(sample), prevPos(prevPos), newPos(newPos) {}
    ~MoveSampleCommand() {}

    void execute() override { controller.moveSample(sample, newPos); }
    void undo() override { controller.moveSample(sample, prevPos); }

  private:
    TrackListController &controller;
    Sample &sample;
    double prevPos;
    double newPos;
};
