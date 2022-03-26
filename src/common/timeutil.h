#pragma once

using namespace juce;

namespace trackman {

namespace timeutil {

static const int ticksPerQuarterNote = 96;

static int secondsToTicks(float tempo, double seconds) {
    auto quarterNotesPerSecond = tempo / 60;
    return quarterNotesPerSecond * seconds * ticksPerQuarterNote;
}

static double ticksToSeconds(float tempo, int ticks) {
    auto quarterNotesPerSecond = tempo / 60;
    return (double)ticks / (double)ticksPerQuarterNote / (double)quarterNotesPerSecond;
}

static String formatSecsAsTime(double secs) {
    auto t = RelativeTime(secs);
    return ((int)t.inHours() > 0 ? String((int)t.inHours()) + ":" : "") + String((int)t.inMinutes() % 60) + ":" +
           String((int)t.inSeconds() % 60).paddedLeft('0', 2) + "." +
           String((int)t.inMilliseconds() % 1000).paddedLeft('0', 3);
}

} // namespace timeutil
} // namespace trackman
