#include <Arduino.h>
#include <M5Unified.h>

// External driver initialization
extern void initialize_driver();

void setup() {
    // Initialize serial for debugging (before M5.begin for early debug output)
    Serial.begin(115200);
    delay(100);
    
    Serial.println("UltimatumCardputerKey Starting...");
    
    // Initialize M5Stack Cardputer
    auto cfg = M5.config();
    M5.begin(cfg);
    
    // Initialize display
    M5.Display.setRotation(1);
    M5.Display.setTextSize(2);
    M5.Display.println("Cardputer Ready");
    
    // Initialize driver
    initialize_driver();
    
    Serial.println("Initialization complete");
}

void loop() {
    // Update M5 state (handles keyboard, display, and other peripherals)
    M5.update();
    
    // Main loop logic here
    // TODO: Implement keyboard input handling
    // TODO: Implement display updates
    // TODO: Add main application functionality
    
    delay(10);
}