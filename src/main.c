#include <pebble.h>

#include "windows/main_window.h"
#include "config.h"

// A struct for our specific settings (see config.h)
ClaySettings settings;

// Initialize the default settings
static void default_settings() {
  settings.BackgroundColor = BG_COLOR;
  settings.SecondsColor = SECONDS_COLOR;
  settings.HoursColor = HOURS_COLOR;
  settings.MinutesColor = MINUTES_COLOR;
  settings.DateColor = DATE_COLOR_FG;
  settings.ShowSeconds = true;
  settings.ShowDate = true;
}

static void tick_handler(struct tm *time_now, TimeUnits changed) {
  main_window_update(time_now->tm_hour, time_now->tm_min, time_now->tm_sec);
  date_update(time_now);
}

// Read settings from persistent storage
static void clay_load_settings() {
  // Load the default settings
  default_settings();
  // Read settings from persistent storage, if they exist
  persist_read_data(SETTINGS_KEY, &settings, sizeof(settings));
}

// Save the settings to persistent storage
static void clay_save_settings() {
  persist_write_data(SETTINGS_KEY, &settings, sizeof(settings));

  // Update the display based on new settings
  main_window_apply_settings(settings);

  tick_timer_service_unsubscribe();
  // If the seconds are shown, subscribe to seconds, otherwise subscribe to minutes
  if (settings.ShowSeconds) {
    tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
  } else {
    tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  }
}

// Handle the response from AppMessage
static void clay_inbox_received_handler(DictionaryIterator *iter, void *context) {
  // Background Color
  Tuple *bg_color_t = dict_find(iter, MESSAGE_KEY_bgColor);
  if (bg_color_t) {
    settings.BackgroundColor = GColorFromHEX(bg_color_t->value->int32);
  }

  // Seconds Color
  Tuple *sec_color_t = dict_find(iter, MESSAGE_KEY_secondsColor);
  if (sec_color_t) {
    settings.SecondsColor = GColorFromHEX(sec_color_t->value->int32);
  }

  // Hours Color
  Tuple *hours_color_t = dict_find(iter, MESSAGE_KEY_hoursColor);
  if (hours_color_t) {
    settings.HoursColor = GColorFromHEX(hours_color_t->value->int32);
  }

  // Minutes Color
  Tuple *minutes_color_t = dict_find(iter, MESSAGE_KEY_minutesColor);
  if (minutes_color_t) {
    settings.MinutesColor = GColorFromHEX(minutes_color_t->value->int32);
  }

  // Date Color
  Tuple *date_color_t = dict_find(iter, MESSAGE_KEY_dateColor);
  if (date_color_t) {
    settings.DateColor = GColorFromHEX(date_color_t->value->int32);
  }

  // Second Tick
  Tuple *second_tick_t = dict_find(iter, MESSAGE_KEY_showSeconds);
  if (second_tick_t) {
    settings.ShowSeconds = second_tick_t->value->int32 == 1;
  }

  // Date
  Tuple *date_t = dict_find(iter, MESSAGE_KEY_showDate);
  if (date_t) {
    settings.ShowDate = date_t->value->int32 == 1;
  }

  // Remove Border
  Tuple *remove_border_t = dict_find(iter, MESSAGE_KEY_removeBorder);
  if (remove_border_t) {
    settings.RemoveBorder = remove_border_t->value->int32 == 1;
  }

  // Date in Corner
  Tuple *date_in_corner_t = dict_find(iter, MESSAGE_KEY_corneredDate);
  if (date_in_corner_t) {
    settings.DateInCorner = date_in_corner_t->value->int32 == 1;
  }

  // Save the new settings to persistent storage
  clay_save_settings();
}

static void init() {

  clay_load_settings();

  // Listen for AppMessages
  app_message_register_inbox_received(clay_inbox_received_handler);
  app_message_open(128, 128);

  main_window_push();
  main_window_apply_settings(settings);

  if (settings.ShowSeconds) {
    tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
  } else {
    tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  }
}

static void deinit() {
  tick_timer_service_unsubscribe();
}

int main() {
  init();
  app_event_loop();
  deinit();
}
