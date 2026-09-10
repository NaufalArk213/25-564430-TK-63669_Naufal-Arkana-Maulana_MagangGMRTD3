#include <Wire.h>
#include <ESP32Servo.h>

#define PIN_SERVO_1    13
#define PIN_SERVO_2    14
#define PIN_SERVO_3    27
#define PIN_SERVO_4    26
#define PIN_SERVO_5    25
#define PIN_PIR_SENSOR 15

#define MPU_ADDR 0x68

Servo servo1, servo2, servo3, servo4, servo5;

const int INITIAL_ANGLE = 90;
const int ALERT_ANGLE   = 180;

bool isYawActive = false;
unsigned long yawTimer = 0;

void resetAllServos() {
  servo1.write(INITIAL_ANGLE);
  servo2.write(INITIAL_ANGLE);
  servo3.write(INITIAL_ANGLE);
  servo4.write(INITIAL_ANGLE);
  servo5.write(INITIAL_ANGLE);
}

void triggerMotionAlert() {
  servo1.write(ALERT_ANGLE);
  servo2.write(ALERT_ANGLE);
  servo3.write(ALERT_ANGLE);
  servo4.write(ALERT_ANGLE);
  servo5.write(ALERT_ANGLE);
  delay(1500);

  resetAllServos();
  delay(1000);
}

void setup() {
  Serial.begin(115200);
  pinMode(PIN_PIR_SENSOR, INPUT);

  Wire.begin(21, 22);
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B); // Wake up MPU6050
  Wire.write(0);
  Wire.endTransmission(true);

  servo1.attach(PIN_SERVO_1);
  servo2.attach(PIN_SERVO_2);
  servo3.attach(PIN_SERVO_3);
  servo4.attach(PIN_SERVO_4);
  servo5.attach(PIN_SERVO_5);

  resetAllServos();
}

void loop() {
  int pirState = digitalRead(PIN_PIR_SENSOR);
  if (pirState == HIGH) {
    triggerMotionAlert();
    return;
  }

  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 14, true);

  int16_t ax = (Wire.read() << 8) | Wire.read();
  int16_t ay = (Wire.read() << 8) | Wire.read();
  int16_t az = (Wire.read() << 8) | Wire.read();
  Wire.read(); Wire.read(); // Skip temp
  Wire.read(); Wire.read(); Wire.read(); Wire.read(); // Skip Gyro X & Y
  int16_t gz = (Wire.read() << 8) | Wire.read();

  float roll = atan2((float)ay, (float)az) * 180.0 / M_PI;
  float pitch = atan2(-(float)ax, sqrt((float)ay * ay + (float)az * az)) * 180.0 / M_PI;
  float yawRate = (float)gz / 131.0;

  roll = constrain(roll, -90.0, 90.0);
  pitch = constrain(pitch, -90.0, 90.0);

  int pos1 = constrain(INITIAL_ANGLE - roll, 0, 180);
  int pos2 = constrain(INITIAL_ANGLE - roll, 0, 180);

  int pos3 = constrain(INITIAL_ANGLE + pitch, 0, 180);
  int pos4 = constrain(INITIAL_ANGLE + pitch, 0, 180);

  int pos5 = INITIAL_ANGLE;
  if (abs(yawRate) > 15.0) {
    float yawDir = (yawRate > 0) ? 45.0 : -45.0;
    pos5 = constrain(INITIAL_ANGLE + yawDir, 0, 180);
    isYawActive = true;
    yawTimer = millis();
  } else if (isYawActive) {
    if (millis() - yawTimer < 1000) {
      pos5 = (yawRate > 0) ? INITIAL_ANGLE + 45 : INITIAL_ANGLE - 45;
    } else {
      pos5 = INITIAL_ANGLE;
      isYawActive = false;
    }
  }

  servo1.write(pos1);
  servo2.write(pos2);
  servo3.write(pos3);
  servo4.write(pos4);
  servo5.write(pos5);

  delay(50);
}
