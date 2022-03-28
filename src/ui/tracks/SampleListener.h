#pragma once

#include <JuceHeader.h>

#include "model/Sample.h"
#include "model/Track.h"

namespace trackman {

class SampleThumbnail;

class SampleListener {
  public:
    virtual void sampleMoved(Track &track, Sample &sample, int x, int y) {}
    virtual void sampleResized(Sample &sample, int width) {}
};

}
