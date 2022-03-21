#pragma once

#include <list>

#include "TimeRange.h"

using namespace std;

namespace trackman {

template <class T> class Timeline {
  public:
    Timeline() = default;
    Timeline(const Timeline &other) { timeRanges = other.timeRanges; }
    ~Timeline() = default;

    void reset() { timeRanges.clear(); }
    void addRange(double startTime, double endTime, T data) {
        timeRanges.push_back(TimeRange(startTime, endTime, data));
    }
    list<T> getAt(double t);

  private:
    list<TimeRange<T>> timeRanges;
};

template <class T> list<T> Timeline<T>::getAt(double t) {
    list<T> found;
    for (TimeRange<T> range : timeRanges) {
        if (range.contains(t)) {
            found.push_back(range.get());
        }
    }
    return found;
}

} // namespace trackman
