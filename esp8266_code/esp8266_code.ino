#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>

//  Access Point parameters
#define UDP_PORT 4210
const char* ACCESS_POINT_NAME = "ESP8266-ACCESS-POINT";
IPAddress IP(192,168,1,1);
IPAddress GATEWAY(192,168,1,1);
IPAddress SUBNET(255,255,255,0);

//  UDP connection object
WiFiUDP Udp;

//  Size of the incoming JSON string
const size_t jsonSize = JSON_OBJECT_SIZE(4) + 70;

//  Buffer for the incoming packet
char incomingPacket[jsonSize];

//  JSON document object
DynamicJsonDocument json(jsonSize);

void setup() {
  // Init serial connection
  Serial.begin(9600);
  Serial.println();
  Serial.printf("Creating %s \n", ACCESS_POINT_NAME);
  
  //  Reset Wi-Fi mode
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  // Setup Access Point
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(IP, GATEWAY, SUBNET);
  boolean result = WiFi.softAP(ACCESS_POINT_NAME, "");
  Serial.println(result ? "Access Point is ready" : "Setting up Access Point failed");

  //  Setup UDP
  Udp.begin(UDP_PORT);
  Serial.printf("Access Point is ready at IP %s, UDP port %d\n", WiFi.softAPIP().toString().c_str(), UDP_PORT);
}

void loop() {
  //  Get the packet size
  int packetSize = Udp.parsePacket();

  // If there was a packet available
  if (packetSize) {
    // Receive incoming UDP packet
    int len = Udp.read(incomingPacket, jsonSize);

    //  If the packet wasn't empty
    if (len > 0) {
      DeserializationError error = deserializeJson(json, incomingPacket);

      //  If the JSON was correctly deserialised
      if (!error) {
        int left_motor_speed = json["left_motor_speed"];
        int right_motor_speed = json["right_motor_speed"];
        int dir = json["direction"];
        int driving_mode = json["driving_mode"];
        Serial.printf("%d %d %d %d\n", left_motor_speed, right_motor_speed, dir, driving_mode);
      } else {
        //  Print the error
        Serial.printf("%s\n", error.c_str());
      }
    }
  }
}
