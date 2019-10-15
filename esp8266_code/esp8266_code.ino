#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>

const char* ACCESS_POINT_NAME = "ESP8266-ACCESS-POINT";

IPAddress IP(192,168,1,1);
IPAddress GATEWAY(192,168,1,1);
IPAddress SUBNET(255,255,255,0);

WiFiUDP Udp;
unsigned int localUdpPort = 4210;
const size_t jsonSize = JSON_OBJECT_SIZE(4) + 70;
char incomingPacket[jsonSize];
DynamicJsonDocument json(jsonSize);

void setup() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  
  Serial.begin(9600);
  Serial.println();
  Serial.printf("Creating %s ", ACCESS_POINT_NAME);
  
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(IP, GATEWAY, SUBNET);
  boolean result = WiFi.softAP(ACCESS_POINT_NAME, "", 13, false, 1);
  Serial.println(result ? "Access Point is ready" : "Setting up Access Point failed");

  Udp.begin(localUdpPort);
  Serial.printf("Access Point is ready at IP %s, UDP port %d\n", WiFi.softAPIP().toString().c_str(), localUdpPort);
}

void loop() {
  int packetSize = Udp.parsePacket();
 
  if (packetSize) {
    // receive incoming UDP packets
    int len = Udp.read(incomingPacket, 255);
    if (len > 0) {
      DeserializationError error = deserializeJson(json, incomingPacket);
  
      if (!error) {
        int left_motor_speed = json["left_motor_speed"];
        int right_motor_speed = json["right_motor_speed"];
        int dir = json["direction"];
        int driving_mode = json["driving_mode"];
        Serial.printf("%d %d %d %d\n", left_motor_speed, right_motor_speed, dir, driving_mode);
      } else {
        Serial.printf("%s\n", error.c_str());
      }
    }
  }
}
