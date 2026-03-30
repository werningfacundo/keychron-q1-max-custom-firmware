# Changelog

All notable changes to this project will be documented here.

## [1.3.0] - 2026-03-29

### Added
- **Mic Mute (Project 5):** PAGE DOWN toggles system-wide microphone mute on macOS.
  Works with all apps including Zoom and WhatsApp by swapping the default audio input
  device to BlackHole (silent virtual driver) rather than adjusting input volume.
  PAGE DOWN LED glows red when muted, blue when active.
  Includes `mic_mute_blackhole_daemon.py` — a macOS login daemon that listens for
  the hotkey (`Cmd+Shift+Ctrl+Opt+M`) and handles the device swap. Daemon always
  starts unmuted; falls back to built-in mic if saved device is unplugged.
  Requires: BlackHole 2ch (`brew install blackhole-2ch`) + reboot, pynput (`pip3 install pynput`).

### Changed
- **TSFM indicator color changed from white to blue** (0/0/255) for visual consistency
  with the mic mute active state. Both indicator LEDs now use blue for "active".

---

## [1.2.0] - 2026-03-27

### Added
- **Key Press Glow (Project 4):** Pressing any key creates a white glow under
  it that spreads softly to neighboring keys and fades out over 1 second.
  Up to 10 simultaneous glows. Pressing the same key again resets its timer.
  All tunable via `config.h` (GLOW_MAX_COUNT, GLOW_DURATION_MS, GLOW_RADIUS,
  GLOW_BRIGHTNESS).

- **Sleep Animation (Project 3):** After 10 minutes of idle, the wave
  gradually slows over 30 seconds, then fades to black over 10 seconds.

- **Wake Animation (Project 3):** First keypress after sleep triggers a ripple
  that expands outward from the pressed key, fading the wave back in behind it.
  Color and speed configurable via config.h.

### Changed
- Firmware version bumped to 1.2.0
- `config.h` now covers all 4 projects in a single file

---

## [1.1.0] - 2026-03-26

### Added
- Toggleable Standard Function Mode (TSFM): HOME key remaps F-row to F1-F12.
  HOME key glows white when active. Fn+Fkey always restores media/brightness.

### Changed
- Keymap folder renamed from `mywave` to `keychron_q1_max_custom_firmware`
- Compile/flash commands updated accordingly

---

## [1.0.0] - 2026-03-21

### Added
- Initial release
- Custom wave RGB effect: eggshell green to phthalo green, top-to-bottom sine wave
- All wave parameters configurable in config.h (colors, direction, speed, frequency)
