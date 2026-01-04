#ifndef WIFIMANAGER_H
#define WIFIMANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include "SDManager.h"

struct SavedNetwork {
    String ssid;
    String password;
    int lastUsed;
};

class WiFiManager {
public:
    static WiFiManager& getInstance();
    
    void initialize();
    
    // Network scanning
    void startScan();
    bool isScanComplete(int& resultCount);
    String getSSID(int index);
    int getRSSI(int index);
    
    // Connection management
    bool connect(const String& ssid, const String& password);
    bool isConnected() const;
    String getConnectedSSID() const;
    String getLocalIP() const;
    int getSignalStrength() const;
    
    // Saved networks
    bool loadSavedNetworks();
    bool saveSavedNetworks();
    bool saveNetwork(const String& ssid, const String& password);
    bool autoConnect();
    int getSavedNetworkCount() const { return savedNetworkCount; }
    const SavedNetwork& getSavedNetwork(int index) const;
    
private:
    WiFiManager();
    WiFiManager(const WiFiManager&) = delete;
    WiFiManager& operator=(const WiFiManager&) = delete;
    
    SavedNetwork savedNetworks[10];
    int savedNetworkCount;
    bool autoConnected;
};

#endif // WIFIMANAGER_H
