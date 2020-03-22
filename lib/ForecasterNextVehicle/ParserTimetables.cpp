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

JsonString ParserTimetables::GetKeyTimetable(const JsonObject& timetables, int wdayCurrent) {
  for (const auto& timetable : timetables) {
    for (const int wday : timetable.value().as<JsonObject>()["days"].as<JsonArray>()) {
      if (wday == wdayCurrent) {
        return timetable.key();
      }
    }
  }
  throw std::logic_error("Timetables or wdayCurrent may be wrong.");
}
