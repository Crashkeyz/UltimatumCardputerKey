#include <Arduino.h>
#include <M5Cardputer.h>
#include <cardputer_adv_driver.h>

// Cardputer ADV backlight pin
#define TFT_BL 38

void setup() {
    // Initialize USB Serial
    Serial.begin(115200);
    Serial.println("Cardputer ADV initializing...");
    
    // Enable backlight
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);
    
    // Initialize M5Cardputer with keyboard support
    auto cfg = M5.config();
    M5Cardputer.begin(cfg, true);
    
    // Setup display
    M5Cardputer.Display.setRotation(1);
    M5Cardputer.Display.fillScreen(TFT_BLACK);
    M5Cardputer.Display.setTextSize(2);
    M5Cardputer.Display.setCursor(10, 10);
    M5Cardputer.Display.setTextColor(TFT_GREEN, TFT_BLACK);
    M5Cardputer.Display.println("Cardputer ADV");
    M5Cardputer.Display.setTextColor(TFT_WHITE, TFT_BLACK);
    M5Cardputer.Display.println("Firmware Ready");
    
    // Initialize driver (SD card)
    initialize_driver();
    
    Serial.println("Setup complete!");
}

void loop() {
    M5Cardputer.update();
    
    // Check keyboard using M5Cardputer library
    if (M5Cardputer.Keyboard.isChange() && M5Cardputer.Keyboard.isPressed()) {
        Keyboard_Class::KeysState status = M5Cardputer.Keyboard.keysState();
        
        for (auto key : status.word) {
            Serial.printf("Key: '%c'\n", key);
            
            // Display key on screen
            M5Cardputer.Display.fillRect(0, 60, 240, 30, TFT_BLACK);
            M5Cardputer.Display.setCursor(10, 60);
            M5Cardputer.Display.setTextColor(TFT_CYAN, TFT_BLACK);
            M5Cardputer.Display.printf("Key: %c", key);
        }
    }
    
    // Check GO button manually (GPIO0)
    static bool lastBtn = HIGH;
    pinMode(0, INPUT_PULLUP);
    bool btnState = digitalRead(0);
    
    if (btnState == LOW && lastBtn == HIGH) {
        Serial.println("GO button pressed");
        M5Cardputer.Display.fillRect(0, 60, 240, 30, TFT_YELLOW);
        M5Cardputer.Display.setCursor(10, 60);
        M5Cardputer.Display.setTextColor(TFT_BLACK, TFT_YELLOW);
        M5Cardputer.Display.print("BTN GO!");
    }
    if (btnState == HIGH && lastBtn == LOW) {
        M5Cardputer.Display.fillRect(0, 60, 240, 30, TFT_BLACK);
    }
    lastBtn = btnState;
    
    delay(10);
}
