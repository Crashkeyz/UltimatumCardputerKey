# Bruce Firmware Sideload Guide

This guide explains how to use the Ultimatum Cardputer firmware as a sideloadable application through Bruce Firmware.

## What is Bruce Firmware?

Bruce Firmware is a popular multi-tool firmware for M5Stack Cardputer that provides various utilities and supports sideloading other firmware applications. It allows you to run multiple firmware applications without completely replacing the base firmware.

## Prerequisites

- M5Stack Cardputer with Bruce Firmware installed
- microSD card (formatted as FAT32)
- USB-C cable for connecting to your computer
- Built firmware binary files (see Building section)

## Building the Firmware for Sideload

### Option 1: Download Pre-built Binary

If available, download the latest release from the [Releases](https://github.com/Crashkeyz/UltimatumCardputerKey/releases) page.

### Option 2: Build from Source

1. Install PlatformIO:
   ```bash
   pip install platformio
   ```

2. Clone the repository:
   ```bash
   git clone https://github.com/Crashkeyz/UltimatumCardputerKey.git
   cd UltimatumCardputerKey
   ```

3. Build the firmware:
   ```bash
   pio run -e m5stack-cardputer
   ```

4. The firmware binary will be created at:
   ```
   .pio/build/m5stack-cardputer/firmware.bin
   ```

## Installation on Bruce Firmware

### Step 1: Prepare Your SD Card

1. Format your microSD card as FAT32 (if not already formatted)
2. Insert the SD card into your computer

### Step 2: Create Firmware Directory

Create the following directory structure on your SD card:
```
SD:/
├── firmware/
│   └── ultimatum/
│       ├── firmware.bin
│       └── manifest.json
```

### Step 3: Copy Firmware Files

1. Copy the built `firmware.bin` to `SD:/firmware/ultimatum/`
2. Copy `bruce_manifest.json` from the repository root to `SD:/firmware/ultimatum/manifest.json`

### Step 4: Copy Partition Table (Optional but Recommended)

For proper memory management, also copy the partition table:
```
SD:/
└── firmware/
    └── ultimatum/
        ├── firmware.bin
        ├── manifest.json
        └── default_16MB.csv
```

Copy `default_16MB.csv` from the repository to `SD:/firmware/ultimatum/`

### Step 5: Insert SD Card and Boot

1. Safely eject the SD card from your computer
2. Insert the SD card into your M5Stack Cardputer
3. Power on or restart your device with Bruce Firmware

### Step 6: Load via Bruce Firmware Menu

1. Navigate to Bruce Firmware's menu system
2. Look for "Sideload" or "Apps" option
3. Select "Ultimatum Cardputer" from the list
4. Bruce Firmware will load and run the Ultimatum firmware

## Directory Structure Reference

Complete directory structure on SD card:
```
SD:/
├── firmware/
│   └── ultimatum/
│       ├── firmware.bin          # Main firmware binary
│       ├── manifest.json         # Metadata for Bruce Firmware
│       └── default_16MB.csv      # Partition table (optional)
└── data/                         # Your data files (used by Ultimatum firmware)
```

## Manifest File Format

The `manifest.json` file contains metadata about the firmware:

```json
{
  "name": "Ultimatum Cardputer",
  "version": "1.0.0",
  "description": "Advanced firmware for M5Stack Cardputer",
  "binary": "firmware.bin",
  "flash_size": "16MB",
  "requirements": {
    "flash": "16MB",
    "psram": "8MB"
  }
}
```

## Features Available in Sideload Mode

When running as a sideload through Bruce Firmware, you'll have access to:

- ✅ Full keyboard support
- ✅ LCD display control
- ✅ SD card access for data storage
- ✅ USB serial debugging
- ✅ All M5Unified library features

## Troubleshooting

### Firmware Not Appearing in Bruce Menu

1. Check that files are in the correct directory: `SD:/firmware/ultimatum/`
2. Verify `manifest.json` is valid JSON (use a JSON validator)
3. Ensure `firmware.bin` exists and is not corrupted
4. Restart Bruce Firmware after copying files

### Device Not Booting After Sideload

1. Power off the device completely
2. Remove and reinsert the SD card
3. Power on and try loading again
4. If issues persist, check Bruce Firmware documentation for compatibility

### SD Card Not Detected

1. Ensure SD card is formatted as FAT32
2. Try a different SD card (some cards may be incompatible)
3. Check that SD card is properly inserted
4. Some SD cards work better at lower speeds

### Memory Issues

1. Make sure you're using the provided `default_16MB.csv` partition table
2. Use an SD card for storing data instead of internal flash
3. Verify your device has 16MB flash and 8MB PSRAM

## Returning to Bruce Firmware

To exit Ultimatum firmware and return to Bruce Firmware:

1. Power off your M5Stack Cardputer
2. Power it back on normally
3. Bruce Firmware will load as the base firmware
4. You can sideload Ultimatum again from the menu when needed

## Advanced: Automated Installation Script

For advanced users, you can create a script to automate the SD card setup:

```bash
#!/bin/bash
# setup-bruce-sideload.sh

SD_PATH="/path/to/your/sdcard"
FIRMWARE_DIR="$SD_PATH/firmware/ultimatum"

# Create directory structure
mkdir -p "$FIRMWARE_DIR"

# Copy firmware files
cp .pio/build/m5stack-cardputer/firmware.bin "$FIRMWARE_DIR/"
cp bruce_manifest.json "$FIRMWARE_DIR/manifest.json"
cp default_16MB.csv "$FIRMWARE_DIR/"

echo "Firmware copied to SD card successfully!"
echo "Safely eject SD card and insert into your Cardputer"
```

Make it executable and run:
```bash
chmod +x setup-bruce-sideload.sh
./setup-bruce-sideload.sh
```

## Further Resources

- [Bruce Firmware Documentation](https://github.com/pr3y/Bruce) (example, adjust to actual repo)
- [UltimatumCardputerKey GitHub](https://github.com/Crashkeyz/UltimatumCardputerKey)
- [M5Stack Cardputer Official Docs](https://docs.m5stack.com/en/core/Cardputer)

## Support

If you encounter issues:

1. Check the [Issues](https://github.com/Crashkeyz/UltimatumCardputerKey/issues) page
2. Review Bruce Firmware documentation for sideload requirements
3. Open a new issue with details about your problem

## License

This firmware is provided under the same license as the main project. See LICENSE file for details.
