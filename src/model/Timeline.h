#pragma once

#include <list>

#include "TimeRange.h"

using namespace std;

namespace trackman {

template <class T, class P> class Timeline {
  public:
    Timeline() = default;
    Timeline(const Timeline &other) { timeRanges = other.timeRanges; }
    ~Timeline() = default;

    void reset() { timeRanges.clear(); }
    void addRange(T startTime, T endTime, P data) {
        timeRanges.push_back(TimeRange(startTime, endTime, data));
    }
    list<P> getAt(T t);

  private:
    list<TimeRange<T, P>> timeRanges;
};

template <class T, class P> list<P> Timeline<T, P>::getAt(T t) {
    list<P> found;
    for (auto range : timeRanges) {
        if (range.contains(t)) {
            found.push_back(range.get());
        }
    }
    return found;
}

} // namespace trackman
