/*
 * =========================================================================
 * Project: Smart Home Security System (Syllabus Core)
 * Board:   STM32F411CEU6 ("Black Pill")
 * Inputs:  PIR Motion Sensor (PA0)
 * Outputs: Active Buzzer (PA1)
 * Comms:   Hardware UART Serial2 (PA2 = TX, PA3 = RX) connected to ESP32
 * =========================================================================
 */

#define PIR_PIN       PA0   // PIR Motion Sensor Output connected here
#define BUZZER_PIN    PA1   // Buzzer connected here (+ Diode 1N4007 across terminals)

// Serial2 is natively mapped to PA2 (TX) and PA3 (RX) on STM32F411
#define SerialESP Serial2

unsigned long lastAlertTime = 0;
const unsigned long alertCooldown = 8000; // 8 seconds cooldown between triggers

void setup() {
  pinMode(PIR_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  // Debug monitor via PC USB
  Serial.begin(115200);

  // Serial link to ESP32 module
  SerialESP.begin(9600);

  // Allow PIR sensor ~2-3 seconds to settle
  delay(2500);
  Serial.println("[STM32] Security System Armed & Monitoring...");
}

void triggerAlarm() {
  Serial.println("[STM32] >>> MOTION DETECTED! Sending alert to ESP32 <<<");

  // 1. Send UART message to ESP32
  SerialESP.println("ALERT:MOTION");

  // 2. Beep buzzer 3 times
  for (int i = 0; i < 3; i++) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(150);
    digitalWrite(BUZZER_PIN, LOW);
    delay(100);
  }
}

void loop() {
  int motion = digitalRead(PIR_PIN);

  if (motion == HIGH) {
    if (millis() - lastAlertTime > alertCooldown) {
      triggerAlarm();
      lastAlertTime = millis();
    }
  }
}

