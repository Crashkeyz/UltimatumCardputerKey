#ifndef SD_LOGGER_H
#define SD_LOGGER_H

#include <Arduino.h>
#include "storage_manager.h"

/**
 * SDLogger - Simple logging system for SD card storage
 * 
 * Features:
 * - Timestamped log entries
 * - Multiple log levels (INFO, WARNING, ERROR)
 * - Dual output (Serial + SD card)
 * - Automatic log rotation when size exceeds limit
 * - Thread-safe operations
 * 
 * Usage:
 *   SDLogger logger;
 *   logger.begin(&storageManager);
 *   logger.info("System started");
 *   logger.error("Failed to initialize component");
 */
class SDLogger {
public:
    enum LogLevel {
        LOG_INFO,
        LOG_WARNING,
        LOG_ERROR
    };
    
    /**
     * Constructor
     */
    SDLogger();
    
    /**
     * Initialize logger with storage manager
     * @param storage Pointer to initialized StorageManager
     * @return true if initialization successful
     */
    bool begin(StorageManager* storage);
    
    /**
     * Set maximum log file size in bytes
     * When exceeded, old log is renamed and new log started
     * @param maxSize Maximum size in bytes (default: 1MB)
     */
    void setMaxLogSize(size_t maxSize);
    
    /**
     * Enable/disable console output
     * @param enabled If true, logs are also printed to Serial
     */
    void setConsoleOutput(bool enabled);
    
    /**
     * Log info message
     * @param message Message to log
     */
    void info(const char* message);
    void info(const String& message);
    
    /**
     * Log warning message
     * @param message Message to log
     */
    void warning(const char* message);
    void warning(const String& message);
    
    /**
     * Log error message
     * @param message Message to log
     */
    void error(const char* message);
    void error(const String& message);
    
    /**
     * Log custom message with level
     * @param level Log level
     * @param message Message to log
     */
    void log(LogLevel level, const char* message);
    void log(LogLevel level, const String& message);
    
    /**
     * Rotate log file (rename current, start new)
     * @return true if rotation successful
     */
    bool rotateLog();
    
    /**
     * Get current log file size
     * @return Size in bytes
     */
    size_t getLogSize();
    
private:
    StorageManager* _storage;
    String _logPath;
    size_t _maxLogSize;
    bool _consoleOutput;
    bool _initialized;
    
    /**
     * Get formatted timestamp string
     */
    String getTimestamp();
    
    /**
     * Get log level string
     */
    String getLevelString(LogLevel level);
    
    /**
     * Check if log rotation needed
     */
    void checkRotation();
    
    /**
     * Write log entry to file and optionally console
     */
    void writeLog(LogLevel level, const String& message);
};

#endif // SD_LOGGER_H
