#pragma once

#include "pebble.h"

static const GPathInfo MINUTE_HAND_POINTS = {
  5,
  (GPoint []) {
    { -5, 10 },
    { 5, 10 },
    { 6, -60},
    { 0, -70 },
    { -6, -60}
  }
};

static const GPathInfo HOUR_HAND_POINTS = {
  5, (GPoint []){
    {-5, 10},
    {5, 10},
    {7, -35},
    {0, -45},
    {-7, -35}
  }
};

static const GPoint TEXT_POINTS[13][2] = {
  { {0, 25}, {0, 100} },
  { {0, 25}, {0, 100} }, // 0001
  { {0, 25}, {72, 25} }, // 0010
  { {0, 25}, {0, 100} }, // 0011
  { {72, 25}, {72, 100} }, // 0100
  { {0, 25}, {72, 100} }, // 0101
  { {0, 25}, {72, 25} }, // 0110
  {}, // 7
  { {72, 25}, {72, 100} }, // 1000
  { {0, 100}, {72, 100} }, // 1001
  { {72, 25}, {0, 100} }, // 1010
  {}, // 11
  { {72, 25}, {72, 100}} // 1100
};

#define KEY_SECOND 0
#define KEY_BACKGROUND 1
#define KEY_DATE_SIZE 2
#define KEY_WEEK_SIZE 3
#define KEY_SECOND_COLOR 10
#define KEY_DATE_COLOR 11
#define KEY_HOUR_COLOR 12
