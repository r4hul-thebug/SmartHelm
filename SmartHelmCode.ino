#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;

int ledPin = 8;
int buzzerPin = 9;

float prevAccel = 0;
float currentAccel = 0;
float change = 0;

float angleX, angleY;

int triggerCount = 0;
float threshold = 170;

// Calibration value
float baseAccel = 0;

// 🔧 CALIBRATION FUNCTION
void calibrateSensor() {
  int16_t ax, ay, az;

  Serial.println("Calibrating... Keep helmet still");

  for (int i = 0; i < 50; i++) {
    mpu.getAcceleration(&ax, &ay, &az);
    baseAccel += sqrt(ax * ax + ay * ay + az * az);
    delay(50);
  }

  baseAccel /= 50;

  Serial.print("Baseline Acceleration: ");
  Serial.println(baseAccel);
}

void setup() {
  Serial.begin(9600);
  Wire.begin();
  mpu.initialize();

  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  if (mpu.testConnection()) {
    Serial.println("MPU6050 Connected Successfully");
  } else {
    Serial.println("MPU6050 Connection Failed");
  }

  calibrateSensor();
}

void loop() {
  int16_t ax, ay, az;

  // Read sensor data
  mpu.getAcceleration(&ax, &ay, &az);

  // Total acceleration
  currentAccel = sqrt(ax * ax + ay * ay + az * az);

  // Sudden change (jerk)
  change = abs(currentAccel - prevAccel);

  // Tilt calculation
  angleX = atan2(ay, az) * 180 / PI;
  angleY = atan2(ax, az) * 180 / PI;

  // Serial output (good for plotter too)
  Serial.print("Accel:");
  Serial.print(currentAccel);
  Serial.print(" | Change:");
  Serial.print(change);
  Serial.print(" | AngleX:");
  Serial.print(angleX);
  Serial.print(" | AngleY:");
  Serial.println(angleY);

  // Step 1: Detect sudden jerk
  if (change > threshold) {
    triggerCount++;
  } else {
    triggerCount = 0;
  }

  // Step 2: Confirm with tilt
  bool accident = false;

  if (triggerCount >= 2) {
    Serial.println("Impact detected... checking tilt");

    delay(100);

    if (abs(angleX) > 45 || abs(angleY) > 45) {
      accident = true;
    }
  }

  // Step 3: Final accident confirmation
  if (accident) {
    Serial.println("🚨 ACCIDENT CONFIRMED!");

    digitalWrite(ledPin, HIGH);
    digitalWrite(buzzerPin, HIGH);

    delay(3500);

    triggerCount = 0;
  } else {
    digitalWrite(ledPin, LOW);
    digitalWrite(buzzerPin, LOW);
  }

  prevAccel = currentAccel;

  delay(10);
}
