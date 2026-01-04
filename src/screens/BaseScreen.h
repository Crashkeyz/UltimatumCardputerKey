#ifndef BASESCREEN_H
#define BASESCREEN_H

#include <Arduino.h>

enum AppState { 
    MAIN_MENU, 
    WIFI_SCANNER, 
    SECURITY_AUDIT, 
    ABOUT, 
    WIFI_CONNECT, 
    AI_ASSISTANT, 
    CODE_WRITER 
};

class BaseScreen {
public:
    virtual ~BaseScreen() {}
    virtual void draw() = 0;
    virtual void handleKey(char key) = 0;
    virtual AppState getNextState() { return currentState; }
    
protected:
    AppState currentState;
};

#endif // BASESCREEN_H
