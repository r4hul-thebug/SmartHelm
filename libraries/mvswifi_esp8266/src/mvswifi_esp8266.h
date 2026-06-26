/*
 * mvswifi_esp8266 Library - ESP8266 WiFi Credential Service (EEPROM-based)
 * Version: 1.0.0
 * Platform: ESP8266/NodeMCU only
 * 
 * Ultra-minimal WiFi credential management using EEPROM storage.
 * Optimized specifically for ESP8266 hardware.
 * 
 * Features:
 * - EEPROM storage (flash emulation)
 * - Auto-detects AP mode and starts server on port 8080
 * - WiFi configuration: /set-wifi?ssid=X&password=Y
 * - Status check: /status
 * - Always overwrites old credentials
 * - Ultra-low memory: ~8KB flash, ~3KB RAM
 * 
 * Usage:
 *   #include <mvswifi_esp8266.h>
 *   
 *   void setup() {
 *     WiFi.softAP("MyDevice_mvstech", "password");
 *     mvswifi.begin();
 *   }
 *   
 *   void loop() {
 *     mvswifi.handle();
 *   }
 */

#ifndef MVSWIFI_ESP8266_H
#define MVSWIFI_ESP8266_H

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>

class MVSWiFi_ESP8266 {
private:
    static MVSWiFi_ESP8266* instance;
    ESP8266WebServer* server;
    bool initialized;
    bool serverStarted;
    bool apModeActive;
    
    // EEPROM addresses
    static const int EEPROM_SIZE = 128;
    static const int SSID_ADDR = 0;
    static const int PASS_ADDR = 32;
    static const int VALID_ADDR = 64;
    
    // Connection tracking
    String lastSSID;
    String connectionStatus;
    unsigned long connectionStartTime;
    bool isConnecting;
    
    // Private constructor for singleton
    MVSWiFi_ESP8266();
    
    // Internal methods
    void initializeEEPROM();
    void detectAPMode();
    void setupWebServer();
    void updateConnectionStatus();
    
    // EEPROM helpers
    void saveCredentials(const String& ssid, const String& password);
    String readSSID();
    String readPassword();
    bool hasSavedCredentials();
    void clearCredentials();
    void writeStringToEEPROM(int addr, const String& data);
    String readStringFromEEPROM(int addr);
    
public:
    // Singleton instance getter
    static MVSWiFi_ESP8266& getInstance();
    
    // Main library functions
    void begin();
    void handle();
    
    // Status functions
    bool isConnected();
    bool isServerActive();
    String getStatus();
    
    // Configuration functions
    bool connectToSavedWiFi(unsigned long timeout = 10000);
    
    // Destructor
    ~MVSWiFi_ESP8266();
};

// Global instance for easy access
extern MVSWiFi_ESP8266& mvswifi;

#endif // MVSWIFI_ESP8266_H