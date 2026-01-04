#ifndef AIMANAGER_H
#define AIMANAGER_H

#include <Arduino.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "SDManager.h"
#include "WiFiManager.h"

struct LearnedPattern {
    String trigger;
    String action;
};

class AIManager {
public:
    static AIManager& getInstance();
    
    void initialize();
    
    // API Configuration
    void setAPIKey(const String& key) { apiKey = key; }
    String getAPIKey() const { return apiKey; }
    bool hasAPIKey() const { return apiKey.length() > 0; }
    
    // AI Query Processing
    String processQuery(const String& query);
    String callDeepSeekAPI(const String& query);
    
    // Pattern Learning
    bool loadPatterns();
    bool savePatterns();
    bool learnPattern(const String& trigger, const String& action);
    int getPatternCount() const { return patternCount; }
    const LearnedPattern& getPattern(int index) const;
    String matchPattern(const String& input);
    
    // Conversation History (optional for future)
    void clearHistory();
    
private:
    AIManager();
    AIManager(const AIManager&) = delete;
    AIManager& operator=(const AIManager&) = delete;
    
    String apiKey;
    LearnedPattern patterns[10];
    int patternCount;
    
    // Helper methods for offline AI responses
    String generateOfflineResponse(const String& query);
    String getSystemStatus();
    String getHardwareInfo();
};

#endif // AIMANAGER_H
