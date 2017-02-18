
#include <usb_midi_device.h>
#include <usb_midi_descr.h>

/*  */
#include "usb_dev.h"
//#include "usb_core.h"
//#include "usb_def.h"

/*
 * Descriptors
 */

#define NUM_MIDI_IN_JACKS 1

#ifndef NUM_MIDI_OUT_JACKS
#define NUM_MIDI_OUT_JACKS 1
#endif

#ifndef NUM_MIDI_IN_JACKS
#define NUM_MIDI_IN_JACKS 1
#endif

static const usb_descriptor_device usbMIDIDescriptor_Device =
    USB_MIDI_DECLARE_DEV_DESC(LUFA_ID_VENDOR, LUFA_ID_MIDI_DEVICE);

typedef struct __attribute__((packed)) {
    usb_descriptor_config_header       Config_Header;
    /* Control Interface */
    usb_descriptor_interface           AC_Interface;
    AC_CS_INTERFACE_DESCRIPTOR(1)      AC_CS_Interface;
    /* Control Interface */
    usb_descriptor_interface           MS_Interface;
    MS_CS_INTERFACE_DESCRIPTOR         MS_CS_Interface;
    MIDI_IN_JACK_DESCRIPTOR            MIDI_IN_JACK_01;
#if NUM_MIDI_IN_JACKS>1
    MIDI_IN_JACK_DESCRIPTOR            MIDI_IN_JACK_02;
#endif
    MIDI_IN_JACK_DESCRIPTOR            MIDI_IN_JACK_21;
    MIDI_OUT_JACK_DESCRIPTOR(1)        MIDI_OUT_JACK_41;
    MIDI_OUT_JACK_DESCRIPTOR(1)        MIDI_OUT_JACK_61;
#if NUM_MIDI_IN_JACKS>1
    MIDI_OUT_JACK_DESCRIPTOR(1)        MIDI_OUT_JACK_62;
#endif
    usb_descriptor_endpoint            DataOutEndpoint;
    MS_CS_BULK_ENDPOINT_DESCRIPTOR(NUM_MIDI_IN_JACKS)  MS_CS_DataOutEndpoint;
    usb_descriptor_endpoint            DataInEndpoint;
    MS_CS_BULK_ENDPOINT_DESCRIPTOR(1)  MS_CS_DataInEndpoint;
} usb_descriptor_config;

static const usb_descriptor_config usbMIDIDescriptor_Config = {
    .Config_Header = {
        .bLength              = sizeof(usb_descriptor_config_header),
        .bDescriptorType      = USB_DESCRIPTOR_TYPE_CONFIGURATION,
        .wTotalLength         = sizeof(usb_descriptor_config),
        .bNumInterfaces       = 0x02,
        .bConfigurationValue  = 0x01,
        .iConfiguration       = 0x00,
        .bmAttributes         = (USB_CONFIG_ATTR_BUSPOWERED |
                                 USB_CONFIG_ATTR_SELF_POWERED),
        .bMaxPower            = MAX_POWER,
    },

    .AC_Interface = {
        .bLength            = sizeof(usb_descriptor_interface),
        .bDescriptorType    = USB_DESCRIPTOR_TYPE_INTERFACE,
        .bInterfaceNumber   = 0x00,
        .bAlternateSetting  = 0x00,
        .bNumEndpoints      = 0x00,
        .bInterfaceClass    = USB_INTERFACE_CLASS_AUDIO,
        .bInterfaceSubClass = USB_INTERFACE_AUDIOCONTROL,
        .bInterfaceProtocol = 0x00,
        .iInterface         = 0x00,
    },

    .AC_CS_Interface = {
        .bLength            = AC_CS_INTERFACE_DESCRIPTOR_SIZE(1),
        .bDescriptorType    = USB_DESCRIPTOR_TYPE_CS_INTERFACE,
        .SubType            = 0x01,
        .bcdADC             = 0x0100,
        .wTotalLength       = AC_CS_INTERFACE_DESCRIPTOR_SIZE(1),
        .bInCollection      = 0x01,
        .baInterfaceNr      = {0x01},
    },

    .MS_Interface = {
        .bLength            = sizeof(usb_descriptor_interface),
        .bDescriptorType    = USB_DESCRIPTOR_TYPE_INTERFACE,
        .bInterfaceNumber   = 0x01,
        .bAlternateSetting  = 0x00,
        .bNumEndpoints      = 0x02,
        .bInterfaceClass    = USB_INTERFACE_CLASS_AUDIO,
        .bInterfaceSubClass = USB_INTERFACE_MIDISTREAMING,
        .bInterfaceProtocol = 0x00,
        .iInterface         = 0x00,
    },

    .MS_CS_Interface = {
        .bLength            = sizeof(MS_CS_INTERFACE_DESCRIPTOR),
        .bDescriptorType    = USB_DESCRIPTOR_TYPE_CS_INTERFACE,
        .SubType            = 0x01,
        .bcdADC             = 0x0100,
        .wTotalLength       = sizeof(MS_CS_INTERFACE_DESCRIPTOR)
                              +sizeof(MIDI_IN_JACK_DESCRIPTOR)
                              +sizeof(MIDI_IN_JACK_DESCRIPTOR)
                              +MIDI_OUT_JACK_DESCRIPTOR_SIZE(1)
                              +MIDI_OUT_JACK_DESCRIPTOR_SIZE(1)
#if NUM_MIDI_IN_JACKS>1
                              +sizeof(MIDI_IN_JACK_DESCRIPTOR)
                              +MIDI_OUT_JACK_DESCRIPTOR_SIZE(1)
#endif
                              +sizeof(usb_descriptor_endpoint)
                              +MS_CS_BULK_ENDPOINT_DESCRIPTOR_SIZE(NUM_MIDI_IN_JACKS)
                              +sizeof(usb_descriptor_endpoint)
                              +MS_CS_BULK_ENDPOINT_DESCRIPTOR_SIZE(1)                             
                                 /* 0x41-4 */,
    },

    .MIDI_IN_JACK_01 = {
        .bLength            = sizeof(MIDI_IN_JACK_DESCRIPTOR),
        .bDescriptorType    = USB_DESCRIPTOR_TYPE_CS_INTERFACE,
        .SubType            = MIDI_IN_JACK,
        .bJackType          = MIDI_JACK_EMBEDDED,
        .bJackId            = 0x01,
        .iJack              = 0x04,
    },

#if NUM_MIDI_IN_JACKS>1
    .MIDI_IN_JACK_02 = {
        .bLength            = sizeof(MIDI_IN_JACK_DESCRIPTOR),
        .bDescriptorType    = USB_DESCRIPTOR_TYPE_CS_INTERFACE,
        .SubType            = MIDI_IN_JACK,
        .bJackType          = MIDI_JACK_EMBEDDED,
        .bJackId            = 0x02,
        .iJack              = 0x05,
    },
#endif

    .MIDI_IN_JACK_21 = {
        .bLength            = sizeof(MIDI_IN_JACK_DESCRIPTOR),
        .bDescriptorType    = USB_DESCRIPTOR_TYPE_CS_INTERFACE,
        .SubType            = MIDI_IN_JACK,
        .bJackType          = MIDI_JACK_EXTERNAL,
        .bJackId            = 0x21,
        .iJack              = 0x00,
    },

    .MIDI_OUT_JACK_41 = {
        .bLength            = MIDI_OUT_JACK_DESCRIPTOR_SIZE(1),
        .bDescriptorType    = USB_DESCRIPTOR_TYPE_CS_INTERFACE,
        .SubType            = MIDI_OUT_JACK,
        .bJackType          = MIDI_JACK_EMBEDDED,
        .bJackId            = 0x41,
        .bNrInputPins       = 0x01,
		.baSource[0].baSourceId  = 0x21,
		.baSource[0].baSourcePin = 0x01,
        .iJack              = 0x00,
    },

    .MIDI_OUT_JACK_61 = {
        .bLength            = MIDI_OUT_JACK_DESCRIPTOR_SIZE(1),
        .bDescriptorType    = USB_DESCRIPTOR_TYPE_CS_INTERFACE,
        .SubType            = MIDI_OUT_JACK,
        .bJackType          = MIDI_JACK_EXTERNAL,
        .bJackId            = 0x61,
        .bNrInputPins       = 0x01,
		.baSource[0]        = {0x01, 0x01},
        .iJack              = 0x00,
    },

#if NUM_MIDI_IN_JACKS>1
    .MIDI_OUT_JACK_62 = {
        .bLength            = MIDI_OUT_JACK_DESCRIPTOR_SIZE(1),
        .bDescriptorType    = USB_DESCRIPTOR_TYPE_CS_INTERFACE,
        .SubType            = MIDI_OUT_JACK,
        .bJackType          = MIDI_JACK_EXTERNAL,
        .bJackId            = 0x62,
        .bNrInputPins       = 0x01,
		.baSource[0]        = {0x02, 0x01},
        .iJack              = 0x00,
    },
#endif

    .DataOutEndpoint = {
        .bLength            = sizeof(usb_descriptor_endpoint),
        .bDescriptorType    = USB_DESCRIPTOR_TYPE_ENDPOINT,
        .bEndpointAddress   = (USB_DESCRIPTOR_ENDPOINT_OUT |
                             USB_MIDI_RX_ENDP),
        .bmAttributes       = USB_EP_TYPE_BULK,
        .wMaxPacketSize     = USB_MIDI_RX_EPSIZE,
        .bInterval          = 0x00,
    },

    .MS_CS_DataOutEndpoint = {
      .bLength              = MS_CS_BULK_ENDPOINT_DESCRIPTOR_SIZE(NUM_MIDI_IN_JACKS),
      .bDescriptorType      = USB_DESCRIPTOR_TYPE_CS_ENDPOINT,
      .SubType              = 0x01,
      .bNumEmbMIDIJack      = NUM_MIDI_IN_JACKS,
      .baAssocJackID[0]     = 0x01,
#if NUM_MIDI_IN_JACKS>1
      .baAssocJackID[1]     = 0x02,
#endif
	},

    .DataInEndpoint = {
        .bLength          = sizeof(usb_descriptor_endpoint),
        .bDescriptorType  = USB_DESCRIPTOR_TYPE_ENDPOINT,
        .bEndpointAddress = (USB_DESCRIPTOR_ENDPOINT_IN | USB_MIDI_TX_ENDP),
        .bmAttributes     = USB_EP_TYPE_BULK,
        .wMaxPacketSize   = USB_MIDI_TX_EPSIZE,
        .bInterval        = 0x00,
    },

    .MS_CS_DataInEndpoint = {
      .bLength              = MS_CS_BULK_ENDPOINT_DESCRIPTOR_SIZE(1),
      .bDescriptorType      = USB_DESCRIPTOR_TYPE_CS_ENDPOINT,
      .SubType              = 0x01,
      .bNumEmbMIDIJack      = 0x01,
      .baAssocJackID[0]     = 0x41,
	},

};

/*

  String Descriptors:

*/

/* Unicode language identifier: 0x0409 is US English */
static const usb_descriptor_string usbMIDIDescriptor_LangID = {
    .bLength         = USB_DESCRIPTOR_STRING_LEN(1),
    .bDescriptorType = USB_DESCRIPTOR_TYPE_STRING,
    .wString         = {0x0409},
};

static const usb_descriptor_string usbMIDIDescriptor_iManufacturer = {
    .bLength = USB_DESCRIPTOR_STRING_LEN(8),
    .bDescriptorType = USB_DESCRIPTOR_TYPE_STRING,
    .wString = {'L', 'U', 'F', 'A', ' ', 'M', 'L', 'U'},
};

#ifdef STM32F746xx
static const usb_descriptor_string usbMIDIDescriptor_iProduct = {
    .bLength = USB_DESCRIPTOR_STRING_LEN(12),
    .bDescriptorType = USB_DESCRIPTOR_TYPE_STRING,
    .wString = {'S', 'T', 'M', '3', '2', 'F', '7', ' ', 'M', 'I', 'D', 'I'},
};
#else
static const usb_descriptor_string usbMIDIDescriptor_iProduct = {
    .bLength = USB_DESCRIPTOR_STRING_LEN(12),
    .bDescriptorType = USB_DESCRIPTOR_TYPE_STRING,
    .wString = {'X', 'M', 'C', '4', '5', '0', '0', ' ', 'M', 'I', 'D', 'I'},
};
#endif

static const usb_descriptor_string usbVcomDescriptor_iSN = {
    .bLength = USB_DESCRIPTOR_STRING_LEN(6),
    .bDescriptorType = USB_DESCRIPTOR_TYPE_STRING,
    .wString = {'7', '4', '6', '.', '1', '1'},
};

static const usb_descriptor_string usbMIDIDescriptor_iInterface = {
    .bLength = USB_DESCRIPTOR_STRING_LEN(4),
    .bDescriptorType = USB_DESCRIPTOR_TYPE_STRING,
    .wString = {'M', 'I', 'D', 'I'},
};

static const usb_descriptor_string usbMIDIDescriptor_iJack1 = {
    .bLength = USB_DESCRIPTOR_STRING_LEN(5),
    .bDescriptorType = USB_DESCRIPTOR_TYPE_STRING,
    .wString = {'J', 'a', 'c', 'k', '1'},
};

static const usb_descriptor_string usbMIDIDescriptor_iJack2 = {
    .bLength = USB_DESCRIPTOR_STRING_LEN(5),
    .bDescriptorType = USB_DESCRIPTOR_TYPE_STRING,
    .wString = {'J', 'a', 'c', 'k', '2'},
};

int descriptor_table_length = 9;

usb_descriptor_list_t descriptor_table[9] = {
	{0x0100, 0x0000, sizeof(usb_descriptor_device),(uint8_t*)&usbMIDIDescriptor_Device },
	{0x0200, 0x0000, sizeof(usb_descriptor_config),(uint8_t*)&usbMIDIDescriptor_Config },
	{0x0300, 0x0000, USB_DESCRIPTOR_STRING_LEN(1), (uint8_t*)&usbMIDIDescriptor_LangID},
	{0x0301, 0x0409, USB_DESCRIPTOR_STRING_LEN(8), (uint8_t*)&usbMIDIDescriptor_iManufacturer},
	{0x0302, 0x0409, USB_DESCRIPTOR_STRING_LEN(12), (uint8_t*)&usbMIDIDescriptor_iProduct},
	{0x0303, 0x0409, USB_DESCRIPTOR_STRING_LEN(6), (uint8_t*)&usbVcomDescriptor_iSN},
	{0x0304, 0x0409, USB_DESCRIPTOR_STRING_LEN(5), (uint8_t*)&usbMIDIDescriptor_iJack1},
	{0x0305, 0x0409, USB_DESCRIPTOR_STRING_LEN(5), (uint8_t*)&usbMIDIDescriptor_iJack2}
};

