#include "simple_analog.h"

#include <pebble.h>

#define NUM_Y 25
#define DAY_Y 90

static Window *window;
static Layer *s_date_layer, *s_hands_layer;
static GBitmap *back_bitmap;
static BitmapLayer *back_layer;

static GPath *s_minute_arrow, *s_hour_arrow;
static char s_num_buffer[4], s_day_buffer[6];
static int hand_layout;

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

  int32_t hour_angle = (TRIG_MAX_ANGLE * (((t->tm_hour % 12) * 6) + (t->tm_min / 10))) / (12 * 6);
  gpath_rotate_to(s_hour_arrow, hour_angle);
  gpath_draw_filled(ctx, s_hour_arrow);
  gpath_draw_outline(ctx, s_hour_arrow);

  hand_layout = 1 << (int)(minute_angle * 4 / TRIG_MAX_ANGLE);
  hand_layout |= 1 << (int)(hour_angle * 4 / TRIG_MAX_ANGLE);
//  APP_LOG(APP_LOG_LEVEL_DEBUG, "hand layout %d", hand_layout);

  // second hand
#ifdef PBL_COLOR
  graphics_context_set_stroke_color(ctx, GColorRed);
  graphics_context_set_fill_color(ctx, GColorRed);
#else
  graphics_context_set_stroke_color(ctx, GColorWhite);
  graphics_context_set_fill_color(ctx, GColorWhite);
#endif
//  graphics_draw_line(ctx, second_hand, center);
  graphics_fill_circle(ctx, second_hand, 3);

  // dot in the middle
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, GRect(bounds.size.w / 2 - 1, bounds.size.h / 2 - 1, 3, 3), 0, GCornerNone);
}

void draw_outline_text(GContext *ctx, const char* text, const GFont font, const GRect rect, GColor forecolor, GColor outlinecolor) {
  graphics_context_set_text_color(ctx, outlinecolor);
  graphics_draw_text(ctx, text, font, GRect(rect.origin.x+1, rect.origin.y+1, rect.size.w, rect.size.h), GTextOverflowModeFill, GTextAlignmentCenter, NULL);
  graphics_draw_text(ctx, text, font, GRect(rect.origin.x+1, rect.origin.y-1, rect.size.w, rect.size.h), GTextOverflowModeFill, GTextAlignmentCenter, NULL);
  graphics_draw_text(ctx, text, font, GRect(rect.origin.x-1, rect.origin.y+1, rect.size.w, rect.size.h), GTextOverflowModeFill, GTextAlignmentCenter, NULL);
  graphics_draw_text(ctx, text, font, GRect(rect.origin.x-1, rect.origin.y-1, rect.size.w, rect.size.h), GTextOverflowModeFill, GTextAlignmentCenter, NULL);
  graphics_context_set_text_color(ctx, forecolor);
  graphics_draw_text(ctx, text, font, rect, GTextOverflowModeFill, GTextAlignmentCenter, NULL);
}

static void date_update_proc(Layer *layer, GContext *ctx) {
  time_t now = time(NULL);
  struct tm *t = localtime(&now);

  // date
  strftime(s_num_buffer, sizeof(s_num_buffer), "%d", t);
  GPoint date_point = TEXT_POINTS[hand_layout][0];
  GFont font = fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK);
#ifdef PBL_COLOR
  draw_outline_text(ctx, s_num_buffer, font, GRect(date_point.x, date_point.y, 40, 30), GColorGreen, GColorArmyGreen);
#else
  draw_outline_text(ctx, s_num_buffer, font, GRect(date_point.x, date_point.y, 40, 30), GColorWhite, GColorBlack);
#endif

  // week
  strftime(s_day_buffer, sizeof(s_day_buffer), "%a", t);
  GPoint week_point = TEXT_POINTS[hand_layout][1];
  font =  fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD);
#ifdef PBL_COLOR
  if (t->tm_wday == 0 || t->tm_wday == 6) {
    draw_outline_text(ctx, s_day_buffer, font, GRect(week_point.x, week_point.y, 40, 30), GColorRed, GColorBulgarianRose);
  } else {
    draw_outline_text(ctx, s_day_buffer, font, GRect(week_point.x, week_point.y, 40, 30), GColorGreen, GColorArmyGreen);
  }
#else
  draw_outline_text(ctx, s_day_buffer, font, GRect(week_point.x, week_point.y, 40, 30), GColorWhite, GColorBlack);
#endif
}

static void handle_second_tick(struct tm *tick_time, TimeUnits units_changed) {
  layer_mark_dirty(window_get_root_layer(window));
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  back_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BG_LOGO);
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
