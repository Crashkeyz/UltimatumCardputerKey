#include "KeyboardUtils.h"

// Static member initialization
bool KeyboardUtils::shiftPressed = false;
bool KeyboardUtils::ctrlPressed = false;
bool KeyboardUtils::altPressed = false;
bool KeyboardUtils::optPressed = false;

char KeyboardUtils::keymap[9][8] = {
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

void KeyboardUtils::initialize(Adafruit_TCA8418& tca) {
    tca.begin();
    tca.matrix(7, 8);
    tca.flush();
}

char KeyboardUtils::readKey(Adafruit_TCA8418& tca) {
    if (tca.available() > 0) {
        int key = tca.getEvent();
        bool pressed = !(key & 0x80);
        key &= 0x7F;
        
        uint8_t row = key / 10;
        uint8_t col = key % 10;
        
        if (pressed) {
            return applyKeymap(row, col);
        }
    }
    return 0;
}

char KeyboardUtils::applyKeymap(uint8_t row, uint8_t col) {
    if (row >= 9 || col >= 8) {
        return 0;
    }
    
    char ch = keymap[row][col];
    
    // Apply shift for letters
    if (shiftPressed && ch >= 'a' && ch <= 'z') {
        ch = ch - 32;  // Convert to uppercase
    }
    
    // Apply shift for numbers and symbols
    if (shiftPressed) {
        switch (ch) {
            case '1': ch = '!'; break;
            case '2': ch = '@'; break;
            case '3': ch = '#'; break;
            case '4': ch = '$'; break;
            case '5': ch = '%'; break;
            case '6': ch = '^'; break;
            case '7': ch = '&'; break;
            case '8': ch = '*'; break;
            case '9': ch = '('; break;
            case '0': ch = ')'; break;
            case '-': ch = '_'; break;
            case '=': ch = '+'; break;
            case '[': ch = '{'; break;
            case ']': ch = '}'; break;
            case ';': ch = ':'; break;
            case '\'': ch = '"'; break;
            case ',': ch = '<'; break;
            case '.': ch = '>'; break;
            case '/': ch = '?'; break;
            case '\\': ch = '|'; break;
        }
    }
    
    return ch;
}
