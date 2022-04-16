#include <math.h> /* round, floor, ceil, trunc */

#include "uart_kvm.h"
#include "esphome/core/log.h"

namespace esphome
{
      namespace uart_kvm
      {

            static const char *TAG = "uart_kvm";

            // Each packet is 6 bytes
            static const uint8_t PACKET_LEN = 6;

            uint8_t invocation_count = 0;

            void UartKvm::setup()
            {
                  // Nothing to do on setup
            }

            /*
                  Called every ~16ms
            */
            void UartKvm::loop()
            {
                  /*
                        I am waiting for feedback from the discord on this.
                        I don't need to be called tens of times every second but I do need to be called once every few seconds.
                        As of right now, I am just going to only do useful work every 100th invocation.
                        At 20ms per invocation, that gives us 50 invocations per second.
                        100/50 ~= 2 seconds
                  */

                  invocation_count += 1;
                  if (invocation_count % 100 != 0)
                  {
                        // ESP_LOGD(TAG, "invocation_count: %i. Bail early", invocation_count);
                        return;
                  }
                  invocation_count = 0;
                  _inquire_active_bank();
            }

            /*
             * Helpers
             */

            void UartKvm::_inquire_active_bank()
            {

                  uint8_t pkt[] = {
                      0xAA,
                      0xBB,
                      0x03,
                      0x10,
                      0x00,
                      0xEE};
                  this->write_array(pkt, PACKET_LEN);
                  this->_read_kvm_reply();
            }

            /*
                  Unfortunately, the KVM seems to also log ascii strings / errors to the UART.
                  But, since we really only care about ONE TYPE of reply packet that looks like:

                        0xAA 0xBB 0x03 0x11 0xXX 0xEE

                  Where 0xXX will be somewhere between 0x00 and whatever the number of ports on the switch is.
            */
            void UartKvm::_read_kvm_reply()
            {
                  uint8_t response[PACKET_LEN];
                  uint8_t peeked;

                  while (this->available())
                  {
                        // Get the byte
                        this->peek_byte(&peeked);
                        ESP_LOGD(TAG, "peeked: 0x%x \t %u", peeked, peeked);

                        if (peeked == 0xAA || peeked == 0xBB || (0x00 <= peeked && peeked <= 0x0F))
                        {
                              this->read();
                        }
                        else if (peeked == 0xEE)
                        {
                              // We still read the byte for completeness then break out of the while loop to read/process the packet we have.
                              this->read();
                              ESP_LOGD(TAG, "EOM");
                              break;
                        }
                        else
                        {
                              /* I don't know how to say "clear it and move on" */
                              this->read();
                              break;
                        }
                  }

                  bool read_success = read_array(response, PACKET_LEN);
                  if (!read_success)
                  {
                        // Don't need this as there will be plenty of:
                        //          Reading from UART timed out at byte 0!
                        // in the logs
                        ////
                        ESP_LOGE(TAG, "Failure reading UART bytes!");
                        status_set_warning();
                        return;
                  }

                  // KVM responds with 0 index so we add 1 to map back to the way the remote/input buttons are labeled
                  this->active_bank_ = response[4] + 1;
                  this->active_bank_sensor_->publish_state(this->active_bank_);
                  ESP_LOGD(TAG, "active_bank: %i", this->active_bank_);
            }

            void UartKvm::set_active_bank(int bank)
            {
                  ESP_LOGD(TAG, "set_active_bank: bank: %i", bank);
                  uint8_t pkt[] = {
                      0xAA,
                      0xBB,
                      0x03,
                      0x01,
                      bank,
                      0xEE};
                  _dump_packet(pkt);
                  this->write_array(pkt, PACKET_LEN);
                  this->_read_kvm_reply();
            }

            void UartKvm::_dump_packet(uint8_t *ptr)
            {

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
            }

      } // namespace uart_kvm
} // namespace esphome
