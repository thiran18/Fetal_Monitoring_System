#include <Wire.h>
#include <MPU6050.h>
#include <TCA9548A.h>
#include "BluetoothSerial.h"

#define SCL_PIN 22
#define SDA_PIN 21
BluetoothSerial SerialBT;
MPU6050 mpu1, mpu2, mpu3;
TCA9548A mux;

#define BUZZER_PIN 4
#define LED_BUILTIN 2
#define MPU_COUNT 3
#define NORMAL_THRESHOLD 1200  // Adjust according to your requirements
#define STRETCHING_THRESHOLD 1500
#define KICKS_THRESHOLD 1700
#define ABNORMAL_INTERVAL 5000

void TCA9548A(uint8_t bus) {
  Wire.beginTransmission(0x70);
  Wire.write(1 << bus);
  Wire.endTransmission();
}

void setup() {

  Serial.begin(115200);
  SerialBT.begin("ESP32_Test"); // Name of the Bluetooth device
  pinMode(LED,OUTPUT);


  
  Serial.println("The device started, now you can pair it with Bluetooth!");

  Wire.begin(SDA_PIN, SCL_PIN);  // Initialize I2C with specified pins
  mux.begin();

  TCA9548A(2);
  mpu1.initialize();

  TCA9548A(3);
  mpu2.initialize();

  TCA9548A(6);
  mpu3.initialize();

  pinMode(BUZZER_PIN, OUTPUT);
}

void loop() {
  int16_t ax, ay, az;
  static unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();

  if (Serial.available()) {
    SerialBT.write(Serial.read());
    digitalWrite(LED,HIGH);
    delay(500);
  }
  if (SerialBT.available()) {
    Serial.write(SerialBT.read());
  }
  delay(20);

  if (currentMillis - previousMillis >= ABNORMAL_INTERVAL) {
    bool abnormal = checkForAbnormalCondition();
    if (abnormal) {
      triggerAlarm();
    }
    previousMillis = currentMillis;
  }
}

bool checkForAbnormalCondition() {
  int16_t ax, ay, az;
  bool abnormal = false;

  // Read data from MPU1
  mux.openChannel(2);
  mpu1.getAcceleration(&ax, &ay, &az);
  float magnitude = sqrt(ax * ax + ay * ay + az * az);
  if (magnitude > KICKS_THRESHOLD) {
    Serial.println(magnitude);
    Serial.println("Kicks detected from MPU1");
    abnormal = true;
  } else if (magnitude > STRETCHING_THRESHOLD) {
    Serial.println(magnitude);
    Serial.println("Stretching/Rolling detected from MPU1");
  } else if (magnitude > NORMAL_THRESHOLD) {
    Serial.println(magnitude);
    Serial.println("Normal position detected from MPU1");
  }

  // Read data from MPU2
  mux.openChannel(3);
  mpu2.getAcceleration(&ax, &ay, &az);
  magnitude = sqrt(ax * ax + ay * ay + az * az);
  if (magnitude > KICKS_THRESHOLD) {
    Serial.println(magnitude);
    Serial.println("Kicks detected from MPU2");
    abnormal = true;
  } else if (magnitude > STRETCHING_THRESHOLD) {
    Serial.println(magnitude);
    Serial.println("Stretching/Rolling detected from MPU2");
  } else if (magnitude > NORMAL_THRESHOLD) {
    Serial.println(magnitude);
    Serial.println("Normal position detected from MPU2");
  }

  // Read data from MPU3
  mux.openChannel(6);
  mpu3.getAcceleration(&ax, &ay, &az);
  magnitude = sqrt(ax * ax + ay * ay + az * az);
  if (magnitude > KICKS_THRESHOLD) {
    Serial.println(magnitude);
    Serial.println("Kicks detected from MPU3");
    abnormal = true;
  } else if (magnitude > STRETCHING_THRESHOLD) {
    Serial.println(magnitude);
    Serial.println("Stretching/Rolling detected from MPU3");
  } else if (magnitude > NORMAL_THRESHOLD) {
    Serial.println(magnitude);
    Serial.println("Normal position detected from MPU3");
  }

  return abnormal;
}

void triggerAlarm() {
  // Code to trigger the alarm, e.g., turning on the buzzer
  digitalWrite(BUZZER_PIN, HIGH);
  delay(1000);  // Buzzer on for 1 second
  digitalWrite(BUZZER_PIN, LOW);
}
