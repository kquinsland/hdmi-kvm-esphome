Soon after ordering the switch, the seller provided me _basic_ information about how to control the KVM over RS232 and TCP:

[`01.png`](./from_seller/01.png)

[`02.jpg`](./from_seller/02.jpg)

I was able to plug the unique byte string `0xAA 0xBB 0x03` into google and found some additional documentation:

[`4x1-DVI-KVM-CommunicationProtocol.pdf`](./from_google/4x1-DVI-KVM-CommunicationProtocol.pdf)

[`16x1-hdmi-communication-protocol.pdf`](./from_google/16x1-hdmi-communication-protocol.pdf)


Only after I had done about 75% of the ESPHome custom component code did the seller provide me with [`8x1 KVM product integration information.zip`](./from_seller/8x1%20KVM%20product%20integration%20information.zip).

The `8x1 KVM product integration information` folder is the zip extracted. It contains additional software that appears to support more functionality than the basic serial documentation!

It shouldn't be super difficult to run the program and monitor the UART with either WireShark or a logic analyzer to figure out how to do the "program exclusive" things like:

- Change the IP address
- Change the LED display timeout

I don't need this functionality for my purposes so I'm not going to bother... 


I did dump the strings [`Controler.exe.strings`](./from_seller/8x1%20KVM%20product%20integration%20information/8x1-HDMI-Switch-Controler/Controler.exe.strings) and file hashes [`sha256sums.txt`](./from_seller/8x1%20KVM%20product%20integration%20information/8x1-HDMI-Switch-Controler/sha256sums.txt) just for posterity.

If you do manage to reverse engineer the magic UART commands to adjust TCP/IP config and other settings, please do share! PRs welcome :D.
