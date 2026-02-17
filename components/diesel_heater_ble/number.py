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

CONFIG_SCHEMA = cv.Schema(
    {
        cv.Optional(CONF_PARENT_ID): cv.use_id(DieselHeaterBLE),
        cv.Optional(CONF_PARENT_ALIAS): cv.use_id(DieselHeaterBLE),
        cv.Optional(CONF_POWER_LEVEL): number.number_schema(
            DieselHeaterPowerNumber,
            unit_of_measurement="%",
        ),
        cv.Optional(CONF_SET_TEMP): number.number_schema(
            DieselHeaterSetpointNumber,
            unit_of_measurement="°C",
        ),
    }
)


async def to_code(config):
    parent_id = config.get(CONF_PARENT_ID) or config.get(CONF_PARENT_ALIAS)
    parent = await cg.get_variable(parent_id)
    if CONF_POWER_LEVEL in config:
        conf = config[CONF_POWER_LEVEL]
        var_p = await number.new_number(conf, min_value=0.0, max_value=100.0, step=1.0)
        cg.add(var_p.set_parent(parent))
        cg.add(parent.set_power_number(var_p))
    if CONF_SET_TEMP in config:
        conf = config[CONF_SET_TEMP]
        var_s = await number.new_number(conf, min_value=5.0, max_value=35.0, step=0.5)
        cg.add(var_s.set_parent(parent))
        cg.add(parent.set_setpoint_number(var_s))
