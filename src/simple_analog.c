#include "simple_analog.h"

#include <pebble.h>

static Window *window;
static Layer *s_date_layer, *s_hands_layer;
static TextLayer *t_date, *t_week;
static GBitmap *back_bitmap;
static BitmapLayer *back_layer;

static GPath *s_minute_arrow = NULL, *s_hour_arrow = NULL;
static char s_num_buffer[4], s_day_buffer[6];
static int hand_layout, pre_hand_layout, bt_connection;
static int second_style, date_size, week_size, week_style, background, hand_style;
static GColor second_color, date_color, hour_color, minute_color;
static PropertyAnimation *anim_date, *anim_week;

// load minute/hour hands
void load_hands() {
  if (s_minute_arrow) {
    gpath_destroy(s_minute_arrow);
  }
  if (s_hour_arrow) {
    gpath_destroy(s_hour_arrow);
  }
  s_minute_arrow = gpath_create(HAND_STYLES[hand_style][0]);
  s_hour_arrow = gpath_create(HAND_STYLES[hand_style][1]);
}

bool set_persist_int(DictionaryIterator *iter, const uint32_t key, int *value) {
  Tuple *tuple = dict_find(iter, key);
  if (tuple) {
    *value = tuple->value->int16;
    persist_write_int(key, *value);
    return 1;
  }
  return 0;
}

bool set_persist_color(DictionaryIterator *iter, const uint32_t key, GColor *value) {
  Tuple *tuple = dict_find(iter, key);
  if (tuple) {
    persist_write_int(key, tuple->value->int32);
#ifdef PBL_COLOR
    *value = GColorFromHEX(tuple->value->int32);
#endif
    return 1;
  }
  return 0;
}

GBitmap *get_background_bitmap() {
  if (background == 1) {
#ifndef PBL_COLOR
      second_color = GColorBlack;
      date_color = GColorBlack;
      hour_color = GColorBlack;
      minute_color = GColorBlack;
#endif
    return gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BG_NUM_WHITE);
  } else if (background == 2) {
#ifndef PBL_COLOR
      second_color = GColorWhite;
      date_color = GColorWhite;
      hour_color = GColorWhite;
      minute_color = GColorWhite;
#endif
    return gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BG_LOGO);
  } else {
#ifndef PBL_COLOR
      second_color = GColorWhite;
      date_color = GColorWhite;
      hour_color = GColorWhite;
      minute_color = GColorWhite;
#endif
    return gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BG_NUM);
  }
}

static void config_received_handler(DictionaryIterator *iter, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "config received");
  set_persist_int(iter, KEY_SECOND, &second_style);
  set_persist_int(iter, KEY_DATE_SIZE, &date_size);
  set_persist_int(iter, KEY_WEEK_SIZE, &week_size);
  set_persist_int(iter, KEY_WEEK_STYLE, &week_style);
  if (set_persist_int(iter, KEY_HAND_STYLE, &hand_style)) {
    load_hands();
  }
  if (set_persist_int(iter, KEY_BACKGROUND, &background)) {
    gbitmap_destroy(back_bitmap);
    back_bitmap = get_background_bitmap();
    bitmap_layer_set_bitmap(back_layer, back_bitmap);
  }
#ifdef PBL_COLOR
  set_persist_color(iter, KEY_SECOND_COLOR, &second_color);
  set_persist_color(iter, KEY_DATE_COLOR, &date_color);
  set_persist_color(iter, KEY_HOUR_COLOR, &hour_color);
  set_persist_color(iter, KEY_MINUTE_COLOR, &minute_color);
#endif
}

// update hands layer
static void hands_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  GPoint center = grect_center_point(&bounds);
  int16_t second_hand_length = bounds.size.w / 2;

  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  int32_t second_angle = TRIG_MAX_ANGLE * t->tm_sec / 60;
  GPoint second_hand = {
    .x = (int16_t)(sin_lookup(second_angle) * (int32_t)second_hand_length / TRIG_MAX_RATIO) + center.x,
    .y = (int16_t)(-cos_lookup(second_angle) * (int32_t)second_hand_length / TRIG_MAX_RATIO) + center.y,
  };

#ifdef PBL_COLOR
  graphics_context_set_antialiased(ctx, true);
  graphics_context_set_stroke_width(ctx, 1);
#endif

  // minute/hour hand
  graphics_context_set_stroke_color(ctx, GColorBlack);
  graphics_context_set_fill_color(ctx, minute_color);
  int32_t minute_angle = TRIG_MAX_ANGLE * (t->tm_min * 60 + t->tm_sec) / 3600;
  gpath_move_to(s_minute_arrow, center);
  gpath_rotate_to(s_minute_arrow, minute_angle);
  gpath_draw_filled(ctx, s_minute_arrow);
  gpath_draw_outline(ctx, s_minute_arrow);

  graphics_context_set_fill_color(ctx, hour_color);
  int32_t hour_angle = (TRIG_MAX_ANGLE * (((t->tm_hour % 12) * 6) + (t->tm_min / 10))) / (12 * 6);
  gpath_move_to(s_hour_arrow, center);
  gpath_rotate_to(s_hour_arrow, hour_angle);
  gpath_draw_filled(ctx, s_hour_arrow);
  gpath_draw_outline(ctx, s_hour_arrow);

  hand_layout = 1 << (int)(minute_angle * 4 / TRIG_MAX_ANGLE);
  hand_layout |= 1 << (int)(hour_angle * 4 / TRIG_MAX_ANGLE);
  if (hand_layout != pre_hand_layout) {
    //APP_LOG(APP_LOG_LEVEL_DEBUG, "animation stated %d", hand_layout);
    if (anim_date) {
      property_animation_destroy(anim_date);
    }
    GRect date_s = GRect(TEXT_POINTS[pre_hand_layout][0].x, TEXT_POINTS[pre_hand_layout][0].y, TEXT_WIDTH, TEXT_HEIGHT);
    GRect date_f = GRect(TEXT_POINTS[hand_layout][0].x, TEXT_POINTS[hand_layout][0].y, TEXT_WIDTH, TEXT_HEIGHT);
    anim_date = property_animation_create_layer_frame((Layer*)t_date, &date_s, &date_f);
    animation_set_duration((Animation*)anim_date, ANIM_DURATION);
    animation_set_curve((Animation*)anim_date, AnimationCurveEaseInOut);
    animation_schedule((Animation*)anim_date);
    if (anim_week) {
      property_animation_destroy(anim_week);
    }
    GRect week_s = GRect(TEXT_POINTS[pre_hand_layout][1].x, TEXT_POINTS[pre_hand_layout][1].y, TEXT_WIDTH, TEXT_HEIGHT);
    GRect week_f = GRect(TEXT_POINTS[hand_layout][1].x, TEXT_POINTS[hand_layout][1].y, TEXT_WIDTH, TEXT_HEIGHT);
    anim_week = property_animation_create_layer_frame((Layer*)t_week, &week_s, &week_f);
    animation_set_duration((Animation*)anim_week, ANIM_DURATION);
    animation_set_curve((Animation*)anim_week, AnimationCurveEaseInOut);
    animation_schedule((Animation*)anim_week);

    pre_hand_layout = hand_layout;
  }
//  APP_LOG(APP_LOG_LEVEL_DEBUG, "hand layout %d", hand_layout);

  // second hand
  graphics_context_set_stroke_color(ctx, second_color);
  graphics_context_set_fill_color(ctx, second_color);
  if (second_style == 1) {
    graphics_fill_circle(ctx, second_hand, 3);
  } else if (second_style > 1) {
#ifdef PBL_COLOR
    if (second_style > 2) {
      graphics_context_set_stroke_width(ctx, 3);
    }
#endif
    graphics_draw_line(ctx, second_hand, center);
  }

  // dot in the middle
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, GRect(center.x - 1, center.y - 1, 3, 3), 0, GCornerNone);
}

// update date number
static void update_date(TextLayer *layer, GContext *ctx) {
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  GFont font;

  // date
  snprintf(s_num_buffer, sizeof(s_num_buffer), "%d", t->tm_mday);
  text_layer_set_text(layer, s_num_buffer);
  if (date_size == 0) {
    font = fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD);
  } else if (date_size == 1) {
    font = fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK);
  } else if (date_size == 2) {
    font = fonts_get_system_font(FONT_KEY_BITHAM_42_LIGHT);
#ifdef PBL_COLOR
  } else if (date_size == 4) {
    font = fonts_get_system_font(FONT_KEY_LECO_42_NUMBERS);
#endif
  } else {
    font = fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD);
  }
  text_layer_set_text_color(layer, date_color);
  text_layer_set_background_color(layer, GColorClear);
  text_layer_set_text_alignment(layer, GTextAlignmentCenter);
  text_layer_set_font(layer, font);
}

// update day of week text
static void update_week(TextLayer *layer, GContext *ctx) {
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  GFont font;

  strftime(s_day_buffer, sizeof(s_day_buffer), "%a", t);
  text_layer_set_text(layer, s_day_buffer);
  if (week_size == 0) {
    font =  fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);
  } else if (week_size == 1) {
    font =  fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD);
  } else {
    font =  fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK);
  }
  text_layer_set_font(layer, font);
#ifdef PBL_COLOR
  if (week_style == 2 && (t->tm_wday == 0 || t->tm_wday == 6)) {
    text_layer_set_text_color(layer, GColorRed);
  } else {
#endif
    text_layer_set_text_color(layer, date_color);
#ifdef PBL_COLOR
  }
#endif
  text_layer_set_background_color(layer, GColorClear);
  text_layer_set_text_alignment(layer, GTextAlignmentCenter);
}

// update text layer
static void date_update_proc(Layer *layer, GContext *ctx) {
  GFont font;

//  APP_LOG(APP_LOG_LEVEL_DEBUG, "date_update stated");

  // date
  GPoint date_point = TEXT_POINTS[hand_layout][0];
  if (anim_date && !animation_is_scheduled((Animation*)anim_date)) {
    layer_set_frame((Layer*)t_date, GRect(date_point.x, date_point.y, TEXT_WIDTH, TEXT_HEIGHT));
  }
  update_date(t_date, ctx);

  // week
  if (week_style > 0) {
    GPoint week_point = TEXT_POINTS[hand_layout][1];
    if (anim_week && !animation_is_scheduled((Animation*)anim_week)) {
      layer_set_frame((Layer*)t_week, GRect(week_point.x, week_point.y, TEXT_WIDTH, TEXT_HEIGHT));
    }
    update_week(t_week, ctx);
  }

  // bluetooth
  if (!bluetooth_connection_service_peek()) {
    font =  fonts_get_system_font(FONT_KEY_GOTHIC_14);
#ifdef PBL_COLOR
    graphics_context_set_text_color(ctx, GColorRed);
#else
    graphics_context_set_text_color(ctx, GColorWhite);
#endif
    graphics_draw_text(ctx, "disconnect", font, GRect(0, 15, 144, 10), GTextOverflowModeFill, GTextAlignmentCenter, NULL);
    if (bt_connection) {
      vibes_short_pulse();
    }
    bt_connection = 0;
  } else {
    bt_connection = 1;
  }
}

static void handle_second_tick(struct tm *tick_time, TimeUnits units_changed) {
  layer_mark_dirty(window_get_root_layer(window));
}

GColor get_persist_color(const uint32_t key, GColor color) {
#ifdef PBL_COLOR
  if (persist_exists(key)) {
    return GColorFromHEX(persist_read_int(key));
  }
#endif
  return color;
}

int get_persist_int(const uint32_t key, int value) {
  if (persist_exists(key)) {
    return persist_read_int(key);
  } else {
    return value;
  }
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  // settings
  second_style = get_persist_int(KEY_SECOND, 1);
  date_size = get_persist_int(KEY_DATE_SIZE, 2);
  week_size = get_persist_int(KEY_WEEK_SIZE, 1);
  week_style = get_persist_int(KEY_WEEK_STYLE, 2);
  background = get_persist_int(KEY_BACKGROUND, 0);
  hand_style = get_persist_int(KEY_HAND_STYLE, 1);
#ifdef PBL_COLOR
  second_color = get_persist_color(KEY_SECOND_COLOR, GColorRed);
  date_color = get_persist_color(KEY_DATE_COLOR, GColorGreen);
  hour_color = get_persist_color(KEY_HOUR_COLOR, GColorWhite);
  minute_color = get_persist_color(KEY_MINUTE_COLOR, GColorWhite);
#endif
  // load minute and hour hands
  load_hands();
  
  back_bitmap = get_background_bitmap();
  back_layer = bitmap_layer_create(bounds);
  bitmap_layer_set_bitmap(back_layer, back_bitmap);
  layer_add_child(window_layer, bitmap_layer_get_layer(back_layer));

  s_date_layer = layer_create(bounds);
  layer_set_update_proc(s_date_layer, date_update_proc);
  layer_add_child(window_layer, s_date_layer);
  t_date = text_layer_create(GRect(0, 0, 0, 0));
  layer_add_child(s_date_layer, text_layer_get_layer(t_date));
  t_week = text_layer_create(GRect(0, 0, 0, 0));
  layer_add_child(s_date_layer, text_layer_get_layer(t_week));
  
  s_hands_layer = layer_create(bounds);
  layer_set_update_proc(s_hands_layer, hands_update_proc);
  layer_add_child(window_layer, s_hands_layer);

  anim_date = NULL;
  anim_week = NULL;
}

static void window_unload(Window *window) {
  animation_unschedule_all();
  if (anim_date) {
    property_animation_destroy(anim_date);
  }
  if (anim_week) {
    property_animation_destroy(anim_week);
  }
  bitmap_layer_destroy(back_layer);
  gbitmap_destroy(back_bitmap);
  //  layer_destroy(s_simple_bg_layer);
  text_layer_destroy(t_date);
  text_layer_destroy(t_week);
  layer_destroy(s_date_layer);
  layer_destroy(s_hands_layer);
  // destroy hands
  gpath_destroy(s_minute_arrow);
  gpath_destroy(s_hour_arrow);
}

static void init() {
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(window, true);

  s_day_buffer[0] = '\0';
  s_num_buffer[0] = '\0';
  hand_layout = 0;
  pre_hand_layout = 0;
  bt_connection = 0;
  
  tick_timer_service_subscribe(SECOND_UNIT, handle_second_tick);
  app_message_register_inbox_received(config_received_handler);
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
}

static void deinit() {
  tick_timer_service_unsubscribe();
  window_destroy(window);
}

int main() {
  init();
  app_event_loop();
  deinit();
}
