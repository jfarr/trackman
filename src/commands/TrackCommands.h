#pragma once

#include <JuceHeader.h>

#include "Command.h"
#include "controls/tracks/TrackListController.h"
#include "model/Track.h"

class AddSampleCommand : public Command {
  public:
    AddSampleCommand(TrackListController &controller, Track &track, juce::File file, int pos)
        : Command("add sample"), controller(controller), track(track), file(file), pos(pos) {}
    ~AddSampleCommand() override {}

    void execute() override { sample = controller.addSample(track, file, pos); }
    void undo() override { controller.deleteSample(track, sample); }

  private:
    TrackListController &controller;
    Track &track;
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
    void undo() override {}

  private:
    TrackListController &controller;
    Track &track;
    Sample &sample;
};
