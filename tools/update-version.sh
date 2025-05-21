#!/bin/bash
#
# Update DINA version number in all necessary files
#

# Exit on any error
set -e

if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <new_version>"
    echo "Example: $0 1.0.2"
    exit 1
fi

NEW_VERSION="$1"
TOOLS_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_DIR="$(dirname "$TOOLS_DIR")"

# Update version in config.mk
CONFIG_MK="$REPO_DIR/config.mk"
echo "Updating version in $CONFIG_MK..."
sed -i "s/^VERSION = .*/VERSION = $NEW_VERSION/" "$CONFIG_MK"

# Update version in debian/changelog
CHANGELOG="$REPO_DIR/packaging/debian/changelog"
echo "Updating version in $CHANGELOG..."

# Get current date in Debian changelog format
DATE=$(date -R)

# Create new changelog entry
NEW_ENTRY="dina ($NEW_VERSION-1) unstable; urgency=medium

  * Update to version $NEW_VERSION
  * See git log for detailed changes

 -- Aaron Hewitt <aaron.graham.hewitt@gmail.com>  $DATE
"

# Prepend new entry to changelog
echo "$NEW_ENTRY" > "${CHANGELOG}.new"
cat "$CHANGELOG" >> "${CHANGELOG}.new"
mv "${CHANGELOG}.new" "$CHANGELOG"

echo "Version updated to $NEW_VERSION in all files"
echo "Remember to commit these changes and create a git tag:"
echo "git add config.mk packaging/debian/changelog"
echo "git commit -m \"release: Bump version to $NEW_VERSION\""
echo "git tag v$NEW_VERSION"
echo "git push && git push --tags"