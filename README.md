# UltimatumCardputerKey

Firmware for M5Stack Cardputer ADV (ESP32-S3)

## Which Branch to Use?

**Use the `copilot/optimize-flashing-to-sd-card` branch for the latest stable version with SD card support and optimized memory usage.**

This branch includes:
- ✅ Optimized partition scheme for 16MB flash
- ✅ SD card support for external data storage
- ✅ Reduced flash memory usage
- ✅ OTA update capability

## Hardware
- **Device**: M5Stack Cardputer ADV
- **MCU**: ESP32-S3-WROOM-1-N16R8
- **Flash**: 16MB
- **PSRAM**: 8MB
- **Display**: 1.14" LCD (240x135)
- **Keyboard**: Full QWERTY matrix
- **SD Card**: microSD card slot (optional but recommended)

## Memory Optimization

### Flash Memory Management
This firmware uses an optimized partition scheme that allocates:
- **App partitions**: 2 × 3.75MB (for OTA updates)
- **SPIFFS**: ~8.5MB (for file storage)
- **NVS**: 20KB (for settings)
- **Total**: Efficient use of 16MB flash

### SD Card Support
To avoid running out of flash memory:
1. **Insert a microSD card** into your Cardputer's SD slot
2. The firmware will automatically detect and mount it
3. Store large files, assets, and data on the SD card
4. Flash memory is reserved for firmware only

**Benefits of using SD card:**
- Prevents "out of memory" errors during flashing
- Store unlimited data externally
- Easy to swap and backup data
- Faster development iterations

## Build & Flash

**📖 For detailed Evil Portal and Firmware Sideload instructions, see [docs/EVIL_PORTAL_GUIDE.md](docs/EVIL_PORTAL_GUIDE.md)**

**📖 For detailed SD card setup instructions, see [docs/SD_CARD_SETUP.md](docs/SD_CARD_SETUP.md)**

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
- Main code: `src/main.cpp`
- Drivers: `drivers/cardputer_adv_driver.cpp`
- Utilities: `lib/utilities.cpp`
- Evil Portal: `lib/evil_portal.cpp`
- Firmware Sideload: `lib/firmware_sideload.cpp`

### 🎮 Quick Start - Keyboard Controls

Once the firmware is running on your Cardputer:

| Key | Action |
|-----|--------|
| **P** | Start Evil Portal (WiFi captive portal for web access) |
| **S** | Stop Evil Portal |
| **F** | List firmware files on SD card |

### 🌐 Evil Portal - Wireless Firmware Access

The Evil Portal creates a WiFi access point that allows you to:
- Upload firmware wirelessly
- Browse files on SD card
- Manage the device via web interface

**To use:**
1. Press `P` on the keyboard
2. Connect to WiFi: `Cardputer-Portal`
3. Browser will open portal automatically (or go to 192.168.4.1)
4. Upload firmware or manage files
5. Press `S` to stop portal when done

**See [docs/EVIL_PORTAL_GUIDE.md](docs/EVIL_PORTAL_GUIDE.md) for complete documentation.**

### 💾 SD Card Firmware Sideload

Automatically update firmware from SD card:
1. Copy firmware.bin to SD card as `/firmware/autoload.bin`
2. Insert SD card and restart Cardputer
3. Firmware auto-updates on boot
4. File renamed to `.old` after successful update

**See [docs/EVIL_PORTAL_GUIDE.md](docs/EVIL_PORTAL_GUIDE.md) for complete documentation.**

### Using SD Card for Data Storage
The firmware automatically initializes the SD card on boot. You can use it to:
- Store configuration files
- Save logs and debug data
- Store images, fonts, and other assets
- Cache downloaded data
- **🆕 Sideload firmware updates (place as /firmware/autoload.bin)**

Example code to write to SD card:
```cpp
#include <SD.h>

// Write to SD card
File file = SD.open("/data/myfile.txt", FILE_WRITE);
if (file) {
    file.println("Hello from Cardputer!");
    file.close();
}

// Read from SD card
file = SD.open("/data/myfile.txt");
if (file) {
    while (file.available()) {
        Serial.write(file.read());
    }
    file.close();
}
```

## Features
- Full M5Unified library integration
- USB CDC serial debugging
- Keyboard input handling
- LCD display control
- 16MB Flash + 8MB PSRAM support
- **SD card support for external storage**
- **Optimized partition scheme for memory efficiency**
- **🆕 Evil Portal (Captive Portal) for wireless firmware access**
- **🆕 SD Card firmware sideload (auto-update on boot)**
- **🆕 Web-based firmware upload and file management**

## Troubleshooting

### "Out of Memory" Error When Flashing
If you encounter memory errors during flashing:
1. Make sure you're using the `copilot/optimize-flashing-to-sd-card` branch
2. The custom partition scheme (`default_16MB.csv`) is now optimized for your device
3. Insert an SD card to store data externally instead of in flash
4. Clean and rebuild: `pio run --target clean && pio run`

### SD Card Not Detected
If the SD card isn't mounting:
1. Ensure the SD card is properly inserted
2. Try formatting the card as FAT32
3. Check the serial monitor for initialization messages
4. Some SD cards may need lower SPI speeds (already set to 25MHz)

### Partition Scheme Issues
The firmware uses a custom 16MB partition scheme:
- Located in: `default_16MB.csv`
- Configured in: `platformio.ini`
- Provides optimal space allocation for OTA updates and storage

## License
See LICENSE file for details.

