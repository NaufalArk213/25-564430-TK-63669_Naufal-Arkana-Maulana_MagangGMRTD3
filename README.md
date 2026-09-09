# Penjelasan Solusi Tugas Magang GMRT (Day 2)

Berikut ini adalah penjelasan singkat mengenai pembahasan cara kerja dari program yang telah dibuat untuk menyelesaikan Tugas Computer Vision dan Tugas ROS 2.

### Tugas Computer Vision: Deteksi 3 Objek Real-Time
Pada tugas ini, program digunakan untuk mendeteksi 3 objek secara real-time dari tampilan kamera webcam menggunakan model YOLOv8n dan library OpenCV.

Logika penyelesaian:
- Pemrosesan video dari webcam dilakukan per frame menggunakan `cv.VideoCapture(0)`.
- Deteksi dibatasi secara khusus hanya pada 3 kelas objek COCO yaitu orang (`0`), botol (`39`), dan mouse (`64`) dengan mengatur filter `classes=[0, 39, 64]`.
- Gambar hasil anotasi (Bounding Box, Nama Objek, dan Confidence Score) ditampilkan secara live ke jendela layar menggunakan `results[0].plot()` sampai pengguna menekan tombol `q` untuk keluar.

### Tugas ROS 2: Otomatisasi Lintasan TurtleSim 
Pada tugas ini, program mengendalikan gerakan kura-kura simulator TurtleSim secara otomatis untuk membentuk lintasan berbentuk persegi 4 sisi tanpa kontrol manual.

Logika penyelesaian:
- Node ROS 2 membuat publisher ke topik `/turtle1/cmd_vel` menggunakan pesan `geometry_msgs/msg/Twist`.
- Pergerakan dibagi secara modular menjadi fungsi gerak lurus (`move_forward`) dan fungsi rotasi 90 derajat (`turn_90_degrees`).
- Proses diulang sebanyak 4 kali untuk membentuk 4 sisi persegi secara presisi, lalu robot otomatis berhenti di posisi akhir setelah selesai.
