import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import (
    CONF_ID,
    CONF_IP_ADDRESS,
    CONF_NUM_ATTEMPTS,
    CONF_TIMEOUT,
    STATE_CLASS_MEASUREMENT,
    UNIT_PERCENT,
    UNIT_SECOND,
    ICON_EMPTY,
    DEVICE_CLASS_EMPTY,
)

DEPENDENCIES = ["network"]

ping_ns = cg.esphome_ns.namespace("ping")

PingSensor = ping_ns.class_("PingSensorESP8266", sensor.Sensor, cg.PollingComponent)


def validate_timeout(n):
    n = cv.positive_time_period_microseconds(n)
    if n.total_seconds > 8:
        raise cv.Invalid("Maximum timeout cannot be greater than 8 seconds")
    return n


CONF_LOSS = "loss"
CONF_LATENCY = "latency"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(PingSensor),
        cv.Optional(CONF_IP_ADDRESS, default="8.8.8.8"): cv.string,
        cv.Optional(CONF_TIMEOUT, default="1sec"): cv.positive_time_period_milliseconds,
        cv.Optional(CONF_NUM_ATTEMPTS, default=13): cv.int_range(min=1, max=60),
        cv.Optional(CONF_LOSS): sensor.sensor_schema(
            UNIT_PERCENT,
            ICON_EMPTY,
            0,
            DEVICE_CLASS_EMPTY,
            STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_LATENCY): sensor.sensor_schema(
            UNIT_SECOND,
            ICON_EMPTY,
            0,
            DEVICE_CLASS_EMPTY,
            STATE_CLASS_MEASUREMENT,
        ),
    }
).extend(cv.polling_component_schema("60s"))


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    cg.add(var.set_target(config[CONF_IP_ADDRESS]))
    cg.add(var.set_timeout(config[CONF_TIMEOUT]))
    cg.add(var.set_n_packet(config[CONF_NUM_ATTEMPTS]))

    if CONF_LOSS in config:
        sens = await sensor.new_sensor(config[CONF_LOSS])
        cg.add(var.set_packet_loss_sensor(sens))
    if CONF_LATENCY in config:
        sens = await sensor.new_sensor(config[CONF_LATENCY])
        cg.add(var.set_latency_sensor(sens))
