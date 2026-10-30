let alertHistory = [];
let currentStatus = {
  state: "SECURE", // "SECURE" or "ALARM"
  lastTriggered: null,
  triggeredAt: 0,
  device: "STM32F411CEU6"
};

export default function handler(req, res) {
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

  // POST: Trigger alert or manual reset
  if (req.method === "POST") {
    const body = req.body || {};

    if (body.action === "clear") {
      currentStatus.state = "SECURE";
      return res.status(200).json({ success: true, message: "Alarm cleared", status: currentStatus });
    }

    const now = Date.now();
    const timestamp = new Date().toLocaleTimeString("en-US", { hour12: true });
    const fullDate = new Date().toLocaleString();

    const alertEvent = {
      id: now,
      time: timestamp,
      dateTime: fullDate,
      device: body.device || "STM32F411CEU6",
      event: "Motion Detected by PIR Sensor",
      severity: "CRITICAL"
    };

    currentStatus.state = "ALARM";
    currentStatus.lastTriggered = fullDate;
    currentStatus.triggeredAt = now;
    currentStatus.device = alertEvent.device;

    alertHistory.unshift(alertEvent);
    if (alertHistory.length > 25) alertHistory.pop();

    return res.status(200).json({
      success: true,
      message: "Security alert logged",
      alert: alertEvent,
      status: currentStatus
    });
  }

  // Auto-reset state back to SECURE after 5 seconds (5000ms)
  if (currentStatus.state === "ALARM" && (Date.now() - currentStatus.triggeredAt) > 5000) {
    currentStatus.state = "SECURE";
  }

  return res.status(200).json({
    status: currentStatus,
    alerts: alertHistory
  });
}
