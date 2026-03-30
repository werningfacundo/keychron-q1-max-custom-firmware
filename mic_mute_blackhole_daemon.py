#!/usr/bin/env python3
"""
Mic Mute Daemon - BlackHole Input Swap v2
- Mutes by swapping default input to BlackHole (silence)
- Unmutes by restoring the previous real input device
- Always starts UNMUTED, fixes any leftover BlackHole state
- Falls back to built-in mic if saved device was unplugged
"""

import ctypes
import ctypes.util
import struct
import subprocess
import sys
import os

# ── CoreAudio via ctypes ──────────────────────────────────────────────────────

_ca = ctypes.CDLL(ctypes.util.find_library('CoreAudio'))
_cf = ctypes.CDLL(ctypes.util.find_library('CoreFoundation'))

_ca.AudioObjectGetPropertyDataSize.restype = ctypes.c_int32
_ca.AudioObjectGetPropertyDataSize.argtypes = [
    ctypes.c_uint32, ctypes.c_void_p, ctypes.c_uint32,
    ctypes.c_void_p, ctypes.POINTER(ctypes.c_uint32)
]
_ca.AudioObjectGetPropertyData.restype = ctypes.c_int32
_ca.AudioObjectGetPropertyData.argtypes = [
    ctypes.c_uint32, ctypes.c_void_p, ctypes.c_uint32,
    ctypes.c_void_p, ctypes.POINTER(ctypes.c_uint32), ctypes.c_void_p
]
_ca.AudioObjectSetPropertyData.restype = ctypes.c_int32
_ca.AudioObjectSetPropertyData.argtypes = [
    ctypes.c_uint32, ctypes.c_void_p, ctypes.c_uint32,
    ctypes.c_void_p, ctypes.c_uint32, ctypes.c_void_p
]
_cf.CFStringGetLength.restype = ctypes.c_long
_cf.CFStringGetLength.argtypes = [ctypes.c_void_p]
_cf.CFStringGetCString.restype = ctypes.c_bool
_cf.CFStringGetCString.argtypes = [
    ctypes.c_void_p, ctypes.c_char_p, ctypes.c_long, ctypes.c_uint32
]

KAUDIOSYSTEM = 1
PROP_DEFAULT_INPUT = struct.pack('III', 0x64496e20, 0x676c6f62, 0)
LOG_FILE = '/tmp/micmute.log'

# ── Logging ───────────────────────────────────────────────────────────────────

def log(msg):
    with open(LOG_FILE, 'a') as f:
        f.write(msg + '\n')
    print(msg, flush=True)

# ── CoreAudio helpers ─────────────────────────────────────────────────────────

def _cfstr_to_str(cfstr_ptr):
    length = _cf.CFStringGetLength(cfstr_ptr)
    buf = ctypes.create_string_buffer(length * 4 + 1)
    _cf.CFStringGetCString(cfstr_ptr, buf, len(buf), 0x08000100)
    return buf.value.decode('utf-8', errors='replace')

def get_device_uid(dev_id):
    addr = struct.pack('III', 0x75696420, 0x676c6f62, 0)
    sz = ctypes.c_uint32(8)
    cfstr = ctypes.c_void_p(0)
    err = _ca.AudioObjectGetPropertyData(dev_id, addr, 0, None, ctypes.byref(sz), ctypes.byref(cfstr))
    if err == 0 and cfstr.value:
        return _cfstr_to_str(cfstr.value)
    return None

def get_all_device_ids():
    addr = struct.pack('III', 0x64657623, 0x676c6f62, 0)
    size = ctypes.c_uint32(0)
    _ca.AudioObjectGetPropertyDataSize(1, addr, 0, None, ctypes.byref(size))
    buf = (ctypes.c_uint32 * (size.value // 4))()
    _ca.AudioObjectGetPropertyData(1, addr, 0, None, ctypes.byref(size), buf)
    return list(buf)

def get_default_input_device():
    sz = ctypes.c_uint32(4)
    dev_id = ctypes.c_uint32(0)
    err = _ca.AudioObjectGetPropertyData(KAUDIOSYSTEM, PROP_DEFAULT_INPUT, 0, None, ctypes.byref(sz), ctypes.byref(dev_id))
    return dev_id.value if err == 0 else None

def set_default_input_device(dev_id):
    val = ctypes.c_uint32(dev_id)
    err = _ca.AudioObjectSetPropertyData(KAUDIOSYSTEM, PROP_DEFAULT_INPUT, 0, None, 4, ctypes.byref(val))
    return err == 0

def find_device_by_uid_fragment(fragment):
    for dev_id in get_all_device_ids():
        uid = get_device_uid(dev_id)
        if uid and fragment in uid:
            return dev_id
    return None

def device_is_connected(dev_id):
    return dev_id in get_all_device_ids()

def get_builtin_mic_id():
    return find_device_by_uid_fragment('BuiltInMicrophone')

# ── Mute state ────────────────────────────────────────────────────────────────

class MuteState:
    def __init__(self, blackhole_id, builtin_mic_id):
        self.blackhole_id = blackhole_id
        self.builtin_mic_id = builtin_mic_id
        self.muted = False
        self.saved_input_id = None

    def _best_fallback(self):
        """Return the best available real input device."""
        if self.saved_input_id and device_is_connected(self.saved_input_id):
            return self.saved_input_id
        if self.builtin_mic_id and device_is_connected(self.builtin_mic_id):
            log('Saved device gone — falling back to built-in mic')
            return self.builtin_mic_id
        log('ERROR: No real input device found to restore to')
        return None

    def force_unmute(self):
        """Called on startup — always restore a real input device."""
        # Refresh built-in mic ID in case it changed
        self.builtin_mic_id = get_builtin_mic_id()
        target = self.builtin_mic_id or self._best_fallback()
        if target is None:
            log('ERROR: Could not find any real input device on startup')
            return
        uid = get_device_uid(target)
        if set_default_input_device(target):
            self.saved_input_id = target
            self.muted = False
            log(f'Startup: forced unmuted (input set to {uid})')
        else:
            log('ERROR: Could not set default input on startup')

    def mute(self):
        current = get_default_input_device()
        if current == self.blackhole_id:
            log('Already muted')
            self.muted = True
            return
        self.saved_input_id = current
        uid = get_device_uid(current) if current else 'unknown'
        if set_default_input_device(self.blackhole_id):
            self.muted = True
            log(f'Mic muted (swapped {uid} → BlackHole)')
        else:
            log('ERROR: Failed to set BlackHole as default input')

    def unmute(self):
        target = self._best_fallback()
        if target is None:
            return
        uid = get_device_uid(target)
        if set_default_input_device(target):
            self.muted = False
            log(f'Mic unmuted (restored {uid})')
        else:
            log('ERROR: Failed to restore default input')

    def toggle(self):
        if self.muted:
            self.unmute()
        else:
            self.mute()

# ── Hotkey listener ───────────────────────────────────────────────────────────

def run_daemon():
    from pynput import keyboard

    HOTKEY = {
        keyboard.Key.cmd,
        keyboard.Key.shift,
        keyboard.Key.ctrl,
        keyboard.Key.alt,
        keyboard.KeyCode.from_char('m'),
    }

    blackhole_id = find_device_by_uid_fragment('BlackHole')
    if blackhole_id is None:
        log('ERROR: BlackHole device not found. Is it installed and rebooted?')
        sys.exit(1)
    log(f'BlackHole device ID: {blackhole_id}')

    builtin_mic_id = get_builtin_mic_id()
    if builtin_mic_id:
        log(f'Built-in mic device ID: {builtin_mic_id}')
    else:
        log('WARNING: Built-in mic not found — no fallback available')

    state = MuteState(blackhole_id, builtin_mic_id)

    # Always start unmuted — keyboard LED also resets on power cycle
    current_input = get_default_input_device()
    if current_input == blackhole_id:
        log('Leftover muted state detected on startup — fixing...')
        state.force_unmute()
    else:
        state.saved_input_id = current_input
        uid = get_device_uid(current_input) if current_input else 'unknown'
        log('Mic mute daemon started. Current state: unmuted')
        log(f'Active input: {uid}')

    log('Hotkey: Cmd+Shift+Ctrl+Opt+M')

    _pressed = set()
    _fired = False

    def on_press(key):
        nonlocal _fired
        _pressed.add(key)
        if HOTKEY.issubset(_pressed) and not _fired:
            _fired = True
            state.toggle()

    def on_release(key):
        nonlocal _fired
        _pressed.discard(key)
        if not HOTKEY.issubset(_pressed):
            _fired = False

    with keyboard.Listener(on_press=on_press, on_release=on_release) as listener:
        listener.join()

# ── Install as launchd login item ─────────────────────────────────────────────

def install():
    script_path = os.path.abspath(__file__)
    python_path = sys.executable
    plist_path = os.path.expanduser('~/Library/LaunchAgents/com.keychron.micmute.plist')

    plist = f"""<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>Label</key>
    <string>com.keychron.micmute</string>
    <key>ProgramArguments</key>
    <array>
        <string>{python_path}</string>
        <string>{script_path}</string>
    </array>
    <key>RunAtLoad</key>
    <true/>
    <key>KeepAlive</key>
    <true/>
    <key>StandardOutPath</key>
    <string>{LOG_FILE}</string>
    <key>StandardErrorPath</key>
    <string>{LOG_FILE}</string>
</dict>
</plist>"""

    with open(plist_path, 'w') as f:
        f.write(plist)

    subprocess.run(['launchctl', 'load', plist_path], check=True)
    print(f'Installed and started as login item.')
    print(f'Plist: {plist_path}')
    print(f'Log:   {LOG_FILE}')
    print(f'To uninstall: launchctl unload {plist_path} && rm {plist_path}')


if __name__ == '__main__':
    if '--install' in sys.argv:
        install()
    else:
        run_daemon()
