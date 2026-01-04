#ifndef MENUSCREEN_H
#define MENUSCREEN_H

#include "BaseScreen.h"
#include "../utils/DisplayUtils.h"
#include <M5Cardputer.h>

class MenuScreen : public BaseScreen {
public:
    MenuScreen();
    void draw() override;
    void handleKey(char key) override;
    AppState getNextState() override { return nextState; }
    
private:
    static const int menuItemCount = 5;
    const char* menuItems[menuItemCount] = {
        "WiFi Scanner", 
        "Security Audit", 
        "AI Assistant", 
        "Code Writer", 
        "About"
    };
    
    int selectedIndex;
    AppState nextState;
};

#endif // MENUSCREEN_H
