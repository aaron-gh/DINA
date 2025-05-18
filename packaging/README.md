# DINA Packaging

This directory contains packaging files for distributing DINA in various formats.

## Available Packaging Options

### Debian/Ubuntu (.deb)

Located in the `debian/` directory, these files allow building a Debian package for DINA.

**Features of the Debian package:**
- Installs the DINA binary system-wide
- Includes all scripts in `/usr/share/DINA/scripts/`
- Installs skeleton sxhkd config in `/etc/skel/`
- Provides terminal-based first-run interactive setup for each user
- Handles dependencies automatically
- Registers DINA as an XSession option

For detailed instructions, see the [Debian packaging README](debian/README.md).

### Building the Package

Two methods are provided for building the Debian package:

1. **Using the build script (recommended):**
   ```sh
   cd packaging
   ./build.sh
   ```
   The resulting .deb package will be in `packaging/build/`.

2. **Using standard Debian tools:**
   ```sh
   cd packaging/debian
   dpkg-buildpackage -us -uc -b
   ```

### Demo Package Structure

A demonstration of the package structure is available in `packaging/temp-pkg-demo/`.
This is provided as a reference for package maintainers.

## Important Note

These packaging methods are provided as alternatives to the recommended source-based installation. For the best experience and full customization, building from source is still recommended as described in the main README.