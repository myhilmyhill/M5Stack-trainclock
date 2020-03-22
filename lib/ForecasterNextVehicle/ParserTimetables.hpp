#include <ArduinoJson.h>
#include <vector>
#include "TimeArrivalVehicle.hpp"

class ParserTimetables {
public:
  static std::vector<TimeArrivalVehicle> MakeForecasterFromJsonString(const JsonArray& times);
  static JsonString GetKeyTimetable(const JsonObject& timetables, int wdayCurrent);
};
