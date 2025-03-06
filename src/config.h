#pragma once

#include <pebble.h>

#define SETTINGS_KEY 1

#define MINUTES_COLOR PBL_IF_COLOR_ELSE(GColorSunsetOrange, GColorBlack)
#define HOURS_COLOR PBL_IF_COLOR_ELSE(GColorJaegerGreen, GColorDarkGray)
#define SECONDS_COLOR PBL_IF_COLOR_ELSE(GColorChromeYellow, GColorDarkGray)
#define BG_COLOR GColorWhite

#define DATE_COLOR_FG PBL_IF_COLOR_ELSE(GColorBlueMoon, GColorBlack)

#define DOTS_INSET PBL_IF_ROUND_ELSE(18, 9)
#define BATTERY_INSET PBL_IF_ROUND_ELSE(7, 0)
#define MINUTES_INSET PBL_IF_ROUND_ELSE(20, 12)

// A structure containing our settings
typedef struct ClaySettings {
  GColor BackgroundColor;
  GColor SecondsColor;
  GColor HoursColor;
  GColor MinutesColor;
  GColor DateColor;
  bool ShowSeconds;
  bool ShowDate;
  bool RemoveBorder;
} __attribute__((__packed__)) ClaySettings;

static const GPathInfo HOUR_HAND_POINTS = {
  3, (GPoint []){
    {-18, 0},
    {18, 0},
    {0, -50}
  }
};

static const GPathInfo SECOND_HAND_POINTS = {
  16, (GPoint []){
    {1, 0},
    {14, -18},
    {14, -19},
    {0, -31},
    {11, -44},
    {11, -45},
    {-3, -59},
    {-5, -58},
    {-5, -57},
    {7, -45},
    {7, -44},
    {-4, -31},
    {-4, -30},
    {10, -18},
    {10, -17},
    {-1, -2},
  }
};