#!/bin/bash
#
# DINA Debian Package Build Script
#
# This script builds a complete Debian package for DINA.
# It should be run on a Debian-based system with the required build tools.

set -e

echo "=== DINA Debian Package Builder ==="
echo "Building package directly..."

# Create build directory
BUILD_DIR="$(pwd)/packaging/build"
mkdir -p "$BUILD_DIR"

# Check if we've already built DINA
if [ ! -f "DINA" ]; then
    echo "Building DINA binary..."
    make clean
    make
fi

# Create package directory structure
echo "Creating package structure..."
PACKAGE_DIR="$BUILD_DIR/dina-1.0"
mkdir -p "$PACKAGE_DIR/DEBIAN"
mkdir -p "$PACKAGE_DIR/usr/bin"
mkdir -p "$PACKAGE_DIR/usr/share/man/man1"
mkdir -p "$PACKAGE_DIR/usr/share/DINA/scripts"
mkdir -p "$PACKAGE_DIR/usr/share/xsessions"
mkdir -p "$PACKAGE_DIR/etc/skel/.config/sxhkd"
mkdir -p "$PACKAGE_DIR/etc/xdg/autostart"

# Create control file
echo "Creating package metadata..."
cat > "$PACKAGE_DIR/DEBIAN/control" << EOF
Package: dina
Version: 1.0-1
Section: x11
Priority: optional
Architecture: amd64
Depends: libx11-6, libxinerama1, sxhkd, speech-dispatcher, sox, dialog, orca, yad, acpi
Recommends: xterm | x-terminal-emulator
Maintainer: Aaron Hewitt <aaron.graham.hewitt@gmail.com>
Description: Accessibility-first window manager for blind users
 DINA (DINA Is Not Aesthetics) is an accessibility-first fork of dwm
 window manager tailored specifically for blind users and screen reader
 workflows.
 .
 Features include monocle layout, audio feedback, and Orca autostart.
EOF

# Create postinst script
cat > "$PACKAGE_DIR/DEBIAN/postinst" << EOF
#!/bin/sh
set -e

# Update command alternatives
update-alternatives --install /usr/bin/x-window-manager x-window-manager /usr/bin/DINA 50

# Update desktop database for the new .desktop files
if command -v update-desktop-database >/dev/null 2>&1; then
    update-desktop-database -q || true
fi

echo ""
echo "DINA has been installed successfully."
echo "You can select it from your display manager's session menu."
echo "On first login, the interactive setup will run automatically."
echo ""

#DEBHELPER#

exit 0
EOF
chmod 755 "$PACKAGE_DIR/DEBIAN/postinst"

# Create postrm script
cat > "$PACKAGE_DIR/DEBIAN/postrm" << EOF
#!/bin/sh
set -e

if [ "\$1" = "remove" ] || [ "\$1" = "purge" ]; then
    update-alternatives --remove x-window-manager /usr/bin/DINA || true
fi

#DEBHELPER#

exit 0
EOF
chmod 755 "$PACKAGE_DIR/DEBIAN/postrm"

# Copy files
echo "Copying files into package..."
cp "DINA" "$PACKAGE_DIR/usr/bin/"
cp "DINA.1" "$PACKAGE_DIR/usr/share/man/man1/"
cp -r scripts/* "$PACKAGE_DIR/usr/share/DINA/scripts/"
chmod +x "$PACKAGE_DIR/usr/share/DINA/scripts/"*

# Create desktop files if they don't exist
if [ ! -f "packaging/debian/dina.desktop" ]; then
    mkdir -p "packaging/debian"
    cat > "packaging/debian/dina.desktop" << EOF
[Desktop Entry]
Name=DINA
Comment=DINA Is Not Aesthetics - Accessibility-first window manager
Exec=DINA
TryExec=DINA
Type=Application
Keywords=accessibility;screen reader;blind;
Icon=preferences-desktop-accessibility
EOF
fi

if [ ! -f "packaging/debian/dina-first-run.desktop" ]; then
    mkdir -p "packaging/debian"
    cat > "packaging/debian/dina-first-run.desktop" << EOF
[Desktop Entry]
Type=Application
Name=DINA First-Run Setup
Comment=Configure DINA Window Manager
Exec=/usr/share/DINA/dina-first-run.sh
Terminal=false
NoDisplay=true
X-GNOME-Autostart-enabled=true
X-KDE-autostart-after=panel
EOF
fi

if [ ! -f "packaging/debian/dina-first-run.sh" ]; then
    mkdir -p "packaging/debian"
    cat > "packaging/debian/dina-first-run.sh" << EOF
#!/bin/bash
#
# DINA First-Run Setup Script for Debian Package
#
# This script is triggered automatically on first login after installing
# the DINA Debian package. It performs user-specific setup and runs
# the interactive configuration using x-terminal-emulator.

# Create flag file directory
mkdir -p "\$HOME/.config/DINA"

# Function to find a terminal emulator
find_terminal() {
    for term in x-terminal-emulator gnome-terminal konsole xfce4-terminal mate-terminal alacritty xterm; do
        if command -v "\$term" >/dev/null 2>&1; then
            echo "\$term"
            return 0
        fi
    done
    return 1
}

# Check if this is the first run for this user
if [ ! -f "\$HOME/.config/DINA/setup-complete" ]; then
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
    mkdir -p "\$HOME/.local/bin"
    cp -f /usr/share/DINA/scripts/* "\$HOME/.local/bin/"
    chmod +x "\$HOME/.local/bin/"*

    # Create a wrapper script for the interactive setup
    SETUP_WRAPPER="\$HOME/.config/DINA/setup-wrapper.sh"
    cat > "\$SETUP_WRAPPER" << 'WRAPPEREOF'
#!/bin/bash
echo "DINA Interactive Setup"
echo "======================"
echo 
if [ -x "\$HOME/.local/bin/interactive-userinstall" ]; then
    "\$HOME/.local/bin/interactive-userinstall"
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
WRAPPEREOF
    chmod +x "\$SETUP_WRAPPER"

    # Find a terminal emulator
    TERMINAL=\$(find_terminal)
    
    if [ -n "\$TERMINAL" ]; then
        # Launch the setup in a terminal
        case "\$TERMINAL" in
            x-terminal-emulator)
                x-terminal-emulator -e "\$SETUP_WRAPPER"
                ;;
            gnome-terminal|xfce4-terminal|mate-terminal)
                "\$TERMINAL" -- "\$SETUP_WRAPPER"
                ;;
            konsole)
                "\$TERMINAL" -e "\$SETUP_WRAPPER"
                ;;
            *)
                "\$TERMINAL" -e "\$SETUP_WRAPPER"
                ;;
        esac
    else
        # Fallback to YAD dialog if available and no terminal is found
        if command -v yad >/dev/null 2>&1; then
            yad --text="No terminal emulator found. Please run the interactive setup manually:\n\n<tt>\$HOME/.local/bin/interactive-userinstall</tt>" \
                --title="DINA Setup" --button="Close:0"
        else
            # Last resort: create a .desktop file for manual setup
            mkdir -p "\$HOME/.local/share/applications"
            cat > "\$HOME/.local/share/applications/dina-setup.desktop" << DESKTOPEOF
[Desktop Entry]
Type=Application
Name=DINA Setup
Comment=Configure DINA Window Manager
Exec=\$HOME/.local/bin/interactive-userinstall
Terminal=true
Categories=Settings;
DESKTOPEOF
            # Notify the user
            if command -v notify-send >/dev/null 2>&1; then
                notify-send -a "DINA Setup" "Please run DINA Setup manually" \
                           "Look for DINA Setup in your applications menu."
            fi
        fi
    fi

    # Create flag file to indicate first run is complete
    touch "\$HOME/.config/DINA/setup-complete"

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
if [ -f "\$HOME/.config/autostart/dina-first-run.desktop" ]; then
    rm "\$HOME/.config/autostart/dina-first-run.desktop"
fi

exit 0
EOF
    chmod +x "packaging/debian/dina-first-run.sh"
fi

# Copy desktop files
cp "packaging/debian/dina.desktop" "$PACKAGE_DIR/usr/share/xsessions/"
cp "packaging/debian/dina-first-run.desktop" "$PACKAGE_DIR/etc/xdg/autostart/"
cp "packaging/debian/dina-first-run.sh" "$PACKAGE_DIR/usr/share/DINA/"

# Copy sxhkdrc
cp "sxhkd/sxhkdrc" "$PACKAGE_DIR/etc/skel/.config/sxhkd/"

# Set permissions
echo "Setting file permissions..."
chmod 755 "$PACKAGE_DIR/usr/bin/DINA"
chmod 644 "$PACKAGE_DIR/usr/share/man/man1/DINA.1"
chmod 755 "$PACKAGE_DIR/usr/share/DINA/dina-first-run.sh"
chmod 644 "$PACKAGE_DIR/usr/share/xsessions/dina.desktop"
chmod 644 "$PACKAGE_DIR/etc/xdg/autostart/dina-first-run.desktop"
chmod 644 "$PACKAGE_DIR/etc/skel/.config/sxhkd/sxhkdrc"

# Create md5sums file
echo "Generating checksums..."
cd "$PACKAGE_DIR"
find usr etc -type f -exec md5sum {} \; > DEBIAN/md5sums
cd - > /dev/null

# Build the package
echo "Building the package..."
dpkg-deb --build --root-owner-group "$PACKAGE_DIR" || {
    echo "dpkg-deb failed, trying alternative approach..."
    cd "$PACKAGE_DIR"
    tar -czf ../data.tar.gz ./usr ./etc
    cd DEBIAN
    tar -czf ../../control.tar.gz ./*
    cd ../..
    echo "2.0" > debian-binary
    ar r dina_1.0-1_amd64.deb debian-binary control.tar.gz data.tar.gz
    rm debian-binary control.tar.gz data.tar.gz
}

# Rename package to proper format
PACKAGE_FILE="dina_1.0-1_amd64.deb"
if [ -f "$PACKAGE_DIR.deb" ]; then
    mv "$PACKAGE_DIR.deb" "$BUILD_DIR/$PACKAGE_FILE"
elif [ ! -f "$BUILD_DIR/$PACKAGE_FILE" ]; then
    echo "Creating package manually..."
    cd "$BUILD_DIR"
    if [ -f "dina_1.0-1_amd64.deb" ]; then
        # Package already exists at the right location
        echo "Package already exists at the right location."
    else
        echo "Error: Failed to create package."
        exit 1
    fi
fi

echo "=== Package built successfully: $BUILD_DIR/$PACKAGE_FILE ==="
echo "Package size: $(du -h "$BUILD_DIR/$PACKAGE_FILE" | cut -f1)"