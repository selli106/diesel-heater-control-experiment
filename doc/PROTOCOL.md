# Diesel Heater BLE Protocol Documentation

## Overview

This document describes the Bluetooth Low Energy protocol used by various Chinese diesel/air heaters. The protocol has been reverse-engineered from multiple sources including Android APK analysis and BLE traffic sniffing.

## Protocol Variants

### Variant Detection

The component automatically detects the protocol variant based on response headers:

| Variant | Header Bytes | Encryption | Notes |
|---------|--------------|------------|-------|
| AA55    | 0xAA 0x55    | No         | Most common variant |
| AA66    | 0xAA 0x66    | No         | Less common |
| AA55E   | 0xAA 0x55    | Yes        | Encrypted responses |
| AA66E   | 0xAA 0x66    | Yes        | Encrypted responses |

### Encryption

Some heater models use XOR encryption on response data:

```cpp
// Decryption algorithm
std::vector<uint8_t> decrypt(const std::vector<uint8_t> &raw) {
    std::vector<uint8_t> decrypted = raw;
    uint8_t key = raw[2];  // Use command byte as key
    for (size_t i = 3; i < raw.size() - 1; i++) {
        decrypted[i] = raw[i] ^ key;
    }
    return decrypted;
}
```

## BLE Specifications

### Connection Parameters

- **Service UUID**: `0000FFE0-0000-1000-8000-00805F9B34FB`
- **Characteristic UUID**: `0000FFE1-0000-1000-8000-00805F9B34FB`
- **Connection Type**: Write with response + Notify
- **MTU**: 20-23 bytes (standard BLE)
- **Update Rate**: 1 Hz recommended

### Authentication

The heater uses an unconventional authentication method:
- No PIN/passkey prompt
- Authentication via command password bytes
- Default password: `0x0C 0x22` (decimal: 12, 34)
- Password can be changed on some models

## Command Structure

### Request Packet Format

```
Byte 0: Header 1 (0xAA)
Byte 1: Header 2 (0x55)
Byte 2: Password High (0x0C default)
Byte 3: Password Low (0x22 default)
Byte 4: Command Type
Byte 5: Data 1
Byte 6: Data 2
Byte 7: Checksum
```

### Checksum Calculation

```cpp
// Simple sum checksum for basic protocol (mode 0x55/0x66)
uint8_t checksum = 0;
for (int i = 2; i < 7; i++) {
    checksum += packet[i];
}
checksum = checksum % 256;
```

### CRC16 Checksum (Extended Mode 0x88)

For extended mode frames (32 bytes), use CRC16 with Modbus polynomial 0xA001:

```cpp
uint16_t calculate_crc16(uint8_t *data, uint16_t length) {
    uint16_t crc = 0xFFFF;
    for (uint16_t i = 0; i < length; i++) {
        crc ^= data[i];
        for (uint8_t j = 0; j < 8; j++) {
            if (crc & 0x0001) {
                crc = (crc >> 1) ^ 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }
    // Swap bytes: ((crc & 0xFF00) >> 8) | ((crc & 0x00FF) << 8)
    return ((crc & 0xFF00) >> 8) | ((crc & 0x00FF) << 8);
}
```

**CRC Validation for Responses:**
1. Store bytes 29-30 (received CRC)
2. Replace bytes 29-30 with transmitted random bytes (from positions 2-3)
3. Calculate CRC16 over all 31 bytes
4. Compare with stored CRC

## Extended Mode Protocol (0x88/136)

### Frame Structure - 32 bytes

Extended mode provides additional parameters for auto-scheduling, calibration, and configuration:

```
Byte 0:    0xAA (Header)
Byte 1:    0x88 (Extended mode)
Byte 2-3:  Random encryption bytes
Byte 4:    Command
Byte 5-6:  Data (little-endian)
Byte 7-28: Extended parameters
Byte 29-30: CRC16 checksum
Byte 31:   Reserved
```

### Extended Response Format (32 bytes)

| Byte(s) | Field | Type | Description |
|---------|-------|------|-------------|
| 0 | Header | uint8 | Always 0xAA |
| 1 | Mode | uint8 | 0x88 for extended |
| 2-3 | Random | uint8[2] | Encryption bytes |
| 3 | Running State | uint8 | 0=Off, 1=On |
| 4 | Error Code | uint8 | 0-10 (see table) |
| 5 | Running Step | uint8 | Operation phase |
| 6-7 | Altitude | uint16_le | Meters above sea level |
| 8 | Running Mode | uint8 | 1=Level, 2=Temp |
| 9 | Set Level/Temp | uint8 | Target value |
| 10 | Current Level | uint8 | Actual running level |
| 11-12 | Supply Voltage | uint16_le | Volts × 10 |
| 13-14 | Case Temp | int16_le | °C (signed) |
| 15-16 | Cabin Temp | int16_le | °C (signed) |
| 17-18 | Auto Time | uint16_le | Start time (minutes since midnight) |
| 19-20 | Runtime | uint16_le | Duration (minutes) |
| 21 | Is Auto | uint8 | 0=Manual, 1=Auto |
| 22 | Language | uint8 | 0=EN, 1=ZH, 2=RU, 3=Disabled |
| 23 | Temp Offset | int8 | Calibration (-10 to +10°C) |
| 24 | Tank Volume | uint8 | Liters (0-50) |
| 25 | Oil Pump Type | uint8 | 0=16ul, 1=22ul, 2=28ul, 3=32ul |
| 26 | Bluetooth Switch | uint8 | 0=Enabled, 1=Disabled (inverted) |
| 27 | Remote Matching | uint8 | 0=Paired, 1=Unpaired (inverted) |
| 28 | Auto Heating | uint8 | 0=Disabled, 1=Enabled |
| 29-30 | CRC16 | uint16_le | Checksum |

### Random Encryption Bytes

Extended mode uses random bytes for obfuscation:
```javascript
random_byte_1 = floor(random() * 255)
random_byte_2 = floor(random() * 255)
```

These bytes are:
- Generated for each request frame at positions 2-3
- Used to replace bytes 29-30 during CRC calculation
- Provide basic obfuscation (not cryptographic security)

## Protocol Mode Detection

Try connection modes in this order:
1. **Mode 0x88** (Extended) - 32-byte frames with CRC16
2. **Mode 0x55** (Basic) - 18-byte frames with sum checksum  
3. **Mode 0x66** (Variant) - 18-byte frames, error at byte 17

Store successful mode for subsequent communication.

## Command Reference

### 0x01 - Status Request

**Purpose**: Request current heater status

**Request**:
```
AA 55 0C 22 01 00 00 2F
```

**Response**: Full status packet (see Response Format section)

**Update Frequency**: Send every 1-2 seconds for continuous monitoring

### 0x02 - Set Operational Mode

**Purpose**: Switch between manual level control and automatic temperature control

**Request**:
```
AA 55 0C 22 02 [MODE] 00 [CHECKSUM]
```

**Mode Values**:
- `0x01`: Manual level mode (power level 1-10)
- `0x02`: Automatic temperature mode (8-36°C)

**Example** (Switch to temperature mode):
```
AA 55 0C 22 02 02 00 30
```

### 0x03 - Power Control

**Purpose**: Turn heater on or off

**Request**:
```
AA 55 0C 22 03 [POWER] 00 [CHECKSUM]
```

**Power Values**:
- `0x00`: Turn OFF
- `0x01`: Turn ON

**Examples**:
- Turn ON: `AA 55 0C 22 03 01 00 32`
- Turn OFF: `AA 55 0C 22 03 00 00 31`

### 0x04 - Set Target Temperature or Level

**Purpose**: Set target temperature or power level (depends on current mode)

**Request**:
```
AA 55 0C 22 04 [VALUE] 00 [CHECKSUM]
```

**Value Ranges**:
- Level Mode: 1-10 (raw value, some heaters use 0-9)
- Temperature Mode: 8-36 (degrees Celsius)

**Examples**:
- Set Level 5: `AA 55 0C 22 04 05 00 35`
- Set Temperature 22°C: `AA 55 0C 22 04 16 00 4C`

### 0x13 - Automatic Start/Stop

**Purpose**: Enable/disable automatic scheduling

**Request**:
```
AA 55 0C 22 13 [ENABLE] 00 [CHECKSUM]
```

**Enable Values**:
- `0x00`: Disable
- `0x01`: Enable

### 0x14 - Set Language

**Purpose**: Change display language

**Request**:
```
AA 55 0C 22 14 [LANG] 00 [CHECKSUM]
```

**Language Codes**:
- `0x00`: Chinese
- `0x01`: English
- `0x02`: German
- `0x03`: French
- `0x04`: Italian
- `0x05`: Spanish

### 0x15 - Set Temperature Unit

**Purpose**: Switch between Celsius and Fahrenheit

**Request**:
```
AA 55 0C 22 15 [UNIT] 00 [CHECKSUM]
```

**Unit Values**:
- `0x00`: Fahrenheit
- `0x01`: Celsius

### 0x16 - Set Altitude Unit

**Purpose**: Switch altitude display between meters and feet

**Request**:
```
AA 55 0C 22 16 [UNIT] 00 [CHECKSUM]
```

**Unit Values**:
- `0x00`: Feet
- `0x01`: Meters

## Response Format

### Standard Response Packet (20 bytes)

```
Byte 0-1:   Header (0xAA 0x55 or 0xAA 0x66)
Byte 2:     Command Echo
Byte 3:     Power Status
Byte 4:     Error Code
Byte 5:     Running State
Byte 6-7:   Altitude (Little Endian)
Byte 8:     Operational Mode
Byte 9:     Target Temperature/Level
Byte 10:    Current Power Level
Byte 11-12: Supply Voltage (Little Endian, decivolts)
Byte 13-14: Heating Element Temp (Little Endian, °C)
Byte 15-16: Room Temperature (Little Endian, °C)
Byte 17:    Display Error Code
Byte 18:    Reserved (0x00)
Byte 19:    Checksum
```

### Extended Response (32 bytes) - Encrypted Models

Additional data in encrypted protocol variants:

```
Byte 20-21: Start Time
Byte 22-23: Auto Time
Byte 24-25: Run Time
Byte 26:    Auto Mode Enabled
Byte 27:    Language
Byte 28:    Temperature Offset
Byte 29:    Tank Volume
Byte 30:    Oil Pump Type
Byte 31:    Temperature Unit
Byte 32:    Altitude Unit
Byte 33:    Automatic Heating
```

## Data Field Specifications

### Power Status (Byte 3)

| Value | Meaning |
|-------|---------|
| 0x00  | OFF     |
| 0x01  | RUNNING |
| 0x02  | ERROR   |

### Error Codes (Byte 4)

| Code | Description                    |
|------|--------------------------------|
| 0x00 | No Error                       |
| 0x01 | Power Supply Undervoltage      |
| 0x02 | Power Supply Overvoltage       |
| 0x03 | Ignition Coil Failure          |
| 0x04 | Fuel Pump Failure              |
| 0x05 | High Temperature Alarm         |
| 0x06 | Fan Failure                    |
| 0x07 | Cable Damage                   |
| 0x08 | Combustion Failure             |
| 0x09 | Sensor Failure                 |
| 0x0A | Ignition Failure               |

### Running State (Byte 5)

| Value | State           | Description                        |
|-------|-----------------|------------------------------------|
| 0x00  | Warmup          | Initial heating phase              |
| 0x01  | Self Test       | Running diagnostics                |
| 0x02  | Ignition        | Attempting to ignite fuel          |
| 0x03  | Heating         | Normal operation, producing heat   |
| 0x04  | Shutting Down   | Cooling down sequence              |

### Operational Mode (Byte 8)

| Value | Mode                    |
|-------|-------------------------|
| 0x01  | Manual Level Mode       |
| 0x02  | Automatic Temperature   |

## Data Conversion Examples

### Voltage (Bytes 11-12)

```cpp
uint16_t raw = (byte12 << 8) | byte11;
float voltage = raw / 10.0;  // Convert decivolts to volts
// Typical range: 9.0V - 15.0V (for 12V heaters)
```

### Temperature (Bytes 13-16)

```cpp
uint16_t temp_raw = (byte14 << 8) | byte13;
int16_t temperature = static_cast<int16_t>(temp_raw);
// Temperatures can be negative in cold climates
```

### Altitude (Bytes 6-7)

```cpp
uint16_t altitude = (byte7 << 8) | byte6;
// Altitude in meters (or feet, depending on settings)
// Used for automatic fuel/air mixture adjustment
```

## Timing Requirements

### Request Intervals

- **Status Request**: 1-2 seconds (recommended)
- **Control Commands**: Minimum 100ms between commands
- **Mode Changes**: Wait for status confirmation before next command

### Response Timeout

- **Expected Response Time**: 50-200ms
- **Timeout Threshold**: 1000ms
- **Retry Strategy**: Maximum 3 retries with exponential backoff

## State Machine

### Heater Startup Sequence

```
OFF → WARMUP → SELF_TEST → IGNITION → HEATING
```

**Typical Duration**:
- Warmup: 10-30 seconds
- Self Test: 5-10 seconds
- Ignition: 20-60 seconds
- Heating: Until turned off

### Heater Shutdown Sequence

```
HEATING → SHUTTING_DOWN → OFF
```

**Typical Duration**:
- Shutting Down: 60-180 seconds (cooling fan runs)

## Error Handling

### Error Detection

1. Monitor error code byte (4)
2. Check display error code byte (17)
3. Validate checksum on all responses

### Recovery Procedures

| Error | Recovery Action |
|-------|----------------|
| Undervoltage | Check power supply, battery charge |
| Overvoltage | Check voltage regulator |
| Ignition Failure | Wait 5 minutes before retry |
| Combustion Failure | Check fuel level, fuel pump |
| Sensor Failure | Power cycle heater |

## Implementation Notes

### BLE Notifications

The heater sends notifications on characteristic `FFE1`:
- Subscribe to notifications for status updates
- Write commands to same characteristic
- Use write-with-response for reliability

### Connection Management

```cpp
// Recommended connection pattern
1. Scan for device
2. Connect to device
3. Discover services
4. Subscribe to notifications
5. Send status request
6. Process notifications
7. Send control commands as needed
```

### Multi-Device Handling

- Each heater maintains single BLE connection
- Disconnect other clients before connecting
- Connection timeout: 30 seconds
- Auto-reconnect on disconnect

## Testing Commands

### Basic Test Sequence

```bash
# 1. Request status
AA 55 0C 22 01 00 00 2F

# 2. Turn on
AA 55 0C 22 03 01 00 32

# 3. Set to temperature mode
AA 55 0C 22 02 02 00 30

# 4. Set temperature to 20°C
AA 55 0C 22 04 14 00 4A

# 5. Monitor status (repeat)
AA 55 0C 22 01 00 00 2F

# 6. Turn off
AA 55 0C 22 03 00 00 31
```

## References

- [warehog/esphome-diesel-heater-ble](https://github.com/warehog/esphome-diesel-heater-ble)
- [iotmaestro/vevor-heater-ble](https://github.com/iotmaestro/vevor-heater-ble)
- [spin877/Bruciatore_BLE](https://github.com/spin877/Bruciatore_BLE)
- BLE Specification v4.0+

## Changelog

### Protocol Version 1.0
- Initial documentation
- Support for AA55/AA66 variants
- Basic command set

### Protocol Version 2.0
- Added encrypted protocol support
- Extended status packet (32 bytes)
- Additional control commands
- Automatic protocol detection

---

*Last updated: November 2025*
