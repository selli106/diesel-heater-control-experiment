# ESPHome Diesel Heater BLE - Combined Project

A comprehensive ESPHome external component for controlling Bluetooth-enabled diesel/air heaters. This project combines knowledge and protocols from multiple sources to provide maximum compatibility with various heater models.

## 🔥 Features

- **Multi-Protocol Support**: Supports multiple heater variants (AA55, AA66, encrypted variants)
- **ESPHome Integration**: Full native integration with ESPHome/Home Assistant
- **Comprehensive Control**: Power, temperature, heating level, operational modes
- **Sensor Monitoring**: Temperature, voltage, altitude, error codes, runtime statistics
- **Auto-Detection**: Automatically detects heater protocol variant
- **BLE Communication**: Reliable Bluetooth Low Energy communication

## 📋 Supported Heater Models

Based on combined research from multiple projects, this component supports:
- VEVOR Diesel Air Heaters (All-in-One model)
- Generic Chinese diesel heaters with BLE (AA55/AA66 protocols)
- Encrypted and non-encrypted protocol variants

## 🛠️ Hardware Requirements

- ESP32 board with Bluetooth capability
- Diesel/air heater with BLE support
- Power supply for ESP32 (can often be powered from heater's 12V supply with regulator)

## 📡 BLE Protocol Overview

**Connection Details:**
- Service UUID: `0000FFE0-0000-1000-8000-00805F9B34FB`
- Characteristic UUID: `0000FFE1-0000-1000-8000-00805F9B34FB`
- Protocol: Write with response + Notify
- Default password: `0x0C 0x22` (12, 34 in decimal)

**Supported Operations:**
- Status monitoring (temperature, voltage, error codes)
- Power control (on/off)
- Temperature/level adjustment (8-36°C or 1-10 levels)
- Mode switching (automatic temperature vs manual level)
- Configuration (language, units, settings)

**Protocol Variants:**
- AA55 (standard, most common)
- AA66 (less common variant)
- Encrypted variants (auto-detected)

📖 **For complete protocol documentation, see [doc/PROTOCOL.md](doc/PROTOCOL.md)**

## 🚀 Quick Start

### 1. Install ESPHome

```bash
pip install esphome
```

### 2. Create Your Configuration

Create a file named `diesel-heater.yaml`:

```yaml
esphome:
  name: diesel-heater
  platform: ESP32
  board: esp32dev

wifi:
  ssid: !secret wifi_ssid
  password: !secret wifi_password

api:
  encryption:
    key: !secret api_encryption_key

ota:
  password: !secret ota_password

logger:

external_components:
  - source:
      type: local
      path: components
    components: [ diesel_heater_ble ]

esp32_ble_tracker:

ble_client:
  - mac_address: "XX:XX:XX:XX:XX:XX"  # Your heater's MAC address
    id: heater_ble_client

diesel_heater_ble:
  - ble_client_id: heater_ble_client
    id: my_heater

# Sensors
sensor:
  - platform: diesel_heater_ble
    diesel_heater_ble: my_heater
    running_state:
      name: "Heater Running State"
    error_code:
      name: "Heater Error Code"
    set_temp:
      name: "Set Temperature"
    supply_voltage:
      name: "Supply Voltage"
    case_temp:
      name: "Case Temperature"
    cab_temp:
      name: "Room Temperature"

# Controls
switch:
  - platform: diesel_heater_ble
    diesel_heater_ble: my_heater
    power:
      name: "Heater Power"

number:
  - platform: diesel_heater_ble
    diesel_heater_ble: my_heater
    power_level:
      name: "Power Level"
      min_value: 1
      max_value: 10
      step: 1
    set_temp:
      name: "Target Temperature"
      min_value: 8
      max_value: 36
      step: 1

button:
  - platform: diesel_heater_ble
    diesel_heater_ble: my_heater
    level_up:
      name: "Level Up"
    level_down:
      name: "Level Down"
    temp_up:
      name: "Temperature Up"
    temp_down:
      name: "Temperature Down"
```

### 3. Find Your Heater's MAC Address

Use an ESP32 BLE scanner or phone app like "BLE Scanner" to find your heater's MAC address.

### 4. Compile and Upload

```bash
esphome run diesel-heater.yaml
```

## 📊 Available Entities

### Sensors
- Running State
- Error Code
- Running Step
- Altitude
- Running Mode
- Set Level
- Set Temperature
- Supply Voltage
- Case Temperature
- Cabin/Room Temperature
- Start Time
- Auto Time
- Run Time
- Language
- Temperature Offset
- Tank Volume
- Oil Pump Type
- Temperature Unit
- Altitude Unit
- Automatic Heating Status

### Controls
- Power Switch (On/Off)
- Power Level Number (1-10)
- Target Temperature Number (8-36°C)
- Level Up/Down Buttons
- Temperature Up/Down Buttons

## 🔧 Advanced Configuration

### Custom Update Interval

```yaml
diesel_heater_ble:
  - ble_client_id: heater_ble_client
    id: my_heater
    update_interval: 5s  # Default is 1s
```

### Using with Home Assistant

All entities will automatically appear in Home Assistant when using the ESPHome integration.

Example automation:
```yaml
automation:
  - alias: "Turn on heater when cold"
    trigger:
      - platform: numeric_state
        entity_id: sensor.room_temperature
        below: 15
    action:
      - service: switch.turn_on
        entity_id: switch.heater_power
      - service: number.set_value
        entity_id: number.target_temperature
        data:
          value: 20
```

## 🐛 Troubleshooting

### Heater Not Connecting

1. Ensure your heater's Bluetooth is enabled
2. Verify the MAC address is correct
3. Make sure no other device is connected to the heater
4. Check ESP32 Bluetooth is working: `esphome logs diesel-heater.yaml`

### Incorrect Values

1. Different heater models may use different protocols
2. Check the logs for protocol auto-detection messages
3. Some values may need calibration/offset adjustment

### Connection Drops

1. Ensure ESP32 is within range (typically 10-30 feet)
2. Check power supply stability
3. Reduce BLE scan interval if experiencing interference

## 📚 Protocol References

This project combines research from:
- **warehog/esphome-diesel-heater-ble**: ESPHome implementation, protocol analysis
- **iotmaestro/vevor-heater-ble**: VEVOR protocol documentation
- **spin877/Bruciatore_BLE**: Python BLE implementation
- **AirHeaterBLE**: Android APK reverse engineering

## 🤝 Contributing

Contributions are welcome! If you have a different heater model or protocol variant:

1. Enable ESPHome debugging
2. Capture BLE traffic
3. Document the differences
4. Submit a pull request or issue

## ⚠️ Safety Notice

- This software is provided for educational and automation purposes
- Diesel heaters can be dangerous if not used properly
- Never leave heater unattended
- Ensure proper ventilation
- Follow manufacturer safety guidelines
- Author is not responsible for any damage or injury

## 📄 License

This project combines code from multiple sources:
- ESPHome components: Apache License 2.0
- VEVOR protocol implementation: MIT License
- Additional code: MIT License

See LICENSE file for details.

## 🙏 Acknowledgments

- [warehog](https://github.com/warehog) - ESPHome implementation
- [iotmaestro](https://github.com/iotmaestro) - VEVOR protocol documentation
- [spin877](https://github.com/spin877) - Python BLE implementation
- ESPHome community
- Home Assistant community

## 📞 Support

- Open an issue on GitHub
- Check existing issues for solutions
- Join Home Assistant Discord for ESPHome help

---

**Made with ❤️ for the Home Automation community**
