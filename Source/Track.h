#pragma once

#include "TrackControl.h"


class Track
{
public:
	Track(juce::String name);
	~Track();

	TrackControl& getTrackControl() { return trackControl; }

private:
	TrackControl trackControl;
};
