#!/bin/bash
#
# Build script for DINA Debian package
#
# This script automates the process of building a .deb package for DINA

set -e

# Check for required tools
if ! command -v dpkg-deb &> /dev/null; then
    echo "Error: dpkg-deb is required but not installed."
    echo "Please install it with: sudo apt-get install dpkg-dev"
    exit 1
fi

# Set up variables
PACKAGE_NAME="dina"
VERSION="1.0"
ARCH=$(dpkg --print-architecture 2>/dev/null || echo "amd64")
PACKAGE_DIR="$PACKAGE_NAME-$VERSION-$ARCH"
BUILD_DIR="./build"

# Clean any previous build
rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR/$PACKAGE_DIR"

echo "=== Building DINA $VERSION package for $ARCH ==="

# Build DINA binary first
echo "Building DINA binary..."
cd ..
make clean
make
cd packaging

# Create package directory structure
echo "Creating package directory structure..."
mkdir -p "$BUILD_DIR/$PACKAGE_DIR/DEBIAN"
mkdir -p "$BUILD_DIR/$PACKAGE_DIR/usr/bin"
mkdir -p "$BUILD_DIR/$PACKAGE_DIR/usr/share/man/man1"
mkdir -p "$BUILD_DIR/$PACKAGE_DIR/usr/share/DINA/scripts"
mkdir -p "$BUILD_DIR/$PACKAGE_DIR/usr/share/xsessions"
mkdir -p "$BUILD_DIR/$PACKAGE_DIR/etc/skel/.config/sxhkd"
mkdir -p "$BUILD_DIR/$PACKAGE_DIR/etc/xdg/autostart"

# Copy control file
echo "Preparing package metadata..."
cat > "$BUILD_DIR/$PACKAGE_DIR/DEBIAN/control" << EOF
Package: dina
Version: $VERSION
Section: x11
Priority: optional
Architecture: $ARCH
Depends: libx11-6, libxinerama1, 
 sxhkd, speech-dispatcher, sox, dialog, orca, yad, acpi
Recommends: xterm | x-terminal-emulator
Maintainer: Aaron Hewitt <aaron.graham.hewitt@gmail.com>
Description: Accessibility-first window manager for blind users
 DINA (DINA Is Not Aesthetics) is an accessibility-first fork of dwm
 window manager tailored specifically for blind users and screen reader
 workflows.
 .
 Features include monocle layout, audio feedback, and Orca autostart.
EOF

# Copy package files
echo "Copying files into package..."
cp ../DINA "$BUILD_DIR/$PACKAGE_DIR/usr/bin/"
cp ../DINA.1 "$BUILD_DIR/$PACKAGE_DIR/usr/share/man/man1/"
cp -r ../scripts/* "$BUILD_DIR/$PACKAGE_DIR/usr/share/DINA/scripts/"
cp debian/dina.desktop "$BUILD_DIR/$PACKAGE_DIR/usr/share/xsessions/"
cp debian/dina-first-run.desktop "$BUILD_DIR/$PACKAGE_DIR/etc/xdg/autostart/"
cp debian/dina-first-run.sh "$BUILD_DIR/$PACKAGE_DIR/usr/share/DINA/"
cp ../sxhkd/sxhkdrc "$BUILD_DIR/$PACKAGE_DIR/etc/skel/.config/sxhkd/"

# Set permissions
echo "Setting file permissions..."
chmod 755 "$BUILD_DIR/$PACKAGE_DIR/usr/bin/DINA"
chmod 644 "$BUILD_DIR/$PACKAGE_DIR/usr/share/man/man1/DINA.1"
chmod 755 "$BUILD_DIR/$PACKAGE_DIR/usr/share/DINA/scripts/"*
chmod 755 "$BUILD_DIR/$PACKAGE_DIR/usr/share/DINA/dina-first-run.sh"
chmod 644 "$BUILD_DIR/$PACKAGE_DIR/usr/share/xsessions/dina.desktop"
chmod 644 "$BUILD_DIR/$PACKAGE_DIR/etc/xdg/autostart/dina-first-run.desktop"
chmod 644 "$BUILD_DIR/$PACKAGE_DIR/etc/skel/.config/sxhkd/sxhkdrc"

# Create md5sums file
echo "Generating checksums..."
cd "$BUILD_DIR/$PACKAGE_DIR"
find usr etc -type f -exec md5sum {} \; > DEBIAN/md5sums
cd ../../..

# Build the package
echo "Building the package..."
dpkg-deb --build --root-owner-group "$BUILD_DIR/$PACKAGE_DIR"

# Rename package to proper format
PACKAGE_FILE="$PACKAGE_NAME-$VERSION-$ARCH.deb"
mv "$BUILD_DIR/$PACKAGE_DIR.deb" "$BUILD_DIR/$PACKAGE_FILE"

echo "=== Package built successfully: $BUILD_DIR/$PACKAGE_FILE ==="
echo "Package size: $(du -h "$BUILD_DIR/$PACKAGE_FILE" | cut -f1)"
echo ""
echo "To install:"
echo "sudo dpkg -i $BUILD_DIR/$PACKAGE_FILE"
echo "sudo apt-get install -f # To install any missing dependencies"