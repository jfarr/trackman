#pragma once

#include "TrackControl.h"
#include "TrackLaneControl.h"


class Track
{
public:
	Track(juce::String name);
	~Track();

	TrackControl& getTrackControl() { return trackControl; }
	TrackLaneControl& getTrackLaneControl() { return trackLaneControl; }

private:
	TrackControl trackControl;
	TrackLaneControl trackLaneControl;
};
