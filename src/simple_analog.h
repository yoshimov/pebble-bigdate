#pragma once

#include "pebble.h"

// thin style
static const GPathInfo THIN_MINUTE_HAND_POINTS = {
  5, (GPoint []) {
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
  5, (GPoint []) {
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
  5, (GPoint []) {
    { -7, 10 },
    { 7, 10 },
    { 7, -60},
    { 0, -70 },
    { -7, -60}
  }
};

static const GPathInfo BOLD_HOUR_HAND_POINTS = {
  5, (GPoint []){
    {-7, 10},
    {7, 10},
    {7, -35},
    {0, -45},
    {-7, -35}
  }
};

// flying style
static const GPathInfo FLY_MINUTE_HAND_POINTS = {
  5, (GPoint []) {
    { -6, -15 },
    { 6, -15 },
    { 8, -50},
    { 0, -70 },
    { -8, -50}
  }
};

static const GPathInfo FLY_HOUR_HAND_POINTS = {
  5, (GPoint []){
    {-7, -15},
    {7, -15},
    {10, -35},
    {0, -50},
    {-10, -35}
  }
};

// pyramid style
static const GPathInfo PYRAMID_MINUTE_HAND_POINTS = {
  11, (GPoint []) {
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

// leaf style
static const GPathInfo LEAF_MINUTE_HAND_POINTS = {
  6, (GPoint []) {
    { 0, 5 },
    { -5, -30 },
    { -5, -40 },
    { 0, -70},
    { 5, -40 },
    { 5, -30 }
  }
};

static const GPathInfo LEAF_HOUR_HAND_POINTS = {
  6, (GPoint []){
    {0, 5},
    {-5, -15},
    {-5, -25},
    {0, -45},
    {5, -25},
    {5, -15}
  }
};

// dauphin style
static const GPathInfo DAUPHIN_MINUTE_HAND_POINTS = {
  4, (GPoint []) {
    { 0, -73 },
    { -8, -2 },
    { 0, 10 },
    { 8, -2}
  }
};

static const GPathInfo DAUPHIN_HOUR_HAND_POINTS = {
  4, (GPoint []){
    {0, -45},
    {-9, -2},
    {0, 10},
    {9, -2}
  }
};

// arrow style
static const GPathInfo ARROW_MINUTE_HAND_POINTS = {
  8, (GPoint []) {
    {0, -73},
    {-12, -46},
    {-4, -48},
    {-9, 0},
    {0, 10},
    {9, 0},
    {4, -48},
    {12, -46}
  }
};

static const GPathInfo ARROW_HOUR_HAND_POINTS = {
  8, (GPoint []){
    {0, -50},
    {-15, -28},
    {-5, -30},
    {-10, 0},
    {0, 10},
    {10, 0},
    {5, -30},
    {15, -28}
  }
};

// hand style index
static const GPathInfo *HAND_STYLES[8][2] = {
  {&THIN_MINUTE_HAND_POINTS, &THIN_HOUR_HAND_POINTS},
  {&MINUTE_HAND_POINTS, &HOUR_HAND_POINTS},
  {&BOLD_MINUTE_HAND_POINTS, &BOLD_HOUR_HAND_POINTS},
  {&FLY_MINUTE_HAND_POINTS, &FLY_HOUR_HAND_POINTS},
  {&PYRAMID_MINUTE_HAND_POINTS, &PYRAMID_HOUR_HAND_POINTS},
  {&LEAF_MINUTE_HAND_POINTS, &LEAF_HOUR_HAND_POINTS},
  {&DAUPHIN_MINUTE_HAND_POINTS, &DAUPHIN_HOUR_HAND_POINTS},
  {&ARROW_MINUTE_HAND_POINTS, &ARROW_HOUR_HAND_POINTS}
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
