# AGENTS.md — Keychron Q1 Max Custom Firmware

## Project Overview
Custom QMK firmware for Keychron Q1 Max (ANSI encoder, wireless).
Five independent projects live in a single keymap called `keychron_q1_max_custom_firmware`.

## Repo Structure
```
~/keychron_firmware/                          <- Keychron QMK fork (wireless_playground branch)
  keyboards/keychron/q1_max/ansi_encoder/
    keymaps/keychron_q1_max_custom_firmware/
      config.h                       <- ALL user customization. Edit this, not the others.
      keymap.c                       <- TSFM logic, mic mute key handling, RGB indicators
      rgb_matrix_user.inc            <- Wave, glow, sleep/wake animations
      mic_mute_blackhole_daemon.py   <- macOS daemon for Project 5 (mic mute)
      AGENTS.md                      <- This file
      DEVELOPMENT.md                 <- Setup + compile + flash guide
      README.md                      <- Human-facing project docs
      CHANGELOG.md                   <- Version history
```

## Environment

| Item | Value |
|------|-------|
| OS | macOS 14.1 Sonoma, Apple Silicon (M-series) |
| Shell | zsh |
| QMK | pip3 install qmk (NOT Homebrew — broken on Apple Silicon) |
| Compiler | arm-none-eabi-gcc 9.5.0_2 from osx-cross (NOT Homebrew v15 — missing stdint.h) |
| Python | 3.14 at `/Library/Frameworks/Python.framework/Versions/3.14/` |
| PATH | Compiler exports are permanent in `~/.zprofile` — run `source ~/.zprofile` in new sessions |

## Compile & Flash
```bash
source ~/.zprofile && cd ~/keychron_firmware
qmk compile -kb keychron/q1_max/ansi_encoder -km keychron_q1_max_custom_firmware
qmk flash -kb keychron/q1_max/ansi_encoder -km keychron_q1_max_custom_firmware
# When flashing: unplug keyboard -> hold Esc -> plug in -> release Esc
```

## Key Rules
- ONLY edit files in `keyboards/keychron/q1_max/ansi_encoder/keymaps/keychron_q1_max_custom_firmware/`
- ALL tuning/customization goes in `config.h` — never hardcode values in other files
- Always compile before flashing to catch errors
- Use downloadable `.py` scripts for multi-line file writes — never printf, heredoc, or TextEdit
- User is non-technical: provide single copy-paste commands whenever possible

## Critical Implementation Notes
- `KC_MCTRL` and `KC_LNPAD` are intercepted by `process_record_keychron_common()` —
  TSFM remapping for these MUST happen before that call in `process_record_user()`
- `IS_LAYER_ON()` has timing issues for detecting Fn — use the `fn_held` bool instead
- Compiler is arm-none-eabi-gcc 9.5.0_2 from osx-cross — NOT the default Homebrew gcc
  (v15 is broken, missing stdint.h)
- Q1 Max only exists in Keychron's fork on `wireless_playground` branch —
  standard QMK repo at ~/qmk_firmware is irrelevant
- `tsfm_active` and `mic_muted` are RAM-only bools — both reset on keyboard power cycle (by design)

## Project Notes

### Project 2: TSFM
- HOME key (LED 57) toggles `tsfm_active`
- `rgb_matrix_indicators_user()` sets HOME LED blue when active, nothing when inactive (wave renders)
- Indicator color: blue (0/0/255) — changed from white in v1.3.0

### Project 5: Mic Mute
- PAGE DOWN sends custom keycode `MIC_TOG` → fires `Cmd+Shift+Ctrl+Opt+M`
- `mic_muted` bool toggles on each press; `rgb_matrix_indicators_user()` sets PAGE DOWN LED:
  - Red (255/0/0) when muted, blue (0/0/255) when active
- macOS daemon (`mic_mute_blackhole_daemon.py`) swaps default input device to BlackHole on mute,
  restores previous device on unmute
- Volume-based muting does NOT work — Zoom and WhatsApp dynamically restore input volume
- Daemon always starts unmuted; fixes leftover BlackHole state on startup
- Falls back to built-in mic if saved device was unplugged

## Config Reference
```c
// Wave
#define WAVE_DIRECTION      WAVE_DIR_TOP_DOWN
#define WAVE_SPEED_DIVISOR  8
#define WAVE_FREQUENCY      1
#define WAVE_HUE_START      85    // eggshell green
#define WAVE_SAT_START      15
#define WAVE_VAL_START      230
#define WAVE_HUE_END        107   // phthalo green
#define WAVE_SAT_END        255
#define WAVE_VAL_END        30

// TSFM
#define TSFM_INDICATOR_LED    57
#define TSFM_INDICATOR_ENABLE 1
#define TSFM_INDICATOR_R      0
#define TSFM_INDICATOR_G      0
#define TSFM_INDICATOR_B      255   // blue

// Sleep/Wake
#define SLEEP_TIMEOUT_MS    600000
#define SLEEP_SLOWDOWN_MS   30000
#define SLEEP_FADEOUT_MS    10000
#define WAKE_RIPPLE_SPEED   80
#define WAKE_RIPPLE_WIDTH   30

// Glow
#define GLOW_MAX_COUNT      10
#define GLOW_DURATION_MS    1000
#define GLOW_RADIUS         35
#define GLOW_BRIGHTNESS     255

// Mic Mute
#define MIC_INDICATOR_LED    43
#define MIC_INDICATOR_ENABLE 1
#define MIC_MUTED_R          255
#define MIC_MUTED_G          0
#define MIC_MUTED_B          0     // red = muted
#define MIC_ACTIVE_R         0
#define MIC_ACTIVE_G         0
#define MIC_ACTIVE_B         255   // blue = active
```

## LED Index Reference (Q1 Max ANSI)
- Count left-to-right, top-to-bottom starting from 0 (ESC = 0)
- HOME key = LED index 57
- PAGE DOWN key = LED index 43

## Known Issues
- F11 intercepted by macOS Show Desktop at system level (not a firmware bug)
- TSFM and mic mute state reset to defaults on keyboard reboot (by design)
