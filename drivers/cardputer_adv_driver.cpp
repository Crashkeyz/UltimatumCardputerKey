#include <Arduino.h>
#include <M5Unified.h>
#include <SD.h>
#include <FS.h>
#include <SPI.h>
#include "cardputer_adv_driver.h"

// SD Card configuration for M5Stack Cardputer ADV
#define SD_CS_PIN GPIO_NUM_4
#define SD_SCK_PIN GPIO_NUM_40
#define SD_MISO_PIN GPIO_NUM_39
#define SD_MOSI_PIN GPIO_NUM_14
#define SD_SPI_FREQ 25000000  // 25MHz

bool initialize_sd_card() {
    Serial.println("Initializing SD card...");
    
    // Create dedicated SPI bus for SD card (HSPI)
    SPIClass sdcardSPI(HSPI);
    sdcardSPI.begin(SD_SCK_PIN, SD_MISO_PIN, SD_MOSI_PIN, SD_CS_PIN);
    
    // Try to mount SD card with dedicated SPI bus
    if (!SD.begin(SD_CS_PIN, sdcardSPI, SD_SPI_FREQ)) {
        Serial.println("❌ SD Card mount failed or not present");
        Serial.println("Note: Insert SD card and restart if you want to use external storage");
        return false;
    }
    
    uint8_t cardType = SD.cardType();
    if (cardType == CARD_NONE) {
        Serial.println("❌ No SD card attached");
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
    
    // Create folder structure on SD card
    const char* folders[] = {"/data", "/keys", "/logs", "/captures", "/loot", "/firmware"};
    for (int i = 0; i < 6; i++) {
        if (!SD.exists(folders[i])) {
            if (SD.mkdir(folders[i])) {
                Serial.printf("✅ Created %s directory on SD card\n", folders[i]);
            } else {
                Serial.printf("⚠️  Failed to create %s directory\n", folders[i]);
            }
        } else {
            Serial.printf("✓ %s directory already exists\n", folders[i]);
        }
    }
    
    Serial.println("✅ SD CARD READY");
    return true;
}

void initialize_driver() {
    Serial.println("Initializing Cardputer ADV driver...");
    
    // Initialize SD card for external storage
    // This reduces flash memory usage by storing data externally
    // NOTE: M5.begin() must be called before this function
    if (initialize_sd_card()) {
        Serial.println("✅ SD card available for data storage");
        Serial.println("Use SD card to store large files and reduce flash memory usage");
    } else {
        Serial.println("⚠️  SD card not available - using internal flash only");
        Serial.println("To use SD card: Insert card and restart device");
    }
    
    Serial.println("✅ Driver initialized successfully");
}

void driver_loop() {
    // Placeholder for menu system and continuous operation
    // This can be expanded to handle menu navigation, SD card operations, etc.
}

