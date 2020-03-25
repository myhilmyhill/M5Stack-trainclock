#ifdef UNIT_TEST

#include <Arduino.h>
#include <unity.h>
#include <iomanip>
#include <sstream>
#include <TimeArrivalVehicle.hpp>

void assigns()
{
  TimeArrivalVehicle time(12, 34);
  TimeArrivalVehicle timeCopied = time;
  TEST_ASSERT(&time != &timeCopied);
  TEST_ASSERT_TRUE(time == timeCopied);

  TimeArrivalVehicle timeAssigned(0, 0);
  timeAssigned = time;
  TEST_ASSERT(&time != &timeAssigned);
  TEST_ASSERT_TRUE(time == timeAssigned);
}

void compares_by_operators()
{
  TimeArrivalVehicle time(0, 0);
  TimeArrivalVehicle time0000(0, 0);
  TimeArrivalVehicle time0001(0, 1);
  TimeArrivalVehicle time0100(1, 0);
  TEST_ASSERT_TRUE(time == time0000);
  TEST_ASSERT_TRUE(time != time0001);
  TEST_ASSERT_TRUE(time != time0100);
  TEST_ASSERT_TRUE(time0000 < time0001);
  TEST_ASSERT_TRUE(time0000 < time0100);
  TEST_ASSERT_TRUE(time0001 < time0100);
  TEST_ASSERT_TRUE(time0001 > time0000);
  TEST_ASSERT_TRUE(time0100 > time0000);
  TEST_ASSERT_TRUE(time0100 > time0001);
  TEST_ASSERT_TRUE(time <= time0000);
  TEST_ASSERT_TRUE(time <= time0001);
  TEST_ASSERT_TRUE(time0000 >= time);
  TEST_ASSERT_TRUE(time0001 >= time);
}

void converts_to_tm_within_the_month()
{
  tm date;
  std::istringstream ss("2020-03-26 12:34:56");
  ss >> std::get_time(&date, "%Y-%m-%d %H:%M:%S");
  TEST_ASSERT_EQUAL_INT(2020 - 1900, date.tm_year);
  TEST_ASSERT_EQUAL_INT(3 - 1, date.tm_mon);
  TEST_ASSERT_EQUAL_INT(26, date.tm_mday);
  TEST_ASSERT_EQUAL_INT(12, date.tm_hour);
  TEST_ASSERT_EQUAL_INT(34, date.tm_min);
  TEST_ASSERT_EQUAL_INT(56, date.tm_sec);

  // On the same day
  TimeArrivalVehicle time(0, 0);
  tm timeToTm = time.toTm(date);
  TEST_ASSERT_EQUAL_INT(date.tm_year, timeToTm.tm_year);
  TEST_ASSERT_EQUAL_INT(date.tm_mon, timeToTm.tm_mon);
  TEST_ASSERT_EQUAL_INT(date.tm_mday, timeToTm.tm_mday);
  TEST_ASSERT_EQUAL_INT(0, timeToTm.tm_hour);
  TEST_ASSERT_EQUAL_INT(0, timeToTm.tm_min);
  TEST_ASSERT_EQUAL_INT(0, timeToTm.tm_sec);

  // Over midnight within the month
  TimeArrivalVehicle timeOvernight(24, 0);
  tm timeToTmOvernight = timeOvernight.toTm(date);
  TEST_ASSERT_EQUAL_INT(date.tm_year, timeToTmOvernight.tm_year);
  TEST_ASSERT_EQUAL_INT(date.tm_mon, timeToTmOvernight.tm_mon);
  TEST_ASSERT_EQUAL_INT(date.tm_mday + 1, timeToTmOvernight.tm_mday);
  TEST_ASSERT_EQUAL_INT(0, timeToTmOvernight.tm_hour);
  TEST_ASSERT_EQUAL_INT(0, timeToTmOvernight.tm_min);
  TEST_ASSERT_EQUAL_INT(0, timeToTmOvernight.tm_sec);
}

void converts_to_tm_over_midnight_over_the_month()
{
  tm dateEndMonth;
  std::istringstream ssEndMonth("2020-03-31 12:34:56");
  ssEndMonth >> std::get_time(&dateEndMonth, "%Y-%m-%d %H:%M:%S");
  TimeArrivalVehicle timeEndMonth(24, 1);
  tm timeToTmEndMonth = timeEndMonth.toTm(dateEndMonth);
  TEST_ASSERT_EQUAL_INT(dateEndMonth.tm_year, timeToTmEndMonth.tm_year);
  TEST_ASSERT_EQUAL_INT(dateEndMonth.tm_mon + 1, timeToTmEndMonth.tm_mon);
  TEST_ASSERT_EQUAL_INT(1, timeToTmEndMonth.tm_mday);
  TEST_ASSERT_EQUAL_INT(0, timeToTmEndMonth.tm_hour);
  TEST_ASSERT_EQUAL_INT(1, timeToTmEndMonth.tm_min);
  TEST_ASSERT_EQUAL_INT(0, timeToTmEndMonth.tm_sec);
}

void converts_to_tm_over_midnight_on_leap_day()
{
  tm dateLeapDay;
  std::istringstream ssLeapDay("2020-02-28 12:34:56");
  ssLeapDay >> std::get_time(&dateLeapDay, "%Y-%m-%d %H:%M:%S");
  TimeArrivalVehicle timeLeapDay(24, 2);
  tm timeToTmLeapDay = timeLeapDay.toTm(dateLeapDay);
  TEST_ASSERT_EQUAL_INT(dateLeapDay.tm_year, timeToTmLeapDay.tm_year);
  TEST_ASSERT_EQUAL_INT(dateLeapDay.tm_mon, timeToTmLeapDay.tm_mon);
  TEST_ASSERT_EQUAL_INT(dateLeapDay.tm_mday + 1, timeToTmLeapDay.tm_mday);
  TEST_ASSERT_EQUAL_INT(0, timeToTmLeapDay.tm_hour);
  TEST_ASSERT_EQUAL_INT(2, timeToTmLeapDay.tm_min);
  TEST_ASSERT_EQUAL_INT(0, timeToTmLeapDay.tm_sec);
}

void setup()
{
  // NOTE!!! Wait for >2 secs
  // if board doesn't support software reset via Serial.DTR/RTS
  delay(2000);

  UNITY_BEGIN();
  RUN_TEST(assigns);
  RUN_TEST(compares_by_operators);
  RUN_TEST(converts_to_tm_within_the_month);
  RUN_TEST(converts_to_tm_over_midnight_over_the_month);
  RUN_TEST(converts_to_tm_over_midnight_on_leap_day);
  UNITY_END();
}

void loop()
{
  // Do nothing
}

#endif
