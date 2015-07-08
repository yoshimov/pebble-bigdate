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
    {-6, 10},
    {6, 10},
    {7, -35},
    {0, -45},
    {-7, -35}
  }
};
