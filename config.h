// =============================================================================
// config.h - Keychron Q1 Max Custom Firmware
// =============================================================================
// All user-facing customization lives here. Edit these values to change
// the behavior of both projects without touching any other files.
// After editing, recompile and flash for changes to take effect.
// =============================================================================

#pragma once

// -----------------------------------------------------------------------------
// PROJECT 1: Wave Effect
// -----------------------------------------------------------------------------
// A smooth sine wave that flows across the keyboard, shifting between two
// colors. The wave direction, speed, colors, and shape are all configurable.
//
// Color values use QMK HSV format: hue 0-255, saturation 0-255, value 0-255.
// Hue reference: 0=red, 43=yellow, 85=green, 128=cyan, 171=blue, 213=magenta

// Direction the wave flows across the keyboard.
// Options: WAVE_DIR_TOP_DOWN, WAVE_DIR_BOTTOM_UP,
//          WAVE_DIR_LEFT_RIGHT, WAVE_DIR_RIGHT_LEFT
#define WAVE_DIRECTION WAVE_DIR_TOP_DOWN

// Speed: higher divisor = slower wave. Recommended range: 4 (fast) to 16 (slow)
#define WAVE_SPEED_DIVISOR 8

// Frequency: how many full wave cycles appear across the keyboard at once.
// 1 = one smooth wave, 2 = two shorter waves, etc.
#define WAVE_FREQUENCY 1

// Starting color (eggshell green): pale, desaturated, bright
#define WAVE_HUE_START   85    // hue at wave peak
#define WAVE_SAT_START   15    // low saturation = near-white
#define WAVE_VAL_START  230    // bright

// Ending color (phthalo green): rich, saturated, dark
#define WAVE_HUE_END    107    // hue at wave trough
#define WAVE_SAT_END    255    // fully saturated
#define WAVE_VAL_END     30    // very dark

// -----------------------------------------------------------------------------
// PROJECT 2: Toggleable Standard Function Mode (TSFM)
// -----------------------------------------------------------------------------
// Remaps the F-row to send standard F1-F12 keycodes instead of
// media/brightness keys. Useful for gaming (e.g. StarCraft 2).
// Holding Fn while in TSFM mode restores normal media/brightness behavior.
//
// Toggle key: the physical key that turns TSFM on and off.
// Default is the HOME key (row 3, far right).
// To change: replace SC2_TOG in keymap.c with any keycode, and update
// TSFM_INDICATOR_LED to match the new key LED index.

// LED index of the toggle key indicator light.
// Default 57 = HOME key on Q1 Max ANSI 82-key layout.
// To find a different key index: count keys left-to-right, top-to-bottom
// starting from 0 (ESC = 0).
#define TSFM_INDICATOR_LED 57

// Whether to show the indicator light at all.
// 1 = enabled, 0 = disabled
#define TSFM_INDICATOR_ENABLE 1

// Indicator color when TSFM is active (default: solid white)
#define TSFM_INDICATOR_R 255
#define TSFM_INDICATOR_G 255
#define TSFM_INDICATOR_B 255

// Enable RGB_MATRIX_CUSTOM_USER -- required for custom wave effect.
// Do not remove this.
#define RGB_MATRIX_CUSTOM_USER
