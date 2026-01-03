#include <Arduino.h>
#include <SD.h>
#include <FS.h>
#include <Update.h>
#include <M5Unified.h>

// Firmware sideload configuration
#define FIRMWARE_PATH "/firmware"
#define AUTOLOAD_FILE "/firmware/autoload.bin"
#define UPDATE_STATUS_FILE "/firmware/status.txt"

// Write status to SD card
void write_update_status(const char* status) {
    File file = SD.open(UPDATE_STATUS_FILE, FILE_WRITE);
    if (file) {
        file.println(status);
        file.close();
        Serial.println(status);
    }
}

// Check for firmware on SD card and perform update
bool check_and_update_from_sd() {
    Serial.println(F("Checking for firmware update on SD..."));
    
    // Create firmware directory if it doesn't exist
    if (!SD.exists(FIRMWARE_PATH)) {
        SD.mkdir(FIRMWARE_PATH);
        Serial.println(F("Created /firmware directory"));
        return false;
    }
    
    // Check if autoload firmware exists
    if (!SD.exists(AUTOLOAD_FILE)) {
        Serial.println(F("No firmware file at /firmware/autoload.bin"));
        return false;
    }
    
    // Open firmware file
    File firmware = SD.open(AUTOLOAD_FILE);
    if (!firmware) {
        write_update_status("ERROR: Cannot open firmware file");
        return false;
    }
    
    size_t fileSize = firmware.size();
    Serial.printf("Found firmware file: %d bytes\n", fileSize);
    
    // Display update UI
    M5.Display.fillScreen(BLACK);
    M5.Display.setCursor(10, 10);
    M5.Display.setTextColor(YELLOW);
    M5.Display.println(F("FIRMWARE UPDATE"));
    M5.Display.setTextColor(WHITE);
    M5.Display.println(F("Found: autoload.bin"));
    M5.Display.printf("Size: %d KB\n", fileSize / 1024);
    M5.Display.println(F("\nStarting update..."));
    
    // Start update process
    if (!Update.begin(fileSize)) {
        write_update_status("ERROR: Not enough space for OTA");
        M5.Display.setTextColor(RED);
        M5.Display.println(F("ERROR: Not enough space"));
        firmware.close();
        delay(3000);
        return false;
    }
    
    // Write firmware data
    size_t written = 0;
    uint8_t buffer[512];
    int lastPercent = 0;
    
    while (firmware.available()) {
        size_t len = firmware.read(buffer, sizeof(buffer));
        written += Update.write(buffer, len);
        
        int percent = (written * 100) / fileSize;
        if (percent != lastPercent && percent % 10 == 0) {
            M5.Display.printf("Progress: %d%%\n", percent);
            Serial.printf("Update progress: %d%%\n", percent);
            lastPercent = percent;
        }
    }
    
    firmware.close();
    
    // Finalize update
    if (Update.end(true)) {
        write_update_status("SUCCESS: Firmware updated successfully");
        M5.Display.setTextColor(GREEN);
        M5.Display.println(F("\nUPDATE SUCCESS!"));
        M5.Display.println(F("Rebooting..."));
        
        // Rename the file to prevent re-flashing on next boot
        SD.rename(AUTOLOAD_FILE, "/firmware/autoload.bin.old");
        
        delay(2000);
        ESP.restart();
        return true;
    } else {
        String error = "ERROR: " + String(Update.errorString());
        write_update_status(error.c_str());
        M5.Display.setTextColor(RED);
        M5.Display.println(F("\nUPDATE FAILED!"));
        M5.Display.println(Update.errorString());
        delay(5000);
        return false;
    }
}

// List available firmware files on SD card
void list_firmware_files() {
    if (!SD.exists(FIRMWARE_PATH)) {
        Serial.println(F("No firmware directory found"));
        return;
    }
    
    File dir = SD.open(FIRMWARE_PATH);
    if (!dir) {
        Serial.println(F("Failed to open firmware directory"));
        return;
    }
    
    Serial.println(F("Available firmware files:"));
    File file = dir.openNextFile();
    while (file) {
        if (!file.isDirectory()) {
            Serial.printf("  %s (%d bytes)\n", file.name(), file.size());
        }
        file.close();  // Properly close each file
        file = dir.openNextFile();
    }
    dir.close();
}

// Copy firmware from internal to SD card for backup
bool backup_current_firmware() {
    Serial.println(F("Backing up current firmware..."));
    
    if (!SD.exists(FIRMWARE_PATH)) {
        SD.mkdir(FIRMWARE_PATH);
    }
    
    // Create backup filename with timestamp (microseconds for uniqueness)
    char filename[64];
    snprintf(filename, sizeof(filename), "/firmware/backup_%lu_%lu.bin", 
             millis(), micros() % 1000);
    
    File backup = SD.open(filename, FILE_WRITE);
    if (!backup) {
        Serial.println(F("Failed to create backup file"));
        return false;
    }
    
    // NOTE: This is a placeholder implementation
    // Actual firmware backup would require reading from the flash partition
    // using esp_partition API, which is not implemented in this version
    // The file is created for future implementation
    backup.close();
    
    Serial.printf("Backup placeholder created: %s\n", filename);
    Serial.println(F("WARNING: Actual backup not implemented"));
    return true;
}

// Initialize firmware sideload system
void init_firmware_sideload() {
    Serial.println(F("Initializing firmware sideload..."));
    
    // Create firmware directory structure
    if (!SD.exists(FIRMWARE_PATH)) {
        SD.mkdir(FIRMWARE_PATH);
        Serial.println(F("Created firmware directory"));
    }
    
    // Create README file with instructions
    const char readme[] PROGMEM = 
        "FIRMWARE SIDELOAD\n"
        "=================\n\n"
        "To update firmware:\n"
        "1. Place firmware.bin in this folder\n"
        "2. Rename to: autoload.bin\n"
        "3. Restart Cardputer\n"
        "4. Auto-updates on boot\n\n"
        "After update, file renamed to .old\n";
    
    File readme_file = SD.open("/firmware/README.txt", FILE_WRITE);
    if (readme_file) {
        readme_file.print(FPSTR(readme));
        readme_file.close();
    }
    
    Serial.println(F("Firmware sideload ready"));
    Serial.println(F("Place: /firmware/autoload.bin"));
}
