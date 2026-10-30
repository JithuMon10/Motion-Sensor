// In-memory alert store for demonstration
let alertHistory = [];
let currentStatus = {
  state: "SECURE", // "SECURE" or "ALARM"
  lastTriggered: null,
  device: "STM32F411CEU6"
};

export default function handler(req, res) {
  // Set CORS headers
  res.setHeader("Access-Control-Allow-Credentials", true);
  res.setHeader("Access-Control-Allow-Origin", "*");
  res.setHeader("Access-Control-Allow-Methods", "GET,OPTIONS,PATCH,DELETE,POST,PUT");
  res.setHeader(
    "Access-Control-Allow-Headers",
    "X-CSRF-Token, X-Requested-With, Accept, Accept-Version, Content-Length, Content-MD5, Content-Type, Date, X-Api-Version"
  );

  if (req.method === "OPTIONS") {
    res.status(200).end();
    return;
  }

  // POST: Trigger alert from ESP32 or clear alarm from dashboard
  if (req.method === "POST") {
    const body = req.body || {};

    if (body.action === "clear") {
      currentStatus.state = "SECURE";
      return res.status(200).json({ success: true, message: "Alarm cleared", status: currentStatus });
    }

    // Trigger Motion Alert
    const timestamp = new Date().toLocaleTimeString("en-US", { hour12: true });
    const fullDate = new Date().toLocaleString();

    const alertEvent = {
      id: Date.now(),
      time: timestamp,
      dateTime: fullDate,
      device: body.device || "STM32F411CEU6",
      event: "Motion Detected by PIR Sensor",
      severity: "CRITICAL"
    };

    currentStatus.state = "ALARM";
    currentStatus.lastTriggered = fullDate;
    currentStatus.device = alertEvent.device;

    alertHistory.unshift(alertEvent);
    if (alertHistory.length > 25) alertHistory.pop(); // Keep last 25

    return res.status(200).json({
      success: true,
      message: "Security alert logged successfully",
      alert: alertEvent,
      status: currentStatus
    });
  }

  // GET: Fetch status and log
  return res.status(200).json({
    status: currentStatus,
    alerts: alertHistory
  });
}

