#pragma once

#include <JuceHeader.h>

#include "model/Sample.h"
#include "model/Track.h"

namespace trackman {

class SampleThumbnail;

class SampleListener {
  public:
    virtual void sampleSelected(Track &track, Sample &sample) {}
    virtual void sampleMoved(Track &track, Sample &sample, int x, int y) {}
    virtual void sampleResized(Sample &sample, int width) {}
    virtual void mouseDragged(SampleThumbnail &thumbnail, int x, int screenY) {}
    virtual void dragEnded() {}
};

}
