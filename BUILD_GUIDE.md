# DINA Build Guide

## Overview

DINA (DINA Is Not Aesthetics) is a modular window manager for blind users and screen reader workflows. It is based on dwm but with a focus on accessibility.

## Dependencies

To build DINA, you need the following dependencies:

- C compiler (gcc or clang)
- X11 development libraries
- Xinerama extension
- Freetype2 development libraries
- Fontconfig development libraries
- Xft development libraries

On Debian/Ubuntu, you can install these with:

```bash
sudo apt-get install build-essential libx11-dev libxinerama-dev libfreetype6-dev libfontconfig1-dev libxft-dev
```

On Arch Linux, you can install these with:

```bash
sudo pacman -S base-devel libx11 libxinerama freetype2 fontconfig libxft
```

## Building

To build DINA:

1. Clone the repository:
   ```bash
   git clone https://github.com/aaron-gh/DINA.git
   cd DINA
   ```

2. Build the window manager:
   ```bash
   make
   ```

3. Install it (optional):
   ```bash
   sudo make install
   ```

4. For user-specific installation:
   ```bash
   make userinstall
   ```

## Modular Architecture

DINA is organized into the following modules:

- **Core**: Main window manager functionality
  - `core/dina.c`: Main entry point and initialization
  - `core/config.c`: Configuration handling
  - `core/event.c`: X11 event handling

- **Window Management**: Window and workspace management
  - `wm/window.c`: Window management
  - `wm/monitor.c`: Monitor handling
  - `wm/tag.c`: Tag/workspace handling
  - `wm/rules.c`: Window rule application

- **UI**: Drawing and user interface
  - `ui/drw.c`: Drawing utilities

- **Accessibility**: Features for blind users
  - `a11y/notify.c`: Audio notifications
  - `a11y/workspace_memory.c`: Workspace persistence

- **Utilities**: Helper functions
  - `util/util.c`: Various utility functions

## Troubleshooting

### Compilation Issues

If you encounter compilation issues related to missing headers:

1. **Missing Freetype headers**: Ensure you have freetype2 development files installed. The correct include path should be `/usr/include/freetype2/ft2build.h`.

2. **Missing X11 headers**: Ensure X11 development files are installed and correctly referenced in `config.mk`.

3. **Include path issues**: If the modular includes aren't working, make sure your compiler is respecting the `-I` flags in the Makefile.

### Runtime Issues

1. **Screen reader integration**: DINA attempts to start Orca screen reader at startup. Make sure you have Orca installed if you need this feature.

2. **Audio feedback**: DINA uses the `play` command from SoX and `spd-say` for audio feedback. Install these packages if needed.

## Contributing

When contributing to DINA, please follow the modular architecture:

1. Keep functionality in its appropriate module
2. Add detailed Doxygen-style comments
3. Update header files with proper declarations
4. Follow the existing code style