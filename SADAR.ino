#include <Wire.h>
#include <MPU6050.h>
#include <math.h>

MPU6050 mpu;

// ===== Pin =====
const int buzzer = 12;
const int buttonPin = 7;   // Tombol untuk mematikan buzzer

// ===== Konstanta =====
const float G = 9.80665f;
const float ACCEL_SENS_8G = 4096.0f;
const float GYRO_SENS_500DPS = 65.5f;
const float RAD2DEG = 57.2957795f;

// ===== Offset kalibrasi =====
float accelOffsetX = 0, accelOffsetY = 0, accelOffsetZ = 0;

// ===== Variabel buzzer =====
bool buzzerActive = false;
unsigned long buzzerStartTime = 0;

// ===== Threshold =====
float tiltThresholdX_Pos = 15;
float tiltThresholdX_Neg = -25;
float tiltThresholdY_Pos = 18;
float tiltThresholdY_Neg = -20;

// ===== SETUP =====
void setup() {
  Serial.begin(9600);
  Wire.begin();
  mpu.initialize();

  pinMode(buzzer, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);

  Serial.println("MPU6050 Initializing...");
  delay(1000);

  calibrateMPU(2000); // kalibrasi 2 detik
}

// ===== LOOP =====
void loop() {
  int16_t ax, ay, az, gx, gy, gz;
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  // ===== Konversi accelerometer ke m/s² =====
  float ax1 = ((ax - accelOffsetX) / ACCEL_SENS_8G) * G;
  float ay1 = ((ay - accelOffsetY) / ACCEL_SENS_8G) * G;
  float az1 = ((az - accelOffsetZ) / ACCEL_SENS_8G) * G;

  // ===== Konversi gyro ke °/s =====
  float gx_dps = gx / GYRO_SENS_500DPS;

  // ===== Hitung pitch =====
  float pitch = atan2(ax1, sqrt(ay1 * ay1 + az1 * az1)) * RAD2DEG;

  // ===== Debug (optional) =====
  /*
  Serial.print("X: "); Serial.print(ax1);
  Serial.print(" Y: "); Serial.print(ay1);
  Serial.print(" Pitch: "); Serial.println(pitch);
  */

  // ===== DETEKSI MIRING =====
  if (!buzzerActive &&
      (ax1 > tiltThresholdX_Pos || ax1 < tiltThresholdX_Neg ||
       ay1 > tiltThresholdY_Pos || ay1 < tiltThresholdY_Neg)) {
    Serial.println("⚠ Tilt Detected!");
    startBuzzer();
  }

  // ===== DETEKSI GERAK CEPAT (opsional) =====
  /*
  if (!buzzerActive && abs(gx_dps) > 120) {
    Serial.println("⚠ Fast Movement Detected!");
    startBuzzer();
  }
  */

  // ===== HANDLE BUZZER =====
  handleBuzzer();

  delay(100);
}

// ===== BUZZER =====
void startBuzzer() {
  buzzerActive = true;
  buzzerStartTime = millis();
  digitalWrite(buzzer, HIGH);
}

void handleBuzzer() {
  if (buzzerActive) {

    // Auto mati setelah 3 detik
    if (millis() - buzzerStartTime >= 3000) {
      buzzerActive = false;
      digitalWrite(buzzer, LOW);
    }

    // Tombol untuk mematikan
    if (digitalRead(buttonPin) == LOW) {
      buzzerActive = false;
      digitalWrite(buzzer, LOW);
      Serial.println("Buzzer stopped manually.");
    }
  }
}

// ===== KALIBRASI STABIL =====
void calibrateMPU(unsigned long duration) {
  Serial.println("Calibrating MPU... Keep sensor steady!");

  unsigned long startTime = millis();
  long axSum = 0, aySum = 0, azSum = 0;
  int count = 0;

  while (millis() - startTime < duration) {
    int16_t ax, ay, az, gx, gy, gz;
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

    axSum += ax;
    aySum += ay;
    azSum += az;
    count++;

    // Progress indicator
    if (count % 20 == 0) {
      Serial.print(".");
    }

    delay(50); // penting: hindari I2C overload
  }

  accelOffsetX = (float)axSum / count;
  accelOffsetY = (float)aySum / count;
  accelOffsetZ = (float)azSum / count;

  Serial.println("\nCalibration Done!");
  Serial.print("Offset X: "); Serial.print(accelOffsetX);
  Serial.print(" Y: "); Serial.print(accelOffsetY);
  Serial.print(" Z: "); Serial.println(accelOffsetZ);
}
