#include "simple_analog.h"

#include <pebble.h>

static Window *window;
static Layer *s_date_layer, *s_hands_layer;
static GBitmap *back_bitmap;
static BitmapLayer *back_layer;

static GPath *s_minute_arrow, *s_hour_arrow;
static char s_num_buffer[4], s_day_buffer[6];
static int hand_layout;
static int second_style;
static GColor second_color, date_color, hour_color;

static void config_received_handler(DictionaryIterator *iter, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "config received");
  Tuple *second_t = dict_find(iter, KEY_SECOND);
  if (second_t) {
    second_style = second_t->value->int16;
    APP_LOG(APP_LOG_LEVEL_DEBUG, "second style %d", second_style);
    persist_write_int(KEY_SECOND, second_style);
  }
#ifdef PBL_COLOR
  second_t = dict_find(iter, KEY_SECOND_COLOR);
  if (second_t) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "second color %ld", second_t->value->int32);
    second_color = GColorFromHEX(second_t->value->int32);
    persist_write_int(KEY_SECOND_COLOR, second_t->value->int32);
  }
  Tuple *date_t = dict_find(iter, KEY_DATE_COLOR);
  if (date_t) {
    date_color = GColorFromHEX(date_t->value->int32);
    persist_write_int(KEY_DATE_COLOR, date_t->value->int32);
  }
  Tuple *hour_t = dict_find(iter, KEY_HOUR_COLOR);
  if (hour_t) {
    hour_color = GColorFromHEX(hour_t->value->int32);
    persist_write_int(KEY_HOUR_COLOR, hour_t->value->int32);
  }
#endif
}

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
#endif

  // minute/hour hand
  graphics_context_set_fill_color(ctx, hour_color);
  graphics_context_set_stroke_color(ctx, GColorBlack);

  int32_t minute_angle = TRIG_MAX_ANGLE * (t->tm_min * 60 + t->tm_sec) / 3600;
  gpath_rotate_to(s_minute_arrow, minute_angle);
  gpath_draw_filled(ctx, s_minute_arrow);
  gpath_draw_outline(ctx, s_minute_arrow);

  int32_t hour_angle = (TRIG_MAX_ANGLE * (((t->tm_hour % 12) * 6) + (t->tm_min / 10))) / (12 * 6);
  gpath_rotate_to(s_hour_arrow, hour_angle);
  gpath_draw_filled(ctx, s_hour_arrow);
  gpath_draw_outline(ctx, s_hour_arrow);

  hand_layout = 1 << (int)(minute_angle * 4 / TRIG_MAX_ANGLE);
  hand_layout |= 1 << (int)(hour_angle * 4 / TRIG_MAX_ANGLE);
//  APP_LOG(APP_LOG_LEVEL_DEBUG, "hand layout %d", hand_layout);

  // second hand
  graphics_context_set_stroke_color(ctx, second_color);
  graphics_context_set_fill_color(ctx, second_color);
  if (second_style == 1) {
    graphics_fill_circle(ctx, second_hand, 3);
  } else if (second_style > 1) {
      graphics_draw_line(ctx, second_hand, center);
  }

  // dot in the middle
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, GRect(bounds.size.w / 2 - 1, bounds.size.h / 2 - 1, 3, 3), 0, GCornerNone);
}

void draw_outline_text(GContext *ctx, const char* text, const GFont font, const GRect rect, GColor forecolor, GColor outlinecolor) {
//  graphics_context_set_text_color(ctx, outlinecolor);
//  graphics_draw_text(ctx, text, font, GRect(rect.origin.x+1, rect.origin.y+1, rect.size.w, rect.size.h), GTextOverflowModeFill, GTextAlignmentCenter, NULL);
//  graphics_draw_text(ctx, text, font, GRect(rect.origin.x+1, rect.origin.y-1, rect.size.w, rect.size.h), GTextOverflowModeFill, GTextAlignmentCenter, NULL);
//  graphics_draw_text(ctx, text, font, GRect(rect.origin.x-1, rect.origin.y+1, rect.size.w, rect.size.h), GTextOverflowModeFill, GTextAlignmentCenter, NULL);
//  graphics_draw_text(ctx, text, font, GRect(rect.origin.x-1, rect.origin.y-1, rect.size.w, rect.size.h), GTextOverflowModeFill, GTextAlignmentCenter, NULL);
  graphics_context_set_text_color(ctx, forecolor);
  graphics_draw_text(ctx, text, font, rect, GTextOverflowModeFill, GTextAlignmentCenter, NULL);
}

static void date_update_proc(Layer *layer, GContext *ctx) {
  time_t now = time(NULL);
  struct tm *t = localtime(&now);

  // date
  strftime(s_num_buffer, sizeof(s_num_buffer), "%e", t);
  GPoint date_point = TEXT_POINTS[hand_layout][0];
  GFont font = fonts_get_system_font(FONT_KEY_BITHAM_42_LIGHT);
  draw_outline_text(ctx, s_num_buffer, font, GRect(date_point.x, date_point.y, 72, 45), date_color, GColorBlack);

  // week
  strftime(s_day_buffer, sizeof(s_day_buffer), "%a", t);
  GPoint week_point = TEXT_POINTS[hand_layout][1];
  font =  fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD);
#ifdef PBL_COLOR
  if (t->tm_wday == 0 || t->tm_wday == 6) {
    draw_outline_text(ctx, s_day_buffer, font, GRect(week_point.x, week_point.y, 72, 40), GColorRed, GColorBulgarianRose);
  } else {
    draw_outline_text(ctx, s_day_buffer, font, GRect(week_point.x, week_point.y, 72, 40), date_color, GColorArmyGreen);
  }
#else
  draw_outline_text(ctx, s_day_buffer, font, GRect(week_point.x, week_point.y, 72, 40), date_color, GColorBlack);
#endif

  // bluetooth
  if (!bluetooth_connection_service_peek()) {
    font =  fonts_get_system_font(FONT_KEY_GOTHIC_14);
#ifdef PBL_COLOR
    graphics_context_set_text_color(ctx, GColorRed);
#else
    graphics_context_set_text_color(ctx, GColorWhite);
#endif
    graphics_draw_text(ctx, "disconnect", font, GRect(0, 15, 144, 10), GTextOverflowModeFill, GTextAlignmentCenter, NULL);
  }
}

static void handle_second_tick(struct tm *tick_time, TimeUnits units_changed) {
  layer_mark_dirty(window_get_root_layer(window));
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  back_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BG_NUM);
  back_layer = bitmap_layer_create(bounds);
  bitmap_layer_set_bitmap(back_layer, back_bitmap);
  layer_add_child(window_layer, bitmap_layer_get_layer(back_layer));

//  s_simple_bg_layer = layer_create(bounds);
//  layer_set_update_proc(s_simple_bg_layer, bg_update_proc);
//  layer_add_child(window_layer, s_simple_bg_layer);

  s_hands_layer = layer_create(bounds);
  layer_set_update_proc(s_hands_layer, hands_update_proc);
  layer_add_child(window_layer, s_hands_layer);

  s_date_layer = layer_create(bounds);
  layer_set_update_proc(s_date_layer, date_update_proc);
  layer_add_child(window_layer, s_date_layer);

  // settings
  if (persist_exists(KEY_SECOND)) {
    second_style = persist_read_int(KEY_SECOND);
  } else {
    second_style = 1;
  }
#ifdef PBL_COLOR
  if (persist_exists(KEY_SECOND_COLOR)) {
    second_color = GColorFromHEX(persist_read_int(KEY_SECOND_COLOR));
  } else {
    second_color = GColorRed;
  }
  if (persist_exists(KEY_DATE_COLOR)) {
    date_color = GColorFromHEX(persist_read_int(KEY_DATE_COLOR));
  } else {
    date_color = GColorGreen;
  }
  if (persist_exists(KEY_HOUR_COLOR)) {
    hour_color = GColorFromHEX(persist_read_int(KEY_HOUR_COLOR));
  } else {
    hour_color = GColorWhite;
  }
#else
  second_color = GColorWhite;
  date_color = GColorWhite;
  hour_color = GColorWhite;
#endif
}

static void window_unload(Window *window) {
  bitmap_layer_destroy(back_layer);
  gbitmap_destroy(back_bitmap);
  //  layer_destroy(s_simple_bg_layer);
  layer_destroy(s_date_layer);

  layer_destroy(s_hands_layer);
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
  // init hand paths
  s_minute_arrow = gpath_create(&MINUTE_HAND_POINTS);
  s_hour_arrow = gpath_create(&HOUR_HAND_POINTS);

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  GPoint center = grect_center_point(&bounds);
  gpath_move_to(s_minute_arrow, center);
  gpath_move_to(s_hour_arrow, center);

  tick_timer_service_subscribe(SECOND_UNIT, handle_second_tick);
  app_message_register_inbox_received(config_received_handler);
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
}

static void deinit() {
  gpath_destroy(s_minute_arrow);
  gpath_destroy(s_hour_arrow);

  tick_timer_service_unsubscribe();
  window_destroy(window);
}

int main() {
  init();
  app_event_loop();
  deinit();
}
