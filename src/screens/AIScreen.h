#ifndef AISCREEN_H
#define AISCREEN_H

#include "BaseScreen.h"
#include "../managers/AIManager.h"
#include "../utils/DisplayUtils.h"
#include <M5Cardputer.h>

enum AIScreenState {
    AI_INPUT,
    AI_PROCESSING,
    AI_RESPONSE
};

class AIScreen : public BaseScreen {
public:
    AIScreen();
    void draw() override;
    void handleKey(char key) override;
    AppState getNextState() override { return nextState; }
    
private:
    String inputBuffer;
    String responseText;
    AIScreenState aiState;
    AppState nextState;
    
    void drawInputMode();
    void drawProcessingMode();
    void drawResponseMode();
    void processInput();
    void clearInput();
};

#endif // AISCREEN_H
