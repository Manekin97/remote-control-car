#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

//const char* ssid = "TP-Link_2.4";
//const char* password = "1997rafal1997";
const char* ssid = "mordor";
const char* password = "adminadmin";

WiFiUDP Udp;
unsigned int localUdpPort = 4210;
char incomingPacket[256];
char replyPacket[] = "ok.";

void setup() {
  Serial.begin(9600);
  Serial.println();

  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println(" connected");

  Udp.begin(localUdpPort);
  Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), localUdpPort);
}

void loop() {
 int packetSize = Udp.parsePacket();
 
  if (packetSize) {
    // receive incoming UDP packets
    int len = Udp.read(incomingPacket, 255);
    if (len > 0) {
      incomingPacket[len] = '\0';

      Serial.printf("%d", incomingPacket[0]);
    }

    // send back a reply, to the IP address and port we got the packet from
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write(replyPacket);
    Udp.endPacket();
  }
  else {
//      Serial.printf("%d\n", -1);
    }
  

}
