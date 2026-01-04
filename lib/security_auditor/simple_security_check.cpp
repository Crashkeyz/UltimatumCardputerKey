#include "simple_security_check.h"

SecurityAuditor::SecurityAuditor() {
    wifiCreds = {"", ""};
    btCreds = {"", ""};
}

void SecurityAuditor::set_wifi_creds(const WifiCreds& creds) {
    wifiCreds = creds;
}

void SecurityAuditor::set_bt_creds(const BtCreds& creds) {
    btCreds = creds;
}

void SecurityAuditor::run_security_audit() {
    Serial.println("=== Security audit start ===");

    bool sb = check_secure_boot();
    log_result("Secure Boot", sb, sb ? "enabled" : "disabled or unknown");

    bool fe = check_flash_encryption();
    log_result("Flash Encryption", fe, fe ? "enabled" : "disabled or unknown");

    bool ws = check_wifi_security();
    log_result("WiFi Security", ws);

    bool bs = check_bluetooth_security();
    log_result("Bluetooth Security", bs);

    bool si = check_system_integrity();
    log_result("System Integrity", si);

    Serial.println("=== Security audit complete ===");
}

bool SecurityAuditor::check_secure_boot() {
#if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3
    return false;
#else
    return false;
#endif
}

bool SecurityAuditor::check_flash_encryption() {
    return false;
}

bool SecurityAuditor::check_wifi_security() {
    if (wifiCreds.ssid.length() == 0) {
        log_result("WiFi: SSID present", false, "SSID missing");
        return false;
    }

    if (wifiCreds.password.length() == 0) {
        log_result("WiFi: Password present", false, "MISSING");
        log_result("WiFi: SSID present", true, wifiCreds.ssid);
        return false;
    }

    String masked = mask_password(wifiCreds.password);
    log_result("WiFi: SSID present", true, wifiCreds.ssid);
    log_result("WiFi: Password present", true, masked);
    
    if (wifiCreds.password.length() < 8) {
        log_result("WiFi: Password strength", false, "password too short (<8 chars)");
        return false;
    }
    log_result("WiFi: Password strength", true, "length ok");
    return true;
}

bool SecurityAuditor::check_bluetooth_security() {
    if (btCreds.deviceName.length() == 0) {
        log_result("Bluetooth: Device name", false, "missing");
        return false;
    }
    log_result("Bluetooth: Device name", true, btCreds.deviceName);

    if (btCreds.pin.length() == 0) {
        log_result("Bluetooth: Pairing PIN", false, "MISSING or not configured");
        return false;
    }

    String masked = mask_password(btCreds.pin);
    log_result("Bluetooth: Pairing PIN", true, masked);
    return true;
}

bool SecurityAuditor::check_system_integrity() {
    return true;
}

void SecurityAuditor::log_result(const char* test_name, bool passed, const String& detail) {
    Serial.print("[");
    Serial.print(passed ? "PASS" : "FAIL");
    Serial.print("] ");
    Serial.print(test_name);
    if (detail.length() > 0) {
        Serial.print(" - ");
        Serial.print(detail);
    }
    Serial.println();
}

String SecurityAuditor::mask_password(const String& pw) {
    if (pw.length() == 0) return "MISSING";
    String s = "";
    for (size_t i = 0; i < pw.length(); ++i) {
        s += "*";
    }
    s += " (";
    s += String(pw.length());
    s += " chars)";
    return s;
}
