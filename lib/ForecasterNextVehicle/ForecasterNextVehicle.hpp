#include <ctime>
#include <vector>
#include "TimeArrivalVehicle.hpp"

class ForecasterNextVehicle {
private:
  const std::vector<TimeArrivalVehicle>& timetable;
  const tm targetDate;

public:
  ForecasterNextVehicle(
    const std::vector<TimeArrivalVehicle>& timetable,
    tm targetDate);
  tm getNextTimeFrom(const tm& currentTm) const;
  bool isInService(tm current) const;
};
