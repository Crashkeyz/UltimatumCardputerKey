#include <Arduino.h>
#include <M5Unified.h>
#include <M5GFX.h>
#include "../drivers/cardputer_adv_driver.h"
#include "../lib/storage_manager/storage_manager.h"
#include "../lib/storage_manager/sd_logger.h"

// Global storage manager and logger
StorageManager storage;
SDLogger logger;

// SD card status tracking
bool sdCardAvailable = false;
String sdCardStatus = "CHECKING...";
uint64_t sdFreeSpace = 0;
uint64_t sdTotalSpace = 0;

// Constants for better readability
// Note: These are defined here for module independence. The storage_manager
// also defines MB_DIVISOR internally for its own use.
const uint64_t MB_DIVISOR = 1024 * 1024;
const uint64_t KB_DIVISOR = 1024;

/**
 * Initialize SD card storage system
 */
void initializeStorage() {
    Serial.println("\n========== SD CARD INITIALIZATION ==========");
    
    // Initialize storage manager
    if (storage.begin()) {
        sdCardAvailable = true;
        sdCardStatus = "OK";
        sdFreeSpace = storage.getFreeSpace();
        sdTotalSpace = storage.getTotalSpace();
        
        // Create default directory structure
        storage.createDefaultDirectories();
        
        // Initialize logger
        logger.begin(&storage);
        logger.info("System initialized successfully");
        logger.info("SD card storage ready");
        
        Serial.println("[Main] SD card initialized successfully");
    } else {
        sdCardAvailable = false;
        sdCardStatus = "MISSING";
        Serial.println("[Main] WARNING: SD card not available - using internal storage fallback");
    }
    
    Serial.println("============================================\n");
}

/**
 * Display SD card status on screen
 */
void displaySDStatus() {
    // Clear status area
    M5.Display.fillRect(0, 90, 240, 45, BLACK);
    
    // Display SD card icon and status
    M5.Display.setCursor(10, 90);
    M5.Display.setTextSize(1);
    
    if (sdCardAvailable) {
        M5.Display.setTextColor(GREEN);
        M5.Display.print("SD: ");
        M5.Display.setTextColor(WHITE);
        M5.Display.println(sdCardStatus);
        
        // Display storage info
        M5.Display.setCursor(10, 100);
        uint64_t freeMB = sdFreeSpace / MB_DIVISOR;
        uint64_t totalMB = sdTotalSpace / MB_DIVISOR;
        M5.Display.printf("Free: %llu MB / %llu MB", freeMB, totalMB);
        
        // Display card type
        M5.Display.setCursor(10, 110);
        M5.Display.print("Type: ");
        M5.Display.println(storage.getCardType());
    } else {
        M5.Display.setTextColor(RED);
        M5.Display.print("SD: ");
        M5.Display.println(sdCardStatus);
        M5.Display.setTextColor(YELLOW);
        M5.Display.setCursor(10, 100);
        M5.Display.println("Insert SD card");
    }
    
    M5.Display.setTextSize(2);
}

/**
 * Example: Write test data to SD card
 */
void testSDWrite() {
    if (!sdCardAvailable) {
        Serial.println("[Test] SD card not available");
        return;
    }
    
    Serial.println("[Test] Writing test file...");
    logger.info("Testing SD card write operation");
    
    String testData = "Hello from M5Stack Cardputer!\n";
    testData += "Timestamp: " + String(millis()) + " ms\n";
    testData += "Free space: " + String(sdFreeSpace / KB_DIVISOR) + " KB\n";
    
    if (storage.writeFile("/data/test.txt", testData.c_str())) {
        Serial.println("[Test] Test file written successfully");
        logger.info("Test file written successfully");
        
        // Display success on screen
        M5.Display.fillRect(0, 60, 240, 20, BLACK);
        M5.Display.setCursor(10, 60);
        M5.Display.setTextColor(GREEN);
        M5.Display.println("File written!");
        M5.Display.setTextColor(WHITE);
    } else {
        Serial.println("[Test] Failed to write test file");
        logger.error("Failed to write test file");
    }
}

/**
 * Example: Read test data from SD card
 */
void testSDRead() {
    if (!sdCardAvailable) {
        Serial.println("[Test] SD card not available");
        return;
    }
    
    Serial.println("[Test] Reading test file...");
    logger.info("Testing SD card read operation");
    
    String data;
    if (storage.readFile("/data/test.txt", data)) {
        Serial.println("[Test] Test file read successfully:");
        Serial.println(data);
        logger.info("Test file read successfully");
        
        // Display success on screen
        M5.Display.fillRect(0, 60, 240, 20, BLACK);
        M5.Display.setCursor(10, 60);
        M5.Display.setTextColor(CYAN);
        M5.Display.println("File read!");
        M5.Display.setTextColor(WHITE);
    } else {
        Serial.println("[Test] Failed to read test file");
        logger.error("Failed to read test file");
    }
}

/**
 * Example: List files in directory
 */
void testSDList() {
    if (!sdCardAvailable) {
        Serial.println("[Test] SD card not available");
        return;
    }
    
    Serial.println("[Test] Listing /data directory:");
    logger.info("Listing directory contents");
    storage.listDir("/data", false);
}

/**
 * Load configuration from SD card or use defaults
 */
void loadConfiguration() {
    if (sdCardAvailable && storage.fileExists("/config/settings.json")) {
        String configData;
        if (storage.readFile("/config/settings.json", configData)) {
            Serial.println("[Config] Loaded configuration from SD card");
            logger.info("Configuration loaded from SD card");
            // Parse JSON configuration here
        }
    } else {
        Serial.println("[Config] Using default configuration");
        logger.info("Using default configuration");
        
        // Create default configuration on SD card if available
        if (sdCardAvailable) {
            String defaultConfig = "{\n";
            defaultConfig += "  \"device_name\": \"UltimatumCardputerKey\",\n";
            defaultConfig += "  \"version\": \"1.0.0\",\n";
            defaultConfig += "  \"sd_storage\": true,\n";
            defaultConfig += "  \"settings\": {}\n";
            defaultConfig += "}\n";
            
            if (storage.writeFile("/config/settings.json", defaultConfig.c_str())) {
                Serial.println("[Config] Default configuration created on SD card");
                logger.info("Default configuration created on SD card");
            }
        }
    }
}

void setup() {
    // Initialize USB Serial for debugging
    Serial.begin(115200);
    delay(500);
    Serial.println("\n\n========================================");
    Serial.println("   M5Stack Cardputer ADV with SD");
    Serial.println("========================================\n");
    
    // Configure M5Unified for Cardputer
    auto cfg = M5.config();
    cfg.clear_display = true;
    cfg.internal_imu = false;
    cfg.internal_rtc = true;
    cfg.internal_spk = true;
    cfg.internal_mic = false;
    
    // Initialize M5Stack system
    M5.begin(cfg);
    
    // Initialize display
    M5.Display.setRotation(1);
    M5.Display.setTextSize(2);
    M5.Display.fillScreen(BLACK);
    M5.Display.setCursor(10, 10);
    M5.Display.setTextColor(GREEN);
    M5.Display.println("Cardputer ADV");
    M5.Display.setTextColor(WHITE);
    M5.Display.println("with SD Storage");
    
    // Initialize SD card storage
    initializeStorage();
    
    // Display SD card status
    displaySDStatus();
    
    // Load configuration
    loadConfiguration();
    
    // Initialize custom driver
    initialize_driver();
    
    Serial.println("\n[Main] Setup complete!");
    Serial.println("[Main] Press 'w' to write test file");
    Serial.println("[Main] Press 'r' to read test file");
    Serial.println("[Main] Press 'l' to list files");
    Serial.println("[Main] Press 's' to show SD status\n");
    
    if (sdCardAvailable) {
        logger.info("Setup complete - ready for operation");
    }
}

void loop() {
    // Update M5Unified state (keyboard, power, etc)
    M5.update();
    
    // Handle keyboard input
    if (M5.Keyboard.isChange()) {
        if (M5.Keyboard.isPressed()) {
            auto keycode = M5.Keyboard.getKeyCode();
            char key = (char)keycode;
            
            Serial.printf("[Main] Key pressed: 0x%02X ('%c')\n", keycode, key);
            
            // Handle special commands
            switch (key) {
                case 'w':
                case 'W':
                    testSDWrite();
                    break;
                    
                case 'r':
                case 'R':
                    testSDRead();
                    break;
                    
                case 'l':
                case 'L':
                    testSDList();
                    break;
                    
                case 's':
                case 'S':
                    displaySDStatus();
                    break;
                    
                default:
                    // Display key on screen
                    M5.Display.fillRect(0, 60, 240, 20, BLACK);
                    M5.Display.setCursor(10, 60);
                    M5.Display.printf("Key: 0x%02X", keycode);
                    break;
            }
        }
    }
    
    delay(10);
}
