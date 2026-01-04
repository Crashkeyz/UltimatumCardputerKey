#include "AIManager.h"

AIManager& AIManager::getInstance() {
    static AIManager instance;
    return instance;
}

AIManager::AIManager() : patternCount(0) {
}

void AIManager::initialize() {
    SDManager& sd = SDManager::getInstance();
    if (sd.isAvailable()) {
        apiKey = sd.loadAPIKey("/ai_key.txt");
        loadPatterns();
    }
    Serial.println("AI: Initialized");
}

String AIManager::processQuery(const String& query) {
    // First check for learned patterns
    String patternResponse = matchPattern(query);
    if (patternResponse.length() > 0) {
        return patternResponse;
    }
    
    // Try offline AI first
    String offlineResponse = generateOfflineResponse(query);
    if (offlineResponse.length() > 0) {
        return offlineResponse;
    }
    
    // If WiFi connected and API key available, use online API
    WiFiManager& wifi = WiFiManager::getInstance();
    if (wifi.isConnected() && hasAPIKey()) {
        String apiResponse = callDeepSeekAPI(query);
        if (apiResponse.length() > 0) {
            return apiResponse;
        }
    }
    
    // Fallback response
    return "Interesting question! Try 'help' for commands I understand.";
}

String AIManager::matchPattern(const String& input) {
    for (int i = 0; i < patternCount; i++) {
        if (input.equalsIgnoreCase(patterns[i].trigger)) {
            return "Action: " + patterns[i].action;
        }
    }
    return "";
}

String AIManager::generateOfflineResponse(const String& query) {
    String q = query;
    q.toLowerCase();
    q.trim();
    
    // Greetings
    if (q.indexOf("hello") >= 0 || q.indexOf("hi") >= 0 || q.indexOf("hey") >= 0) {
        return "Hey! I'm your Cardputer AI. Ask me anything or say 'help'!";
    }
    
    if (q.indexOf("how are you") >= 0 || q.indexOf("what's up") >= 0) {
        return "Running great! CPU at " + String(ESP.getCpuFreqMHz()) + "MHz, " + 
               String(ESP.getFreeHeap()/1024) + "KB RAM free.";
    }
    
    if (q.indexOf("your name") >= 0 || q.indexOf("who are you") >= 0) {
        return "I'm Thunder AI, your Cardputer assistant!";
    }
    
    // WiFi queries
    WiFiManager& wifi = WiFiManager::getInstance();
    if (q.indexOf("wifi") >= 0 || q.indexOf("network") >= 0) {
        if (wifi.isConnected()) {
            return "WiFi OK! SSID: " + wifi.getConnectedSSID() + ", Signal: " + 
                   String(wifi.getSignalStrength()) + "dBm";
        } else {
            return "WiFi OFF. Use WiFi Scanner from menu to connect.";
        }
    }
    
    // System info
    if (q.indexOf("status") >= 0 || q.indexOf("info") >= 0) {
        return getSystemStatus();
    }
    
    if (q.indexOf("memory") >= 0 || q.indexOf("ram") >= 0) {
        return "Free RAM: " + String(ESP.getFreeHeap()/1024) + "KB / " + 
               String(ESP.getHeapSize()/1024) + "KB";
    }
    
    if (q.indexOf("cpu") >= 0 || q.indexOf("hardware") >= 0) {
        return getHardwareInfo();
    }
    
    // SD card
    SDManager& sd = SDManager::getInstance();
    if (q.indexOf("sd") >= 0 || q.indexOf("card") >= 0) {
        if (sd.isAvailable()) {
            return "SD card OK! Stores patterns, keys, and logs.";
        } else {
            return "SD card not detected. Insert FAT32 card and reboot.";
        }
    }
    
    // Patterns
    if (q.indexOf("pattern") >= 0) {
        if (patternCount > 0) {
            String resp = String(patternCount) + " patterns: ";
            for (int i = 0; i < patternCount && i < 3; i++) {
                resp += patterns[i].trigger + " ";
            }
            return resp;
        } else {
            return "No patterns. Teach me with: learn:word->action";
        }
    }
    
    // Learning
    if (q.indexOf("learn") >= 0 || q.indexOf("teach") >= 0) {
        return "I can learn! Format: learn:trigger->action. Example: learn:w->wifi";
    }
    
    // Help
    if (q.indexOf("help") >= 0 || q.indexOf("command") >= 0) {
        return "I can check WiFi, system status, learn patterns, and chat! Try 'status' or 'wifi'.";
    }
    
    // Version
    if (q.indexOf("version") >= 0 || q.indexOf("about") >= 0) {
        return "Cardputer ADV v3.2.0-refactor. Thunder Castle Edition!";
    }
    
    // Thanks
    if (q.indexOf("thanks") >= 0 || q.indexOf("thank") >= 0) {
        return "You're welcome! Happy to help! 🤖";
    }
    
    return "";  // No match
}

String AIManager::getSystemStatus() {
    WiFiManager& wifi = WiFiManager::getInstance();
    SDManager& sd = SDManager::getInstance();
    
    String status = "WiFi:" + String(wifi.isConnected() ? "ON" : "OFF") + " ";
    status += "API:" + String(hasAPIKey() ? "OK" : "NO") + " ";
    status += "Patterns:" + String(patternCount) + " ";
    status += "SD:" + String(sd.isAvailable() ? "OK" : "NO") + " ";
    status += "Uptime:" + String(millis()/1000) + "s";
    
    return status;
}

String AIManager::getHardwareInfo() {
    return "ESP32-S3 @ " + String(ESP.getCpuFreqMHz()) + "MHz. Flash: " + 
           String(ESP.getFlashChipSize()/(1024*1024)) + "MB. Rev: v" + 
           String(ESP.getChipRevision());
}

String AIManager::callDeepSeekAPI(const String& query) {
    if (!hasAPIKey()) {
        return "";
    }
    
    HTTPClient http;
    http.begin("https://api.deepseek.com/v1/chat/completions");
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", "Bearer " + apiKey);
    
    // Build JSON payload
    JsonDocument doc;
    doc["model"] = "deepseek-chat";
    
    JsonArray messages = doc["messages"].to<JsonArray>();
    
    // System message
    JsonObject systemMsg = messages.add<JsonObject>();
    systemMsg["role"] = "system";
    String systemContent = "You are Thunder AI on ESP32 Cardputer. ";
    systemContent += "Device: ESP32-S3, " + String(ESP.getFreeHeap()/1024) + "KB RAM free. ";
    systemContent += "Keep responses under 100 chars, be helpful and concise.";
    systemMsg["content"] = systemContent;
    
    // User message
    JsonObject userMsg = messages.add<JsonObject>();
    userMsg["role"] = "user";
    userMsg["content"] = query;
    
    doc["max_tokens"] = 150;
    doc["temperature"] = 0.7;
    
    String payload;
    serializeJson(doc, payload);
    
    Serial.println("AI API Request:");
    Serial.println(payload);
    
    int httpCode = http.POST(payload);
    
    if (httpCode == 200) {
        String response = http.getString();
        Serial.println("AI API Response:");
        Serial.println(response);
        
        JsonDocument responseDoc;
        DeserializationError error = deserializeJson(responseDoc, response);
        
        if (!error) {
            const char* content = responseDoc["choices"][0]["message"]["content"];
            http.end();
            return String(content);
        } else {
            Serial.println("JSON parse error");
        }
    } else {
        Serial.printf("HTTP Error: %d\n", httpCode);
    }
    
    http.end();
    return "";
}

bool AIManager::loadPatterns() {
    SDManager& sd = SDManager::getInstance();
    if (!sd.isAvailable()) {
        return false;
    }
    
    bool success;
    String content = sd.readFile("/patterns.txt", success);
    if (!success) {
        return false;
    }
    
    patternCount = 0;
    int startPos = 0;
    
    while (patternCount < 10) {
        int endPos = content.indexOf('\n', startPos);
        if (endPos == -1) {
            endPos = content.length();
        }
        
        String line = content.substring(startPos, endPos);
        line.trim();
        
        if (line.length() > 0) {
            int arrowPos = line.indexOf("->");
            if (arrowPos > 0) {
                patterns[patternCount].trigger = line.substring(0, arrowPos);
                patterns[patternCount].action = line.substring(arrowPos + 2);
                patterns[patternCount].trigger.trim();
                patterns[patternCount].action.trim();
                patternCount++;
            }
        }
        
        if (endPos >= content.length()) {
            break;
        }
        startPos = endPos + 1;
    }
    
    Serial.printf("AI: Loaded %d patterns\n", patternCount);
    return true;
}

bool AIManager::savePatterns() {
    SDManager& sd = SDManager::getInstance();
    if (!sd.isAvailable()) {
        return false;
    }
    
    String content = "";
    for (int i = 0; i < patternCount; i++) {
        content += patterns[i].trigger + "->" + patterns[i].action + "\n";
    }
    
    bool success = sd.writeFile("/patterns.txt", content);
    if (success) {
        Serial.printf("AI: Saved %d patterns\n", patternCount);
    }
    
    return success;
}

bool AIManager::learnPattern(const String& trigger, const String& action) {
    if (patternCount >= 10) {
        return false;
    }
    
    // Check if pattern already exists
    for (int i = 0; i < patternCount; i++) {
        if (patterns[i].trigger.equalsIgnoreCase(trigger)) {
            patterns[i].action = action;
            return savePatterns();
        }
    }
    
    // Add new pattern
    patterns[patternCount].trigger = trigger;
    patterns[patternCount].action = action;
    patternCount++;
    
    return savePatterns();
}

const LearnedPattern& AIManager::getPattern(int index) const {
    static LearnedPattern empty;
    if (index >= 0 && index < patternCount) {
        return patterns[index];
    }
    return empty;
}

void AIManager::clearHistory() {
    // For future conversation history implementation
}
