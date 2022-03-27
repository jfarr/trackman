#pragma once

namespace trackman {

template <class T, class P> class TimeRange {
  public:
    TimeRange(T startTime, T endTime, P data) : startTime(startTime), endTime(endTime), data(data) {}
    TimeRange(const TimeRange &other) : startTime(other.startTime), endTime(other.endTime), data(other.data) {}
    ~TimeRange() = default;

    T getStartTime() const { return startTime; }
    T getEndTime() const { return endTime; }
    bool contains(T t) { return t >= startTime && t < endTime; }
    P get() { return data; }

  private:
    T startTime;
    T endTime;
    P data;
};

} // namespace trackman
