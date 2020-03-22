#include "ParserTimetables.hpp"

std::vector<TimeArrivalVehicle> ParserTimetables::MakeForecasterFromJsonString(const JsonArray& times) {
  std::vector<TimeArrivalVehicle> timetable;

  // Parse JSON:
  // `[{"hour": hour, "minutes": [minute,...]},...]`
  for (const JsonObject& time : times) {
    const int hour = time["hour"];
    for (const int minute : time["minutes"].as<JsonArray>()) {
      timetable.push_back(TimeArrivalVehicle(hour, minute));
    }
  }

  return timetable;
}
