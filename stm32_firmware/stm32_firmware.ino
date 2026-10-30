/*
 * =========================================================================
 * Project: Smart Home Security System (5-Second Auto-Reset Cycle)
 * Board:   STM32F411CEU6 ("Black Pill")
 * 
 * Flow:
 * 1. Motion detected by PIR (PA0)
 * 2. Beep once (0.3s) + Send alert to ESP32 / Vercel
 * 3. Hold for 5 seconds (lockout prevents spamming)
 * 4. Automatically re-arms after 5 seconds!
 * =========================================================================
 */

#define PIR_PIN       PA0
#define BUZZER_PIN    PA1
#define LED_PIN       PC13    // Onboard Blue LED
#define SerialESP     Serial2 // PA2 (TX), PA3 (RX) to ESP32

void beepOnce() {
  // Short, clear beep (0.3 seconds)
  digitalWrite(BUZZER_PIN, HIGH);
  tone(BUZZER_PIN, 2000, 250);
  delay(250);
  digitalWrite(BUZZER_PIN, LOW);
  noTone(BUZZER_PIN);
}

void setup() {
  // Standard high-impedance INPUT so PIR sensor signal is not attenuated
  pinMode(PIR_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  // Turn off LED and Buzzer
  digitalWrite(LED_PIN, HIGH); // OFF on Black Pill
  digitalWrite(BUZZER_PIN, LOW);

  Serial.begin(115200);
  SerialESP.begin(9600);

  // Startup chirp
  beepOnce();
  Serial.println("[STM32] System Ready: 5-Second Cycle Mode");
}

void loop() {
  // Read PIR sensor
  if (digitalRead(PIR_PIN) == HIGH) {
    Serial.println("[STM32] >>> MOTION DETECTED! <<<");

    // 1. Turn on Blue LED
    digitalWrite(LED_PIN, LOW);

    // 2. Alert ESP32 -> Vercel & Phone
    SerialESP.println("ALERT:MOTION");

    // 3. Beep once
    beepOnce();

    // 4. Wait 5 seconds (Auto-reset cycle)
    // Prevents any repeated beeping during this window
    Serial.println("[STM32] Alert active. Waiting 5s before re-arming...");
    delay(5000);

    // 5. Re-arm system
    digitalWrite(LED_PIN, HIGH); // LED OFF
    Serial.println("[STM32] Re-armed! Ready for next motion event.");
  }

  delay(100); // Polling interval
}
