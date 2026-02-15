/*
  MARINE-A³ — Boat Safety Node
  Board: Vega 3
  Communication: Bluetooth (UART)
*/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_BMP280.h>

// ---------- OLED ----------
Adafruit_SSD1306 display(128, 64, &Wire, -1);

// ---------- BMP Sensors ----------
Adafruit_BMP280 bmpTemp;     // Internal temperature
Adafruit_BMP280 bmpPress;    // External pressure

// ---------- PINS ----------
#define TRIG_PIN 5
#define ECHO_PIN 18
#define IR_PIN 19
#define SOS_PIN 23
#define BUZZER_PIN 27

// Bluetooth UART
#define BT_RX 16
#define BT_TX 17

// I2C
#define SDA_PIN 21
#define SCL_PIN 22

// ---------- THRESHOLDS ----------
#define TEMP_LIMIT 40.0
#define PRESSURE_DROP 6.0
#define ULTRA_MIN 8
#define ULTRA_MAX 150

// ---------- TIMING ----------
unsigned long lastPresence = 0;
unsigned long presenceTimeout = 60000;

float basePressure = 0;
bool emergency = false;

// ---------- FUNCTIONS ----------
long readUltrasonic() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long d = pulseIn(ECHO_PIN, HIGH, 30000);
  if (d == 0) return -1;
  return d * 0.034 / 2;
}

void alert(String reason) {
  if (emergency) return;
  emergency = true;

  digitalWrite(BUZZER_PIN, HIGH);
  delay(2000);
  digitalWrite(BUZZER_PIN, LOW);

  Serial2.println("SOS|" + reason);

  display.clearDisplay();
  display.setCursor(0,0);
  display.println("MARINE-A3");
  display.println("EMERGENCY");
  display.println(reason);
  display.display();
}

// ---------- SETUP ----------
void setup() {
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(IR_PIN, INPUT);
  pinMode(SOS_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, BT_RX, BT_TX);

  Wire.begin(SDA_PIN, SCL_PIN);

  bmpTemp.begin(0x76);
  bmpPress.begin(0x77);
  basePressure = bmpPress.readPressure() / 100.0;

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();

  lastPresence = millis();
}

// ---------- LOOP ----------
void loop() {

  // IR presence
  if (digitalRead(IR_PIN) == LOW) {
    lastPresence = millis();
  }
  if (!emergency && millis() - lastPresence > presenceTimeout) {
    alert("NO PRESENCE");
  }

  // Ultrasonic
  long d = readUltrasonic();
  if (!emergency && d != -1 && (d < ULTRA_MIN || d > ULTRA_MAX)) {
    alert("BOAT UNSTABLE");
  }

  // Temperature
  if (!emergency && bmpTemp.readTemperature() > TEMP_LIMIT) {
    alert("HEAT STRESS");
  }

  // Pressure
  float p = bmpPress.readPressure() / 100.0;
  if (!emergency && basePressure - p > PRESSURE_DROP) {
    alert("STORM RISK");
  }

  // Manual SOS
  if (!emergency && digitalRead(SOS_PIN) == HIGH) {
    alert("MANUAL SOS");
  }

  delay(200);
}
