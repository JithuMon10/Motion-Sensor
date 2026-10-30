/*
 * =========================================================================
 * Project: Smart Home Security System (Network Gateway)
 * Board:   ESP32 (or ESP8266)
 * Role:    Receives UART triggers from STM32F411, forwards to Vercel & Phone
 * Comms:   UART2 (RX2 = GPIO 16, TX2 = GPIO 17) or Serial (GPIO 3 RX, GPIO 1 TX)
 * =========================================================================
 */

#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

// -------- 1. ENTER YOUR WIFI CREDENTIALS HERE --------
const char* WIFI_SSID     = "YOUR_WIFI_NAME";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

// -------- 2. YOUR VERCEL API URL --------
// Replace with your deployed Vercel domain (e.g. "https://stm32-security.vercel.app/api/alert")
const char* VERCEL_URL    = "https://YOUR_VERCEL_DOMAIN.vercel.app/api/alert";

// -------- 3. PHONE NOTIFICATION (ntfy.sh) --------
// Install "ntfy" app on your phone and subscribe to this exact topic name:
const char* NTFY_TOPIC    = "stm32-smart-security-alert";

// Hardware Serial on ESP32 to talk to STM32 (Using RX=16, TX=17 or standard Serial)
HardwareSerial stm32Serial(2); // UART2: RX2 = GPIO 16, TX2 = GPIO 17

void setup() {
  // Debug monitor to PC via USB
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n[ESP32] Initializing Gateway...");

  // UART connection to STM32
  // ESP32 GPIO 16 (RX2) -> Connect to STM32 PA2 (TX)
  // ESP32 GPIO 17 (TX2) -> Connect to STM32 PA3 (RX)
  stm32Serial.begin(9600, SERIAL_8N1, 16, 17);

  // Connect to Wi-Fi
  Serial.print("[ESP32] Connecting to Wi-Fi: ");
  Serial.println(WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\n[ESP32] Connected to Wi-Fi!");
  Serial.print("[ESP32] IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.println("[ESP32] Listening for STM32 alerts on UART...");
}

// 1. Send push alert to phone via ntfy.sh
void sendPhoneNotification() {
  if (WiFi.status() != WL_CONNECTED) return;

  WiFiClientSecure client;
  client.setInsecure(); // Bypass SSL cert validation

  HTTPClient https;
  String url = String("https://ntfy.sh/") + NTFY_TOPIC;

  if (https.begin(client, url)) {
    https.addHeader("Content-Type", "text/plain");
    https.addHeader("Title", "🚨 Intruder Alert!");
    https.addHeader("Priority", "urgent");
    https.addHeader("Tags", "warning,rotating_light");

    int code = https.POST("Motion detected by STM32 security node at front area!");
    Serial.printf("[ESP32] Phone push sent (code: %d)\n", code);
    https.end();
  }
}

// 2. Send alert event to Vercel Web Dashboard
void sendVercelAlert() {
  if (WiFi.status() != WL_CONNECTED) return;
  if (String(VERCEL_URL).indexOf("YOUR_VERCEL_DOMAIN") != -1) {
    Serial.println("[ESP32] Note: Update VERCEL_URL with your actual Vercel link.");
    return;
  }

  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient https;
  if (https.begin(client, VERCEL_URL)) {
    https.addHeader("Content-Type", "application/json");
    String payload = "{\"device\":\"STM32F411CEU6\",\"event\":\"Motion Detected\"}";

    int code = https.POST(payload);
    Serial.printf("[ESP32] Vercel alert posted (code: %d)\n", code);
    https.end();
  }
}

void loop() {
  // Check if STM32 sent any data
  if (stm32Serial.available()) {
    String message = stm32Serial.readStringUntil('\n');
    message.trim();

    Serial.print("[ESP32] Received from STM32: ");
    Serial.println(message);

    if (message == "ALERT:MOTION") {
      Serial.println("[ESP32] >>> TRIGGERING CLOUD NOTIFICATIONS <<<");
      sendPhoneNotification();
      sendVercelAlert();
    }
  }
}

