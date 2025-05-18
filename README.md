# DINA — DINA Is Not Aesthetics

**DINA** is a stripped-down, accessibility-first fork of [dwm](https://dwm.suckless.org/) tailored for blind users and screen reader workflows.

---

## 🧠 Philosophy

> DINA doesn't draw. DINA doesn't float. DINA doesn't get in your way.  
> It's for users who hear, not see.

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
- ✅ Orca screen reader autostart for immediate accessibility

---

## 🧱 Installation

### Option 1: Debian Package (recommended)

For Debian-based systems (Debian, Ubuntu, etc.), you can install DINA using the pre-built package:

1. Download the latest release package from [GitHub Releases](https://github.com/aaron-gh/DINA/releases)

2. Install the package and its dependencies:
   ```sh
   sudo dpkg -i dina_1.0-1_amd64.deb
   sudo apt-get install -f  # Installs dependencies
   ```

3. Log out and select DINA from your display manager's session menu

4. On first login, the interactive setup will run automatically

### Option 2: From Source

#### 1. Build and install the `DINA` binary (requires `sudo`):

```sh
make clean
make
sudo make install
```

#### 2. Install user configs and scripts (as your user):

```sh
make userinstall
```

#### 3. Interactive Setup (optional)

For an accessible, interactive setup that helps you configure your preferred applications:

```sh
make interactive-userinstall
```

This will:
- Detect installed browsers, file managers, and terminals
- Let you select your preferred applications by choosing numbers from a simple menu
- Update keybindings for Super+W (browser), Super+F (file manager), and Super+Return (terminal)
- Fully accessible text-based interface designed for screen reader users

### Building Debian Packages

If you want to build your own Debian package, the repository includes a complete packaging system:

```sh
# Build using Docker (works on any system with Docker):
bash packaging/build-with-docker.sh

# Or directly on a Debian-based system:
bash packaging/build-package.sh
```

See the [packaging documentation](packaging/README.md) for more details.

---

## 📁 What's Installed

| Component              | Path                      |
|-----------------------|---------------------------|
| `DINA` binary         | `/usr/local/bin/DINA`     |
| Accessibility scripts | `~/.local/bin/`           |
| `sxhkdrc` config      | `~/.config/sxhkd/sxhkdrc` |

> 💡 The systemd service to restore screen blackout on resume was removed.  
> Users can toggle blackout manually using the provided script.

---

## 🗝️ Default Keybindings (via `sxhkd`)

| Key                  | Action                     |
|---------------------|----------------------------|
| `Super + j`         | Focus previous window      |
| `Super + l`         | Focus next window          |
| `Super + k`         | Kill focused window        |
| `Super + a`         | Launch accessible app menu |
| `Super + b`         | Toggle screen blackout     |
| `Super + f`         | Open file manager          |
| `Super + w`         | Open web browser           |
| `Super + Return`    | Open terminal              |
| `Super + Backspace` | Power/session menu         |
| `Super + [1–9]`     | Switch workspace/tag       |
| `Super + Shift + q` | Quit DINA                  |

---

## 📦 Dependencies

### Runtime Dependencies

You should have the following packages installed:

- `libx11`, `libxinerama`, `libxft`
- `orca`, `yad`, `sxhkd`, `sox`, `acpi`, `speech-dispatcher`
- `python3` (for interactive setup)
- A terminal emulator like `alacritty`, `foot`, or `xterm`

### Build Dependencies

For building from source:
- `make`, `gcc`
- `libx11-dev`, `libxinerama-dev`, `libxft-dev`

For building Debian packages:
- `build-essential`, `debhelper`, `devscripts`, `dpkg-dev`
- Alternatively, just Docker for cross-platform building

---

## 🔗 License

DINA is licensed under the MIT License. See the `LICENSE` file.