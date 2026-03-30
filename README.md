# Keychron Q1 Max Custom Firmware

Custom QMK firmware for the Keychron Q1 Max (ANSI encoder, wireless).
Contains five independent projects that work together in a single keymap.

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
StarCraft 2 where F-keys are used as hotkeys.

- **Toggle:** Press the **HOME** key to switch modes
- **Indicator:** HOME key glows solid blue when TSFM is active, follows wave when off
- **Fn passthrough:** Holding Fn while TSFM is active restores normal
  media/brightness behavior for that keypress
- **Default:** TSFM is off on boot (normal media/brightness behavior)

#### F-key behavior by mode

| Key | Default (TSFM off) | TSFM active |
|-----|-------------------|-------------|
| F1  | Brightness down   | F1          |
| F2  | Brightness up     | F2          |
| F3  | Mission Control   | F3          |
| F4  | Launchpad         | F4          |
| F5  | RGB brightness -  | F5          |
| F6  | RGB brightness +  | F6          |
| F7  | Previous track    | F7          |
| F8  | Play/pause        | F8          |
| F9  | Next track        | F9          |
| F10 | Mute              | F10         |
| F11 | Volume down       | F11         |
| F12 | Volume up         | F12         |

### 3. Sleep/Wake Animation
When the keyboard is idle for 10 minutes, the wave effect gradually slows,
then fades to black. On the first keypress, a ripple expands outward from
the key that was pressed, and the wave returns behind it.

The idle timeout and animation durations are configurable in `config.h`.

### 4. Key Press Glow
Each keypress produces a white glow under the pressed key with a soft aura
that fades out over one second. Up to 10 simultaneous glows are tracked.
Pressing the same key resets its timer. If the cap is reached, the oldest
glow is dropped to make room.

Glow size, brightness, duration, and max count are configurable in `config.h`.

### 5. Mic Mute
Press **PAGE DOWN** to toggle microphone mute system-wide on macOS. Works
with all apps including Zoom and WhatsApp by swapping the default audio input
device to BlackHole (a silent virtual driver) rather than adjusting volume,
which those apps ignore.

- **PAGE DOWN LED glows red** when muted
- **PAGE DOWN LED glows blue** when active (unmuted)
- Requires BlackHole and the companion daemon (see below)

#### macOS Setup

**1. Install BlackHole:**
```bash
brew install blackhole-2ch
```
Then reboot (required for the driver to activate).

**2. Install pynput:**
```bash
pip3 install pynput
```

**3. Install the daemon:**
```bash
python3 ~/path/to/mic_mute_blackhole_daemon.py --install
```

The daemon runs at login, listens for `Cmd+Shift+Ctrl+Opt+M` (sent by the
keyboard on PAGE DOWN), and swaps the default input device. Log: `/tmp/micmute.log`.

**Behavior:**
- On startup: always forces unmuted, restores a real mic if BlackHole was left as default
- On mute: saves current default input, sets BlackHole as default
- On unmute: restores saved device; falls back to built-in mic if that device was unplugged

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
#define TSFM_INDICATOR_LED    57    // LED index of toggle key (57 = HOME on Q1 Max ANSI)
#define TSFM_INDICATOR_ENABLE 1     // 1=show indicator light, 0=hide
#define TSFM_INDICATOR_R      0     // indicator color (blue)
#define TSFM_INDICATOR_G      0
#define TSFM_INDICATOR_B      255
```

### Sleep/Wake settings
```c
#define SLEEP_TIMEOUT_MS    600000  // idle time before sleeping (default: 10 min)
#define SLEEP_SLOWDOWN_MS   30000   // duration of wave slowdown phase (default: 30 sec)
#define SLEEP_FADEOUT_MS    10000   // duration of fade-to-black phase (default: 10 sec)
#define WAKE_RIPPLE_SPEED   80      // ripple expansion speed on wake
#define WAKE_RIPPLE_WIDTH   30      // ripple band width on wake
```

### Key Press Glow settings
```c
#define GLOW_MAX_COUNT      10      // max simultaneous glows
#define GLOW_DURATION_MS    1000    // fade duration in milliseconds
#define GLOW_RADIUS         35      // aura radius around pressed key
#define GLOW_BRIGHTNESS     255     // peak brightness (0-255)
```

### Mic Mute settings
```c
#define MIC_INDICATOR_LED    43     // LED index of PAGE DOWN key
#define MIC_INDICATOR_ENABLE 1      // 1=show indicator, 0=hide
#define MIC_MUTED_R          255    // red = muted
#define MIC_MUTED_G          0
#define MIC_MUTED_B          0
#define MIC_ACTIVE_R         0      // blue = active
#define MIC_ACTIVE_G         0
#define MIC_ACTIVE_B         255
```

## Files

| File | Purpose |
|---|---|
| `config.h` | All user customization |
| `keymap.c` | Layer keymaps, TSFM logic, mic mute key handling, RGB indicators |
| `rgb_matrix_user.inc` | Wave, glow, and sleep/wake animations |
| `mic_mute_blackhole_daemon.py` | macOS daemon for system-wide mic mute (Project 5) |
| `DEVELOPMENT.md` | Setup, compile, and flash instructions |
| `AGENTS.md` | Context file for AI coding agents |
| `CHANGELOG.md` | Version history |

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
