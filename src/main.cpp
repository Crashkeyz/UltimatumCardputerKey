#include <Arduino.h>
#include <M5Unified.h>
#include <M5GFX.h>
#include <M5Cardputer.h>
#include "../drivers/cardputer_adv_driver.h"

void setup() {
    // Initialize USB Serial for debugging
    Serial.begin(115200);
    delay(500);
    Serial.println("Cardputer ADV initializing...");
    
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
    M5.Display.println("Firmware Ready");
    
    // Initialize custom driver
    initialize_driver();
    
    Serial.println("Setup complete!");
}

void loop() {
    // Update M5Unified state (keyboard, power, etc)
    M5.update();
    
    // Call driver loop for menu system and continuous operation
    driver_loop();
    
    // Example: Read keyboard input
    if (M5.Keyboard.isChange()) {
        if (M5.Keyboard.isPressed()) {
            auto keycode = M5.Keyboard.getKeyCode();
            Serial.printf("Key pressed: 0x%02X\n", keycode);
            
            // Display key on screen
            M5.Display.fillRect(0, 60, 240, 30, BLACK);
            M5.Display.setCursor(10, 60);
            M5.Display.printf("Key: 0x%02X", keycode);
        }
    }
    
    delay(10);
}
