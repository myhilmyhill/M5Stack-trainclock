# M5Stack-trainclock

A clock using M5stack to practice hardware and embedded programming.

## Usage

It is required to create your settings file at `src/settings.cpp` in the format of `src/settings.hpp`.
```c
const char* SETTINGS_WIFI_SSID = "ssid";
const char* SETTINGS_WIFI_PASS = "password";
...
```

## Timetables JSON Example

```json
{
  "timetables": {
    "weekday": {
      "days": [1, 2, 3, 4, 5,...],
      "timetable": [
        {
          "hour": 4,
          "minutes": [0, 3,...]
        },
        {
          "hour": 5,
          "minutes": [0, 3,...]
        },
        ...
      ]
    },
    "holiday": {
      "days": [0, 6,...],
      "timetable": [
        {
          "hour": 5,
          "minutes": [0, 3,...]
        },
        ...
      ]
    },
    ...
  }
}
```
