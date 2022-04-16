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

            void UartKvm::setup()
            {
                  // Nothing to do on setup
            }

            void UartKvm::update()
            {
                  ESP_LOGD(TAG, "update");
                  this->_inquire_active_bank();
                  this->_read_kvm_reply();
            }

            /*
                  Unfortunately, the KVM seems to also log ascii strings / errors to the UART. The errors seem to come from manual button presses
                        or IR remote signals. Since all packets to/from the device share a common pre/post amble and the values are

                        0xAA 0xBB 0x03 0x11 0xXX 0xEE

                  Where 0xXX will be somewhere between 0x00 and whatever the number of ports on the switch is.
            */
            void UartKvm::_read_kvm_reply()
            {
                  // Bytes as we read them
                  uint8_t response[PACKET_LEN];
                  uint8_t peeked;
                  uint8_t idx = 0;

                  while (this->available())
                  {
                        // Get the byte
                        this->read_byte(&peeked);
                        ESP_LOGD(TAG, "peeked[%i]: 0x%x \t %u", idx, peeked, peeked);
                        // I can't find any evidence that this OEM makes KVM with more than 16 / 0x0F ports
                        // We also care about the 0x11 byte as that seems to be the "active bank" message type
                        // Because the "led timeout" message type is 0x03 **and** 0x03 is a valid possible byte in the "active bank" message type
                        //    those packets will also 'pass' this filter for LED timeout values that are less tha 0x0F or equal to 0x11 or 0xEE
                        // We will filter those out later.
                        /////
                        if (peeked == 0xAA || peeked == 0xBB || (0x00 <= peeked && peeked <= 0x0F) || peeked == 0x11 || peeked == 0xEE)
                        {
                              response[idx] = peeked;
                              idx += 1;
                        }
                        // If we have 5 bytes, we can start to process: the first 3 will be preamble, byte 4 will be the type and byte 5 will be the value
                        if (idx == (PACKET_LEN - 1) && response[3] == 0x11)
                        {
                              ESP_LOGD(TAG, "EARLY EXIT; active bank!");
                              break;
                        }
                        else if (idx == (PACKET_LEN - 1))
                        {
                              ESP_LOGD(TAG, "EARLY EXIT; unknown type. Start over!");
                              idx = 0;
                        }
                  }

                  /*
                        I don't fully understand _why_, but sometimes the KVM will - UNPROMPTED - send packets that look like this:

                              [0] 0xaa    (170)
                              [1] 0xbb    (187)
                              [2] 0x3     (3)
                              [3] 0x3     (3)
                              [4] 0xaa    (170)
                              [5] 0xbb    (187)

                        From the documentation, it looks like the byte after the preamble (0xaa, 0xbb, 0x03) is the "packet type" and packet type
                              of 0x03 seems to be the LED timeout settings. The 5th byte is supposed to be the number of seconds on the timeout
                              but the value does not seem to match the behavior. In the above packet, the timeout looks like it is 170 seconds.

                        However, the little LED display on the front of the KVM that shows the active bank has been illuminated for the past several
                              hours that I have had it powered on for. 170 seconds is a little under 3 min and the LED display has absolutely been
                              lit for longer than 3 min!

                        We recognize but ignore packets of this type.

                        A packet where the 'type' byte is 0x11 indicates a "active port" message:

                              [0] 0xaa    (170)
                              [1] 0xbb    (187)
                              [2] 0x3     (3)
                              [3] 0x11    (16)
                              [4] 0x0     (0)               # KVM responds with 0 index. Add 1 to "match" the labeling on the device/remote
                              [5] 0xee    (238)


                        We are _very_ interested in these packets :).
                  */

                  switch (response[3])
                  {
                  case 0x03:
                        ESP_LOGD(TAG, "Got LED Timeout Message. Ignore value of: %i", response[4]);
                        return;
                  case 0x11:
                        ESP_LOGD(TAG, "Got Active Bank Message. Processing....");
                        _dump_packet(response);
                        // KVM responds with 0 index so we add 1 to map back to the way the remote/input buttons are labeled
                        this->active_bank_ = response[4] + 1;
                        this->active_bank_sensor_->publish_state(this->active_bank_);
                        ESP_LOGD(TAG, "active_bank: %i", this->active_bank_);
                        return;
                  default:
                        ESP_LOGD(TAG, "Got unknown type of packet. Ignoring: %u ", response[3]);
                        break;
                  }
            }

            void UartKvm::_inquire_active_bank()
            {
                  // Ask the KVM what port is active
                  uint8_t pkt[] = {
                      0xAA,
                      0xBB,
                      0x03,
                      0x10,
                      0x00,
                      0xEE};
                  ESP_LOGD(TAG, "SEND: active bank?");
                  this->write_array(pkt, PACKET_LEN);
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
                  // Send "switch to" command and then send the "what is active" command then listen for reply.
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
