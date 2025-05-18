# DINA Debian Package

This is a Debian package for DINA (DINA Is Not Aesthetics), an accessibility-first window manager tailored for blind users.

## Installation

To install:

```sh
sudo dpkg -i dina-1.0-*.deb
sudo apt-get install -f  # This will install any missing dependencies
```

## Using DINA

After installation:

1. **First Login**:
   - Log out of your current session
   - At the login screen, select "DINA" as your session type
   - Log in
   - The interactive setup will run automatically on first login

2. **Interactive Setup**:
   - When prompted, select your preferred browser, file manager, and terminal
   - Use arrow keys to navigate and Enter to select
   - Press Escape to cancel at any point

3. **Basic Usage**:
   - Super+Return: Launch terminal
   - Super+W: Launch web browser
   - Super+F: Launch file manager
   - Super+A: Open accessible application launcher
   - Super+[1-9]: Switch workspaces
   - Super+Shift+Q: Quit DINA

## Features

- Audio and speech feedback for window operations
- Automatic Orca screen reader startup
- Single application view (monocle layout)
- Keyboard-only navigation
- No visual distractions

## Dependencies

The package automatically installs the following dependencies:
- sxhkd (keyboard shortcuts daemon)
- speech-dispatcher (speech synthesis)
- sox (sound processing)
- dialog (interactive setup)
- orca (screen reader)
- yad (accessible dialogs)
- acpi (battery status)

## Support

For support and more information, visit:
https://github.com/aaron-gh/DINA