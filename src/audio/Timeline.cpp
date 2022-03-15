#include "Timeline.h"

template <class T> std::list<T> Timeline<T>::getAt(double t) {
    std::list<T> found;
    for (TimeRange<T> range : timeRanges) {
        if (range.contains(t)) {
            found.push_back(range.get());
        }
    }
    return found;
}
