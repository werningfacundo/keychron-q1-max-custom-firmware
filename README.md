# Keychron Q1 Max Custom Firmware

Custom QMK firmware for the Keychron Q1 Max (ANSI encoder, wireless).
Contains two independent projects that work together in a single keymap.

## Projects

### 1. Wave Effect
A smooth sine wave animation flowing across the keyboard, shifting between
two colors. Defaults to eggshell green (pale, bright) fading into phthalo
green (rich, dark).

Fully configurable: colors, direction, speed, and frequency are all exposed
as constants in `config.h` — no code changes required.

### 2. Toggleable Standard Function Mode (TSFM)
Remaps the F-row to send standard F1-F12 keycodes directly, bypassing the
default media/brightness behavior. Useful for gaming applications like
StarCraft 2 (SC2 mode) where F-keys are used as hotkeys.

- **Toggle:** Press the **HOME** key to switch modes
- **Indicator:** HOME key glows solid white when TSFM is active
- **Fn passthrough:** Holding Fn while TSFM is active restores normal
  media/brightness behavior for that keypress
- **Default:** TSFM is off on boot (normal media/brightness behavior)

#### F-key mapping when TSFM is active
| Key position | Sends |
|---|---|
| F1 | Brightness down |
| F2 | Brightness up |
| F3 | Mission Control |
| F4 | Launchpad |
| F5 | RGB brightness - |
| F6 | RGB brightness + |
| F7 | Previous track |
| F8 | Play/pause |
| F9 | Next track |
| F10 | Mute |
| F11 | Volume down |
| F12 | Volume up |

## Customization

All user-facing settings are in `config.h`. Edit that file and reflash.
No other files need to be touched for typical customization.

### Wave Effect settings
```c
#define WAVE_DIRECTION      WAVE_DIR_TOP_DOWN   // TOP_DOWN, BOTTOM_UP, LEFT_RIGHT, RIGHT_LEFT
#define WAVE_SPEED_DIVISOR  8                   // higher = slower (range 4-16)
#define WAVE_FREQUENCY      1                   // wave cycles visible at once
#define WAVE_HUE_START      85                  // starting color hue (0-255)
#define WAVE_SAT_START      15                  // starting saturation
#define WAVE_VAL_START      230                 // starting brightness
#define WAVE_HUE_END        107                 // ending color hue
#define WAVE_SAT_END        255                 // ending saturation
#define WAVE_VAL_END        30                  // ending brightness
```

Hue reference: `0=red  43=yellow  85=green  128=cyan  171=blue  213=magenta`

### TSFM settings
```c
#define TSFM_INDICATOR_LED    57    // LED index of toggle key (57 = HOME on Q1 Max ANSI 82)
#define TSFM_INDICATOR_ENABLE 1     // 1=show indicator light, 0=hide
#define TSFM_INDICATOR_R      255   // indicator color (default: white)
#define TSFM_INDICATOR_G      255
#define TSFM_INDICATOR_B      255
```

## Files

| File | Purpose |
|---|---|
| `config.h` | All user customization |
| `keymap.c` | Layer keymaps + TSFM logic |
| `rgb_matrix_user.inc` | Wave effect animation |
| `DEVELOPMENT.md` | Setup, compile, and flash instructions |
| `AGENTS.md` | Context file for AI coding agents |

## Known Issues

**F11 triggers Show Desktop on macOS**
macOS intercepts F11 at the system level regardless of keyboard shortcut settings.
To disable this behavior, run in Terminal:
```bash
defaults write com.apple.symbolichotkeys AppleSymbolicHotKeys -dict-add 36 "{enabled = 0; value = {parameters = (65535, 103, 8388608); type = standard;};}" && killall SystemUIServer
```
Then log out and back in.

## Hardware
Tested on: Keychron Q1 Max, ANSI encoder layout, wireless
Firmware base: [Keychron QMK fork](https://github.com/Keychron/qmk_firmware), `wireless_playground` branch
