#include <ctime>
#include <vector>
#include "TimeArrivalVehicle.hpp"

class ForecasterNextVehicle {
private:
  const std::vector<TimeArrivalVehicle>& timetable;

public:
  ForecasterNextVehicle(const std::vector<TimeArrivalVehicle>& timetable);
  tm getNextTimeFrom(const tm& currentTm) const;
};
