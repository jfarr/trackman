#pragma once

static const int ticksPerQuarterNote = 96;

static int secondsToTicks(float tempo, double seconds) {
    auto quarterNotesPerSecond = tempo / 60;
    return quarterNotesPerSecond * seconds * ticksPerQuarterNote;
}

static double ticksToSeconds(float tempo, int ticks) {
    auto quarterNotesPerSecond = tempo / 60;
    return (double)ticks / (double)ticksPerQuarterNote / (double)quarterNotesPerSecond;
}

static juce::String formatSecsAsTime(double secs) {
    auto t = juce::RelativeTime(secs);
    return ((int)t.inHours() > 0 ? juce::String((int)t.inHours()) + ":" : "") +
           juce::String((int)t.inMinutes() % 60) + ":" +
           juce::String((int)t.inSeconds() % 60).paddedLeft('0', 2) + "." +
           juce::String((int)t.inMilliseconds() % 1000).paddedLeft('0', 3);
}
