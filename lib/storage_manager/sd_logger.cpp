#include "sd_logger.h"

SDLogger::SDLogger() 
    : _storage(nullptr), 
      _logPath("/logs/system.log"), 
      _maxLogSize(1024 * 1024),  // 1MB default
      _consoleOutput(true),
      _initialized(false) {
}

bool SDLogger::begin(StorageManager* storage) {
    if (storage == nullptr || !storage->isReady()) {
        Serial.println("[Logger] ERROR: Storage manager not ready");
        return false;
    }
    
    _storage = storage;
    _initialized = true;
    
    Serial.println("[Logger] Logger initialized");
    log(LOG_INFO, "Logger system started");
    
    return true;
}

void SDLogger::setMaxLogSize(size_t maxSize) {
    _maxLogSize = maxSize;
}

void SDLogger::setConsoleOutput(bool enabled) {
    _consoleOutput = enabled;
}

void SDLogger::info(const char* message) {
    log(LOG_INFO, message);
}

void SDLogger::info(const String& message) {
    log(LOG_INFO, message);
}

void SDLogger::warning(const char* message) {
    log(LOG_WARNING, message);
}

void SDLogger::warning(const String& message) {
    log(LOG_WARNING, message);
}

void SDLogger::error(const char* message) {
    log(LOG_ERROR, message);
}

void SDLogger::error(const String& message) {
    log(LOG_ERROR, message);
}

void SDLogger::log(LogLevel level, const char* message) {
    log(level, String(message));
}

void SDLogger::log(LogLevel level, const String& message) {
    if (!_initialized || _storage == nullptr) {
        // Fallback to serial only if not initialized
        Serial.printf("[Logger] %s: %s\n", getLevelString(level).c_str(), message.c_str());
        return;
    }
    
    writeLog(level, message);
}

bool SDLogger::rotateLog() {
    if (!_initialized || _storage == nullptr) {
        return false;
    }
    
    // Create backup filename with timestamp
    String backupPath = "/logs/system_" + String(millis()) + ".log";
    
    Serial.printf("[Logger] Rotating log to: %s\n", backupPath.c_str());
    
    // Read current log
    String logData;
    if (_storage->readFile(_logPath.c_str(), logData)) {
        // Write to backup
        if (_storage->writeFile(backupPath.c_str(), logData.c_str())) {
            // Delete original
            _storage->deleteFile(_logPath.c_str());
            Serial.println("[Logger] Log rotation complete");
            return true;
        }
    }
    
    Serial.println("[Logger] ERROR: Log rotation failed");
    return false;
}

size_t SDLogger::getLogSize() {
    if (!_initialized || _storage == nullptr) {
        return 0;
    }
    return _storage->getFileSize(_logPath.c_str());
}

String SDLogger::getTimestamp() {
    // Simple timestamp using millis()
    // Note: millis() wraps around after ~49.7 days
    // For production use, consider using RTC for accurate timestamps
    unsigned long ms = millis();
    unsigned long seconds = ms / 1000;
    unsigned long minutes = seconds / 60;
    unsigned long hours = minutes / 60;
    unsigned long days = hours / 24;
    
    seconds = seconds % 60;
    minutes = minutes % 60;
    hours = hours % 24;
    
    char timestamp[32];
    if (days > 0) {
        snprintf(timestamp, sizeof(timestamp), "D%lu:%02lu:%02lu:%02lu.%03lu", 
                 days, hours, minutes, seconds, ms % 1000);
    } else {
        snprintf(timestamp, sizeof(timestamp), "%02lu:%02lu:%02lu.%03lu", 
                 hours, minutes, seconds, ms % 1000);
    }
    
    return String(timestamp);
}

String SDLogger::getLevelString(LogLevel level) {
    switch (level) {
        case LOG_INFO:    return "INFO";
        case LOG_WARNING: return "WARN";
        case LOG_ERROR:   return "ERROR";
        default:          return "UNKNOWN";
    }
}

void SDLogger::checkRotation() {
    if (getLogSize() > _maxLogSize) {
        rotateLog();
    }
}

void SDLogger::writeLog(LogLevel level, const String& message) {
    // Format: [timestamp] LEVEL: message
    String logEntry = "[" + getTimestamp() + "] " + 
                      getLevelString(level) + ": " + 
                      message + "\n";
    
    // Output to console if enabled
    if (_consoleOutput) {
        Serial.print(logEntry);
    }
    
    // Check if rotation needed
    checkRotation();
    
    // Append to log file
    if (!_storage->appendFile(_logPath.c_str(), logEntry.c_str())) {
        Serial.println("[Logger] ERROR: Failed to write to log file");
    }
}
