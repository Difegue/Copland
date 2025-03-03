#pragma once

#include <pebble.h>

#include "../config.h"

void main_window_push();

void main_window_apply_settings(ClaySettings settings);
void main_window_update(int hours, int minutes, int seconds);
void date_update(tm *t);