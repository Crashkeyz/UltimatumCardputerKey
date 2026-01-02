#!/bin/sh
# Installation script for Bruce Firmware sideload
# Usage: ./install-bruce-sideload.sh /path/to/sdcard

set -e  # Exit on error
set -u  # Exit on undefined variable

echo "======================================================================"
echo "  Ultimatum Cardputer - Bruce Firmware Sideload Installer"
echo "======================================================================"
echo ""

# Usage function
usage() {
    echo "Usage: $0 /path/to/sdcard"
    echo ""
    echo "Examples:"
    echo "  macOS:   $0 /Volumes/SDCARD"
    echo "  Linux:   $0 /media/user/SDCARD"
    echo "  Windows: Use install-bruce-sideload.bat instead"
    echo ""
    exit 1
}

# Check if SD card path is provided
if [ $# -eq 0 ]; then
    echo "❌ Error: No SD card path provided"
    echo ""
    usage
fi

SD_PATH="$1"
FIRMWARE_DIR="$SD_PATH/firmware"
TARGET_DIR="$FIRMWARE_DIR/ultimatum"

# Check if SD card path exists
if [ ! -d "$SD_PATH" ]; then
    echo "❌ Error: SD card path not found: $SD_PATH"
    echo ""
    echo "Please check that:"
    echo "  1. SD card is inserted and mounted"
    echo "  2. Path is correct (check with 'ls /Volumes/' on macOS or 'ls /media/' on Linux)"
    echo ""
    exit 1
fi

echo "✓ SD card found at: $SD_PATH"
echo ""

# Check if SD card is writable
if [ ! -w "$SD_PATH" ]; then
    echo "❌ Error: SD card is not writable: $SD_PATH"
    echo "Please check permissions or try running with appropriate privileges"
    exit 1
fi

# Create firmware directory if it doesn't exist
echo "Creating directory structure..."
mkdir -p "$TARGET_DIR" || {
    echo "❌ Error: Could not create directory: $TARGET_DIR"
    exit 1
}

echo "✓ Directory created: $TARGET_DIR"
echo ""

# Check if firmware files exist locally
if [ ! -f ".pio/build/m5stack-cardputer/firmware.bin" ]; then
    echo "❌ Error: Firmware binary not found!"
    echo ""
    echo "Please build the firmware first:"
    echo "  pio run -e m5stack-cardputer"
    echo ""
    echo "Or download a pre-built release from:"
    echo "  https://github.com/Crashkeyz/UltimatumCardputerKey/releases"
    echo ""
    exit 1
fi

if [ ! -f "bruce_manifest.json" ]; then
    echo "❌ Error: bruce_manifest.json not found!"
    echo "Please ensure you're running this script from the project root directory."
    exit 1
fi

if [ ! -f "default_16MB.csv" ]; then
    echo "❌ Error: default_16MB.csv not found!"
    echo "Please ensure you're running this script from the project root directory."
    exit 1
fi

# Copy firmware files
echo "Copying firmware files..."

cp .pio/build/m5stack-cardputer/firmware.bin "$TARGET_DIR/" || {
    echo "❌ Error: Failed to copy firmware.bin"
    exit 1
}

cp bruce_manifest.json "$TARGET_DIR/manifest.json" || {
    echo "❌ Error: Failed to copy manifest.json"
    exit 1
}

cp default_16MB.csv "$TARGET_DIR/" || {
    echo "❌ Error: Failed to copy default_16MB.csv"
    exit 1
}

# Create a README in the target directory
cat > "$TARGET_DIR/README.txt" << 'EOF'
Ultimatum Cardputer Firmware - Bruce Firmware Sideload Package
================================================================

This firmware can be loaded through Bruce Firmware's sideload menu.

To Use:
1. Ensure this folder is at: SD:/firmware/ultimatum/
2. Boot your M5Stack Cardputer with Bruce Firmware
3. Navigate to the sideload or apps menu
4. Select "Ultimatum Cardputer"
5. The firmware will load

To Return to Bruce Firmware:
- Simply restart your device

Files:
- firmware.bin: Main firmware binary
- manifest.json: Metadata for Bruce Firmware loader
- default_16MB.csv: Partition table for proper memory management
- README.txt: This file

For more information:
https://github.com/Crashkeyz/UltimatumCardputerKey/blob/main/docs/BRUCE_FIRMWARE_SIDELOAD.md

Version: 1.0.0
EOF

echo "✓ Files copied successfully!"
echo ""
echo "======================================================================"
echo "  Installation Complete!"
echo "======================================================================"
echo ""
echo "Files installed to: $TARGET_DIR"
echo ""
echo "Directory contents:"
ls -lh "$TARGET_DIR" 2>/dev/null || ls -l "$TARGET_DIR"
echo ""
echo "Next Steps:"
echo "  1. Safely eject your SD card from the computer"
echo "  2. Insert the SD card into your M5Stack Cardputer"
echo "  3. Power on or restart your device (should be running Bruce Firmware)"
echo "  4. Navigate to Bruce Firmware's sideload/apps menu"
echo "  5. Select 'Ultimatum Cardputer' from the list"
echo "  6. The firmware will load and run"
echo ""
echo "To return to Bruce Firmware, simply restart your device."
echo ""
echo "For detailed instructions and troubleshooting:"
echo "  https://github.com/Crashkeyz/UltimatumCardputerKey/blob/main/docs/BRUCE_FIRMWARE_SIDELOAD.md"
echo ""

exit 0
