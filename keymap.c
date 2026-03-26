// =============================================================================
// keymap.c - Keychron Q1 Max Custom Firmware
// =============================================================================
// Implements two projects:
//
// PROJECT 1: Wave Effect
//   Defined in rgb_matrix_user.inc, configured in config.h.
//   Activated on boot via keyboard_post_init_user() below.
//
// PROJECT 2: Toggleable Standard Function Mode (TSFM)
//   Remaps the F-row to send F1-F12 directly instead of media/brightness.
//   Useful for gaming (e.g. StarCraft 2).
//   Toggle key: HOME (row 3, far right). Configured in config.h.
//   When active: HOME key glows with indicator color from config.h.
//   When active + Fn held: F-row sends normal media/brightness as usual.
//
//   IMPLEMENTATION NOTE: KC_MCTRL and KC_LNPAD are intercepted by
//   process_record_keychron_common() and never reach normal QMK processing.
//   TSFM remapping for these two keys must happen BEFORE that call.
//
//   KNOWN ISSUE: macOS intercepts F11 for Show Desktop at system level.
//   See README.md for the fix.
// =============================================================================

#include QMK_KEYBOARD_H
#include "keychron_common.h"

// --- TSFM State ---
static bool tsfm_active = false;
static bool fn_held     = false;

// --- Layers ---
enum layers {
    MAC_BASE,
    MAC_FN,
    WIN_BASE,
    WIN_FN,
};

// --- Custom Keycodes ---
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

    // Mac Fn layer. F-row sends original media/brightness so Fn+Fkey
    // works correctly even when TSFM is active.
    [MAC_FN] = LAYOUT_ansi_82(
        _______,  KC_BRID,  KC_BRIU,  KC_MCTRL, KC_LNPAD, RGB_VAD,  RGB_VAI,  KC_MPRV,  KC_MPLY,  KC_MNXT,  KC_MUTE,  KC_VOLD,  KC_VOLU,  _______,            RGB_TOG,
        _______,  BT_HST1,  BT_HST2,  BT_HST3,  P2P4G,    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,            _______,
        RGB_TOG,  RGB_MOD,  RGB_VAI,  RGB_HUI,  RGB_SAI,  RGB_SPI,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,            _______,
        _______,  RGB_RMOD, RGB_VAD,  RGB_HUD,  RGB_SAD,  RGB_SPD,  _______,  _______,  _______,  _______,  _______,  _______,            _______,            KC_END,
        _______,            _______,  _______,  _______,  _______,  BAT_LVL,  NK_TOGG,  _______,  _______,  _______,  _______,            _______,  _______,
        _______,  _______,  _______,                                _______,                                _______,  _______,  _______,  _______,  _______,  _______),

    // Windows base layer. HOME replaced with TSFM_TOG.
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

bool process_record_user(uint16_t keycode, keyrecord_t *record) {

    // TSFM remapping must happen BEFORE process_record_keychron_common()
    // because KC_MCTRL and KC_LNPAD are swallowed there otherwise.
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

void keyboard_post_init_user(void) {
    rgb_matrix_mode(RGB_MATRIX_CUSTOM_WAVE_EGGSHELL_PTHALO);
}
