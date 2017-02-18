#ifndef __USB_MIDI_DESCR_H_
#define __USB_MIDI_DESCR_H_

#include <stdint.h>
#include <usb.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * USB MIDI Requests
 */

/*
 * Descriptors, etc.
 */

#define USB_DESCRIPTOR_TYPE_CS_INTERFACE     0x24
#define USB_DESCRIPTOR_TYPE_CS_ENDPOINT      0x25
 
#define USB_DEVICE_CLASS_UNDEFINED        0x00
#define USB_DEVICE_CLASS_CDC              0x02
#define USB_DEVICE_SUBCLASS_UNDEFINED     0x00

#define USB_INTERFACE_CLASS_AUDIO         0x01
#define USB_INTERFACE_SUBCLASS_UNDEFINED  0x00
#define USB_INTERFACE_AUDIOCONTROL        0x01
#define USB_INTERFACE_AUDIOSTREAMING      0x02
#define USB_INTERFACE_MIDISTREAMING       0x03

/* MIDI Streaming class specific interfaces */
#define MIDI_IN_JACK                      0x02
#define MIDI_OUT_JACK                     0x03

#define MIDI_JACK_EMBEDDED                0x01
#define MIDI_JACK_EXTERNAL                0x02


#define USB_MIDI_DECLARE_DEV_DESC(vid, pid)         \
{                                                   \
.bLength = 18,           \
.bDescriptorType = USB_DESCRIPTOR_TYPE_DEVICE,      \
.bcdUSB = 0x0110,                                   \
.bDeviceClass = USB_DEVICE_CLASS_UNDEFINED,         \
.bDeviceSubClass = USB_DEVICE_SUBCLASS_UNDEFINED,   \
.bDeviceProtocol = 0x00,                            \
.bMaxPacketSize0 = 0x40,                            \
.idVendor = vid,                                    \
.idProduct = pid,                                   \
.bcdDevice = 0x0200,                                \
.iManufacturer = 0x01,                              \
.iProduct = 0x02,                                   \
.iSerialNumber = 0x03,                              \
.bNumConfigurations = 0x01,                         \
}

#define AC_CS_INTERFACE_DESCRIPTOR_SIZE(DataSize) (8 + DataSize)
#define AC_CS_INTERFACE_DESCRIPTOR(DataSize)        \
 struct {                                           \
      uint8_t  bLength;                               \
      uint8_t  bDescriptorType;                       \
      uint8_t  SubType;                               \
      uint16_t bcdADC;                                \
      uint16_t wTotalLength;                          \
      uint8_t  bInCollection;                         \
      uint8_t  baInterfaceNr[DataSize];               \
  } __attribute__((packed))

typedef struct __attribute__((packed)) {
      uint8_t  bLength;
      uint8_t  bDescriptorType;
      uint8_t  SubType;
      uint16_t bcdADC;
      uint16_t wTotalLength;
  } MS_CS_INTERFACE_DESCRIPTOR;

typedef struct __attribute__((packed)) {
      uint8_t  bLength;
      uint8_t  bDescriptorType;
      uint8_t  SubType;
      uint8_t  bJackType;
      uint8_t  bJackId;
      uint8_t  iJack;
  } MIDI_IN_JACK_DESCRIPTOR;

typedef struct __attribute__((packed)) {
      uint8_t  baSourceId;
      uint8_t  baSourcePin;
  } MIDI_OUT_JACK_SOURCE;

#define MIDI_OUT_JACK_DESCRIPTOR_SIZE(DataSize) (7 + 2*DataSize)
#define MIDI_OUT_JACK_DESCRIPTOR(DataSize)        \
struct __attribute__((packed)) {                  \
      uint8_t  bLength;                               \
      uint8_t  bDescriptorType;                       \
      uint8_t  SubType;                               \
      uint8_t  bJackType;                             \
      uint8_t  bJackId;                               \
      uint8_t  bNrInputPins;                          \
      MIDI_OUT_JACK_SOURCE baSource[DataSize];        \
/*      uint8_t  baSourceId[DataSize]; */             \
/*      uint8_t  baSourcePin[DataSize];  */             \
      uint8_t  iJack;                                 \
  } 


#define MS_CS_BULK_ENDPOINT_DESCRIPTOR_SIZE(DataSize) (4 + DataSize)
#define MS_CS_BULK_ENDPOINT_DESCRIPTOR(DataSize)    \
struct __attribute__((packed)) {                    \
      uint8_t  bLength;                               \
      uint8_t  bDescriptorType;                       \
      uint8_t  SubType;                               \
      uint8_t  bNumEmbMIDIJack;                       \
      uint8_t  baAssocJackID[DataSize];               \
  } 

#ifdef __cplusplus
}
#endif

#endif
