# AGENTS.md — Keychron Q1 Max Custom Firmware

## Project Overview
Custom QMK firmware for Keychron Q1 Max (ANSI encoder, wireless).
Two independent projects live in a single keymap called `mywave`.

## Repo Structure
```
~/keychron_firmware/                          <- Keychron QMK fork (wireless_playground branch)
  keyboards/keychron/q1_max/ansi_encoder/
    keymaps/mywave/
      config.h             <- ALL user customization. Edit this, not the others.
      keymap.c             <- TSFM logic + layer keymaps
      rgb_matrix_user.inc  <- Wave effect animation
      AGENTS.md            <- This file
      DEVELOPMENT.md       <- Setup + compile + flash guide
      README.md            <- Human-facing project docs
```

## Compile & Flash
```bash
# Required every new terminal session (until added to ~/.zprofile)
export PATH="/opt/homebrew/Cellar/arm-none-eabi-gcc@9/9.5.0_2/bin:$PATH"
export PATH="/opt/homebrew/Cellar/arm-none-eabi-binutils/2.41/bin:$PATH"

cd ~/keychron_firmware
qmk compile -kb keychron/q1_max/ansi_encoder -km mywave
qmk flash -kb keychron/q1_max/ansi_encoder -km mywave
# When flashing: unplug keyboard -> hold Esc -> plug in -> release Esc
```

## Key Rules
- ONLY edit files in `keyboards/keychron/q1_max/ansi_encoder/keymaps/mywave/`
- ALL tuning/customization goes in `config.h` — never hardcode values in other files
- Always compile before flashing to catch errors
- Use `printf '...\n' >` syntax for file writes — never heredoc (causes bquote errors on this machine)
- User is non-technical: provide single copy-paste commands whenever possible
- Never edit files with TextEdit (corrupts with smart quotes)

## Critical Implementation Notes
- `KC_MCTRL` and `KC_LNPAD` are intercepted by `process_record_keychron_common()` —
  TSFM remapping for these MUST happen before that call in `process_record_user()`
- `IS_LAYER_ON()` has timing issues for detecting Fn — use the `fn_held` bool instead
- Compiler is arm-none-eabi-gcc 9.5.0_2 from osx-cross — NOT the default Homebrew gcc
  (v15 is broken, missing stdint.h)
- Q1 Max only exists in Keychron's fork on `wireless_playground` branch —
  standard QMK repo at ~/qmk_firmware is irrelevant

## Known Issues
- F11 intercepted by macOS Show Desktop at system level (not a firmware bug)
- Compiler PATH must be re-exported each new terminal session
- TSFM state resets to off on keyboard reboot (by design, could be persisted with EEPROM)

## LED Index Reference (Q1 Max ANSI 82)
- Count left-to-right, top-to-bottom starting from 0 (ESC = 0)
- HOME key = LED index 57
