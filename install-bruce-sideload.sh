#!/bin/bash
# Installation script for Bruce Firmware sideload
# Usage: ./install-bruce-sideload.sh /path/to/sdcard

echo "======================================================================"
echo "  Ultimatum Cardputer - Bruce Firmware Sideload Installer"
echo "======================================================================"
echo ""

# Check if SD card path is provided
if [ -z "$1" ]; then
    echo "Usage: ./install-bruce-sideload.sh /path/to/sdcard"
    echo ""
    echo "Examples:"
    echo "  macOS:   ./install-bruce-sideload.sh /Volumes/SDCARD"
    echo "  Linux:   ./install-bruce-sideload.sh /media/user/SDCARD"
    echo "  Windows: Use install-bruce-sideload.bat instead"
    echo ""
    exit 1
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
    exit 1
fi

echo "✓ SD card found at: $SD_PATH"
echo ""

# Create firmware directory if it doesn't exist
echo "Creating directory structure..."
mkdir -p "$TARGET_DIR"

if [ ! -d "$TARGET_DIR" ]; then
    echo "❌ Error: Could not create directory: $TARGET_DIR"
    exit 1
fi

echo "✓ Directory created: $TARGET_DIR"
echo ""

# Check if firmware files exist locally
if [ ! -f ".pio/build/m5stack-cardputer/firmware.bin" ]; then
    echo "❌ Error: Firmware binary not found!"
    echo ""
    echo "Please build the firmware first:"
    echo "  pio run -e m5stack-cardputer"
    echo ""
    exit 1
fi

# Copy firmware files
echo "Copying firmware files..."

cp .pio/build/m5stack-cardputer/firmware.bin "$TARGET_DIR/" && \
cp bruce_manifest.json "$TARGET_DIR/manifest.json" && \
cp default_16MB.csv "$TARGET_DIR/"

if [ $? -eq 0 ]; then
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
    ls -lh "$TARGET_DIR"
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
else
    echo "❌ Error: Failed to copy files"
    exit 1
fi
