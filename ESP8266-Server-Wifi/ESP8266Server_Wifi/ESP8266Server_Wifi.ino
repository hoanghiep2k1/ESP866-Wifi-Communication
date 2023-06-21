#include <ESP8266WiFi.h>
#define controlPLC 16
#define ledoff 4
#define ledon 5
// Thông tin cấu hình wifi
const char* ssid = "espserver";
const char* password = "12345678";
// Cấu hình IP và cổng kết nối server
IPAddress serverIP(192, 168, 4, 1); // Địa chỉ IP của AP
const uint16_t serverPort = 1234; // Cổng kết nối của AP
WiFiClient client; // Khởi tạo client
void setup() {
  pinMode(controlPLC, INPUT);
  pinMode(ledoff,OUTPUT);
  pinMode(ledon,OUTPUT);
  Serial.begin(9600);
  delay(1000);
  // Kết nối tới Access Point
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to AP...");
  }
  Serial.println("Connected to AP");
  // Kết nối tới AP
  Serial.println("Connecting to AP server...");
  while (!client.connect(serverIP, serverPort)) {
    delay(1000);
    Serial.println("Connection failed, retrying...");
  }
  Serial.println("Connected to AP server");
}
void loop() {
  // Kiểm tra kết nối
  if (!client.connected()) {
    Serial.println("Connection lost. Reconnecting...");
    while (!client.connect(serverIP, serverPort)) {
      Serial.println("Connection failed. Retrying...");
      delay(5000);
    }
    Serial.println("Connection re-established");
  }
  //Gửi yêu cầu tới AP
  if(digitalRead(controlPLC) == HIGH){
    client.print("HMIPLCon\r");
    digitalWrite(ledon,HIGH);
    digitalWrite(ledoff,LOW);
    delay(1000);
  }
  else if(digitalRead(controlPLC) == LOW){
    client.print("HMIPLCoff\r");
    digitalWrite(ledon,LOW);
    digitalWrite(ledoff,HIGH);
    delay(1000);
  }
}
