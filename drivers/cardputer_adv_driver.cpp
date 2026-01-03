#include <Arduino.h>
#include <M5Unified.h>
#include <SD.h>
#include <FS.h>
#include <SPI.h>
#include "cardputer_adv_driver.h"

// SD Card configuration for M5Stack Cardputer
#define SD_CS_PIN GPIO_NUM_4
#define SD_SPI_FREQ 25000000  // 25MHz

bool initialize_sd_card() {
    Serial.println(F("Initializing SD card..."));
    
    // Note: This function should be called after M5.begin() in setup()
    // M5Unified configures the SPI pins automatically for M5Stack devices
    
    // Try to mount SD card with explicit CS pin and frequency
    if (!SD.begin(SD_CS_PIN, SPI, SD_SPI_FREQ)) {
        Serial.println(F("SD Card mount failed"));
        Serial.println(F("Insert SD card and restart"));
        return false;
    }
    
    uint8_t cardType = SD.cardType();
    if (cardType == CARD_NONE) {
        Serial.println(F("No SD card attached"));
        return false;
    }
    
    // Print SD card info
    Serial.print(F("SD Card Type: "));
    if (cardType == CARD_MMC) {
        Serial.println(F("MMC"));
    } else if (cardType == CARD_SD) {
        Serial.println(F("SDSC"));
    } else if (cardType == CARD_SDHC) {
        Serial.println(F("SDHC"));
    } else {
        Serial.println(F("UNKNOWN"));
    }
    
    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("SD Size: %lluMB\n", cardSize);
    Serial.printf("Total: %lluMB\n", SD.totalBytes() / (1024 * 1024));
    Serial.printf("Used: %lluMB\n", SD.usedBytes() / (1024 * 1024));
    
    // Create data directory if it doesn't exist
    if (!SD.exists("/data")) {
        SD.mkdir("/data");
        Serial.println(F("Created /data directory"));
    }
    
    Serial.println(F("SD card initialized"));
    return true;
}

void initialize_driver() {
    Serial.println(F("Initializing driver..."));
    
    // Initialize SD card for external storage
    // This reduces flash memory usage by storing data externally
    // NOTE: M5.begin() must be called before this function
    if (initialize_sd_card()) {
        Serial.println(F("SD available for storage"));
        Serial.println(F("Store large files on SD"));
    } else {
        Serial.println(F("SD not available"));
        Serial.println(F("Insert SD card and restart"));
    }
    
    // Add your custom driver initialization here
    // Examples:
    // - WiFi setup
    // - Custom peripheral initialization
    
    Serial.println(F("Driver initialized"));
}
