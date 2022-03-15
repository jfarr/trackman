#pragma once

#include <list>

#include "TimeRange.h"

template <class T> class Timeline {
  public:
    Timeline() = default;
    ~Timeline() = default;

    void reset() { timeRanges.clear(); }
    void addRange(double startTime, double endTime, T data) {
        timeRanges.push_back(TimeRange(startTime, endTime, data));
    }
    std::list<T> getAt(double t);

  private:
    std::list<TimeRange<T>> timeRanges;
};
