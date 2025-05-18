#!/bin/bash
#
# DINA First-Run Setup Script for Debian Package
#
# This script is triggered automatically on first login after installing
# the DINA Debian package. It performs user-specific setup and runs
# the interactive configuration.

# Create flag file directory
mkdir -p "$HOME/.config/DINA"

# Check if this is the first run for this user
if [ ! -f "$HOME/.config/DINA/setup-complete" ]; then
    # Create notification that setup is starting
    if command -v notify-send >/dev/null 2>&1; then
        notify-send -a "DINA Setup" "DINA first-run setup is starting" \
                   "This will configure your preferred applications for DINA."
    fi

    # If using Orca, announce that setup is starting
    if pgrep -x "orca" > /dev/null; then
        if command -v spd-say >/dev/null 2>&1; then
            spd-say -r -20 "DINA first-run setup is starting. This will configure your preferred applications."
        fi
    fi

    # Copy scripts to user's local bin directory
    mkdir -p "$HOME/.local/bin"
    cp -f /usr/share/DINA/scripts/* "$HOME/.local/bin/"
    chmod +x "$HOME/.local/bin/"*

    # Run the interactive setup
    "$HOME/.local/bin/interactive-userinstall" || /usr/share/DINA/scripts/interactive-userinstall

    # Create flag file to indicate setup is complete
    touch "$HOME/.config/DINA/setup-complete"

    # Notify that setup is complete
    if command -v notify-send >/dev/null 2>&1; then
        notify-send -a "DINA Setup" "DINA setup complete" \
                   "Your preferences have been configured."
    fi

    # If using Orca, announce that setup is complete
    if pgrep -x "orca" > /dev/null; then
        if command -v spd-say >/dev/null 2>&1; then
            spd-say -r -20 "DINA setup is complete. Your preferences have been configured."
        fi
    fi
fi

# Remove this script from autostart if it exists
if [ -f "$HOME/.config/autostart/dina-first-run.desktop" ]; then
    rm "$HOME/.config/autostart/dina-first-run.desktop"
fi

exit 0