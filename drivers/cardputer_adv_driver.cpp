#include <Arduino.h>
#include <M5Unified.h>
#include <SD.h>
#include <FS.h>
#include <SPI.h>
#include "cardputer_adv_driver.h"

// SD Card pins for M5Stack Cardputer
// The Cardputer uses the built-in SD card slot
#define SD_SPI_SCK_PIN  36
#define SD_SPI_MISO_PIN 35
#define SD_SPI_MOSI_PIN 37
#define SD_SPI_CS_PIN   4

bool initialize_sd_card() {
    Serial.println("Initializing SD card...");
    
    // Initialize SPI for SD card
    SPI.begin(SD_SPI_SCK_PIN, SD_SPI_MISO_PIN, SD_SPI_MOSI_PIN, SD_SPI_CS_PIN);
    
    // Try to mount SD card
    if (!SD.begin(SD_SPI_CS_PIN, SPI, 25000000)) {
        Serial.println("SD Card mount failed or not present");
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
    if (initialize_sd_card()) {
        Serial.println("SD card available for data storage");
        Serial.println("Use SD card to store large files and reduce flash memory usage");
    } else {
        Serial.println("SD card not available - using internal flash only");
    }
    
    // Add your custom driver initialization here
    // Examples:
    // - WiFi setup
    // - Custom peripheral initialization
    
    Serial.println("Driver initialized successfully");
}
