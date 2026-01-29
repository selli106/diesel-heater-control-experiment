# Frequently Asked Questions (FAQ)

## General Questions

### What heaters are compatible?

This component has been tested with:
- **VEVOR** diesel air heaters (All-in-One model, 5kW-8kW)
- **Generic Chinese heaters** with BLE (various brands)
- **Encrypted protocol heaters** (detected automatically)

Compatibility indicators:
- ✅ Has Bluetooth/BLE
- ✅ Uses "AirHeaterBLE" or similar Android app
- ✅ Service UUID: `0000FFE0-...-00805F9B34FB`
- ✅ Characteristic UUID: `0000FFE1-...-00805F9B34FB`

### Do I need programming knowledge?

No programming required! You only need to:
1. Copy and edit YAML configuration files
2. Follow step-by-step installation guide
3. Basic understanding of WiFi and Bluetooth

### Can I use this without Home Assistant?

Yes! The ESP32 works standalone:
- Web interface at `http://diesel-heater.local`
- MQTT support for other home automation
- REST API for custom integrations

However, Home Assistant provides the best experience.

### Is this safe?

**Technical safety**: Yes, the component only reads/writes standard BLE commands

**Heater safety**: Follow these guidelines:
- Never leave heater unattended
- Ensure proper ventilation
- Follow manufacturer safety instructions
- Install CO detector
- Keep fire extinguisher nearby

**This software is provided as-is with no warranty**

### Will this void my warranty?

- Component only communicates via Bluetooth
- No physical modifications required
- Same commands as official app
- However, check your warranty terms

## Technical Questions

### What's the difference between this and the official app?

**Advantages:**
- ✅ Home automation integration
- ✅ Scheduled operation
- ✅ Remote access (with HA)
- ✅ Sensor logging and history
- ✅ Complex automations
- ✅ Works with voice assistants

**Limitations:**
- ❌ No firmware updates (use official app)
- ❌ Some advanced settings might not be exposed

### How much does the ESP32 consume?

Typical power consumption:
- **Active BLE**: 80-160mA @ 5V (0.4-0.8W)
- **WiFi connected**: 120-240mA @ 5V (0.6-1.2W)
- **Deep sleep**: Not recommended (loses connection)

Daily energy: ~15-30 Wh (negligible compared to heater)

### What's the range?

**Bluetooth**: 
- Indoor: 10-30 feet (3-10m)
- Outdoor: up to 100 feet (30m) line-of-sight
- Obstacles reduce range significantly

**WiFi control**: Unlimited (works anywhere you have internet with HA)

### Can multiple ESP32s connect to one heater?

**No**. Bluetooth Low Energy allows only one connection at a time.

You can:
- Have multiple heaters, each with its own ESP32
- Switch between ESP32 and official app (disconnect one first)

### Does this work with my phone/tablet?

Not directly. The ESP32 connects to the heater, then you control it via:
- Home Assistant mobile app
- Web browser (built-in web server)
- Voice assistants (via Home Assistant)

## Setup Questions

### How do I find the MAC address?

**Method 1: Phone App**
1. Install "BLE Scanner" (Android) or "nRF Connect" (iOS/Android)
2. Turn on heater
3. Scan for devices
4. Look for "Air Heater" or strong signal device
5. Note MAC address (XX:XX:XX:XX:XX:XX)

**Method 2: ESPHome Scanner**
1. Flash ESP32 with BLE scanner code
2. Check logs for nearby devices
3. Identify by signal strength

### What if I don't know my WiFi password?

Options:
1. Check your router settings page
2. Use Ethernet (if supported by ESP32 model)
3. Configure WiFi using fallback AP mode
4. Use USB for initial setup, configure WiFi later

### Can I use Ethernet instead of WiFi?

Some ESP32 boards support Ethernet:
- ESP32 with LAN8720 module
- ESP32 PoE boards
- WT32-ETH01

Configuration:
```yaml
# Replace wifi: section with:
ethernet:
  type: LAN8720
  mdc_pin: GPIO23
  mdio_pin: GPIO18
  clk_mode: GPIO0_IN
  phy_addr: 1
  power_pin: GPIO16
```

### Do I need the official app at all?

**Initial setup**: Official app useful for:
- Firmware updates
- Advanced configuration
- Pairing new remote controls

**Daily use**: ESPHome can replace official app completely

**Best practice**: Keep official app for occasional maintenance

## Functionality Questions

### Can I set schedules?

Yes! Multiple ways:

**Method 1: ESPHome automation**
```yaml
time:
  - platform: homeassistant
    on_time:
      - hours: 6
        then:
          - switch.turn_on: heater_power
```

**Method 2: Home Assistant automation**
```yaml
automation:
  - alias: "Morning heat"
    trigger:
      platform: time
      at: "06:00:00"
    action:
      service: switch.turn_on
      entity_id: switch.heater_power
```

### Can I control it with voice?

Yes, via Home Assistant:
- "Alexa, turn on the heater"
- "Hey Google, set heater to 22 degrees"
- Apple HomeKit support with HA bridge

### What if power goes out?

**ESP32 behavior**:
- Reboots when power returns
- Reconnects to WiFi automatically
- Reconnects to heater automatically
- Restores last state (if configured)

**Heater behavior**:
- Follows heater's built-in power loss handling
- Usually stays off for safety
- Some models remember last state

### Can I monitor fuel level?

**Directly**: No, most heaters don't have fuel level sensors

**Alternatives**:
- Calculate from runtime and fuel consumption rate
- External ultrasonic sensor
- Smart fuel monitor (separate project)

### Does it log data?

**Short-term**: ESPHome keeps recent sensor history

**Long-term**: Home Assistant records:
- All sensor values
- State changes
- Historical graphs
- Can export to database

## Troubleshooting

### "Failed to connect to heater"

**Check:**
1. ✅ MAC address is correct
2. ✅ Heater Bluetooth is on
3. ✅ ESP32 is within range
4. ✅ No other device connected
5. ✅ ESP32 has power
6. ✅ No Bluetooth interference

**Try:**
- Power cycle heater
- Restart ESP32
- Disconnect official app
- Move ESP32 closer

### "Invalid responses" or "Checksum errors"

**Cause**: Wrong protocol variant or interference

**Solutions:**
1. Check logs for detected heater class
2. Enable debug logging
3. Report your heater model
4. Try moving away from interference sources

### Values stuck at 0 or not updating

**Check:**
1. Heater is turned on
2. Connection is established
3. Logs show regular status requests
4. No error codes

**Try:**
- Restart ESP32
- Turn heater off and on
- Check for firmware updates

### "Disconnects frequently"

**Common causes:**
- Poor signal strength → Move ESP32 closer
- Power supply issues → Use stable power source
- WiFi interference → Change WiFi channel
- Too many devices → Reduce BLE scan interval

### Home Assistant shows "Unavailable"

**Network issues:**
1. Check ESP32 is on network
2. Ping the device IP
3. Check firewall settings
4. Verify WiFi hasn't changed

**API issues:**
1. Correct encryption key
2. API enabled in config
3. No port conflicts
4. ESPHome add-on running

## Customization

### Can I change sensor names?

Yes! Edit configuration:
```yaml
sensor:
  - platform: diesel_heater_ble
    cab_temp:
      name: "Living Room Temperature"  # Custom name
```

### Can I disable sensors I don't need?

Yes! Simply remove or comment out:
```yaml
sensor:
  - platform: diesel_heater_ble
    cab_temp:
      name: "Room Temperature"
    # case_temp:  # This sensor is disabled
    #   name: "Case Temperature"
```

### Can I add custom automations?

Absolutely! Examples:

**Frost protection:**
```yaml
automation:
  - alias: "Frost protection"
    trigger:
      platform: numeric_state
      entity_id: sensor.room_temperature
      below: 5
    action:
      - service: switch.turn_on
        entity_id: switch.heater_power
      - service: number.set_value
        data:
          entity_id: number.target_temperature
          value: 8
```

**Error notifications:**
```yaml
automation:
  - alias: "Heater error alert"
    trigger:
      platform: state
      entity_id: sensor.heater_error_code
    condition:
      condition: template
      value_template: "{{ trigger.to_state.state | int > 0 }}"
    action:
      service: notify.mobile_app
      data:
        message: "Heater error: {{ states('sensor.error_description') }}"
```

### Can I create a thermostat-style control?

Yes! Use climate platform (coming soon) or create dashboard cards:
- Temperature display
- Target temperature slider
- On/Off toggle
- Current status

## Advanced Topics

### Can I use MQTT instead of API?

Yes! Add to configuration:
```yaml
mqtt:
  broker: YOUR_MQTT_BROKER
  username: !secret mqtt_username
  password: !secret mqtt_password
```

All entities auto-publish to MQTT topics.

### Can I use multiple heaters?

Yes! Example:
```yaml
ble_client:
  - mac_address: "AA:BB:CC:DD:EE:01"
    id: heater1_client
  - mac_address: "AA:BB:CC:DD:EE:02"
    id: heater2_client

diesel_heater_ble:
  - ble_client_id: heater1_client
    id: garage_heater
  - ble_client_id: heater2_client
    id: workshop_heater
```

**Note**: Each needs its own ESP32!

### Can I integrate with other platforms?

Via Home Assistant:
- ✅ Node-RED
- ✅ OpenHAB
- ✅ Domoticz
- ✅ IFTTT
- ✅ Zapier

Direct (advanced):
- REST API exposed by ESPHome
- MQTT for any MQTT-compatible system

### How do I update the component?

**From GitHub:**
```bash
cd components
git pull origin main
esphome run diesel-heater.yaml
```

**Manual:**
1. Download updated component files
2. Replace in `components/` folder
3. Recompile and upload

### Can I contribute?

Yes, please! Ways to help:
1. 📝 Test with different heater models
2. 🐛 Report issues
3. 💡 Suggest features  
4. 📚 Improve documentation
5. 💻 Submit code improvements
6. ⭐ Star the repository

## Safety & Legal

### Is this legal?

**Yes**, in most jurisdictions:
- Uses standard Bluetooth protocol
- No encryption bypassing
- Same commands as official app
- Reverse engineering for interoperability

**However**: Check local laws regarding:
- Radio equipment modifications
- IoT device regulations
- Warranty implications

### What if something goes wrong?

**Software issues**: 
- Heater has built-in safety systems
- Component can't override safety features
- Worst case: Disconnect ESP32, use official app

**Hardware issues**:
- Use quality power supplies
- Follow electrical safety
- Don't modify heater itself

### Privacy concerns?

**Data stays local:**
- No cloud services required
- All data processed on your network
- Home Assistant can be fully local

**Best practices:**
- Don't expose to internet without security
- Use strong passwords
- Enable Home Assistant authentication
- Regular security updates

### Insurance and liability?

**Important:**
- This is a DIY modification
- Check your insurance policy
- Document your setup
- Follow safety guidelines
- Install proper detectors (CO, smoke)

**Author disclaims liability** - use at your own risk!

## Still Have Questions?

1. 📖 Check documentation in `doc/` folder
2. 🔍 Search existing GitHub issues
3. 💬 Ask on Home Assistant community forum
4. 🐛 Open a new GitHub issue
5. 📧 Contact maintainers

---

**Updated**: November 2025  
**Version**: 1.0.0
