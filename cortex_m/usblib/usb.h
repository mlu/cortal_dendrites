#ifndef _USB_H_
#define _USB_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/*
 * USB module states
 */

typedef enum _usb_dev_state {
    USB_UNCONNECTED,
    USB_ATTACHED,
    USB_POWERED,
    USB_SUSPENDED,
    USB_ADDRESSED,
    USB_CONFIGURED
} usb_dev_state_type;


/*
 * Requests
 */

/* Structure of cobtrol ep stup request */

typedef struct __attribute__((packed)) _USB_SETUP_PACKET {
	uint8_t 	bmRequestType;
	uint8_t 	bRequest;
	uint16_t	wValue;
	uint16_t	wIndex;
	uint16_t	wLength;
} 	USB_SETUP_PACKET;

#define REQUEST_TYPE      0x60  /* Mask to get request type */
#define STANDARD_REQUEST  0x00  /* Standard request */
#define CLASS_REQUEST     0x20  /* Class request */
#define VENDOR_REQUEST    0x40  /* Vendor request */

#define RECIPIENT         0x1F  /* Mask to get recipient */

#define USB_DIR_OUT       0               /* to device */
#define USB_DIR_IN        0x80            /* to host */

typedef enum _RECIPIENT_TYPE
{
  DEVICE_RECIPIENT,     /* Recipient device */
  INTERFACE_RECIPIENT,  /* Recipient interface */
  ENDPOINT_RECIPIENT,   /* Recipient endpoint */
  OTHER_RECIPIENT
} RECIPIENT_TYPE;

typedef enum _STANDARD_REQUESTS
{
  GET_STATUS = 0,
  CLEAR_FEATURE,
  RESERVED1,
  SET_FEATURE,
  RESERVED2,
  SET_ADDRESS,
  GET_DESCRIPTOR,
  SET_DESCRIPTOR,
  GET_CONFIGURATION,
  SET_CONFIGURATION,
  GET_INTERFACE,
  SET_INTERFACE,
  TOTAL_sREQUEST,  /* Total number of Standard request */
  SYNCH_FRAME = 12
} STANDARD_REQUESTS;

/* Feature selector of a SET_FEATURE or CLEAR_FEATURE */
typedef enum _FEATURE_SELECTOR
{
  ENDPOINT_STALL,
  DEVICE_REMOTE_WAKEUP
} FEATURE_SELECTOR;

/*
 * Descriptors
 */

/* Descriptor types */
typedef enum _DESCRIPTOR_TYPE
{
    USB_DESCRIPTOR_TYPE_DEVICE = 1,
    USB_DESCRIPTOR_TYPE_CONFIGURATION,
    USB_DESCRIPTOR_TYPE_STRING,
    USB_DESCRIPTOR_TYPE_INTERFACE,
    USB_DESCRIPTOR_TYPE_ENDPOINT
} DESCRIPTOR_TYPE;

/* Descriptor structs and declaration helpers */

#define USB_DESCRIPTOR_STRING_LEN(x) (2 + (x << 1))

#define USB_DESCRIPTOR_STRING(len)              \
  struct __attribute__((packed)) {    \
      uint8_t bLength;                            \
      uint8_t bDescriptorType;                    \
      uint16_t bString[len];                      \
  } 

typedef struct __attribute__((packed)) usb_descriptor_device {
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint16_t bcdUSB;
    uint8_t  bDeviceClass;
    uint8_t  bDeviceSubClass;
    uint8_t  bDeviceProtocol;
    uint8_t  bMaxPacketSize0;
    uint16_t idVendor;
    uint16_t idProduct;
    uint16_t bcdDevice;
    uint8_t  iManufacturer;
    uint8_t  iProduct;
    uint8_t  iSerialNumber;
    uint8_t  bNumConfigurations;
} usb_descriptor_device;

typedef struct __attribute__((packed)) usb_descriptor_config_header {
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint16_t wTotalLength;
    uint8_t  bNumInterfaces;
    uint8_t  bConfigurationValue;
    uint8_t  iConfiguration;
    uint8_t  bmAttributes;
    uint8_t  bMaxPower;
} usb_descriptor_config_header;

typedef struct __attribute__((packed)) usb_descriptor_interface {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bInterfaceNumber;
    uint8_t bAlternateSetting;
    uint8_t bNumEndpoints;
    uint8_t bInterfaceClass;
    uint8_t bInterfaceSubClass;
    uint8_t bInterfaceProtocol;
    uint8_t iInterface;
} usb_descriptor_interface;

typedef struct __attribute__((packed)) usb_descriptor_endpoint {
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bEndpointAddress;
    uint8_t  bmAttributes;
    uint16_t wMaxPacketSize;
    uint8_t  bInterval;
}usb_descriptor_endpoint;

typedef struct __attribute__((packed)) usb_descriptor_string {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t wString[];
} usb_descriptor_string;

/* Common values that go inside descriptors */

#define USB_CONFIG_ATTR_BUSPOWERED        0b10000000
#define USB_CONFIG_ATTR_SELF_POWERED      0b11000000

#define USB_EP_TYPE_INTERRUPT             0x03
#define USB_EP_TYPE_BULK                  0x02

#define USB_DESCRIPTOR_ENDPOINT_IN        0x80
#define USB_DESCRIPTOR_ENDPOINT_OUT       0x00

#ifdef __cplusplus
}
#endif

#endif
