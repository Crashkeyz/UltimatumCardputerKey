#ifndef SDMANAGER_H
#define SDMANAGER_H

#include <Arduino.h>
#include <SD.h>
#include <SPI.h>

class SDManager {
public:
    static SDManager& getInstance();
    
    bool initialize();
    bool isAvailable() const { return available; }
    
    // File operations with error handling
    String readFile(const String& path, bool& success);
    bool writeFile(const String& path, const String& content);
    bool appendFile(const String& path, const String& content);
    bool fileExists(const String& path);
    
    // Specific file loaders for common patterns
    String loadAPIKey(const String& filename = "/ai_key.txt");
    String loadVoiceKey(const String& filename = "/voice_key.txt");
    
private:
    SDManager();
    SDManager(const SDManager&) = delete;
    SDManager& operator=(const SDManager&) = delete;
    
    bool available;
    bool tryInitialize(int maxRetries = 3);
};

#endif // SDMANAGER_H
