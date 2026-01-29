# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added
- Complete BLE protocol reverse engineering from AirHeaterBLE APK JavaScript analysis
- CRC16 Modbus checksum implementation for extended mode (0x88)
- Full response frame parsing for all three protocol modes (AA55, AA66, AA88)
- Protocol mode detection and tracking (1=AA55, 2=AA88, 3=AA66)
- JavaScript protocol implementation documentation in APK_RESEARCH.md
- Extended mode features: auto-start scheduling, temperature calibration, tank/pump configuration
- Enhanced PROTOCOL.md with CRC16 algorithm and 32-byte frame specifications

### Changed
- Updated C++ BLE component with multi-protocol support and CRC16 validation
- Improved frame parsing with proper little-endian conversion
- Enhanced entity state publishing from parsed response frames

### Planned
- Climate entity support for easier temperature control
- Additional sensors for extended mode parameters (altitude, temps, voltage)
- Fuel level estimation based on runtime
- Advanced diagnostics and health monitoring
- Multi-heater coordination features

## [1.0.0] - 2025-11-27

### Added - Initial Release

#### Core Features
- **Multi-Protocol Support**: Automatic detection of AA55, AA66, and encrypted variants
- **ESPHome Integration**: Full native ESPHome external component
- **Comprehensive Control**: Power, temperature, heating level, and operational modes
- **Sensor Monitoring**: 25+ sensors for complete heater monitoring
- **Home Assistant Integration**: Seamless integration with automatic discovery

#### Sensors
- Running state, error codes, and operational status
- Temperature monitoring (room, case, heating element)
- Supply voltage monitoring
- Runtime statistics (start time, auto time, total runtime)
- Configuration sensors (language, units, modes)
- Altitude and environmental sensors

#### Controls
- Power switch (on/off)
- Power level control (1-10)
- Target temperature control (8-36°C)
- Level up/down buttons
- Temperature up/down buttons

#### Documentation
- Comprehensive README with quick start guide
- Detailed protocol documentation (PROTOCOL.md)
- Step-by-step installation guide (INSTALLATION.md)
- Extensive FAQ covering common questions
- Contributing guidelines
- Example configurations

#### Protocol Implementation
- BLE communication with service UUID FFE0 and characteristic FFE1
- Request/response packet handling
- Checksum validation
- Automatic protocol variant detection
- Encryption support for encrypted protocol variants
- Error handling and recovery

#### Compatibility
Based on combined research from:
- **warehog/esphome-diesel-heater-ble**: ESPHome implementation
- **iotmaestro/vevor-heater-ble**: VEVOR protocol documentation
- **spin877/Bruciatore_BLE**: Python BLE implementation and protocol analysis
- **AirHeaterBLE APK**: Android app protocol research

#### Tested Heater Models
- VEVOR Diesel Air Heaters (All-in-One model, 5kW-8kW)
- Generic Chinese diesel heaters with BLE
- Various encrypted and non-encrypted protocol variants

### Technical Details

#### BLE Protocol
- Service UUID: `0000FFE0-0000-1000-8000-00805F9B34FB`
- Characteristic UUID: `0000FFE1-0000-1000-8000-00805F9B34FB`
- Default password: `0x0C 0x22` (12, 34 in decimal)
- Update interval: 1 second (configurable)

#### Command Set
- `0x01`: Status request
- `0x02`: Set operational mode
- `0x03`: Power control
- `0x04`: Set temperature/level
- `0x13`: Automatic start/stop
- `0x14`: Set language
- `0x15`: Set temperature unit
- `0x16`: Set altitude unit

#### Response Format
- Standard: 20 bytes
- Extended (encrypted models): 33 bytes
- Includes power status, temperatures, error codes, and configuration

### Safety Features
- Error code monitoring and reporting
- Connection status tracking
- Automatic reconnection on disconnect
- Checksum validation for all communications

### Known Limitations
- Single BLE connection only (cannot use with official app simultaneously)
- No firmware update capability (use official app)
- BLE range limitations (10-30 feet typical)
- Some advanced heater settings not exposed

### Requirements
- ESP32 board with Bluetooth capability
- ESPHome 2023.x or later
- Home Assistant 2023.x or later (optional but recommended)
- Diesel/air heater with BLE support

## Version History

### [1.0.0] - 2025-11-27
- Initial combined release
- Merged protocols from multiple sources
- Complete documentation
- Full ESPHome component implementation

---

## Release Notes

### For Users

**What's New in 1.0.0:**
This is the first stable release combining protocols and implementations from multiple diesel heater BLE projects. It provides comprehensive control and monitoring of BLE-enabled diesel heaters through ESPHome and Home Assistant.

**Upgrading:**
This is the initial release. No upgrade path needed.

**Breaking Changes:**
None - this is the first release.

**Important Notes:**
- Always keep the official heater app for firmware updates
- Ensure proper ventilation when using diesel heaters
- Follow all manufacturer safety guidelines
- This software comes with no warranty - use at your own risk

### For Developers

**Architecture:**
- ESPHome external component structure
- C++ core implementation with Python configuration
- Automatic protocol detection and variant handling
- Extensible design for adding new protocols

**Testing:**
- Tested on ESP32 DevKit boards
- Verified with VEVOR and generic Chinese heaters
- Tested in Home Assistant 2023.x environment

**Next Steps:**
- Add climate entity support
- Implement fuel level estimation
- Expand protocol variant coverage
- Improve diagnostic capabilities

---

## Links

- **Repository**: https://github.com/YOUR-REPO/esphome-diesel-heater-ble
- **Documentation**: See `doc/` folder
- **Issues**: https://github.com/YOUR-REPO/esphome-diesel-heater-ble/issues
- **Discussions**: https://github.com/YOUR-REPO/esphome-diesel-heater-ble/discussions

## Contributors

### Original Project Authors
- **warehog**: ESPHome implementation and protocol analysis
- **iotmaestro**: VEVOR protocol documentation
- **spin877**: Python BLE implementation

### Combined Project
- Integration and documentation
- Protocol combination
- ESPHome component refinement

### Special Thanks
- ESPHome community
- Home Assistant community
- All testers and contributors

---

## Semantic Versioning

We follow [Semantic Versioning](https://semver.org/):

- **MAJOR**: Incompatible API changes
- **MINOR**: New functionality (backward compatible)
- **PATCH**: Bug fixes (backward compatible)

---

*For detailed changes, see individual commit messages and pull requests.*
