#ifdef UNIT_TEST

#include <Arduino.h>
#include <ArduinoJson.h>
#include <unity.h>

#include <ForecasterNextVehicle.hpp>
#include <ParserTimetables.hpp>
#include <iomanip>
#include <sstream>
#include <vector>

static const size_t jsonSize = 1024;

static ForecasterNextVehicle* forecasterWeekend;
static ForecasterNextVehicle* forecasterHoliday;

tm toTm(const char* string) {
  tm time;
  std::istringstream ss(string);
  ss >> std::get_time(&time, "%Y-%m-%d %H:%M:%S");
  mktime(&time);
  return time;
}

void makes() {
  static const char* json =
      "{"
      "  \"timetables\":{"
      "    \"weekday\": {"
      "      \"days\": [1,2,3,4,5],"
      "      \"timetable\" :["
      "        {\"hour\":22,\"minutes\": [33,40,50,59]},"
      "        {\"hour\":24,\"minutes\": [0,10,20,30,40,50]}"
      "      ]"
      "    },"
      "    \"holiday\": {"
      "      \"days\": [0,6],"
      "      \"timetable\" :["
      "        {\"hour\":23,\"minutes\": [33,40,50,59]},"
      "        {\"hour\":25,\"minutes\": [0,10,20,30,40,50]}"
      "      ]"
      "    }"
      "  }"
      "}";
  DynamicJsonDocument doc(1024);
  if (deserializeJson(doc, json) != DeserializationError::Ok) {
    TEST_FAIL_MESSAGE("DeserializationError");
    UNITY_END();
  }
  JsonObject timetables = doc.as<JsonObject>()["timetables"];

  // Thursday
  const tm currentWeekday = toTm("2020-03-26 12:34:56");
  TEST_ASSERT_EQUAL_INT(4, currentWeekday.tm_wday);
  static ForecasterNextVehicle _forecasterWeekday =
      ParserTimetables::makeForecasterInitial(timetables, currentWeekday);
  TEST_ASSERT_EQUAL_INT(currentWeekday.tm_wday,
                        _forecasterWeekday.getTargetDateWday());

  // Friday
  const tm currentWeekend = toTm("2020-03-27 12:34:56");
  TEST_ASSERT_EQUAL_INT(5, currentWeekend.tm_wday);
  static ForecasterNextVehicle _forecasterWeekend =
      ParserTimetables::makeForecasterInitial(timetables, currentWeekend);
  TEST_ASSERT_EQUAL_INT(currentWeekend.tm_wday,
                        _forecasterWeekend.getTargetDateWday());
  forecasterWeekend = &_forecasterWeekend;

  // Friday midnight
  const tm currentWeekendMidnight = toTm("2020-03-28 00:49:00");
  TEST_ASSERT_EQUAL_INT(6, currentWeekendMidnight.tm_wday);
  static ForecasterNextVehicle _forecasterMidnight =
      ParserTimetables::makeForecasterInitial(timetables,
                                              currentWeekendMidnight);
  TEST_ASSERT_EQUAL_INT(5, _forecasterMidnight.getTargetDateWday());

  // Saturday
  const tm currentHoliday = toTm("2020-03-28 12:34:56");
  TEST_ASSERT_EQUAL_INT(6, currentHoliday.tm_wday);
  static ForecasterNextVehicle _forecasterHoliday =
      ParserTimetables::makeForecasterInitial(timetables, currentHoliday);
  TEST_ASSERT_EQUAL_INT(currentHoliday.tm_wday,
                        _forecasterHoliday.getTargetDateWday());
  forecasterHoliday = &_forecasterHoliday;
}

void makes_no_services_on_midnight() {
  static const char* json =
      "{"
      "  \"timetables\":{"
      "    \"weekday\": {"
      "      \"days\": [0,1,2,3,4,5,6],"
      "      \"timetable\" :["
      "        {\"hour\":22,\"minutes\": [33,40,50,59]}"
      "      ]"
      "    }"
      "  }"
      "}";

  DynamicJsonDocument doc(1024);
  if (deserializeJson(doc, json) != DeserializationError::Ok) {
    TEST_FAIL_MESSAGE("DeserializationError");
    UNITY_END();
  }

  JsonObject timetables = doc.as<JsonObject>()["timetables"];

  // Out of services on Thursday
  const tm current = toTm("2020-03-26 22:59:00");
  TEST_ASSERT_EQUAL_INT(4, current.tm_wday);
  static ForecasterNextVehicle forecaster =
      ParserTimetables::makeForecasterInitial(timetables, current);
  TEST_ASSERT_EQUAL_INT(5, forecaster.getTargetDateWday());
}

void forecasts_before_start() {
  const tm current = toTm("2020-03-27 22:00:00");  // Friday
  TEST_ASSERT_TRUE(forecasterWeekend->isInService(current));
  const tm next = forecasterWeekend->getNextTimeFrom(current);
  TEST_ASSERT_EQUAL_INT(22, next.tm_hour);
  TEST_ASSERT_EQUAL_INT(33, next.tm_min);
}

void forecasts_in_service() {
  const tm ontime = toTm("2020-03-27 22:33:00");  // Friday
  TEST_ASSERT_TRUE(forecasterWeekend->isInService(ontime));
  const tm nextOnTime = forecasterWeekend->getNextTimeFrom(ontime);
  TEST_ASSERT_EQUAL_INT(22, nextOnTime.tm_hour);
  TEST_ASSERT_EQUAL_INT(40, nextOnTime.tm_min);

  const tm current = toTm("2020-03-27 22:34:00");  // Friday
  TEST_ASSERT_TRUE(forecasterWeekend->isInService(current));
  const tm next = forecasterWeekend->getNextTimeFrom(current);
  TEST_ASSERT_EQUAL_INT(22, next.tm_hour);
  TEST_ASSERT_EQUAL_INT(40, next.tm_min);

  const tm shortly = toTm("2020-03-27 22:39:00");  // Friday
  TEST_ASSERT_TRUE(forecasterWeekend->isInService(shortly));
  const tm nextShortly = forecasterWeekend->getNextTimeFrom(shortly);
  TEST_ASSERT_EQUAL_INT(next.tm_hour, nextShortly.tm_hour);
  TEST_ASSERT_EQUAL_INT(next.tm_min, nextShortly.tm_min);

  const tm last =
      toTm("2020-03-28 00:49:00");  // as Friday (on calendar, Saturday)
  TEST_ASSERT_TRUE(forecasterWeekend->isInService(last));
  const tm nextLast = forecasterWeekend->getNextTimeFrom(last);
  TEST_ASSERT_EQUAL_INT(0, nextLast.tm_hour);
  TEST_ASSERT_EQUAL_INT(50, nextLast.tm_min);
}

void forecasts_out_of_service() {
  const tm over =
      toTm("2020-03-28 00:50:00");  // as Friday (on calendar, Saturday)
  TEST_ASSERT_FALSE(forecasterWeekend->isInService(over));
  const tm nextLast = forecasterWeekend->getNextTimeFrom(over);
  TEST_ASSERT_EQUAL_INT(22, nextLast.tm_hour);
  TEST_ASSERT_EQUAL_INT(33, nextLast.tm_min);
}

void setup() {
  // NOTE!!! Wait for >2 secs
  // if board doesn't support software reset via Serial.DTR/RTS
  delay(2000);

  UNITY_BEGIN();
  RUN_TEST(makes);
  RUN_TEST(makes_no_services_on_midnight);
  RUN_TEST(forecasts_before_start);
  RUN_TEST(forecasts_in_service);
  RUN_TEST(forecasts_out_of_service);
  UNITY_END();
}

void loop() {
  // Do nothing
}

#endif
