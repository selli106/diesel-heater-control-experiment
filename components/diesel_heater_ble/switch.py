import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import switch
from esphome.const import CONF_ID

from . import DieselHeaterBLE, DieselHeaterMainSwitch, CONF_PARENT_ID

CONF_PARENT_ALIAS = "diesel_heater_ble"
CONF_POWER = "power"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.Optional(CONF_PARENT_ID): cv.use_id(DieselHeaterBLE),
        cv.Optional(CONF_PARENT_ALIAS): cv.use_id(DieselHeaterBLE),
        cv.Optional(CONF_POWER): switch.switch_schema(
            DieselHeaterMainSwitch,
        ),
    }
)


async def to_code(config):
    parent_id = config.get(CONF_PARENT_ID) or config.get(CONF_PARENT_ALIAS)
    parent = await cg.get_variable(parent_id)
    if CONF_POWER in config:
        conf = config[CONF_POWER]
        var = await switch.new_switch(conf)
        cg.add(var.set_parent(parent))
        cg.add(parent.set_main_switch(var))
