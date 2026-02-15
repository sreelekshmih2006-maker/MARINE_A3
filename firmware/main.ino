void setup() {
  initSensors();
  initDisplay();
  initCommunication();
}

void loop() {
  readSensors();
  evaluateRisk();
  handleAlerts();
  sendHeartbeat();
}
