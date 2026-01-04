#include "DisplayUtils.h"

void DisplayUtils::printWrapped(const String& text, int x, int y, int maxWidth, int lineHeight, uint16_t color) {
    M5Cardputer.Display.setTextColor(color, TFT_BLACK);
    
    int yPos = y;
    int charIdx = 0;
    int charsPerLine = maxWidth / 6;  // Approximate chars per line for size 1, adjust based on text size
    
    while (charIdx < text.length()) {
        int endIdx = min(charIdx + charsPerLine, (int)text.length());
        
        // Try to break at word boundary
        if (endIdx < text.length() && text[endIdx] != ' ') {
            int lastSpace = text.lastIndexOf(' ', endIdx);
            if (lastSpace > charIdx) {
                endIdx = lastSpace;
            }
        }
        
        String line = text.substring(charIdx, endIdx);
        line.trim();
        
        M5Cardputer.Display.setCursor(x, yPos);
        M5Cardputer.Display.print(line);
        
        yPos += lineHeight;
        charIdx = endIdx;
        
        // Skip leading spaces on next line
        while (charIdx < text.length() && text[charIdx] == ' ') {
            charIdx++;
        }
    }
}

void DisplayUtils::printWrappedWithPrefix(const String& prefix, const String& text, int x, int y, int maxWidth, int lineHeight, uint16_t prefixColor, uint16_t textColor) {
    M5Cardputer.Display.setCursor(x, y);
    M5Cardputer.Display.setTextColor(prefixColor, TFT_BLACK);
    M5Cardputer.Display.print(prefix);
    
    int prefixWidth = prefix.length() * 6;  // Approximate
    printWrapped(text, x + prefixWidth, y, maxWidth - prefixWidth, lineHeight, textColor);
}

void DisplayUtils::drawBorder(int x, int y, int w, int h, uint16_t color) {
    M5Cardputer.Display.drawRect(x, y, w, h, color);
}

void DisplayUtils::drawThickBorder(int x, int y, int w, int h, uint16_t color, int thickness) {
    for (int i = 0; i < thickness; i++) {
        M5Cardputer.Display.drawRect(x + i, y + i, w - 2*i, h - 2*i, color);
    }
}

void DisplayUtils::drawHeader(const String& title, uint16_t color) {
    M5Cardputer.Display.setTextSize(2);
    M5Cardputer.Display.setCursor(10, 8);
    M5Cardputer.Display.setTextColor(color, TFT_BLACK);
    M5Cardputer.Display.print(title);
}

uint16_t DisplayUtils::getRSSIColor(int rssi) {
    if (rssi > -50) return TFT_GREEN;
    if (rssi > -60) return TFT_YELLOW;
    if (rssi > -70) return TFT_ORANGE;
    return TFT_RED;
}

String DisplayUtils::getRSSIBar(int rssi) {
    if (rssi > -50) return "||||";
    if (rssi > -60) return "|||";
    if (rssi > -70) return "||";
    if (rssi > -80) return "|";
    return "";
}

void DisplayUtils::clearArea(int x, int y, int w, int h) {
    M5Cardputer.Display.fillRect(x, y, w, h, TFT_BLACK);
}
