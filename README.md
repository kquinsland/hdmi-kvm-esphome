# `uart_kvm` custom component for ESPHome

This custom component allows for basic control over a generic 8:1 HDMI2.0 KVM switch.
Specifically developed for model number [`PX-UHDKVM801-2.0`](https://www.aliexpress.com/item/1005003927404402.html).

Photos of the internals can be found [on my personal site]()


## Using the component


**Note:** It is possible to get _basic_ control over the KVM without this custom component! ESPHome has _native_ support for _writing_ to a UART. If you only need to tell the KVM what to do then you can use the native [`uart.write`](https://esphome.io/components/uart.html#uart-write-action) call!

However, if  you also want to probe the KVM to get the current active input bank, you will need to _read_ from the UART which can't be done with the native UART component. That's where this custom component comes into play :).
