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

### Quick Installation (Recommended)

Use the provided installation scripts for automatic setup:

**Linux/macOS:**
```bash
./install-bruce-sideload.sh /path/to/sdcard
```

**Windows:**
```cmd
install-bruce-sideload.bat E:
```
(Replace `E:` with your SD card drive letter)

The script will:
- ✅ Verify all required files exist
- ✅ Create the proper directory structure
- ✅ Copy firmware files to the correct location
- ✅ Create a helpful README on the SD card
- ✅ Provide clear next-step instructions

### Manual Installation

If you prefer manual installation:

#### Step 1: Prepare Your SD Card

1. Format your microSD card as FAT32 (if not already formatted)
2. Insert the SD card into your computer

#### Step 2: Create Firmware Directory

Create the following directory structure on your SD card:
```
SD:/
├── firmware/
│   └── ultimatum/
│       ├── firmware.bin
│       ├── manifest.json
│       └── default_16MB.csv
```

#### Step 3: Copy Firmware Files

1. Copy the built `firmware.bin` to `SD:/firmware/ultimatum/`
2. Copy `bruce_manifest.json` from the repository root to `SD:/firmware/ultimatum/manifest.json`
3. Copy `default_16MB.csv` from the repository to `SD:/firmware/ultimatum/`

#### Step 4: Insert SD Card and Boot

1. Safely eject the SD card from your computer
2. Insert the SD card into your M5Stack Cardputer
3. Power on or restart your device with Bruce Firmware

#### Step 5: Load via Bruce Firmware Menu

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
│       └── default_16MB.csv      # Partition table (optional but recommended)
└── data/                         # Your data files (used by Ultimatum firmware)
```

## Manifest File Format

The `manifest.json` file contains metadata about the firmware:

```json
{
  "name": "Ultimatum Cardputer",
  "version": "1.0.0",
  "description": "Advanced firmware for M5Stack Cardputer",
  "device": "M5Stack Cardputer",
  "binary": "firmware.bin",
  "partition": "default_16MB.csv",
  "flash_size": "16MB"
}
```

## Features Available in Sideload Mode

When running as a sideload through Bruce Firmware, you'll have access to:

- ✅ Full keyboard support
- ✅ LCD display control
- ✅ SD card access for data storage
- ✅ USB serial debugging
- ✅ All M5Unified library features
- ✅ Optimized partition scheme for 16MB flash
- ✅ OTA update capability

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

### Installation Script Errors

**Linux/macOS:**
- Ensure the script is executable: `chmod +x install-bruce-sideload.sh`
- Check SD card mount point: `df -h` or `mount | grep SD`
- Verify you have write permissions to the SD card

**Windows:**
- Run Command Prompt as Administrator if permission errors occur
- Verify the drive letter is correct in "This PC"
- Ensure the SD card is not write-protected

## Returning to Bruce Firmware

To exit Ultimatum firmware and return to Bruce Firmware:

1. Power off your M5Stack Cardputer
2. Power it back on normally
3. Bruce Firmware will load as the base firmware
4. You can sideload Ultimatum again from the menu when needed

## Verification Steps

After installation, verify everything is correct:

1. **Check Directory Structure:**
   ```bash
   # On your computer (with SD card mounted)
   ls -la /path/to/sdcard/firmware/ultimatum/
   ```
   Should show: `firmware.bin`, `manifest.json`, `default_16MB.csv`, `README.txt`

2. **Check File Sizes:**
   - `firmware.bin`: Should be several MB (typically 1-3 MB)
   - `manifest.json`: Small file (~1 KB)
   - `default_16MB.csv`: Small file (<1 KB)

3. **Verify JSON:**
   ```bash
   cat /path/to/sdcard/firmware/ultimatum/manifest.json
   ```
   Should be valid JSON with proper structure

## Further Resources

- [Bruce Firmware GitHub](https://github.com/pr3y/Bruce) - Multi-tool firmware for M5Stack devices
- [UltimatumCardputerKey GitHub](https://github.com/Crashkeyz/UltimatumCardputerKey)
- [M5Stack Cardputer Official Docs](https://docs.m5stack.com/en/core/Cardputer)
- [Quick Start Guide](QUICK_START_BRUCE.md) - Fast 3-step installation

## Support

If you encounter issues:

1. Check the [Issues](https://github.com/Crashkeyz/UltimatumCardputerKey/issues) page
2. Review Bruce Firmware documentation for sideload requirements
3. Open a new issue with details about your problem

## License

This firmware is provided under the same license as the main project. See LICENSE file for details.
