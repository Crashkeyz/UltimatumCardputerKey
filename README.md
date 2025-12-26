# UltimatumCardputerKey

Firmware for M5Stack Cardputer ADV (ESP32-S3) with comprehensive SD card storage support

## Hardware
- **Device**: M5Stack Cardputer ADV
- **MCU**: ESP32-S3-WROOM-1-N16R8
- **Flash**: 16MB
- **PSRAM**: 8MB
- **Display**: 1.14" LCD (240x135)
- **Keyboard**: Full QWERTY matrix
- **SD Card**: MicroSD slot (supports SDHC/SDXC, FAT32 recommended)

## Features
- Full M5Unified library integration
- USB CDC serial debugging
- Keyboard input handling
- LCD display control
- 16MB Flash + 8MB PSRAM support
- **Comprehensive SD card storage system**
- **Automatic logging to SD card**
- **Configuration management from SD card**
- **Graceful fallback when SD card absent**

## SD Card Storage Features

### Automatic Directory Structure
The firmware automatically creates the following directory structure on the SD card:
```
/SDCARD/
├── config/
│   └── settings.json      # Device configuration
├── data/
│   ├── captures/          # For captured data
│   └── recordings/        # For recordings
├── logs/
│   └── system.log         # System event logs
└── downloads/             # Downloaded files
```

### Storage Manager API
The firmware includes a comprehensive `StorageManager` class with:
- SD card initialization and status checking
- File read/write/delete operations
- Directory creation and management
- Free space monitoring
- Robust error handling

### Logging System
Built-in logging system with:
- Timestamped log entries
- Multiple log levels (INFO, WARNING, ERROR)
- Dual output (Serial console + SD card)
- Automatic log rotation when size limit reached
- Console output can be toggled

### SD Card Requirements
- **Capacity**: 4GB - 128GB recommended
- **Format**: FAT32 (required for cards up to 32GB, exFAT for larger)
- **Speed Class**: Class 10 or higher recommended for better performance
- **Quality**: Use reputable brand SD cards to avoid corruption

## Build & Flash

### Using PlatformIO CLI
```bash
# Install PlatformIO
pip install platformio

# Build firmware
pio run

# Flash to device (connect via USB-C)
pio run --target upload

# Monitor serial output
pio device monitor
```

### Using VS Code + PlatformIO Extension
1. Install PlatformIO IDE extension
2. Open project folder
3. Click "Build" (✓) in toolbar
4. Click "Upload" (→) to flash
5. Click "Serial Monitor" (🔌) to view output

## Development

### Project Structure
- `src/main.cpp` - Main application code with SD card integration
- `drivers/cardputer_adv_driver.cpp` - Custom hardware drivers
- `lib/storage_manager/` - SD card storage management library
  - `storage_manager.h/cpp` - Core storage operations
  - `sd_logger.h/cpp` - Logging system
- `lib/utilities.cpp` - Utility functions
- `config/` - Configuration files

### Using SD Card Storage in Your Code

```cpp
#include "lib/storage_manager/storage_manager.h"
#include "lib/storage_manager/sd_logger.h"

// Initialize storage
StorageManager storage;
if (storage.begin()) {
    // Create directory
    storage.createDir("/mydata");
    
    // Write file
    storage.writeFile("/mydata/test.txt", "Hello World!");
    
    // Read file
    String data;
    storage.readFile("/mydata/test.txt", data);
    
    // Check space
    uint64_t free = storage.getFreeSpace();
    
    // List directory
    storage.listDir("/mydata");
}

// Initialize logger
SDLogger logger;
logger.begin(&storage);
logger.info("System started");
logger.error("An error occurred");
```

## Interactive Commands

When the firmware is running, you can use keyboard shortcuts:
- **'w'** - Write test file to SD card
- **'r'** - Read test file from SD card
- **'l'** - List files in /data directory
- **'s'** - Show SD card status on display

## Display Information

The display shows:
- Device name and title
- SD card status (OK / MISSING)
- Free space and total space
- SD card type (SDHC/SDSC/MMC)
- Last key pressed
- File operation status

## Troubleshooting

### SD Card Not Detected
1. Ensure SD card is properly inserted
2. Check SD card is formatted as FAT32
3. Try a different SD card
4. Check serial monitor for error messages

### File Operations Failing
1. Check SD card has free space
2. Verify SD card is not write-protected
3. Ensure directory paths start with '/'
4. Check serial monitor for detailed error messages

### System Won't Boot
1. Remove SD card and try again
2. Check USB cable and power supply
3. Verify firmware was flashed correctly
4. Monitor serial output for error messages

## Documentation

For detailed SD card usage information, see:
- [SD Card Setup Guide](docs/SD_CARD_GUIDE.md) - Comprehensive SD card documentation
- [API Reference](docs/SD_CARD_GUIDE.md#api-reference) - Storage manager API details

## License
See LICENSE file for details.

