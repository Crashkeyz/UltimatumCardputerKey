#include "SDManager.h"

SDManager& SDManager::getInstance() {
    static SDManager instance;
    return instance;
}

SDManager::SDManager() : available(false) {
}

bool SDManager::initialize() {
    Serial.println("SD: Initializing...");
    
    // Configure SPI for Cardputer ADV
    SPI.begin(40, 39, 14, 12);  // SCK, MISO, MOSI, CS
    delay(100);
    
    // Try initialization with retries
    available = tryInitialize(3);
    
    if (available) {
        Serial.println("SD Card: SUCCESS");
    } else {
        Serial.println("SD Card: FAILED");
        Serial.println("Insert FAT32 formatted SD card and reboot");
    }
    
    return available;
}

bool SDManager::tryInitialize(int maxRetries) {
    for (int retry = 0; retry < maxRetries; retry++) {
        Serial.printf("SD attempt %d/%d...\n", retry + 1, maxRetries);
        
        if (SD.begin(12, SPI, 25000000)) {
            // Verify SD card is actually accessible
            uint8_t cardType = SD.cardType();
            if (cardType != CARD_NONE) {
                Serial.printf("SD Card Type: %d\n", cardType);
                Serial.printf("SD Card Size: %lluMB\n", SD.cardSize() / (1024 * 1024));
                return true;
            } else {
                Serial.println("No SD card detected");
            }
        }
        
        if (retry < maxRetries - 1) {
            delay(500);
        }
    }
    
    return false;
}

String SDManager::readFile(const String& path, bool& success) {
    success = false;
    
    if (!available) {
        Serial.println("SD card not available");
        return "";
    }
    
    File file = SD.open(path);
    if (!file) {
        Serial.printf("Failed to open file: %s\n", path.c_str());
        return "";
    }
    
    String content = "";
    while (file.available()) {
        content += (char)file.read();
    }
    file.close();
    
    success = true;
    return content;
}

bool SDManager::writeFile(const String& path, const String& content) {
    if (!available) {
        Serial.println("SD card not available");
        return false;
    }
    
    File file = SD.open(path, FILE_WRITE);
    if (!file) {
        Serial.printf("Failed to open file for writing: %s\n", path.c_str());
        return false;
    }
    
    size_t written = file.print(content);
    file.close();
    
    bool success = (written == content.length());
    if (success) {
        Serial.printf("Wrote %d bytes to %s\n", written, path.c_str());
    } else {
        Serial.printf("Write failed: %s\n", path.c_str());
    }
    
    return success;
}

bool SDManager::appendFile(const String& path, const String& content) {
    if (!available) {
        Serial.println("SD card not available");
        return false;
    }
    
    File file = SD.open(path, FILE_APPEND);
    if (!file) {
        Serial.printf("Failed to open file for appending: %s\n", path.c_str());
        return false;
    }
    
    size_t written = file.print(content);
    file.close();
    
    return (written == content.length());
}

bool SDManager::fileExists(const String& path) {
    if (!available) {
        return false;
    }
    
    return SD.exists(path);
}

String SDManager::loadAPIKey(const String& filename) {
    bool success;
    String key = readFile(filename, success);
    
    if (success) {
        key.trim();
        Serial.println("AI Key: Loaded");
        return key;
    } else {
        Serial.printf("AI Key: Missing (create %s on SD)\n", filename.c_str());
        return "";
    }
}

String SDManager::loadVoiceKey(const String& filename) {
    bool success;
    String key = readFile(filename, success);
    
    if (success) {
        key.trim();
        Serial.println("Voice Key: Loaded");
        return key;
    } else {
        Serial.println("Voice Key: Not found");
        return "";
    }
}
