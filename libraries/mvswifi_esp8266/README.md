# mvswifi_esp8266

**ESP8266/NodeMCU WiFi Credential Service using EEPROM Storage**

Ultra-minimal WiFi credential management library optimized specifically for ESP8266 and NodeMCU hardware. Uses EEPROM storage for persistent credential management.

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Platform](https://img.shields.io/badge/platform-ESP8266-blue.svg)](https://www.espressif.com/en/products/socs/esp8266)
[![Version](https://img.shields.io/badge/version-1.0.0-green.svg)](https://github.com/vishaljain/mvswifi_esp8266)

---

## 🌟 Features

- ✅ **EEPROM Storage** - Reliable flash-based credential storage
- ✅ **ESP8266 Optimized** - Specifically designed for NodeMCU/ESP8266
- ✅ **Ultra-Lightweight** - Only ~8KB flash, ~3KB RAM
- ✅ **Auto-Detection** - Automatically detects AP mode
- ✅ **Simple API** - Just 2 lines to add WiFi provisioning
- ✅ **Android Compatible** - Works with mvsConnect app
- ✅ **Always Overwrites** - Latest credentials always win
- ✅ **Non-Blocking** - Doesn't interfere with your code

---

## 📦 Installation

### Arduino IDE:
1. Download `mvswifi_esp8266.zip`
2. Arduino IDE → **Sketch** → **Include Library** → **Add .ZIP Library**
3. Select the downloaded ZIP file
4. Library installed! Ready to use.

### PlatformIO:
```ini
[env:nodemcuv2]
platform = espressif8266
board = nodemcuv2
framework = arduino
lib_deps = 
    mvswifi_esp8266
```

---

## 🚀 Quick Start

### Basic Usage (Add WiFi Provisioning in 3 Lines):

```cpp
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <mvswifi_esp8266.h>  // ← Line 1: Include library

const char* DEVICE_NAME = "MyNodeMCU";
ESP8266WebServer server(80);

void setup() {
    Serial.begin(115200);
    
    // Your existing WiFi setup
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP(String(DEVICE_NAME) + "_mvstech", "password");
    
    mvswifi.begin();  // ← Line 2: Start WiFi service
    
    // Optional: Connect to saved WiFi
    mvswifi.connectToSavedWiFi(10000);
    
    // Your existing server code
    server.on("/", []() {
        server.send(200, "text/html", "<h1>Hello NodeMCU!</h1>");
    });
    server.begin();
}

void loop() {
    mvswifi.handle();      // ← Line 3: Handle WiFi operations
    server.handleClient(); // Your existing code
}
```

**That's it! Your ESP8266 now has professional WiFi provisioning!** 🎉

---

## 📱 Android App Integration

### WiFi Configuration:
```
http://192.168.4.1:8080/set-wifi?ssid=HomeWiFi&password=mypassword
```

**Response:**
```
OK: Connecting to HomeWiFi
Status: http://192.168.4.1:8080/status
```

### Check Connection Status:
```
http://192.168.4.1:8080/status
```

**Responses:**
```
CONNECTING: HomeWiFi (5s)
CONNECTED: HomeWiFi IP:192.168.1.150
FAILED: Cannot connect to HomeWiFi
READY: No WiFi configured
```

---

## 🔧 API Reference

### Main Functions:

```cpp
void begin();
// Initialize the library (auto-detects AP mode)
// Call once in setup()

void handle();
// Process WiFi operations
// Call in every loop() iteration

bool isConnected();
// Returns true if connected to WiFi
// Returns: bool

bool isServerActive();
// Returns true if credential server is running
// Returns: bool

String getStatus();
// Get current connection status string
// Returns: Status message

bool connectToSavedWiFi(unsigned long timeout = 10000);
// Connect using saved credentials from EEPROM
// Parameters: timeout in milliseconds (default 10s)
// Returns: true if connected, false if failed
```

### Example Usage:

```cpp
void setup() {
    mvswifi.begin();
    
    if (mvswifi.connectToSavedWiFi()) {
        Serial.println("Connected: " + mvswifi.getStatus());
    }
}

void loop() {
    mvswifi.handle();
    
    if (mvswifi.isConnected()) {
        // Do something when WiFi is connected
    }
}
```

---

## 💾 EEPROM Storage Details

### Storage Mechanism:
```
EEPROM Layout (128 bytes):
├── 0-31:   SSID storage (32 bytes, max 31 chars)
├── 32-63:  Password storage (32 bytes, max 31 chars)
├── 64:     Valid flag (1 byte, 0xFF = valid)
└── 65-127: Unused (63 bytes)

Location: Flash sector (EEPROM emulation)
Max SSID: 31 characters
Max Password: 31 characters
```

### EEPROM on ESP8266:
- ⚠️ **Not real EEPROM** - Flash memory emulation
- ⚠️ **Limited writes** - ~10,000-100,000 write cycles
- ✅ **Persistent** - Survives power loss
- ✅ **Simple** - Easy to understand and debug

---

## 📊 Memory Usage

| Metric | Value |
|--------|-------|
| **Flash (Code)** | ~8KB |
| **RAM (Runtime)** | ~3KB |
| **EEPROM (Storage)** | 128 bytes |
| **Max SSID** | 31 characters |
| **Max Password** | 31 characters |

---

## 🎯 Use Cases

Perfect for:
- ✅ NodeMCU IoT devices
- ✅ ESP8266 projects with Android apps
- ✅ Smart home devices
- ✅ Wireless sensors
- ✅ ESP8266-based products
- ✅ Budget-friendly IoT solutions

---

## 🔒 Security Considerations

- **Flash-based Storage**: Credentials stored in flash memory
- **Automatic Overwrite**: Old credentials replaced safely
- **Simple & Reliable**: Straightforward storage mechanism
- ⚠️ **Not Encrypted**: EEPROM storage is not encrypted (ESP8266 limitation)

---

## 🐛 Troubleshooting

### Problem: Server not starting
```cpp
// Check if AP mode is active
Serial.println(WiFi.getMode()); // Should be WIFI_AP or WIFI_AP_STA
```

### Problem: Cannot save credentials
```cpp
// Test EEPROM functionality
EEPROM.begin(128);
EEPROM.write(0, 0xFF);
if (EEPROM.commit()) {
    Serial.println("EEPROM working!");
}
```

### Problem: Connection fails
```cpp
// Check saved credentials
Serial.println("Saved WiFi: " + mvswifi.getStatus());
```

### Problem: Memory issues
```cpp
// Check free heap
Serial.printf("Free heap: %d bytes\n", ESP.getFreeHeap());
```

---

## 📝 Examples

Check the `examples/` folder for:
- **Basic** - Simple WiFi provisioning setup
- More examples coming soon!

---

## 🔗 Related Projects

- **mvswifi_esp32** - ESP32 version (NVS-based)
- **mvsConnect** - Android app for device management

---

## 📄 License

MIT License - See [LICENSE](LICENSE) file for details.

Copyright (c) 2025 Vishal Jain

---

## 👤 Author

**Vishal Jain**
- Email: jain.vishal1695@gmail.com
- GitHub: [@vishaljain](https://github.com/vishaljain)

---

## 🤝 Contributing

Contributions welcome! Please feel free to submit a Pull Request.

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

---

## 📮 Support

- **Issues**: [GitHub Issues](https://github.com/vishaljain/mvswifi_esp8266/issues)
- **Email**: jain.vishal1695@gmail.com
- **Documentation**: [Full API Docs](https://github.com/vishaljain/mvswifi_esp8266/wiki)

---

## ⚠️ Important Notes

### EEPROM Write Cycles:
- EEPROM emulation has limited write cycles (~10K-100K)
- Library only writes when credentials change
- Avoid frequent credential updates
- Perfect for normal IoT use cases

### NodeMCU Compatibility:
- ✅ Works with all NodeMCU versions
- ✅ Compatible with WeMos D1 Mini
- ✅ Works with generic ESP8266 modules
- ✅ Tested on ESP-12E/F

---

## ⭐ Star History

If you find this library helpful, please consider giving it a star on GitHub!

---

**Made with ❤️ for ESP8266/NodeMCU developers**

*Simple, efficient, reliable WiFi provisioning for ESP8266*