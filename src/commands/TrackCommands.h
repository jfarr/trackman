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

    virtual void execute() override { sample = controller.addSample(track, file, pos); }
    void undo() override { controller.deleteSample(track, sample); }

  private:
    TrackListController &controller;
    Track &track;
    juce::File file;
    int pos;
    Sample *sample;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AddSampleCommand)
};
