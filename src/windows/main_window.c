#include "main_window.h"

static Window *s_window;
static Layer *s_canvas, *s_date_layer;
static TextLayer *s_day_label, *s_num_label;
static GPath *s_hour_arrow, *s_seconds_arrow;

static int s_hours, s_minutes, s_seconds;
static char s_num_buffer[4], s_day_buffer[10];

static int32_t get_angle_for_hour(int hour) {
  // Progress through 12 hours, out of 360 degrees
  return (hour * 360) / 12;
}

static int32_t get_angle_for_hour_and_minute(int hour, int minute) {
  // Progress through 12 hours, out of 360 degrees
  return (hour * 360) / 12 + (minute * 360) / (12 * 60);
}

static int32_t get_angle_for_minute(int minute) {
  // Progress through 60 minutes, out of 360 degrees
  return (minute * 360) / 60;
}

static void date_update_proc(Layer *layer, GContext *ctx) {
  text_layer_set_text(s_day_label, s_day_buffer);
  text_layer_set_text(s_num_label, s_num_buffer);
}

static void layer_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);

  // Find the center of the screen
  GPoint center = grect_center_point(&bounds);

  // 12 hours only, with a minimum size
  s_hours -= (s_hours > 12) ? 12 : 0;

  // Hour path
  int hour_angle = get_angle_for_hour_and_minute(s_hours, s_minutes);
  graphics_context_set_stroke_width(ctx, 2);
  graphics_context_set_fill_color(ctx, HOURS_COLOR);
  graphics_context_set_stroke_color(ctx, HOURS_COLOR);
  gpath_rotate_to(s_hour_arrow, DEG_TO_TRIGANGLE(hour_angle));
  gpath_draw_filled(ctx, s_hour_arrow);
  gpath_draw_outline(ctx, s_hour_arrow);

  // Adjust geometry variables for hour dots
  GRect frame = grect_inset(bounds, GEdgeInsets(3 * HOURS_RADIUS));

  // Draw dots for 0, 3, 6 and 9
  int hours[] = {0, 3, 6, 9};
  for (int i = 0; i < 4; i++) {
    int hour_angle = get_angle_for_hour(hours[i]);
    GPoint pos = gpoint_from_polar(frame, GOvalScaleModeFitCircle, DEG_TO_TRIGANGLE(hour_angle));

    graphics_context_set_fill_color(ctx, GColorLightGray);
    graphics_fill_circle(ctx, pos, 4);
  }

  // Minutes 
  frame = grect_inset(bounds, GEdgeInsets(4 * INSET));
  graphics_context_set_stroke_width(ctx, 10);
  graphics_context_set_stroke_color(ctx, MINUTES_COLOR);
  int minute_angle = get_angle_for_minute(s_minutes);
  GPoint minute_hand = gpoint_from_polar(frame, GOvalScaleModeFitCircle, DEG_TO_TRIGANGLE(minute_angle));
  graphics_draw_line(ctx, center, minute_hand);

  // dot+circle in the middle
  graphics_context_set_stroke_width(ctx, 2);
  graphics_context_set_fill_color(ctx, SECONDS_COLOR);
  graphics_context_set_stroke_color(ctx, SECONDS_COLOR);
  graphics_fill_circle(ctx, center, 12);
  int second_angle = get_angle_for_minute(s_seconds);
  gpath_rotate_to(s_seconds_arrow, DEG_TO_TRIGANGLE(second_angle)); 
  gpath_draw_filled(ctx, s_seconds_arrow);
  gpath_draw_outline(ctx, s_seconds_arrow);

  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, GRect(bounds.size.w / 2 - 1, bounds.size.h / 2 - 1, 3, 3), 0, GCornerNone);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  // Date layer
  s_date_layer = layer_create(bounds);
  layer_set_update_proc(s_date_layer, date_update_proc);
  layer_add_child(window_layer, s_date_layer);

  s_day_label = text_layer_create(PBL_IF_ROUND_ELSE(
    GRect(63, 114, 27, 20),
    GRect(4, 144, 40, 40)));
  text_layer_set_text(s_day_label, s_day_buffer);
  text_layer_set_background_color(s_day_label, DATE_COLOR_BG);
  text_layer_set_text_color(s_day_label, DATE_COLOR_FG);
  text_layer_set_font(s_day_label, fonts_get_system_font(FONT_KEY_LECO_20_BOLD_NUMBERS));

  layer_add_child(s_date_layer, text_layer_get_layer(s_day_label));

  s_num_label = text_layer_create(PBL_IF_ROUND_ELSE(
    GRect(90, 114, 27, 20),
    GRect(116, 144, 40, 40)));
  text_layer_set_text(s_num_label, s_num_buffer);
  text_layer_set_background_color(s_num_label, DAY_COLOR_BG);
  text_layer_set_text_color(s_num_label, DAY_COLOR_FG);
  text_layer_set_font(s_num_label, fonts_get_system_font(FONT_KEY_LECO_20_BOLD_NUMBERS));

  layer_add_child(s_date_layer, text_layer_get_layer(s_num_label));

  // Main hands layer
  s_canvas = layer_create(bounds);
  layer_set_update_proc(s_canvas, layer_update_proc);
  layer_add_child(window_layer, s_canvas);
}

static void window_unload(Window *window) {
  layer_destroy(s_canvas);
  gpath_destroy(s_hour_arrow);
  gpath_destroy(s_seconds_arrow);
  window_destroy(s_window);
}

void main_window_push() {
  s_window = window_create();

  // init hand paths
  s_hour_arrow = gpath_create(&HOUR_HAND_POINTS);
  s_seconds_arrow = gpath_create(&SECOND_HAND_POINTS);

  Layer *window_layer = window_get_root_layer(s_window);
  GRect bounds = layer_get_bounds(window_layer);
  GPoint center = grect_center_point(&bounds);
  gpath_move_to(s_hour_arrow, center);
  gpath_move_to(s_seconds_arrow, center);

  window_set_background_color(s_window, BG_COLOR);
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(s_window, true);
}

void main_window_update(int hours, int minutes, int seconds) {
  s_hours = hours;
  s_minutes = minutes;
  s_seconds = seconds;
  layer_mark_dirty(s_canvas);
}

void date_update(tm *t) {
    strftime(s_day_buffer, sizeof(s_day_buffer), "%d", t);
    strftime(s_num_buffer, sizeof(s_num_buffer), "%m", t);
    layer_mark_dirty(s_date_layer);
}
