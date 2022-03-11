#pragma once

#include "model/Sample.h"
#include "model/Track.h"

class SampleListener {
  public:
    virtual void sampleSelected(Track &track, Sample &sample) {}
    virtual void sampleMoved(Sample &sample, int x) {}
    virtual void sampleResized(Sample &sample, int width) {}
    virtual void dragEnded() {}
};
