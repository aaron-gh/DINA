#!/bin/bash
#
# DINA First-Run Setup Script for Debian Package
#
# This script is triggered automatically on first login after installing
# the DINA Debian package. It performs user-specific setup and runs
# the interactive configuration using x-terminal-emulator.

# Create flag file directory
mkdir -p "$HOME/.config/DINA"

# Function to find a terminal emulator
find_terminal() {
    for term in x-terminal-emulator gnome-terminal konsole xfce4-terminal mate-terminal alacritty xterm; do
        if command -v "$term" >/dev/null 2>&1; then
            echo "$term"
            return 0
        fi
    done
    return 1
}

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

    # Create a wrapper script for the interactive setup
    SETUP_WRAPPER="$HOME/.config/DINA/setup-wrapper.sh"
    cat > "$SETUP_WRAPPER" << 'EOF'
#!/bin/bash
echo "DINA Interactive Setup"
echo "======================"
echo 
if [ -x "$HOME/.local/bin/interactive-userinstall" ]; then
    "$HOME/.local/bin/interactive-userinstall"
elif [ -x "/usr/share/DINA/scripts/interactive-userinstall" ]; then
    "/usr/share/DINA/scripts/interactive-userinstall"
else
    echo "Error: interactive-userinstall script not found"
    echo "Press Enter to exit..."
    read
    exit 1
fi
echo
echo "Setup completed. Press Enter to close this window..."
read
EOF
    chmod +x "$SETUP_WRAPPER"

    # Find a terminal emulator
    TERMINAL=$(find_terminal)
    
    if [ -n "$TERMINAL" ]; then
        # Launch the setup in a terminal
        case "$TERMINAL" in
            x-terminal-emulator)
                x-terminal-emulator -e "$SETUP_WRAPPER"
                ;;
            gnome-terminal|xfce4-terminal|mate-terminal)
                "$TERMINAL" -- "$SETUP_WRAPPER"
                ;;
            konsole)
                "$TERMINAL" -e "$SETUP_WRAPPER"
                ;;
            *)
                "$TERMINAL" -e "$SETUP_WRAPPER"
                ;;
        esac
    else
        # Fallback to YAD dialog if available and no terminal is found
        if command -v yad >/dev/null 2>&1; then
            yad --text="No terminal emulator found. Please run the interactive setup manually:\n\n<tt>$HOME/.local/bin/interactive-userinstall</tt>" \
                --title="DINA Setup" --button="Close:0"
        else
            # Last resort: create a .desktop file for manual setup
            mkdir -p "$HOME/.local/share/applications"
            cat > "$HOME/.local/share/applications/dina-setup.desktop" << EOF
[Desktop Entry]
Type=Application
Name=DINA Setup
Comment=Configure DINA Window Manager
Exec=$HOME/.local/bin/interactive-userinstall
Terminal=true
Categories=Settings;
EOF
            # Notify the user
            if command -v notify-send >/dev/null 2>&1; then
                notify-send -a "DINA Setup" "Please run DINA Setup manually" \
                           "Look for DINA Setup in your applications menu."
            fi
        fi
    fi

    # Create flag file to indicate first run is complete
    touch "$HOME/.config/DINA/setup-complete"

    # Notify that setup process has been handled
    if command -v notify-send >/dev/null 2>&1; then
        notify-send -a "DINA Setup" "DINA first-run setup completed" \
                   "Your preferences have been configured."
    fi

    # If using Orca, announce completion
    if pgrep -x "orca" > /dev/null; then
        if command -v spd-say >/dev/null 2>&1; then
            spd-say -r -20 "DINA setup has been completed."
        fi
    fi
fi

# Remove this script from autostart if it exists
if [ -f "$HOME/.config/autostart/dina-first-run.desktop" ]; then
    rm "$HOME/.config/autostart/dina-first-run.desktop"
fi

exit 0