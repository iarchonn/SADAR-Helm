#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;

// Define LED pins
const int redLED = 13;
// const int greenLED = 12;
// const int blueLED = 11;

const float G = 9.80665f;
const float ACCEL_SENS_8G = 4096.0f;     // LSB/g
const float GYRO_SENS_500DPS = 65.5f;    // LSB/(°/s)
const float DEG2RAD = 0.017453292519943295f;

// Offset hasil kalibrasi
float accelOffsetX = 0, accelOffsetY = 0, accelOffsetZ = 0;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  mpu.initialize();

  // pinMode(redLED, OUTPUT);
  // pinMode(greenLED, OUTPUT);
  // pinMode(blueLED, OUTPUT);

  Serial.println("MPU6050 and LEDs initialized.");

  // Lakukan kalibrasi awal
  calibrateMPU(5000);
}

void loop() {
  int16_t ax, ay, az, gx, gy, gz;
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  // Kurangi offset agar pembacaan lebih presisi
  float ax1 = ((ax - accelOffsetX) / ACCEL_SENS_8G) * G;
  float ay1 = ((ay - accelOffsetY) / ACCEL_SENS_8G) * G;
  float az1 = ((az - accelOffsetZ) / ACCEL_SENS_8G) * G;

  Serial.print("Accel -> ");
  Serial.print("X: "); Serial.print(ax1);
  Serial.print(" | Y: "); Serial.print(ay1);
  Serial.print(" | Z: "); Serial.print(az1);
  Serial.print(" Gyro -> ");
  Serial.print("gX: "); Serial.print(gx);
  Serial.print(" | gY: "); Serial.print(gy);
  Serial.print(" | gZ: "); Serial.print(gz);
  Serial.println();


  // // Reset all LEDs
  digitalWrite(redLED, LOW);
  // digitalWrite(greenLED, LOW);
  // digitalWrite(blueLED, LOW);

  // // Respond to tilt based on X and Y
  // if (ax1 < -5) {
  //   digitalWrite(redLED, HIGH);} // Tilt left
  // } else if (ax1 > 5) {
  //   digitalWrite(greenLED, HIGH); // Tilt right
  // }

  // if (ay1 > 5 || ay1 < -5) {
  //   digitalWrite(blueLED, HIGH); // Forward or backward tilt
  // }

  delay(500);
}

// Fungsi kalibrasi: menghitung rata-rata selama 5 detik
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

    // LED merah berkedip selama kalibrasi
    digitalWrite(redLED, HIGH);
    delay(100);
    digitalWrite(redLED, LOW);
    delay(100);
  }

  accelOffsetX = (float)axSum / count;
  accelOffsetY = (float)aySum / count;
  accelOffsetZ = (float)azSum / count;

  Serial.println("Calibration Done!");
  Serial.print("Accel offsets: ");
  Serial.print(accelOffsetX); Serial.print(", ");
  Serial.print(accelOffsetY); Serial.print(", ");
  Serial.println(accelOffsetZ);
}
