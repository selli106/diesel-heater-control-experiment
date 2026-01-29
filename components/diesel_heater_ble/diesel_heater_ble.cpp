#include "diesel_heater_ble.h"
#include "esphome/core/log.h"

namespace esphome {
namespace diesel_heater_ble {

static const char *const TAG = "diesel_heater_ble";

void DieselHeaterBLE::dump_config() {
  ESP_LOGCONFIG(TAG, "Diesel Heater BLE");
  ESP_LOGCONFIG(TAG, "  Service UUID: 0x%04X", this->service_uuid_);
  ESP_LOGCONFIG(TAG, "  Char UUID:    0x%04X", this->char_uuid_);
  ESP_LOGCONFIG(TAG, "  Requested protocol: %d", static_cast<int>(this->requested_protocol_));
}

void DieselHeaterBLE::on_ble_client_ready() {
  this->subscribe_();
  this->detect_protocol_();
}

void DieselHeaterBLE::detect_protocol_() {
  if (this->requested_protocol_ != ProtocolVariant::AUTO) {
    this->protocol_ = this->requested_protocol_;
    return;
  }
  this->protocol_ = ProtocolVariant::AA55;
}

uint8_t DieselHeaterBLE::checksum_(const uint8_t *buf) const {
  uint32_t sum = 0;
  for (int i = 2; i <= 6; i++) sum += buf[i];
  return static_cast<uint8_t>(sum & 0xFF);
}

bool DieselHeaterBLE::subscribe_() {
  auto *svc = this->parent_->get_service(this->service_uuid_);
  if (svc == nullptr) return false;
  auto *chr = svc->get_characteristic(this->char_uuid_);
  if (chr == nullptr) return false;
  this->write_char_ = chr;
  bool ok = chr->subscribe([this](const std::vector<uint8_t> &data) { this->handle_notify_(data); });
  ESP_LOGD(TAG, "Subscribe %s", ok ? "ok" : "fail");
  return ok;
}

bool DieselHeaterBLE::write_char_(const std::vector<uint8_t> &data) {
  if (!this->write_char_.has_value()) return false;
  return (*this->write_char_)->write_value(data);
}

void DieselHeaterBLE::send_frame_(uint8_t cmd, uint8_t p1, uint8_t p2, uint8_t p3) {
  uint8_t buf[8];
  buf[0] = 0xAA;
  buf[1] = (this->protocol_ == ProtocolVariant::AA66) ? 0x66 : 0x55;
  buf[2] = cmd;
  buf[3] = p1;
  buf[4] = p2;
  buf[5] = p3;
  buf[6] = this->password_[0] ^ this->password_[1];
  buf[7] = this->calculate_checksum_(std::vector<uint8_t>(buf, buf + 7));
  this->write_char_(std::vector<uint8_t>(buf, buf + 8));
}

uint8_t DieselHeaterBLE::calculate_checksum_(const std::vector<uint8_t> &frame) {
  // Simple sum checksum for basic protocol (mode 0x55/0x66)
  uint8_t sum = 0;
  for (size_t i = 2; i < frame.size(); i++) {
    sum += frame[i];
  }
  return sum;
}

uint16_t DieselHeaterBLE::calculate_crc16_(const std::vector<uint8_t> &data, uint16_t length) {
  // CRC16 with polynomial 0xA001 (Modbus) for extended mode (0x88)
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
  // Swap bytes for little-endian transmission
  return ((crc & 0xFF00) >> 8) | ((crc & 0x00FF) << 8);
}

void DieselHeaterBLE::handle_notify_(const std::vector<uint8_t> &data) {
  if (data.size() < 8) {
    ESP_LOGW(TAG, \"Frame too short: %d bytes\", data.size());
    return;
  }

  // Check header
  if (data[0] != 0xAA) {
    ESP_LOGW(TAG, \"Invalid header: 0x%02X\", data[0]);
    return;
  }

  uint8_t mode = data[1];
  
  // Mode 1: Basic AA55 (18 bytes)
  if (mode == 0x55 && data.size() >= 18) {
    this->protocol_mode_ = 1;
    this->last_update_ = millis();
    
    // Parse basic frame
    uint8_t running_state = data[3];
    uint8_t error_code = data[4];
    uint8_t running_step = data[5];
    uint16_t altitude = data[6] + (data[7] << 8);
    uint8_t running_mode = data[8];
    uint8_t set_value = data[9];
    uint16_t supply_voltage_raw = data[11] + (data[12] << 8);
    int16_t case_temp = (int16_t)(data[13] + (data[14] << 8));
    int16_t cabin_temp = (int16_t)(data[15] + (data[16] << 8));
    
    float supply_voltage = supply_voltage_raw / 10.0f;
    
    ESP_LOGD(TAG, \"Mode 1 (AA55): State=%d, Error=%d, Step=%d, Voltage=%.1fV\", 
             running_state, error_code, running_step, supply_voltage);
    
    // Update entities
    if (this->main_switch_ != nullptr) {
      this->main_switch_->publish_state(running_state != 0);
    }
    if (this->power_sensor_ != nullptr) {
      this->power_sensor_->publish_state(set_value);
    }
    if (this->power_number_ != nullptr) {
      this->power_number_->publish_state(set_value);
    }
  }
  // Mode 3: Variant AA66 (18 bytes)
  else if (mode == 0x66 && data.size() >= 18) {
    this->protocol_mode_ = 3;
    this->last_update_ = millis();
    
    // Same as mode 1 but error code at byte 17
    uint8_t running_state = data[3];
    uint8_t error_code = data[17];
    uint8_t running_step = data[5];
    
    ESP_LOGD(TAG, \"Mode 3 (AA66): State=%d, Error=%d, Step=%d\", 
             running_state, error_code, running_step);
    
    if (this->main_switch_ != nullptr) {
      this->main_switch_->publish_state(running_state != 0);
    }
  }
  // Mode 2: Extended AA88/136 (32 bytes)
  else if (mode == 0x88 && data.size() >= 32) {
    // Validate CRC16
    std::vector<uint8_t> frame_copy = data;
    uint16_t received_crc = data[29] + (data[30] << 8);
    
    // Replace bytes 29-30 with random bytes from positions 2-3 for CRC validation
    frame_copy[29] = data[2];
    frame_copy[30] = data[3];
    
    uint16_t calculated_crc = this->calculate_crc16_(frame_copy, 31);
    
    if (calculated_crc != received_crc) {
      ESP_LOGW(TAG, \"CRC16 mismatch: calculated=0x%04X, received=0x%04X\", calculated_crc, received_crc);
      return;
    }
    
    this->protocol_mode_ = 2;
    this->last_update_ = millis();
    
    // Parse extended frame (all mode 1 fields plus extended data)
    uint8_t running_state = data[3];
    uint8_t error_code = data[4];
    uint8_t set_value = data[9];
    
    ESP_LOGD(TAG, \"Mode 2 (AA88): State=%d, Error=%d, SetValue=%d\", 
             running_state, error_code, set_value);
    
    // Update entities
    if (this->main_switch_ != nullptr) {
      this->main_switch_->publish_state(running_state != 0);
    }
    if (this->power_sensor_ != nullptr) {
      this->power_sensor_->publish_state(set_value);
    }
    if (this->power_number_ != nullptr) {
      this->power_number_->publish_state(set_value);
    }
  } else {
    ESP_LOGW(TAG, \"Unknown protocol mode: 0x%02X (size: %d)\", mode, data.size());
  }

  // Legacy handling for compatibility
  uint8_t cmd = data[2];
  uint8_t p1 = data[3];
  uint8_t p2 = data[4];
  uint8_t p3 = data[5];
  (void)cmd;
  (void)p1;
  (void)p2;
  (void)p3;

  if (this->setpoint_number_ != nullptr && !isnan(this->last_setpoint_)) {
    this->setpoint_number_->publish_state(this->last_setpoint_);
  }
}

void DieselHeaterBLE::command_start(bool on) {
  this->last_on_ = on;
  this->send_frame_(on ? 0x02 : 0x01, on ? 1 : 0, 0, 0);
}

void DieselHeaterBLE::command_set_setpoint(float val_celsius) {
  if (val_celsius < 5) val_celsius = 5;
  if (val_celsius > 35) val_celsius = 35;
  this->last_setpoint_ = val_celsius;
  this->send_frame_(0x04, static_cast<uint8_t>(val_celsius), 0, 0);
}

void DieselHeaterBLE::command_set_power(float val_percent) {
  if (val_percent < 0) val_percent = 0;
  if (val_percent > 100) val_percent = 100;
  this->last_power_ = val_percent;
  this->send_frame_(0x03, static_cast<uint8_t>(val_percent), 0, 0);
}

void DieselHeaterBLE::command_request_status() {
  this->send_frame_(0x01, 0, 0, 0);
}

void DieselHeaterMainSwitch::write_state(bool state) {
  if (this->parent_ != nullptr) this->parent_->command_start(state);
}

void DieselHeaterSetpointNumber::control(float value) {
  if (this->parent_ != nullptr) this->parent_->command_set_setpoint(value);
}

void DieselHeaterPowerNumber::control(float value) {
  if (this->parent_ != nullptr) this->parent_->command_set_power(value);
}

void DieselHeaterRequestStatusButton::press_action() {
  if (this->parent_ != nullptr) this->parent_->command_request_status();
}

}  // namespace diesel_heater_ble
}  // namespace esphome
