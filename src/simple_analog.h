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
  { {0, 20}, {0, 110} },
  { {0, 20}, {0, 110} }, // 0001
  { {0, 20}, {72, 30} }, // 0010
  { {0, 20}, {0, 110} }, // 0011
  { {72, 20}, {72, 110} }, // 0100
  { {0, 20}, {72, 110} }, // 0101
  { {0, 20}, {72, 30} }, // 0110
  {}, // 7
  { {72, 20}, {72, 110} }, // 1000
  { {0, 100}, {72, 110} }, // 1001
  { {72, 20}, {0, 110} }, // 1010
  {}, // 11
  { {72, 20}, {72, 110}} // 1100
};

#define KEY_SECOND 0
#define KEY_BACKGROUND 1
#define KEY_DATE_SIZE 2
#define KEY_WEEK_SIZE 3
#define KEY_SECOND_COLOR 10
#define KEY_DATE_COLOR 11
#define KEY_HOUR_COLOR 12
