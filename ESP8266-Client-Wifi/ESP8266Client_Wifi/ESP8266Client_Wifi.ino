#include <ESP8266WiFi.h>
#include <ModbusRtu.h>
#include <SoftwareSerial.h>
// Thông tin cấu hình wifi
const char* ssid = "espserver";
const char* password = "12345678";
// Cấu hình IP và cổng kết nối server
IPAddress localIP(192, 168, 4, 1); // Địa chỉ IP của AP
IPAddress subnet(255, 255, 255, 0); // Subnet mask của AP
const uint16_t serverPort = 1234; // Cổng kết nối của AP
WiFiServer server(serverPort); // Khởi tạo server
#define RS485_PIN_DIR 13
#define ledon 12
#define ledoff 14
SoftwareSerial SerialPLC(5,0);
uint16_t au16data[16];
uint8_t u8state;
uint8_t u8query;
Modbus master(0, SerialPLC, RS485_PIN_DIR);
modbus_t telegram[1];
unsigned long u32wait;
uint16_t outputs[1];
void initModbus()
{
  telegram[0].u8id = 1;         
  telegram[0].u8fct = 5;            
  telegram[0].u16RegAdd = 0;     
  telegram[0].u16CoilsNo = 1;       
  telegram[0].au16reg = outputs;      

  SerialPLC.begin(38400);
  master.start();
  master.setTimeOut(500);
  u32wait = millis()+100;
  u8state = u8query = 0;
}
void setup() {
  pinMode(ledon,OUTPUT);
  pinMode(ledoff,OUTPUT);
  Serial.begin(9600);
  delay(1000);
  // Khởi động Access Point
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(localIP, localIP, subnet);
  Serial.println("AP started");
  // Khởi động server
  server.begin();
  Serial.println("Server started");
  initModbus();
}
void loop() {
  WiFiClient client = server.available();
  if (client) {
    Serial.println("New client connected");
    while (client.connected()) {
      if (client.available()) {
        switch (u8state)
        {
        case 0:
          if (millis() > u32wait)
              u8state++;
          break;
        case 1:
          master.query(telegram[u8query]);
          u8state++;
          u8query++;
          if (u8query > 3)
              u8query = 0;
          break;
        case 2:
          master.poll();
          if (master.getState() == COM_IDLE)
          {
              u8state = 1;
              u32wait = millis()+100;
          }
          break;
        }
        //Nhan yeu cau tu client
        String request = client.readStringUntil('\r');
        Serial.println("Received request: " + request);
        if(request == "HMIPLCon"){
          outputs[0]=1;
          digitalWrite(ledon,HIGH);
          digitalWrite(ledoff,LOW);
        }
        else if(request == "HMIPLCoff"){
          outputs[0]=0;
          digitalWrite(ledon,LOW);
          digitalWrite(ledoff,HIGH);
        }
      }
    }
    Serial.println("Client disconnected");
  }
}