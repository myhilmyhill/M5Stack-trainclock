#pragma once

#include <stdexcept>

class TimeArrivalVehicle {
private:
  int _h;
  int _m;

public:
  const int& h;
  const int& m;

  TimeArrivalVehicle(int h, int m);
  TimeArrivalVehicle(const TimeArrivalVehicle& other)
  : TimeArrivalVehicle(other.h, other.m) {}

  TimeArrivalVehicle& operator=(const TimeArrivalVehicle& rhs);
  tm toTm(const tm& date) const;

  bool operator==(const TimeArrivalVehicle& rhs) const {
    return h == rhs.h && m == rhs.m;
  }

  bool operator!=(const TimeArrivalVehicle& rhs) const {
    return !(*this == rhs);
  }

  bool operator<(const TimeArrivalVehicle& rhs) const {
    return h * 100 + m < rhs.h * 100 + rhs.m;
  }

  bool operator>(const TimeArrivalVehicle& rhs) const {
    return !(*this < rhs) && *this != rhs;
  }

  bool operator>=(const TimeArrivalVehicle& rhs) const {
    return !(*this < rhs);
  }

  bool operator<=(const TimeArrivalVehicle& rhs) const {
    return !(*this > rhs);
  }
};
