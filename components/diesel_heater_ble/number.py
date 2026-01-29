import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import number
from esphome.const import CONF_ID

from . import (
    DieselHeaterBLE,
    DieselHeaterSetpointNumber,
    DieselHeaterPowerNumber,
    CONF_PARENT_ID,
)

CONF_PARENT_ALIAS = "diesel_heater_ble"

CONF_POWER_LEVEL = "power_level"
CONF_SET_TEMP = "set_temp"

SETPOINT_SCHEMA = number.number_schema(
    min_value=5.0, max_value=35.0, step=0.5, unit_of_measurement="°C"
)
POWER_SCHEMA = number.number_schema(
    min_value=0.0, max_value=100.0, step=1.0, unit_of_measurement="%"
)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.Optional(CONF_PARENT_ID): cv.use_id(DieselHeaterBLE),
        cv.Optional(CONF_PARENT_ALIAS): cv.use_id(DieselHeaterBLE),
        cv.Optional(CONF_POWER_LEVEL): number.NUMBER_SCHEMA.extend(
            {cv.GenerateID(): cv.declare_id(DieselHeaterPowerNumber)}
        ).extend(POWER_SCHEMA),
        cv.Optional(CONF_SET_TEMP): number.NUMBER_SCHEMA.extend(
            {cv.GenerateID(): cv.declare_id(DieselHeaterSetpointNumber)}
        ).extend(SETPOINT_SCHEMA),
    }
)


async def to_code(config):
    parent_id = config.get(CONF_PARENT_ID) or config.get(CONF_PARENT_ALIAS)
    parent = await cg.get_variable(parent_id)
    if CONF_POWER_LEVEL in config:
        conf = config[CONF_POWER_LEVEL]
        var_p = cg.new_Pvariable(conf[CONF_ID])
        await number.register_number(var_p, conf, static=True)
        cg.add(var_p.set_parent(parent))
        cg.add(parent.set_power_number(var_p))
    if CONF_SET_TEMP in config:
        conf = config[CONF_SET_TEMP]
        var_s = cg.new_Pvariable(conf[CONF_ID])
        await number.register_number(var_s, conf, static=True)
        cg.add(var_s.set_parent(parent))
        cg.add(parent.set_setpoint_number(var_s))
