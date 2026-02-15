#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

// ================= OLED =================
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// ================= DHT ==================
#define DHTTYPE DHT11
DHT dht(DHT_PIN, DHTTYPE);

// ================= TIMING =================
unsigned long lastHeartbeat = 0;
unsigned long lastMotion = 0;
unsigned long lastStatusUpdate = 0;

const unsigned long HEARTBEAT_INTERVAL = 30000;  // 30 sec
const unsigned long MOTION_TIMEOUT = 60000;       // 1 min
const unsigned long STATUS_INTERVAL = 2000;

// ================= STATE =================
bool emergencyActive = false;
String emergencyReason = "";

// ================= FUNCTIONS =================

long readUltrasonic() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  if (duration == 0) return -1;

  return duration * 0.034 / 2;
}

void buzzAlert(int times) {
  for (int i = 0; i < times; i++) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(200);
    digitalWrite(BUZZER_PIN, LOW);
    delay(200);
  }
}

void showDisplay(String line1, String line2, String line3) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("MARINE-A3");
  display.println(line1);
  display.println(line2);
  display.println(line3);
  display.display();
}

void triggerEmergency(String reason) {
  if (emergencyActive) return;

  emergencyActive = true;
  emergencyReason = reason;

  buzzAlert(5);

  Serial.println("EMERGENCY: " + reason);
  Serial2.println("SOS|" + reason);

  showDisplay("EMERGENCY!", reason, "SOS SENT");
}

void sendHeartbeat() {
  Serial2.println("HEARTBEAT|OK");
  Serial.println("Heartbeat sent");
}

// ================= SETUP =================

void setup() {
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(PIR_PIN, INPUT);
  pinMode(SOS_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, BT_RX, BT_TX);

  Wire.begin(SDA_PIN, SCL_PIN);

  dht.begin();

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  showDisplay("System Booting", "Please Wait", "");

  lastMotion = millis();
  lastHeartbeat = millis();
}

// ================= LOOP =================

void loop() {

  // ---------- PIR: Human presence ----------
  if (digitalRead(PIR_PIN) == HIGH) {
    lastMotion = millis();
  }

  if (!emergencyActive && millis() - lastMotion > MOTION_TIMEOUT) {
    triggerEmergency("NO MOTION");
  }

  // ---------- Ultrasonic: Boat stability ----------
  long distance = readUltrasonic();
  if (!emergencyActive && distance != -1) {
    if (distance < 8 || distance > 150) {
      triggerEmergency("BOAT UNSTABLE");
    }
  }

  // ---------- Temperature: Heat risk ----------
  float temp = dht.readTemperature();
  if (!emergencyActive && !isnan(temp) && temp > 40) {
    triggerEmergency("HEAT RISK");
  }

  // ---------- Manual SOS ----------
  if (!emergencyActive && digitalRead(SOS_PIN) == HIGH) {
    triggerEmergency("MANUAL SOS");
  }

  // ---------- Heartbeat ----------
  if (!emergencyActive && millis() - lastHeartbeat > HEARTBEAT_INTERVAL) {
    sendHeartbeat();
    lastHeartbeat = millis();
  }

  // ---------- Normal Status ----------
  if (!emergencyActive && millis() - lastStatusUpdate > STATUS_INTERVAL) {
    showDisplay("STATUS: SAFE", "Monitoring...", "");
    lastStatusUpdate = millis();
  }

  delay(200);
}
