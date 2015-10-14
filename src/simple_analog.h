#pragma once

#include "pebble.h"

// thin style
static const GPathInfo THIN_MINUTE_HAND_POINTS = {
  5,
  (GPoint []) {
    { -3, 10 },
    { 3, 10 },
    { 3, -65},
    { 0, -70 },
    { -3, -65}
  }
};

static const GPathInfo THIN_HOUR_HAND_POINTS = {
  5, (GPoint []){
    {-3, 10},
    {3, 10},
    {3, -40},
    {0, -45},
    {-3, -40}
  }
};

// normal style
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

// bold style
static const GPathInfo BOLD_MINUTE_HAND_POINTS = {
  5,
  (GPoint []) {
    { -7, 10 },
    { 7, 10 },
    { 8, -60},
    { 0, -70 },
    { -8, -60}
  }
};

static const GPathInfo BOLD_HOUR_HAND_POINTS = {
  5, (GPoint []){
    {-7, 10},
    {7, 10},
    {9, -35},
    {0, -45},
    {-9, -35}
  }
};

// flying style
static const GPathInfo FLY_MINUTE_HAND_POINTS = {
  5,
  (GPoint []) {
    { -6, -15 },
    { 6, -15 },
    { 8, -50},
    { 0, -70 },
    { -8, -50}
  }
};

static const GPathInfo FLY_HOUR_HAND_POINTS = {
  5, (GPoint []){
    {-6, -15},
    {6, -15},
    {9, -35},
    {0, -45},
    {-9, -35}
  }
};

// pyramid style
static const GPathInfo PYRAMID_MINUTE_HAND_POINTS = {
  11,
  (GPoint []) {
    { -3, 10 },
    { 3, 10 },
    { 3, -40},
    { 10, -40},
    { 3, -50},
    { 2, -65},
    { 0, -70 },
    { -2, -65},
    { -3, -50},
    { -10, -40},
    { -3, -40}
  }
};

static const GPathInfo PYRAMID_HOUR_HAND_POINTS = {
  11, (GPoint []){
    { -3, 10 },
    { 3, 10 },
    { 3, -20},
    { 10, -20},
    { 3, -30},
    { 2, -40},
    { 0, -45 },
    { -2, -40},
    { -3, -30},
    { -10, -20},
    { -3, -20}
  }
};

// hand style index
static const GPathInfo *HAND_STYLES[5][2] = {
  {&THIN_MINUTE_HAND_POINTS, &THIN_HOUR_HAND_POINTS},
  {&MINUTE_HAND_POINTS, &HOUR_HAND_POINTS},
  {&BOLD_MINUTE_HAND_POINTS, &BOLD_HOUR_HAND_POINTS},
  {&FLY_MINUTE_HAND_POINTS, &FLY_HOUR_HAND_POINTS},
  {&PYRAMID_MINUTE_HAND_POINTS, &PYRAMID_HOUR_HAND_POINTS}
};

// date and week text location
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
#define KEY_WEEK_STYLE 4
#define KEY_HAND_STYLE 5
#define KEY_SECOND_COLOR 10
#define KEY_DATE_COLOR 11
#define KEY_HOUR_COLOR 12
#define KEY_MINUTE_COLOR 13
