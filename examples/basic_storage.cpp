/**
 * Basic SD Card Storage Example
 * 
 * This example demonstrates basic SD card operations:
 * - Initialize SD card
 * - Create directories
 * - Write files
 * - Read files
 * - List directory contents
 * - Check storage space
 */

#include <Arduino.h>
#include <M5Unified.h>
#include "lib/storage_manager/storage_manager.h"

StorageManager storage;

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("\n=== SD Card Basic Example ===\n");
    
    // Initialize M5Unified (required for Cardputer)
    auto cfg = M5.config();
    M5.begin(cfg);
    
    // Initialize SD card
    Serial.println("Initializing SD card...");
    if (!storage.begin()) {
        Serial.println("ERROR: SD card initialization failed!");
        Serial.println("Please check:");
        Serial.println("  - SD card is inserted");
        Serial.println("  - SD card is formatted as FAT32");
        Serial.println("  - SD card is not damaged");
        return;
    }
    
    Serial.println("SD card initialized successfully!\n");
    
    // Display card information
    Serial.println("=== SD Card Information ===");
    Serial.println("Type: " + storage.getCardType());
    Serial.printf("Total space: %llu MB\n", storage.getTotalSpace() / (1024 * 1024));
    Serial.printf("Used space: %llu MB\n", storage.getUsedSpace() / (1024 * 1024));
    Serial.printf("Free space: %llu MB\n\n", storage.getFreeSpace() / (1024 * 1024));
    
    // Create directories
    Serial.println("=== Creating Directories ===");
    storage.createDir("/examples");
    storage.createDir("/examples/data");
    Serial.println("Directories created\n");
    
    // Write a file
    Serial.println("=== Writing File ===");
    String content = "Hello from M5Stack Cardputer!\n";
    content += "This is a test file.\n";
    content += "Timestamp: " + String(millis()) + " ms\n";
    
    if (storage.writeFile("/examples/data/test.txt", content.c_str())) {
        Serial.println("File written successfully");
    } else {
        Serial.println("ERROR: Failed to write file");
        Serial.println("Error: " + storage.getLastError());
    }
    Serial.println();
    
    // Read the file
    Serial.println("=== Reading File ===");
    String readContent;
    if (storage.readFile("/examples/data/test.txt", readContent)) {
        Serial.println("File contents:");
        Serial.println(readContent);
    } else {
        Serial.println("ERROR: Failed to read file");
    }
    Serial.println();
    
    // Check file size
    Serial.println("=== File Information ===");
    size_t fileSize = storage.getFileSize("/examples/data/test.txt");
    Serial.printf("File size: %d bytes\n\n", fileSize);
    
    // Append to file
    Serial.println("=== Appending to File ===");
    String appendContent = "Additional line added by append\n";
    if (storage.appendFile("/examples/data/test.txt", appendContent.c_str())) {
        Serial.println("Content appended successfully");
    }
    Serial.println();
    
    // Read again to show appended content
    Serial.println("=== Reading Updated File ===");
    if (storage.readFile("/examples/data/test.txt", readContent)) {
        Serial.println("Updated file contents:");
        Serial.println(readContent);
    }
    Serial.println();
    
    // List directory
    Serial.println("=== Directory Listing ===");
    storage.listDir("/examples/data", false);
    Serial.println();
    
    // Check if file exists
    Serial.println("=== File Existence Check ===");
    if (storage.fileExists("/examples/data/test.txt")) {
        Serial.println("File exists: /examples/data/test.txt");
    }
    if (!storage.fileExists("/examples/data/missing.txt")) {
        Serial.println("File does not exist: /examples/data/missing.txt");
    }
    Serial.println();
    
    Serial.println("=== Example Complete ===");
    Serial.println("Check your SD card - you should see:");
    Serial.println("  /examples/data/test.txt");
}

void loop() {
    delay(1000);
}
