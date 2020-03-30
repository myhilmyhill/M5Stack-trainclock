#pragma once

#include <ctime>
#include <vector>

#include "TimeArrivalVehicle.hpp"

class ForecasterNextVehicle {
 private:
  tm targetDate;
  std::vector<TimeArrivalVehicle> timetable;

 public:
  ForecasterNextVehicle(std::vector<TimeArrivalVehicle>&& timetable,
                        tm targetDate);
  tm getNextTimeFrom(const tm& currentTm) const;
  bool isInService(tm current) const;
  int getTargetDateWday() const { return targetDate.tm_wday; };
  void setTargetDate(const tm& current);
};
