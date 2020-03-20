#include <algorithm>
#include <stdexcept>
#include "ForecasterNextVehicle.hpp"

ForecasterNextVehicle::ForecasterNextVehicle(std::vector<TimeArrivalVehicle>* timetable)
: timetable(*timetable) {
  std::sort(timetable->begin(), timetable->end());
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
