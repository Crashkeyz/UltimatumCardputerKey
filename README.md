# UltimatumCardputerKey

Firmware for M5Stack Cardputer ADV (ESP32-S3)

## Hardware
- **Device**: M5Stack Cardputer ADV
- **MCU**: ESP32-S3-WROOM-1-N16R8
- **Flash**: 16MB
- **PSRAM**: 8MB
- **Display**: 1.14" LCD (240x135)
- **Keyboard**: Full QWERTY matrix

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
- Main code: `src/main.cpp`
- Drivers: `drivers/cardputer_adv_driver.cpp`
- Utilities: `lib/utilities.cpp`

## Features
- Full M5Unified library integration
- USB CDC serial debugging
- Keyboard input handling
- LCD display control
- 16MB Flash + 8MB PSRAM support

## License
See LICENSE file for details.

