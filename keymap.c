// =============================================================================
// keymap.c - Keychron Q1 Max Custom Firmware
// =============================================================================
// PROJECT 1: Wave Effect - rgb_matrix_user.inc, configured in config.h
// PROJECT 2: TSFM - HOME toggles F-row, glows white when active
// PROJECT 3: Sleep/Wake - idle timer, slowdown, fadeout, wake ripple
// PROJECT 4: Key Press Glow - white glow under pressed keys, fades over 1s
// =============================================================================

#include QMK_KEYBOARD_H
#include "keychron_common.h"
#include <math.h>

// --- Sleep states ---
#define SLEEP_AWAKE     0
#define SLEEP_SLOWDOWN  1
#define SLEEP_FADEOUT   2
#define SLEEP_ASLEEP    3
#define SLEEP_WAKING    4

uint8_t  sleep_state = SLEEP_AWAKE;
uint32_t sleep_timer = 0;

// --- Wake ripple ---
bool     wake_active = false;
uint8_t  wake_led    = 0;
uint32_t wake_timer  = 0;

// --- Key press glow ---
typedef struct {
    uint8_t  x;
    uint8_t  y;
    uint32_t timestamp;
} glow_t;

glow_t  glows[GLOW_MAX_COUNT];
uint8_t glow_count = 0;

// --- TSFM state ---
static bool tsfm_active = false;
static bool fn_held     = false;

// --- Layers ---
enum layers {
    MAC_BASE,
    MAC_FN,
    WIN_BASE,
    WIN_FN,
};

// --- Custom keycodes ---
enum custom_keycodes {
    TSFM_TOG = SAFE_RANGE,
};

// --- Keymaps ---
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    [MAC_BASE] = LAYOUT_ansi_82(
        KC_ESC,   KC_BRID,  KC_BRIU,  KC_MCTRL, KC_LNPAD, RGB_VAD,  RGB_VAI,  KC_MPRV,  KC_MPLY,  KC_MNXT,  KC_MUTE,  KC_VOLD,  KC_VOLU,  KC_DEL,             KC_MUTE,
        KC_GRV,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,  KC_EQL,   KC_BSPC,            KC_PGUP,
        KC_TAB,   KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_LBRC,  KC_RBRC,  KC_BSLS,            KC_PGDN,
        KC_CAPS,  KC_A,     KC_S,     KC_D,     KC_F,     KC_G,     KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,  KC_QUOT,            KC_ENT,             TSFM_TOG,
        KC_LSFT,            KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,            KC_RSFT,  KC_UP,
        KC_LCTL,  KC_LOPTN, KC_LCMMD,                               KC_SPC,                                 KC_RCMMD,MO(MAC_FN),KC_RCTL,  KC_LEFT,  KC_DOWN,  KC_RGHT),

    [MAC_FN] = LAYOUT_ansi_82(
        _______,  KC_BRID,  KC_BRIU,  KC_MCTRL, KC_LNPAD, RGB_VAD,  RGB_VAI,  KC_MPRV,  KC_MPLY,  KC_MNXT,  KC_MUTE,  KC_VOLD,  KC_VOLU,  _______,            RGB_TOG,
        _______,  BT_HST1,  BT_HST2,  BT_HST3,  P2P4G,    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,            _______,
        RGB_TOG,  RGB_MOD,  RGB_VAI,  RGB_HUI,  RGB_SAI,  RGB_SPI,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,            _______,
        _______,  RGB_RMOD, RGB_VAD,  RGB_HUD,  RGB_SAD,  RGB_SPD,  _______,  _______,  _______,  _______,  _______,  _______,            _______,            KC_END,
        _______,            _______,  _______,  _______,  _______,  BAT_LVL,  NK_TOGG,  _______,  _______,  _______,  _______,            _______,  _______,
        _______,  _______,  _______,                                _______,                                _______,  _______,  _______,  _______,  _______,  _______),

    [WIN_BASE] = LAYOUT_ansi_82(
        KC_ESC,   KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F6,    KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,   KC_F12,   KC_DEL,             KC_MUTE,
        KC_GRV,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,  KC_EQL,   KC_BSPC,            KC_PGUP,
        KC_TAB,   KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_LBRC,  KC_RBRC,  KC_BSLS,            KC_PGDN,
        KC_CAPS,  KC_A,     KC_S,     KC_D,     KC_F,     KC_G,     KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,  KC_QUOT,            KC_ENT,             TSFM_TOG,
        KC_LSFT,            KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,            KC_RSFT,  KC_UP,
        KC_LCTL,  KC_LGUI,  KC_LALT,                                KC_SPC,                                 KC_RALT, MO(WIN_FN),KC_RCTL,  KC_LEFT,  KC_DOWN,  KC_RGHT),

    [WIN_FN] = LAYOUT_ansi_82(
        _______,  KC_BRID,  KC_BRIU,  KC_TASK,  KC_FILE,  RGB_VAD,  RGB_VAI,  KC_MPRV,  KC_MPLY,  KC_MNXT,  KC_MUTE,  KC_VOLD,  KC_VOLU,  _______,            RGB_TOG,
        _______,  BT_HST1,  BT_HST2,  BT_HST3,  P2P4G,    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,            _______,
        RGB_TOG,  RGB_MOD,  RGB_VAI,  RGB_HUI,  RGB_SAI,  RGB_SPI,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,            _______,
        _______,  RGB_RMOD, RGB_VAD,  RGB_HUD,  RGB_SAD,  RGB_SPD,  _______,  _______,  _______,  _______,  _______,  _______,            _______,            KC_END,
        _______,            _______,  _______,  _______,  _______,  BAT_LVL,  NK_TOGG,  _______,  _______,  _______,  _______,            _______,  _______,
        _______,  _______,  _______,                                _______,                                _______,  _______,  _______,  _______,  _______,  _______)
};

#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][2] = {
    [MAC_BASE] = {ENCODER_CCW_CW(KC_VOLD, KC_VOLU)},
    [MAC_FN]   = {ENCODER_CCW_CW(RGB_VAD, RGB_VAI)},
    [WIN_BASE] = {ENCODER_CCW_CW(KC_VOLD, KC_VOLU)},
    [WIN_FN]   = {ENCODER_CCW_CW(RGB_VAD, RGB_VAI)},
};
#endif

// --- Register a glow at the given LED index ---
static void register_glow(uint8_t led_index) {
    uint8_t  lx  = g_led_config.point[led_index].x;
    uint8_t  ly  = g_led_config.point[led_index].y;
    uint32_t now = timer_read32();

    // Reset if same key pressed again
    for (uint8_t g = 0; g < glow_count; g++) {
        int16_t dx = (int16_t)lx - (int16_t)glows[g].x;
        int16_t dy = (int16_t)ly - (int16_t)glows[g].y;
        if ((dx * dx + dy * dy) < (GLOW_RADIUS * GLOW_RADIUS / 4)) {
            glows[g].timestamp = now;
            return;
        }
    }

    if (glow_count < GLOW_MAX_COUNT) {
        glows[glow_count].x         = lx;
        glows[glow_count].y         = ly;
        glows[glow_count].timestamp = now;
        glow_count++;
    } else {
        // Drop oldest, add new
        for (uint8_t g = 0; g < GLOW_MAX_COUNT - 1; g++) {
            glows[g] = glows[g + 1];
        }
        glows[GLOW_MAX_COUNT - 1].x         = lx;
        glows[GLOW_MAX_COUNT - 1].y         = ly;
        glows[GLOW_MAX_COUNT - 1].timestamp = now;
    }
}

// --- Expire old glows ---
static void cleanup_glows(void) {
    uint32_t now = timer_read32();
    uint8_t  i   = 0;
    while (i < glow_count) {
        if (now - glows[i].timestamp > GLOW_DURATION_MS) {
            for (uint8_t j = i; j < glow_count - 1; j++) {
                glows[j] = glows[j + 1];
            }
            glow_count--;
        } else {
            i++;
        }
    }
}

// --- Key processing ---
bool process_record_user(uint16_t keycode, keyrecord_t *record) {

    if (record->event.pressed) {
        uint32_t now = timer_read32();
        uint8_t  row = record->event.key.row;
        uint8_t  col = record->event.key.col;
        uint8_t  led = g_led_config.matrix_co[row][col];

        // Wake from sleep
        if (sleep_state == SLEEP_ASLEEP || sleep_state == SLEEP_FADEOUT ||
            sleep_state == SLEEP_SLOWDOWN) {
            if (led != NO_LED) {
                wake_led    = led;
                wake_timer  = now;
                wake_active = true;
            }
            sleep_state = SLEEP_WAKING;
        }
        sleep_timer = now;

        // Register glow
        if (led != NO_LED) {
            register_glow(led);
        }
    }

    // TSFM remapping before keychron_common
    if (tsfm_active && !fn_held && record->event.pressed) {
        switch (keycode) {
            case KC_BRID:  tap_code(KC_F1);  return false;
            case KC_BRIU:  tap_code(KC_F2);  return false;
            case KC_MCTRL: tap_code(KC_F3);  return false;
            case KC_LNPAD: tap_code(KC_F4);  return false;
            case RGB_VAD:  tap_code(KC_F5);  return false;
            case RGB_VAI:  tap_code(KC_F6);  return false;
            case KC_MPRV:  tap_code(KC_F7);  return false;
            case KC_MPLY:  tap_code(KC_F8);  return false;
            case KC_MNXT:  tap_code(KC_F9);  return false;
            case KC_MUTE:  tap_code(KC_F10); return false;
            case KC_VOLD:  tap_code(KC_F11); return false;
            case KC_VOLU:  tap_code(KC_F12); return false;
            default: break;
        }
    }

    if (!process_record_keychron_common(keycode, record)) {
        return false;
    }

    switch (keycode) {
        case MO(MAC_FN):
        case MO(WIN_FN):
            fn_held = record->event.pressed;
            return true;

        case TSFM_TOG:
            if (record->event.pressed) {
                tsfm_active = !tsfm_active;
            }
            return false;

        default:
            break;
    }
    return true;
}

// --- Housekeeping: sleep state machine + glow cleanup ---
void housekeeping_task_user(void) {
    uint32_t now     = timer_read32();
    uint32_t idle_ms = now - sleep_timer;

    cleanup_glows();

    // Check wake ripple completion
    if (wake_active) {
        uint32_t wake_elapsed   = now - wake_timer;
        uint32_t ripple_done_ms = (224 * 1000) / WAKE_RIPPLE_SPEED;
        if (wake_elapsed > ripple_done_ms) {
            wake_active = false;
            sleep_state = SLEEP_AWAKE;
            sleep_timer = now;
        }
    }

    switch (sleep_state) {
        case SLEEP_AWAKE:
            if (idle_ms > SLEEP_TIMEOUT_MS) {
                sleep_state = SLEEP_SLOWDOWN;
                sleep_timer = now;
            }
            break;
        case SLEEP_SLOWDOWN:
            if (idle_ms > SLEEP_SLOWDOWN_MS) {
                sleep_state = SLEEP_FADEOUT;
                sleep_timer = now;
            }
            break;
        case SLEEP_FADEOUT:
            if (idle_ms > SLEEP_FADEOUT_MS) {
                sleep_state = SLEEP_ASLEEP;
            }
            break;
        case SLEEP_ASLEEP:
        case SLEEP_WAKING:
            break;
    }
}

// --- RGB indicators ---
bool rgb_matrix_indicators_user(void) {
#if TSFM_INDICATOR_ENABLE
    if (tsfm_active) {
        rgb_matrix_set_color(TSFM_INDICATOR_LED,
                             TSFM_INDICATOR_R,
                             TSFM_INDICATOR_G,
                             TSFM_INDICATOR_B);
    }
#endif
    return true;
}

// --- Initialization ---
void keyboard_post_init_user(void) {
    sleep_timer = timer_read32();
    rgb_matrix_mode(RGB_MATRIX_CUSTOM_WAVE_EGGSHELL_PTHALO);
}
