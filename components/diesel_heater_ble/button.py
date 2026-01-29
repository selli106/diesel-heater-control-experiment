import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import button
from esphome.const import CONF_ID

from . import DieselHeaterBLE, DieselHeaterRequestStatusButton, CONF_PARENT_ID

CONF_PARENT_ALIAS = "diesel_heater_ble"
CONF_REQUEST_STATUS = "request_status"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.Optional(CONF_PARENT_ID): cv.use_id(DieselHeaterBLE),
        cv.Optional(CONF_PARENT_ALIAS): cv.use_id(DieselHeaterBLE),
        cv.Optional(CONF_REQUEST_STATUS): button.BUTTON_SCHEMA.extend(
            {cv.GenerateID(): cv.declare_id(DieselHeaterRequestStatusButton)}
        ),
    }
)


async def to_code(config):
    parent_id = config.get(CONF_PARENT_ID) or config.get(CONF_PARENT_ALIAS)
    parent = await cg.get_variable(parent_id)
    if CONF_REQUEST_STATUS in config:
        conf = config[CONF_REQUEST_STATUS]
        var = cg.new_Pvariable(conf[CONF_ID])
        await button.register_button(var, conf)
        cg.add(var.set_parent(parent))
        cg.add(parent.set_request_status_button(var))
