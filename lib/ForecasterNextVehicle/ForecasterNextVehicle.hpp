#include <ctime>
#include <vector>
#include "TimeArrivalVehicle.hpp"

class ForecasterNextVehicle {
private:
  const std::vector<TimeArrivalVehicle>& timetable;

public:
  /**
   * \param timetable Caution: changing to be sorted.
   */
  ForecasterNextVehicle(std::vector<TimeArrivalVehicle>* timetable);
  tm getNextTimeFrom(const tm& currentTm) const;
};
