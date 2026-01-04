#ifndef SIMPLE_SECURITY_CHECK_H
#define SIMPLE_SECURITY_CHECK_H

#include <Arduino.h>
#include <WiFi.h>
#include "esp_system.h"

struct WifiCreds {
    String ssid;
    String password;
};

struct BtCreds {
    String deviceName;
    String pin;
};

class SecurityAuditor {
public:
    SecurityAuditor();

    void set_wifi_creds(const WifiCreds& creds);
    void set_bt_creds(const BtCreds& creds);

    void run_security_audit();

private:
    WifiCreds wifiCreds;
    BtCreds btCreds;

    bool check_secure_boot();
    bool check_flash_encryption();
    bool check_wifi_security();
    bool check_bluetooth_security();
    bool check_system_integrity();

    void log_result(const char* test_name, bool passed, const String& detail = "");
    String mask_password(const String& pw);
};

#endif
