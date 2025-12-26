/**
 * SD Card Logger Example
 * 
 * This example demonstrates the logging system:
 * - Initialize logger
 * - Log different message types (info, warning, error)
 * - Configure log settings
 * - Check log size
 * - Rotate logs
 */

#include <Arduino.h>
#include <M5Unified.h>
#include "lib/storage_manager/storage_manager.h"
#include "lib/storage_manager/sd_logger.h"

StorageManager storage;
SDLogger logger;

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("\n=== SD Card Logger Example ===\n");
    
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
    
    // Create log directory
    storage.createDir("/logs");
    
    // Initialize logger
    Serial.println("Initializing logger...");
    if (!logger.begin(&storage)) {
        Serial.println("ERROR: Logger initialization failed!");
        return;
    }
    Serial.println("Logger initialized\n");
    
    // Configure logger
    logger.setMaxLogSize(100 * 1024);  // 100KB max log size
    logger.setConsoleOutput(true);     // Enable console output
    
    // Log different message types
    Serial.println("=== Logging Messages ===");
    logger.info("Logger example started");
    logger.info("System initialized successfully");
    
    delay(100);
    logger.warning("This is a warning message");
    logger.warning("Low battery detected (example)");
    
    delay(100);
    logger.error("This is an error message");
    logger.error("Sensor read failed (example)");
    
    Serial.println();
    
    // Simulate application events
    Serial.println("=== Simulating Application Events ===");
    
    logger.info("Starting WiFi connection");
    delay(50);
    logger.info("WiFi connected successfully");
    
    delay(100);
    logger.info("Reading sensor data");
    delay(50);
    logger.info("Temperature: 25.5°C, Humidity: 60%");
    
    delay(100);
    logger.warning("Battery level low: 15%");
    
    delay(100);
    logger.info("Saving data to SD card");
    delay(50);
    logger.info("Data saved successfully");
    
    Serial.println();
    
    // Check log size
    Serial.println("=== Log Information ===");
    size_t logSize = logger.getLogSize();
    Serial.printf("Current log size: %d bytes\n", logSize);
    
    // Show log file location
    Serial.println("Log file location: /logs/system.log");
    Serial.println();
    
    // Read and display log file
    Serial.println("=== Log File Contents ===");
    String logContent;
    if (storage.readFile("/logs/system.log", logContent)) {
        Serial.println(logContent);
    }
    
    Serial.println("=== Example Complete ===");
    Serial.println("Check SD card at /logs/system.log to see the log file");
}

int loopCount = 0;

void loop() {
    delay(5000);
    
    // Continue logging in loop
    loopCount++;
    logger.info("Loop iteration: " + String(loopCount));
    
    if (loopCount % 3 == 0) {
        logger.warning("Periodic warning (every 3rd iteration)");
    }
    
    if (loopCount >= 10) {
        logger.info("Stopping periodic logging after 10 iterations");
        while(1) delay(1000);  // Stop
    }
}
