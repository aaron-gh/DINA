# DINA — DINA Is Not Aesthetics

**DINA** is a stripped-down, accessibility-first window manager tailored for blind users and screen reader workflows. Originally inspired by minimalist principles, DINA has evolved into its own independent project focused solely on accessibility.

---

## 🧠 Philosophy

> DINA doesn't draw. DINA doesn't float. DINA doesn't get in your way.  
> It's for users who hear, not see.

DINA removes all visual overhead and focuses on **speech-based**, **keyboard-only** navigation — ideal for `Orca` users and terminal-centric workflows.

---

## ✨ What DINA Does

- ✅ Completely headless **monocle layout** (fullscreen windows) 
- ❌ No status bar or UI elements whatsoever
- ❌ No layout switching or tiling options
- ❌ No floating window support
- ❌ No visual elements - 100% optimized for screen readers
- ✅ Audio + speech feedback via `sox` and `spd-say`
- ✅ YAD-based application launcher, accessible with Orca
- ✅ Screen blackout toggle script for full visual suppression
- ✅ Low battery warning via `notify-send` + `spd-say`
- ✅ Advanced multi-layered keybinding system built directly into DINA
- ✅ Orca screen reader integration with automatic startup
- ✅ Workspace memory that remembers which applications belong on which workspaces
- ✅ Fully modular code architecture for easy maintenance

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
- Sets up built-in keybinding configuration
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

> 💡 The systemd service to restore screen blackout on resume was removed.  
> Users can toggle blackout manually using the provided script.

---

## 🗝️ Keybinding System

DINA features a multi-layered keybinding system with different modes for different contexts. This creates a more efficient keyboard workflow and reduces conflicts between shortcuts.

## Modes

- **Normal Mode** (default): Basic window management and navigation
- **Command Mode** (`Super + c`): Advanced commands and settings
- **Window Mode** (`Super + w`): Window manipulation commands
- **Tag Mode** (`Super + t`): Workspace/tag management

Each mode has its own set of keybindings, and the system provides audio feedback when switching modes. Press `Escape` to return to normal mode from any other mode.

## Default Keybindings

### Normal Mode

| Key                  | Action                     |
|---------------------|----------------------------|
| `Super + j`         | Focus previous window      |
| `Super + l`         | Focus next window          |
| `Super + k`         | Kill focused window        |
| `Super + a`         | Launch accessible app menu |
| `Super + b`         | Toggle screen blackout     |
| `Super + Return`    | Open terminal              |
| `Super + Backspace` | Power/session menu         |
| `Super + [1–9]`     | Switch workspace/tag       |
| `Super + Shift + [1-9]` | Move window to workspace |
| `Super + Shift + q` | Quit DINA                  |
| `Super + c`         | Enter command mode         |
| `Super + w`         | Enter window mode          |
| `Super + t`         | Enter tag mode             |

### Command Mode
| Key               | Action                     |
|------------------|----------------------------|
| `Super + q`      | Example command action     |
| `Super + w`      | Example command action     |
| `Super + e`      | Example command action     |
| `Escape`         | Return to normal mode      |

### Window Mode
| Key               | Action                     |
|------------------|----------------------------|
| `Super + h`      | Move window left           |
| `Super + j`      | Move window down           |
| `Super + k`      | Move window up             |
| `Super + l`      | Move window right          |
| `Escape`         | Return to normal mode      |

### Tag Mode
| Key               | Action                     |
|------------------|----------------------------|
| `Super + [1-9]`  | Switch to workspace        |
| `Escape`         | Return to normal mode      |

---

## ⚙️ Configuration System

DINA uses a human-readable configuration file located at `~/.config/dina/config`. This file allows you to customize keybindings, autostart applications, workspace settings, and accessibility options without recompiling.

### Key Features

1. **Human-Readable Format**: Easy-to-edit TOML-like syntax that doesn't require programming knowledge
2. **Dynamic Loading**: Changes take effect when DINA starts without recompilation
3. **Comprehensive Options**: Control all aspects of DINA's behavior

### Configuration Sections

- **Appearance**: Border width, font settings
- **Layout**: Layout options (currently only monocle is supported)
- **Workspaces**: Tag names and configuration
- **Keybindings**: Keyboard shortcuts for all actions
- **Autostart**: Programs to launch when DINA starts
- **Accessibility**: Sound and speech preferences
- **Rules**: Window placement rules

### Example Configuration

```toml
[keybindings]
modifier = "super"
"modifier+Return" = "exec:alacritty"
"modifier+a" = "exec:~/.local/bin/app-launcher"
"modifier+b" = "exec:~/.local/bin/toggle-blackout"

[autostart]
orca = "~/.local/bin/start-orca"
compositor = "picom --daemon"

[accessibility]
enable_sounds = true
enable_screen_reader = true
announce_workspace_changes = true
```

For full documentation, see the `man dina-config` manual page.

## 💻 Project Independence

DINA began as a fork of dwm but has since evolved into a completely independent project with:

- ✅ Human-readable configuration (no recompiling needed)
- ✅ Modular architecture with clean separation of concerns
- ✅ Specialized accessibility features for blind users
- ✅ Focus on screen reader compatibility rather than visual elements
- ✅ Utility scripts for configuration management

While we appreciate the minimalist philosophy that inspired this project, DINA now stands on its own with a focus on accessibility first.

## 🗂️ Workspace Configuration

DINA remembers which applications belong on which workspaces, creating a consistent and predictable environment.

### How It Works

1. **Automatic Placement**: When you launch an application that has previously been assigned to a specific workspace, DINA automatically places it there and announces this with audio feedback.

2. **Setting Preferences**: To set a workspace preference for an application:
   - Move the application to your desired workspace using `Super + Shift + [1-9]`
   - DINA will remember this preference and apply it automatically next time

3. **Configuration File**: Your workspace layout is stored in `~/.config/dina/workspace` as a readable text file you can also edit manually.

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
- `orca`, `yad`, `sox`, `acpi`, `speech-dispatcher`
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

---

## 👏 Credits

DINA was created with accessibility as the primary goal. Special thanks to:

- The Orca screen reader project for making Linux accessible
- The Linux accessibility community for their feedback and support
- Various open source projects that have inspired DINA's minimalist approach

DINA is maintained as an open source project dedicated to making Linux window management accessible for blind users.
