// =============================================================================
// config.h - Keychron Q1 Max Custom Firmware
// =============================================================================
// All user-facing customization lives here. Edit these values to change
// the behavior of all projects without touching any other files.
// After editing, recompile and flash for changes to take effect.
// =============================================================================

#pragma once

// Firmware version. Update this when making changes.
#define FIRMWARE_VERSION "1.2.0"

// -----------------------------------------------------------------------------
// PROJECT 1: Wave Effect
// -----------------------------------------------------------------------------
// A smooth sine wave that flows across the keyboard, shifting between two
// colors. Direction, speed, colors, and shape are all configurable here.
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

// LED index of the toggle key indicator light.
// Default 57 = HOME key on Q1 Max ANSI 82-key layout.
#define TSFM_INDICATOR_LED     57
#define TSFM_INDICATOR_ENABLE   1
#define TSFM_INDICATOR_R        0
#define TSFM_INDICATOR_G        0
#define TSFM_INDICATOR_B      255

// -----------------------------------------------------------------------------
// PROJECT 3: Sleep / Wake Animation
// -----------------------------------------------------------------------------
// When idle, the wave gradually slows and fades to black.
// On first keypress, a ripple expands outward from the pressed key.

// How long keyboard must be idle before sleep animation begins (ms)
// Default: 600000 = 10 minutes (matches Keychron hardware default)
#define SLEEP_TIMEOUT_MS    600000

// How long the wave takes to slow down before fading (ms)
#define SLEEP_SLOWDOWN_MS    30000

// How long the fade to black takes after wave stops (ms)
#define SLEEP_FADEOUT_MS     10000

// Wake ripple: set WAKE_RIPPLE_CUSTOM 1 to use custom color, 0 to use wave colors
#define WAKE_RIPPLE_CUSTOM       0
#define WAKE_RIPPLE_HUE         85
#define WAKE_RIPPLE_SAT         15
#define WAKE_RIPPLE_VAL        230

// How fast the wake ripple expands (higher = faster)
#define WAKE_RIPPLE_SPEED       80

// How wide the ripple ring is (higher = wider)
#define WAKE_RIPPLE_WIDTH       30

// -----------------------------------------------------------------------------
// PROJECT 4: Key Press Glow
// -----------------------------------------------------------------------------
// When a key is pressed, a white glow appears under it and fades out.
// A soft aura spreads to neighboring keys based on distance.

// Maximum simultaneous active glows
#define GLOW_MAX_COUNT       10

// How long each glow lasts in total (ms)
#define GLOW_DURATION_MS   1000

// Radius of the aura around the pressed key (in LED position units, 0-255)
#define GLOW_RADIUS          35

// Peak brightness of glow at center (0-255)
#define GLOW_BRIGHTNESS     255

// Required for custom RGB effects. Do not remove.
#define RGB_MATRIX_CUSTOM_USER

// --- Mic Mute Indicator (Project 5) ---
#define MIC_INDICATOR_LED    43       // PAGE DOWN key
#define MIC_INDICATOR_ENABLE 1
#define MIC_MUTED_R          255      // red when muted
#define MIC_MUTED_G          0
#define MIC_MUTED_B          0
#define MIC_ACTIVE_R         0        // blue when active
#define MIC_ACTIVE_G         0
#define MIC_ACTIVE_B         255

