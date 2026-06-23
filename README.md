# 🍃 Air Quality Monitor (Sistem Pemantau Kualitas Udara)

Sistem berbasis IoT (*Internet of Things*) menggunakan mikrokontroler ESP32 untuk memantau suhu, kelembapan, dan kadar gas di udara secara *real-time*. Data ditampilkan pada layar OLED fisik dan dikirim ke *dashboard* aplikasi smartphone melalui platform Blynk Cloud.

## 🚀 Fitur Utama
* **Pemantauan Real-time:** Membaca suhu dan kelembapan via DHT22 serta densitas gas via MQ135.
* **Indikator LED Fisik:** Lampu LED Otomatis berubah berdasarkan level polusi (Hijau = Baik, Kuning = Sedang, Merah = Buruk).
* **Notifikasi Pintar (Smart Push Notification):** Mengirimkan peringatan bahaya ke aplikasi Blynk di HP *hanya sekali* ketika polusi menyentuh zona buruk (menghindari spamming).
* **Dual Display:** Menampilkan matriks data pada layar OLED 128x64 lokal dan *Dashboard* Blynk IoT global.

---

## 🛠️ Komponen Elektronik & Hardware
1. **ESP32 Development Board** (Sebagai mikrokontroler utama)
2. **Sensor DHT22** (Sensor Suhu & Kelembapan)
3. **Sensor MQ135** (Sensor Kualitas Udara / Gas)
4. **Layar OLED SSD1306 (128x64 I2C)**
5. **3x LED** (Merah, Kuning, Hijau) beserta Resistor 220 Ohm.
6. Kabel Jumper & Breadboard.

---

## 💻 Panduan Instalasi & Cara Menjalankan

### 1. Prasyarat Software
Pastikan Anda telah menginstal **Arduino IDE** (Versi 1.8.x atau versi terbaru) pada komputer Anda.

### 2. Instalasi Library di Arduino IDE
Masuk ke menu **Tools > Manage Libraries...** di Arduino IDE, kemudian cari dan instal library berikut:
* `Blynk` (oleh Volodymyr Shymanskyy)
* `DHT sensor library` (oleh Adafruit)
* `Adafruit SSD1306` (oleh Adafruit)
* `Adafruit GFX Library` (oleh Adafruit)

### 3. Konfigurasi Kode Program
Sebelum mengunggah kode ke ESP32, sesuaikan konfigurasi jaringan Wi-Fi dan token Blynk Anda pada baris kode berikut:

```cpp
char ssid[] = "NAMA_WIFI_ANDA";
char pass[] = "PASSWORD_WIFI_ANDA";

#define BLYNK_TEMPLATE_ID "TEMPLATE_ID_ANDA"
#define BLYNK_TEMPLATE_NAME "NAMA_TEMPLATE_ANDA"
#define BLYNK_AUTH_TOKEN "AUTH_TOKEN_BLYNK_ANDA"
