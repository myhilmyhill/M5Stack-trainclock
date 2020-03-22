#include <algorithm>
#include <stdexcept>
#include "ForecasterNextVehicle.hpp"

ForecasterNextVehicle::ForecasterNextVehicle(
  const std::vector<TimeArrivalVehicle>& timetable,
  tm targetDate)
: timetable(timetable), targetDate(targetDate) {
  if (!std::is_sorted(timetable.cbegin(), timetable.cend())) {
    std::invalid_argument("Timetable must be sorted.");
  }
}

tm ForecasterNextVehicle::getNextTimeFrom(const tm& currentTm) const {
  const TimeArrivalVehicle current(currentTm.tm_hour, currentTm.tm_min);
  for (auto& item : timetable) {
    if (current < item) {
      return item.toTm(currentTm);
    }
  }
  return timetable.front().toTm(currentTm);
}

bool ForecasterNextVehicle::isInService(tm current) const {
  current.tm_hour = 0;
  current.tm_min = 0;
  current.tm_sec = 0;

  tm target = targetDate;
  target.tm_hour = 0;
  target.tm_min = 0;
  target.tm_sec = 0;

  return mktime(&target) < mktime(&current);
}
