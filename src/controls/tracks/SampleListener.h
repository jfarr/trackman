#pragma once

#include <JuceHeader.h>

#include "model/Sample.h"
#include "model/Track.h"

class SampleListener {
  public:
    virtual void sampleSelected(Track &track, Sample &sample) {}
    virtual void sampleMoved(Track &track, Sample &sample, int x, int y) {}
    virtual void sampleResized(Sample &sample, int width) {}
    virtual void dragEnded() {}
};
