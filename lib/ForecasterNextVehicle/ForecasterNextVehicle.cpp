#include <algorithm>
#include <stdexcept>
#include "ForecasterNextVehicle.hpp"

ForecasterNextVehicle::ForecasterNextVehicle(const std::vector<TimeArrivalVehicle>& timetable)
: timetable(timetable) {
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
