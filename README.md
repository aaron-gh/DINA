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
- ✅ Workspace memory that remembers which applications belong on which workspaces

---

## 🧱 Installation

### Option 1: Debian Package (recommended)

For Debian-based systems (Debian, Ubuntu, etc.), you can install DINA using the pre-built package:

1. Download the latest release package from [GitHub Releases](https://github.com/aaron-gh/DINA/releases)

2. Install the package and its dependencies:
   ```sh
   sudo dpkg -i dina_1.0.1-1_amd64.deb
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

#### 2. Setup user configs and scripts (as your user):

```sh
make userinstall
```

This will run the interactive setup that:
- Installs all utility scripts to your ~/.local/bin directory
- Sets up sxhkd configuration for keyboard shortcuts
- Detects and configures your preferred applications

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
| `Super + Shift + [1-9]` | Move window to workspace |
| `Super + Shift + q` | Quit DINA                  |

---

## 🗂️ Workspace Configuration

DINA remembers which applications belong on which workspaces, creating a consistent and predictable environment.

### How It Works

1. **Automatic Placement**: When you launch an application that has previously been assigned to a specific workspace, DINA automatically places it there and announces this with audio feedback.

2. **Setting Preferences**: To set a workspace preference for an application:
   - Move the application to your desired workspace using `Super + Shift + [1-9]`
   - DINA will remember this preference and apply it automatically next time

3. **Configuration File**: Your workspace layout is stored in `~/.config/dina/workspace_layout` as a readable text file you can also edit manually.

4. **Format**: Each line in the file follows this pattern:
   ```
   ApplicationClass|ApplicationInstance|WorkspaceNumber
   ```

5. **Clearing Preferences**: To remove an application from the workspace memory, simply move it to workspace 1. DINA will announce that the application is no longer being tracked.

### Example Configuration

```
# DINA Workspace Layout Configuration
Firefox|Navigator|2
Thunderbird|Mail|3
Terminal|xterm|4
```

> 💡 Applications will only be placed on workspaces 2-9 automatically. Workspace 1 is considered the default and not remembered.

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