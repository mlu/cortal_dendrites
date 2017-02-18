#include "usb_dev.h"
#include "usb_cdc.h"

/*
 * Descriptors
 */

#define LUFA_ID_VENDOR                0x03EB
#define LUFA_CDC_PRODUCT              0x2044

static const  __attribute__((packed)) usb_descriptor_device usbVcomDescriptor_Device =
    USB_CDC_DECLARE_DEV_DESC(LUFA_ID_VENDOR, LUFA_CDC_PRODUCT);

typedef struct {
    usb_descriptor_config_header Config_Header;
    usb_descriptor_interface     CCI_Interface;
    CDC_FUNCTIONAL_DESCRIPTOR(2) CDC_Functional_IntHeader;
    CDC_FUNCTIONAL_DESCRIPTOR(2) CDC_Functional_CallManagement;
    CDC_FUNCTIONAL_DESCRIPTOR(1) CDC_Functional_ACM;
    CDC_FUNCTIONAL_DESCRIPTOR(2) CDC_Functional_Union;
    usb_descriptor_endpoint      ManagementEndpoint;
    usb_descriptor_interface     DCI_Interface;
    usb_descriptor_endpoint      DataOutEndpoint;
    usb_descriptor_endpoint      DataInEndpoint;
} __attribute__((packed)) usb_descriptor_config;

#define MAX_POWER (100 >> 1)
static const usb_descriptor_config usbVcomDescriptor_Config = {
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

    .CCI_Interface = {
        .bLength            = sizeof(usb_descriptor_interface),
        .bDescriptorType    = USB_DESCRIPTOR_TYPE_INTERFACE,
        .bInterfaceNumber   = 0x00,
        .bAlternateSetting  = 0x00,
        .bNumEndpoints      = 0x01,
        .bInterfaceClass    = USB_INTERFACE_CLASS_CDC,
        .bInterfaceSubClass = USB_INTERFACE_SUBCLASS_CDC_ACM,
        .bInterfaceProtocol = 0x01, /* Common AT Commands */
        .iInterface         = 0x00,
    },

    .CDC_Functional_IntHeader = {
        .bLength         = CDC_FUNCTIONAL_DESCRIPTOR_SIZE(2),
        .bDescriptorType = 0x24,
        .SubType         = 0x00,
        .Data            = {0x01, 0x10},
    },

    .CDC_Functional_CallManagement = {
        .bLength         = CDC_FUNCTIONAL_DESCRIPTOR_SIZE(2),
        .bDescriptorType = 0x24,
        .SubType         = 0x01,
        .Data            = {0x03, 0x01},
    },

    .CDC_Functional_ACM = {
        .bLength         = CDC_FUNCTIONAL_DESCRIPTOR_SIZE(1),
        .bDescriptorType = 0x24,
        .SubType         = 0x02,
        .Data            = {0x02},
    },

    .CDC_Functional_Union = {
        .bLength         = CDC_FUNCTIONAL_DESCRIPTOR_SIZE(2),
        .bDescriptorType = 0x24,
        .SubType         = 0x06,
        .Data            = {0x00, 0x01},
    },

    .ManagementEndpoint = {
        .bLength          = sizeof(usb_descriptor_endpoint),
        .bDescriptorType  = USB_DESCRIPTOR_TYPE_ENDPOINT,
        .bEndpointAddress = (USB_DESCRIPTOR_ENDPOINT_IN |
                             USB_CDC_MANAGEMENT_ENDP),
        .bmAttributes     = USB_EP_TYPE_INTERRUPT,
        .wMaxPacketSize   = USB_CDC_MANAGEMENT_EPSIZE,
        .bInterval        = 0xFF,
    },

    .DCI_Interface = {
        .bLength            = sizeof(usb_descriptor_interface),
        .bDescriptorType    = USB_DESCRIPTOR_TYPE_INTERFACE,
        .bInterfaceNumber   = 0x01,
        .bAlternateSetting  = 0x00,
        .bNumEndpoints      = 0x02,
        .bInterfaceClass    = USB_INTERFACE_CLASS_DIC,
        .bInterfaceSubClass = 0x00, /* None */
        .bInterfaceProtocol = 0x00, /* None */
        .iInterface         = 0x00,
    },

    .DataOutEndpoint = {
        .bLength          = sizeof(usb_descriptor_endpoint),
        .bDescriptorType  = USB_DESCRIPTOR_TYPE_ENDPOINT,
        .bEndpointAddress = (USB_DESCRIPTOR_ENDPOINT_OUT |
                             USB_CDC_RX_ENDP),
        .bmAttributes     = USB_EP_TYPE_BULK,
        .wMaxPacketSize   = USB_CDC_RX_EPSIZE,
        .bInterval        = 0x00,
    },

    .DataInEndpoint = {
        .bLength          = sizeof(usb_descriptor_endpoint),
        .bDescriptorType  = USB_DESCRIPTOR_TYPE_ENDPOINT,
        .bEndpointAddress = (USB_DESCRIPTOR_ENDPOINT_IN | USB_CDC_TX_ENDP),
        .bmAttributes     = USB_EP_TYPE_BULK,
        .wMaxPacketSize   = USB_CDC_TX_EPSIZE,
        .bInterval        = 0x00,
    },
};

/*

  String Descriptors:

*/

/* Unicode language identifier: 0x0409 is US English */
static const usb_descriptor_string usbVcomDescriptor_LangID = {
    .bLength         = USB_DESCRIPTOR_STRING_LEN(1),
    .bDescriptorType = USB_DESCRIPTOR_TYPE_STRING,
    .wString         = {0x0409},
};

static const usb_descriptor_string usbVcomDescriptor_iManufacturer = {
    .bLength = USB_DESCRIPTOR_STRING_LEN(8),
    .bDescriptorType = USB_DESCRIPTOR_TYPE_STRING,
    .wString = {'L', 'U', 'F', 'A', ' ', 'M', 'L', 'U'},
};

static const usb_descriptor_string usbVcomDescriptor_iProduct = {
    .bLength = USB_DESCRIPTOR_STRING_LEN(11),
    .bDescriptorType = USB_DESCRIPTOR_TYPE_STRING,
    .wString = {'C', 'D', 'C', ' ', 's', 't', 'm', 'f', '7',  '4', '6'},
};

static const usb_descriptor_string usbVcomDescriptor_iSN = {
    .bLength = USB_DESCRIPTOR_STRING_LEN(6),
    .bDescriptorType = USB_DESCRIPTOR_TYPE_STRING,
    .wString = {'7', '4', '6', '.', '1', '1'},
};

int descriptor_table_length = 6;

usb_descriptor_list_t descriptor_table[6] = {
	{0x0100, 0x0000, sizeof(usb_descriptor_device),(uint8_t*)&usbVcomDescriptor_Device },
	{0x0200, 0x0000, sizeof(usb_descriptor_config),(uint8_t*)&usbVcomDescriptor_Config },
	{0x0300, 0x0000, USB_DESCRIPTOR_STRING_LEN(1), (uint8_t*)&usbVcomDescriptor_LangID},
	{0x0301, 0x0409, USB_DESCRIPTOR_STRING_LEN(8), (uint8_t*)&usbVcomDescriptor_iManufacturer},
	{0x0302, 0x0409, USB_DESCRIPTOR_STRING_LEN(11), (uint8_t*)&usbVcomDescriptor_iProduct},
	{0x0303, 0x0409, USB_DESCRIPTOR_STRING_LEN(6), (uint8_t*)&usbVcomDescriptor_iSN}
};

