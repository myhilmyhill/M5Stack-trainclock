#include "TimeArrivalVehicle.hpp"

TimeArrivalVehicle::TimeArrivalVehicle(int h, int m)
: _h(h), _m(m), h(_h), m(_m) {
  if (!(0 <= h)) {
    throw std::invalid_argument("Hour must be positive.");
  }
  if (!(0 <= m && m <= 59)) {
    throw std::invalid_argument("Minute must be between 0 and 59.");
  }
}

TimeArrivalVehicle& TimeArrivalVehicle::operator=(const TimeArrivalVehicle& rhs) {
  _h = rhs.h;
  _m = rhs.m;
  return *this;
}

tm TimeArrivalVehicle::toTm(const tm& date) const {
  // Does not support DST time.
  tm returnTm = date;
  returnTm.tm_hour = h;
  returnTm.tm_min = m;
  returnTm.tm_sec = 0;
  mktime(&returnTm);
  return returnTm;
}
