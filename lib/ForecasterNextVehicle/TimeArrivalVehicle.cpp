#include "TimeArrivalVehicle.hpp"

TimeArrivalVehicle::TimeArrivalVehicle(int h, int m)
: h(h), m(m) {
  if (!(0 <= h)) {
    throw std::invalid_argument("Hour must be positive.");
  }
  if (!(0 <= m && m <= 59)) {
    throw std::invalid_argument("Minute must be between 0 and 59.");
  }
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
