# DEVELOPMENT.md — Keychron Q1 Max Custom Firmware

## Prerequisites
These are already installed. This section is for reference if setting up on a new machine.

- macOS (Apple Silicon)
- Homebrew: `/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"`
- QMK: `pip3 install qmk && qmk setup`
- Keychron QMK fork: `git clone https://github.com/Keychron/qmk_firmware --branch wireless_playground --recurse-submodules ~/keychron_firmware`
- Compiler (osx-cross v9, NOT default Homebrew gcc which is broken on Apple Silicon):
  ```bash
  brew uninstall --ignore-dependencies arm-none-eabi-binutils
  brew uninstall --ignore-dependencies arm-none-eabi-gcc
  brew install osx-cross/arm/arm-none-eabi-gcc@9
  ```
- Compiler PATH is permanently set in `~/.zprofile` — run `source ~/.zprofile` at the start of a new terminal session before compiling.

## Project 5: macOS Mic Mute Daemon

The mic mute feature requires two things outside the firmware:

**1. BlackHole virtual audio driver:**
```bash
brew install blackhole-2ch
```
Reboot after installing — required for the driver to activate.

**2. pynput Python library:**
```bash
pip3 install pynput
```

**3. Install and start the daemon:**
```bash
python3 ~/keychron_firmware/keyboards/keychron/q1_max/ansi_encoder/keymaps/keychron_q1_max_custom_firmware/mic_mute_blackhole_daemon.py --install
```

This registers the daemon as a login item. It starts automatically on login.
Log file: `/tmp/micmute.log`

To uninstall:
```bash
launchctl unload ~/Library/LaunchAgents/com.keychron.micmute.plist && rm ~/Library/LaunchAgents/com.keychron.micmute.plist
```

## Making Changes
All user-facing customization is in `config.h`. Edit that file first before touching anything else.

To open the keymap folder in Finder:
```bash
open ~/keychron_firmware/keyboards/keychron/q1_max/ansi_encoder/keymaps/keychron_q1_max_custom_firmware/
```

When editing `.c`, `.h`, or `.inc` files, use a plain text editor that does NOT add smart quotes.
Recommended: VS Code. Do NOT use TextEdit (corrupts files with smart quotes).

## Compile
```bash
source ~/.zprofile && cd ~/keychron_firmware
qmk compile -kb keychron/q1_max/ansi_encoder -km keychron_q1_max_custom_firmware
```
A successful compile ends with all `[OK]` lines and a `.bin` file.

## Flash
```bash
qmk flash -kb keychron/q1_max/ansi_encoder -km keychron_q1_max_custom_firmware
```
When it says "Waiting for bootloader...":
1. Unplug the USB cable
2. Hold Esc on the keyboard
3. Plug the USB cable back in
4. Release Esc

The keyboard will flash automatically and reboot.

## Compile + Flash in One Command
```bash
source ~/.zprofile && cd ~/keychron_firmware && qmk compile -kb keychron/q1_max/ansi_encoder -km keychron_q1_max_custom_firmware && qmk flash -kb keychron/q1_max/ansi_encoder -km keychron_q1_max_custom_firmware
```

## Troubleshooting

**"arm-none-eabi-gcc: command not found"**
Run `source ~/.zprofile` first.

**"bquote>" appears when pasting a command**
The command contains hidden characters from copy-paste. Press Ctrl+C and retype manually.

**Wave effect is janky/snapping**
Speed was accidentally changed via Fn+arrow keys. Press Fn+Down Arrow a few times to slow it back down — no reflashing needed.

**Keyboard not detected when flashing**
Make sure the keyboard is in wired/USB mode (not Bluetooth). The side switch should be set to USB/wired.

**F11 triggers Show Desktop on macOS**
macOS intercepts F11 at system level. To disable:
```bash
defaults write com.apple.symbolichotkeys AppleSymbolicHotKeys -dict-add 36 "{enabled = 0; value = {parameters = (65535, 103, 8388608); type = standard;};}" && killall SystemUIServer
```
Then log out and back in.

**Mic mute has no effect on Zoom or WhatsApp**
Make sure the daemon is running (`cat /tmp/micmute.log`) and BlackHole is installed and visible in System Settings → Sound. If BlackHole is missing, reinstall it and reboot.
