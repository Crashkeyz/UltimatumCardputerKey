#ifndef KEYBOARDUTILS_H
#define KEYBOARDUTILS_H

#include <Arduino.h>
#include <utility/Adafruit_TCA8418/Adafruit_TCA8418.h>

class KeyboardUtils {
public:
    static void initialize(Adafruit_TCA8418& tca);
    static char readKey(Adafruit_TCA8418& tca);
    static char applyKeymap(uint8_t row, uint8_t col);
    
    // Modifier states
    static bool isShiftPressed() { return shiftPressed; }
    static bool isCtrlPressed() { return ctrlPressed; }
    static bool isAltPressed() { return altPressed; }
    static bool isOptPressed() { return optPressed; }
    
    static void setShiftPressed(bool pressed) { shiftPressed = pressed; }
    static void setCtrlPressed(bool pressed) { ctrlPressed = pressed; }
    static void setAltPressed(bool pressed) { altPressed = pressed; }
    static void setOptPressed(bool pressed) { optPressed = pressed; }
    
private:
    static bool shiftPressed;
    static bool ctrlPressed;
    static bool altPressed;
    static bool optPressed;
    
    static char keymap[9][8];
};

#endif // KEYBOARDUTILS_H
