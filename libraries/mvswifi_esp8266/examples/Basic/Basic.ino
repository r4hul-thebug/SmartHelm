/*
 * mvswifi_esp8266 Basic Example
 * 
 * This example shows how to add WiFi credential capability 
 * to your ESP8266/NodeMCU project using EEPROM storage.
 * 
 * Hardware: ESP8266 / NodeMCU
 * Library: mvswifi_esp8266
 * 
 * Features:
 * - EEPROM storage (flash emulation)
 * - Auto-detects AP mode
 * - WiFi credential service on port 8080
 * - Your main server on port 80
 * - Android app compatible
 */

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <mvswifi_esp8266.h>

// Device configuration
const char* DEVICE_NAME = "MyNodeMCU";
const char* AP_PASSWORD = "nodemcu123";

// Your main application server (port 80)
ESP8266WebServer server(80);

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("🚀 Starting ESP8266 with mvswifi...");
    
    // Configure WiFi in AP+STA mode
    WiFi.mode(WIFI_AP_STA);
    
    // Start Access Point (required for Android app discovery)
    String apName = String(DEVICE_NAME) + "_mvstech";
    WiFi.softAP(apName.c_str(), AP_PASSWORD);
    
    Serial.printf("📡 AP: %s (Password: %s)\n", apName.c_str(), AP_PASSWORD);
    Serial.printf("🌐 AP IP: %s\n", WiFi.softAPIP().toString().c_str());
    
    // Start mvswifi credential service (EEPROM-based)
    mvswifi.begin();
    
    // Try to connect to saved WiFi (non-blocking)
    if (mvswifi.connectToSavedWiFi(10000)) {
        Serial.println("✅ Connected to saved WiFi network");
        Serial.printf("🌐 WiFi IP: %s\n", WiFi.localIP().toString().c_str());
    } else {
        Serial.println("ℹ️  No saved WiFi - AP mode ready for configuration");
    }
    
    // Setup your main application server
    setupMainServer();
    
    Serial.println("\n🎉 Setup complete!");
    Serial.println("📱 Android App URLs:");
    Serial.println("   Main Device: http://192.168.4.1/");
    Serial.println("   WiFi Setup: http://192.168.4.1:8080/set-wifi?ssid=YourWiFi&password=YourPass");
    Serial.println("   Status Check: http://192.168.4.1:8080/status");
}

void loop() {
    // Handle WiFi credential service
    mvswifi.handle();
    
    // Handle your main application server
    server.handleClient();
    
    // Your main application code here
}

void setupMainServer() {
    // Your main page
    server.on("/", HTTP_GET, []() {
        String html = "<html><head>";
        html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
        html += "<title>" + String(DEVICE_NAME) + "</title>";
        html += "<style>";
        html += "body { font-family: Arial, sans-serif; margin: 40px; background: #f0f0f0; }";
        html += ".container { background: white; padding: 30px; border-radius: 10px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); }";
        html += "h1 { color: #333; text-align: center; }";
        html += ".status { background: #e3f2fd; padding: 15px; border-radius: 5px; margin: 20px 0; }";
        html += "</style></head><body>";
        
        html += "<div class='container'>";
        html += "<h1>🚀 " + String(DEVICE_NAME) + " (ESP8266)</h1>";
        html += "<h2>Hello from NodeMCU with EEPROM!</h2>";
        
        html += "<div class='status'>";
        html += "<h3>📊 Device Status</h3>";
        html += "<p><strong>Platform:</strong> ESP8266/NodeMCU</p>";
        html += "<p><strong>Storage:</strong> EEPROM (Flash Emulation)</p>";
        html += "<p><strong>AP:</strong> " + WiFi.softAPSSID() + "</p>";
        
        if (WiFi.status() == WL_CONNECTED) {
            html += "<p><strong>WiFi:</strong> Connected to " + WiFi.SSID() + "</p>";
            html += "<p><strong>WiFi IP:</strong> " + WiFi.localIP().toString() + "</p>";
        } else {
            html += "<p><strong>WiFi:</strong> Not connected</p>";
        }
        
        html += "<p><strong>Uptime:</strong> " + String(millis() / 1000) + " seconds</p>";
        html += "<p><strong>Free Heap:</strong> " + String(ESP.getFreeHeap() / 1024) + " KB</p>";
        html += "</div>";
        
        html += "<p><small>WiFi Config: <a href='http://192.168.4.1:8080/'>http://192.168.4.1:8080/</a></small></p>";
        html += "</div></body></html>";
        
        server.send(200, "text/html", html);
    });
    
    // Sample download endpoint
    server.on("/download/sample.txt", HTTP_GET, []() {
        String content = "ESP8266 Sample File\n";
        content += "Device: " + String(DEVICE_NAME) + "\n";
        content += "Platform: ESP8266/NodeMCU\n";
        content += "Storage: EEPROM\n";
        content += "Timestamp: " + String(millis()) + " ms\n";
        
        server.sendHeader("Content-Disposition", "attachment; filename=nodemcu_sample.txt");
        server.send(200, "text/plain", content);
    });
    
    server.onNotFound([]() {
        server.send(404, "text/plain", "File not found");
    });
    
    server.begin();
    Serial.println("✅ Main server started on port 80");
}