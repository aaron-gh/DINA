# DINA — DINA Is Not Aesthetics

**DINA** is a stripped-down, accessibility-first fork of [dwm](https://dwm.suckless.org/) tailored for blind users and screen reader workflows.

---

## 🧠 Philosophy

> DINA doesn’t draw. DINA doesn’t float. DINA doesn’t get in your way.  
> It’s for users who hear, not see.

DINA removes all visual overhead and focuses on **speech-based**, **keyboard-only** navigation — ideal for `Orca` users and terminal-centric workflows.

---

## ✨ What DINA Does

- ✅ Always uses **monocle layout** (fullscreen windows)
- ❌ No status bar
- ❌ No layout switching
- ❌ No floating window support
- ✅ Audio + speech feedback via `sox` and `spd-say`
- ✅ YAD-based application launcher, accessible with Orca
- ✅ Screen blackout toggle script for full visual suppression
- ✅ Low battery warning via `notify-send` + `spd-say`
- ✅ Clean keyboard control via `sxhkd`

---

## 🧱 Installation

### 1. Build and install the `dwm` binary (requires `sudo`):

```sh
make
sudo make install
```

### 2. Install user configs and scripts (as your user):

```sh
make userinstall
```

### 3. Interactive Setup (optional)

For an accessible, interactive setup that helps you configure your preferred applications:

```sh
make interactive-userinstall
```

This will:
- Detect installed browsers, file managers, and terminals
- Let you select your preferred applications using arrow keys
- Update keybindings for Super+W (browser), Super+F (file manager), and Super+Return (terminal)
- Fully accessible with dialog menus for screen reader users

---

## 📁 What’s Installed

| Component                  | Path                                      |
|---------------------------|-------------------------------------------|
| `dwm` binary              | `/usr/local/bin/dwm`                      |
| Accessibility scripts     | `~/.local/bin/`                           |
| `sxhkdrc` config          | `~/.config/sxhkd/sxhkdrc`                 |

> 💡 The systemd service to restore screen blackout on resume was removed.  
Users can toggle blackout manually using the provided script.

---

## 🗝️ Default Keybindings (via `sxhkd`)

| Key                        | Action                     |
|---------------------------|----------------------------|
| `Super + j`               | Focus previous window      |
| `Super + l`               | Focus next window          |
| `Super + k`               | Kill focused window        |
| `Super + a`               | Launch accessible app menu |
| `Super + b`               | Toggle screen blackout     |
| `Super + f`               | Open file manager          |
| `Super + w`               | Open web browser           |
| `Super + Return`          | Open terminal              |
| `Super + Backspace`       | Power/session menu         |
| `Super + [1–9]`           | Switch workspace/tag       |
| `Super + Shift + q`       | Quit DINA                  |

---

## 📦 Runtime Dependencies

You should have the following packages installed:

- `libx11`, `libxinerama`, `xorg-xrandr`
- `make`, `gcc`
- `orca`, `yad`, `sxhkd`, `sox`, `acpi`, `speech-dispatcher`
- `dialog` (for interactive setup)
- A terminal emulator like `alacritty`, `foot`, or `xterm`

---

## 🔗 License

DINA is licensed under the MIT License. See the `LICENSE` file.
