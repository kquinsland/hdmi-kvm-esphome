import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins
from esphome.components import uart
from esphome.components import sensor
from esphome.const import CONF_ID, CONF_HEIGHT, ICON_RULER, DEVICE_CLASS_EMPTY, STATE_CLASS_MEASUREMENT, UNIT_METER


DEPENDENCIES = ['uart']
AUTO_LOAD = ['sensor']

uart_kvm = cg.esphome_ns.namespace('uart_kvm')
JarvisCB2CSensor = uart_kvm.class_('JarvisCB2CSensor', cg.Component, sensor.Sensor, uart.UARTDevice)

# We have 2 'properties' of the KVM that we can read over UART
# - Current Bank / active computer
# - Buzzer status
SENSOR_ACTIVE_BANK = "active_bank"
SENSOR_BUZZER = "buzzer"

CONFIG_SCHEMA = cv.COMPONENT_SCHEMA.extend(
    {
        # UID for the component
        cv.GenerateID(): cv.declare_id(JarvisCB2CSensor),

        # TODO: refine these!
        cv.Optional(SENSOR_ACTIVE_BANK): sensor.sensor_schema(
            unit_of_measurement=UNIT_METER,
            icon=ICON_RULER,
            # Unit works in mm/tenths of inches. The measure is reported using a high/low byte
            #   with one decimal of accuracy. Since we are reporting distance in METERS, we indicate 3
            #    digits of precision. E.G.: Desk might be at 25.9 inches which is 0.65786 meter. We would
            #     report this as .658 meter
            accuracy_decimals=4,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),

        # TODO: refine these!
        cv.Optional(SENSOR_BUZZER): sensor.sensor_schema(
            unit_of_measurement=UNIT_METER,
            icon=ICON_RULER,
            # Unit works in mm/tenths of inches. The measure is reported using a high/low byte
            #   with one decimal of accuracy. Since we are reporting distance in METERS, we indicate 3
            #    digits of precision. E.G.: Desk might be at 25.9 inches which is 0.65786 meter. We would
            #     report this as .658 meter
            accuracy_decimals=4,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
    }
).extend(uart.UART_DEVICE_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)

    if SENSOR_ACTIVE_BANK in config:
        sens = await sensor.new_sensor(config[SENSOR_ACTIVE_BANK])
        cg.add(var.set_active_bank_sensor(sens))

    if SENSOR_BUZZER in config:
        sens = await sensor.new_sensor(config[SENSOR_BUZZER])
        cg.add(var.set_buzzer_status_sensor(sens))
