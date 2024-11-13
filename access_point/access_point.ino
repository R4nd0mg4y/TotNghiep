#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <Wire.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#define Buzzer D6
Adafruit_MPU6050 mpu;
ESP8266WebServer server(80);

// Firebase configuration
FirebaseData firebaseData;
FirebaseAuth auth;
FirebaseConfig config;
bool setOn;
String EEPROMemail="";
// Variables for storing Wi-Fi credentials
String ssid = "";
String password = "";
String email = "";

// Wi-Fi AP for initial configuration
void setupAccessPoint() {
  WiFi.softAP("ESP8266_Config");

  // Web interface for Wi-Fi configuration
  server.on("/", []() {
    String html = "<html><body><h1>Wi-Fi Configuration</h1>"
                  "<form action='/connect' method='POST'>"
                  "SSID: <input type='text' name='ssid'><br>"
                  "Password: <input type='password' name='password'><br>"
                  "Email: <input type='email' name='email'><br>"
                  "<input type='submit' value='Submit'></form></body></html>";
    server.send(200, "text/html", html);
  });

  server.on("/connect", HTTP_POST, []() {
    ssid = server.arg("ssid");
    password = server.arg("password");
    email = server.arg("email");
    email.replace(".", ""); 
    writeStringToEEPROM(0,email);
    

    WiFi.begin(ssid.c_str(), password.c_str());

    server.send(200, "text/html", "<html><body><h1>Connecting...</h1></body></html>");

    // int attempts = 0;
    // while (attempts < 20) { 
    //   delay(1000);
    //   attempts++;
    //   if (WiFi.status() == WL_CONNECTED) {
    //     Serial.println("Connected!");
    //     Serial.print("IP Address: ");
    //     Serial.println(WiFi.localIP());
    //     WiFi.softAPdisconnect(true);  
    //     server.send(200, "text/html", "<html><body><h1>Connected!</h1></body></html>");
    //     break;
    //   }
    // }
    
    // if (WiFi.status() != WL_CONNECTED) {
    //   Serial.println("Failed to connect.");
    //   server.send(200, "text/html", "<html><body><h1>Failed to connect.</h1></body></html>");
    // }
  });

  server.begin();
}


void handleMPU6050() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  Serial.print("Acceleration Y: ");
  Serial.println(a.acceleration.y);
  Serial.println(a.acceleration.x);
   Serial.println(a.acceleration.z);


  if (Firebase.setFloat(firebaseData, "devices/"+EEPROMemail+"/acceleration/accelerationY", a.acceleration.y)) {
    Serial.println("Data sent to Firebase successfully");
  } else {
    Serial.println("Failed to send data to Firebase");
    Serial.println(firebaseData.errorReason());
  }
   if (Firebase.setFloat(firebaseData, "devices/"+EEPROMemail+"/acceleration/accelerationX", a.acceleration.x)) {
    Serial.println("Data sent to Firebase successfully");
  } else {
    Serial.println("Failed to send data to Firebase");
    Serial.println(firebaseData.errorReason());
  }
  if (Firebase.setFloat(firebaseData, "devices/"+EEPROMemail+"/acceleration/accelerationZ", a.acceleration.z)) {
    Serial.println("Data sent to Firebase successfully");
  } else {
    Serial.println("Failed to send data to Firebase");
    Serial.println(firebaseData.errorReason());
  }



  if (Firebase.getBool(firebaseData, "devices/"+EEPROMemail+"/setOn")) {
    if (firebaseData.dataType() == "boolean") {
      setOn = firebaseData.boolData();
      Serial.print("Set On: ");
      Serial.println(setOn); 
    }
  }

  
  if (setOn == 1 && a.acceleration.y >= -8.8 && a.acceleration.z >0) {
    if (Firebase.getFloat(firebaseData, "devices/"+EEPROMemail+"/volume")) {
            int volume = (int)((firebaseData.floatData() * 255) / 100);
            analogWrite(Buzzer, volume); 
            Serial.println(volume);    
    }
    delay(500); 
    digitalWrite(Buzzer, LOW); 
}

}

// Wi-Fi connection function
// void wifiConnect() {
//   Serial.println("Connecting to Wi-Fi...");
//   while (WiFi.status() != WL_CONNECTED) {
//     delay(1000);
//     Serial.print(".");
//   }
//   Serial.println("Connected!");
//   Serial.print("IP Address: ");
//   Serial.println(WiFi.localIP());
// }
void writeStringToEEPROM(int addrOffset, const String &strToWrite)
{
  byte len = strToWrite.length();
  EEPROM.write(addrOffset, len);
  for (int i = 0; i < len; i++)
  {
    EEPROM.write(addrOffset + 1 + i, strToWrite[i]);
  }
   EEPROM.commit();
}

String readStringFromEEPROM(int addrOffset)
{
  int newStrLen = EEPROM.read(addrOffset);
  char data[newStrLen + 1];
  for (int i = 0; i < newStrLen; i++)
  {
    data[i] = EEPROM.read(addrOffset + 1 + i);
  }
  data[newStrLen] = '\0'; // !!! NOTE !!! Remove the space between the slash "/" and "0" (I've added a space because otherwise there is a display bug)
  return String(data);
}

void setup(void) {
  Serial.begin(115200);
  EEPROM.begin(512);
  pinMode(Buzzer, OUTPUT);

  // Optional: Clear any stored Wi-Fi credentials
  // WiFi.disconnect(true);

  setupAccessPoint();

  EEPROMemail = readStringFromEEPROM(0);
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");


  config.host = "iotproject-5c153-default-rtdb.firebaseio.com";
  config.signer.tokens.legacy_token = "xVQ2KNP72m8P3mwFa77Utk4LBb8YXttOcc8HlQta";
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
 
}

void loop() {
  server.handleClient();
  
  if (WiFi.status() == WL_CONNECTED) {
    handleMPU6050();
  }

  delay(500);
}
