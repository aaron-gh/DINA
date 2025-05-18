# DINA Debian Package

This directory contains files needed to build a Debian package for DINA. This packaging method is provided as an alternative to the recommended source-based installation.

## Overview

The DINA Debian package:
1. Installs the DINA binary system-wide
2. Installs user scripts and configuration files
3. Automatically runs the interactive setup on first launch for each user

## Building the Package

### Prerequisites

You need the following packages installed:
```
sudo apt-get install debhelper dh-make devscripts build-essential
```

### Build Steps

From the repository root:
```
cd packaging/debian
dpkg-buildpackage -us -uc -b
```

The resulting .deb file will be created in the parent directory.

## Package Contents

The Debian package installs:
- DINA binary to `/usr/bin/DINA`
- Man page to `/usr/share/man/man1/DINA.1`
- Scripts to `/usr/share/DINA/scripts/`
- sxhkd configuration to `/etc/skel/.config/sxhkd/`
- First-run script for the interactive setup

## First-Run Interactive Setup

The package adds a first-run script that:
1. Checks if DINA has been configured for the current user
2. If not, runs the interactive-userinstall script
3. Sets up user-specific configuration files

This allows automated setup while maintaining the accessibility benefits of DINA.

## Important Note

This packaging method is provided as an alternative to the recommended source-based installation. For the best experience and full customization, building from source is still recommended.