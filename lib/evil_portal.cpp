#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <SD.h>
#include <FS.h>

// Evil Portal Configuration
#define DNS_PORT 53
#define WEB_PORT 80
#define AP_SSID "Cardputer-Portal"

// Global objects
WebServer server(WEB_PORT);
DNSServer dnsServer;
bool portalActive = false;

// HTML content for the captive portal (stored in PROGMEM to save RAM)
const char PORTAL_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Cardputer Portal</title>
    <style>
        body{font-family:Arial,sans-serif;max-width:800px;margin:50px auto;padding:20px;background:#f0f0f0}
        .container{background:white;padding:30px;border-radius:10px;box-shadow:0 2px 10px rgba(0,0,0,0.1)}
        h1{color:#333}h2{font-size:1.2em}
        .btn{display:inline-block;padding:10px 20px;margin:10px 5px;background:#4CAF50;color:white;text-decoration:none;border-radius:5px;border:none;cursor:pointer}
        .btn:hover{background:#45a049}
        .info{background:#e3f2fd;padding:15px;border-radius:5px;margin:10px 0}
        input[type="file"]{margin:10px 0}
    </style>
</head>
<body>
    <div class="container">
        <h1>🎮 Cardputer Portal</h1>
        <div class="info"><p><strong>Welcome!</strong> Manage your Cardputer device.</p></div>
        <h2>📁 Files</h2>
        <div><a href="/files" class="btn">Browse</a> <a href="/upload" class="btn">Upload</a></div>
        <h2>🔧 Firmware</h2>
        <form method="POST" action="/update" enctype="multipart/form-data">
            <input type="file" name="firmware" accept=".bin">
            <button type="submit" class="btn">Upload Firmware</button>
        </form>
        <h2>ℹ️ Info</h2>
        <div class="info"><p>Free Heap: <span id="heap"></span> bytes</p><p>SD: <span id="sd"></span></p></div>
    </div>
    <script>fetch('/api/info').then(r=>r.json()).then(d=>{document.getElementById('heap').textContent=d.heap;document.getElementById('sd').textContent=d.sd?'Available':'Not found'})</script>
</body>
</html>
)rawliteral";

// File list HTML (compact version)
void handleFileList() {
    String html = F("<html><body><h1>SD Files</h1><ul>");
    
    if (SD.exists("/")) {
        File root = SD.open("/");
        File file = root.openNextFile();
        
        while (file) {
            html += F("<li>");
            html += file.name();
            html += F(" (");
            html += String(file.size());
            html += F(" bytes)</li>");
            file = root.openNextFile();
        }
    } else {
        html += F("<li>SD not available</li>");
    }
    
    html += F("</ul><a href='/'>Back</a></body></html>");
    server.send(200, F("text/html"), html);
}

// API info endpoint
void handleApiInfo() {
    String json = F("{\"heap\":");
    json += ESP.getFreeHeap();
    json += F(",\"sd\":");
    json += (SD.cardType() != CARD_NONE) ? F("true") : F("false");
    json += F("}");
    server.send(200, F("application/json"), json);
}

// Root handler
void handleRoot() {
    server.send_P(200, "text/html", PORTAL_HTML);
}

// Catch-all handler for captive portal
void handleNotFound() {
    server.send_P(200, "text/html", PORTAL_HTML);
}

// Initialize Evil Portal
bool init_evil_portal() {
    Serial.println(F("Starting Evil Portal..."));
    
    // Set up Access Point
    WiFi.mode(WIFI_AP);
    WiFi.softAP(AP_SSID);
    
    IPAddress IP = WiFi.softAPIP();
    Serial.print(F("AP IP address: "));
    Serial.println(IP);
    
    // Start DNS server for captive portal
    dnsServer.start(DNS_PORT, "*", IP);
    
    // Set up web server routes
    server.on("/", handleRoot);
    server.on("/files", handleFileList);
    server.on("/api/info", handleApiInfo);
    server.onNotFound(handleNotFound);
    
    // Start web server
    server.begin();
    
    portalActive = true;
    Serial.println(F("Evil Portal started!"));
    Serial.print(F("Connect to WiFi: "));
    Serial.println(AP_SSID);
    Serial.println(F("Then open any website"));
    
    return true;
}

// Handle Evil Portal requests
void handle_evil_portal() {
    if (portalActive) {
        dnsServer.processNextRequest();
        server.handleClient();
    }
}

// Stop Evil Portal
void stop_evil_portal() {
    if (portalActive) {
        server.stop();
        dnsServer.stop();
        WiFi.softAPdisconnect(true);
        portalActive = false;
        Serial.println(F("Evil Portal stopped"));
    }
}

// Check if portal is active
bool is_portal_active() {
    return portalActive;
}
