
#include <usb_midi_device.h>
#include <usb_midi_descr.h>

/*  */
#include "usb_dev.h"
//#include "usb_core.h"
//#include "usb_def.h"

/*
 * Descriptors
 */

static const usb_descriptor_device usbMIDIDescriptor_Device =
    USB_MIDI_DECLARE_DEV_DESC(LUFA_ID_VENDOR, LUFA_ID_MIDI_DEVICE);

typedef struct {
    usb_descriptor_config_header       Config_Header;
    /* Control Interface */
    usb_descriptor_interface           AC_Interface;
    AC_CS_INTERFACE_DESCRIPTOR(1)      AC_CS_Interface;
    /* Control Interface */
    usb_descriptor_interface           MS_Interface;
    MS_CS_INTERFACE_DESCRIPTOR         MS_CS_Interface;
    MIDI_IN_JACK_DESCRIPTOR            MIDI_IN_JACK_1;
    MIDI_IN_JACK_DESCRIPTOR            MIDI_IN_JACK_2;
    MIDI_OUT_JACK_DESCRIPTOR(1)        MIDI_OUT_JACK_3;
    MIDI_OUT_JACK_DESCRIPTOR(1)        MIDI_OUT_JACK_4;
    usb_descriptor_endpoint            DataOutEndpoint;
    MS_CS_BULK_ENDPOINT_DESCRIPTOR(1)  MS_CS_DataOutEndpoint;
    usb_descriptor_endpoint            DataInEndpoint;
    MS_CS_BULK_ENDPOINT_DESCRIPTOR(1)  MS_CS_DataInEndpoint;
} __attribute__((packed)) usb_descriptor_config;

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
        .iInterface         = 0x04,
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
                              +sizeof(usb_descriptor_endpoint)
                              +MS_CS_BULK_ENDPOINT_DESCRIPTOR_SIZE(1)
                              +sizeof(usb_descriptor_endpoint)
                              +MS_CS_BULK_ENDPOINT_DESCRIPTOR_SIZE(1)                             
                                 /* 0x41-4 */,
    },

    .MIDI_IN_JACK_1 = {
        .bLength            = sizeof(MIDI_IN_JACK_DESCRIPTOR),
        .bDescriptorType    = USB_DESCRIPTOR_TYPE_CS_INTERFACE,
        .SubType            = MIDI_IN_JACK,
        .bJackType          = MIDI_JACK_EMBEDDED,
        .bJackId            = 0x01,
        .iJack              = 0x05,
    },

    .MIDI_IN_JACK_2 = {
        .bLength            = sizeof(MIDI_IN_JACK_DESCRIPTOR),
        .bDescriptorType    = USB_DESCRIPTOR_TYPE_CS_INTERFACE,
        .SubType            = MIDI_IN_JACK,
        .bJackType          = MIDI_JACK_EXTERNAL,
        .bJackId            = 0x02,
        .iJack              = 0x00,
    },

    .MIDI_OUT_JACK_3 = {
        .bLength            = MIDI_OUT_JACK_DESCRIPTOR_SIZE(1),
        .bDescriptorType    = USB_DESCRIPTOR_TYPE_CS_INTERFACE,
        .SubType            = MIDI_OUT_JACK,
        .bJackType          = MIDI_JACK_EMBEDDED,
        .bJackId            = 0x03,
        .bNrInputPins       = 0x01,
        .baSourceId         = {0x02},
        .baSourcePin        = {0x01},
        .iJack              = 0x00,
    },

    .MIDI_OUT_JACK_4 = {
        .bLength            = MIDI_OUT_JACK_DESCRIPTOR_SIZE(1),
        .bDescriptorType    = USB_DESCRIPTOR_TYPE_CS_INTERFACE,
        .SubType            = MIDI_OUT_JACK,
        .bJackType          = MIDI_JACK_EXTERNAL,
        .bJackId            = 0x04,
        .bJackId            = 0x03,
        .bNrInputPins       = 0x01,
        .baSourceId         = {0x01},
        .baSourcePin        = {0x01},
        .iJack              = 0x00,
    },

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
      .bLength              = MS_CS_BULK_ENDPOINT_DESCRIPTOR_SIZE(1),
      .bDescriptorType      = USB_DESCRIPTOR_TYPE_CS_ENDPOINT,
      .SubType              = 0x01,
      .bNumEmbMIDIJack      = 0x01,
      .baAssocJackID        = {0x01},
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
      .baAssocJackID        = {0x03},
	},

};

/*

  String Descriptors:

*/

/* Unicode language identifier: 0x0409 is US English */
static const usb_descriptor_string usbMIDIDescriptor_LangID = {
    .bLength         = USB_DESCRIPTOR_STRING_LEN(1),
    .bDescriptorType = USB_DESCRIPTOR_TYPE_STRING,
    .bString         = {0x09, 0x04},
};

static const usb_descriptor_string usbMIDIDescriptor_iManufacturer = {
    .bLength = USB_DESCRIPTOR_STRING_LEN(8),
    .bDescriptorType = USB_DESCRIPTOR_TYPE_STRING,
    .bString = {'L', 0, 'U', 0, 'F', 0, 'A', 0,
                ' ', 0, 'M', 0, 'L', 0, 'U', 0},
};

static const usb_descriptor_string usbMIDIDescriptor_iProduct = {
    .bLength = USB_DESCRIPTOR_STRING_LEN(12),
    .bDescriptorType = USB_DESCRIPTOR_TYPE_STRING,
    .bString = {'X', 0, 'M', 0, 'C', 0, '4', 0, '5', 0, '0', 0, '0', 0, ' ', 0, 'M', 0, 'I', 0, 'D', 0, 'I', 0},
};

static const usb_descriptor_string usbMIDIDescriptor_iInterface = {
    .bLength = USB_DESCRIPTOR_STRING_LEN(4),
    .bDescriptorType = USB_DESCRIPTOR_TYPE_STRING,
    .bString = {'M', 0, 'I', 0, 'D', 0, 'I', 0},
};

static const usb_descriptor_string usbMIDIDescriptor_iJack1 = {
    .bLength = USB_DESCRIPTOR_STRING_LEN(5),
    .bDescriptorType = USB_DESCRIPTOR_TYPE_STRING,
    .bString = {'J', 0, 'a', 0, 'c', 0, 'k', 0, '1', 0},
};


int descriptor_table_length = 7;

usb_descriptor_list_t descriptor_table[7] = {
	{0x0100, 0x0000, sizeof(usb_descriptor_device),(uint8_t*)&usbMIDIDescriptor_Device },
	{0x0200, 0x0000, sizeof(usb_descriptor_config),(uint8_t*)&usbMIDIDescriptor_Config },
	{0x0300, 0x0000, USB_DESCRIPTOR_STRING_LEN(1), (uint8_t*)&usbMIDIDescriptor_LangID},
	{0x0301, 0x0409, USB_DESCRIPTOR_STRING_LEN(8), (uint8_t*)&usbMIDIDescriptor_iManufacturer},
	{0x0302, 0x0409, USB_DESCRIPTOR_STRING_LEN(12), (uint8_t*)&usbMIDIDescriptor_iProduct},
	{0x0303, 0x0409, USB_DESCRIPTOR_STRING_LEN(4), (uint8_t*)&usbMIDIDescriptor_iInterface},
	{0x0304, 0x0409, USB_DESCRIPTOR_STRING_LEN(5), (uint8_t*)&usbMIDIDescriptor_iJack1}
};

