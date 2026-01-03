#include <Arduino.h>
#include <M5Unified.h>
#include <M5GFX.h>
#include "../drivers/cardputer_adv_driver.h"
#include "../lib/firmware_sideload.h"
#include "../lib/evil_portal.h"

bool portalMode = false;

void setup() {
    // Initialize USB Serial for debugging
    Serial.begin(115200);
    delay(500);
    Serial.println(F("Cardputer ADV initializing..."));
    
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
    M5.Display.println(F("Cardputer ADV"));
    M5.Display.setTextColor(WHITE);
    M5.Display.println(F("Firmware Ready"));
    
    // Initialize custom driver (includes SD card)
    initialize_driver();
    
    // Initialize firmware sideload system
    init_firmware_sideload();
    
    // Check for firmware update on SD card
    if (check_and_update_from_sd()) {
        // If update successful, device will reboot
        // This line only executes if no update found
        Serial.println(F("No firmware update found"));
    }
    
    M5.Display.println(F("\nPress 'P' for Portal"));
    M5.Display.println(F("Press 'F' for Files"));
    
    Serial.println(F("Setup complete!"));
    Serial.println(F("Press 'P' for Evil Portal"));
    Serial.println(F("Press 'F' for firmware files"));
}

void loop() {
    // Update M5Unified state (keyboard, power, etc)
    M5.update();
    
    // Handle Evil Portal if active
    if (portalMode) {
        handle_evil_portal();
    }
    
    // Example: Read keyboard input
    if (M5.Keyboard.isChange()) {
        if (M5.Keyboard.isPressed()) {
            auto keycode = M5.Keyboard.getKeyCode();
            char key = (char)keycode;
            Serial.printf("Key pressed: 0x%02X (%c)\n", keycode, key);
            
            // Handle special keys
            if (key == 'P' || key == 'p') {
                if (!portalMode) {
                    M5.Display.fillScreen(BLACK);
                    M5.Display.setCursor(10, 10);
                    M5.Display.setTextColor(CYAN);
                    M5.Display.println(F("Starting Portal..."));
                    
                    if (init_evil_portal()) {
                        portalMode = true;
                        M5.Display.setTextColor(GREEN);
                        M5.Display.println(F("Portal Active!"));
                        M5.Display.setTextColor(WHITE);
                        M5.Display.println(F("\nConnect to WiFi:"));
                        M5.Display.println(F("Cardputer-Portal"));
                        M5.Display.println(F("\nPress 'S' to stop"));
                    } else {
                        M5.Display.setTextColor(RED);
                        M5.Display.println(F("Failed to start"));
                    }
                }
            } else if (key == 'S' || key == 's') {
                if (portalMode) {
                    stop_evil_portal();
                    portalMode = false;
                    M5.Display.fillScreen(BLACK);
                    M5.Display.setCursor(10, 10);
                    M5.Display.setTextColor(GREEN);
                    M5.Display.println(F("Portal Stopped"));
                    M5.Display.setTextColor(WHITE);
                    M5.Display.println(F("\nPress 'P' for Portal"));
                }
            } else if (key == 'F' || key == 'f') {
                M5.Display.fillScreen(BLACK);
                M5.Display.setCursor(10, 10);
                M5.Display.setTextColor(CYAN);
                M5.Display.println(F("Firmware Files:"));
                M5.Display.setTextColor(WHITE);
                list_firmware_files();
                delay(3000);
                M5.Display.fillScreen(BLACK);
                M5.Display.setCursor(10, 10);
                M5.Display.println(F("Press 'P' for Portal"));
                M5.Display.println(F("Press 'F' for Files"));
            }
            
            // Display key on screen (if not in portal mode)
            if (!portalMode) {
                M5.Display.fillRect(0, 100, 240, 30, BLACK);
                M5.Display.setCursor(10, 100);
                M5.Display.printf("Key: 0x%02X", keycode);
            }
        }
    }
    
    delay(10);
}
