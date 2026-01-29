# Compatibility Matrix

This project supports common BLE diesel/air heaters that use the FFE0/FFE1 service/characteristic UUIDs and AA55/AA66 packet headers.

Supported protocol variants:
- AA55 (default)
- AA66
- Encrypted XOR variants (baseline password 0x0C 0x22)

Heater models verified (community contributed):
- Vevor 5kW/8kW (2022–2024 batches) — AA55
- Generic Chinese diesel heater (blue display) — AA55
- Generic Chinese diesel heater (green display) — AA66

ESP32 boards tested:
- ESP32-WROOM-32 DevKit (ESPHome latest)
- ESP32-C3 (note: BLE client stability varies)

Notes:
- Protocol auto-detection attempts AA55 first, then AA66.
- If your unit requires a different password, set it via `password:` in the component config.
- Please open an issue or PR to extend this table with your hardware results.
