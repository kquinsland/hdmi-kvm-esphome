import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart
from esphome.components import sensor
from esphome.const import CONF_ID, ENTITY_CATEGORY_CONFIG, DEVICE_CLASS_EMPTY


DEPENDENCIES = ['uart']
AUTO_LOAD = ['sensor']

uart_kvm = cg.esphome_ns.namespace('uart_kvm')
UartKvm = uart_kvm.class_('UartKvm', cg.PollingComponent, sensor.Sensor, uart.UARTDevice)

# We have 2 'properties' of the KVM that we can read over UART
# - Current Bank / active computer
SENSOR_ACTIVE_BANK = "active_bank"

CONFIG_SCHEMA = cv.COMPONENT_SCHEMA.extend(
    {
        # UID for the component
        cv.GenerateID(): cv.declare_id(UartKvm),

        cv.Optional(SENSOR_ACTIVE_BANK): sensor.sensor_schema(
            icon="mdi:form-select",
            # We are unitless so we do not have a device class and HA does not (currently) have
            #   a state_class for things that are just arbitrary numbers
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_EMPTY,
            entity_category=ENTITY_CATEGORY_CONFIG
        ),

    }
).extend(cv.polling_component_schema("5s")).extend(uart.UART_DEVICE_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)

    if SENSOR_ACTIVE_BANK in config:
        sens = await sensor.new_sensor(config[SENSOR_ACTIVE_BANK])
        cg.add(var.set_active_bank_sensor(sens))
