# Installation Guide

## Prerequisites

### Hardware
- **ESP32 Development Board** (ESP32, ESP32-C3, ESP32-S3, etc.)
- **Diesel/Air Heater** with Bluetooth support
- **USB Cable** for flashing ESP32
- **Power Supply** (can use heater's 12V with buck converter to 5V)

### Software
- Python 3.9 or later
- ESPHome
- Home Assistant (optional, but recommended)

### Knowledge
- Basic command line usage
- Basic YAML syntax
- How to find BLE MAC addresses

## Step-by-Step Installation

### 1. Install ESPHome

#### Option A: Using pip (Recommended)
```bash
# Install ESPHome
pip install esphome

# Verify installation
esphome version
```

#### Option B: Using Home Assistant Add-on
1. Open Home Assistant
2. Navigate to Settings → Add-ons
3. Click "Add-on Store"
4. Search for "ESPHome"
5. Click "Install"

### 2. Find Your Heater's MAC Address

#### Option A: Using Phone App
1. Install "BLE Scanner" or "nRF Connect" on your phone
2. Enable Bluetooth
3. Turn on your heater
4. Scan for BLE devices
5. Look for device name containing "Air", "Heater", or similar
6. Note the MAC address (format: XX:XX:XX:XX:XX:XX)

#### Option B: Using ESP32 Scanner
1. Flash ESP32 with BLE scanner firmware
2. Check logs for discovered devices
3. Identify your heater by signal strength when close to it

### 3. Set Up Your Project

#### Create Project Directory
```bash
mkdir diesel-heater
cd diesel-heater
```

#### Clone or Download This Repository
```bash
# Option A: Clone with git
git clone https://github.com/<your-repo>/esphome-diesel-heater-ble.git
cd esphome-diesel-heater-ble

# Option B: Download and extract ZIP
# Download from GitHub and extract to diesel-heater directory
```

#### Copy Example Configuration
```bash
# Copy the example configuration
cp examples/basic-config.yaml diesel-heater.yaml

# Copy secrets template
cp examples/secrets.yaml.template secrets.yaml
```

### 4. Configure Your Setup

#### Edit secrets.yaml
```bash
# Edit with your preferred editor
nano secrets.yaml
# or
code secrets.yaml
```

Update these values:
- `wifi_ssid`: Your WiFi network name
- `wifi_password`: Your WiFi password
- `api_encryption_key`: Generate with `openssl rand -base64 32`
- `ota_password`: Choose a secure password

#### Edit diesel-heater.yaml
```bash
nano diesel-heater.yaml
```

Key changes:
1. Replace `XX:XX:XX:XX:XX:XX` with your heater's MAC address
2. Adjust `board` setting if using different ESP32 model
3. Remove sensors you don't need (optional)
4. Adjust names/icons as desired

### 5. Compile and Flash

#### First-Time Flash (USB)
```bash
# Connect ESP32 via USB
# Compile and upload
esphome run diesel-heater.yaml

# Select the serial port when prompted
# Example: /dev/ttyUSB0 (Linux) or COM3 (Windows)
```

#### Check for Errors
If compilation fails:
1. Check YAML syntax (indentation matters!)
2. Verify all required fields are filled
3. Check ESPHome version compatibility
4. Review error messages carefully

### 6. Configure Home Assistant (Optional)

#### Auto-Discovery
1. Open Home Assistant
2. Navigate to Settings → Devices & Services
3. Look for "Discovered" ESPHome device
4. Click "Configure"
5. Enter the API encryption key from secrets.yaml
6. Click "Submit"

#### Manual Addition
1. Settings → Devices & Services → Add Integration
2. Search for "ESPHome"
3. Enter device IP address or hostname (diesel-heater.local)
4. Enter API encryption key
5. Click "Submit"

### 7. Verify Connection

#### Check Logs
```bash
# View live logs
esphome logs diesel-heater.yaml

# Look for:
# - "Connected to heater"
# - Regular status updates
# - No error messages
```

#### Test in Home Assistant
1. Find "Diesel Heater" in Devices
2. Try turning power on/off
3. Adjust temperature/level
4. Monitor sensors for updates

## Hardware Installation

### Powering the ESP32

#### Option 1: Separate Power Supply
- Use 5V USB power adapter
- Simplest and safest method
- Requires separate power source

#### Option 2: From Heater's 12V Supply
**Components needed:**
- Buck converter (12V to 5V)
- Example: LM2596 module
- Fuse (2A recommended)

**Wiring:**
```
Heater 12V+ → Fuse → Buck Converter IN+
Heater GND  → Buck Converter IN-

Buck Converter OUT+ → ESP32 5V
Buck Converter OUT- → ESP32 GND
```

**⚠️ Safety:**
- Use fuse to protect ESP32
- Verify voltage with multimeter before connecting
- Ensure good connections to prevent voltage spikes

### Mounting

1. **Location**: Keep ESP32 away from extreme heat
2. **Enclosure**: Use plastic case to protect from dust/moisture
3. **Antenna**: Position for good WiFi signal
4. **Access**: Leave USB port accessible for updates

### Range Considerations

- **BLE Range**: Typically 10-30 feet (3-10 meters)
- **Obstacles**: Walls, metal reduce range
- **Position ESP32**: Close to heater for reliable connection
- **WiFi Range**: Ensure good WiFi signal where ESP32 is mounted

## Troubleshooting

### ESP32 Won't Flash

**Problem**: Can't connect to serial port

**Solutions:**
1. Install CH340/CP210x USB drivers
2. Try different USB cable (data cable, not charge-only)
3. Hold BOOT button while connecting
4. Check `board` setting matches your ESP32

### Heater Won't Connect

**Problem**: BLE connection fails

**Solutions:**
1. Verify MAC address is correct
2. Ensure heater Bluetooth is enabled
3. Check ESP32 is within range
4. Disconnect other devices from heater
5. Power cycle both ESP32 and heater

### Wrong Values Displayed

**Problem**: Sensors show incorrect data

**Solutions:**
1. Check heater protocol variant in logs
2. Different models may use different formats
3. Report issue with your heater model
4. Enable debug logging to capture raw data

### Connection Drops

**Problem**: Frequent disconnections

**Solutions:**
1. Reduce distance between ESP32 and heater
2. Check power supply stability
3. Reduce BLE scan interval
4. Check for WiFi interference (change channel)

### Home Assistant Won't Connect

**Problem**: Can't add ESPHome device

**Solutions:**
1. Verify ESP32 is on same network as HA
2. Check firewall settings
3. Verify API encryption key is correct
4. Try manual addition with IP address
5. Check ESPHome add-on logs

## OTA Updates

After initial USB flash, you can update over WiFi:

```bash
# Compile and upload over WiFi
esphome run diesel-heater.yaml

# Select "Over The Air" option
# Choose your device from the list
```

Or update through Home Assistant:
1. Open ESPHome dashboard
2. Click "Update" on your device
3. Wait for compilation and upload

## Advanced Configuration

### Custom Update Interval

```yaml
diesel_heater_ble:
  - ble_client_id: heater_ble_client
    id: my_heater
    update_interval: 2s  # Default is 1s
```

### Enable Deep Debugging

```yaml
logger:
  level: VERBOSE
  logs:
    diesel_heater_ble: VERBOSE
    ble_client: DEBUG
    esp32_ble: DEBUG
```

### Custom Automations

See `examples/basic-config.yaml` for automation examples:
- Time-based heating schedules
- Temperature-based control
- Safety shutoffs
- Error notifications

## Backup Your Configuration

```bash
# Backup your config files
cp diesel-heater.yaml diesel-heater.yaml.backup
cp secrets.yaml secrets.yaml.backup

# Or use git
git init
git add diesel-heater.yaml secrets.yaml
git commit -m "Working configuration"
```

## Getting Help

### Check Logs First
```bash
esphome logs diesel-heater.yaml
```

### Documentation
- ESPHome: https://esphome.io
- This project: README.md and doc/PROTOCOL.md
- Home Assistant: https://www.home-assistant.io

### Community Support
- GitHub Issues: Report bugs or ask questions
- Home Assistant Forum: ESPHome section
- Discord: ESPHome Discord server

## Next Steps

1. **Customize**: Adjust sensors and controls to your needs
2. **Automate**: Create automations in Home Assistant
3. **Monitor**: Set up alerts for errors or issues
4. **Optimize**: Tune update intervals and thresholds
5. **Share**: Help others by documenting your heater model

## Security Notes

- Change default OTA password
- Use strong WiFi password
- Keep firmware updated
- Don't expose ESP32 directly to internet
- Use Home Assistant's authentication

## Maintenance

### Regular Tasks
- Check logs for errors (weekly)
- Verify sensors are accurate (monthly)
- Update ESPHome (when new version available)
- Backup configuration (before major changes)

### Heater Maintenance
- Follow manufacturer's maintenance schedule
- Clean/replace fuel filter regularly
- Check fuel quality
- Inspect glow plug and combustion chamber

---

**Ready to start?** Follow the steps above and you'll have your heater connected in about 30 minutes!

Need help? Open an issue on GitHub or check the troubleshooting section.
