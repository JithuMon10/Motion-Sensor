/*
 * =========================================================================
 * Project: Smart Home Security System (Teacher Presentation Mode)
 * Board:   STM32F411CEU6 ("Black Pill")
 * 
 * Features:
 * - Edge Detection: Triggers ONCE when motion starts (never loops/spams!)
 * - Must return to LOW before it can trigger again
 * - 15-second cooldown protection
 * - Crisp, polite double-chirp beep
 * =========================================================================
 */

#define PIR_PIN       PA0
#define BUZZER_PIN    PA1
#define LED_PIN       PC13    // Onboard Blue LED
#define SerialESP     Serial2 // PA2 (TX), PA3 (RX) to ESP32

int previousState = LOW;
unsigned long lastTriggerTime = 0;
const unsigned long COOLDOWN_MS = 15000; // 15 seconds cooldown between alarms

void politeBeep() {
  // Two quick, crisp chirps (0.3s total) - Works on both active and passive buzzers
  for (int i = 0; i < 2; i++) {
    digitalWrite(BUZZER_PIN, HIGH);
    tone(BUZZER_PIN, 2200, 80);
    delay(80);
    digitalWrite(BUZZER_PIN, LOW);
    noTone(BUZZER_PIN);
    delay(70);
  }
}

void setup() {
  pinMode(PIR_PIN, INPUT_PULLDOWN); // Use internal pull-down to prevent floating pin
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  // Turn off LED and Buzzer initially
  digitalWrite(LED_PIN, HIGH); // OFF on Black Pill
  digitalWrite(BUZZER_PIN, LOW);

  Serial.begin(115200);
  SerialESP.begin(9600);

  // Ready signal: 1 short chirp
  politeBeep();
  Serial.println("[STM32] System Armed (Teacher Presentation Mode)");
}

void loop() {
  int currentState = digitalRead(PIR_PIN);

  // ONLY trigger on RISING EDGE (when sensor transitions from LOW to HIGH)
  if (currentState == HIGH && previousState == LOW) {
    if (millis() - lastTriggerTime > COOLDOWN_MS) {
      Serial.println("[STM32] >>> SINGLE MOTION EVENT DETECTED! <<<");

      // 1. Turn on Blue LED indicator
      digitalWrite(LED_PIN, LOW);

      // 2. Send alert to ESP32 / Vercel / Phone
      SerialESP.println("ALERT:MOTION");

      // 3. Short polite double-chirp
      politeBeep();

      // 4. Record trigger time
      lastTriggerTime = millis();

      // Turn off Blue LED
      digitalWrite(LED_PIN, HIGH);
    }
  }

  // Update previous state for edge detection
  previousState = currentState;

  delay(50); // Small 50ms debounce delay
}
