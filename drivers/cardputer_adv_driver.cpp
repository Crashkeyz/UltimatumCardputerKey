#include <Arduino.h>
#include <M5Unified.h>
#include <SD.h>
#include <FS.h>
#include <SPI.h>
#include <WiFi.h>
#include "cardputer_adv_driver.h"

// SD Card configuration for M5Stack Cardputer
#define SD_CS_PIN GPIO_NUM_4
#define SD_SCK_PIN GPIO_NUM_37
#define SD_MISO_PIN GPIO_NUM_36
#define SD_MOSI_PIN GPIO_NUM_35
#define SD_SPI_FREQ 25000000  // 25MHz

// Dedicated SPI instance for SD card (HSPI) to avoid TFT display conflicts
SPIClass sdcardSPI(HSPI);

bool initialize_sd_card() {
    Serial.println("Initializing SD card...");
    
    // Note: This function should be called after M5.begin() in setup()
    // Create dedicated HSPI bus for SD card to avoid conflicts with M5Unified's TFT display
    sdcardSPI.begin(SD_SCK_PIN, SD_MISO_PIN, SD_MOSI_PIN, SD_CS_PIN);
    
    // Try to mount SD card with dedicated SPI bus and frequency
    if (!SD.begin(SD_CS_PIN, sdcardSPI, SD_SPI_FREQ)) {
        Serial.println("SD Card mount failed or not present");
        Serial.println("Note: Insert SD card and restart if you want to use external storage");
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
    
    // Create required directory structure for security toolkit
    const char* directories[] = {"/data", "/keys", "/logs", "/captures", "/loot", "/firmware"};
    for (int i = 0; i < 6; i++) {
        if (!SD.exists(directories[i])) {
            SD.mkdir(directories[i]);
            Serial.printf("Created %s directory on SD card\n", directories[i]);
        } else {
            Serial.printf("Directory %s already exists\n", directories[i]);
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
    sdCardAvailable = initialize_sd_card();
    if (sdCardAvailable) {
        Serial.println("SD card available for data storage");
        Serial.println("Use SD card to store large files and reduce flash memory usage");
    } else {
        Serial.println("SD card not available - using internal flash only");
        Serial.println("To use SD card: Insert card and restart device");
    }
    
    // Initialize WiFi in station mode (ready for scanning)
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);
    
    Serial.println("Driver initialized successfully");
    
    // Display main menu
    display_main_menu();
}

// ========================================
// MENU SYSTEM & UI
// ========================================

enum MenuState {
    MENU_MAIN,
    MENU_WIFI,
    MENU_RF,
    MENU_LORA,
    MENU_SYSTEM,
    MENU_WIFI_SCAN,
    MENU_WIFI_DEAUTH,
    MENU_WIFI_BEACON,
    MENU_WIFI_EVIL_PORTAL,
    MENU_WIFI_PROMISCUOUS
};

MenuState currentMenu = MENU_MAIN;
int menuSelection = 0;
bool sdCardAvailable = false;

void display_status_bar() {
    M5.Display.fillRect(0, 0, 240, 16, TFT_DARKGREY);
    M5.Display.setTextColor(TFT_WHITE);
    M5.Display.setTextSize(1);
    
    // SD card status
    M5.Display.setCursor(2, 4);
    M5.Display.print(sdCardAvailable ? "[SD]" : "[--]");
    
    // WiFi status
    M5.Display.setCursor(40, 4);
    M5.Display.print(WiFi.status() == WL_CONNECTED ? "[W+]" : "[W-]");
    
    // Free heap
    M5.Display.setCursor(140, 4);
    M5.Display.printf("%dK", ESP.getFreeHeap() / 1024);
}

void display_main_menu() {
    M5.Display.fillScreen(TFT_BLACK);
    display_status_bar();
    
    M5.Display.setTextSize(2);
    M5.Display.setTextColor(TFT_GREEN);
    M5.Display.setCursor(10, 25);
    M5.Display.println("SKELETON KEY");
    
    M5.Display.setTextSize(1);
    M5.Display.setTextColor(TFT_WHITE);
    
    const char* menuItems[] = {
        "1. WiFi Attacks",
        "2. RF/SubGHz",
        "3. LoRa Module",
        "4. System Info"
    };
    
    for (int i = 0; i < 4; i++) {
        M5.Display.setCursor(10, 50 + i * 15);
        if (i == menuSelection) {
            M5.Display.setTextColor(TFT_YELLOW);
            M5.Display.print("> ");
        } else {
            M5.Display.setTextColor(TFT_WHITE);
            M5.Display.print("  ");
        }
        M5.Display.println(menuItems[i]);
    }
    
    M5.Display.setCursor(10, 120);
    M5.Display.setTextColor(TFT_CYAN);
    M5.Display.println("UP/DOWN: Navigate");
    M5.Display.println("ENTER: Select  ESC: Back");
}

void display_wifi_menu() {
    M5.Display.fillScreen(TFT_BLACK);
    display_status_bar();
    
    M5.Display.setTextSize(2);
    M5.Display.setTextColor(TFT_CYAN);
    M5.Display.setCursor(10, 25);
    M5.Display.println("WiFi Attacks");
    
    M5.Display.setTextSize(1);
    M5.Display.setTextColor(TFT_WHITE);
    
    const char* menuItems[] = {
        "1. WiFi Scan",
        "2. Deauth Attack",
        "3. Beacon Spam",
        "4. Evil Portal",
        "5. Promiscuous Mode"
    };
    
    for (int i = 0; i < 5; i++) {
        M5.Display.setCursor(10, 50 + i * 12);
        if (i == menuSelection) {
            M5.Display.setTextColor(TFT_YELLOW);
            M5.Display.print("> ");
        } else {
            M5.Display.setTextColor(TFT_WHITE);
            M5.Display.print("  ");
        }
        M5.Display.println(menuItems[i]);
    }
}

void wifi_scan() {
    M5.Display.fillScreen(TFT_BLACK);
    display_status_bar();
    
    M5.Display.setTextSize(1);
    M5.Display.setCursor(10, 25);
    M5.Display.setTextColor(TFT_CYAN);
    M5.Display.println("Scanning WiFi...");
    
    Serial.println("Starting WiFi scan...");
    int n = WiFi.scanNetworks();
    
    M5.Display.fillRect(0, 40, 240, 95, TFT_BLACK);
    M5.Display.setCursor(10, 40);
    M5.Display.setTextColor(TFT_GREEN);
    M5.Display.printf("Found %d networks\n\n", n);
    
    M5.Display.setTextColor(TFT_WHITE);
    for (int i = 0; i < min(n, 6); i++) {
        M5.Display.printf("%d: %s (%ddBm)\n", i + 1, 
                          WiFi.SSID(i).c_str(), 
                          WiFi.RSSI(i));
        Serial.printf("Network %d: %s (RSSI: %d, Channel: %d)\n", 
                      i + 1, WiFi.SSID(i).c_str(), WiFi.RSSI(i), WiFi.channel(i));
    }
    
    M5.Display.setCursor(10, 120);
    M5.Display.setTextColor(TFT_CYAN);
    M5.Display.println("Press any key to return");
    
    // Save to SD card if available
    if (sdCardAvailable) {
        File logFile = SD.open("/logs/wifi_scan.txt", FILE_APPEND);
        if (logFile) {
            logFile.printf("=== WiFi Scan %lu ===\n", millis());
            for (int i = 0; i < n; i++) {
                logFile.printf("%s,%d,%d\n", WiFi.SSID(i).c_str(), WiFi.RSSI(i), WiFi.channel(i));
            }
            logFile.close();
            Serial.println("Scan results saved to SD card");
        }
    }
}

void display_system_info() {
    M5.Display.fillScreen(TFT_BLACK);
    display_status_bar();
    
    M5.Display.setTextSize(1);
    M5.Display.setTextColor(TFT_CYAN);
    M5.Display.setCursor(10, 25);
    M5.Display.println("System Information");
    
    M5.Display.setTextColor(TFT_WHITE);
    M5.Display.setCursor(10, 40);
    M5.Display.printf("Chip: ESP32-S3\n");
    M5.Display.printf("Flash: %d MB\n", ESP.getFlashChipSize() / (1024 * 1024));
    M5.Display.printf("PSRAM: %d KB\n", ESP.getPsramSize() / 1024);
    M5.Display.printf("Free Heap: %d KB\n", ESP.getFreeHeap() / 1024);
    M5.Display.printf("CPU Freq: %d MHz\n", ESP.getCpuFreqMHz());
    M5.Display.printf("\nSD Card: %s\n", sdCardAvailable ? "Available" : "Not detected");
    M5.Display.printf("WiFi: %s\n", WiFi.status() == WL_CONNECTED ? "Connected" : "Disconnected");
    
    M5.Display.setCursor(10, 120);
    M5.Display.setTextColor(TFT_CYAN);
    M5.Display.println("Press any key to return");
}

void driver_loop() {
    M5.update();
    
    if (M5.Keyboard.isChange() && M5.Keyboard.isPressed()) {
        auto keycode = M5.Keyboard.getKeyCode();
        char key = (char)keycode;
        
        Serial.printf("Key pressed: 0x%02X (%c)\n", keycode, key >= 32 && key < 127 ? key : '?');
        
        // Handle navigation based on current menu
        switch (currentMenu) {
            case MENU_MAIN:
                if (keycode == 0x52 || key == 'w') { // Up arrow or W
                    menuSelection = (menuSelection - 1 + 4) % 4;
                    display_main_menu();
                } else if (keycode == 0x51 || key == 's') { // Down arrow or S
                    menuSelection = (menuSelection + 1) % 4;
                    display_main_menu();
                } else if (keycode == 0x28 || key == '\n' || key == '\r') { // Enter
                    if (menuSelection == 0) {
                        currentMenu = MENU_WIFI;
                        menuSelection = 0;
                        display_wifi_menu();
                    } else if (menuSelection == 1) {
                        M5.Display.fillScreen(TFT_BLACK);
                        M5.Display.setCursor(10, 50);
                        M5.Display.setTextColor(TFT_YELLOW);
                        M5.Display.println("RF/SubGHz module");
                        M5.Display.println("not yet implemented");
                        delay(2000);
                        display_main_menu();
                    } else if (menuSelection == 2) {
                        M5.Display.fillScreen(TFT_BLACK);
                        M5.Display.setCursor(10, 50);
                        M5.Display.setTextColor(TFT_YELLOW);
                        M5.Display.println("LoRa module");
                        M5.Display.println("not yet implemented");
                        delay(2000);
                        display_main_menu();
                    } else if (menuSelection == 3) {
                        display_system_info();
                        currentMenu = MENU_SYSTEM;
                    }
                }
                break;
                
            case MENU_WIFI:
                if (keycode == 0x52 || key == 'w') { // Up arrow or W
                    menuSelection = (menuSelection - 1 + 5) % 5;
                    display_wifi_menu();
                } else if (keycode == 0x51 || key == 's') { // Down arrow or S
                    menuSelection = (menuSelection + 1) % 5;
                    display_wifi_menu();
                } else if (keycode == 0x28 || key == '\n' || key == '\r') { // Enter
                    if (menuSelection == 0) {
                        wifi_scan();
                        currentMenu = MENU_WIFI_SCAN;
                    } else {
                        M5.Display.fillScreen(TFT_BLACK);
                        M5.Display.setCursor(10, 50);
                        M5.Display.setTextColor(TFT_YELLOW);
                        M5.Display.println("Feature not yet");
                        M5.Display.println("implemented");
                        delay(2000);
                        display_wifi_menu();
                    }
                } else if (keycode == 0x29 || key == 0x1B) { // ESC
                    currentMenu = MENU_MAIN;
                    menuSelection = 0;
                    display_main_menu();
                }
                break;
                
            case MENU_WIFI_SCAN:
            case MENU_SYSTEM:
                // Any key returns to previous menu
                if (currentMenu == MENU_WIFI_SCAN) {
                    currentMenu = MENU_WIFI;
                    menuSelection = 0;
                    display_wifi_menu();
                } else {
                    currentMenu = MENU_MAIN;
                    menuSelection = 0;
                    display_main_menu();
                }
                break;
        }
    }
}
