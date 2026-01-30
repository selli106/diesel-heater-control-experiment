"""ESPHome Diesel Heater BLE Component (codegen)"""
import esphome.codegen as cg
from esphome.components import ble_client, number, switch, button, sensor
import esphome.config_validation as cv
from esphome.const import CONF_ID

CODEOWNERS = ["@warehog", "@combined"]
DEPENDENCIES = ["ble_client"]
AUTO_LOAD = ["sensor", "switch", "number", "button"]

CONF_PASSWORD = "password"
CONF_PROTOCOL = "protocol"
CONF_PARENT_ID = "diesel_heater_ble_id"

diesel_heater_ble_ns = cg.esphome_ns.namespace("diesel_heater_ble")
DieselHeaterBLE = diesel_heater_ble_ns.class_(
    "DieselHeaterBLE", cg.Component, ble_client.BLEClientNode
)

ProtocolVariant = diesel_heater_ble_ns.enum("ProtocolVariant")
PROTOCOL_VARIANT = {
    "AUTO": ProtocolVariant.AUTO,
    "AA55": ProtocolVariant.AA55,
    "AA66": ProtocolVariant.AA66,
}

DieselHeaterSetpointNumber = diesel_heater_ble_ns.class_(
    "DieselHeaterSetpointNumber", number.Number
)
DieselHeaterPowerNumber = diesel_heater_ble_ns.class_(
    "DieselHeaterPowerNumber", number.Number
)
DieselHeaterMainSwitch = diesel_heater_ble_ns.class_(
    "DieselHeaterMainSwitch", switch.Switch
)
DieselHeaterRequestStatusButton = diesel_heater_ble_ns.class_(
    "DieselHeaterRequestStatusButton", button.Button
)
DieselHeaterPowerSensor = diesel_heater_ble_ns.class_(
    "DieselHeaterPowerSensor", sensor.Sensor
)

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(DieselHeaterBLE),
            cv.Optional(CONF_PASSWORD, default=[0x0C, 0x22]): cv.All(
                cv.ensure_list(cv.int_range(min=0, max=255)), cv.Length(min=2, max=2)
            ),
            cv.Optional(CONF_PROTOCOL, default="AUTO"): cv.enum(
                PROTOCOL_VARIANT, upper=True
            ),
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
    .extend(ble_client.BLE_CLIENT_SCHEMA)
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await ble_client.register_ble_node(var, config)
    # Configure base settings
    pw = config[CONF_PASSWORD]
    cg.add(var.set_password(pw[0], pw[1]))
    cg.add(var.set_protocol(config[CONF_PROTOCOL]))
