// =====================================================
// CARDPUTER ADV - Refactored Architecture
// Professional firmware with manager-based design
// =====================================================

#include <Arduino.h>
#include <M5Cardputer.h>
#include "cardputer_adv_driver.h"
#include <utility/Adafruit_TCA8418/Adafruit_TCA8418.h>
#include <Wire.h>

// Manager classes
#include "managers/SDManager.h"
#include "managers/WiFiManager.h"
#include "managers/AIManager.h"

// Utility classes  
#include "utils/DisplayUtils.h"
#include "utils/KeyboardUtils.h"

// Screens
#include "screens/BaseScreen.h"
#include "screens/MenuScreen.h"
#include "screens/AIScreen.h"

// Legacy includes for non-refactored screens
#include "simple_security_check.h"

#define TFT_BL 38
#define FIRMWARE_VERSION "3.2.0-refactor"

// Global objects
Adafruit_TCA8418 tca8418;
SecurityAuditor auditor;

// State management
AppState currentState = MAIN_MENU;
MenuScreen* menuScreen = nullptr;
AIScreen* aiScreen = nullptr;

// WiFi Scanner state (not yet fully refactored)
String scannedSSIDs[10];
int scannedRSSI[10];
int scannedCount = 0;
int selectedNetwork = -1;
String wifiPassword = "";
bool isScanning = false;
int scanStartTime = 0;

// Code Writer state (not yet fully refactored)
String codeBuffer = "";
String codePrompt = "";
String codeFilename = "code.cpp";
bool codeMode = false;
int codeScroll = 0;

// Forward declarations
void handleKeyPress(char key);
void wifiScanScreen();
void securityAuditScreen();
void runSecurityAudit();
void aboutScreen();
void codeWriterScreen();
void generateCode();
void saveCodeToSD();
void showThunderCastleSplash();
void playThunderCastleSound();
void showDevilScreen();
void playSinisterAudio();
void showWiFiConnectScreen();
void connectToWiFi();
void saveWiFiCredentials();
void backgroundAILearning();
void handleSerialCommands();

// Background learning
unsigned long lastLearningCycle = 0;
int learningExperiment = 0;
String lastDiscovery = "";

void setup() {
    auto cfg = M5.config();
    cfg.output_power = true;
    cfg.internal_imu = false;
    cfg.internal_rtc = false;
    cfg.internal_spk = true;
    cfg.internal_mic = true;
    
    M5Cardputer.begin(cfg, false);
    M5Cardputer.Display.setRotation(1);
    M5Cardputer.Display.fillScreen(TFT_BLACK);
    
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);
    
    Wire.begin(2, 1, 100000);
    KeyboardUtils::initialize(tca8418);
    
    Serial.begin(115200);
    delay(500);
    while (!Serial && millis() < 3000);
    Serial.println("\n\n=== CARDPUTER ADV REFACTORED ===");
    Serial.println("Firmware: " FIRMWARE_VERSION);
    Serial.println("Initializing...");
    Serial.flush();
    
    // Initialize managers
    SDManager& sd = SDManager::getInstance();
    sd.initialize();
    
    WiFiManager& wifi = WiFiManager::getInstance();
    wifi.initialize();
    
    AIManager& ai = AIManager::getInstance();
    ai.initialize();
    
    // Load and auto-connect WiFi
    if (sd.isAvailable()) {
        wifi.loadSavedNetworks();
        if (wifi.getSavedNetworkCount() > 0) {
            Serial.println("WiFi: Auto-connecting...");
            wifi.autoConnect();
        }
    }
    
    // Splash screens
    showThunderCastleSplash();
    playThunderCastleSound();
    delay(2000);
    
    // Loading animation
    M5Cardputer.Display.fillScreen(TFT_BLACK);
    M5Cardputer.Display.setTextSize(3);
    M5Cardputer.Display.setTextColor(TFT_CYAN, TFT_BLACK);
    M5Cardputer.Display.setCursor(20, 50);
    M5Cardputer.Display.print("LOADING");
    
    for (int i = 0; i < 5; i++) {
        M5Cardputer.Display.print(".");
        M5Cardputer.Speaker.tone(800 - i*50, 100);
        delay(400);
    }
    M5Cardputer.Speaker.stop();
    delay(500);
    
    showDevilScreen();
    playSinisterAudio();
    delay(2500);
    
    // Initialize screens
    menuScreen = new MenuScreen();
    aiScreen = new AIScreen();
    
    menuScreen->draw();
    
    Serial.println("Setup complete!");
}

void loop() {
    M5Cardputer.update();
    
    // Real-time WiFi scan updates
    if (isScanning && currentState == WIFI_SCANNER) {
        WiFiManager& wifi = WiFiManager::getInstance();
        int scanResult;
        if (wifi.isScanComplete(scanResult)) {
            scannedCount = (scanResult < 10) ? scanResult : 10;
            for (int i = 0; i < scannedCount; i++) {
                scannedSSIDs[i] = wifi.getSSID(i);
                scannedRSSI[i] = wifi.getRSSI(i);
            }
            isScanning = false;
            wifiScanScreen();
        } else if (millis() - scanStartTime > 100) {
            scanStartTime = millis();
            wifiScanScreen();
        }
    }
    
    // Handle Serial commands
    if (Serial.available() > 0) {
        handleSerialCommands();
    }
    
    // Background AI learning every 30 seconds
    if (millis() - lastLearningCycle > 30000) {
        backgroundAILearning();
        lastLearningCycle = millis();
    }
    
    // Read keyboard
    char key = KeyboardUtils::readKey(tca8418);
    if (key != 0) {
        handleKeyPress(key);
    }
    
    // Handle button on GPIO0
    static bool lastBtn = HIGH;
    pinMode(0, INPUT_PULLUP);
    bool btnState = digitalRead(0);
    
    if (btnState == LOW && lastBtn == HIGH) {
        if (currentState == MAIN_MENU) handleKeyPress('\n');
    }
    lastBtn = btnState;
    
    delay(10);
}

void handleKeyPress(char key) {
    switch (currentState) {
        case MAIN_MENU:
            menuScreen->handleKey(key);
            if (menuScreen->getNextState() != MAIN_MENU) {
                currentState = menuScreen->getNextState();
                switch (currentState) {
                    case WIFI_SCANNER:
                        wifiScanScreen();
                        break;
                    case SECURITY_AUDIT:
                        securityAuditScreen();
                        break;
                    case AI_ASSISTANT:
                        aiScreen->draw();
                        break;
                    case CODE_WRITER:
                        codeWriterScreen();
                        break;
                    case ABOUT:
                        aboutScreen();
                        break;
                    default:
                        break;
                }
            }
            break;
            
        case AI_ASSISTANT:
            aiScreen->handleKey(key);
            if (aiScreen->getNextState() != AI_ASSISTANT) {
                currentState = aiScreen->getNextState();
                if (currentState == MAIN_MENU) {
                    menuScreen->draw();
                }
            }
            break;
            
        case WIFI_SCANNER:
            if (key == 27) {
                WiFiManager& wifi = WiFiManager::getInstance();
                isScanning = false;
                currentState = MAIN_MENU;
                menuScreen->draw();
            } else if (key == 's' || key == 'S') {
                WiFiManager& wifi = WiFiManager::getInstance();
                wifi.startScan();
                isScanning = true;
                scannedCount = 0;
                scanStartTime = millis();
                wifiScanScreen();
            } else if (key >= '1' && key <= '9') {
                int selection = key - '1';
                if (selection < scannedCount) {
                    selectedNetwork = selection;
                    wifiPassword = "";
                    
                    // Check saved networks
                    WiFiManager& wifi = WiFiManager::getInstance();
                    for (int i = 0; i < wifi.getSavedNetworkCount(); i++) {
                        const SavedNetwork& saved = wifi.getSavedNetwork(i);
                        if (scannedSSIDs[selectedNetwork] == saved.ssid) {
                            wifiPassword = saved.password;
                            connectToWiFi();
                            return;
                        }
                    }
                    
                    // New network - need password
                    currentState = WIFI_CONNECT;
                    showWiFiConnectScreen();
                }
            }
            break;
            
        case WIFI_CONNECT:
            if (key == 27) {
                wifiPassword = "";
                currentState = WIFI_SCANNER;
                wifiScanScreen();
            } else if (key == '\n') {
                if (wifiPassword.length() > 0) {
                    connectToWiFi();
                }
            } else if (key == '\b') {
                if (wifiPassword.length() > 0) {
                    wifiPassword.remove(wifiPassword.length() - 1);
                    showWiFiConnectScreen();
                }
            } else if (key >= 32 && key <= 126 && wifiPassword.length() < 64) {
                wifiPassword += key;
                showWiFiConnectScreen();
            }
            break;
            
        case CODE_WRITER:
            if (key == 27) {
                currentState = MAIN_MENU;
                codePrompt = "";
                codeBuffer = "";
                codeMode = false;
                menuScreen->draw();
            } else if (!codeMode) {
                if (key == '\n') {
                    if (codePrompt.length() > 0) {
                        generateCode();
                    }
                } else if (key == '\b') {
                    if (codePrompt.length() > 0) {
                        codePrompt.remove(codePrompt.length() - 1);
                        codeWriterScreen();
                    }
                } else if (key >= 32 && key <= 126 && codePrompt.length() < 200) {
                    codePrompt += key;
                    codeWriterScreen();
                }
            } else {
                if (key == '\n') {
                    codeBuffer += '\n';
                    codeWriterScreen();
                } else if (key == '\b') {
                    if (codeBuffer.length() > 0) {
                        codeBuffer.remove(codeBuffer.length() - 1);
                        codeWriterScreen();
                    }
                } else if (key == '\t') {
                    codeBuffer += "    ";
                    codeWriterScreen();
                } else if (key >= 32 && key <= 126) {
                    codeBuffer += key;
                    codeWriterScreen();
                } else if (key == 's' && KeyboardUtils::isCtrlPressed()) {
                    saveCodeToSD();
                }
            }
            break;
            
        case SECURITY_AUDIT:
            if (key == 27) {
                currentState = MAIN_MENU;
                menuScreen->draw();
            } else if (key == 's' || key == 'S') {
                runSecurityAudit();
            }
            break;
            
        case ABOUT:
            if (key == 27) {
                currentState = MAIN_MENU;
                menuScreen->draw();
            }
            break;
    }
}

// Remaining screen functions from original - will be refactored in next phase
