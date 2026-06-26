# Smart Helmet for Accident Detection

An intelligent, low-cost sensor-fusion prototype designed to autonomously detect two-wheeler accidents and trigger immediate local alerts. Built using an **Arduino Uno R3** and an **MPU6050 6-axis Inertial Measurement Unit (IMU)**, this project implements a dual-stage verification algorithm to maximize sensitivity to genuine crashes while minimizing false alarms caused by typical road irregularities like potholes or speed bumps.

This project was developed as part of the **Sensor & Transducer Lab (January – May 2026)** in the **Department of Electronics and Instrumentation Engineering** at the **National Institute of Technology Silchar**.

---

## Motivation & Objectives
Road traffic accidents remain a leading cause of mortality, particularly for two-wheeler riders. The "golden hour" after an accident is critical, yet accidents in remote areas often go unreported. Standard helmets provide passive protection but cannot actively alert anyone. 

**Project Objectives:**
* Interface an MPU6050 IMU with an Arduino Uno via I²C to continuously sample 3-axis acceleration at 5 ms intervals.
* Implement a **two-stage accident detection algorithm**: (i) threshold-based jerk magnitude detection, followed by (ii) tilt-angle confirmation to eliminate false positives.
* Perform automatic startup calibration to establish a baseline independent of how the helmet is worn.
* Activate a simultaneous visual (LED) and auditory (Buzzer) alert upon crash confirmation.
* Keep the total Bill of Materials (BOM) cost under ₹800 to ensure mass accessibility.

---

## System Architecture & Logic

The system continuously samples kinematic data and processes it using the following logic:

1. **Initialization & Calibration:** The system collects 50 acceleration magnitude samples at startup to calculate a resting baseline (`baseAccel`), adapting to the helmet's current mounting orientation.
2. **Jerk Check:** It calculates the scalar magnitude of acceleration (currentAccel = √(ax² + ay² + az²)) and finds the rate of change (change = |currentAccel - prevAccel|). If this change exceeds **170 raw sensor units** for at least **2 consecutive samples**, it flags a "suspected impact".
3. **Tilt Confirmation:** After a brief 100 ms stabilization pause, it calculates the helmet's tilt angles using trigonometric ratios. If either |angleX| or |angleY| exceeds **45°** (indicating the rider has fallen), an accident is confirmed.

    [ INPUT ] MPU6050 6-Axis IMU (Accel Data)
                 │  (via I²C Bus)
                 ▼
    [PROCESSING] Arduino Uno (Jerk Calc -> 100ms Delay -> Tilt-Angle Check)
                 │
                 ▼
    [ OUTPUT ]  Dual Alert: Red LED (Pin 8) + Active Buzzer (Pin 9)
                Duration: 2.5s -> Auto-Reset back to Monitoring

---

## Hardware Components & Wiring

| Component | Pin / Terminal | Connected To (Arduino Uno) | Signal / Role |
| :--- | :--- | :--- | :--- |
| **MPU6050 IMU** | VCC | 5V | Power Supply |
| **MPU6050 IMU** | GND | GND | Common Ground |
| **MPU6050 IMU** | SDA | A4 | I²C Data Line |
| **MPU6050 IMU** | SCL | A5 | I²C Clock Line |
| **MPU6050 IMU** | AD0 | GND | Pulls low to establish I²C address `0x68` |
| **Active Buzzer** | Positive (+) | Pin 9 | Digital Output (Auditory Alert) |
| **Active Buzzer** | Negative (−) | GND | Common Ground |
| **Red LED** | Anode (+) | Pin 8 (via 220Ω resistor) | Digital Output (Visual Alert) |
| **Red LED** | Cathode (−) | GND Directly | Common Ground |

> **Note:** For laboratory debugging, the system runs reliably via a USB connection. For untethered field deployment, a standard 9V battery can be attached to the Arduino barrel jack.

---

## Software Setup

### Prerequisites
1. Download and install the [Arduino IDE 2.x](https://www.arduino.cc/en/software).
2. Open the **Library Manager** (`Ctrl+Shift+I`) and install the following dependencies:
   * **Wire.h** (Built-in I²C library)
   * **MPU6050.h** (by ElectronicCats)

### Installation & Upload
1. Clone this repository to your local machine:
   git clone https://github.com/YOUR_USERNAME/Smart-Helmet-Accident-Detection.git
2. Open the main `.ino` file in your Arduino IDE.
3. Connect your Arduino Uno board via USB.
4. Select `Arduino Uno` under **Tools -> Board** and choose your active COM port.
5. Click **Upload** to flash the firmware.

---

## Testing Protocol & Results

Follow these bench-testing steps to verify correct execution:

1. **Startup Calibration:** Power on the circuit and keep the helmet completely still for ~2.5 seconds. The Serial Monitor will print a baseline acceleration around `~16384` raw units (representing Earth's gravity at a ±2g scale).
2. **False Positive Rejection Test:** Gently tap or shake the sensor slightly to simulate minor road vibrations or potholes. Check the Serial Monitor to ensure `Change < 170` and that no alert triggers.
3. **Accident Simulation:** Apply a rapid, sharp jerk to the sensor **AND** immediately tilt it over past 45 degrees.
4. **Expected Output:**
   * The Serial Monitor will log: `Impact detected... checking tilt` followed by `ACCIDENT CONFIRMED!`.
   * The LED and active buzzer will activate simultaneously for exactly 2.5 seconds before auto-resetting back to monitoring mode. (Alert triggers within ~15 ms of confirmation).

---

## Real-World Applications
* **Road Safety:** Primary application for motorcycle, scooter, and bicycle riders to reduce response times during accidents.
* **Industrial & Mining Safety:** Wearable tech for workers in hazardous environments where occupational fall risks are high.
* **Elderly Care:** Adaptation for individuals prone to balance loss or sudden collapse.

---

## Future Enhancements
* **Remote GSM/GPS Alerts:** Integrating a **SIM800L GSM module** and a **GPS tracker** to automatically send cellular SOS texts with live map coordinates to emergency contacts upon accident confirmation.
* **Sensor Fusion Filtering:** Implementing a **Complementary or Kalman Filter** that pairs both accelerometer and gyroscope telemetry to mitigate dynamic-braking tilt errors.
* **Bluetooth App Integration:** Pairing an HC-05 Bluetooth module with a companion Android/iOS app to provide push notifications, ride analytics, and remote monitoring.
* **Advanced Severity Analysis:** Training lightweight machine learning models to classify crash impacts into tiered severities (Minor, Moderate, Critical) for smarter emergency dispatching.
* **Power Management Optimization:** Transitioning from the standard 9V alkaline battery to a high-density rechargeable LiPo system coupled with micro-sleep states to extend field operation past 24 hours.
