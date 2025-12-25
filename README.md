# UltimatumCardputerKey

Firmware for M5Stack Cardputer (ESP32-S3) device.

## Build Instructions

### Prerequisites
- PlatformIO Core or PlatformIO IDE
- USB cable for flashing

### Building
```bash
pio run
```

### Flashing to Cardputer
```bash
# Via USB
pio run --target upload

# Via OTA (if configured)
pio run --target upload --upload-port Cardputer-OTA.local
```

### Serial Monitor
```bash
pio device monitor
```

## Hardware
- Board: M5Stack Cardputer
- MCU: ESP32-S3
- Framework: Arduino 
