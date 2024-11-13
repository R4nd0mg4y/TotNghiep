#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>  // Thư viện Firebase ESP8266 Client
#include <Wire.h>

#define Buzzer D6
Adafruit_MPU6050 mpu;

// WiFi credentials
#define WIFI_SSID "Wifi"
#define WIFI_PASSWORD "12345678"

// Firebase configuration
FirebaseData firebaseData;
FirebaseAuth auth;
FirebaseConfig config;

bool setOn;

void wifiConnect() {

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);  // Kết nối Wi-Fi với thông tin từ Firebase
  Serial.print("Đang kết nối tới ");

  Serial.println(" ...");

  int teller = 0;
  while (WiFi.status() != WL_CONNECTED) {  // Chờ kết nối thành công
    delay(1000);
    Serial.print(++teller);
    Serial.print(' ');
  }

  Serial.println('\n');
  Serial.println("Kết nối thành công!");
  Serial.print("Địa chỉ IP: ");
  Serial.println(WiFi.localIP());  // In địa chỉ IP của ESP8266
}


void setup(void) {
  Serial.begin(115200);
  pinMode(Buzzer, OUTPUT);

  // WiFi connection
  wifiConnect();

  // Firebase configuration
  config.host = "iotproject-5c153-default-rtdb.firebaseio.com";  // Firebase host (without "https://")
  config.signer.tokens.legacy_token = "xVQ2KNP72m8P3mwFa77Utk4LBb8YXttOcc8HlQta";  // Firebase Database secret

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);  // Auto reconnect WiFi

  // MPU6050 initialization
  while (!Serial)
    delay(10);

  Serial.println("Adafruit MPU6050 test!");

  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange()) {
    case MPU6050_RANGE_2_G:
      Serial.println("+-2G");
      break;
    case MPU6050_RANGE_4_G:
      Serial.println("+-4G");
      break;
    case MPU6050_RANGE_8_G:
      Serial.println("+-8G");
      break;
    case MPU6050_RANGE_16_G:
      Serial.println("+-16G");
      break;
  }

  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  Serial.print("Gyro range set to: ");
  switch (mpu.getGyroRange()) {
    case MPU6050_RANGE_250_DEG:
      Serial.println("+- 250 deg/s");
      break;
    case MPU6050_RANGE_500_DEG:
      Serial.println("+- 500 deg/s");
      break;
    case MPU6050_RANGE_1000_DEG:
      Serial.println("+- 1000 deg/s");
      break;
    case MPU6050_RANGE_2000_DEG:
      Serial.println("+- 2000 deg/s");
      break;
  }

  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth()) {
    case MPU6050_BAND_260_HZ:
      Serial.println("260 Hz");
      break;
    case MPU6050_BAND_184_HZ:
      Serial.println("184 Hz");
      break;
    case MPU6050_BAND_94_HZ:
      Serial.println("94 Hz");
      break;
    case MPU6050_BAND_44_HZ:
      Serial.println("44 Hz");
      break;
    case MPU6050_BAND_21_HZ:
      Serial.println("21 Hz");
      break;
    case MPU6050_BAND_10_HZ:
      Serial.println("10 Hz");
      break;
    case MPU6050_BAND_5_HZ:
      Serial.println("5 Hz");
      break;
  }

  Serial.println("");
  delay(100);
}

void Buzzer_Init() {
  digitalWrite(Buzzer, HIGH);
  delay(500);
  digitalWrite(Buzzer, LOW);
  delay(500);
}

void loop() {
  /* Get new sensor events with the readings */
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  /* Print out the values */
  Serial.print("Acceleration X: ");
  Serial.print(a.acceleration.x);
  Serial.print(", Y: ");
  Serial.print(a.acceleration.y);
  Serial.print(", Z: ");
  Serial.print(a.acceleration.z);
  Serial.println(" m/s^2");

  Serial.print("Rotation X: ");
  Serial.print(g.gyro.x);
  Serial.print(", Y: ");
  Serial.print(g.gyro.y);
  Serial.print(", Z: ");
  Serial.print(g.gyro.z);
  Serial.println(" rad/s");

  Serial.print("Temperature: ");
  Serial.print(temp.temperature);
  Serial.println(" degC");

  // Push data to Firebase
  if (Firebase.setFloat(firebaseData, "/accelerationY", a.acceleration.y)) {
    Serial.println("Data sent to Firebase successfully");
  } else {
    Serial.println("Failed to send data to Firebase");
    Serial.println(firebaseData.errorReason());
  }

  if(Firebase.getBool(firebaseData, "/setOn")){
    if (firebaseData.dataType() == "boolean") {
      setOn = firebaseData.boolData();
      Serial.print("Set On: ");
      Serial.println(setOn); 
  }}

  if (setOn==1&&a.acceleration.y >= -8.8) {
    Serial.println("Straighten your back!");
    Buzzer_Init();     
  }

  Serial.println("");
  delay(500);
}
