A lightweight USB library
=========================

usb.h                 General USB defines, things found in the USB specificatio.
usb_dev.c/h           USB device code and defintions, MCU independent.

usb_cdc.c/h           USB serial device
usb_cdc_descr.c       Descriptor for USB serial device, modify this for VID/PID and string descriptors 

usb_midi.c/h          USB MIDI device
usb_midi_descr.c      Descriptor for USB MIDI device, modify this for VID/PID and string descriptors 


MCU specific code for XMC4500 and STM32F4xx FS USB, uses MCU specific includes from vendor toolchains for register definitions.

stm32f4/usb_dev_stm32f4.c   Device implementation for STM32F4xx chips
stm32f4/usb_regs.h          Register definitions for the USB chip hardware

xmc4500/usb_dev_xmc4500.c   Device implementation for XMC4500 chips


It seems that the STM32F4 and the XMC4500 uses the same IP core for USB, there are only minor differences in the on chip implementation of some registers and register flags.



As always, this is work in progress and is probably filled with nasty bugs but it works for me.
