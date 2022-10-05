/* AgSoftAp - WiFi Access Point & GNSS UDP Bridge
 * for the AgOpenGPS application.
 *
 * Charlie Price 2022 
*/
//#define SERIAL_DEBUG
#include "BluetoothSerial.h"

/* Check if Bluetooth configurations are enabled in the SDK */
/* If not, then you have to recompile the SDK */
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;

#include <EEPROM.h>//https://github.com/espressif/arduino-esp32/tree/master/libraries/EEPROM
#include <WiFi.h>
#include <WiFiUdp.h>
#include <WebServer.h>

//Constants
#define EEPROM_SIZE 12

#define net 4
const char *ssid = "RoverNet";
const char *passphrase = "74777477";

IPAddress local_IP(192,168,4,1);      //IP of the SoftAP
IPAddress gateway(192,168,4,1);       //
IPAddress subnet(255,255,255,0);      //

IPAddress udpRemoteIp(192,168,4,255); //broadcast address for GNSS packets received on Serial2
int       udpRemotePort=9999;         //
int       udpLocalPort=7777;          //

char packetBuffer[255]; //buffer to hold outgoing packet
int packetPtr;
char c;
WiFiUDP Udp;

WebServer server(80);

//UART2, Serial2 pins for the GNSS UDP Bridge
#define RXD2 16
#define TXD2 17
#define SERIAL2_BAUD 115200

boolean blink;
int LED_BUILTIN = 2;

void WiFiEvent(WiFiEvent_t event) {
  Serial.printf("[WiFi-event] event: %d\n", event);
  if (event == ARDUINO_EVENT_WIFI_AP_STAIPASSIGNED) {
    Serial.print("Obtained IP address: ");
    Serial.println(WiFi.localIP());
  } else if (event == ARDUINO_EVENT_WIFI_AP_STACONNECTED) {
    Serial.print("station connected to AP");
  }
}
/*
void WiFiEvent(WiFiEvent_t event, system_event_info_t info) {
  Serial.printf("[WiFi-event] event: %d\n", event);
  if (event == SYSTEM_EVENT_AP_STAIPASSIGNED) {
    Serial.print("Obtained IP address: "); // Gotta do some printf
    Serial.print(ip4_addr1(&(info.got_ip.ip_info.ip)));
    Serial.print(".");  
    Serial.print(ip4_addr2(&(info.got_ip.ip_info.ip)));
    Serial.print(".");  
    Serial.print(ip4_addr3(&(info.got_ip.ip_info.ip)));
    Serial.print(".");  
    Serial.print(ip4_addr4(&(info.got_ip.ip_info.ip)));
  }
}
*/

void setup() {
 Serial2.begin(SERIAL2_BAUD, SERIAL_8N1, RXD2, TXD2);
 Serial.println("Serial 2 started.");
 
  #if defined(SERIAL_DEBUG)
 Serial.begin(115200);
 //while(!Serial) {
     
 //}
 delay(2000);
 #endif 
 
 Serial.println();
 
 
 WiFi.mode(WIFI_AP);
 
 Serial.print("Setting soft-AP configuration ... ");
 if (WiFi.softAPConfig(local_IP, gateway, subnet)) {
   Serial.println("Ready");
 } else {
   Serial.println("Failed!");  
 }
 
 delay(2000);
 
 Serial.print("Setting soft-AP ... ");
 if (WiFi.softAP(ssid,passphrase)) {
   Serial.println("Ready");
 } else {
   Serial.println("Failed!");  
 }
 delay(2000);
 
 WiFi.onEvent(WiFiEvent);
 server.begin();
 
 //WiFi.begin();
 
 //WiFi.softAP(ssid);
 //WiFi.softAP(ssid, passphrase, channel, ssdi_hidden, max_connection)
 
 Serial.print("AgSoftAp IP address = ");
 Serial.println(WiFi.softAPIP());
 
 packetPtr = 0;
 //Udp.begin(local_IP, udpLocalPort);

 SerialBT.begin("SoftAP");
 Serial.println("Bluetooth Started! Ready to pair...");

 pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
    
 int packetSize = Udp.parsePacket();
  if (packetSize) {
    Serial.print("Received packet of size ");
    Serial.println(packetSize);
    Serial.print("From ");
    IPAddress remoteIp = Udp.remoteIP();
    Serial.print(remoteIp);
    Serial.print(", port ");
    Serial.println(Udp.remotePort());
    // read the packet into packetBufffer
    int len = Udp.read(packetBuffer, 255);
    if (len > 0) {
      packetBuffer[len] = 0;
    }
    Serial.println("Contents:");
    Serial.println(packetBuffer);
    // send a reply, to the IP address and port that sent us the packet we received
    //Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    //Udp.write(ReplyBuffer);
    //Udp.endPacket();
  }
  
 if (Serial2.available()) {
    c = Serial2.read();
    packetBuffer[packetPtr++] = c;
    //Serial.print(c);
    //if (c == 10)
    //  packetPtr=0;
 }
 
 if (c == 10) { 
    Udp.beginPacket(udpRemoteIp,udpRemotePort);
    int bytesWritten = Udp.write((byte*)packetBuffer,packetPtr);
    //Serial.print("bytes sent "); Serial.print(bytesWritten); Serial.print(" of "); Serial.println(packetPtr);
    int endCode = Udp.endPacket(); 
    //Serial.println(endCode);
    
    packetBuffer[packetPtr++] = 0;
    Serial.print(packetBuffer);
    
    c=0;
    packetPtr=0;
    memset(packetBuffer, 0, sizeof packetBuffer);     
 }

// if (blink) {
   digitalWrite(LED_BUILTIN, HIGH);
   blink = false;
// } else {
//   digitalWrite(LED_BUILTIN, LOW);
//   blink = true;
// }

 
}
