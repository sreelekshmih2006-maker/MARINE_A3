/*
  MARINE-A³ — Relay / Gateway Node
  Board: Vega 3
  Communication: Bluetooth IN → Wi-Fi OUT
*/

#include <WiFi.h>

// ---------- Wi-Fi ----------
const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

// ---------- Bluetooth ----------
#define BT_RX 16
#define BT_TX 17

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, BT_RX, BT_TX);

  WiFi.begin(ssid, password);
  delay(3000);
}

void loop() {
  if (Serial2.available()) {
    String msg = Serial2.readStringUntil('\n');
    Serial.println("Received: " + msg);

    if (WiFi.status() == WL_CONNECTED) {
      WiFiClient client;
      if (client.connect("example.com", 80)) {
        client.println("POST /alert HTTP/1.1");
        client.println("Host: example.com");
        client.println("Content-Type: text/plain");
        client.println("Connection: close");
        client.println("Content-Length: " + String(msg.length()));
        client.println();
        client.print(msg);
      }
    }
  }
}
