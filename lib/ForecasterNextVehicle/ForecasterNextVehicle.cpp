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
  tm last = timetable.back().toTm(targetDate);
  return mktime(&current) < mktime(&last);
}

void ForecasterNextVehicle::setTargetDate(const tm& current) {
  if (!isInService(current)) {
    targetDate = current;
  }
}
