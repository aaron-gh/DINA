#!/bin/bash
#
# Build DINA Debian package using Docker
#
# This script builds a .deb package for DINA in a Docker container,
# making it possible to build on any system with Docker installed

set -e

# Check if Docker is installed and the user has permissions
if ! command -v docker &> /dev/null; then
    echo "Error: Docker is required but not installed."
    echo "Please install Docker first."
    exit 1
fi

# Check if Docker daemon is running
if ! docker info &> /dev/null; then
    echo "Error: Docker daemon is not running or you don't have permissions."
    echo "Try running the script with sudo, or make sure the Docker service is running."
    echo "You can also add your user to the docker group with: sudo usermod -aG docker \$USER"
    echo "After adding your user to the docker group, you'll need to log out and back in."
    exit 1
fi

# Directory where the script is located
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_DIR="$(dirname "$SCRIPT_DIR")"
OUTPUT_DIR="$SCRIPT_DIR/build"

# Create output directory
mkdir -p "$OUTPUT_DIR"

echo "=== Building DINA Debian package using Docker ==="
echo "Repository: $REPO_DIR"
echo "Output directory: $OUTPUT_DIR"
echo ""

# Build the Docker image
echo "Building Docker image..."
docker build -t dina-package-builder -f "$SCRIPT_DIR/Dockerfile" "$REPO_DIR"

# Run the container to build the package
echo "Building package in container..."
docker run --rm -v "$OUTPUT_DIR:/output" dina-package-builder

# Check if the package was built successfully
if [ "$(ls -A "$OUTPUT_DIR"/*.deb 2>/dev/null)" ]; then
    echo ""
    echo "Package built successfully!"
    echo "Package file(s):"
    ls -lh "$OUTPUT_DIR"/*.deb
    echo ""
    echo "To install on a Debian-based system:"
    echo "  sudo dpkg -i $OUTPUT_DIR/dina_*.deb"
    echo "  sudo apt-get install -f  # To install any missing dependencies"
else
    echo ""
    echo "Error: Failed to build the package."
    echo "Check the Docker build output for errors."
    exit 1
fi