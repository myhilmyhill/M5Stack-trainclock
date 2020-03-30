#ifdef UNIT_TEST

#include <Arduino.h>
#include <ArduinoJson.h>
#include <unity.h>

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

void parses() {
  DynamicJsonDocument doc(jsonSize);
  deserializeJson(doc, json);
  JsonObject object = doc.as<JsonObject>();
  TEST_ASSERT_NOT_NULL(&object);

  JsonArray jsonWeekday = object["timetables"]["weekday"]["timetable"];
  JsonArray jsonHoliday = object["timetables"]["holiday"]["timetable"];
  TEST_ASSERT_NOT_NULL(&jsonWeekday);
  TEST_ASSERT_NOT_NULL(&jsonHoliday);

  std::vector<TimeArrivalVehicle> weekday =
      ParserTimetables::makeTimetable(jsonWeekday);
  std::vector<TimeArrivalVehicle> holiday =
      ParserTimetables::makeTimetable(jsonHoliday);
  TEST_ASSERT_NOT_NULL(&weekday);
  TEST_ASSERT_NOT_NULL(&holiday);
  TEST_ASSERT_EQUAL_INT(22, weekday.front().h);
  TEST_ASSERT_EQUAL_INT(33, weekday.front().m);
  TEST_ASSERT_EQUAL_INT(24, weekday.back().h);
  TEST_ASSERT_EQUAL_INT(50, weekday.back().m);
  TEST_ASSERT_EQUAL_INT(23, holiday.front().h);
  TEST_ASSERT_EQUAL_INT(33, holiday.front().m);
  TEST_ASSERT_EQUAL_INT(25, holiday.back().h);
  TEST_ASSERT_EQUAL_INT(50, holiday.back().m);
}

void gets_key_from_date() {
  DynamicJsonDocument doc(jsonSize);
  deserializeJson(doc, json);
  const JsonObject timetables = doc.as<JsonObject>()["timetables"];
  TEST_ASSERT_NOT_NULL(&timetables);

  // Weekday
  {
    tm dateMon;
    std::istringstream ssMon("2020-03-27 12:34:56");  // Friday
    ssMon >> std::get_time(&dateMon, "%Y-%m-%d %H:%M:%S");
    mktime(&dateMon);
    TEST_ASSERT_EQUAL_INT(5, dateMon.tm_wday);
    TEST_ASSERT_EQUAL_STRING(
        "weekday",
        ParserTimetables::GetKeyTimetable(timetables, dateMon.tm_wday).c_str());
  }
  // Holiday
  {
    tm dateSat;
    std::istringstream ssSat("2020-03-28 12:34:56");  // Saturday
    ssSat >> std::get_time(&dateSat, "%Y-%m-%d %H:%M:%S");
    mktime(&dateSat);
    TEST_ASSERT_EQUAL_INT(6, dateSat.tm_wday);
    TEST_ASSERT_EQUAL_STRING(
        "holiday",
        ParserTimetables::GetKeyTimetable(timetables, dateSat.tm_wday).c_str());
  }
}

void setup() {
  // NOTE!!! Wait for >2 secs
  // if board doesn't support software reset via Serial.DTR/RTS
  delay(2000);

  UNITY_BEGIN();
  RUN_TEST(parses);
  RUN_TEST(gets_key_from_date);
  UNITY_END();
}

void loop() {
  // Do nothing
}

#endif
