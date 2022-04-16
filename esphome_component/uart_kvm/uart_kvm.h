#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/uart/uart.h"
#include "esphome/core/hal.h"

namespace esphome
{
  namespace uart_kvm
  {

    class UartKvm : public PollingComponent, public sensor::Sensor, public uart::UARTDevice
    {
    public:
      float get_setup_priority() const override { return setup_priority::LATE; }
      void setup() override;
      // void loop() override;
      void update() override;
      void dump_config() override;

      // API to greater ESPHome
      void set_active_bank_sensor(sensor::Sensor *sensor) { this->active_bank_sensor_ = sensor; }

      // Functions to call from lambda
      void set_active_bank(int bank);
      int get_active_bank() { return this->active_bank_; }

      void set_buzzer_status(bool status);

    protected:
      sensor::Sensor *active_bank_sensor_{nullptr};

      int active_bank_ = 1;

      static void _dump_packet(uint8_t *ptr);
      void _inquire_active_bank();

      void _read_kvm_reply();
    };

  } // namespace uart_kvm
} // namespace esphome
