# BLE Protocol Implementation Update

**Date**: November 27, 2025  
**Source**: JavaScript analysis of AirHeaterBLE_1.08 APK (app-service.js)

## Summary

Comprehensive reverse engineering of the official AirHeaterBLE Android app revealed complete BLE protocol implementation details, including CRC16 algorithm, extended mode features, and all data parsing logic. This update enhances the ESPHome component with full multi-protocol support.

## Key Discoveries

### 1. Three Protocol Modes

| Mode | Header | Frame Size | Checksum | Features |
|------|--------|------------|----------|----------|
| **1 (AA55)** | 0xAA 0x55 | 18 bytes | Simple sum | Basic control |
| **2 (AA88)** | 0xAA 0x88 | 32 bytes | CRC16 Modbus | Extended features |
| **3 (AA66)** | 0xAA 0x66 | 18 bytes | Simple sum | Variant (error at byte 17) |

### 2. CRC16 Implementation

**Algorithm**: Modbus CRC with polynomial 0xA001

```cpp
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
// Byte swap
return ((crc & 0xFF00) >> 8) | ((crc & 0x00FF) << 8);
```

### 3. Password Mechanism

- **Default**: 1234 (integer)
- **Encoding**: Split into two bytes
  - Byte 1: `Math.floor(passkey/100)` → 12 → 0x0C
  - Byte 2: `passkey % 100` → 34 → 0x22
- **Storage**: localStorage in app, configurable by user

### 4. Extended Mode Features (32-byte frames)

Additional parameters beyond basic mode:

1. **Auto Time** (bytes 17-18): Start time in minutes since midnight
2. **Runtime** (bytes 19-20): Duration in minutes
3. **Is Auto** (byte 21): Auto mode enabled
4. **Language** (byte 22): 0=EN, 1=ZH, 2=RU, 3=Disabled
5. **Temp Offset** (byte 23): Calibration -10 to +10°C
6. **Tank Volume** (byte 24): 0-50 liters
7. **Oil Pump Type** (byte 25): 0=16ul, 1=22ul, 2=28ul, 3=32ul
8. **Bluetooth Switch** (byte 26): 0=Enabled (inverted)
9. **Remote Matching** (byte 27): 0=Paired (inverted)
10. **Auto Heating** (byte 28): Temperature-based auto start/stop

### 5. Random Encryption Bytes

Extended mode uses random bytes for obfuscation:
- Generated: `Math.floor(Math.random() * 255)` for bytes 2-3
- Purpose: Basic obfuscation, not cryptographic security
- CRC Validation: Replace bytes 29-30 with random bytes before calculating CRC

## Files Updated

### 1. doc/APK_RESEARCH.md
✅ Added complete "JavaScript Protocol Implementation" section
- Frame construction function
- CRC16 algorithm
- Protocol modes explanation
- Password mechanism
- Extended mode features
- Response frame parsing
- Data conversion utilities
- Implementation recommendations

### 2. doc/PROTOCOL.md
✅ Enhanced with extended mode documentation
- CRC16 checksum function
- Extended mode frame structure (32 bytes)
- All extended parameter definitions
- Random encryption byte handling
- Protocol mode detection order

### 3. components/diesel_heater_ble/diesel_heater_ble.cpp
✅ Implemented complete protocol support
- `calculate_crc16_()` function with Modbus polynomial
- Enhanced `handle_notify_()` with all three protocol modes
- Mode 1 (AA55): 18-byte basic frame parsing
- Mode 2 (AA88): 32-byte extended frame with CRC16 validation
- Mode 3 (AA66): Variant mode with error at byte 17
- Proper entity state publishing from parsed data

### 4. components/diesel_heater_ble/diesel_heater_ble.h
✅ Added protocol tracking
- `calculate_crc16_()` declaration
- `protocol_mode_` state variable (0=unknown, 1=AA55, 2=AA88, 3=AA66)
- `last_update_` timestamp for connection monitoring

### 5. CHANGELOG.md
✅ Documented all changes
- Added section for protocol implementation updates
- Listed all new features and enhancements
- Documented planned improvements

## Implementation Status

### ✅ Complete
- [x] JavaScript analysis and documentation
- [x] CRC16 implementation
- [x] Multi-protocol support (AA55, AA66, AA88)
- [x] Basic response frame parsing
- [x] Protocol mode detection
- [x] Entity state publishing

### ⚠️ Partial
- [ ] Extended mode parameters (only documented, not exposed as entities)
- [ ] Additional sensors (temperature, voltage, error codes)
- [ ] Auto-start configuration entities

### ❌ Not Started
- [ ] Time entities for auto-start scheduling
- [ ] Binary sensors for switches
- [ ] Configuration flow for extended parameters
- [ ] Hardware testing with all protocol modes

## Testing Requirements

### Protocol Mode Testing
1. **Mode 1 (AA55)**: Test with basic heaters
2. **Mode 2 (AA88)**: Test with advanced heaters supporting extended features
3. **Mode 3 (AA66)**: Test with variant heaters

### CRC16 Validation
- Verify CRC16 calculation matches JavaScript implementation
- Test with known good frames from packet capture
- Validate random byte handling

### Extended Parameters
- Test auto-start time setting and retrieval
- Verify temperature offset calibration
- Test language selection
- Validate tank volume and oil pump type configuration

## Next Steps

### Priority 1: Core Functionality
1. ✅ Document JavaScript findings
2. ✅ Implement CRC16 checksum
3. ✅ Add multi-protocol support
4. Expose extended mode parameters as entities

### Priority 2: Enhanced Features
1. Add temperature sensors (cabin, case)
2. Add voltage sensor
3. Add error code sensor with text mapping
4. Add altitude sensor

### Priority 3: Configuration
1. Create time entities for auto-start
2. Add binary sensors for bluetooth/remote/auto-heating
3. Implement configuration flow for extended parameters
4. Add language selection

### Priority 4: Testing & Validation
1. Hardware testing with real heaters
2. Validate all three protocol modes
3. Test CRC16 with extended mode heaters
4. Verify all extended parameters

## References

### Source Files
- **APK**: AirHeaterBLE_1.08_APKPure/com.clj.airheater
- **JavaScript**: assets/apps/__UNI__1471D01/www/app-service.js (171KB)
- **Native Libraries**: config.arm64_v8a/lib/arm64-v8a/*.so

### Documentation
- APK_RESEARCH.md: Complete JavaScript analysis
- PROTOCOL.md: Enhanced protocol specification
- COMPATIBILITY.md: Hardware compatibility matrix
- FAQ.md: Troubleshooting and common questions

### Implementation
- diesel_heater_ble.cpp: Full BLE component implementation
- diesel_heater_ble.h: Component header with protocol tracking
- *.py: Python platform wrappers

## Conclusion

The JavaScript analysis provided complete protocol specifications, eliminating guesswork and enabling full implementation of all heater features. The ESPHome component now supports all three protocol modes with proper CRC16 validation and extended parameter parsing.

**Key Achievement**: Reversed engineered the complete official protocol from the Android app, providing a solid foundation for 100% feature parity with the official AirHeaterBLE application.

**Status**: Core protocol implementation complete, ready for entity expansion and hardware testing.
