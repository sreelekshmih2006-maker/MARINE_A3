/*
  MARINE-A³
  Autonomous • Aware • Always-On Marine Safety System

  Board : Vega 3
  Sensors:
   - BMP280 (Temp)
   - BMP280 (Pressure)
*/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_BMP280.h>

// ---------- OLED ----------
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// ---------- BMP Sensors ----------
Adafruit_BMP280 bmpTemp;     // Internal temperature
Adafruit_BMP280 bmpPressure; // External pressure

// ---------- Timing ----------
unsigned long lastHeartbeat = 0;
unsigned long lastPresence  = 0;
unsigned long lastStatus    = 0;

const unsigned long HEARTBEAT_INTERVAL = 30000;
const unsigned long PRESENCE_TIMEOUT  = 60000;
const unsigned long STATUS_INTERVAL   = 2000;

// ---------- Thresholds ----------
const float TEMP_LIMIT = 40.0;        // °C
const float PRESSURE_DROP = 6.0;       // hPa
const float ULTRA_MIN = 8.0;            // cm
const float ULTRA_MAX = 150.0;          // cm

// ---------- State ----------
bool emergencyActive = false;
float baselinePressure = 0;

// ---------- Functions ----------

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

void buzzAlert(int count) {
  for (int i = 0; i < count; i++) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(200);
    digitalWrite(BUZZER_PIN, LOW);
    delay(200);
  }
}

void showDisplay(String a, String b, String c) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("MARINE-A3");
  display.println(a);
  display.println(b);
  display.println(c);
  display.display();
}

void triggerEmergency(String reason) {
  if (emergencyActive) return;

  emergencyActive = true;
  buzzAlert(5);

  Serial.println("EMERGENCY: " + reason);
  Serial2.println("SOS|" + reason);

  showDisplay("EMERGENCY!", reason, "SOS SENT");
}

void sendHeartbeat() {
  Serial2.println("HEARTBEAT|OK");
}

// ---------- Setup ----------

void setup() {
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(IR_PIN, INPUT);
  pinMode(SOS_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, BT_RX, BT_TX);

  Wire.begin(SDA_PIN, SCL_PIN);

  if (!bmpTemp.begin(0x76) || !bmpPressure.begin(0x77)) {
    Serial.println("BMP init failed!");
    while (1);
  }

  baselinePressure = bmpPressure.readPressure() / 100.0;

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  showDisplay("Booting...", "Calibrating", "");

  lastPresence = millis();
  lastHeartbeat = millis();
}

// ---------- Loop ----------

void loop() {

  // ----- IR Presence -----
  if (digitalRead(IR_PIN) == LOW) {
    lastPresence = millis();
  }

  if (!emergencyActive && millis() - lastPresence > PRESENCE_TIMEOUT) {
    triggerEmergency("NO PRESENCE");
  }

  // ----- Ultrasonic Boat Stability -----
  long dist = readUltrasonic();
  if (!emergencyActive && dist != -1) {
    if (dist < ULTRA_MIN || dist > ULTRA_MAX) {
      triggerEmergency("BOAT UNSTABLE");
    }
  }

  // ----- Temperature (BMP-TEMP) -----
  float temp = bmpTemp.readTemperature();
  if (!emergencyActive && temp > TEMP_LIMIT) {
    triggerEmergency("HEAT STRESS");
  }

  // ----- Pressure (BMP-PRESS) -----
  float pressure = bmpPressure.readPressure() / 100.0;
  if (!emergencyActive && baselinePressure - pressure > PRESSURE_DROP) {
    triggerEmergency("STORM RISK");
  }

  // ----- Manual SOS -----
  if (!emergencyActive && digitalRead(SOS_PIN) == HIGH) {
    triggerEmergency("MANUAL SOS");
  }

  // ----- Heartbeat -----
  if (!emergencyActive && millis() - lastHeartbeat > HEARTBEAT_INTERVAL) {
    sendHeartbeat();
    lastHeartbeat = millis();
  }

  // ----- Normal Display -----
  if (!emergencyActive && millis() - lastStatus > STATUS_INTERVAL) {
    showDisplay("STATUS: SAFE", "Monitoring...", "");
    lastStatus = millis();
  }

  delay(200);
}
