#include "storage_manager.h"

// Constants for size conversions
const uint64_t MB_DIVISOR = 1024 * 1024;

StorageManager::StorageManager() : _initialized(false), _cardPresent(false), _lastError("") {
}

bool StorageManager::begin() {
    Serial.println("[Storage] Initializing SD card...");
    
    // M5Stack Cardputer uses default SPI pins for SD card
    // The M5Unified library handles the pin configuration
    if (!SD.begin()) {
        setError("SD card mount failed");
        Serial.println("[Storage] ERROR: SD card mount failed");
        _initialized = false;
        _cardPresent = false;
        return false;
    }
    
    uint8_t cardType = SD.cardType();
    if (cardType == CARD_NONE) {
        setError("No SD card attached");
        Serial.println("[Storage] ERROR: No SD card attached");
        _initialized = false;
        _cardPresent = false;
        return false;
    }
    
    _initialized = true;
    _cardPresent = true;
    
    // Print card information
    Serial.print("[Storage] SD card type: ");
    if (cardType == CARD_MMC) {
        Serial.println("MMC");
    } else if (cardType == CARD_SD) {
        Serial.println("SDSC");
    } else if (cardType == CARD_SDHC) {
        Serial.println("SDHC");
    } else {
        Serial.println("UNKNOWN");
    }
    
    uint64_t cardSize = SD.cardSize() / MB_DIVISOR;
    Serial.printf("[Storage] SD card size: %llu MB\n", cardSize);
    Serial.printf("[Storage] Total space: %llu MB\n", SD.totalBytes() / MB_DIVISOR);
    Serial.printf("[Storage] Used space: %llu MB\n", SD.usedBytes() / MB_DIVISOR);
    Serial.printf("[Storage] Free space: %llu MB\n", (SD.totalBytes() - SD.usedBytes()) / MB_DIVISOR);
    
    Serial.println("[Storage] SD card initialized successfully");
    return true;
}

bool StorageManager::isReady() {
    return _initialized && _cardPresent;
}

String StorageManager::getCardType() {
    if (!_initialized) {
        return "Not Initialized";
    }
    
    uint8_t cardType = SD.cardType();
    if (cardType == CARD_NONE) {
        return "No Card";
    } else if (cardType == CARD_MMC) {
        return "MMC";
    } else if (cardType == CARD_SD) {
        return "SDSC";
    } else if (cardType == CARD_SDHC) {
        return "SDHC";
    } else {
        return "UNKNOWN";
    }
}

uint64_t StorageManager::getTotalSpace() {
    if (!isReady()) {
        return 0;
    }
    return SD.totalBytes();
}

uint64_t StorageManager::getFreeSpace() {
    if (!isReady()) {
        return 0;
    }
    return SD.totalBytes() - SD.usedBytes();
}

uint64_t StorageManager::getUsedSpace() {
    if (!isReady()) {
        return 0;
    }
    return SD.usedBytes();
}

bool StorageManager::writeFile(const char* path, const char* data) {
    if (!isReady()) {
        setError("SD card not ready");
        return false;
    }
    
    Serial.printf("[Storage] Writing file: %s\n", path);
    
    File file = SD.open(path, FILE_WRITE);
    if (!file) {
        setError(String("Failed to open file for writing: ") + path);
        Serial.printf("[Storage] ERROR: Failed to open file: %s\n", path);
        return false;
    }
    
    size_t bytesWritten = file.print(data);
    file.close();
    
    if (bytesWritten == 0) {
        setError("Failed to write data to file");
        Serial.println("[Storage] ERROR: Failed to write data");
        return false;
    }
    
    Serial.printf("[Storage] File written: %s (%d bytes)\n", path, bytesWritten);
    return true;
}

bool StorageManager::appendFile(const char* path, const char* data) {
    if (!isReady()) {
        setError("SD card not ready");
        return false;
    }
    
    Serial.printf("[Storage] Appending to file: %s\n", path);
    
    File file = SD.open(path, FILE_APPEND);
    if (!file) {
        setError(String("Failed to open file for appending: ") + path);
        Serial.printf("[Storage] ERROR: Failed to open file: %s\n", path);
        return false;
    }
    
    size_t bytesWritten = file.print(data);
    file.close();
    
    if (bytesWritten == 0) {
        setError("Failed to append data to file");
        Serial.println("[Storage] ERROR: Failed to append data");
        return false;
    }
    
    Serial.printf("[Storage] Data appended: %s (%d bytes)\n", path, bytesWritten);
    return true;
}

bool StorageManager::readFile(const char* path, String& data) {
    if (!isReady()) {
        setError("SD card not ready");
        return false;
    }
    
    Serial.printf("[Storage] Reading file: %s\n", path);
    
    File file = SD.open(path, FILE_READ);
    if (!file) {
        setError(String("Failed to open file for reading: ") + path);
        Serial.printf("[Storage] ERROR: Failed to open file: %s\n", path);
        return false;
    }
    
    // Get file size and pre-allocate string
    size_t fileSize = file.size();
    if (fileSize > 0) {
        data.reserve(fileSize + 1);
    }
    
    // Read in chunks for better performance
    const size_t CHUNK_SIZE = 512;
    char buffer[CHUNK_SIZE];
    data = "";
    
    while (file.available()) {
        size_t bytesToRead = min((size_t)file.available(), CHUNK_SIZE);
        size_t bytesRead = file.read((uint8_t*)buffer, bytesToRead);
        if (bytesRead > 0) {
            data.concat(buffer, bytesRead);
        } else {
            break;
        }
    }
    file.close();
    
    Serial.printf("[Storage] File read: %s (%d bytes)\n", path, data.length());
    return true;
}

bool StorageManager::deleteFile(const char* path) {
    if (!isReady()) {
        setError("SD card not ready");
        return false;
    }
    
    Serial.printf("[Storage] Deleting file: %s\n", path);
    
    if (!SD.exists(path)) {
        setError("File does not exist");
        Serial.printf("[Storage] ERROR: File does not exist: %s\n", path);
        return false;
    }
    
    if (!SD.remove(path)) {
        setError(String("Failed to delete file: ") + path);
        Serial.printf("[Storage] ERROR: Failed to delete file: %s\n", path);
        return false;
    }
    
    Serial.printf("[Storage] File deleted: %s\n", path);
    return true;
}

bool StorageManager::fileExists(const char* path) {
    if (!isReady()) {
        return false;
    }
    return SD.exists(path);
}

size_t StorageManager::getFileSize(const char* path) {
    if (!isReady()) {
        return 0;
    }
    
    File file = SD.open(path, FILE_READ);
    if (!file) {
        return 0;
    }
    
    size_t size = file.size();
    file.close();
    return size;
}

bool StorageManager::createDir(const char* path) {
    if (!isReady()) {
        setError("SD card not ready");
        return false;
    }
    
    Serial.printf("[Storage] Creating directory: %s\n", path);
    
    // Check if directory already exists
    if (SD.exists(path)) {
        Serial.printf("[Storage] Directory already exists: %s\n", path);
        return true;
    }
    
    if (!SD.mkdir(path)) {
        setError(String("Failed to create directory: ") + path);
        Serial.printf("[Storage] ERROR: Failed to create directory: %s\n", path);
        return false;
    }
    
    Serial.printf("[Storage] Directory created: %s\n", path);
    return true;
}

bool StorageManager::removeDir(const char* path) {
    if (!isReady()) {
        setError("SD card not ready");
        return false;
    }
    
    Serial.printf("[Storage] Removing directory: %s\n", path);
    
    if (!SD.rmdir(path)) {
        setError(String("Failed to remove directory: ") + path);
        Serial.printf("[Storage] ERROR: Failed to remove directory: %s\n", path);
        return false;
    }
    
    Serial.printf("[Storage] Directory removed: %s\n", path);
    return true;
}

bool StorageManager::listDir(const char* path, bool recursive) {
    if (!isReady()) {
        setError("SD card not ready");
        return false;
    }
    
    Serial.printf("[Storage] Listing directory: %s\n", path);
    
    File root = SD.open(path);
    if (!root) {
        setError(String("Failed to open directory: ") + path);
        Serial.printf("[Storage] ERROR: Failed to open directory: %s\n", path);
        return false;
    }
    
    if (!root.isDirectory()) {
        setError("Not a directory");
        Serial.println("[Storage] ERROR: Not a directory");
        root.close();
        return false;
    }
    
    if (recursive) {
        listDirRecursive(root, 0);
    } else {
        File file = root.openNextFile();
        while (file) {
            if (file.isDirectory()) {
                Serial.print("  DIR : ");
                Serial.println(file.name());
            } else {
                Serial.print("  FILE: ");
                Serial.print(file.name());
                Serial.print("  SIZE: ");
                Serial.println(file.size());
            }
            file = root.openNextFile();
        }
    }
    
    root.close();
    return true;
}

void StorageManager::listDirRecursive(File dir, int numTabs) {
    // Limit recursion depth to prevent stack overflow
    if (numTabs > MAX_RECURSION_DEPTH) {
        Serial.println("  [Max depth reached]");
        return;
    }
    
    while (true) {
        File entry = dir.openNextFile();
        if (!entry) {
            break;
        }
        
        for (uint8_t i = 0; i < numTabs; i++) {
            Serial.print('\t');
        }
        
        Serial.print(entry.name());
        if (entry.isDirectory()) {
            Serial.println("/");
            listDirRecursive(entry, numTabs + 1);
        } else {
            Serial.print("\t\t");
            Serial.println(entry.size());
        }
        entry.close();
    }
}

bool StorageManager::createDefaultDirectories() {
    if (!isReady()) {
        setError("SD card not ready");
        return false;
    }
    
    Serial.println("[Storage] Creating default directory structure...");
    
    bool success = true;
    
    // Create main directories
    success &= createDir("/config");
    success &= createDir("/data");
    success &= createDir("/data/captures");
    success &= createDir("/data/recordings");
    success &= createDir("/logs");
    success &= createDir("/downloads");
    
    if (success) {
        Serial.println("[Storage] Default directory structure created successfully");
    } else {
        Serial.println("[Storage] WARNING: Some directories failed to create");
    }
    
    return success;
}

String StorageManager::getLastError() {
    return _lastError;
}

void StorageManager::end() {
    if (_initialized) {
        SD.end();
        _initialized = false;
        _cardPresent = false;
        Serial.println("[Storage] SD card unmounted");
    }
}

void StorageManager::setError(const String& error) {
    _lastError = error;
}
