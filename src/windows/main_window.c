#include "main_window.h"

static Window *s_window;
static Layer *s_canvas, *s_date_layer, *s_bg_layer;
static TextLayer *s_day_label;
static GPath *s_hour_arrow, *s_seconds_arrow;
static GFont s_font;

static GColor clay_bg_color, clay_hours_color, clay_minutes_color, clay_seconds_color, clay_date_color;
static bool show_seconds;

static int s_hours, s_minutes, s_seconds;
static char s_day_buffer[10];

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

static void bg_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  GPoint center = grect_center_point(&bounds);

  graphics_context_set_fill_color(ctx, clay_bg_color);
  graphics_fill_circle(ctx, center, 68);
}

static void date_update_proc(Layer *layer, GContext *ctx) {
  text_layer_set_text(s_day_label, s_day_buffer);
}

static void canvas_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);

  // Find the center of the screen
  GPoint center = grect_center_point(&bounds);

  // 12 hours only, with a minimum size
  s_hours -= (s_hours > 12) ? 12 : 0;

  // Hour path
  int hour_angle = get_angle_for_hour_and_minute(s_hours, s_minutes);
  graphics_context_set_stroke_width(ctx, 2);
  graphics_context_set_fill_color(ctx, clay_hours_color);
  graphics_context_set_stroke_color(ctx, clay_hours_color);
  gpath_rotate_to(s_hour_arrow, DEG_TO_TRIGANGLE(hour_angle));
  gpath_draw_filled(ctx, s_hour_arrow);
  gpath_draw_outline(ctx, s_hour_arrow);

  // Battery level arc
  BatteryChargeState state = battery_state_service_peek();
  int32_t angle_end = DEG_TO_TRIGANGLE(state.charge_percent * 3.65);
  
  // Draw an arc for the battery level
  if (state.charge_percent > 60) {
    graphics_context_set_stroke_color(ctx, HOURS_COLOR);
  } else if (state.charge_percent > 30) {
    graphics_context_set_stroke_color(ctx, SECONDS_COLOR);
  } else {
    graphics_context_set_stroke_color(ctx, MINUTES_COLOR);
  }
  graphics_draw_arc(ctx, bounds, GOvalScaleModeFitCircle, DEG_TO_TRIGANGLE(0), angle_end);

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
  graphics_context_set_stroke_color(ctx, clay_minutes_color);
  int minute_angle = get_angle_for_minute(s_minutes);
  GPoint minute_hand = gpoint_from_polar(frame, GOvalScaleModeFitCircle, DEG_TO_TRIGANGLE(minute_angle));
  graphics_draw_line(ctx, center, minute_hand);

  // dot+circle in the middle
  graphics_context_set_stroke_width(ctx, 2);
  graphics_context_set_fill_color(ctx, clay_seconds_color);
  graphics_context_set_stroke_color(ctx, clay_seconds_color);
  graphics_fill_circle(ctx, center, 12);

  if (show_seconds) {
    int second_angle = get_angle_for_minute(s_seconds);
    gpath_rotate_to(s_seconds_arrow, DEG_TO_TRIGANGLE(second_angle)); 
    gpath_draw_filled(ctx, s_seconds_arrow);
    gpath_draw_outline(ctx, s_seconds_arrow);
  }

  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, GRect(bounds.size.w / 2 - 1, bounds.size.h / 2 - 1, 3, 3), 0, GCornerNone);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  // Background circle at center w/ bgcolor
  s_bg_layer = layer_create(bounds);
  layer_set_update_proc(s_bg_layer, bg_update_proc);
  layer_add_child(window_layer, s_bg_layer);

  // Main hands layer
  s_canvas = layer_create(bounds);
  layer_set_update_proc(s_canvas, canvas_update_proc);
  layer_add_child(window_layer, s_canvas);

  // Date layer
  s_date_layer = layer_create(bounds);
  layer_set_update_proc(s_date_layer, date_update_proc);
  layer_add_child(window_layer, s_date_layer);

  s_day_label = text_layer_create(GRect(48, 100, 90, 40));
  text_layer_set_text(s_day_label, s_day_buffer);
  text_layer_set_background_color(s_day_label, clay_bg_color);
  text_layer_set_text_color(s_day_label, clay_date_color);
  text_layer_set_font(s_day_label, s_font);

  layer_add_child(s_date_layer, text_layer_get_layer(s_day_label));
}

static void window_unload(Window *window) {
  layer_destroy(s_canvas);
  gpath_destroy(s_hour_arrow);
  gpath_destroy(s_seconds_arrow);
  window_destroy(s_window);
}

void main_window_push() {
  s_window = window_create();

  window_set_background_color(s_window, GColorBlack);

  // Load custom font
  s_font = fonts_load_custom_font(
    resource_get_handle(RESOURCE_ID_APPLE_GARAMOND_24));

  // init hand paths
  s_hour_arrow = gpath_create(&HOUR_HAND_POINTS);
  s_seconds_arrow = gpath_create(&SECOND_HAND_POINTS);

  Layer *window_layer = window_get_root_layer(s_window);
  GRect bounds = layer_get_bounds(window_layer);
  GPoint center = grect_center_point(&bounds);
  gpath_move_to(s_hour_arrow, center);
  gpath_move_to(s_seconds_arrow, center);

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

void main_window_apply_settings(ClaySettings settings) {

  clay_bg_color = settings.BackgroundColor;
  clay_date_color = settings.DateColor;
  clay_hours_color = settings.HoursColor;
  clay_minutes_color = settings.MinutesColor;
  clay_seconds_color = settings.SecondsColor;

  show_seconds = settings.ShowSeconds;

  layer_set_hidden((Layer *)s_date_layer, !settings.ShowDate);
  text_layer_set_text_color(s_day_label, clay_date_color);
}

void date_update(tm *t) {

  strftime(s_day_buffer, sizeof(s_day_buffer), "%d/%m", t);

  if (s_hours > 3 && s_hours < 9) {
    layer_set_frame((Layer *)s_day_label, GRect(48, 35, 90, 40));
  } else {
    layer_set_frame((Layer *)s_day_label, GRect(48, 100, 90, 40));
  }

  layer_mark_dirty(s_date_layer);
}
