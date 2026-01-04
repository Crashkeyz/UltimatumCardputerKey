#include "WiFiManager.h"

WiFiManager& WiFiManager::getInstance() {
    static WiFiManager instance;
    return instance;
}

WiFiManager::WiFiManager() : savedNetworkCount(0), autoConnected(false) {
}

void WiFiManager::initialize() {
    WiFi.mode(WIFI_STA);
    Serial.println("WiFi: Initialized in station mode");
}

void WiFiManager::startScan() {
    WiFi.scanDelete();
    WiFi.scanNetworks(true);  // Async scan
    Serial.println("WiFi: Scan started");
}

bool WiFiManager::isScanComplete(int& resultCount) {
    int result = WiFi.scanComplete();
    if (result >= 0) {
        resultCount = result;
        return true;
    }
    return false;
}

String WiFiManager::getSSID(int index) {
    return WiFi.SSID(index);
}

int WiFiManager::getRSSI(int index) {
    return WiFi.RSSI(index);
}

bool WiFiManager::connect(const String& ssid, const String& password) {
    Serial.printf("WiFi: Connecting to %s...\n", ssid.c_str());
    
    WiFi.begin(ssid.c_str(), password.c_str());
    
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        Serial.print(".");
        attempts++;
    }
    Serial.println();
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.printf("WiFi: Connected to %s\n", ssid.c_str());
        Serial.printf("IP: %s\n", WiFi.localIP().toString().c_str());
        return true;
    } else {
        Serial.println("WiFi: Connection failed");
        return false;
    }
}

bool WiFiManager::isConnected() const {
    return WiFi.status() == WL_CONNECTED;
}

String WiFiManager::getConnectedSSID() const {
    return WiFi.SSID();
}

String WiFiManager::getLocalIP() const {
    return WiFi.localIP().toString();
}

int WiFiManager::getSignalStrength() const {
    return WiFi.RSSI();
}

bool WiFiManager::loadSavedNetworks() {
    SDManager& sd = SDManager::getInstance();
    if (!sd.isAvailable()) {
        Serial.println("WiFi: Can't load networks - SD not available");
        return false;
    }
    
    bool success;
    String content = sd.readFile("/wifi_networks.txt", success);
    if (!success) {
        Serial.println("WiFi: No saved networks file");
        return false;
    }
    
    savedNetworkCount = 0;
    int startPos = 0;
    
    while (savedNetworkCount < 10) {
        int endPos = content.indexOf('\n', startPos);
        if (endPos == -1) {
            endPos = content.length();
        }
        
        String line = content.substring(startPos, endPos);
        line.trim();
        
        if (line.length() > 0) {
            int sepPos = line.indexOf('|');
            if (sepPos > 0) {
                savedNetworks[savedNetworkCount].ssid = line.substring(0, sepPos);
                
                int sepPos2 = line.indexOf('|', sepPos + 1);
                if (sepPos2 > sepPos) {
                    savedNetworks[savedNetworkCount].password = line.substring(sepPos + 1, sepPos2);
                    savedNetworks[savedNetworkCount].lastUsed = line.substring(sepPos2 + 1).toInt();
                } else {
                    savedNetworks[savedNetworkCount].password = line.substring(sepPos + 1);
                    savedNetworks[savedNetworkCount].lastUsed = 0;
                }
                
                savedNetworkCount++;
            }
        }
        
        if (endPos >= content.length()) {
            break;
        }
        startPos = endPos + 1;
    }
    
    Serial.printf("WiFi: Loaded %d saved networks\n", savedNetworkCount);
    return savedNetworkCount > 0;
}

bool WiFiManager::saveSavedNetworks() {
    SDManager& sd = SDManager::getInstance();
    if (!sd.isAvailable()) {
        Serial.println("WiFi: Can't save networks - SD not available");
        return false;
    }
    
    String content = "";
    for (int i = 0; i < savedNetworkCount; i++) {
        content += savedNetworks[i].ssid + "|";
        content += savedNetworks[i].password + "|";
        content += String(savedNetworks[i].lastUsed) + "\n";
    }
    
    bool success = sd.writeFile("/wifi_networks.txt", content);
    if (success) {
        Serial.printf("WiFi: Saved %d networks\n", savedNetworkCount);
    }
    
    return success;
}

bool WiFiManager::saveNetwork(const String& ssid, const String& password) {
    // Check if network already exists
    for (int i = 0; i < savedNetworkCount; i++) {
        if (savedNetworks[i].ssid == ssid) {
            savedNetworks[i].password = password;
            savedNetworks[i].lastUsed = millis();
            return saveSavedNetworks();
        }
    }
    
    // Add new network if space available
    if (savedNetworkCount < 10) {
        savedNetworks[savedNetworkCount].ssid = ssid;
        savedNetworks[savedNetworkCount].password = password;
        savedNetworks[savedNetworkCount].lastUsed = millis();
        savedNetworkCount++;
        return saveSavedNetworks();
    }
    
    // Replace oldest network
    int oldestIndex = 0;
    int oldestTime = savedNetworks[0].lastUsed;
    for (int i = 1; i < savedNetworkCount; i++) {
        if (savedNetworks[i].lastUsed < oldestTime) {
            oldestTime = savedNetworks[i].lastUsed;
            oldestIndex = i;
        }
    }
    
    savedNetworks[oldestIndex].ssid = ssid;
    savedNetworks[oldestIndex].password = password;
    savedNetworks[oldestIndex].lastUsed = millis();
    
    return saveSavedNetworks();
}

bool WiFiManager::autoConnect() {
    if (savedNetworkCount == 0) {
        Serial.println("WiFi: No saved networks for auto-connect");
        return false;
    }
    
    // Sort by last used (most recent first)
    for (int i = 0; i < savedNetworkCount - 1; i++) {
        for (int j = i + 1; j < savedNetworkCount; j++) {
            if (savedNetworks[j].lastUsed > savedNetworks[i].lastUsed) {
                SavedNetwork temp = savedNetworks[i];
                savedNetworks[i] = savedNetworks[j];
                savedNetworks[j] = temp;
            }
        }
    }
    
    // Try each network
    for (int i = 0; i < savedNetworkCount; i++) {
        Serial.printf("WiFi: Trying saved network %s...\n", savedNetworks[i].ssid.c_str());
        if (connect(savedNetworks[i].ssid, savedNetworks[i].password)) {
            savedNetworks[i].lastUsed = millis();
            saveSavedNetworks();
            autoConnected = true;
            return true;
        }
    }
    
    Serial.println("WiFi: Auto-connect failed for all saved networks");
    return false;
}

const SavedNetwork& WiFiManager::getSavedNetwork(int index) const {
    static SavedNetwork empty;
    if (index >= 0 && index < savedNetworkCount) {
        return savedNetworks[index];
    }
    return empty;
}
