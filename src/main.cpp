#include <Arduino.h>
#include <M5Unified.h>

// External driver initialization
extern void initialize_driver();

void setup() {
    // Initialize M5Stack Cardputer
    auto cfg = M5.config();
    M5.begin(cfg);
    
    // Initialize serial for debugging
    Serial.begin(115200);
    delay(100);
    
    Serial.println("UltimatumCardputerKey Starting...");
    
    // Initialize display
    M5.Display.setRotation(1);
    M5.Display.setTextSize(2);
    M5.Display.println("Cardputer Ready");
    
    // Initialize driver
    initialize_driver();
    
    Serial.println("Initialization complete");
}

void loop() {
    // Update M5 state
    M5.update();
    
    // Main loop logic here
    delay(10);
}