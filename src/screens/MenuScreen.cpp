#include "MenuScreen.h"

MenuScreen::MenuScreen() : selectedIndex(0), nextState(MAIN_MENU) {
    currentState = MAIN_MENU;
}

void MenuScreen::draw() {
    M5Cardputer.Display.fillScreen(TFT_BLACK);
    
    // Thunder Castle title
    M5Cardputer.Display.setTextSize(2);
    M5Cardputer.Display.setCursor(10, 8);
    M5Cardputer.Display.setTextColor(TFT_RED, TFT_BLACK);
    M5Cardputer.Display.println("CARDPUTER ADV");
    
    // Version
    M5Cardputer.Display.setTextSize(2);
    M5Cardputer.Display.setCursor(10, 25);
    M5Cardputer.Display.setTextColor(TFT_CYAN, TFT_BLACK);
    M5Cardputer.Display.println("v3.2.0-refactor");
    
    // Menu items
    M5Cardputer.Display.setTextSize(2);
    int yPos = 50;
    
    for (int i = 0; i < menuItemCount; i++) {
        M5Cardputer.Display.setCursor(15, yPos);
        
        if (i == selectedIndex) {
            M5Cardputer.Display.setTextColor(TFT_BLACK, TFT_CYAN);
            M5Cardputer.Display.print("> ");
        } else {
            M5Cardputer.Display.setTextColor(TFT_WHITE, TFT_BLACK);
            M5Cardputer.Display.print("  ");
        }
        
        M5Cardputer.Display.print(menuItems[i]);
        yPos += 14;
    }
    
    // Navigation hint
    M5Cardputer.Display.setTextSize(2);
    M5Cardputer.Display.setCursor(10, 118);
    M5Cardputer.Display.setTextColor(TFT_YELLOW, TFT_BLACK);
    M5Cardputer.Display.print(",. ENT:Select");
}

void MenuScreen::handleKey(char key) {
    if (key == ',') {  // Up
        selectedIndex--;
        if (selectedIndex < 0) {
            selectedIndex = menuItemCount - 1;
        }
        draw();
    } else if (key == '.') {  // Down
        selectedIndex++;
        if (selectedIndex >= menuItemCount) {
            selectedIndex = 0;
        }
        draw();
    } else if (key == '\n') {  // Enter
        switch (selectedIndex) {
            case 0:
                nextState = WIFI_SCANNER;
                break;
            case 1:
                nextState = SECURITY_AUDIT;
                break;
            case 2:
                nextState = AI_ASSISTANT;
                break;
            case 3:
                nextState = CODE_WRITER;
                break;
            case 4:
                nextState = ABOUT;
                break;
        }
    }
}
