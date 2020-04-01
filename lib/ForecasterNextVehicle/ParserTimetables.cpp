#include "ParserTimetables.hpp"

ForecasterNextVehicle ParserTimetables::makeForecasterInitial(
    const JsonObject& timetables,
    const tm& current) {
  // Read timetable on the previous day for late-night service, such as 24:00.
  tm previous = current;
  previous.tm_mday -= 1;
  mktime(&previous);
  ForecasterNextVehicle forecasterYesterday =
      makeForecaster(timetables, previous);
  if (forecasterYesterday.isInService(current)) {
    return forecasterYesterday;
  }

  // If the previous day and current timetable are the same, reuse the previous.
  if (GetKeyTimetable(timetables, current.tm_wday) ==
      GetKeyTimetable(timetables, forecasterYesterday.getTargetDateWday())) {
    forecasterYesterday.setTargetDate(current);
    if (forecasterYesterday.isInService(current)) {
      return forecasterYesterday;
    } else {
      tm next = current;
      next.tm_mday += 1;
      mktime(&next);
      return makeForecaster(timetables, next);
    }
  }

  // TODO: remove duplicated codes
  auto forecaster = makeForecaster(timetables, current);
  if (forecaster.isInService(current)) {
    return forecaster;
  } else {
    tm next = current;
    next.tm_mday += 1;
    mktime(&next);
    return makeForecaster(timetables, next);
  }
}

void ParserTimetables::makeForecasterCurrent(
    ForecasterNextVehicle& refForecaster,
    const JsonObject& timetables,
    const tm& current) {
  // If the previous day and current timetable are the same, reuse the previous.
  if (GetKeyTimetable(timetables, current.tm_wday) ==
      GetKeyTimetable(timetables, refForecaster.getTargetDateWday())) {
    refForecaster.setTargetDate(current);
  } else {
    refForecaster = makeForecaster(timetables, current);
  }
}

ForecasterNextVehicle ParserTimetables::makeForecaster(
    const JsonObject& timetables,
    const tm& current) {
  const JsonString key = GetKeyTimetable(timetables, current.tm_wday);
  auto timetable = makeTimetable(timetables[key]["timetable"]);
  std::sort(timetable.begin(), timetable.end());
  return ForecasterNextVehicle(std::move(timetable), current);
}

std::vector<TimeArrivalVehicle> ParserTimetables::makeTimetable(
    const JsonArray& times) {
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

JsonString ParserTimetables::GetKeyTimetable(const JsonObject& timetables,
                                             int wdayCurrent) {
  for (const auto& timetable : timetables) {
    for (const int wday :
         timetable.value().as<JsonObject>()["days"].as<JsonArray>()) {
      if (wday == wdayCurrent) {
        return timetable.key();
      }
    }
  }
  throw std::logic_error("Timetables or wdayCurrent may be wrong.");
}
