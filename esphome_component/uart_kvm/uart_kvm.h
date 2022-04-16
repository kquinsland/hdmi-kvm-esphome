#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/uart/uart.h"
#include "esphome/core/hal.h"

namespace esphome
{
  namespace uart_kvm
  {

    class UartKvm : public Component, public sensor::Sensor, public uart::UARTDevice
    {
    public:
      float get_setup_priority() const override { return setup_priority::LATE; }
      void setup() override;
      void loop() override;
      void dump_config() override;

      void set_active_bank_sensor(sensor::Sensor *sensor) { this->active_bank_sensor_ = sensor; }
      void set_buzzer_status_sensor(sensor::Sensor *sensor) { this->buzzer_status_sensor_ = sensor; }
      void set_number_of_banks(uint32_t num_banks) { this->num_banks_ = num_banks; }

      // Functions to call from lambda
      void set_active_bank(int bank);
      int get_active_bank() { return this->active_bank_; }

      void set_buzzer_status(bool status);
      bool get_buzzer_status() { return this->buzzer_enabled_; }

    protected:
      sensor::Sensor *active_bank_sensor_{nullptr};
      sensor::Sensor *buzzer_status_sensor_{nullptr};

      int active_bank_;
      bool buzzer_enabled_;
      int num_banks_;

      static void _dump_packet(uint8_t *ptr);
    };

  } // namespace uart_kvm
} // namespace esphome
