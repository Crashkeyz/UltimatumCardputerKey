/**
 * Data Logging Example
 * 
 * This example demonstrates logging sensor data to SD card:
 * - Create CSV data files
 * - Append sensor readings with timestamps
 * - Monitor file size and available space
 * - Implement simple data rotation
 */

#include <Arduino.h>
#include <M5Unified.h>
#include "lib/storage_manager/storage_manager.h"

StorageManager storage;

// Constants for size conversions
const uint64_t MB_DIVISOR = 1024 * 1024;

// Simulated sensor readings
float getTemperature() {
    return 20.0 + random(-50, 50) / 10.0;  // 15-25°C
}

float getHumidity() {
    return 50.0 + random(-200, 200) / 10.0;  // 30-70%
}

float getPressure() {
    return 1013.25 + random(-50, 50) / 10.0;  // ±5 hPa
}

void createCSVHeader() {
    String header = "Timestamp(ms),Temperature(C),Humidity(%),Pressure(hPa)\n";
    storage.writeFile("/data/sensors.csv", header.c_str());
    Serial.println("CSV file created with header");
}

void logSensorData() {
    // Get sensor readings
    float temp = getTemperature();
    float humidity = getHumidity();
    float pressure = getPressure();
    
    // Format as CSV
    String csvLine = String(millis()) + ",";
    csvLine += String(temp, 2) + ",";
    csvLine += String(humidity, 2) + ",";
    csvLine += String(pressure, 2) + "\n";
    
    // Append to file
    if (storage.appendFile("/data/sensors.csv", csvLine.c_str())) {
        Serial.printf("Logged: T=%.2f°C, H=%.2f%%, P=%.2fhPa\n", 
                      temp, humidity, pressure);
    } else {
        Serial.println("ERROR: Failed to log data");
    }
}

void checkStorageSpace() {
    uint64_t freeMB = storage.getFreeSpace() / MB_DIVISOR;
    uint64_t totalMB = storage.getTotalSpace() / MB_DIVISOR;
    
    Serial.printf("Storage: %llu MB free / %llu MB total\n", freeMB, totalMB);
    
    if (freeMB < 10) {
        Serial.println("WARNING: Less than 10MB free!");
    }
}

void rotateLogFile() {
    // Check file size
    size_t fileSize = storage.getFileSize("/data/sensors.csv");
    size_t maxSize = 100 * 1024;  // 100KB
    
    if (fileSize > maxSize) {
        Serial.println("File size limit reached, rotating...");
        
        // Create backup filename
        String backupName = "/data/sensors_" + String(millis()) + ".csv";
        
        // Read current file
        String data;
        if (storage.readFile("/data/sensors.csv", data)) {
            // Write to backup
            if (storage.writeFile(backupName.c_str(), data.c_str())) {
                Serial.println("Backup created: " + backupName);
                
                // Start new file
                createCSVHeader();
                Serial.println("New log file started");
            }
        }
    }
}

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("\n=== Data Logging Example ===\n");
    
    // Initialize M5Unified
    auto cfg = M5.config();
    M5.begin(cfg);
    
    // Initialize SD card
    Serial.println("Initializing SD card...");
    if (!storage.begin()) {
        Serial.println("ERROR: SD card initialization failed!");
        return;
    }
    Serial.println("SD card initialized\n");
    
    // Create data directory
    storage.createDir("/data");
    
    // Create CSV file with header
    if (!storage.fileExists("/data/sensors.csv")) {
        createCSVHeader();
    } else {
        Serial.println("Using existing CSV file");
    }
    
    // Show initial storage space
    checkStorageSpace();
    Serial.println();
    
    Serial.println("Starting data logging (every 2 seconds)...");
    Serial.println("Data will be saved to /data/sensors.csv");
    Serial.println();
}

int logCount = 0;

void loop() {
    // Log sensor data
    logSensorData();
    logCount++;
    
    // Check for rotation every 10 readings
    if (logCount % 10 == 0) {
        rotateLogFile();
        
        // Show file size
        size_t fileSize = storage.getFileSize("/data/sensors.csv");
        Serial.printf("Current file size: %d bytes\n", fileSize);
    }
    
    // Check storage space every 20 readings
    if (logCount % 20 == 0) {
        checkStorageSpace();
    }
    
    // Stop after 50 readings for this example
    if (logCount >= 50) {
        Serial.println("\n=== Example Complete ===");
        Serial.println("50 sensor readings logged");
        Serial.println("Check SD card at /data/sensors.csv");
        
        // Show final file contents (last 10 lines)
        String data;
        if (storage.readFile("/data/sensors.csv", data)) {
            Serial.println("\nLast entries from CSV:");
            // Print last few lines
            int lastNewline = data.lastIndexOf('\n', data.length() - 2);
            for (int i = 0; i < 5 && lastNewline > 0; i++) {
                lastNewline = data.lastIndexOf('\n', lastNewline - 1);
            }
            if (lastNewline > 0) {
                Serial.println(data.substring(lastNewline + 1));
            }
        }
        
        while(1) delay(1000);  // Stop
    }
    
    delay(2000);  // Log every 2 seconds
}
