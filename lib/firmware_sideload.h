#ifndef FIRMWARE_SIDELOAD_H
#define FIRMWARE_SIDELOAD_H

#include <Arduino.h>

// Check for firmware on SD card and perform update if found
bool check_and_update_from_sd();

// List available firmware files on SD card
void list_firmware_files();

// Backup current firmware to SD card
bool backup_current_firmware();

// Initialize firmware sideload system
void init_firmware_sideload();

#endif // FIRMWARE_SIDELOAD_H
