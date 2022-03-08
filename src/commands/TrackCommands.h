#pragma once

#include <JuceHeader.h>

#include "Command.h"
#include "controls/tracks/TrackListController.h"
#include "model/Track.h"

class AddSampleCommand : public Command {
  public:
    AddSampleCommand(
        DesktopController &desktop, TrackListController &controller, Track *track, juce::File file, int pos)
        : Command("add sample"), desktop(desktop), controller(controller), track(track), file(file), pos(pos) {}
    ~AddSampleCommand() override {}

    void execute() override {
        if (track == nullptr) {
            juce::String name =
                juce::String("Track ") + juce::String::formatted(juce::String("%d"), controller.getTrackListSize() + 1);
            newTrack = desktop.addTrack(name);
            track = newTrack;
        }
        sample = controller.addSample(*track, file, pos);
    }
    void undo() override {
        controller.deleteSample(*track, sample);
        if (newTrack != nullptr) {
            desktop.deleteTrack(newTrack, true);
        }
    }

  private:
    DesktopController &desktop;
    TrackListController &controller;
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

    virtual void execute() override { controller.deleteSample(track, &sample); }
    void undo() override { controller.undeleteSample(track, &sample); }

  private:
    TrackListController &controller;
    Track &track;
    Sample &sample;
};
