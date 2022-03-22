#pragma once

namespace trackman {

template <class T> class TimeRange {
  public:
    TimeRange(double startTime, double endTime, T data) : startTime(startTime), endTime(endTime), data(data) {}
    TimeRange(const TimeRange &other) : startTime(other.startTime), endTime(other.endTime), data(other.data) {}
    ~TimeRange() = default;

    double getStartTime() const { return startTime; }
    double getEndTime() const { return endTime; }
    bool contains(double t) { return t >= startTime && t < endTime; }
    T get() { return data; }

  private:
    double startTime;
    double endTime;
    T data;
};

} // namespace trackman
