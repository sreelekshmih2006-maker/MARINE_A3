## Pin Connections (Logical)

- Ultrasonic Sensor → Boat tilt / water level
- PIR Sensor → Fisherman presence
- Touch Sensor → Manual SOS
- DHT Sensor → Temperature & humidity
- BMP Sensor → Atmospheric pressure
- OLED Display → System status
- Buzzer → Local alert
- RTC Module → Heartbeat timing
- Bluetooth / WiFi → Emergency signal transmission
🔌 MARINE-A³ — Pin Connections

Hardware Configuration

2 × Vega 3 boards

1 × Bluetooth module

1 × Wi-Fi (built-in on Vega 3)

Sensors mounted on Boat Safety Node

🛥️ Vega Board 1 — Boat Safety Node

(Sensors + Bluetooth)

Power Notes

All sensors use 3.3 V unless specified

Common GND for all components

Ultrasonic sensor uses 5 V

Ultrasonic Sensor (Boat Stability / Water Ingress)
Ultrasonic Pin	Vega 3 GPIO
VCC	5 V
GND	GND
TRIG	GPIO 5
ECHO	GPIO 18
IR Sensor (Fisherman Presence)
IR Pin	Vega 3 GPIO
VCC	3.3 V
GND	GND
OUT	GPIO 19
BMP280 — Temperature Sensor (Internal)

I²C Address: 0x76

BMP-TEMP Pin	Vega 3 GPIO
VCC	3.3 V
GND	GND
SDA	GPIO 21
SCL	GPIO 22
BMP280 — Pressure Sensor (External)

I²C Address: 0x77
(SDO pin tied HIGH)

BMP-PRESS Pin	Vega 3 GPIO
VCC	3.3 V
GND	GND
SDA	GPIO 21
SCL	GPIO 22
SDO	3.3 V
Touch Sensor (Manual SOS)
Touch Pin	Vega 3 GPIO
VCC	3.3 V
GND	GND
OUT	GPIO 23
OLED Display (I²C)
OLED Pin	Vega 3 GPIO
VCC	3.3 V
GND	GND
SDA	GPIO 21
SCL	GPIO 22
Buzzer (Local Alert)
Buzzer Pin	Vega 3 GPIO
+	GPIO 27
−	GND
Bluetooth Module (HC-05 / HC-06)
Bluetooth Pin	Vega 3 GPIO
VCC	5 V
GND	GND
TX	GPIO 16 (RX2)
RX	GPIO 17 (TX2)

⚠️ Note: Use a voltage divider on Bluetooth RX if required.

🌊 Vega Board 2 — Relay / Gateway Node

(Bluetooth Receiver + Wi-Fi)

Bluetooth Module
Bluetooth Pin	Vega 3 GPIO
VCC	5 V
GND	GND
TX	GPIO 16
RX	GPIO 17
Wi-Fi

Vega 3 built-in Wi-Fi

No external wiring required

🔁 Communication Flow
[ Boat Sensors ]
       ↓
[ Vega 1 – Boat Safety Node ]
       ↓  Bluetooth (UART)
[ Vega 2 – Relay Node ]
       ↓  Wi-Fi
[ Shore / Server / Authorities ]

📌 Pin Usage Summary
Vega 1 (Boat Node)
Function	GPIO
Ultrasonic TRIG	5
Ultrasonic ECHO	18
IR Sensor	19
Touch SOS	23
I²C SDA	21
I²C SCL	22
Buzzer	27
Bluetooth RX	16
Bluetooth TX	17
Vega 2 (Relay Node)
Function	GPIO
Bluetooth RX	16
Bluetooth TX	17
