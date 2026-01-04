#ifndef DISPLAYUTILS_H
#define DISPLAYUTILS_H

#include <M5Cardputer.h>

class DisplayUtils {
public:
    // Text wrapping and rendering
    static void printWrapped(const String& text, int x, int y, int maxWidth, int lineHeight, uint16_t color = TFT_WHITE);
    static void printWrappedWithPrefix(const String& prefix, const String& text, int x, int y, int maxWidth, int lineHeight, uint16_t prefixColor, uint16_t textColor);
    
    // Borders and boxes
    static void drawBorder(int x, int y, int w, int h, uint16_t color);
    static void drawThickBorder(int x, int y, int w, int h, uint16_t color, int thickness = 3);
    
    // Headers
    static void drawHeader(const String& title, uint16_t color = TFT_RED);
    
    // Signal strength indicator
    static uint16_t getRSSIColor(int rssi);
    static String getRSSIBar(int rssi);
    
    // Screen clearing helpers
    static void clearArea(int x, int y, int w, int h);
    
private:
    DisplayUtils() {}  // Static class
};

#endif // DISPLAYUTILS_H
