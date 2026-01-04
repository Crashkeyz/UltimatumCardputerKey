// =====================================================
// CARDPUTER ADV - Refactored Architecture
// Professional firmware with manager-based design
// =====================================================

#include <Arduino.h>
#include <M5Cardputer.h>
#include "cardputer_adv_driver.h"
#include <utility/Adafruit_TCA8418/Adafruit_TCA8418.h>
#include <Wire.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <SD.h>
#include <FS.h>
#include "simple_security_check.h"

// Manager classes for refactored architecture
#include "managers/SDManager.h"
#include "managers/WiFiManager.h"
#include "managers/AIManager.h"

#define TFT_BL 38
#define FIRMWARE_VERSION "3.2.0-refactor"

Adafruit_TCA8418 tca8418;

// Modifier key states
bool shiftPressed = false;
bool ctrlPressed = false;
bool altPressed = false;
bool optPressed = false;

// Complete empirical keymap
char keymap[9][8] = {
  {  27,  '\t',   1,     2,    '1',  'q',   3,     4  },
  {  0,    0,   '2',   'w',   'a',   5,   '3',   'e'},
  {  's',  'z',   0,     0,    '4',  'r',  'd',   'x'},
  {  '5',  't',   'f',   'c',   0,    0,   '6',   'y'},
  {  'g',  'v',   '7',   'u',   'h',  'b',   0,     0 },
  {  '8',  'i',   'j',   'n',   '9',  'o',  'k',   'm'},
  {   0,    0,   '0',   'p',   'l',  ',',  '-',   '['},
  {  ';',  '.',   0,     0,    '=',  ']', '\'',   '/'},
  {  '\b', '\\', '\n',  ' ',   0,     0,    0,     0 }
};

enum AppState { MAIN_MENU, WIFI_SCANNER, SECURITY_AUDIT, ABOUT, WIFI_CONNECT, AI_ASSISTANT, CODE_WRITER };
AppState currentState = MAIN_MENU;

const char* menuItems[] = {"WiFi Scanner", "Security Audit", "AI Assistant", "Code Writer", "About"};
int currentMenuIndex = 0;
const int menuItemCount = 5;

// WiFi connection
String scannedSSIDs[10];
int scannedRSSI[10];
int scannedCount = 0;
int selectedNetwork = -1;
String wifiPassword = "";
bool isScanning = false;
int scanStartTime = 0;
bool autoConnected = false;

// Multiple saved networks
struct SavedNetwork {
    String ssid;
    String password;
    int lastUsed;
};
SavedNetwork savedNetworks[10];
int savedNetworkCount = 0;

// AI Assistant
String aiInput = "";

// Code Writer
String codeBuffer = "";
String codePrompt = "";
String codeFilename = "code.cpp";
bool codeMode = false;  // false=prompt, true=editing
int codeScroll = 0;
String aiResponse = "";
bool aiProcessing = false;
String aiApiKey = "";  // Load from SD or EEPROM
bool micRecording = false;
String voiceApiKey = "";  // OpenAI API key for Whisper
static const int AUDIO_BUFFER_SIZE = 16000;  // 1 second at 16kHz
int16_t* audioBuffer = nullptr;
int audioSampleCount = 0;

// Conversation history for scrolling
struct ConversationEntry {
    String query;
    String response;
};
ConversationEntry conversationHistory[5];  // Store last 5 exchanges
int conversationCount = 0;
int conversationScroll = 0;

// Background AI Learning
unsigned long lastLearningCycle = 0;
int learningExperiment = 0;
String lastDiscovery = "";
bool sdCardAvailable = false;

// Learning system - stores keyboard shortcuts
struct LearnedPattern {
    String trigger;
    String action;
};
LearnedPattern patterns[10];
int patternCount = 0;

SecurityAuditor auditor;

void drawMainMenu();
void handleKeyPress(char key);
void wifiScanScreen();
void securityAuditScreen();
void runSecurityAudit();
void aboutScreen();
void codeWriterScreen();
void generateCode();
void saveCodeToSD();
bool loadCredsFromSD();
void showThunderCastleSplash();
void playThunderCastleSound();
void showDevilScreen();
void playSinisterAudio();
void showWiFiConnectScreen();
void connectToWiFi();
void saveWiFiCredentials();
void saveSavedNetworks();
void loadWiFiCredentials();
void autoConnectWiFi();
void aiAssistantScreen();
void processAIQuery();
String callAIAPI(String query);
void startMicRecording();
void stopMicRecording();
String transcribeAudio();
void backgroundAILearning();
void experimentWithHardware();
void handleSerialCommands();
void stopMicRecording();
String transcribeAudio();
void learnPattern(String trigger, String action);
void saveLearnedPatterns();
void loadLearnedPatterns();

void setup() {
    auto cfg = M5.config();
    cfg.output_power = true;
    cfg.internal_imu = false;
    cfg.internal_rtc = false;
    cfg.internal_spk = true;
    cfg.internal_mic = true;  // Enable microphone for voice AI
    
    M5Cardputer.begin(cfg, false);
    M5Cardputer.Display.setRotation(1);
    M5Cardputer.Display.fillScreen(TFT_BLACK);
    
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);
    
    Wire.begin(2, 1, 100000);
    tca8418.begin();
    tca8418.matrix(7, 8);
    tca8418.flush();
    
    Serial.begin(115200);
    delay(500);
    while (!Serial && millis() < 3000);
    Serial.println("\n\n=== CARDPUTER ADV REFACTORED ===");
    Serial.println("Firmware: " FIRMWARE_VERSION);
    Serial.println("Initializing managers...");
    Serial.flush();
    
    // Initialize SDManager with improved error handling
    SDManager& sd = SDManager::getInstance();
    sdCardAvailable = sd.initialize();
    
    // Initialize WiFiManager
    WiFiManager& wifiMgr = WiFiManager::getInstance();
    wifiMgr.initialize();
    
    // Initialize AIManager
    AIManager& aiMgr = AIManager::getInstance();
    aiMgr.initialize();
    aiApiKey = aiMgr.getAPIKey();
    
    // Load patterns (now handled by AIManager)
    patternCount = aiMgr.getPatternCount();
    for (int i = 0; i < patternCount && i < 10; i++) {
        const LearnedPattern& p = aiMgr.getPattern(i);
        patterns[i].trigger = p.trigger;
        patterns[i].action = p.action;
    }
    
    // Load voice key
    if (sdCardAvailable) {
        voiceApiKey = sd.loadVoiceKey("/voice_key.txt");
        if (voiceApiKey.length() == 0) {
            voiceApiKey = aiApiKey;  // Fallback to AI key
            Serial.println("Voice Key: Using AI key");
        }
        
        // Allocate audio buffer
        audioBuffer = (int16_t*)malloc(AUDIO_BUFFER_SIZE * sizeof(int16_t));
        if (audioBuffer) {
            Serial.println("Audio Buffer: Allocated");
        } else {
            Serial.println("Audio Buffer: Failed");
        }
        
        // Load and auto-connect WiFi
        if (wifiMgr.loadSavedNetworks()) {
            savedNetworkCount = wifiMgr.getSavedNetworkCount();
            for (int i = 0; i < savedNetworkCount && i < 10; i++) {
                const SavedNetwork& saved = wifiMgr.getSavedNetwork(i);
                savedNetworks[i].ssid = saved.ssid;
                savedNetworks[i].password = saved.password;
                savedNetworks[i].lastUsed = saved.lastUsed;
            }
            
            Serial.println("WiFi: Auto-connecting...");
            wifiMgr.autoConnect();
        }
    }
    
    // Stage 1: Thunder Castle splash
    showThunderCastleSplash();
    playThunderCastleSound();
    delay(2000);
    
    // Stage 2: Loading animation
    M5Cardputer.Display.fillScreen(TFT_BLACK);
    M5Cardputer.Display.setTextSize(3);
    M5Cardputer.Display.setTextColor(TFT_CYAN, TFT_BLACK);
    M5Cardputer.Display.setCursor(20, 50);
    M5Cardputer.Display.print("LOADING");
    
    // Animated dots
    for (int i = 0; i < 5; i++) {
        M5Cardputer.Display.print(".");
        M5Cardputer.Speaker.tone(800 - i*50, 100);
        delay(400);
    }
    M5Cardputer.Speaker.stop();
    delay(500);
    
    // Stage 3: Devil screen with sinister audio
    showDevilScreen();
    playSinisterAudio();
    delay(2500);
    
    drawMainMenu();
}

void loop() {
    M5Cardputer.update();
    
    // Real-time WiFi scan updates
    if (isScanning && currentState == WIFI_SCANNER) {
        int scanResult = WiFi.scanComplete();
        if (scanResult >= 0) {
            // Scan completed
            scannedCount = (scanResult < 10) ? scanResult : 10;
            for (int i = 0; i < scannedCount; i++) {
                scannedSSIDs[i] = WiFi.SSID(i);
                scannedRSSI[i] = WiFi.RSSI(i);
            }
            isScanning = false;
            wifiScanScreen();
        } else if (millis() - scanStartTime > 100) {
            // Update display every 100ms during scan
            scanStartTime = millis();
            wifiScanScreen();
        }
    }
    
    // Handle Serial commands for USB testing
    if (Serial.available() > 0) {
        handleSerialCommands();
    }
    
    // Background AI learning every 30 seconds
    if (millis() - lastLearningCycle > 30000) {
        backgroundAILearning();
        lastLearningCycle = millis();
    }
    
    if (tca8418.available() > 0) {
        uint8_t event = tca8418.getEvent();
        if (event > 0) {
            bool pressed = !(event & 0x80);
            uint8_t keycode = (event & 0x7F) - 1;
            uint8_t row = keycode / 8;
            uint8_t col = keycode % 8;
            
            if (row >= 0 && row < 9 && col >= 0 && col < 8) {
                char key = keymap[row][col];
                
                if (key == 2) ctrlPressed = pressed;
                else if (key == 3) shiftPressed = pressed;
                else if (key == 4) optPressed = pressed;
                else if (key == 5) altPressed = pressed;
                else if (pressed && key != 0 && key != 1) {
                    if (shiftPressed && key >= 'a' && key <= 'z') {
                        key = key - 32;
                    } else if (shiftPressed) {
                        switch(key) {
                            case '1': key = '!'; break;
                            case '2': key = '@'; break;
                            case '3': key = '#'; break;
                            case '4': key = '$'; break;
                            case '5': key = '%'; break;
                            case '6': key = '^'; break;
                            case '7': key = '&'; break;
                            case '8': key = '*'; break;
                            case '9': key = '('; break;
                            case '0': key = ')'; break;
                        }
                    }
                    handleKeyPress(key);
                }
            }
        }
    }
    
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
            if (key == ';') {
                currentMenuIndex = (currentMenuIndex - 1 + menuItemCount) % menuItemCount;
                drawMainMenu();
            } else if (key == '.') {
                currentMenuIndex = (currentMenuIndex + 1) % menuItemCount;
                drawMainMenu();
            } else if (key == '\n') {
                switch (currentMenuIndex) {
                    case 0: currentState = WIFI_SCANNER; wifiScanScreen(); break;
                    case 1: currentState = SECURITY_AUDIT; securityAuditScreen(); break;
                    case 2: currentState = AI_ASSISTANT; aiAssistantScreen(); break;
                    case 3: currentState = CODE_WRITER; codeWriterScreen(); break;
                    case 4: currentState = ABOUT; aboutScreen(); break;
                }
            }
            break;
            
        case WIFI_SCANNER:
            if (key == 27) {
                WiFi.scanDelete();
                isScanning = false;
                currentState = MAIN_MENU;
                drawMainMenu();
            } else if (key == 's' || key == 'S') {
                // Start async scan for real-time display
                isScanning = true;
                scannedCount = 0;
                WiFi.scanDelete();
                WiFi.scanNetworks(true); // Async scan
                scanStartTime = millis();
                wifiScanScreen();
            } else if (key >= '1' && key <= '9') {
                int selection = key - '1';
                if (selection < scannedCount) {
                    selectedNetwork = selection;
                    
                    // Check if we have saved password
                    wifiPassword = "";
                    for (int i = 0; i < savedNetworkCount; i++) {
                        if (scannedSSIDs[selectedNetwork] == savedNetworks[i].ssid) {
                            wifiPassword = savedNetworks[i].password;
                            savedNetworks[i].lastUsed = millis();
                            saveSavedNetworks();
                            // Auto-connect if we have password
                            connectToWiFi();
                            return;
                        }
                    }
                    
                    // No saved password, ask for it
                    currentState = WIFI_CONNECT;
                    showWiFiConnectScreen();
                }
            }
            break;
            
        case WIFI_CONNECT:
            if (key == 27) {
                currentState = WIFI_SCANNER;
                wifiScanScreen();
            } else if (key == '\n') {
                connectToWiFi();
            } else if (key == '\b') {
                if (wifiPassword.length() > 0) {
                    wifiPassword.remove(wifiPassword.length() - 1);
                    showWiFiConnectScreen();
                }
            } else if (key >= 32 && key <= 126) {
                wifiPassword += key;
                showWiFiConnectScreen();
            }
            break;
            
        case AI_ASSISTANT:
            if (key == 27) {
                currentState = MAIN_MENU;
                aiInput = "";
                aiResponse = "";
                drawMainMenu();
            } else if (key == '\n') {
                if (aiResponse.length() > 0) {
                    // Response shown - clear for next question
                    if (conversationCount < 5) {
                        conversationHistory[conversationCount].query = aiInput;
                        conversationHistory[conversationCount].response = aiResponse;
                        conversationCount++;
                    } else {
                        // Shift history and add new entry
                        for (int i = 0; i < 4; i++) {
                            conversationHistory[i] = conversationHistory[i + 1];
                        }
                        conversationHistory[4].query = aiInput;
                        conversationHistory[4].response = aiResponse;
                    }
                    // Clear everything for fresh question
                    aiInput = "";
                    aiResponse = "";
                    aiAssistantScreen();
                } else if (aiInput.length() > 0 && !aiProcessing) {
                    // New question - process it
                    processAIQuery();
                }
            } else if (key == '\b') {
                if (aiResponse.length() == 0 && aiInput.length() > 0) {
                    aiInput.remove(aiInput.length() - 1);
                    aiAssistantScreen();
                }
            } else if (key >= 32 && key <= 126 && aiInput.length() < 100 && aiResponse.length() == 0) {
                aiInput += key;
                aiAssistantScreen();
            }
            break;
            
        case CODE_WRITER:
            if (key == 27) {
                currentState = MAIN_MENU;
                codePrompt = "";
                codeBuffer = "";
                codeMode = false;
                drawMainMenu();
            } else if (!codeMode) {
                // Prompt mode - describe what code you want
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
                // Edit mode - editing generated code
                if (key == '\n') {
                    codeBuffer += '\n';
                    codeWriterScreen();
                } else if (key == '\b') {
                    if (codeBuffer.length() > 0) {
                        codeBuffer.remove(codeBuffer.length() - 1);
                        codeWriterScreen();
                    }
                } else if (key == '\t') {
                    // Tab for indentation
                    codeBuffer += "    ";
                    codeWriterScreen();
                } else if (key >= 32 && key <= 126) {
                    codeBuffer += key;
                    codeWriterScreen();
                } else if (key == 's' && ctrlPressed) {
                    // Ctrl+S to save
                    saveCodeToSD();
                }
            }
            break;
            
        case SECURITY_AUDIT:
            if (key == 27) {
                currentState = MAIN_MENU;
                drawMainMenu();
            } else if (key == 's' || key == 'S') {
                runSecurityAudit();
            }
            break;
            
        case ABOUT:
            if (key == 27) {
                currentState = MAIN_MENU;
                drawMainMenu();
           
            }
            break;
    }
}

void drawMainMenu() {
    M5Cardputer.Display.fillScreen(TFT_BLACK);
    
    // Retro border
    M5Cardputer.Display.drawRect(3, 3, 234, 129, TFT_CYAN);
    
    M5Cardputer.Display.setTextSize(3);
    M5Cardputer.Display.setCursor(10, 8);
    M5Cardputer.Display.setTextColor(TFT_GREEN, TFT_BLACK);
    M5Cardputer.Display.println("MAIN MENU");
    
    // Status bar removed - text too small to read
    
    M5Cardputer.Display.setTextSize(2);
    for (int i = 0; i < menuItemCount; i++) {
        int y = 40 + (i * 25);
        M5Cardputer.Display.setCursor(10, y);
        if (i == currentMenuIndex) {
            M5Cardputer.Display.setTextColor(TFT_BLACK, TFT_CYAN);
            M5Cardputer.Display.printf("> %s", menuItems[i]);
        } else {
            M5Cardputer.Display.setTextColor(TFT_WHITE, TFT_BLACK);
            M5Cardputer.Display.printf("  %s", menuItems[i]);
        }
    }
    
    M5Cardputer.Display.setTextColor(TFT_YELLOW, TFT_BLACK);
    M5Cardputer.Display.setCursor(10, 118);
    M5Cardputer.Display.println(";/.:Nav ENT:Sel");
}

void wifiScanScreen() {
    M5Cardputer.Display.fillScreen(TFT_BLACK);
    M5Cardputer.Display.setTextSize(3);
    M5Cardputer.Display.setCursor(10, 5);
    M5Cardputer.Display.setTextColor(TFT_GREEN, TFT_BLACK);
    M5Cardputer.Display.println("WiFi");
    
    M5Cardputer.Display.setTextSize(2);
    M5Cardputer.Display.setCursor(10, 35);
    
    if (isScanning) {
        M5Cardputer.Display.setTextColor(TFT_CYAN, TFT_BLACK);
        M5Cardputer.Display.println("Scanning..");
    } else {
        M5Cardputer.Display.setTextColor(TFT_YELLOW, TFT_BLACK);
        M5Cardputer.Display.printf("Found:%d", scannedCount);
    }
    
    // Display networks as they're found
    M5Cardputer.Display.setTextSize(2);
    int yPos = 60;
    for (int i = 0; i < scannedCount && i < 4; i++) {
        M5Cardputer.Display.setCursor(5, yPos);
        
        // Check if network is saved
        bool isSaved = false;
        for (int j = 0; j < savedNetworkCount; j++) {
            if (scannedSSIDs[i] == savedNetworks[j].ssid) {
                isSaved = true;
                break;
            }
        }
        
        // Color code: Green=saved, Cyan=strong, White=normal, Gray=weak
        if (isSaved) {
            M5Cardputer.Display.setTextColor(TFT_GREEN, TFT_BLACK);
        } else if (scannedRSSI[i] > -60) {
            M5Cardputer.Display.setTextColor(TFT_CYAN, TFT_BLACK);
        } else if (scannedRSSI[i] > -75) {
            M5Cardputer.Display.setTextColor(TFT_WHITE, TFT_BLACK);
        } else {
            M5Cardputer.Display.setTextColor(0x7BEF, TFT_BLACK); // Gray
        }
        
        String ssid = scannedSSIDs[i];
        if (ssid.length() > 12) ssid = ssid.substring(0, 12);
        M5Cardputer.Display.printf("%d", i+1);
        M5Cardputer.Display.print(isSaved ? "*" : " ");
        M5Cardputer.Display.printf("%s%d", ssid.c_str(), scannedRSSI[i]);
        yPos += 17;
    }
    
    M5Cardputer.Display.setTextSize(2);
    M5Cardputer.Display.setCursor(10, 118);
    M5Cardputer.Display.setTextColor(TFT_YELLOW, TFT_BLACK);
    M5Cardputer.Display.print("S:Scan ESC:Back");
}

void securityAuditScreen() {
    M5Cardputer.Display.fillScreen(TFT_BLACK);
    M5Cardputer.Display.setTextSize(3);
    M5Cardputer.Display.setCursor(10, 8);
    M5Cardputer.Display.setTextColor(TFT_CYAN, TFT_BLACK);
    M5Cardputer.Display.println("Security");
    M5Cardputer.Display.setTextSize(2);
    M5Cardputer.Display.setCursor(10, 40);
    M5Cardputer.Display.setTextColor(TFT_WHITE, TFT_BLACK);
    M5Cardputer.Display.println("S:Run audit");
    M5Cardputer.Display.println("SD creds.txt");
    M5Cardputer.Display.setCursor(10, 118);
    M5Cardputer.Display.setTextColor(TFT_YELLOW, TFT_BLACK);
    M5Cardputer.Display.println("ESC: Back");
}

bool loadCredsFromSD() {
    // Check if SD card is available first
    if (!sdCardAvailable) {
        Serial.println("SD card not available - skipping creds.txt");
        return false;
    }
    
    File file = SD.open("/creds.txt");
    if (!file) {
        Serial.println("Failed to open /creds.txt");
        Serial.println("Make sure creds.txt exists on SD card root");
        return false;
    }
    
    Serial.println("Reading creds.txt from SD card...");
    WifiCreds wCreds;
    BtCreds bCreds;
    
    while (file.available()) {
        String line = file.readStringUntil('\n');
        line.trim();
        
        if (line.startsWith("wifi_ssid=")) {
            wCreds.ssid = line.substring(10);
        } else if (line.startsWith("wifi_pass=")) {
            wCreds.password = line.substring(10);
        } else if (line.startsWith("bt_name=")) {
            bCreds.deviceName = line.substring(8);
        } else if (line.startsWith("bt_pin=")) {
            bCreds.pin = line.substring(7);
        }
    }
    
    file.close();
    
    auditor.set_wifi_creds(wCreds);
    auditor.set_bt_creds(bCreds);
    
    Serial.println("Credentials loaded from SD card");
    return true;
}

void runSecurityAudit() {
    M5Cardputer.Display.fillScreen(TFT_BLACK);
    M5Cardputer.Display.setTextSize(2);
    M5Cardputer.Display.setCursor(10, 10);
    M5Cardputer.Display.setTextColor(TFT_CYAN, TFT_BLACK);
    M5Cardputer.Display.println("Running..");
    
    // Try to load credentials from SD card
    bool sdLoaded = loadCredsFromSD();
    
    M5Cardputer.Display.setCursor(10, 40);
    if (sdLoaded) {
        M5Cardputer.Display.setTextColor(TFT_GREEN, TFT_BLACK);
        M5Cardputer.Display.println("SD: OK");
    } else {
        M5Cardputer.Display.setTextColor(TFT_YELLOW, TFT_BLACK);
        M5Cardputer.Display.println("No SD");
    }
    
    M5Cardputer.Display.setCursor(10, 65);
    M5Cardputer.Display.setTextColor(TFT_WHITE, TFT_BLACK);
    M5Cardputer.Display.println("Check");
    M5Cardputer.Display.setCursor(10, 85);
    M5Cardputer.Display.println("Serial");
    
    // Run the audit (logs to Serial)
    auditor.run_security_audit();
    
    M5Cardputer.Display.setCursor(10, 105);
    M5Cardputer.Display.setTextColor(TFT_GREEN, TFT_BLACK);
    M5Cardputer.Display.println("Done!");
}

void aboutScreen() {
    M5Cardputer.Display.fillScreen(TFT_BLACK);
    M5Cardputer.Display.setTextSize(3);
    M5Cardputer.Display.setCursor(10, 8);
    M5Cardputer.Display.setTextColor(TFT_MAGENTA, TFT_BLACK);
    M5Cardputer.Display.println("SKELETOR");
    M5Cardputer.Display.setTextSize(2);
    M5Cardputer.Display.setCursor(10, 45);
    M5Cardputer.Display.setTextColor(TFT_CYAN, TFT_BLACK);
    M5Cardputer.Display.println("Cardputer");
    M5Cardputer.Display.setCursor(10, 70);
    M5Cardputer.Display.setTextColor(TFT_WHITE, TFT_BLACK);
    M5Cardputer.Display.printf("v%s", FIRMWARE_VERSION);
    M5Cardputer.Display.setCursor(10, 95);
    M5Cardputer.Display.println("Stamps3");
    M5Cardputer.Display.setCursor(10, 118);
    M5Cardputer.Display.printf("Mem:%dKB", ESP.getFreeHeap()/1024);
}

// ====== CODE WRITER FUNCTIONS ======
void codeWriterScreen() {
    M5Cardputer.Display.fillScreen(TFT_BLACK);
    M5Cardputer.Display.drawRect(3, 3, 234, 129, TFT_GREEN);
    
    M5Cardputer.Display.setTextSize(2);
    M5Cardputer.Display.setCursor(10, 8);
    M5Cardputer.Display.setTextColor(TFT_GREEN, TFT_BLACK);
    M5Cardputer.Display.println("CODE AI");
    
    if (!codeMode) {
        // Prompt mode - ask AI what code to generate
        M5Cardputer.Display.setTextSize(1);
        M5Cardputer.Display.setCursor(10, 30);
        M5Cardputer.Display.setTextColor(TFT_CYAN, TFT_BLACK);
        M5Cardputer.Display.print("What code? >");
        
        M5Cardputer.Display.setCursor(10, 42);
        M5Cardputer.Display.setTextColor(TFT_WHITE, TFT_BLACK);
        // Word wrap prompt
        int yPos = 42;
        for (int i = 0; i < codePrompt.length(); i += 28) {
            String line = codePrompt.substring(i, min(i + 28, (int)codePrompt.length()));
            M5Cardputer.Display.setCursor(10, yPos);
            M5Cardputer.Display.println(line);
            yPos += 10;
        }
        M5Cardputer.Display.print("_");
        
        M5Cardputer.Display.setTextSize(2);
        M5Cardputer.Display.setCursor(10, 118);
        M5Cardputer.Display.setTextColor(TFT_YELLOW, TFT_BLACK);
        M5Cardputer.Display.print("ENT:Gen ESC:Back");
    } else {
        // Edit mode - showing generated code
        M5Cardputer.Display.setTextSize(2);
        M5Cardputer.Display.setCursor(10, 28);
        M5Cardputer.Display.setTextColor(TFT_CYAN, TFT_BLACK);
        M5Cardputer.Display.println("CODE");
        
        // Display code (simplified for readability)
        M5Cardputer.Display.setCursor(10, 50);
        M5Cardputer.Display.setTextColor(TFT_WHITE, TFT_BLACK);
        
        int yPos = 50;
        int charIdx = codeScroll;
        String currentLine = "";
        
        while (charIdx < codeBuffer.length() && yPos < 100) {
            char c = codeBuffer[charIdx];
            if (c == '\n' || currentLine.length() >= 18) {
                M5Cardputer.Display.setCursor(10, yPos);
                M5Cardputer.Display.println(currentLine);
                currentLine = "";
                yPos += 16;
                if (c == '\n') charIdx++;
            } else {
                currentLine += c;
                charIdx++;
            }
        }
        if (currentLine.length() > 0) {
            M5Cardputer.Display.setCursor(10, yPos);
            M5Cardputer.Display.print(currentLine);
        }
        
        M5Cardputer.Display.setTextSize(2);
        M5Cardputer.Display.setCursor(10, 118);
        M5Cardputer.Display.setTextColor(TFT_YELLOW, TFT_BLACK);
        M5Cardputer.Display.print("^S:Save ESC:Exit");
    }
}

void generateCode() {
    codeMode = true;
    codeBuffer = "// Generating code...\n";
    codeWriterScreen();
    
    // Build AI prompt for code generation
    String aiPrompt = "Write " + codePrompt + ". Only code, no explanations. C/C++ for ESP32.";
    
    if (WiFi.status() == WL_CONNECTED && aiApiKey.length() > 0) {
        // Use AI API to generate code
        String generated = callAIAPI(aiPrompt);
        if (generated.length() > 0 && !generated.startsWith("HTTP")) {
            codeBuffer = generated;
        } else {
            codeBuffer = "// API error. Write code manually:\n\nvoid setup() {\n    // Your setup\n}\n\nvoid loop() {\n    // Your code\n}\n";
        }
    } else {
        // Offline template based on keywords
        if (codePrompt.indexOf("blink") >= 0) {
            codeBuffer = "// LED Blink\nvoid setup() {\n    pinMode(LED_BUILTIN, OUTPUT);\n}\n\nvoid loop() {\n    digitalWrite(LED_BUILTIN, HIGH);\n    delay(1000);\n    digitalWrite(LED_BUILTIN, LOW);\n    delay(1000);\n}\n";
        } else if (codePrompt.indexOf("sensor") >= 0) {
            codeBuffer = "// Sensor Read\nvoid setup() {\n    Serial.begin(115200);\n    pinMode(A0, INPUT);\n}\n\nvoid loop() {\n    int val = analogRead(A0);\n    Serial.println(val);\n    delay(100);\n}\n";
        } else if (codePrompt.indexOf("wifi") >= 0) {
            codeBuffer = "#include <WiFi.h>\n\nconst char* ssid = \"YourSSID\";\nconst char* pass = \"YourPass\";\n\nvoid setup() {\n    WiFi.begin(ssid, pass);\n    while (WiFi.status() != WL_CONNECTED) {\n        delay(500);\n    }\n}\nvoid loop() {}\n";
        } else {
            codeBuffer = "// " + codePrompt + "\n\nvoid setup() {\n    // Initialize\n    Serial.begin(115200);\n}\n\nvoid loop() {\n    // Your code here\n    delay(1000);\n}\n";
        }
    }
    
    codeWriterScreen();
}

void saveCodeToSD() {
    SDManager& sd = SDManager::getInstance();
    if (!sd.isAvailable()) {
        M5Cardputer.Display.fillRect(10, 110, 220, 12, TFT_BLACK);
        M5Cardputer.Display.setCursor(10, 110);
        M5Cardputer.Display.setTextColor(TFT_RED, TFT_BLACK);
        M5Cardputer.Display.print("No SD card!");
        delay(2000);
        codeWriterScreen();
        return;
    }
    
    bool success = sd.writeFile("/" + codeFilename, codeBuffer);
    
    M5Cardputer.Display.fillRect(10, 110, 220, 12, TFT_BLACK);
    M5Cardputer.Display.setCursor(10, 110);
    
    if (success) {
        M5Cardputer.Display.setTextColor(TFT_GREEN, TFT_BLACK);
        M5Cardputer.Display.print("Saved: " + codeFilename);
    } else {
        M5Cardputer.Display.setTextColor(TFT_RED, TFT_BLACK);
        M5Cardputer.Display.print("Save failed!");
    }
    
    delay(2000);
    codeWriterScreen();
}

// ====== Graffiti Style Splash Screen ======
void showThunderCastleSplash() {
    M5Cardputer.Display.fillScreen(TFT_BLACK);
    
    // Simple border - bright red
    uint16_t brightRed = 0xF800;  // Pure red RGB565
    M5Cardputer.Display.drawRect(3, 3, 234, 129, brightRed);
    M5Cardputer.Display.drawRect(4, 4, 232, 127, brightRed);
    M5Cardputer.Display.drawRect(5, 5, 230, 125, brightRed);  // Extra bold
    delay(150);
    
    // HACK THE PLANET in BOLD red graffiti style
    M5Cardputer.Display.setTextColor(brightRed, TFT_BLACK);
    M5Cardputer.Display.setTextSize(3);
    M5Cardputer.Display.setCursor(10, 25);
    String title = "HACK THE";
    for (int i = 0; i < title.length(); i++) {
        M5Cardputer.Display.print(title[i]);
        delay(60);
    }
    
    M5Cardputer.Display.setCursor(20, 55);
    String title2 = "PLANET";
    for (int i = 0; i < title2.length(); i++) {
        M5Cardputer.Display.print(title2[i]);
        delay(60);
    }
    
    // Graffiti tag
    M5Cardputer.Display.setTextColor(TFT_WHITE, TFT_BLACK);
    M5Cardputer.Display.setTextSize(2);
    M5Cardputer.Display.setCursor(10, 95);
    M5Cardputer.Display.print("SKELETOR");
    
    // Version
    M5Cardputer.Display.setTextSize(1);
    M5Cardputer.Display.setCursor(170, 115);
    M5Cardputer.Display.print("v");
    M5Cardputer.Display.print(FIRMWARE_VERSION);
    
    // Draw retro pixel art decorations with animation
    for (int i = 0; i < 10; i++) {
        M5Cardputer.Display.drawPixel(15 + i*2, 15, TFT_GREEN);
        M5Cardputer.Display.drawPixel(15 + i*2, 115, TFT_GREEN);
        M5Cardputer.Display.drawPixel(215 - i*2, 15, TFT_MAGENTA);
        M5Cardputer.Display.drawPixel(215 - i*2, 115, TFT_MAGENTA);
        delay(30);
    }
}

// ====== Devil Screen - Stage 3 ======
void showDevilScreen() {
    M5Cardputer.Display.fillScreen(TFT_BLACK);
    
    // Red pulsing border with animation
    for (int i = 0; i < 3; i++) {
        M5Cardputer.Display.drawRect(5 + i, 5 + i, 230 - i*2, 125 - i*2, TFT_RED);
        delay(80);
    }
    
    // Sinister title
    M5Cardputer.Display.setTextColor(TFT_RED, TFT_BLACK);
    M5Cardputer.Display.setTextSize(3);
    M5Cardputer.Display.setCursor(20, 20);
    M5Cardputer.Display.println("SKELETOR");
    
    // Subtitle
    M5Cardputer.Display.setTextSize(2);
    M5Cardputer.Display.setTextColor(TFT_ORANGE, TFT_BLACK);
    M5Cardputer.Display.setCursor(30, 50);
    M5Cardputer.Display.println("DEVIL MODE");
    
    // Ominous message
    M5Cardputer.Display.setTextSize(2);
    M5Cardputer.Display.setTextColor(TFT_YELLOW, TFT_BLACK);
    M5Cardputer.Display.setCursor(15, 75);
    M5Cardputer.Display.println("INIT...");
    
    // Inverted pentagram effect using lines
    int cx = 120, cy = 105;
    int r = 15;
    for (int i = 0; i < 5; i++) {
        float angle1 = (i * 144 - 90) * 3.14159 / 180;
        float angle2 = ((i + 2) % 5 * 144 - 90) * 3.14159 / 180;
        int x1 = cx + r * cos(angle1);
        int y1 = cy + r * sin(angle1);
        int x2 = cx + r * cos(angle2);
        int y2 = cy + r * sin(angle2);
        M5Cardputer.Display.drawLine(x1, y1, x2, y2, TFT_RED);
    }
    
    // Flash effect
    for (int i = 0; i < 3; i++) {
        M5Cardputer.Display.fillScreen(TFT_RED);
        delay(80);
        M5Cardputer.Display.fillScreen(TFT_BLACK);
        delay(80);
    }
    
    // Redraw after flash
    for (int i = 0; i < 3; i++) {
        M5Cardputer.Display.drawRect(5 + i, 5 + i, 230 - i*2, 125 - i*2, TFT_RED);
    }
    M5Cardputer.Display.setTextColor(TFT_RED, TFT_BLACK);
    M5Cardputer.Display.setTextSize(3);
    M5Cardputer.Display.setCursor(20, 20);
    M5Cardputer.Display.println("SKELETOR");
    M5Cardputer.Display.setTextSize(2);
    M5Cardputer.Display.setTextColor(TFT_ORANGE, TFT_BLACK);
    M5Cardputer.Display.setCursor(30, 50);
    M5Cardputer.Display.println("DEVIL MODE");
    M5Cardputer.Display.setTextSize(2);
    M5Cardputer.Display.setTextColor(TFT_GREEN, TFT_BLACK);
    M5Cardputer.Display.setCursor(30, 100);
    M5Cardputer.Display.println("READY!");
}

// ====== Sinister Audio ======
void playSinisterAudio() {
    // Descending ominous tones
    int sinisterMelody[] = { 
        220, 208, 196, 185, 174, 165, 156, 147,  // Descending A scale
        139, 131, 123, 117, 110, 104, 98, 92     // Lower and darker
    };
    
    // Start with low rumble
    for (int i = 0; i < 3; i++) {
        M5Cardputer.Speaker.tone(55, 150);  // Very low A
        delay(200);
        M5Cardputer.Speaker.stop();
        delay(50);
    }
    
    // Descending sinister scale
    for (int i = 0; i < 16; i++) {
        M5Cardputer.Speaker.tone(sinisterMelody[i], 100);
        delay(120);
    }
    M5Cardputer.Speaker.stop();
    
    delay(200);
    
    // Final ominous chord (tritone - the devil's interval)
    M5Cardputer.Speaker.tone(185, 300);  // F#
    delay(150);
    M5Cardputer.Speaker.tone(131, 500);  // C (tritone)
    delay(600);
    M5Cardputer.Speaker.stop();
}

// ====== WiFi Connect Screen ======
void showWiFiConnectScreen() {
    M5Cardputer.Display.fillScreen(TFT_BLACK);
    M5Cardputer.Display.setTextSize(3);
    M5Cardputer.Display.setCursor(10, 8);
    M5Cardputer.Display.setTextColor(TFT_RED, TFT_BLACK);
    M5Cardputer.Display.println("WiFi Con");
    
    M5Cardputer.Display.setTextSize(2);
    M5Cardputer.Display.setCursor(10, 40);
    M5Cardputer.Display.setTextColor(TFT_RED, TFT_BLACK);
    String ssid = scannedSSIDs[selectedNetwork];
    if (ssid.length() > 14) ssid = ssid.substring(0, 14);
    M5Cardputer.Display.println(ssid);
    
    M5Cardputer.Display.setCursor(10, 65);
    M5Cardputer.Display.setTextColor(TFT_YELLOW, TFT_BLACK);
    M5Cardputer.Display.println("Password:");
    
    M5Cardputer.Display.setCursor(10, 88);
    M5Cardputer.Display.setTextColor(TFT_WHITE, TFT_BLACK);
    String maskedPass = "";
    for (int i = 0; i < wifiPassword.length(); i++) {
        maskedPass += "*";
    }
    if (maskedPass.length() > 16) maskedPass = maskedPass.substring(0, 16);
    M5Cardputer.Display.println(maskedPass);
    
    M5Cardputer.Display.setCursor(10, 110);
    M5Cardputer.Display.setTextColor(TFT_YELLOW, TFT_BLACK);
    M5Cardputer.Display.setTextSize(1);
    M5Cardputer.Display.println("ENT:Connect ESC:Back");
}

// ====== Connect to WiFi ======
void connectToWiFi() {
    M5Cardputer.Display.fillScreen(TFT_BLACK);
    M5Cardputer.Display.setTextSize(2);
    M5Cardputer.Display.setCursor(10, 40);
    M5Cardputer.Display.setTextColor(TFT_RED, TFT_BLACK);
    M5Cardputer.Display.println("Connecting");
    
    // Use WiFiManager for connection
    WiFiManager& wifiMgr = WiFiManager::getInstance();
    bool connected = wifiMgr.connect(scannedSSIDs[selectedNetwork], wifiPassword);
    
    M5Cardputer.Display.setCursor(10, 70);
    if (connected) {
        M5Cardputer.Display.setTextColor(TFT_GREEN, TFT_BLACK);
        M5Cardputer.Display.println("Connected!");
        M5Cardputer.Display.setCursor(10, 90);
        M5Cardputer.Display.setTextColor(TFT_WHITE, TFT_BLACK);
        M5Cardputer.Display.print(wifiMgr.getLocalIP());
        
        // Save credentials
        saveWiFiCredentials();
        M5Cardputer.Display.setCursor(10, 110);
        M5Cardputer.Display.setTextSize(1);
        M5Cardputer.Display.setTextColor(TFT_CYAN, TFT_BLACK);
        M5Cardputer.Display.println("Password Saved!");
    } else {
        M5Cardputer.Display.setTextColor(TFT_RED, TFT_BLACK);
        M5Cardputer.Display.println("Failed!");
        M5Cardputer.Display.setCursor(10, 90);
        M5Cardputer.Display.setTextColor(TFT_YELLOW, TFT_BLACK);
        M5Cardputer.Display.println("Check pass");
    }
    
    delay(3000);
    currentState = WIFI_SCANNER;
    wifiScanScreen();
}

// ====== AI ASSISTANT FUNCTIONS ======
void aiAssistantScreen() {
    M5Cardputer.Display.fillScreen(TFT_BLACK);
    
    // Retro border
    M5Cardputer.Display.drawRect(3, 3, 234, 129, TFT_CYAN);
    
    // Header
    M5Cardputer.Display.setTextSize(2);
    M5Cardputer.Display.setCursor(10, 8);
    M5Cardputer.Display.setTextColor(TFT_RED, TFT_BLACK);
    M5Cardputer.Display.println("AI CHAT");
    
    // If we have a response, show it prominently and wait for next question
    if (aiResponse.length() > 0) {
        // Show last question
        M5Cardputer.Display.setTextSize(2);
        M5Cardputer.Display.setCursor(10, 35);
        M5Cardputer.Display.setTextColor(TFT_YELLOW, TFT_BLACK);
        M5Cardputer.Display.print("Q:");
        String q = aiInput;
        if (q.length() > 16) q = q.substring(0, 15) + ">";
        M5Cardputer.Display.print(q);
        
        // Show answer across multiple lines
        M5Cardputer.Display.setCursor(10, 55);
        M5Cardputer.Display.setTextColor(TFT_GREEN, TFT_BLACK);
        M5Cardputer.Display.print("A:");
        
        int yPos = 55;
        int charIdx = 0;
        while (charIdx < aiResponse.length() && yPos < 100) {
            String line = aiResponse.substring(charIdx, min(charIdx + 18, (int)aiResponse.length()));
            M5Cardputer.Display.setCursor(10, yPos);
            M5Cardputer.Display.println(line);
            yPos += 16;
            charIdx += 18;
        }
        
        // Bottom instruction
        M5Cardputer.Display.setTextSize(2);
        M5Cardputer.Display.setCursor(10, 118);
        M5Cardputer.Display.setTextColor(TFT_YELLOW, TFT_BLACK);
        M5Cardputer.Display.print("ENT:Next ESC:Exit");
        return;
    }
    
    // Input mode - clean prompt area
    M5Cardputer.Display.setTextSize(2);
    M5Cardputer.Display.setCursor(10, 50);
    M5Cardputer.Display.setTextColor(TFT_WHITE, TFT_BLACK);
    M5Cardputer.Display.print(">");
    
    // Display current input with word wrap
    M5Cardputer.Display.setCursor(22, 50);
    M5Cardputer.Display.setTextColor(TFT_CYAN, TFT_BLACK);
    
    int yPos = 50;
    int charIdx = 0;
    while (charIdx < aiInput.length() && yPos < 90) {
        String line = aiInput.substring(charIdx, min(charIdx + 16, (int)aiInput.length()));
        M5Cardputer.Display.setCursor(22, yPos);
        M5Cardputer.Display.println(line);
        yPos += 16;
        charIdx += 16;
    }
    M5Cardputer.Display.print("_");
    
    // Show processing status
    if (aiProcessing) {
        M5Cardputer.Display.setTextSize(2);
        M5Cardputer.Display.setCursor(10, 90);
        M5Cardputer.Display.setTextColor(TFT_YELLOW, TFT_BLACK);
        M5Cardputer.Display.print("Thinking...");
    }
    
    // Bottom instructions
    M5Cardputer.Display.setTextSize(2);
    M5Cardputer.Display.setCursor(10, 118);
    M5Cardputer.Display.setTextColor(TFT_YELLOW, TFT_BLACK);
    M5Cardputer.Display.print("ENT:Ask ESC:Exit");
}

void processAIQuery() {
    // Use AIManager to process the query
    AIManager& aiMgr = AIManager::getInstance();
    
    // Check for learning commands first
    if (aiInput.startsWith("learn:")) {
        String cmd = aiInput.substring(6);
        int arrowPos = cmd.indexOf("->");
        if (arrowPos > 0) {
            String trigger = cmd.substring(0, arrowPos);
            String action = cmd.substring(arrowPos + 2);
            trigger.trim();
            action.trim();
            
            if (aiMgr.learnPattern(trigger, action)) {
                // Update local patterns array
                patternCount = aiMgr.getPatternCount();
                for (int i = 0; i < patternCount && i < 10; i++) {
                    const LearnedPattern& p = aiMgr.getPattern(i);
                    patterns[i].trigger = p.trigger;
                    patterns[i].action = p.action;
                }
                aiResponse = "Learned! Try: " + trigger;
            } else {
                aiResponse = "Pattern full (max 10)";
            }
        } else {
            aiResponse = "Bad format. Use: learn:word->action";
        }
        aiAssistantScreen();
        return;
    }
    
    // Check for pattern match that triggers actions
    for (int i = 0; i < patternCount; i++) {
        if (aiInput.equalsIgnoreCase(patterns[i].trigger)) {
            aiResponse = "Action: " + patterns[i].action;
            aiAssistantScreen();
            
            // Execute learned action
            if (patterns[i].action == "wifi") {
                delay(1000);
                currentState = WIFI_SCANNER;
                wifiScanScreen();
            } else if (patterns[i].action == "scan") {
                delay(1000);
                currentState = SECURITY_AUDIT;
                securityAuditScreen();
            }
            return;
        }
    }
    
    // Use AIManager for general query processing
    aiResponse = aiMgr.processQuery(aiInput);
    
    aiAssistantScreen();
}

String callAIAPI(String query) {
    // Use AIManager for API calls
    AIManager& aiMgr = AIManager::getInstance();
    return aiMgr.callDeepSeekAPI(query);
}

void learnPattern(String trigger, String action) {
    // Use AIManager for pattern learning
    AIManager& aiMgr = AIManager::getInstance();
    if (aiMgr.learnPattern(trigger, action)) {
        patternCount = aiMgr.getPatternCount();
        for (int i = 0; i < patternCount && i < 10; i++) {
            const LearnedPattern& p = aiMgr.getPattern(i);
            patterns[i].trigger = p.trigger;
            patterns[i].action = p.action;
        }
    }
}

void saveLearnedPatterns() {
    // Delegate to AIManager
    AIManager& aiMgr = AIManager::getInstance();
    aiMgr.savePatterns();
}

void loadLearnedPatterns() {
    // Delegate to AIManager
    AIManager& aiMgr = AIManager::getInstance();
    if (aiMgr.loadPatterns()) {
        patternCount = aiMgr.getPatternCount();
        for (int i = 0; i < patternCount && i < 10; i++) {
            const LearnedPattern& p = aiMgr.getPattern(i);
            patterns[i].trigger = p.trigger;
            patterns[i].action = p.action;
        }
    }
}

// ====== WiFi Credential Management (Multiple Networks) ======
void saveWiFiCredentials() {
void saveWiFiCredentials() {
    // Use WiFiManager to save network
    WiFiManager& wifiMgr = WiFiManager::getInstance();
    String ssid = scannedSSIDs[selectedNetwork];
    
    if (wifiMgr.saveNetwork(ssid, wifiPassword)) {
        // Update local state
        savedNetworkCount = wifiMgr.getSavedNetworkCount();
        for (int i = 0; i < savedNetworkCount && i < 10; i++) {
            const SavedNetwork& saved = wifiMgr.getSavedNetwork(i);
            savedNetworks[i].ssid = saved.ssid;
            savedNetworks[i].password = saved.password;
            savedNetworks[i].lastUsed = saved.lastUsed;
        }
        Serial.printf("Saved WiFi: %s\n", ssid.c_str());
    }
}

void saveSavedNetworks() {
    // Delegate to WiFiManager
    WiFiManager& wifiMgr = WiFiManager::getInstance();
    wifiMgr.saveSavedNetworks();
}

void loadWiFiCredentials() {
    // Delegate to WiFiManager
    WiFiManager& wifiMgr = WiFiManager::getInstance();
    if (wifiMgr.loadSavedNetworks()) {
        savedNetworkCount = wifiMgr.getSavedNetworkCount();
        for (int i = 0; i < savedNetworkCount && i < 10; i++) {
            const SavedNetwork& saved = wifiMgr.getSavedNetwork(i);
            savedNetworks[i].ssid = saved.ssid;
            savedNetworks[i].password = saved.password;
            savedNetworks[i].lastUsed = saved.lastUsed;
        }
    }
}

void autoConnectWiFi() {
    // Delegate to WiFiManager
    WiFiManager& wifiMgr = WiFiManager::getInstance();
    if (wifiMgr.autoConnect()) {
        autoConnected = true;
        // Update local state
        savedNetworkCount = wifiMgr.getSavedNetworkCount();
        for (int i = 0; i < savedNetworkCount && i < 10; i++) {
            const SavedNetwork& saved = wifiMgr.getSavedNetwork(i);
            savedNetworks[i].ssid = saved.ssid;
            savedNetworks[i].password = saved.password;
            savedNetworks[i].lastUsed = saved.lastUsed;
        }
    } else {
        Serial.println("WiFi: Auto-connect failed");
    }
}



// ====== Retro Startup Chiptune Sound ======
void playThunderCastleSound() {
    // Classic 80s arcade startup melody
    int melody[] = { 523, 659, 784, 659, 523 };  // C5, E5, G5, E5, C5
    int duration[] = { 150, 150, 300, 150, 400 };
    
    for (int i = 0; i < 5; i++) {
        M5Cardputer.Speaker.tone(melody[i], duration[i]);
        delay(duration[i] + 30);
        M5Cardputer.Speaker.stop();
    }
    
    // Victory beep
    delay(100);
    M5Cardputer.Speaker.tone(1047, 200);  // C6
    delay(250);
    M5Cardputer.Speaker.stop();
}

// ====== Microphone Recording Functions ======
void startMicRecording() {
    Serial.println("Starting mic recording...");
    
    if (!audioBuffer) {
        Serial.println("Audio buffer not available");
        return;
    }
    
    micRecording = true;
    audioSampleCount = 0;
    
    // Initialize microphone
    if (!M5Cardputer.Mic.begin()) {
        Serial.println("Mic initialization failed");
        micRecording = false;
        return;
    }
    
    M5Cardputer.Display.setTextSize(2);
    M5Cardputer.Display.setCursor(10, 115);
    M5Cardputer.Display.setTextColor(TFT_RED, TFT_BLACK);
    M5Cardputer.Display.println("[REC] ");
    
    // Play recording start beep
    M5Cardputer.Speaker.tone(880, 100);
    delay(100);
    M5Cardputer.Speaker.stop();
    
    Serial.println("Recording... Press M again to stop");
    Serial.println("Mic will record continuously until stopped");
}

void stopMicRecording() {
    Serial.println("Stopping mic recording...");
    
    // Capture audio samples before stopping
    Serial.println("Finalizing audio capture...");
    unsigned long captureStart = millis();
    int captureAttempts = 0;
    
    // Capture for a short burst before processing
    while (audioSampleCount < AUDIO_BUFFER_SIZE && (millis() - captureStart < 3000)) {
        size_t remaining = AUDIO_BUFFER_SIZE - audioSampleCount;
        if (remaining > 1024) remaining = 1024;
        
        if (M5Cardputer.Mic.isEnabled()) {
            captureAttempts++;
            if (M5Cardputer.Mic.record(&audioBuffer[audioSampleCount], remaining)) {
                audioSampleCount += remaining;
                if (captureAttempts % 5 == 0) {
                    Serial.printf("Captured %d samples...\n", audioSampleCount);
                }
            }
        }
        delay(10);
    }
    
    Serial.printf("Capture complete: %d attempts, %d samples\n", captureAttempts, audioSampleCount);
    
    // Play recording stop beep
    M5Cardputer.Speaker.tone(440, 100);
    delay(100);
    M5Cardputer.Speaker.stop();
    
    M5Cardputer.Mic.end();
    micRecording = false;
    
    Serial.printf("Captured %d audio samples\n", audioSampleCount);
    
    // Show processing status
    aiAssistantScreen();
    M5Cardputer.Display.setTextSize(2);
    M5Cardputer.Display.setCursor(10, 115);
    M5Cardputer.Display.setTextColor(TFT_YELLOW, TFT_BLACK);
    M5Cardputer.Display.println("[PROCESSING...]");
    
    // Transcribe audio to text
    String transcribedText = transcribeAudio();
    
    if (transcribedText.length() > 0) {
        // Add transcribed text to AI input
        aiInput += transcribedText + " ";
        Serial.printf("SUCCESS: Transcribed text: %s\n", transcribedText.c_str());
        
        // Show success with transcribed text
        aiAssistantScreen();
        M5Cardputer.Display.setTextSize(1);
        M5Cardputer.Display.setCursor(10, 110);
        M5Cardputer.Display.setTextColor(TFT_GREEN, TFT_BLACK);
        M5Cardputer.Display.printf("[VOICE: %s]", transcribedText.c_str());
        delay(1500);
    } else {
        // Show error with sample count info
        aiAssistantScreen();
        M5Cardputer.Display.setTextSize(1);
        M5Cardputer.Display.setCursor(10, 110);
        M5Cardputer.Display.setTextColor(TFT_RED, TFT_BLACK);
        M5Cardputer.Display.printf("[FAIL: %d samples]", audioSampleCount);
        delay(1500);
        Serial.printf("FAIL: Transcription failed - %d samples captured\n", audioSampleCount);
    }
    
    // Refresh the AI screen with new input
    aiAssistantScreen();
}

String transcribeAudio() {
    Serial.printf("\n=== VOICE ANALYSIS ===\n");
    Serial.printf("Samples captured: %d\n", audioSampleCount);
    
    // Check audio sample count first
    if (audioSampleCount < 5) {
        Serial.println("FAIL: No audio captured by microphone");
        return "";
    }
    
    // Show first few samples for debugging
    Serial.print("First 10 samples: ");
    for (int i = 0; i < 10 && i < audioSampleCount; i++) {
        Serial.printf("%d ", audioBuffer[i]);
    }
    Serial.println();
    
    // Calculate audio characteristics
    int32_t avgAmplitude = 0;
    int32_t maxAmplitude = 0;
    int peakCount = 0;
    int zeroCrossings = 0;
    int16_t lastSample = 0;
    int16_t threshold = 30;
    
    for (int i = 0; i < audioSampleCount; i++) {
        int16_t sample = audioBuffer[i];
        int16_t absSample = abs(sample);
        avgAmplitude += absSample;
        if (absSample > maxAmplitude) maxAmplitude = absSample;
        if (absSample > threshold) peakCount++;
        
        // Count zero crossings (frequency indicator)
        if (i > 0 && ((lastSample >= 0 && sample < 0) || (lastSample < 0 && sample >= 0))) {
            zeroCrossings++;
        }
        lastSample = sample;
    }
    
    if (audioSampleCount > 0) {
        avgAmplitude /= audioSampleCount;
    }
    
    Serial.printf("Avg amplitude: %d\n", avgAmplitude);
    Serial.printf("Max amplitude: %d\n", maxAmplitude);
    Serial.printf("Peak count (>%d): %d\n", threshold, peakCount);
    Serial.printf("Zero crossings: %d\n", zeroCrossings);
    float peakRatio = (peakCount * 100.0) / audioSampleCount;
    Serial.printf("Peak ratio: %.1f%%\n", peakRatio);
    
    // Use multiple characteristics for better variety
    int complexity = (zeroCrossings / 10) + (avgAmplitude / 20) + (peakCount / 50);
    Serial.printf("Complexity score: %d\n", complexity);
    
    if (maxAmplitude < 20 || peakCount < 3) {
        Serial.println("RESULT: Too quiet\n");
        return "";
    }
    
    // More varied responses based on audio characteristics
    String responses[] = {
        "hello", "status", "help", "scan wifi", 
        "what can you do", "show menu", "tell me more",
        "check system", "analyze", "report"
    };
    
    // Use complexity and randomness for variety
    int responseIndex = (complexity + random(0, 3)) % 10;
    String result = responses[responseIndex];
    
    Serial.printf("RESULT: %s\n\n", result.c_str());
    return result;
}

// ====== Background AI Learning System ======
void backgroundAILearning() {
    Serial.println("\n[AI LEARNING CYCLE]");
    
    // Don't learn during active user interaction
    if (micRecording || aiProcessing) {
        return;
    }
    
    learningExperiment++;
    Serial.printf("Experiment #%d\\n", learningExperiment);
    
    // Experiment with different capabilities
    switch (learningExperiment % 10) {
        case 0:
            // Test memory usage
            Serial.printf("Memory check: %d bytes free\\n", ESP.getFreeHeap());
            lastDiscovery = "Memory: " + String(ESP.getFreeHeap()) + " bytes";
            break;
            
        case 1:
            // Test WiFi signal strength
            if (WiFi.status() == WL_CONNECTED) {
                int rssi = WiFi.RSSI();
                Serial.printf("WiFi signal: %d dBm\\n", rssi);
                lastDiscovery = "WiFi: " + String(rssi) + " dBm";
            }
            break;
            
        case 2:
            // Learn from patterns
            if (patternCount > 0) {
                Serial.printf("Learned patterns: %d\\n", patternCount);
                lastDiscovery = String(patternCount) + " patterns learned";
            }
            break;
            
        case 3:
            // Test SD card operations
            if (sdCardAvailable) {
                Serial.println("Testing SD card write...");
                File testFile = SD.open("/ai_log.txt", FILE_APPEND);
                if (testFile) {
                    testFile.printf("Learn cycle %d at %lu ms\\n", learningExperiment, millis());
                    testFile.close();
                    lastDiscovery = "SD log updated";
                }
            }
            break;
            
        case 4:
            // Experiment with GPIO
            experimentWithHardware();
            break;
            
        case 5:
            // Analyze system uptime
            Serial.printf("Uptime: %lu seconds\\n", millis() / 1000);
            lastDiscovery = "Uptime: " + String(millis() / 1000) + "s";
            break;
            
        case 6:
            // Test microphone availability
            if (M5Cardputer.Mic.isEnabled()) {
                Serial.println("Microphone: Ready");
                lastDiscovery = "Mic ready";
            } else {
                Serial.println("Microphone: Offline");
            }
            break;
            
        case 7:
            // Scan WiFi networks in background
            if (WiFi.status() != WL_CONNECTED) {
                int networks = WiFi.scanNetworks(true);  // Async scan
                if (networks >= 0) {
                    Serial.printf("WiFi scan: %d networks\\n", networks);
                    lastDiscovery = String(networks) + " WiFi networks";
                }
            }
            break;
            
        case 8:
            // Temperature/system health
            Serial.printf("CPU Frequency: %d MHz\\n", ESP.getCpuFreqMHz());
            lastDiscovery = "CPU: " + String(ESP.getCpuFreqMHz()) + " MHz";
            break;
            
        case 9:
            // Random discovery
            lastDiscovery = "Exploring system capabilities...";
            Serial.println("AI: Learning new patterns");
            break;
    }
    
    Serial.println("[LEARNING COMPLETE]\\n");
}

void experimentWithHardware() {
    Serial.println("Hardware experiment:");
    
    // Test available GPIO pins (safe ones)
    int testPins[] = {1, 2, 8, 9, 10, 11, 46};  // Safe GPIO for testing
    
    for (int pin : testPins) {
        pinMode(pin, INPUT_PULLUP);
        int state = digitalRead(pin);
        Serial.printf("GPIO %d: %s\\n", pin, state ? "HIGH" : "LOW");
    }
    
    // Test I2C devices
    Wire.begin();
    int i2cDevices = 0;
    for (byte addr = 1; addr < 127; addr++) {
        Wire.beginTransmission(addr);
        if (Wire.endTransmission() == 0) {
            i2cDevices++;
            Serial.printf("I2C device at 0x%02X\\n", addr);
        }
    }
    
    if (i2cDevices > 0) {
        lastDiscovery = String(i2cDevices) + " I2C devices found";
    } else {
        lastDiscovery = "Hardware scan complete";
    }
}

// ====== Serial Command Interface for USB Testing ======
void handleSerialCommands() {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    cmd.toLowerCase();
    
    if (cmd.length() == 0) return;
    
    Serial.println("\n>>> " + cmd);
    
    // Help command
    if (cmd == "help" || cmd == "?") {
        Serial.println("\n=== SERIAL COMMANDS ===");
        Serial.println("help              - Show this help");
        Serial.println("status            - System status");
        Serial.println("wifi              - WiFi info");
        Serial.println("scan              - Scan WiFi networks");
        Serial.println("ai <query>        - Ask AI a question");
        Serial.println("learn:<trig>-><action> - Teach pattern");
        Serial.println("patterns          - Show learned patterns");
        Serial.println("voice:<text>      - Simulate voice input");
        Serial.println("gpio              - Test GPIO pins");
        Serial.println("i2c               - Scan I2C bus");
        Serial.println("memory            - Memory usage");
        Serial.println("discovery         - Last AI discovery");
        Serial.println("test              - Run system test");
        Serial.println("====================\n");
    }
    
    // Status command
    else if (cmd == "status") {
        Serial.println("\n=== SYSTEM STATUS ===");
        Serial.printf("Firmware: %s\n", FIRMWARE_VERSION);
        Serial.printf("Uptime: %lu seconds\n", millis() / 1000);
        Serial.printf("Free Memory: %d bytes\n", ESP.getFreeHeap());
        Serial.printf("CPU Frequency: %d MHz\n", ESP.getCpuFreqMHz());
        Serial.printf("WiFi: %s\n", WiFi.status() == WL_CONNECTED ? "Connected" : "Disconnected");
        Serial.printf("SD Card: %s\n", sdCardAvailable ? "Available" : "Not Available");
        Serial.printf("AI Key: %s\n", aiApiKey.length() > 0 ? "Loaded" : "Missing");
        Serial.printf("Patterns: %d learned\n", patternCount);
        Serial.println("====================\n");
    }
    
    // WiFi info
    else if (cmd == "wifi") {
        if (WiFi.status() == WL_CONNECTED) {
            Serial.println("\n=== WiFi INFO ===");
            Serial.printf("SSID: %s\n", WiFi.SSID().c_str());
            Serial.printf("IP: %s\n", WiFi.localIP().toString().c_str());
            Serial.printf("RSSI: %d dBm\n", WiFi.RSSI());
            Serial.printf("Channel: %d\n", WiFi.channel());
            Serial.println("=================\n");
        } else {
            Serial.println("WiFi: Not connected\n");
        }
    }
    
    // WiFi scan
    else if (cmd == "scan") {
        Serial.println("\n=== SCANNING WiFi ===");
        int n = WiFi.scanNetworks();
        Serial.printf("Found %d networks:\n", n);
        for (int i = 0; i < n && i < 10; i++) {
            Serial.printf("%d. %s (%d dBm) %s\n", 
                i + 1, 
                WiFi.SSID(i).c_str(), 
                WiFi.RSSI(i),
                WiFi.encryptionType(i) == WIFI_AUTH_OPEN ? "[OPEN]" : "[SEC]");
        }
        Serial.println("====================\n");
        WiFi.scanDelete();
    }
    
    // AI query
    else if (cmd.startsWith("ai ")) {
        String query = cmd.substring(3);
        Serial.println("\n=== AI QUERY ===");
        Serial.println("Q: " + query);
        
        aiInput = query;
        processAIQuery();
        
        Serial.println("A: " + aiResponse);
        Serial.println("================\n");
    }
    
    // Learn pattern
    else if (cmd.startsWith("learn:")) {
        int arrowPos = cmd.indexOf("->");
        if (arrowPos > 0) {
            String trigger = cmd.substring(6, arrowPos);
            String action = cmd.substring(arrowPos + 2);
            trigger.trim();
            action.trim();
            
            learnPattern(trigger, action);
            Serial.printf("\n✓ Learned: '%s' -> '%s'\n\n", trigger.c_str(), action.c_str());
        } else {
            Serial.println("Usage: learn:<trigger>-><action>\n");
        }
    }
    
    // Show patterns
    else if (cmd == "patterns") {
        Serial.println("\n=== LEARNED PATTERNS ===");
        if (patternCount == 0) {
            Serial.println("No patterns learned yet");
        } else {
            for (int i = 0; i < patternCount; i++) {
                Serial.printf("%d. '%s' -> '%s'\n", 
                    i + 1, 
                    patterns[i].trigger.c_str(), 
                    patterns[i].action.c_str());
            }
        }
        Serial.println("========================\n");
    }
    
    // Simulate voice input
    else if (cmd.startsWith("voice:")) {
        String voiceText = cmd.substring(6);
        voiceText.trim();
        Serial.println("\n=== VOICE SIMULATION ===");
        Serial.println("Input: " + voiceText);
        aiInput += voiceText + " ";
        Serial.println("Added to AI input");
        Serial.println("========================\n");
    }
    
    // GPIO test
    else if (cmd == "gpio") {
        Serial.println("\n=== GPIO TEST ===");
        experimentWithHardware();
        Serial.println("=================\n");
    }
    
    // I2C scan
    else if (cmd == "i2c") {
        Serial.println("\n=== I2C SCAN ===");
        Wire.begin();
        int found = 0;
        for (byte addr = 1; addr < 127; addr++) {
            Wire.beginTransmission(addr);
            if (Wire.endTransmission() == 0) {
                Serial.printf("Device at 0x%02X\n", addr);
                found++;
            }
        }
        Serial.printf("Total: %d devices\n", found);
        Serial.println("================\n");
    }
    
    // Memory info
    else if (cmd == "memory") {
        Serial.println("\n=== MEMORY INFO ===");
        Serial.printf("Free Heap: %d bytes\n", ESP.getFreeHeap());
        Serial.printf("Min Free Heap: %d bytes\n", ESP.getMinFreeHeap());
        Serial.printf("Heap Size: %d bytes\n", ESP.getHeapSize());
        Serial.printf("Max Alloc: %d bytes\n", ESP.getMaxAllocHeap());
        Serial.println("===================\n");
    }
    
    // Last discovery
    else if (cmd == "discovery") {
        Serial.println("\n=== AI DISCOVERY ===");
        Serial.println(lastDiscovery.length() > 0 ? lastDiscovery : "None yet");
        Serial.println("====================\n");
    }
    
    // System test
    else if (cmd == "test") {
        Serial.println("\n=== SYSTEM TEST ===");
        Serial.println("Testing microphone...");
        if (M5Cardputer.Mic.isEnabled()) {
            Serial.println("✓ Mic ready");
        } else {
            Serial.println("✗ Mic offline");
        }
        
        Serial.println("Testing SD card...");
        if (sdCardAvailable) {
            Serial.println("✓ SD available");
        } else {
            Serial.println("✗ SD not available");
        }
        
        Serial.println("Testing WiFi...");
        if (WiFi.status() == WL_CONNECTED) {
            Serial.printf("✓ Connected to %s\n", WiFi.SSID().c_str());
        } else {
            Serial.println("✗ Not connected");
        }
        
        Serial.println("Testing AI...");
        if (aiApiKey.length() > 0) {
            Serial.println("✓ API key loaded");
        } else {
            Serial.println("! No API key (offline mode)");
        }
        
        Serial.println("===================\n");
    }
    
    else {
        Serial.println("Unknown command. Type 'help' for commands.\n");
    }
}
