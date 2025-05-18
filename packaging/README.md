# DINA Packaging System

This directory contains tools and configurations for building Debian packages for DINA (DINA Is Not Aesthetics), an accessibility-focused window manager.

## Overview

The DINA packaging system provides the following options for building .deb packages:

1. **Docker-based build** - Build a .deb package on any system with Docker, regardless of the host OS
2. **Direct Debian build** - Build a .deb package directly on a Debian-based system
3. **Manual package creation** - Create a .deb package manually when standard tools fail

## Docker-Based Build (Recommended)

The Docker-based build method allows you to build DINA packages on any system that has Docker installed, including non-Debian systems like Arch Linux, Fedora, or macOS.

### Prerequisites

- Docker installed and running
- User has permissions to run Docker commands

### Build Steps

1. From the repository root directory, run:
   ```sh
   bash packaging/build-with-docker.sh
   ```

2. The script will:
   - Build a Docker image with all necessary dependencies
   - Compile DINA inside the container
   - Create the .deb package
   - Copy the package to `packaging/build/`

## Direct Debian Build

If you're already running a Debian-based system (Debian, Ubuntu, etc.), you can build the package directly.

### Prerequisites

On a Debian-based system, install the required build tools:
```sh
sudo apt-get install build-essential debhelper devscripts dpkg-dev
```

### Build Steps

1. From the repository root directory, run:
   ```sh
   bash packaging/build-package.sh
   ```

2. The script will:
   - Compile DINA if needed
   - Create the package structure
   - Build the .deb package
   - Place the package in `packaging/build/`

## Package Structure

The resulting Debian package includes:

- DINA binary at `/usr/bin/DINA`
- Man page at `/usr/share/man/man1/DINA.1`
- Accessibility scripts at `/usr/share/DINA/scripts/`
- sxhkd configuration at `/etc/skel/.config/sxhkd/sxhkdrc`
- First-run setup script at `/usr/share/DINA/dina-first-run.sh`
- XSession entry at `/usr/share/xsessions/dina.desktop`
- Autostart entry at `/etc/xdg/autostart/dina-first-run.desktop`

## First-Run Experience

The package includes an automatic first-run setup that:

1. Detects when a user logs into DINA for the first time
2. Launches a terminal-based interactive setup
3. Configures user preferences for applications
4. Copies the scripts to the user's local bin directory
5. Creates properly configured keybindings

## Installation Instructions

After building the package, install it with:

```sh
sudo dpkg -i packaging/build/dina_1.0-1_amd64.deb
sudo apt-get install -f  # To resolve any dependencies
```

## Additional Resources

- For detailed Debian packaging information, see the documentation in `packaging/debian/README.md`
- The Docker build system is defined in `packaging/Dockerfile`