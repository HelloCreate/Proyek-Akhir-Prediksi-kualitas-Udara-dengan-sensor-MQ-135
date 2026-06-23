/*
 * Proyek: Air Quality Monitor (Sistem Pemantau Kualitas Udara)
 * Deskripsi: Membaca suhu/kelembapan (DHT22) dan kadar gas (MQ135),
 * menampilkannya di OLED, mengirim data ke Blynk IoT, 
 * serta mengontrol indikator LED & notifikasi HP.
 */

// Konfigurasi Blynk Cloud (Pastikan Token dan ID sesuai dengan akun Blynk Anda)
#define BLYNK_TEMPLATE_ID "TMPL6xEJdWMDV"
#define BLYNK_TEMPLATE_NAME "AIR QUALITY MONITOR"
#define BLYNK_AUTH_TOKEN "8ip--7RAfh0kDRjEmAeV9bAs9COKBgeT"

// Import Library yang Dibutuhkan
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Konfigurasi Jaringan Wi-Fi
char ssid[] = "FAREL";
char pass[] = "77777777";

// Definisikan Pin Sensor
#define DHTPIN 4       // Pin Data Sensor DHT22 disambungkan ke GPIO 4
#define DHTTYPE DHT22  // Jenis sensor yang digunakan adalah DHT22
#define MQ135 34       // Pin Analog Sensor MQ135 disambungkan ke GPIO 34 (ADC)

// Definisikan Pin Indikator LED
#define LED_HIJAU 25   // LED Hijau (Udara Baik) di GPIO 25
#define LED_KUNING 26  // LED Kuning (Udara Sedang) di GPIO 26
#define LED_MERAH 27   // LED Merah (Udara Buruk) di GPIO 27

// Inisialisasi Sensor DHT22
DHT dht(DHTPIN, DHTTYPE);

// Konfigurasi Dimensi Layar OLED SSD1306 (128x64 Piksel)
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Inisialisasi Objek Display OLED dengan Komunikasi I2C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Objek Timer untuk mengatur interval pengiriman data ke Blynk tanpa memblokir loop
BlynkTimer timer;

// Variabel flag agar notifikasi ke HP tidak terkirim terus-menerus saat kondisi buruk
bool notifTerkirim = false;

/*
 * Fungsi: kirimData
 * Deskripsi: Mengambil data sensor, memproses logika kualitas udara,
 * mengontrol LED, mengupdate OLED, dan mengirim data ke Blynk.
 */
void kirimData() {
  // Membaca nilai suhu dan kelembapan dari sensor DHT22
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  // Membaca nilai analog dari sensor gas MQ135
  int gas = analogRead(MQ135);

  String statusUdara;

  // Validasi: Cek apakah sensor DHT22 merespon dengan benar
  if (isnan(t) || isnan(h)) {
    Serial.println("Gagal membaca DHT22!");
    return;
  }

  // --- Logika Klasifikasi Kualitas Udara & Kontrol LED ---
  
  // Kondisi 1: Kualitas Udara Baik (Nilai gas di bawah 1550)
  if (gas < 1550) {
    statusUdara = "Baik";
    digitalWrite(LED_HIJAU, HIGH);
    digitalWrite(LED_KUNING, LOW);
    digitalWrite(LED_MERAH, LOW);
    notifTerkirim = false; // Reset flag notifikasi jika udara kembali normal
  }
  // Kondisi 2: Kualitas Udara Sedang (Nilai gas antara 1550 - 1999)
  else if (gas < 2000) {
    statusUdara = "Sedang";
    digitalWrite(LED_HIJAU, LOW);
    digitalWrite(LED_KUNING, HIGH);
    digitalWrite(LED_MERAH, LOW);
    notifTerkirim = false; // Reset flag notifikasi jika udara membaik
  }
  // Kondisi 3: Kualitas Udara Buruk (Nilai gas 2000 ke atas)
  else {
    statusUdara = "Buruk";
    digitalWrite(LED_HIJAU, LOW);
    digitalWrite(LED_KUNING, LOW);
    digitalWrite(LED_MERAH, HIGH);

    // Kirim notifikasi Blynk logEvent ke Smartphone (Hanya dikirim SEKALI saat pertama kali buruk)
    if (!notifTerkirim) {
      Blynk.logEvent(
        "air_quality_monitor",
        String("⚠️ Kualitas udara buruk terdeteksi!\n") +
        "Gas: " + String(gas) +
        "\nSuhu: " + String(t, 1) + " C" +
        "\nKelembapan: " + String(h, 1) + " %"
      );
      Serial.println("NOTIFIKASI TERKIRIM KE HP");
      notifTerkirim = true; // Kunci flag agar tidak spamming notifikasi
    }
  }

  // --- Pengiriman Data ke Blynk Virtual Pin ---
  Blynk.virtualWrite(V0, gas);
  Blynk.virtualWrite(V1, t);
  Blynk.virtualWrite(V2, h);
  Blynk.virtualWrite(V3, statusUdara);

  // --- Pembaruan Tampilan Layar OLED ---
  display.clearDisplay();       // Bersihkan layar
  display.setTextSize(1);       // Set ukuran font ke standard
  display.setTextColor(WHITE);  // Set warna font putih

  // Tampilkan Informasi Suhu
  display.setCursor(0, 0);
  display.print("Temp : "); display.print(t); display.print(" C");

  // Tampilkan Informasi Kelembapan
  display.setCursor(0, 15);
  display.print("Hum  : "); display.print(h); display.print(" %");

  // Tampilkan Nilai Gas
  display.setCursor(0, 30);
  display.print("Gas  : "); display.print(gas);

  // Tampilkan Status Kualitas Udara
  display.setCursor(0, 45);
  display.print("Status: "); display.print(statusUdara);

  display.display(); // Terapkan perubahan ke layar OLED

  // --- Output Debugging via Serial Monitor ---
  Serial.print("Suhu: "); Serial.print(t);
  Serial.print(" C | Kelembapan: "); Serial.print(h);
  Serial.print(" % | Gas: "); Serial.print(gas);
  Serial.print(" | Status: "); Serial.println(statusUdara);
}

void setup() {
  // Inisialisasi Komunikasi Serial dengan Baud Rate 115200
  Serial.begin(115200);

  // Mengaktifkan Sensor DHT22
  dht.begin();

  // Konfigurasi Pin LED sebagai Output
  pinMode(LED_HIJAU, OUTPUT);
  pinMode(LED_KUNING, OUTPUT);
  pinMode(LED_MERAH, OUTPUT);

  // Matikan semua LED di awal sistem menyala
  digitalWrite(LED_HIJAU, LOW);
  digitalWrite(LED_KUNING, LOW);
  digitalWrite(LED_MERAH, LOW);

  // Inisialisasi Layar OLED (Alamat I2C umum: 0x3C)
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED tidak terdeteksi!");
    while (true); // Kunci sistem jika hardware OLED bermasalah
  }

  display.clearDisplay();
  display.display();

  // Menghubungkan ESP32 ke Jaringan Wi-Fi dan Server Blynk IoT
  Serial.println("Menghubungkan ke WiFi...");
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // Mengatur Interval Timer: Menjalankan fungsi kirimData setiap 3000ms (3 detik)
  timer.setInterval(3000L, kirimData);
}

void loop() {
  Blynk.run(); // Menjaga koneksi dan memproses sinkronisasi data Blynk Cloud
  timer.run(); // Menjalankan penjadwalan fungsi dari BlynkTimer
}
