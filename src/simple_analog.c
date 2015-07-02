#include "simple_analog.h"

#include <pebble.h>

static Window *window;
static Layer *s_simple_bg_layer, *s_date_layer, *s_hands_layer;
static TextLayer *s_day_label, *s_num_label;
static GBitmap *back_bitmap;
static BitmapLayer *back_layer;

//static GPath *s_tick_paths[NUM_CLOCK_TICKS];
static GPoint bglines[NUM_CLOCK_TICKS][2];
static GPath *s_minute_arrow, *s_hour_arrow;
static char s_num_buffer[4], s_day_buffer[6];

static void bg_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, bounds, 0, GCornerNone);
  graphics_context_set_stroke_color(ctx, GColorWhite);
#ifdef PBL_COLOR
  graphics_context_set_antialiased(ctx, true);
#endif
  for (int i = 0; i < NUM_CLOCK_TICKS; ++i) {
//    gpath_draw_outline(ctx, s_tick_paths[i]);
//    if (i % 5 == 0) {
//      graphics_context_set_stroke_width(ctx, 2);
//    } else {
//      graphics_context_set_stroke_width(ctx, 1);
//    }
    graphics_draw_line(ctx, bglines[i][0], bglines[i][1]);
  }
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
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_context_set_stroke_color(ctx, GColorBlack);

  int32_t minute_angle = TRIG_MAX_ANGLE * (t->tm_min * 60 + t->tm_sec) / 3600;
  gpath_rotate_to(s_minute_arrow, minute_angle);
  gpath_draw_filled(ctx, s_minute_arrow);
  gpath_draw_outline(ctx, s_minute_arrow);

  gpath_rotate_to(s_hour_arrow, (TRIG_MAX_ANGLE * (((t->tm_hour % 12) * 6) + (t->tm_min / 10))) / (12 * 6));
  gpath_draw_filled(ctx, s_hour_arrow);
  gpath_draw_outline(ctx, s_hour_arrow);

  // second hand
#ifdef PBL_COLOR
  graphics_context_set_stroke_color(ctx, GColorRed);
#else
  graphics_context_set_stroke_color(ctx, GColorWhite);
#endif
  graphics_draw_line(ctx, second_hand, center);

  // dot in the middle
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, GRect(bounds.size.w / 2 - 1, bounds.size.h / 2 - 1, 3, 3), 0, GCornerNone);
}

static void date_update_proc(Layer *layer, GContext *ctx) {
  time_t now = time(NULL);
  struct tm *t = localtime(&now);

  strftime(s_day_buffer, sizeof(s_day_buffer), "%a", t);
  text_layer_set_text(s_day_label, s_day_buffer);

  strftime(s_num_buffer, sizeof(s_num_buffer), "%d", t);
  text_layer_set_text(s_num_label, s_num_buffer);
}

static void handle_second_tick(struct tm *tick_time, TimeUnits units_changed) {
  layer_mark_dirty(window_get_root_layer(window));
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  back_bitmap = gbitmap_create_with_resource(RESOURCE_ID_modern_background2);
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

  s_day_label = text_layer_create(GRect(45, 100, 60, 32));
  text_layer_set_text(s_day_label, s_day_buffer);
  text_layer_set_background_color(s_day_label, GColorClear);
#ifdef PBL_COLOR
  text_layer_set_text_color(s_day_label, GColorBrightGreen);
#else
  text_layer_set_text_color(s_day_label, GColorWhite);
#endif
  text_layer_set_font(s_day_label, fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK));

  layer_add_child(s_date_layer, text_layer_get_layer(s_day_label));

  s_num_label = text_layer_create(GRect(50, 30, 60, 32));
  text_layer_set_text(s_num_label, s_num_buffer);
  text_layer_set_background_color(s_num_label, GColorClear);
#ifdef PBL_COLOR
  text_layer_set_text_color(s_num_label, GColorBrightGreen);
#else
  text_layer_set_text_color(s_num_label, GColorWhite);
#endif
  text_layer_set_font(s_num_label, fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK));

  layer_add_child(s_date_layer, text_layer_get_layer(s_num_label));
}

static void window_unload(Window *window) {
  bitmap_layer_destroy(back_layer);
  gbitmap_destroy(back_bitmap);
  //  layer_destroy(s_simple_bg_layer);
  layer_destroy(s_date_layer);

  text_layer_destroy(s_day_label);
  text_layer_destroy(s_num_label);

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

  // init hand paths
  s_minute_arrow = gpath_create(&MINUTE_HAND_POINTS);
  s_hour_arrow = gpath_create(&HOUR_HAND_POINTS);

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  GPoint center = grect_center_point(&bounds);
  gpath_move_to(s_minute_arrow, center);
  gpath_move_to(s_hour_arrow, center);

  int32_t angle, pos = 70, length = 2;
  for (int i = 0; i < NUM_CLOCK_TICKS; ++i) {
    angle = TRIG_MAX_ANGLE * (int32_t)i / NUM_CLOCK_TICKS;
//    APP_LOG(APP_LOG_LEVEL_INFO, "angle %ld", angle);
    bglines[i][0].x = (int16_t)(sin_lookup(angle) * pos / TRIG_MAX_RATIO) + center.x;
    bglines[i][0].y = (int16_t)(-cos_lookup(angle) * pos / TRIG_MAX_RATIO) + center.y;
    bglines[i][1].x = (int16_t)(sin_lookup(angle) * (pos + length) / TRIG_MAX_RATIO) + center.x;
    bglines[i][1].y = (int16_t)(-cos_lookup(angle) * (pos + length) / TRIG_MAX_RATIO) + center.y;
//    s_tick_paths[i] = gpath_create(&path);
  }

  tick_timer_service_subscribe(SECOND_UNIT, handle_second_tick);
}

static void deinit() {
  gpath_destroy(s_minute_arrow);
  gpath_destroy(s_hour_arrow);

  for (int i = 0; i < NUM_CLOCK_TICKS; ++i) {
//    gpath_destroy(s_tick_paths[i]);
  }

  tick_timer_service_unsubscribe();
  window_destroy(window);
}

int main() {
  init();
  app_event_loop();
  deinit();
}
