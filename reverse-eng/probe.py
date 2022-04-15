"""
Trying to poke KVM
"""

import serial
import logging
import string
logging.basicConfig(level=logging.DEBUG)


ser = serial.Serial('/dev/ttyUSB0')
ser.baudrate = 9600

"""
We can do a few things:

- get/set active computer
- get/set buzzer state
- get/set auto-input detection


Each packet is 6 bytes and shares a common header


0xAA 0xBB 0x03 0x01 0xXX 0xEE               Where XX is to change the port. not 0 indexed; screen 1 is 0x01
0xAA 0xBB 0x03 0x02 0xXX 0xEE               Where 0x01 is buzzer enabled
0xAA 0xBB 0x03 0x81 0xXX 0xEE               Where 0x01 enables auto input, 0x00 disables

0xAA 0xBB 0x03 0x10 0x00 0xEE               send this to GET the active port

and then the unit will REPLY with
0xAA 0xBB 0x03 0x11 0xXX 0xEE               where 0x00 will be the active PC. this IS zero indexed: 0x00 is port 1


"""
PKT_SIZE = 6
PREAMBLE = bytes([0xAA, 0xBB, 0x03])
POSTAMBLE = bytes([0xEE])

# This is the 8 port. Docs indicate that there might be a 4 and 16 port version, too.
NUMBER_OF_BANKS = 8


def _get_active_bank():
    """
    """
    _cmd = bytes([0x10, 0x00])
    _pkt = PREAMBLE + _cmd + POSTAMBLE
    logging.debug(f"sending: {_pkt}")
    ser.write(_pkt)

    reply = ser.read(size=PKT_SIZE)
    logging.debug(f"Here is reply: {reply}")

    # TODO basic validaion of the bytes we read
    _active_bank = int(_pkt[4])

    # Range stop is not inclusive and the protocol says that the switch will reply with 0 index
    if _active_bank in range(0, NUMBER_OF_BANKS):
        return _active_bank + 1
    else:
        logging.error("unknown bank")


def _set_active_bank(bank_number: int = 1) -> None:
    """
    """
    # When specifying the bank to use, protocol assumes NOT 0 indexed :/.
    if bank_number not in range(1, NUMBER_OF_BANKS+1):
        raise ValueError(f"Invalid bank number: {bank_number}")

    _cmd = bytes([1, bank_number])
    _pkt = PREAMBLE + _cmd + POSTAMBLE
    logging.debug(f"sending: {_pkt}")
    ser.write(_pkt)

    # I was just curious, but it looks like the unit WILL reply back. Sometimes with bytes and sometimes with ASCII
    #   messages of some log out!
    #       b'\x07\x1d\xaa\xbb\x03\x11'
    # I'm not super sure what this means... but good to know :)
    reply = ser.read(size=PKT_SIZE)
    logging.debug(f"Here is reply: {reply}")


def _set_buzzer_state(enabled: bool = False) -> None:
    """
    """
    # 0xAA 0xBB 0x03 0x02 0xXX 0xEE
    _cmd = bytes([0x02, enabled])
    _pkt = PREAMBLE + _cmd + POSTAMBLE
    logging.debug(f"sending: {_pkt}")
    ser.write(_pkt)
    # IN testing, I would sometimes see ASCII printed to the UART, too :/
    # Not after every command... but usually after this one. the ASCII is almost
    #       always some sort of 'i2c write error NN'


if __name__ == "__main__":
    logging.info(f"Inquiring about active port...")
    active_port = _get_active_bank()
    logging.info(f"active_port: {active_port}")

    desired_bank = 8
    logging.info(f"Setting desired_bank: {desired_bank}")
    _set_active_bank(desired_bank)

    logging.info(f"disable buzzer")
    _set_buzzer_state()

    logging.info(f"enable buzzer")
    _set_buzzer_state(True)
    logging.info(f"back to bank 1")
    _set_active_bank(1)
    ser.close()
