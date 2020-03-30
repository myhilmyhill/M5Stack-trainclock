#ifdef UNIT_TEST

#include <Arduino.h>
#include <ArduinoJson.h>
#include <unity.h>

#include <ForecasterNextVehicle.hpp>
#include <ParserTimetables.hpp>
#include <iomanip>
#include <sstream>
#include <vector>

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
static const size_t jsonSize = 1024;

static ForecasterNextVehicle* forecasterWeekday;

tm toTm(const char* string) {
  tm time;
  std::istringstream ss(string);
  ss >> std::get_time(&time, "%Y-%m-%d %H:%M:%S");
  mktime(&time);
  return time;
}

void makes() {
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, json);
  JsonObject timetables = doc.as<JsonObject>()["timetables"];

  const tm current = toTm("2020-03-27 12:34:56");  // Friday
  TEST_ASSERT_EQUAL_INT(5, current.tm_wday);
  static ForecasterNextVehicle _forecasterWeekday =
      ParserTimetables::makeForecasterInitial(timetables, current);
  forecasterWeekday = &_forecasterWeekday;
}

void forecasts_before_start() {
  const tm current = toTm("2020-03-27 22:00:00");  // Friday
  TEST_ASSERT_TRUE(forecasterWeekday->isInService(current));
  const tm next = forecasterWeekday->getNextTimeFrom(current);
  TEST_ASSERT_EQUAL_INT(22, next.tm_hour);
  TEST_ASSERT_EQUAL_INT(33, next.tm_min);
}

void forecasts_in_service() {
  const tm ontime = toTm("2020-03-27 22:33:00");  // Friday
  TEST_ASSERT_TRUE(forecasterWeekday->isInService(ontime));
  const tm nextOnTime = forecasterWeekday->getNextTimeFrom(ontime);
  TEST_ASSERT_EQUAL_INT(22, nextOnTime.tm_hour);
  TEST_ASSERT_EQUAL_INT(40, nextOnTime.tm_min);

  const tm current = toTm("2020-03-27 22:34:00");  // Friday
  TEST_ASSERT_TRUE(forecasterWeekday->isInService(current));
  const tm next = forecasterWeekday->getNextTimeFrom(current);
  TEST_ASSERT_EQUAL_INT(22, next.tm_hour);
  TEST_ASSERT_EQUAL_INT(40, next.tm_min);

  const tm shortly = toTm("2020-03-27 22:39:00");  // Friday
  TEST_ASSERT_TRUE(forecasterWeekday->isInService(shortly));
  const tm nextShortly = forecasterWeekday->getNextTimeFrom(shortly);
  TEST_ASSERT_EQUAL_INT(next.tm_hour, nextShortly.tm_hour);
  TEST_ASSERT_EQUAL_INT(next.tm_min, nextShortly.tm_min);

  const tm last =
      toTm("2020-03-28 00:49:00");  // as Friday (on calendar, Saturday)
  TEST_ASSERT_TRUE(forecasterWeekday->isInService(last));
  const tm nextLast = forecasterWeekday->getNextTimeFrom(last);
  TEST_ASSERT_EQUAL_INT(22, nextLast.tm_hour);
  TEST_ASSERT_EQUAL_INT(33, nextLast.tm_min);
}

void forecasts_out_of_service() {
  const tm over =
      toTm("2020-03-28 00:50:00");  // as Friday (on calendar, Saturday)
  TEST_ASSERT_FALSE(forecasterWeekday->isInService(over));
}

void setup() {
  // NOTE!!! Wait for >2 secs
  // if board doesn't support software reset via Serial.DTR/RTS
  delay(2000);

  UNITY_BEGIN();
  RUN_TEST(makes);
  RUN_TEST(forecasts_before_start);
  RUN_TEST(forecasts_in_service);
  RUN_TEST(forecasts_out_of_service);
  UNITY_END();
}

void loop() {
  // Do nothing
}

#endif
