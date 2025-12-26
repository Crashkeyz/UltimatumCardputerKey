# SD Card Storage Guide

Comprehensive guide for using SD card storage with M5Stack Cardputer ADV firmware.

## Table of Contents
- [Overview](#overview)
- [SD Card Requirements](#sd-card-requirements)
- [Setup Instructions](#setup-instructions)
- [Directory Structure](#directory-structure)
- [API Reference](#api-reference)
- [Usage Examples](#usage-examples)
- [Best Practices](#best-practices)
- [Troubleshooting](#troubleshooting)

## Overview

The M5Stack Cardputer ADV firmware includes comprehensive SD card storage support to offload data from internal flash memory. This allows you to:
- Store large amounts of data (logs, captures, configurations)
- Preserve internal flash for firmware code
- Easily transfer data between devices
- Maintain persistent storage across firmware updates

### Key Features
- Automatic SD card initialization on boot
- Automatic directory structure creation
- Robust error handling with graceful fallbacks
- File and directory management
- Free space monitoring
- Logging system with automatic rotation
- Configuration management from SD card

## SD Card Requirements

### Capacity
- **Minimum**: 512MB (though not recommended)
- **Recommended**: 4GB - 32GB for most applications
- **Maximum**: 128GB (depending on card type and format)

### File System Format
- **For cards up to 32GB**: FAT32 (required)
- **For cards 64GB and larger**: exFAT (may require library support)
- **Not supported**: NTFS, ext4

### Speed Class
- **Minimum**: Class 4
- **Recommended**: Class 10 or UHS-I for better performance
- Higher speed class improves file write/read performance

### Quality Considerations
- Use reputable brand SD cards (SanDisk, Samsung, Kingston, etc.)
- Avoid cheap no-name cards that may corrupt or fail
- Industrial-grade cards recommended for mission-critical applications

## Setup Instructions

### 1. Format the SD Card

**On Windows:**
1. Insert SD card into computer
2. Open "This PC" / "My Computer"
3. Right-click on SD card drive
4. Select "Format..."
5. Choose "FAT32" as file system
6. Click "Start"

**On macOS:**
1. Open "Disk Utility"
2. Select the SD card
3. Click "Erase"
4. Choose "MS-DOS (FAT)" as format
5. Click "Erase"

**On Linux:**
```bash
# Find SD card device (e.g., /dev/sdb1)
lsblk

# Unmount if mounted
sudo umount /dev/sdb1

# Format as FAT32
sudo mkfs.vfat -F 32 /dev/sdb1
```

### 2. Insert SD Card
1. Power off the M5Stack Cardputer
2. Insert microSD card into the SD card slot (metal contacts facing up)
3. Push gently until it clicks into place
4. Power on the device

### 3. Verify Initialization
The firmware will automatically:
1. Detect the SD card
2. Initialize the file system
3. Create default directory structure
4. Display status on screen
5. Log initialization to system log

Check the serial monitor for detailed initialization messages:
```
[Storage] Initializing SD card...
[Storage] SD card type: SDHC
[Storage] SD card size: 16384 MB
[Storage] Total space: 15831 MB
[Storage] Free space: 15830 MB
[Storage] SD card initialized successfully
```

## Directory Structure

The firmware automatically creates the following structure:

```
/SDCARD/
├── config/
│   ├── settings.json       # Main device configuration
│   └── device_config.json  # Device-specific settings
├── data/
│   ├── captures/           # Data capture storage
│   └── recordings/         # Audio/video recordings
├── logs/
│   └── system.log          # System event log (auto-created)
└── downloads/              # Downloaded files
```

### Directory Descriptions

**`/config/`**
- Stores configuration files in JSON format
- Settings persist across reboots
- Automatically created if missing

**`/data/`**
- Primary storage for user data
- Subdirectories for different data types
- Organized structure for easy file management

**`/logs/`**
- System and application logs
- Timestamped entries
- Automatic log rotation when size limit reached

**`/downloads/`**
- Files downloaded from network
- Temporary storage for received data

## API Reference

### StorageManager Class

#### Initialization

```cpp
StorageManager storage;

bool begin()
```
Initialize SD card and mount file system.
- **Returns**: `true` if successful, `false` if SD card not present or mount failed

```cpp
bool isReady()
```
Check if SD card is initialized and ready.
- **Returns**: `true` if ready for operations

```cpp
String getCardType()
```
Get SD card type as string.
- **Returns**: "SDHC", "SDSC", "SDXC", "MMC", "Unknown", or "No Card"

#### Space Management

```cpp
uint64_t getTotalSpace()
```
Get total capacity of SD card in bytes.
- **Returns**: Total space in bytes

```cpp
uint64_t getFreeSpace()
```
Get available free space in bytes.
- **Returns**: Free space in bytes

```cpp
uint64_t getUsedSpace()
```
Get used space in bytes.
- **Returns**: Used space in bytes

#### File Operations

```cpp
bool writeFile(const char* path, const char* data)
```
Write data to file (overwrites existing file).
- **Parameters**: 
  - `path`: Full path to file (e.g., "/data/test.txt")
  - `data`: Data to write (null-terminated string)
- **Returns**: `true` if successful

```cpp
bool appendFile(const char* path, const char* data)
```
Append data to existing file.
- **Parameters**: 
  - `path`: Full path to file
  - `data`: Data to append
- **Returns**: `true` if successful

```cpp
bool readFile(const char* path, String& data)
```
Read entire file into String.
- **Parameters**: 
  - `path`: Full path to file
  - `data`: Reference to String to store file contents
- **Returns**: `true` if successful

```cpp
bool deleteFile(const char* path)
```
Delete file from SD card.
- **Parameters**: 
  - `path`: Full path to file
- **Returns**: `true` if successful

```cpp
bool fileExists(const char* path)
```
Check if file exists.
- **Parameters**: 
  - `path`: Full path to file
- **Returns**: `true` if file exists

```cpp
size_t getFileSize(const char* path)
```
Get file size in bytes.
- **Parameters**: 
  - `path`: Full path to file
- **Returns**: File size in bytes (0 if doesn't exist)

#### Directory Operations

```cpp
bool createDir(const char* path)
```
Create directory (returns true if already exists).
- **Parameters**: 
  - `path`: Directory path (e.g., "/mydata")
- **Returns**: `true` if created or already exists

```cpp
bool removeDir(const char* path)
```
Remove empty directory.
- **Parameters**: 
  - `path`: Directory path
- **Returns**: `true` if successful

```cpp
bool listDir(const char* path, bool recursive = false)
```
List directory contents to Serial.
- **Parameters**: 
  - `path`: Directory path
  - `recursive`: If true, list subdirectories recursively
- **Returns**: `true` if successful

```cpp
bool createDefaultDirectories()
```
Create standard directory structure.
- **Returns**: `true` if all directories created

#### Error Handling

```cpp
String getLastError()
```
Get last error message.
- **Returns**: String describing last error

```cpp
void end()
```
Unmount SD card and clean up.

### SDLogger Class

#### Initialization

```cpp
SDLogger logger;

bool begin(StorageManager* storage)
```
Initialize logger with storage manager.
- **Parameters**: 
  - `storage`: Pointer to initialized StorageManager
- **Returns**: `true` if successful

#### Configuration

```cpp
void setMaxLogSize(size_t maxSize)
```
Set maximum log file size before rotation.
- **Parameters**: 
  - `maxSize`: Size in bytes (default: 1MB)

```cpp
void setConsoleOutput(bool enabled)
```
Enable/disable console output.
- **Parameters**: 
  - `enabled`: true to output to Serial

#### Logging Methods

```cpp
void info(const char* message)
void info(const String& message)
```
Log informational message.

```cpp
void warning(const char* message)
void warning(const String& message)
```
Log warning message.

```cpp
void error(const char* message)
void error(const String& message)
```
Log error message.

```cpp
bool rotateLog()
```
Manually rotate log file.
- **Returns**: `true` if successful

```cpp
size_t getLogSize()
```
Get current log file size.
- **Returns**: Size in bytes

## Usage Examples

### Example 1: Basic File Operations

```cpp
#include "lib/storage_manager/storage_manager.h"

void setup() {
    Serial.begin(115200);
    
    StorageManager storage;
    if (storage.begin()) {
        // Write data
        String data = "Temperature: 25.5°C\nHumidity: 60%";
        storage.writeFile("/data/sensor.txt", data.c_str());
        
        // Read data
        String readData;
        if (storage.readFile("/data/sensor.txt", readData)) {
            Serial.println("Read from SD:");
            Serial.println(readData);
        }
        
        // Check file size
        size_t size = storage.getFileSize("/data/sensor.txt");
        Serial.printf("File size: %d bytes\n", size);
        
        // Delete file
        storage.deleteFile("/data/sensor.txt");
    }
}
```

### Example 2: Logging System

```cpp
#include "lib/storage_manager/storage_manager.h"
#include "lib/storage_manager/sd_logger.h"

StorageManager storage;
SDLogger logger;

void setup() {
    Serial.begin(115200);
    
    if (storage.begin()) {
        logger.begin(&storage);
        logger.setMaxLogSize(512 * 1024);  // 512KB max
        
        logger.info("System started");
        logger.warning("Low battery detected");
        logger.error("Sensor read failed");
    }
}

void loop() {
    // Your code
    if (errorOccurred) {
        logger.error("An error occurred in main loop");
    }
}
```

### Example 3: Data Logging with Timestamps

```cpp
void logSensorData(float temperature, float humidity) {
    if (!storage.isReady()) {
        Serial.println("SD card not ready");
        return;
    }
    
    // Create CSV format entry
    String logEntry = String(millis()) + ",";
    logEntry += String(temperature, 2) + ",";
    logEntry += String(humidity, 2) + "\n";
    
    // Append to log file
    if (storage.appendFile("/data/sensors.csv", logEntry.c_str())) {
        Serial.println("Sensor data logged");
    }
}
```

### Example 4: Configuration Management

```cpp
void saveConfiguration() {
    if (!storage.isReady()) return;
    
    String config = "{\n";
    config += "  \"wifi_ssid\": \"MyNetwork\",\n";
    config += "  \"wifi_password\": \"password123\",\n";
    config += "  \"device_name\": \"Cardputer-001\"\n";
    config += "}\n";
    
    storage.writeFile("/config/settings.json", config.c_str());
}

void loadConfiguration() {
    if (!storage.isReady() || !storage.fileExists("/config/settings.json")) {
        // Use defaults
        return;
    }
    
    String config;
    if (storage.readFile("/config/settings.json", config)) {
        // Parse JSON and apply settings
        Serial.println("Configuration loaded from SD");
    }
}
```

### Example 5: Check Free Space Before Writing

```cpp
void safeWrite(const char* path, const char* data) {
    if (!storage.isReady()) {
        Serial.println("SD card not available");
        return;
    }
    
    // Calculate required space (with 10% buffer)
    size_t dataSize = strlen(data);
    size_t requiredSpace = dataSize * 1.1;
    
    // Check available space
    uint64_t freeSpace = storage.getFreeSpace();
    
    if (freeSpace < requiredSpace) {
        Serial.printf("ERROR: Not enough space! Need %d bytes, have %llu bytes\n", 
                      requiredSpace, freeSpace);
        return;
    }
    
    // Safe to write
    storage.writeFile(path, data);
}
```

## Best Practices

### 1. Always Check SD Card Status

```cpp
if (storage.isReady()) {
    // Perform SD operations
} else {
    // Fallback to internal storage or skip
    Serial.println("SD card not available - using fallback");
}
```

### 2. Close Files Properly

The StorageManager automatically closes files after operations, but if using raw File objects:

```cpp
File file = SD.open("/data/test.txt", FILE_WRITE);
if (file) {
    file.print("data");
    file.close();  // Always close!
}
```

### 3. Check Free Space Regularly

```cpp
uint64_t freeMB = storage.getFreeSpace() / (1024 * 1024);
if (freeMB < 10) {
    Serial.println("WARNING: Less than 10MB free!");
    // Clean up old files or stop logging
}
```

### 4. Use Absolute Paths

Always use paths starting with `/`:
```cpp
storage.writeFile("/data/test.txt", "data");  // ✓ Good
storage.writeFile("data/test.txt", "data");   // ✗ May fail
```

### 5. Handle Errors Gracefully

```cpp
if (!storage.writeFile("/data/log.txt", data)) {
    Serial.println("Write failed: " + storage.getLastError());
    // Try alternative storage or notify user
}
```

### 6. Avoid Excessive Writes

SD cards have limited write cycles. Batch writes when possible:

```cpp
// Bad: Many small writes
for (int i = 0; i < 100; i++) {
    storage.writeFile("/data/log.txt", String(i).c_str());
}

// Good: Single batched write
String batch = "";
for (int i = 0; i < 100; i++) {
    batch += String(i) + "\n";
}
storage.writeFile("/data/log.txt", batch.c_str());
```

### 7. Use Logging System for Diagnostics

```cpp
logger.info("Starting WiFi connection");
if (WiFi.begin() == WL_CONNECTED) {
    logger.info("WiFi connected successfully");
} else {
    logger.error("WiFi connection failed");
}
```

### 8. Implement Log Rotation

For long-running applications, implement automatic log rotation:

```cpp
if (logger.getLogSize() > 1024 * 1024) {  // 1MB
    logger.rotateLog();
}
```

## Troubleshooting

### SD Card Not Detected

**Symptoms:**
- Display shows "SD: MISSING"
- Serial shows "SD card mount failed"

**Solutions:**
1. Ensure SD card is fully inserted (you should hear/feel a click)
2. Check SD card is formatted as FAT32
3. Try a different SD card to rule out hardware issues
4. Verify SD card works in computer
5. Clean SD card contacts with isopropyl alcohol

### Mount Fails Even with Card Inserted

**Symptoms:**
- "SD card mount failed" error
- Card works in computer

**Solutions:**
1. Reformat SD card as FAT32
2. Try lower capacity card (some high-capacity cards have compatibility issues)
3. Ensure card is not corrupted (check with computer disk utility)
4. Verify card is not write-protected

### Write Operations Failing

**Symptoms:**
- `writeFile()` returns false
- "Failed to open file for writing" errors

**Solutions:**
1. Check SD card has free space: `storage.getFreeSpace()`
2. Verify SD card is not write-protected (physical switch on adapter)
3. Ensure parent directory exists: `storage.createDir("/data")`
4. Check file path is valid (starts with `/`, no invalid characters)
5. Try writing to root directory to test: `storage.writeFile("/test.txt", "test")`

### Read Operations Failing

**Symptoms:**
- `readFile()` returns false
- "Failed to open file for reading" errors

**Solutions:**
1. Verify file exists: `storage.fileExists("/data/test.txt")`
2. Check file permissions
3. Ensure SD card is properly mounted
4. List directory to verify file is present: `storage.listDir("/data")`

### Card Corruption

**Symptoms:**
- Files disappear or contain garbage
- "SD card mount failed" after working before
- File system errors

**Solutions:**
1. Power off device before removing SD card
2. Use `storage.end()` before removing card
3. Backup SD card regularly
4. Use quality SD cards from reputable brands
5. Check for loose connections
6. Reformat SD card (will erase all data)

### Low Performance

**Symptoms:**
- Slow write/read speeds
- System lag during SD operations

**Solutions:**
1. Use Class 10 or higher speed card
2. Batch write operations instead of many small writes
3. Reduce log verbosity if excessive logging
4. Check for fragmentation (reformat if needed)
5. Verify card speed rating matches specifications

### Logger Not Working

**Symptoms:**
- No log file created
- Log messages not appearing

**Solutions:**
1. Ensure storage manager initialized: `storage.begin()`
2. Initialize logger: `logger.begin(&storage)`
3. Check `/logs` directory exists: `storage.createDir("/logs")`
4. Verify SD card has space
5. Check serial output for error messages

### Files Created But Empty

**Symptoms:**
- Files exist but contain no data
- File size is 0 bytes

**Solutions:**
1. Ensure data string is not empty
2. Check return value of write operation
3. Verify file handle is valid
4. Try writing simple test string
5. Check for memory issues (insufficient RAM)

## Additional Resources

### Memory Considerations

The M5Stack Cardputer has:
- **16MB Flash**: For firmware and code
- **8MB PSRAM**: For runtime data and buffers

When working with large files:
- Use PSRAM for temporary buffers
- Read/write in chunks for very large files
- Monitor free heap: `ESP.getFreeHeap()`

### Performance Tips

**Fast Operations:**
- Reading existing files
- Writing to sequential files
- Directory operations

**Slow Operations:**
- Creating many small files
- Random access to large files
- Deleting many files

**Optimization:**
- Batch related operations
- Reuse file handles when possible
- Minimize directory scans

### Safety Considerations

**Data Integrity:**
- Always close files properly
- Don't remove SD card while writing
- Use checksums for critical data
- Implement backup strategies

**Power Loss Protection:**
- Files are closed after each operation
- Minimize writes during low battery
- Consider power loss during write

## Support

For issues or questions:
1. Check this guide's troubleshooting section
2. Review serial monitor output for detailed errors
3. Test with a different SD card
4. Check project issues on GitHub

---

**Last Updated**: 2025-12-26  
**Firmware Version**: 1.0.0 with SD Storage Support
