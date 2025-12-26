#ifndef STORAGE_MANAGER_H
#define STORAGE_MANAGER_H

#include <Arduino.h>
#include <SD.h>
#include <FS.h>

/**
 * StorageManager - Comprehensive SD card storage management for M5Stack Cardputer
 * 
 * This class provides a complete interface for SD card operations including:
 * - SD card initialization and status monitoring
 * - File read/write/delete operations
 * - Directory creation and management
 * - Free space checking
 * - Robust error handling
 * 
 * Usage:
 *   StorageManager storage;
 *   if (storage.begin()) {
 *     storage.writeFile("/logs/test.txt", "Hello World");
 *   }
 */
class StorageManager {
public:
    /**
     * Constructor - Initialize storage manager
     */
    StorageManager();
    
    /**
     * Initialize SD card
     * @return true if SD card initialized successfully, false otherwise
     */
    bool begin();
    
    /**
     * Check if SD card is ready for operations
     * @return true if SD card is available and mounted
     */
    bool isReady();
    
    /**
     * Get SD card type as string
     * @return String describing card type (e.g., "SDHC", "SDSC", "SDXC", "MMC", "Unknown")
     */
    String getCardType();
    
    /**
     * Get total space on SD card in bytes
     * @return Total space in bytes
     */
    uint64_t getTotalSpace();
    
    /**
     * Get free space on SD card in bytes
     * @return Free space in bytes
     */
    uint64_t getFreeSpace();
    
    /**
     * Get used space on SD card in bytes
     * @return Used space in bytes
     */
    uint64_t getUsedSpace();
    
    /**
     * Write data to file on SD card
     * @param path Path to file (e.g., "/data/file.txt")
     * @param data Data to write
     * @return true if write successful, false otherwise
     */
    bool writeFile(const char* path, const char* data);
    
    /**
     * Append data to file on SD card
     * @param path Path to file
     * @param data Data to append
     * @return true if append successful, false otherwise
     */
    bool appendFile(const char* path, const char* data);
    
    /**
     * Read entire file from SD card
     * @param path Path to file
     * @param data String to store file contents
     * @return true if read successful, false otherwise
     */
    bool readFile(const char* path, String& data);
    
    /**
     * Delete file from SD card
     * @param path Path to file
     * @return true if delete successful, false otherwise
     */
    bool deleteFile(const char* path);
    
    /**
     * Check if file exists
     * @param path Path to file
     * @return true if file exists, false otherwise
     */
    bool fileExists(const char* path);
    
    /**
     * Get file size in bytes
     * @param path Path to file
     * @return File size in bytes, or 0 if file doesn't exist
     */
    size_t getFileSize(const char* path);
    
    /**
     * Create directory on SD card
     * @param path Directory path (e.g., "/logs")
     * @return true if directory created or already exists, false otherwise
     */
    bool createDir(const char* path);
    
    /**
     * Remove directory from SD card
     * @param path Directory path
     * @return true if directory removed, false otherwise
     */
    bool removeDir(const char* path);
    
    /**
     * List files in directory
     * @param path Directory path
     * @param recursive If true, list subdirectories recursively
     * @return true if listing successful, false otherwise
     */
    bool listDir(const char* path, bool recursive = false);
    
    /**
     * Create default directory structure on SD card
     * Creates: /config, /data, /data/captures, /data/recordings, /logs, /downloads
     * @return true if all directories created successfully
     */
    bool createDefaultDirectories();
    
    /**
     * Get last error message
     * @return String containing last error message
     */
    String getLastError();
    
    /**
     * Unmount SD card
     */
    void end();

private:
    bool _initialized;
    bool _cardPresent;
    String _lastError;
    
    /**
     * Helper function to recursively list directory contents
     */
    void listDirRecursive(File dir, int numTabs);
    
    /**
     * Helper function to set error message
     */
    void setError(const String& error);
};

#endif // STORAGE_MANAGER_H
