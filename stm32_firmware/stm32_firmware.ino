/*
 * =========================================================================
 * STM32F411 Diagnostic & Security Firmware
 * - Tests onboard LED (PC13)
 * - Tests Buzzer (handles both Active & Passive buzzers)
 * - Tests PIR Sensor input on PA0
 * =========================================================================
 */

#define PIR_PIN       PA0
#define BUZZER_PIN    PA1
#define LED_PIN       PC13    // Onboard Blue LED on STM32 Black Pill
#define SerialESP     Serial2 // PA2 (TX), PA3 (RX)

unsigned long lastAlertTime = 0;
const unsigned long alertCooldown = 5000;

void beepBuzzer() {
  // Test 1: Direct DC pulse (for Active Buzzer)
  digitalWrite(BUZZER_PIN, HIGH);
  delay(150);
  digitalWrite(BUZZER_PIN, LOW);
  delay(50);

  // Test 2: Frequency tone (for Passive Buzzer)
  tone(BUZZER_PIN, 2400, 200);
  delay(200);
  noTone(BUZZER_PIN);
}

void setup() {
  pinMode(PIR_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  // Turn ON onboard LED to prove the STM32 is alive and running code
  digitalWrite(LED_PIN, LOW); // On Black Pill, LOW turns onboard LED ON

  Serial.begin(115200);
  SerialESP.begin(9600);

  // Startup Test: Beep the buzzer ONCE at startup to test if buzzer works!
  beepBuzzer();
  delay(500);
  digitalWrite(LED_PIN, HIGH); // Turn off LED after test

  Serial.println("[STM32] Self-test complete. Monitoring PA0...");
}

void loop() {
  int motion = digitalRead(PIR_PIN);

  // Mirror PIR sensor state directly to onboard LED (Instant visual check)
  if (motion == HIGH) {
    digitalWrite(LED_PIN, LOW); // LED ON when motion is detected
    
    if (millis() - lastAlertTime > alertCooldown) {
      Serial.println("[STM32] >>> MOTION DETECTED! <<<");
      
      // Trigger ESP32
      SerialESP.println("ALERT:MOTION");

      // Beep
      beepBuzzer();
      
      lastAlertTime = millis();
    }
  } else {
    digitalWrite(LED_PIN, HIGH); // LED OFF when clear
  }

  delay(50);
}
