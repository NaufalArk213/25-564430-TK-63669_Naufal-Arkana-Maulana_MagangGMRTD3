# Penjelasan Solusi Tugas Magang GMRT (Day 3)

Berikut ini adalah penjelasan singkat mengenai pembahasan cara kerja dari program yang telah dibuat untuk menyelesaikan Tugas Day 3.

Tugas Following-Axis Servo Motor & Motion Detection ESP32
Pada tugas ini, program C++ Arduino Framework digunakan untuk mengontrol sistem 5 motor servo berbasis pembacaan sensor akselerometer/gyroscope MPU6050 dan sensor gerak PIR pada ESP32.

Logika penyelesaian:
- Pemrosesan data sensor MPU6050 dilakukan secara berkala untuk membaca nilai akselerasi (`acceleration`) dan kecepatan sudut gyroscope (`gyro`).
- Sudut kemiringan **Roll** dihitung menggunakan `atan2(accel.acceleration.y, accel.acceleration.z)` untuk menggerakkan Servo 1 & Servo 2 **melawan arah kemiringan** (`INITIAL_ANGLE - roll`).
- Sudut kemiringan **Pitch** dihitung menggunakan `atan2(-accel.acceleration.x, sqrt(...))` untuk menggerakkan Servo 3 & Servo 4 **searah kemiringan** (`INITIAL_ANGLE + pitch`).
- Kecepatan rotasi **Yaw** memicu Servo 5 berputar $\pm 45^\circ$, menahan posisi selama 1 detik menggunakan penundaan non-blocking `millis()`, kemudian otomatis reset ke posisi awal $90^\circ$.
- Apabila PIR Sensor mendeteksi pergerakan eksternal (`digitalRead(PIN_PIR_SENSOR) == HIGH`), seluruh servo secara serentak berputar ke sudut alert $180^\circ$ selama 1.5 detik sebelum kembali ke posisi awal $90^\circ$.


Link Simulasi Wokwi & Rekaman Video (Day 3)

- **Link Simulasi Wokwi Online**: [https://wokwi.com/projects/474785487673409537]
- **Link Video Rekaman GDrive (.webm)**: [https://drive.google.com/file/d/15-97bK_w-NjBFwQ6r-JXdMVBJ9ZfORFC/view?usp=sharing]

