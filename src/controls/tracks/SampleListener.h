#pragma once

#include "SampleThumbnail.h"

class SampleListener {
  public:
    virtual void sampleDropped(SampleThumbnail * thumbnail, juce::Point<int> pos) = 0;
};
