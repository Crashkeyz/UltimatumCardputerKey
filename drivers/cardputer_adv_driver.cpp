#include <Arduino.h>
#include <M5Unified.h>
#include <SD.h>
#include <FS.h>
#include <SPI.h>
#include "cardputer_adv_driver.h"

bool initialize_sd_card() {
    Serial.println("Initializing SD card...");
    
    // M5Unified handles SD card initialization for M5Stack devices
    // We need to initialize M5 before accessing SD card
    // The SD card is accessed through the standard SD library after M5.begin()
    
    // Try to mount SD card with default settings
    // M5Unified configures the pins automatically for M5Stack devices
    if (!SD.begin(GPIO_NUM_4, SPI, 25000000)) {
        Serial.println("SD Card mount failed or not present");
        Serial.println("Note: Insert SD card and restart if you want to use external storage");
        return false;
    }
    
    uint8_t cardType = SD.cardType();
    if (cardType == CARD_NONE) {
        Serial.println("No SD card attached");
        return false;
    }
    
    // Print SD card info
    Serial.print("SD Card Type: ");
    if (cardType == CARD_MMC) {
        Serial.println("MMC");
    } else if (cardType == CARD_SD) {
        Serial.println("SDSC");
    } else if (cardType == CARD_SDHC) {
        Serial.println("SDHC");
    } else {
        Serial.println("UNKNOWN");
    }
    
    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %lluMB\n", cardSize);
    Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
    Serial.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));
    
    // Create data directory if it doesn't exist
    if (!SD.exists("/data")) {
        SD.mkdir("/data");
        Serial.println("Created /data directory on SD card");
    }
    
    Serial.println("SD card initialized successfully");
    return true;
}

void initialize_driver() {
    Serial.println("Initializing Cardputer ADV driver...");
    
    // Initialize SD card for external storage
    // This reduces flash memory usage by storing data externally
    if (initialize_sd_card()) {
        Serial.println("SD card available for data storage");
        Serial.println("Use SD card to store large files and reduce flash memory usage");
    } else {
        Serial.println("SD card not available - using internal flash only");
        Serial.println("To use SD card: Insert card and restart device");
    }
    
    // Add your custom driver initialization here
    // Examples:
    // - WiFi setup
    // - Custom peripheral initialization
    
    Serial.println("Driver initialized successfully");
}
