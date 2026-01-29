#pragma once

#include "esphome/components/ble_client/ble_client.h"
#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/number/number.h"
#include "esphome/components/switch/switch.h"
#include "esphome/components/button/button.h"

namespace esphome {
namespace diesel_heater_ble {

enum class ProtocolVariant { AUTO = 0, AA55 = 1, AA66 = 2 };

class DieselHeaterBLE;

class DieselHeaterMainSwitch : public switch_::Switch {
 public:
  void set_parent(DieselHeaterBLE *p) { parent_ = p; }

 protected:
  void write_state(bool state) override;

 private:
  DieselHeaterBLE *parent_{nullptr};
};

class DieselHeaterSetpointNumber : public number::Number {
 public:
  void set_parent(DieselHeaterBLE *p) { parent_ = p; }

 protected:
  void control(float value) override;

 private:
  DieselHeaterBLE *parent_{nullptr};
};

class DieselHeaterPowerNumber : public number::Number {
 public:
  void set_parent(DieselHeaterBLE *p) { parent_ = p; }

 protected:
  void control(float value) override;

 private:
  DieselHeaterBLE *parent_{nullptr};
};

class DieselHeaterRequestStatusButton : public button::Button {
 public:
  void set_parent(DieselHeaterBLE *p) { parent_ = p; }

 protected:
  void press_action() override;

 private:
  DieselHeaterBLE *parent_{nullptr};
};

class DieselHeaterPowerSensor : public sensor::Sensor {
};

class DieselHeaterBLE : public esphome::ble_client::BLEClientNode, public Component {
 public:
  void set_password(uint8_t a, uint8_t b) {
    password_[0] = a;
    password_[1] = b;
  }
  void set_protocol(ProtocolVariant p) { requested_protocol_ = p; }

  void set_main_switch(DieselHeaterMainSwitch *s) { main_switch_ = s; }
  void set_setpoint_number(DieselHeaterSetpointNumber *n) { setpoint_number_ = n; }
  void set_power_number(DieselHeaterPowerNumber *n) { power_number_ = n; }
  void set_power_sensor(DieselHeaterPowerSensor *s) { power_sensor_ = s; }
  void set_request_status_button(DieselHeaterRequestStatusButton *b) { request_button_ = b; }

  void dump_config() override;
  void on_ble_client_ready() override;
  void on_connect() override {}
  void on_disconnect() override {}

  void command_start(bool on);
  void command_set_setpoint(float val_celsius);
  void command_set_power(float val_percent);
  void command_request_status();

 protected:
  void detect_protocol_();
  void send_frame_(uint8_t cmd, uint8_t p1 = 0, uint8_t p2 = 0, uint8_t p3 = 0);
  void handle_notify_(const std::vector<uint8_t> &data);
  uint8_t calculate_checksum_(const std::vector<uint8_t> &frame);
  uint16_t calculate_crc16_(const std::vector<uint8_t> &data, uint16_t length);

  bool write_char_(const std::vector<uint8_t> &data);
  bool subscribe_();

  ProtocolVariant protocol_{ProtocolVariant::AUTO};
  ProtocolVariant requested_protocol_{ProtocolVariant::AUTO};
  uint8_t password_[2]{0x0C, 0x22};

  uint8_t protocol_mode_{0};  // 0=unknown, 1=AA55, 2=AA88, 3=AA66
  uint32_t last_update_{0};   // millis() of last valid frame

  uint16_t service_uuid_{0xFFE0};
  uint16_t char_uuid_{0xFFE1};

  optional<ble_client::BLECharacteristic *> write_char_{};

  DieselHeaterMainSwitch *main_switch_{nullptr};
  DieselHeaterSetpointNumber *setpoint_number_{nullptr};
  DieselHeaterPowerNumber *power_number_{nullptr};
  DieselHeaterPowerSensor *power_sensor_{nullptr};
  DieselHeaterRequestStatusButton *request_button_{nullptr};

  float last_power_{NAN};
  float last_setpoint_{NAN};
  bool last_on_{false};
};

}  // namespace diesel_heater_ble
}  // namespace esphome
