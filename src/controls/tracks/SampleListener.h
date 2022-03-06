#pragma once

#include "model/Sample.h"
#include "model/Track.h"

class SampleListener {
  public:
    virtual void sampleAdded(Track &track, juce::File file, int pos) {}
    virtual void sampleMoved(Sample &sample, juce::Point<int> pos) {}
};
