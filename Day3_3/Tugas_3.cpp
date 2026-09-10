#include <Wire.h>
#include <ESP32Servo.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

#define PIN_SERVO_1    13
#define PIN_SERVO_2    14
#define PIN_SERVO_3    27
#define PIN_SERVO_4    26
#define PIN_SERVO_5    25
#define PIN_PIR_SENSOR 15

Adafruit_MPU6050 mpu;

Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;
Servo servo5;

const int INITIAL_ANGLE = 90;
const int ALERT_ANGLE   = 180;

bool isYawActive = false;
unsigned long yawTimer = 0;

void resetAllServos();
void triggerMotionAlert();

void setup() {
  Serial.begin(115200);

  pinMode(PIN_PIR_SENSOR, INPUT);

  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);

  servo1.setPeriodHertz(50);
  servo2.setPeriodHertz(50);
  servo3.setPeriodHertz(50);
  servo4.setPeriodHertz(50);
  servo5.setPeriodHertz(50);

  servo1.attach(PIN_SERVO_1, 500, 2400);
  servo2.attach(PIN_SERVO_2, 500, 2400);
  servo3.attach(PIN_SERVO_3, 500, 2400);
  servo4.attach(PIN_SERVO_4, 500, 2400);
  servo5.attach(PIN_SERVO_5, 500, 2400);

  resetAllServos();

  if (!mpu.begin()) {
    while (1) delay(10);
  }

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
}

void loop() {
  int pirState = digitalRead(PIN_PIR_SENSOR);
  if (pirState == HIGH) {
    triggerMotionAlert();
    return;
  }

  sensors_event_t accel, gyro, temp;
  mpu.getEvent(&accel, &gyro, &temp);

  float roll = atan2(accel.acceleration.y, accel.acceleration.z) * 180.0 / M_PI;
  float pitch = atan2(-accel.acceleration.x, sqrt(accel.acceleration.y * accel.acceleration.y + accel.acceleration.z * accel.acceleration.z)) * 180.0 / M_PI;
  float yawRate = gyro.gyro.z * 180.0 / M_PI;

  roll = constrain(roll, -90.0, 90.0);
  pitch = constrain(pitch, -90.0, 90.0);

  int pos_servo1 = constrain(INITIAL_ANGLE - roll, 0, 180);
  int pos_servo2 = constrain(INITIAL_ANGLE - roll, 0, 180);

  int pos_servo3 = constrain(INITIAL_ANGLE + pitch, 0, 180);
  int pos_servo4 = constrain(INITIAL_ANGLE + pitch, 0, 180);

  int pos_servo5 = INITIAL_ANGLE;
  if (abs(yawRate) > 15.0) {
    float yawDir = (yawRate > 0) ? 45.0 : -45.0;
    pos_servo5 = constrain(INITIAL_ANGLE + yawDir, 0, 180);
    isYawActive = true;
    yawTimer = millis();
  } else if (isYawActive) {
    if (millis() - yawTimer < 1000) {
      pos_servo5 = (yawRate > 0) ? INITIAL_ANGLE + 45 : INITIAL_ANGLE - 45;
    } else {
      pos_servo5 = INITIAL_ANGLE;
      isYawActive = false;
    }
  }

  servo1.write(pos_servo1);
  servo2.write(pos_servo2);
  servo3.write(pos_servo3);
  servo4.write(pos_servo4);
  servo5.write(pos_servo5);

  delay(50);
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

void resetAllServos() {
  servo1.write(INITIAL_ANGLE);
  servo2.write(INITIAL_ANGLE);
  servo3.write(INITIAL_ANGLE);
  servo4.write(INITIAL_ANGLE);
  servo5.write(INITIAL_ANGLE);
}
