#include <math.h> /* round, floor, ceil, trunc */

#include "uart_kvm.h"
#include "esphome/core/log.h"

namespace esphome
{
      namespace uart_kvm
      {

            static const char *TAG = "uart_kvm";

            static const unsigned char PREAMBLE[] = {0xAA, 0xBB, 0x03};
            static const unsigned char POSTAMBLE[] = {0xEE};

            // Each packet is 6 bytes
            static const uint8_t PACKET_LEN = 6;

            void UartKvm::setup()
            {
                  this->dump_config();
                  // TODO: inquire about the current bank and buzzer status
                  ESP_LOGD(TAG, "Setup: I don't do anything yet");
            }

            /*
                  Called every ~16ms
            */
            void UartKvm::loop()
            {
                  uint8_t response[PACKET_LEN];
                  uint8_t peeked;

                  /*
                        In testing, it appears that the firmware on the KVM will dump both ascii / logs *and* bytes
                        to the output. Ascii text shows up when buttons are pressed (usually) and almost never when
                        issuing a command.

                        We still read from the uart every time we're called... but we just have to be a bit more rigorous
                              when it comes to doing the validation of what we read.
                  */
                  // while (this->available())
                  // {
                  //       // Get the byte
                  //       this->peek_byte(&peeked);
                  //       ESP_LOGD(TAG, "peeked: %x \t %u", peeked, peeked);

                  //       if (peeked != 0xee)
                  //       {
                  //             this->read();
                  //             // There's nothing else for us to do so bail early
                  //             return;
                  //       }
                  //       else
                  //       {
                  //             // We still read the byte for completeness then break out of the while loop to read/process the packet we have.
                  //             this->read();
                  //             ESP_LOGD(TAG, "EOM");
                  //             break;
                  //       }
                  // }

                  // bool read_success = read_array(response, PACKET_LEN);
                  // if (!read_success)
                  // {
                  //       // Don't need this as there will be plenty of:
                  //       //          Reading from UART timed out at byte 0!
                  //       // in the logs
                  //       ////
                  //       // ESP_LOGE(TAG, "Failure reading UART bytes!");
                  //       status_set_warning();
                  //       return;
                  // }

                  // // Checksum is good! Extract the 'command' byte and dispatch
                  // uint8_t pkt_type = response[2];
                  // switch (pkt_type)
                  // {
                  // case 1:
                  //       /*
                  //             command 1: height report packet looks like this:

                  //                   [0] 0xf2         (242)
                  //                   [1] 0xf2         (242)
                  //                   [2] 0x1          (1)
                  //                   [3] 0x3          (3)
                  //                   [4] 0x1          (1)
                  //                   [5] 0x97         (151)
                  //                   [6] 0x3          (3)
                  //                   [7] 0x9f         (159)
                  //                   [8] 0x7e         (126)

                  //             There should be 3 params, the first two are the high/low bytes and the third has unknown purpose
                  //             See: https://github.com/phord/Jarvis#height-report
                  //       */
                  //       uint8_t height_hi = response[4];  // 0x01
                  //       uint8_t height_low = response[5]; // 0x97

                  //       // 0197 is 407 ... and the display says 40.7 on it!
                  //       this->current_pos_ = (height_hi << 8) + height_low;
                  //       ESP_LOGD(TAG, "height: %d", this->current_pos_);

                  //       // Raw input will be in tenths of $UNIT. EG 407 -> 40.7 inches
                  //       // _to_mm(407) => 1033.78
                  //       // 1033.78 * .001 = 1.03378
                  //       // We will let ESPHome do the truncation for us; we define 4 decimals of accuracy so
                  //       //   the reading will show as 1.0338m in HA. This is human friendly but still has enough
                  //       //   precision in it so user can do meter to mm conversion (for whatever reason...) and they'll
                  //       //   get something pretty accurate.
                  //       this->height_sensor_->publish_state(_to_mm(this->current_pos_) * .001);
                  //       break;
                  // }

                  // // If goto_height() has been called since we last ran
                  // this->_adjust_height();
            }

            /*
             * Helpers
             */

            void UartKvm::set_active_bank(int bank)
            {
                  ESP_LOGD(TAG, "set_active_bank: bank: %i", bank);
            }

            void set_buzzer_status(bool status)
            {
                  ESP_LOGD(TAG, "set_buzzer_status: bank: %b", set_buzzer_status);
            }

            void UartKvm::_dump_packet(uint8_t *ptr)
            {
                  return;
                  ESP_LOGD(TAG, "Got Bytes:");
                  for (int i = 0; i < PACKET_LEN; i++)
                  {
                        ESP_LOGD(TAG, "[%i] 0x%x \t (%u)", i, ptr[i], ptr[i]);
                  }
            }

            void UartKvm::dump_config()
            {
                  ESP_LOGCONFIG(TAG, "UartKvm");
                  LOG_SENSOR("", "active_bank", this->active_bank_sensor_);
                  LOG_SENSOR("", "buzzer", this->buzzer_status_sensor_);
                  ESP_LOGCONFIG("", "num_banks", this->num_banks_);
            }

      } // namespace uart_kvm
} // namespace esphome
