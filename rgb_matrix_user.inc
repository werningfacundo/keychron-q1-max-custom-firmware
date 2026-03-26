// =============================================================================
// PROJECT 1: Wave Effect
// File: rgb_matrix_user.inc
// =============================================================================
// Custom RGB animation: a smooth sine wave flowing across the keyboard,
// shifting between two colors (eggshell green -> phthalo green by default).
//
// All customization is in config.h. Do not edit magic numbers here.
//
// How it works:
//   - A sine wave is calculated per-key based on the key position axis
//     (determined by WAVE_DIRECTION) minus a time value that advances each
//     frame, creating the flowing motion.
//   - The sine output (0-255) is used to interpolate hue, saturation, and
//     brightness between the start and end colors defined in config.h.
//   - WAVE_FREQUENCY scales the position axis, compressing or expanding
//     how many wave cycles are visible across the keyboard at once.
// =============================================================================

RGB_MATRIX_EFFECT(WAVE_EGGSHELL_PTHALO)

#ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS

// Helper: get the position axis for a given LED based on wave direction
static uint8_t wave_get_position(uint8_t i) {
#if WAVE_DIRECTION == WAVE_DIR_TOP_DOWN
    return g_led_config.point[i].y;
#elif WAVE_DIRECTION == WAVE_DIR_BOTTOM_UP
    return 64 - g_led_config.point[i].y;
#elif WAVE_DIRECTION == WAVE_DIR_LEFT_RIGHT
    return g_led_config.point[i].x;
#elif WAVE_DIRECTION == WAVE_DIR_RIGHT_LEFT
    return 224 - g_led_config.point[i].x;
#else
    return g_led_config.point[i].y;
#endif
}

static bool WAVE_EGGSHELL_PTHALO(effect_params_t* params) {
    RGB_MATRIX_USE_LIMITS(led_min, led_max);

    // Advance time based on speed setting
    uint16_t time = scale16by8(g_rgb_timer, rgb_matrix_config.speed / WAVE_SPEED_DIVISOR);

    for (uint8_t i = led_min; i < led_max; i++) {
        // Sine wave: position along direction axis * frequency, offset by time
        uint8_t pos  = wave_get_position(i);
        uint8_t wave = sin8((pos * WAVE_FREQUENCY) - time);

        // Interpolate hue, saturation, brightness between start and end colors
        uint8_t hue = WAVE_HUE_START + scale8(wave, WAVE_HUE_END - WAVE_HUE_START);
        uint8_t sat = WAVE_SAT_START + scale8(wave, WAVE_SAT_END - WAVE_SAT_START);
        uint8_t val = WAVE_VAL_START - scale8(wave, WAVE_VAL_START - WAVE_VAL_END);

        // Respect global brightness setting
        val = scale8(val, rgb_matrix_config.hsv.v);

        RGB rgb = hsv_to_rgb((HSV){hue, sat, val});
        rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
    }

    return rgb_matrix_check_finished_leds(led_max);
}

#endif
