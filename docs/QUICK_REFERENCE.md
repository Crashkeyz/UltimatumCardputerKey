# SD Card Storage - Quick Reference

## Quick Start

```cpp
#include "lib/storage_manager/storage_manager.h"
#include "lib/storage_manager/sd_logger.h"

StorageManager storage;
SDLogger logger;

void setup() {
    // Initialize SD card
    if (storage.begin()) {
        // Create directories
        storage.createDefaultDirectories();
        
        // Initialize logger
        logger.begin(&storage);
        logger.info("System started");
        
        // Write a file
        storage.writeFile("/data/test.txt", "Hello!");
        
        // Read a file
        String data;
        storage.readFile("/data/test.txt", data);
    }
}
```

## Common Operations

### Check SD Card Status
```cpp
if (storage.isReady()) {
    Serial.println("SD card is ready");
    Serial.println("Type: " + storage.getCardType());
    Serial.printf("Free: %llu MB\n", storage.getFreeSpace() / (1024*1024));
}
```

### Write File
```cpp
String data = "Temperature: 25.5\n";
if (storage.writeFile("/data/sensor.txt", data.c_str())) {
    Serial.println("File written successfully");
}
```

### Append to File
```cpp
String entry = "New data line\n";
storage.appendFile("/data/log.txt", entry.c_str());
```

### Read File
```cpp
String content;
if (storage.readFile("/data/sensor.txt", content)) {
    Serial.println("File contents:");
    Serial.println(content);
}
```

### Check if File Exists
```cpp
if (storage.fileExists("/data/sensor.txt")) {
    Serial.println("File exists!");
}
```

### Delete File
```cpp
if (storage.deleteFile("/data/old.txt")) {
    Serial.println("File deleted");
}
```

### Create Directory
```cpp
storage.createDir("/mydata");
storage.createDir("/mydata/subfolder");
```

### List Directory
```cpp
storage.listDir("/data", false);  // Non-recursive
storage.listDir("/data", true);   // Recursive
```

### Get File Size
```cpp
size_t size = storage.getFileSize("/data/sensor.txt");
Serial.printf("File size: %d bytes\n", size);
```

## Logging Examples

### Basic Logging
```cpp
logger.info("Application started");
logger.warning("Low battery detected");
logger.error("Sensor read failed");
```

### Configure Logger
```cpp
logger.setMaxLogSize(512 * 1024);  // 512KB max
logger.setConsoleOutput(true);     // Enable Serial output
```

### Manual Log Rotation
```cpp
if (logger.getLogSize() > 1000000) {
    logger.rotateLog();
}
```

## Error Handling

### Check for Errors
```cpp
if (!storage.writeFile("/data/test.txt", "data")) {
    Serial.println("Error: " + storage.getLastError());
}
```

### Safe Write with Space Check
```cpp
void safeWrite(const char* path, const char* data) {
    if (!storage.isReady()) {
        Serial.println("SD card not ready");
        return;
    }
    
    size_t needed = strlen(data);
    if (storage.getFreeSpace() < needed) {
        Serial.println("Not enough space!");
        return;
    }
    
    storage.writeFile(path, data);
}
```

## Interactive Commands (in main.cpp)

When firmware is running:
- **Press 'w'**: Write test file
- **Press 'r'**: Read test file
- **Press 'l'**: List files in /data
- **Press 's'**: Show SD status on display

## Default Directory Structure

```
/SDCARD/
├── config/         # Configuration files
├── data/           # User data
│   ├── captures/   # Captured data
│   └── recordings/ # Recordings
├── logs/           # System logs
└── downloads/      # Downloaded files
```

## Troubleshooting

### SD Card Not Detected
1. Check card is inserted properly
2. Verify card is FAT32 formatted
3. Try different card

### Write Fails
1. Check free space: `storage.getFreeSpace()`
2. Verify directory exists
3. Check card not write-protected

### Performance Issues
1. Use Class 10+ SD card
2. Batch writes instead of many small writes
3. Reduce logging verbosity

## Best Practices

1. **Always check SD status**: Use `storage.isReady()` before operations
2. **Use absolute paths**: Start paths with `/`
3. **Check free space**: Monitor available space regularly
4. **Batch operations**: Group related writes together
5. **Handle errors**: Check return values and use `getLastError()`
6. **Close properly**: Files are auto-closed, but verify operations complete
7. **Backup important data**: SD cards can fail or corrupt

## More Information

See [SD_CARD_GUIDE.md](SD_CARD_GUIDE.md) for comprehensive documentation.
