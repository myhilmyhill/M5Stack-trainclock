#pragma once

#include <ArduinoJson.h>

#include <vector>

#include "ForecasterNextVehicle.hpp"
#include "TimeArrivalVehicle.hpp"

class ParserTimetables {
 public:
  static ForecasterNextVehicle makeForecasterInitial(
      const JsonObject& timetables,
      const tm& current);
  static void makeForecasterCurrent(ForecasterNextVehicle& forecasterOld,
                                    const JsonObject& timetables,
                                    const tm& current);
  static std::vector<TimeArrivalVehicle> makeTimetable(const JsonArray& times);
  static JsonString GetKeyTimetable(const JsonObject& timetables,
                                    int wdayCurrent);

 private:
  static ForecasterNextVehicle makeForecaster(const JsonObject& timetables,
                                              const tm& current);
};
