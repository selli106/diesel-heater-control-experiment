import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import CONF_ID, UNIT_PERCENT, ICON_POWER

from . import DieselHeaterBLE, DieselHeaterPowerSensor, CONF_PARENT_ID

CONFIG_SCHEMA = sensor.SENSOR_SCHEMA.extend(
    {
        cv.GenerateID(): cv.declare_id(DieselHeaterPowerSensor),
        cv.Required(CONF_PARENT_ID): cv.use_id(DieselHeaterBLE),
    }
).extend(sensor.sensor_schema(unit_of_measurement=UNIT_PERCENT, icon=ICON_POWER))


async def to_code(config):
    parent = await cg.get_variable(config[CONF_PARENT_ID])
    var = cg.new_Pvariable(config[CONF_ID])
    await sensor.register_sensor(var, config)
    cg.add(parent.set_power_sensor(var))
