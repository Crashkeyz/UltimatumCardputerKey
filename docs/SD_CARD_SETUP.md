# SD Card Setup Guide

## Overview
The M5Stack Cardputer ADV has a built-in microSD card slot that can be used to store data externally, preventing flash memory overflow issues.

## Why Use SD Card?

### Problem
When flashing the Cardputer ADV firmware, you may encounter "out of memory" errors because:
- The firmware binary is large
- ESP32-S3 has 16MB flash, but partition overhead reduces available space
- OTA updates require two app partitions

### Solution
By using an SD card:
- Store application data externally
- Keep flash memory for firmware only
- Eliminate memory errors during flashing
- Easy data backup and transfer

## Setup Instructions

### 1. Prepare SD Card
- Use a microSD card (Class 10 or better recommended)
- Format as FAT32
- Insert into Cardputer's SD slot (bottom of device)

### 2. Use the Correct Branch
```bash
git checkout copilot/optimize-flashing-to-sd-card
```

### 3. Flash the Firmware
```bash
pio run --target upload
```

The firmware will automatically:
- Detect the SD card
- Create a `/data` directory
- Report SD card status via serial monitor

### 4. Verify SD Card
Check serial monitor output for:
```
Initializing SD card...
SD Card Type: SDHC
SD Card Size: 32000MB
Total space: 32000MB
Used space: 0MB
Created /data directory on SD card
SD card initialized successfully
```

## Using SD Card in Your Code

### Writing Files
```cpp
#include <SD.h>

void saveToSD() {
    File file = SD.open("/data/config.txt", FILE_WRITE);
    if (file) {
        file.println("My configuration data");
        file.close();
        Serial.println("Data saved to SD card");
    }
}
```

### Reading Files
```cpp
void readFromSD() {
    File file = SD.open("/data/config.txt");
    if (file) {
        while (file.available()) {
            String line = file.readStringUntil('\n');
            Serial.println(line);
        }
        file.close();
    }
}
```

### Listing Files
```cpp
void listFiles() {
    File root = SD.open("/data");
    File file = root.openNextFile();
    
    while (file) {
        Serial.print("FILE: ");
        Serial.print(file.name());
        Serial.print(" SIZE: ");
        Serial.println(file.size());
        file = root.openNextFile();
    }
}
```

## Partition Scheme

The optimized partition scheme (`default_16MB.csv`) allocates:

| Partition | Size | Purpose |
|-----------|------|---------|
| app0 | 3.75MB | Main firmware |
| app1 | 3.75MB | OTA update slot |
| spiffs | 8.5MB | File system |
| nvs | 20KB | Settings storage |
| coredump | 64KB | Debug info |

**Total: 16MB efficiently utilized**

## Troubleshooting

### SD Card Not Detected
1. Check card is properly inserted
2. Try different SD card (some cards have compatibility issues)
3. Reformat card as FAT32
4. Check serial monitor for error messages

### Still Running Out of Memory
1. Verify you're using the `copilot/optimize-flashing-to-sd-card` branch
2. Clean build: `pio run --target clean`
3. Check `platformio.ini` references `default_16MB.csv`
4. Reduce code size by removing unused libraries

### Build Errors
```bash
# Clean and rebuild
pio run --target clean
pio run

# Update dependencies
pio pkg update
```

## Best Practices

1. **Always check SD card presence before writing**
   ```cpp
   if (SD.exists("/data")) {
       // SD card is available
   }
   ```

2. **Close files after use**
   ```cpp
   file.close(); // Always close files to prevent corruption
   ```

3. **Use `/data` directory for organization**
   - Firmware automatically creates this
   - Keeps your files organized

4. **Handle SD card removal gracefully**
   - Don't assume SD card is always present
   - Provide fallback to internal storage if needed

## Additional Resources

- [M5Stack Cardputer Documentation](https://docs.m5stack.com/en/core/Cardputer)
- [ESP32 Partition Tables](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/partition-tables.html)
- [Arduino SD Library](https://www.arduino.cc/reference/en/libraries/sd/)
