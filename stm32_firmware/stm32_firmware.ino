/*
 * =========================================================================
 * Project: Smart Home Security System with HC-SR04 Ultrasonic Sensor
 * Board:   STM32F411CEU6 ("Black Pill")
 * 
 * Logic:
 * - Monitors distance continuously
 * - If an object/intruder enters within 50 cm:
 *     1. Beeps buzzer ONCE (0.3s)
 *     2. Sends ALERT:MOTION to ESP32 -> Vercel & Phone
 *     3. Waits 5 seconds before re-arming
 * =========================================================================
 */

#define TRIG_PIN      PA0     // HC-SR04 Trig Pin
#define ECHO_PIN      PA4     // HC-SR04 Echo Pin
#define BUZZER_PIN    PA1     // Buzzer Pin
#define LED_PIN       PC13    // Onboard Blue LED
#define SerialESP     Serial2 // PA2 (TX), PA3 (RX) to ESP32

// Trigger distance threshold in centimeters (change if you want closer/farther)
const int TRIGGER_DISTANCE_CM = 50; 

void beepOnce() {
  digitalWrite(BUZZER_PIN, HIGH);
  tone(BUZZER_PIN, 2000, 250);
  delay(250);
  digitalWrite(BUZZER_PIN, LOW);
  noTone(BUZZER_PIN);
}

int getDistanceCM() {
  // Clear trigger pin
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  // Send 10µs ultrasonic pulse
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Read the echo return time (timeout 30ms = ~5 meters max)
  long duration = pulseIn(ECHO_PIN, HIGH, 30000);

  if (duration == 0) {
    return 999; // Out of range or no echo received
  }

  // Calculate distance in cm: Speed of sound = 343 m/s = 0.0343 cm/µs
  int distance = duration * 0.0343 / 2;
  return distance;
}

void setup() {
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  digitalWrite(LED_PIN, HIGH); // LED OFF initially
  digitalWrite(BUZZER_PIN, LOW);

  Serial.begin(115200);
  SerialESP.begin(9600);

  // Test beep on boot
  beepOnce();
  Serial.println("[STM32] Ultrasonic Security Ready! Threshold: 50 cm");
}

void loop() {
  int distance = getDistanceCM();

  // Print distance to Serial Monitor for live visual feedback
  if (distance < 999) {
    Serial.print("[STM32] Current Distance: ");
    Serial.print(distance);
    Serial.println(" cm");
  }

  // If distance is within threshold (e.g. hand placed closer than 50 cm)
  if (distance > 2 && distance <= TRIGGER_DISTANCE_CM) {
    Serial.println(">>> INTRUDER / MOTION DETECTED WITHIN RANGE! <<<");

    // 1. Turn on Blue LED
    digitalWrite(LED_PIN, LOW);

    // 2. Alert ESP32 -> Vercel & Phone
    SerialESP.println("ALERT:MOTION");

    // 3. Beep once
    beepOnce();

    // 4. Wait 5 seconds (Website countdown & cooldown)
    Serial.println("[STM32] Alarm active. Waiting 5s before re-arming...");
    delay(5000);

    // 5. Re-arm
    digitalWrite(LED_PIN, HIGH); // LED OFF
    Serial.println("[STM32] Re-armed! Ready for next detection.");
  }

  delay(150); // Polling delay
}
