#pragma once

static const int ticksPerQuarterNote = 96;

static int secondsToTicks(float tempo, double seconds) {
    auto quarterNotesPerSecond = tempo / 60;
    return quarterNotesPerSecond * seconds * ticksPerQuarterNote;
}

static double ticksToSeconds(float tempo, int ticks) {
    auto quarterNotesPerSecond = tempo / 60;
    return ticks / ticksPerQuarterNote / quarterNotesPerSecond;
}
