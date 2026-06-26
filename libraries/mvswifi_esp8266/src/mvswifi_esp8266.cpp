/*
 * mvswifi_esp8266 Library Implementation
 * ESP8266 WiFi Credential Service (EEPROM-based)
 */

#include "mvswifi_esp8266.h"

// Static member initialization
MVSWiFi_ESP8266* MVSWiFi_ESP8266::instance = nullptr;

// Global instance reference
MVSWiFi_ESP8266& mvswifi = MVSWiFi_ESP8266::getInstance();

// Constructor
MVSWiFi_ESP8266::MVSWiFi_ESP8266() : 
    server(nullptr), 
    initialized(false), 
    serverStarted(false),
    apModeActive(false),
    connectionStartTime(0),
    isConnecting(false) {
}

// Destructor
MVSWiFi_ESP8266::~MVSWiFi_ESP8266() {
    if (server) {
        delete server;
    }
}

// Singleton pattern
MVSWiFi_ESP8266& MVSWiFi_ESP8266::getInstance() {
    if (!instance) {
        instance = new MVSWiFi_ESP8266();
    }
    return *instance;
}

// Initialize the library
void MVSWiFi_ESP8266::begin() {
    if (initialized) return;
    
    Serial.println("🌐 mvswifi_esp8266 v1.0 - ESP8266 WiFi Service (EEPROM)");
    
    // Initialize EEPROM
    initializeEEPROM();
    
    // Detect current WiFi mode
    detectAPMode();
    
    // Start server if AP mode is active
    if (apModeActive) {
        setupWebServer();
        Serial.printf("✅ Credential server: http://%s:8080\n", WiFi.softAPIP().toString().c_str());
    } else {
        Serial.println("ℹ️  No AP mode detected - server not started");
    }
    
    initialized = true;
}

// Handle library operations (call this in loop)
void MVSWiFi_ESP8266::handle() {
    if (!initialized || !serverStarted) return;
    
    if (server) {
        server->handleClient();
    }
    
    updateConnectionStatus();
}

// Initialize EEPROM
void MVSWiFi_ESP8266::initializeEEPROM() {
    EEPROM.begin(EEPROM_SIZE);
}

// Detect if AP mode is active
void MVSWiFi_ESP8266::detectAPMode() {
    WiFiMode_t mode = WiFi.getMode();
    
    if (mode == WIFI_AP || mode == WIFI_AP_STA) {
        apModeActive = true;
        Serial.printf("📡 AP Mode detected: %s\n", WiFi.softAPSSID().c_str());
    } else {
        apModeActive = false;
        Serial.println("📱 Station Mode - No AP detected");
    }
}

// Setup minimal web server
void MVSWiFi_ESP8266::setupWebServer() {
    if (server) {
        delete server;
    }
    
    server = new ESP8266WebServer(8080);
    
    // WiFi configuration endpoint
    server->on("/set-wifi", HTTP_GET, [this]() {
        if (server->hasArg("ssid")) {
            String ssid = server->arg("ssid");
            String password = server->hasArg("password") ? server->arg("password") : "";
            
            Serial.printf("🔑 WiFi config: %s\n", ssid.c_str());
            
            // Always overwrite old credentials
            saveCredentials(ssid, password);
            
            // Start connection immediately
            lastSSID = ssid;
            isConnecting = true;
            connectionStartTime = millis();
            connectionStatus = "CONNECTING";
            
            WiFi.begin(ssid.c_str(), password.c_str());
            
            // Send immediate response
            String response = "OK: Connecting to " + ssid + "\n";
            response += "Status: http://" + WiFi.softAPIP().toString() + ":8080/status";
            
            server->send(200, "text/plain", response);
            
        } else {
            server->send(400, "text/plain", "Usage: /set-wifi?ssid=YourWiFi&password=YourPass");
        }
    });
    
    // Simple status endpoint
    server->on("/status", HTTP_GET, [this]() {
        updateConnectionStatus();
        server->send(200, "text/plain", connectionStatus);
    });
    
    // 404 handler
    server->onNotFound([this]() {
        String message = "mvswifi_esp8266 - Endpoints:\n";
        message += "GET /set-wifi?ssid=X&password=Y\n";
        message += "GET /status\n";
        server->send(404, "text/plain", message);
    });
    
    server->begin();
    serverStarted = true;
    Serial.println("✅ Minimal server started on port 8080");
}

// Update connection status
void MVSWiFi_ESP8266::updateConnectionStatus() {
    if (!isConnecting && WiFi.status() != WL_CONNECTED) {
        // Not trying to connect and not connected
        if (lastSSID.length() > 0) {
            connectionStatus = "FAILED: Cannot connect to " + lastSSID;
        } else {
            connectionStatus = "READY: No WiFi configured";
        }
        return;
    }
    
    if (isConnecting) {
        unsigned long elapsed = (millis() - connectionStartTime) / 1000;
        
        if (WiFi.status() == WL_CONNECTED) {
            // Successfully connected
            connectionStatus = "CONNECTED: " + lastSSID + " IP:" + WiFi.localIP().toString();
            isConnecting = false;
            Serial.printf("✅ Connected to %s (IP: %s)\n", lastSSID.c_str(), WiFi.localIP().toString().c_str());
        } else if (elapsed > 30) {
            // Connection timeout after 30 seconds
            connectionStatus = "FAILED: Connection timeout to " + lastSSID;
            isConnecting = false;
            Serial.printf("❌ Connection failed: %s\n", lastSSID.c_str());
        } else {
            // Still connecting
            connectionStatus = "CONNECTING: " + lastSSID + " (" + String(elapsed) + "s)";
        }
    } else if (WiFi.status() == WL_CONNECTED) {
        // Already connected
        connectionStatus = "CONNECTED: " + WiFi.SSID() + " IP:" + WiFi.localIP().toString();
    }
}

// Public status functions
bool MVSWiFi_ESP8266::isConnected() {
    return WiFi.status() == WL_CONNECTED;
}

bool MVSWiFi_ESP8266::isServerActive() {
    return serverStarted && server;
}

String MVSWiFi_ESP8266::getStatus() {
    updateConnectionStatus();
    return connectionStatus;
}

// Connect to saved WiFi with timeout
bool MVSWiFi_ESP8266::connectToSavedWiFi(unsigned long timeout) {
    if (!hasSavedCredentials()) {
        Serial.println("ℹ️  No saved WiFi credentials in EEPROM");
        return false;
    }
    
    String ssid = readSSID();
    String password = readPassword();
    
    if (ssid.length() == 0) {
        Serial.println("❌ Invalid saved credentials");
        return false;
    }
    
    Serial.printf("🔄 Connecting to saved WiFi: %s\n", ssid.c_str());
    
    lastSSID = ssid;
    isConnecting = true;
    connectionStartTime = millis();
    WiFi.begin(ssid.c_str(), password.c_str());
    
    // Wait for connection with timeout
    unsigned long startTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startTime < timeout) {
        delay(100);
    }
    
    isConnecting = false;
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.printf("✅ Connected! IP: %s\n", WiFi.localIP().toString().c_str());
        return true;
    }
    
    Serial.println("❌ Connection failed");
    return false;
}

// EEPROM helper functions
void MVSWiFi_ESP8266::saveCredentials(const String& ssid, const String& password) {
    writeStringToEEPROM(SSID_ADDR, ssid);
    writeStringToEEPROM(PASS_ADDR, password);
    EEPROM.write(VALID_ADDR, 0xFF);
    EEPROM.commit();
    Serial.println("💾 Credentials saved to EEPROM (overwritten)");
}

String MVSWiFi_ESP8266::readSSID() {
    return readStringFromEEPROM(SSID_ADDR);
}

String MVSWiFi_ESP8266::readPassword() {
    return readStringFromEEPROM(PASS_ADDR);
}

bool MVSWiFi_ESP8266::hasSavedCredentials() {
    return EEPROM.read(VALID_ADDR) == 0xFF;
}

void MVSWiFi_ESP8266::clearCredentials() {
    EEPROM.write(VALID_ADDR, 0x00);
    EEPROM.commit();
    Serial.println("🗑️ Credentials cleared from EEPROM");
}

void MVSWiFi_ESP8266::writeStringToEEPROM(int addr, const String& data) {
    int len = data.length();
    if (len > 31) len = 31; // Safety limit
    EEPROM.write(addr, len);
    for (int i = 0; i < len; i++) {
        EEPROM.write(addr + 1 + i, data[i]);
    }
}

String MVSWiFi_ESP8266::readStringFromEEPROM(int addr) {
    int len = EEPROM.read(addr);
    if (len > 31 || len < 0) return ""; // Safety check
    
    String data = "";
    for (int i = 0; i < len; i++) {
        data += char(EEPROM.read(addr + 1 + i));
    }
    return data;
}