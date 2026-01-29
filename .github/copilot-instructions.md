# ESPHome Diesel Heater BLE - Combined Project

This workspace combines multiple diesel/air heater BLE control projects:
- warehog/esphome-diesel-heater-ble
- iotmaestro/vevor-heater-ble  
- spin877/Bruciatore_BLE
- AirHeaterBLE APK project

## Project Checklist

- [x] Verify that the copilot-instructions.md file in the .github directory is created.
- [x] Fetch and analyze GitHub repositories
- [x] Examine local APK project structure
- [x] Design combined project structure
- [x] Create ESPHome configuration files
- [x] Create documentation and README

## Project Status: ✅ COMPLETE

All project files have been successfully created and organized.

## Project Structure

```
heater/
├── .github/
│   └── copilot-instructions.md
├── components/
│   └── diesel_heater_ble/
│       └── __init__.py
├── doc/
│   ├── PROTOCOL.md          # Comprehensive BLE protocol documentation
│   ├── INSTALLATION.md      # Step-by-step installation guide
│   └── FAQ.md               # Frequently asked questions
├── examples/
│   ├── basic-config.yaml    # Complete example configuration
│   └── secrets.yaml.template # Template for secrets
├── README.md                 # Main project documentation
├── LICENSE                   # MIT License with attributions
├── CONTRIBUTING.md           # Contribution guidelines
└── CHANGELOG.md              # Version history
```

## Project Guidelines

This is an ESPHome-based project for controlling diesel/air heaters via Bluetooth Low Energy (BLE).

### Key Features
- Multi-protocol support (AA55, AA66, encrypted variants)
- Automatic protocol detection
- Comprehensive sensor and control integration
- Home Assistant ready
- Extensive documentation

### Core Technologies
- ESPHome framework
- ESP32 Bluetooth Low Energy
- Home Assistant integration
- YAML configuration

### Documentation Highlights
- Complete BLE protocol specification with command reference
- Step-by-step installation guide with troubleshooting
- FAQ covering 40+ common questions
- Example configurations for various use cases
- Safety guidelines and best practices

### Usage Instructions
1. Review README.md for project overview
2. Follow doc/INSTALLATION.md for setup
3. Use examples/basic-config.yaml as starting point
4. Refer to doc/PROTOCOL.md for protocol details
5. Check doc/FAQ.md for common questions

### Safety Notice
⚠️ This software controls potentially dangerous heating equipment. Users must:
- Follow manufacturer safety instructions
- Ensure proper ventilation
- Install CO and smoke detectors
- Never leave heater unattended
- Use at their own risk
