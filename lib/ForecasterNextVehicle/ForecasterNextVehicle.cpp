#include "ForecasterNextVehicle.hpp"

#include <algorithm>
#include <stdexcept>

ForecasterNextVehicle::ForecasterNextVehicle(
    std::vector<TimeArrivalVehicle>&& timetable,
    tm targetDate)
    : targetDate(targetDate), timetable(timetable) {
  if (!std::is_sorted(timetable.cbegin(), timetable.cend())) {
    std::invalid_argument("Timetable must be sorted.");
  }
}

tm ForecasterNextVehicle::getNextTimeFrom(tm current) const {
  const time_t currentTime = mktime(&current);
  for (const auto& item : timetable) {
    tm time = item.toTm(targetDate);
    if (currentTime < mktime(&time)) {
      return item.toTm(current);
    }
  }
  return timetable.front().toTm(current);
}

bool ForecasterNextVehicle::isInService(tm current) const {
  tm last = timetable.back().toTm(targetDate);
  return mktime(&current) < mktime(&last);
}

void ForecasterNextVehicle::setTargetDate(const tm& current) {
  if (!isInService(current)) {
    targetDate = current;
  }
}
