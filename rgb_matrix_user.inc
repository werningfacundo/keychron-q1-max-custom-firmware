// =============================================================================
// PROJECT 1: Wave Effect
// PROJECT 3: Sleep/Wake Animation
// PROJECT 4: Key Press Glow
// File: rgb_matrix_user.inc
// =============================================================================

RGB_MATRIX_EFFECT(WAVE_EGGSHELL_PTHALO)

#ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS

// --- Sleep states (must match keymap.c) ---
#define SLEEP_AWAKE     0
#define SLEEP_SLOWDOWN  1
#define SLEEP_FADEOUT   2
#define SLEEP_ASLEEP    3
#define SLEEP_WAKING    4

// --- Shared state (defined in keymap.c) ---
typedef struct {
    uint8_t  x;
    uint8_t  y;
    uint32_t timestamp;
} glow_t;

extern uint8_t  sleep_state;
extern uint32_t sleep_timer;
extern bool     wake_active;
extern uint8_t  wake_led;
extern uint32_t wake_timer;
extern glow_t   glows[GLOW_MAX_COUNT];
extern uint8_t  glow_count;

// --- Helper: get wave position axis based on direction ---
static inline uint8_t wave_get_position(uint8_t i) {
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

// --- Helper: compute time with sleep slowdown applied ---
static uint16_t get_wave_time(void) {
    uint16_t time = scale16by8(g_rgb_timer, rgb_matrix_config.speed / WAVE_SPEED_DIVISOR);
    if (sleep_state == SLEEP_SLOWDOWN) {
        uint32_t elapsed  = timer_read32() - sleep_timer;
        uint8_t  progress = (elapsed >= SLEEP_SLOWDOWN_MS) ? 255 : (uint8_t)((elapsed * 255) / SLEEP_SLOWDOWN_MS);
        time = scale16by8(time, 255 - progress);
    } else if (sleep_state == SLEEP_FADEOUT || sleep_state == SLEEP_ASLEEP) {
        time = 0;
    }
    return time;
}

// --- Helper: compute glow overlay brightness for one LED ---
static uint8_t compute_glow(uint8_t i) {
    if (glow_count == 0) return 0;

    uint32_t now       = timer_read32();
    uint8_t  lx        = g_led_config.point[i].x;
    uint8_t  ly        = g_led_config.point[i].y;
    uint8_t  max_glow  = 0;

    for (uint8_t g = 0; g < glow_count; g++) {
        uint32_t age = now - glows[g].timestamp;
        if (age > GLOW_DURATION_MS) continue;

        int16_t  dx   = (int16_t)lx - (int16_t)glows[g].x;
        int16_t  dy   = (int16_t)ly - (int16_t)glows[g].y;
        uint16_t dist = (uint16_t)sqrt((float)(dx * dx + dy * dy));

        if (dist > GLOW_RADIUS) continue;

        // Spatial falloff: bright at center, dim at edge
        uint8_t spatial = 255 - (uint8_t)((dist * 255) / GLOW_RADIUS);

        // Temporal: gradual fade over full duration
        uint8_t temporal = 255 - (uint8_t)((age * 255) / GLOW_DURATION_MS);

        uint8_t brightness = scale8(scale8(spatial, temporal), GLOW_BRIGHTNESS);
        if (brightness > max_glow) max_glow = brightness;
    }
    return max_glow;
}

// --- Main effect ---
static bool WAVE_EGGSHELL_PTHALO(effect_params_t* params) {
    RGB_MATRIX_USE_LIMITS(led_min, led_max);
    uint16_t time = get_wave_time();
    uint32_t now  = timer_read32();

    for (uint8_t i = led_min; i < led_max; i++) {
        uint8_t pos  = wave_get_position(i);
        uint8_t wave = sin8((pos * WAVE_FREQUENCY) - time);

        uint8_t hue = WAVE_HUE_START + scale8(wave, WAVE_HUE_END - WAVE_HUE_START);
        uint8_t sat = WAVE_SAT_START + scale8(wave, WAVE_SAT_END - WAVE_SAT_START);
        uint8_t val = WAVE_VAL_START - scale8(wave, WAVE_VAL_START - WAVE_VAL_END);

        // Sleep fadeout
        if (sleep_state == SLEEP_FADEOUT) {
            uint32_t elapsed  = now - sleep_timer;
            uint8_t  progress = (elapsed >= SLEEP_FADEOUT_MS) ? 255 : (uint8_t)((elapsed * 255) / SLEEP_FADEOUT_MS);
            val = scale8(val, 255 - progress);
        } else if (sleep_state == SLEEP_ASLEEP) {
            val = 0;
        }

        // Wake ripple overlay
        if (wake_active) {
            uint32_t wake_elapsed  = now - wake_timer;
            uint8_t  ripple_radius = (uint8_t)((wake_elapsed * WAKE_RIPPLE_SPEED) / 1000);
            uint8_t  wx            = g_led_config.point[wake_led].x;
            uint8_t  wy            = g_led_config.point[wake_led].y;
            uint8_t  lx            = g_led_config.point[i].x;
            uint8_t  ly            = g_led_config.point[i].y;
            int16_t  dx            = (int16_t)lx - (int16_t)wx;
            int16_t  dy            = (int16_t)ly - (int16_t)wy;
            uint8_t  dist          = (uint8_t)sqrt((float)(dx * dx + dy * dy));

            if (dist >= ripple_radius && dist <= ripple_radius + WAKE_RIPPLE_WIDTH) {
                uint8_t ring_pos    = dist - ripple_radius;
                uint8_t ring_bright = 255 - (ring_pos * 255 / WAKE_RIPPLE_WIDTH);
#if WAKE_RIPPLE_CUSTOM
                hue = WAKE_RIPPLE_HUE;
                sat = WAKE_RIPPLE_SAT;
                val = scale8(WAKE_RIPPLE_VAL, ring_bright);
#else
                hue = WAVE_HUE_START;
                sat = WAVE_SAT_START;
                val = scale8(WAVE_VAL_START, ring_bright);
#endif
            } else if (dist < ripple_radius) {
                val = scale8(val, (ripple_radius > 0) ? (uint8_t)((dist * 255) / ripple_radius) : 0);
            } else {
                val = 0;
            }
        }

        // Key press glow overlay (white, additive)
        uint8_t glow = compute_glow(i);
        if (glow > 0) {
            // Blend toward white based on glow intensity
            sat = scale8(sat, 255 - glow);
            val = val > glow ? val : glow;
        }

        val = scale8(val, rgb_matrix_config.hsv.v);
        RGB rgb = hsv_to_rgb((HSV){hue, sat, val});
        rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
    }
    return rgb_matrix_check_finished_leds(led_max);
}

#endif
