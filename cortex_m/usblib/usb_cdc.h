#ifndef _USB_CDC_H_
#define _USB_CDC_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * CDC ACM Requests
 */

#define USB_CDC_SET_LINE_CODING        0x20
#define USB_CDC_GET_LINE_CODING        0x21
#define USB_CDC_SET_COMM_FEATURE       0x02
#define USB_CDC_SET_CONTROL_LINE_STATE 0x22
#define USB_CDC_SEND_BREAK             0x23
#define USB_CDC_CONTROL_LINE_DTR       (0x01)
#define USB_CDC_CONTROL_LINE_RTS       (0x02)

/*
 * Descriptors, etc.
 */

#define CDC_FUNCTIONAL_DESCRIPTOR_SIZE(DataSize) (3 + DataSize)
#define CDC_FUNCTIONAL_DESCRIPTOR(DataSize)     \
  struct {                                      \
      uint8_t bLength;                            \
      uint8_t bDescriptorType;                    \
      uint8_t SubType;                            \
      uint8_t Data[DataSize];                     \
  } __attribute__((packed))

#define USB_DEVICE_CLASS_CDC              0x02
#define USB_DEVICE_SUBCLASS_CDC           0x00
#define USB_INTERFACE_CLASS_CDC           0x02
#define USB_INTERFACE_SUBCLASS_CDC_ACM    0x02
#define USB_INTERFACE_CLASS_DIC           0x0A

/*
 * Endpoint configuration
 */

#define USB_CDC_CTRL_ENDP            0
#define USB_CDC_CTRL_EPSIZE          0x40

#define USB_CDC_TX_ENDP              1
#define USB_CDC_TX_EPSIZE            0x40

#define USB_CDC_MANAGEMENT_ENDP      2
#define USB_CDC_MANAGEMENT_EPSIZE    0x40

#define USB_CDC_RX_ENDP              1
#define USB_CDC_RX_EPSIZE            0x40

#ifndef __cplusplus
#define USB_CDC_DECLARE_DEV_DESC(vid, pid)                           \
  {                                                                     \
      .bLength            = sizeof(usb_descriptor_device),              \
      .bDescriptorType    = USB_DESCRIPTOR_TYPE_DEVICE,                 \
      .bcdUSB             = 0x0200,                                     \
      .bDeviceClass       = USB_DEVICE_CLASS_CDC,                       \
      .bDeviceSubClass    = USB_DEVICE_SUBCLASS_CDC,                    \
      .bDeviceProtocol    = 0x00,                                       \
      .bMaxPacketSize0    = 0x40,                                       \
      .idVendor           = vid,                                        \
      .idProduct          = pid,                                        \
      .bcdDevice          = 0x0200,                                     \
      .iManufacturer      = 0x01,                                       \
      .iProduct           = 0x02,                                       \
      .iSerialNumber      = 0x03,                                       \
      .bNumConfigurations = 0x01,                                       \
 }
#endif

/*
 * CDC ACM interface
 */

void usb_cdcacm_enable();
void usb_cdcacm_disable();

typedef struct usb_cdcacm_line_coding {
    uint32_t dwDTERate;           /* Baud rate */

#define USB_CDC_STOP_BITS_1   0
#define USB_CDC_STOP_BITS_1_5 1
#define USB_CDC_STOP_BITS_2   2
    uint8_t bCharFormat;          /* Stop bits */

#define USB_CDC_PARITY_NONE  0
#define USB_CDC_PARITY_ODD   1
#define USB_CDC_PARITY_EVEN  2
#define USB_CDC_PARITY_MARK  3
#define USB_CDC_PARITY_SPACE 4
    uint8_t bParityType;          /* Parity type */

    uint8_t bDataBits;            /* Data bits: 5, 6, 7, 8, or 16 */
} __attribute__((packed)) usb_cdcacm_line_coding;

/* Retrieve a copy of the current line coding structure. */
void usb_cdcacm_get_line_coding(usb_cdcacm_line_coding*);

/* Line coding conveniences. */
int usb_cdcacm_get_baud(void);        /* dwDTERate */
int usb_cdcacm_get_stop_bits(void);   /* bCharFormat */
int usb_cdcacm_get_parity(void);      /* bParityType */
int usb_cdcacm_get_n_data_bits(void); /* bDataBits */

#ifdef __cplusplus
}
#endif

#endif
