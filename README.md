# 🛡️ Smart Home Security System with STM32 & ESP32

An IoT-based Smart Security and Intruder Detection System. The project uses an **STM32F411CEU6 ("Black Pill")** as the core controller to monitor a PIR motion sensor and sound a buzzer, with an **ESP32** acting as a Wi-Fi gateway to log security events on a **Vercel Web Dashboard** and send instant push notifications to a smartphone.

---

## 📐 System Architecture

```
[ PIR Sensor ] ---> [ STM32F411CEU6 ] ---> [ Buzzer + 1N4007 ]
                           |
                     (UART Serial)
                           v
                       [ ESP32 ]
                           |
                      (Wi-Fi / HTTPS)
                           v
     +---------------------+---------------------+
     |                                           |
     v                                           v
[ Vercel Web Dashboard ]                 [ Phone Alert (ntfy.sh) ]
```

---

## 🔌 Hardware Connections

### 1. PIR Sensor to STM32
- **VCC** ➔ STM32 `5V`
- **GND** ➔ STM32 `GND`
- **OUT** ➔ STM32 `PA0`

### 2. Active Buzzer & Diode
- **Buzzer (+)** ➔ STM32 `PA1`
- **Buzzer (-)** ➔ STM32 `GND`
- **1N4007 Diode** ➔ Across Buzzer (+ and -): Cathode (stripe) to (+), Anode to (-)

### 3. STM32 to ESP32 (UART Bridge)
- **STM32 PA2 (TX)** ➔ ESP32 `GPIO 16 (RX2)`
- **STM32 PA3 (RX)** ➔ ESP32 `GPIO 17 (TX2)`
- **STM32 GND** ➔ ESP32 `GND` *(Mandatory common ground)*

---

## 📁 Repository Structure

```text
├── api/
│   └── alert.js             # Vercel Serverless Function (Event logging & state management)
├── public/
│   └── index.html           # Real-time Web Dashboard with live audio alarm
├── stm32_firmware/
│   └── stm32_firmware.ino   # Core STM32 code (PIR reading, buzzer pattern, UART trigger)
├── esp32_firmware/
│   └── esp32_firmware.ino   # ESP32 Wi-Fi gateway code (HTTP POST to Vercel & ntfy.sh)
├── vercel.json              # Vercel deployment and routing rules
└── package.json             # Web application metadata
```

---

## 🚀 Setup & Deployment

### 1. Deploying the Web Dashboard on Vercel
1. Log in to [Vercel](https://vercel.com).
2. Click **"Add New..."** ➔ **"Project"**.
3. Import this GitHub repository: `JithuMon10/Motion-Sensor`.
4. Click **Deploy**. Vercel will provide your live URL (e.g. `https://motion-sensor-xxx.vercel.app`).

### 2. Flashing the ESP32
1. Open `esp32_firmware/esp32_firmware.ino` in Arduino IDE.
2. Enter your Wi-Fi name, password, and your Vercel URL.
3. Select board: **ESP32 Dev Module**.
4. Upload to the ESP32 (hold the **BOOT** button on the board if it says `Connecting........`).

### 3. Flashing the STM32F411
1. Open `stm32_firmware/stm32_firmware.ino` in Arduino IDE.
2. Select board: **Generic STM32F4 series** ➔ **BlackPill F411CE**.
3. Upload to the STM32 via USB.

### 4. Phone Notifications (ntfy)
1. Install **ntfy** on iOS or Android.
2. Subscribe to the topic: `stm32-smart-security-alert`.
