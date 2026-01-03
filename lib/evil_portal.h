#ifndef EVIL_PORTAL_H
#define EVIL_PORTAL_H

#include <Arduino.h>

// Initialize Evil Portal (Captive Portal for firmware access)
bool init_evil_portal();

// Handle Evil Portal requests (call in loop)
void handle_evil_portal();

// Stop Evil Portal
void stop_evil_portal();

// Check if portal is active
bool is_portal_active();

#endif // EVIL_PORTAL_H
