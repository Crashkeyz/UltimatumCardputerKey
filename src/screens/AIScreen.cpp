#include "AIScreen.h"

AIScreen::AIScreen() : aiState(AI_INPUT), nextState(AI_ASSISTANT) {
    currentState = AI_ASSISTANT;
    inputBuffer = "";
    responseText = "";
}

void AIScreen::draw() {
    switch (aiState) {
        case AI_INPUT:
            drawInputMode();
            break;
        case AI_PROCESSING:
            drawProcessingMode();
            break;
        case AI_RESPONSE:
            drawResponseMode();
            break;
    }
}

void AIScreen::drawInputMode() {
    M5Cardputer.Display.fillScreen(TFT_BLACK);
    
    // Border
    DisplayUtils::drawBorder(3, 3, 234, 129, TFT_CYAN);
    
    // Header
    DisplayUtils::drawHeader("AI CHAT", TFT_RED);
    
    // Input prompt
    M5Cardputer.Display.setTextSize(2);
    M5Cardputer.Display.setCursor(10, 50);
    M5Cardputer.Display.setTextColor(TFT_WHITE, TFT_BLACK);
    M5Cardputer.Display.print(">");
    
    // Display input with word wrap - improved wrapping
    M5Cardputer.Display.setTextColor(TFT_CYAN, TFT_BLACK);
    int yPos = 50;
    int xPos = 22;
    int maxCharsPerLine = 18;
    int lineHeight = 16;
    int maxLines = 3;
    
    int charIdx = 0;
    int lineCount = 0;
    
    while (charIdx < inputBuffer.length() && lineCount < maxLines) {
        int endIdx = min(charIdx + maxCharsPerLine, (int)inputBuffer.length());
        String line = inputBuffer.substring(charIdx, endIdx);
        
        M5Cardputer.Display.setCursor(xPos, yPos);
        M5Cardputer.Display.print(line);
        
        yPos += lineHeight;
        charIdx = endIdx;
        lineCount++;
    }
    
    // Cursor
    M5Cardputer.Display.print("_");
    
    // Instructions
    M5Cardputer.Display.setTextSize(2);
    M5Cardputer.Display.setCursor(10, 118);
    M5Cardputer.Display.setTextColor(TFT_YELLOW, TFT_BLACK);
    M5Cardputer.Display.print("ENT:Ask ESC:Exit");
}

void AIScreen::drawProcessingMode() {
    M5Cardputer.Display.fillScreen(TFT_BLACK);
    
    DisplayUtils::drawBorder(3, 3, 234, 129, TFT_CYAN);
    DisplayUtils::drawHeader("AI CHAT", TFT_RED);
    
    M5Cardputer.Display.setTextSize(2);
    M5Cardputer.Display.setCursor(10, 60);
    M5Cardputer.Display.setTextColor(TFT_YELLOW, TFT_BLACK);
    M5Cardputer.Display.print("Thinking...");
}

void AIScreen::drawResponseMode() {
    M5Cardputer.Display.fillScreen(TFT_BLACK);
    
    DisplayUtils::drawBorder(3, 3, 234, 129, TFT_CYAN);
    DisplayUtils::drawHeader("AI CHAT", TFT_RED);
    
    // Show last question (truncated)
    M5Cardputer.Display.setTextSize(2);
    M5Cardputer.Display.setCursor(10, 30);
    M5Cardputer.Display.setTextColor(TFT_YELLOW, TFT_BLACK);
    M5Cardputer.Display.print("Q:");
    
    String q = inputBuffer;
    if (q.length() > 16) {
        q = q.substring(0, 15) + ">";
    }
    M5Cardputer.Display.print(q);
    
    // Show response with proper wrapping
    M5Cardputer.Display.setCursor(10, 50);
    M5Cardputer.Display.setTextColor(TFT_GREEN, TFT_BLACK);
    M5Cardputer.Display.print("A:");
    
    // Wrap response text properly
    int yPos = 50;
    int maxCharsPerLine = 19;
    int lineHeight = 16;
    int maxLines = 4;
    
    int charIdx = 0;
    int lineCount = 0;
    
    while (charIdx < responseText.length() && lineCount < maxLines) {
        int endIdx = min(charIdx + maxCharsPerLine, (int)responseText.length());
        
        // Try to break at word boundary
        if (endIdx < responseText.length() && responseText[endIdx] != ' ') {
            int lastSpace = responseText.lastIndexOf(' ', endIdx);
            if (lastSpace > charIdx && lastSpace > charIdx + maxCharsPerLine - 10) {
                endIdx = lastSpace;
            }
        }
        
        String line = responseText.substring(charIdx, endIdx);
        line.trim();
        
        M5Cardputer.Display.setCursor(10, yPos);
        M5Cardputer.Display.setTextColor(TFT_GREEN, TFT_BLACK);
        M5Cardputer.Display.print(line);
        
        yPos += lineHeight;
        charIdx = endIdx;
        lineCount++;
        
        // Skip leading spaces
        while (charIdx < responseText.length() && responseText[charIdx] == ' ') {
            charIdx++;
        }
    }
    
    // Instructions
    M5Cardputer.Display.setTextSize(2);
    M5Cardputer.Display.setCursor(10, 118);
    M5Cardputer.Display.setTextColor(TFT_YELLOW, TFT_BLACK);
    M5Cardputer.Display.print("ENT:Next ESC:Exit");
}

void AIScreen::handleKey(char key) {
    if (key == 0) return;
    
    if (key == 27) {  // ESC
        nextState = MAIN_MENU;
        return;
    }
    
    if (aiState == AI_INPUT) {
        if (key == '\n') {  // Enter
            if (inputBuffer.length() > 0) {
                processInput();
            }
        } else if (key == '\b') {  // Backspace
            if (inputBuffer.length() > 0) {
                inputBuffer.remove(inputBuffer.length() - 1);
                draw();
            }
        } else if (key >= 32 && key <= 126) {  // Printable characters
            // Buffer keyboard input
            if (inputBuffer.length() < 200) {
                inputBuffer += key;
                draw();
            }
        }
    } else if (aiState == AI_RESPONSE) {
        if (key == '\n') {  // Enter - start new question
            clearInput();
            aiState = AI_INPUT;
            draw();
        }
    }
}

void AIScreen::processInput() {
    // Check for learning command format: learn:trigger->action
    if (inputBuffer.startsWith("learn:")) {
        String cmd = inputBuffer.substring(6);
        int arrowPos = cmd.indexOf("->");
        
        if (arrowPos > 0) {
            String trigger = cmd.substring(0, arrowPos);
            String action = cmd.substring(arrowPos + 2);
            trigger.trim();
            action.trim();
            
            AIManager& ai = AIManager::getInstance();
            if (ai.learnPattern(trigger, action)) {
                responseText = "Learned! Try: " + trigger;
            } else {
                responseText = "Pattern full (max 10)";
            }
        } else {
            responseText = "Bad format. Use: learn:word->action";
        }
    } else {
        // Process as normal query
        aiState = AI_PROCESSING;
        draw();
        
        AIManager& ai = AIManager::getInstance();
        responseText = ai.processQuery(inputBuffer);
    }
    
    aiState = AI_RESPONSE;
    draw();
}

void AIScreen::clearInput() {
    inputBuffer = "";
    responseText = "";
}
