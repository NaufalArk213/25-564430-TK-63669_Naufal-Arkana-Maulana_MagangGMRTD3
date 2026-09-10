/*
 * =================================================================================
 * PENUGASAN MAGANG GMRT 2026 - DAY 3 (C++ ARDUINO FRAMEWORK FOR ESP32)
 * KASUS: FOLLOWING-AXIS SERVO MOTOR AND MOTION DETECTION
 * =================================================================================
 * Board        : ESP32 Dev Module
 * Sensor       : MPU6050 (I2C: SDA=21, SCL=22) & PIR Motion Sensor (Digital)
 * Aktuator     : 5x Motor Servo (Servo 1, 2, 3, 4, 5)
 * Platform     : Wokwi Simulator (https://wokwi.com)
 * =================================================================================
 */

#include <Wire.h>               // Library komunikasi I2C (untuk MPU6050)
#include <ESP32Servo.h>         // Library kontrol motor servo khusus ESP32
#include <Adafruit_MPU6050.h>   // Library sensor Gyroscope & Accelerometer MPU6050
#include <Adafruit_Sensor.h>

// ==========================================
// 1. ALOKASI PIN HARDWARE PADA ESP32
// ==========================================
#define PIN_SERVO_1    13   // Servo Roll 1 (Berdekatan dengan Servo 2)
#define PIN_SERVO_2    14   // Servo Roll 2 (Berdekatan dengan Servo 1)
#define PIN_SERVO_3    27   // Servo Pitch 3 (Berdekatan dengan Servo 4)
#define PIN_SERVO_4    26   // Servo Pitch 4 (Berdekatan dengan Servo 3)
#define PIN_SERVO_5    25   // Servo Yaw 5 (Bebas diletakkan)
#define PIN_PIR_SENSOR 15   // Pin Input Digital dari Sensor PIR Motion

// ==========================================
// 2. OBJEK DAN VARIABEL GLOBAL
// ==========================================
Adafruit_MPU6050 mpu;

Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;
Servo servo5;

// Posisi awal (initial) = 90 derajat (Tegak lurus / Posisi 0 derajat referensi)
const int INITIAL_ANGLE = 90;
const int ALERT_ANGLE   = 180; // Posisi tujuan saat PIR mendeteksi gerakan

// Variabel penanda waktu untuk penundaan non-blocking pada Servo Yaw 5
bool isYawActive = false;
unsigned long yawTimer = 0;

// Deklarasi fungsi pendukung
void resetAllServos();
void triggerMotionAlert();

void setup() {
  // Inisialisasi komunikasi serial untuk monitoring
  Serial.begin(115200);

  // Inisialisasi pin PIR Sensor sebagai Input Digital
  pinMode(PIN_PIR_SENSOR, INPUT);

  // Alokasi timer PWM untuk ESP32Servo
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);

  // Set frekuensi servo standar (50Hz)
  servo1.setPeriodHertz(50);
  servo2.setPeriodHertz(50);
  servo3.setPeriodHertz(50);
  servo4.setPeriodHertz(50);
  servo5.setPeriodHertz(50);

  // Menghubungkan pin ESP32 ke objek servo (dengan rentang pulsa 500us - 2400us)
  servo1.attach(PIN_SERVO_1, 500, 2400);
  servo2.attach(PIN_SERVO_2, 500, 2400);
  servo3.attach(PIN_SERVO_3, 500, 2400);
  servo4.attach(PIN_SERVO_4, 500, 2400);
  servo5.attach(PIN_SERVO_5, 500, 2400);

  // Set posisi awal semua servo ke posisi tegak lurus (90 derajat)
  resetAllServos();

  // Inisialisasi sensor MPU6050 via komunikasi I2C
  Serial.print("Menginisialisasi MPU6050... ");
  if (!mpu.begin()) {
    Serial.println("Gagal terhubung! Periksa sambungan kabel I2C (SDA/SCL).");
    while (1) delay(10);
  }
  Serial.println("Berhasil!");

  // Konfigurasi sensitivitas sensor MPU6050
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
}

void loop() {
  // =========================================================================
  // ATURAN 4: DETEKSI GERAKAN EKSTERNAL DARI SENSOR PIR
  // =========================================================================
  int pirState = digitalRead(PIN_PIR_SENSOR);
  if (pirState == HIGH) {
    Serial.println("\n[ALERT] GERAKAN EKSTERNAL TERDETEKSI DARI PIR SENSOR!");
    triggerMotionAlert();
    return; // Berhenti sejenak dari pembacaan MPU6050 saat aksi alert berjalan
  }

  // =========================================================================
  // MEMBACA DATA AKSELERASI & GYRO DARI MPU6050
  // =========================================================================
  sensors_event_t accel, gyro, temp;
  mpu.getEvent(&accel, &gyro, &temp);

  // Menghitung sudut kemiringan Roll (sumbu X) dan Pitch (sumbu Y) dari Akselerometer
  float roll = atan2(accel.acceleration.y, accel.acceleration.z) * 180.0 / M_PI;
  float pitch = atan2(-accel.acceleration.x, sqrt(accel.acceleration.y * accel.acceleration.y + accel.acceleration.z * accel.acceleration.z)) * 180.0 / M_PI;

  // Membaca kecepatan rotasi Yaw (sumbu Z) dari Gyroscope
  float yawRate = gyro.gyro.z * 180.0 / M_PI;

  // Batasi rentang kemiringan dari -90 derajat hingga +90 derajat
  roll = constrain(roll, -90.0, 90.0);
  pitch = constrain(pitch, -90.0, 90.0);

  // =========================================================================
  // ATURAN 1: ROLL (SERVO 1 & 2 BERPUTAR MELAWAN ARAH KEMIRINGAN)
  // Roll Positif -> Servo 1 & 2 berputar ke arah Negatif (INITIAL - roll)
  // Roll Negatif -> Servo 1 & 2 berputar ke arah Positif (INITIAL - roll)
  // =========================================================================
  int pos_servo1 = constrain(INITIAL_ANGLE - roll, 0, 180);
  int pos_servo2 = constrain(INITIAL_ANGLE - roll, 0, 180);

  // =========================================================================
  // ATURAN 2: PITCH (SERVO 3 & 4 BERPUTAR SEARAH KEMIRINGAN)
  // Pitch Positif -> Servo 3 & 4 berputar ke arah Positif (INITIAL + pitch)
  // Pitch Negatif -> Servo 3 & 4 berputar ke arah Negatif (INITIAL + pitch)
  // =========================================================================
  int pos_servo3 = constrain(INITIAL_ANGLE + pitch, 0, 180);
  int pos_servo4 = constrain(INITIAL_ANGLE + pitch, 0, 180);

  // =========================================================================
  // ATURAN 3: YAW (SERVO 5 BERPUTAR MENGIKUTI YAW, DIAM 1 DETIK, LALU RESET)
  // =========================================================================
  int pos_servo5 = INITIAL_ANGLE;
  if (abs(yawRate) > 15.0) { // Ambang batas rotasi yaw
    float yawDir = (yawRate > 0) ? 45.0 : -45.0;
    pos_servo5 = constrain(INITIAL_ANGLE + yawDir, 0, 180);
    isYawActive = true;
    yawTimer = millis(); // Catat waktu rotasi selesai
  } else {
    if (isYawActive) {
      if (millis() - yawTimer < 1000) {
        // Tahan posisi selama 1 detik setelah rotasi selesai
        pos_servo5 = (yawRate > 0) ? INITIAL_ANGLE + 45 : INITIAL_ANGLE - 45;
      } else {
        // Kembali ke posisi awal 90 derajat
        pos_servo5 = INITIAL_ANGLE;
        isYawActive = false;
      }
    }
  }

  // Kirim perintah sudut ke masing-masing motor servo
  servo1.write(pos_servo1);
  servo2.write(pos_servo2);
  servo3.write(pos_servo3);
  servo4.write(pos_servo4);
  servo5.write(pos_servo5);

  // Tampilkan telemetri sudut di Serial Monitor untuk debugging
  Serial.printf("Roll: %5.1f | Pitch: %5.1f | S1: %3d | S2: %3d | S3: %3d | S4: %3d | S5: %3d\n",
                roll, pitch, pos_servo1, pos_servo2, pos_servo3, pos_servo4, pos_servo5);

  delay(50); // Refresh rate 20Hz
}

// =========================================================================
// FUNGSI RESPONS GERAKAN EKSTERNAL (PIR MOTION ALERT)
// =========================================================================
void triggerMotionAlert() {
  Serial.println("-> Semua servo bergerak serentak ke posisi Alert (180 deg)...");
  servo1.write(ALERT_ANGLE);
  servo2.write(ALERT_ANGLE);
  servo3.write(ALERT_ANGLE);
  servo4.write(ALERT_ANGLE);
  servo5.write(ALERT_ANGLE);
  delay(1500); // Tahan posisi alert selama 1.5 detik

  Serial.println("-> Semua servo kembali serentak ke posisi semula (90 deg)...");
  resetAllServos();
  delay(1000);
}

// FUNGSI RESET SEMUA SERVO KE POSISI TEGAK LURUS (90 DERAJAT)
void resetAllServos() {
  servo1.write(INITIAL_ANGLE);
  servo2.write(INITIAL_ANGLE);
  servo3.write(INITIAL_ANGLE);
  servo4.write(INITIAL_ANGLE);
  servo5.write(INITIAL_ANGLE);
}
