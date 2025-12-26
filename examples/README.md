# SD Card Storage Examples

This directory contains example code demonstrating various SD card storage features.

## Examples

### 1. basic_storage.cpp
Basic SD card operations including:
- SD card initialization
- Directory creation
- File writing and reading
- File information retrieval
- Directory listing

**What it demonstrates:**
- `storage.begin()` - Initialize SD card
- `storage.createDir()` - Create directories
- `storage.writeFile()` - Write files
- `storage.readFile()` - Read files
- `storage.appendFile()` - Append to files
- `storage.listDir()` - List directory contents
- `storage.fileExists()` - Check file existence
- `storage.getFileSize()` - Get file size

### 2. logger_example.cpp
Logging system usage including:
- Logger initialization
- Different log levels (info, warning, error)
- Log configuration
- Viewing log contents

**What it demonstrates:**
- `logger.begin()` - Initialize logger
- `logger.info()` - Log info messages
- `logger.warning()` - Log warnings
- `logger.error()` - Log errors
- `logger.setMaxLogSize()` - Configure max log size
- `logger.setConsoleOutput()` - Enable/disable console output
- `logger.getLogSize()` - Check current log size

### 3. data_logging.cpp
Practical data logging example including:
- CSV file creation with headers
- Sensor data logging with timestamps
- Automatic log rotation
- Storage space monitoring

**What it demonstrates:**
- Creating structured data files (CSV)
- Appending timestamped data
- File size monitoring
- Automatic log rotation when size limit reached
- Storage space checking

## How to Use These Examples

### Option 1: Copy Code to main.cpp
1. Copy the example code you want to try
2. Replace the contents of `src/main.cpp` with the example
3. Build and upload to your Cardputer

### Option 2: Create Separate Project
1. Create a new PlatformIO project
2. Copy the example file to `src/main.cpp`
3. Copy the library files from `lib/storage_manager/`
4. Use the same `platformio.ini` configuration

### Option 3: Integrate into Your Code
1. Study the example code
2. Copy relevant functions to your project
3. Adapt to your specific needs

## Hardware Requirements

- M5Stack Cardputer ADV
- MicroSD card (formatted as FAT32)
- USB-C cable for programming and power

## Expected Output

Each example provides detailed serial output showing:
- Initialization status
- Operation results
- Error messages (if any)
- File contents
- Storage information

Connect to the serial monitor at 115200 baud to see the output.

## Testing the Examples

### Before Running
1. Format SD card as FAT32
2. Insert SD card into Cardputer
3. Connect Cardputer via USB-C
4. Open serial monitor

### Verifying Results
After running examples, you can:
1. Remove SD card safely
2. Insert into computer
3. Check created files and directories
4. Open files to verify contents

## Troubleshooting

### "SD card initialization failed"
- Check SD card is inserted
- Verify SD card is formatted as FAT32
- Try a different SD card

### "Failed to write file"
- Check SD card has free space
- Verify SD card is not write-protected
- Ensure directory exists

### No serial output
- Check USB cable connection
- Verify serial monitor baud rate is 115200
- Try pressing reset button on Cardputer

## Modifying the Examples

Feel free to modify these examples:
- Change file paths and names
- Adjust timing intervals
- Add your own sensor readings
- Modify log formats
- Experiment with different operations

## Additional Resources

- [SD Card Setup Guide](../docs/SD_CARD_GUIDE.md) - Comprehensive documentation
- [Quick Reference](../docs/QUICK_REFERENCE.md) - Quick API reference
- [README.md](../README.md) - Project overview

## Example Output

### basic_storage.cpp
```
=== SD Card Basic Example ===

Initializing SD card...
SD card initialized successfully!

=== SD Card Information ===
Type: SDHC
Total space: 15831 MB
Used space: 1 MB
Free space: 15830 MB

=== Creating Directories ===
Directories created

=== Writing File ===
File written successfully

=== Reading File ===
File contents:
Hello from M5Stack Cardputer!
This is a test file.
Timestamp: 12345 ms
```

### logger_example.cpp
```
=== SD Card Logger Example ===

Initializing SD card...
SD card initialized

Initializing logger...
Logger initialized

=== Logging Messages ===
[00:00:05.123] INFO: Logger example started
[00:00:05.234] INFO: System initialized successfully
[00:00:05.456] WARN: This is a warning message
[00:00:05.567] ERROR: This is an error message
```

### data_logging.cpp
```
=== Data Logging Example ===

Initializing SD card...
SD card initialized

CSV file created with header
Storage: 15830 MB free / 15831 MB total

Starting data logging (every 2 seconds)...

Logged: T=22.45°C, H=55.30%, P=1015.20hPa
Logged: T=21.80°C, H=58.70%, P=1012.90hPa
```

## Contributing

If you create useful examples, feel free to contribute them back to the project!
