#ifndef _USB_DEV_H_
#define _USB_DEV_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include "usb.h"

#define EP_DIR_OUT     1
#define EP_DIR_IN      2

#define USB_BUF_STATUS_LOCKED      1
#define USB_BUF_STATUS_SENT        2
#define USB_BUF_STATUS_MLP         4

#define USB_EPBUF_NUM         16


typedef struct {
	uint16_t wValue; 
	uint16_t wIndex; 
	uint32_t length; 
	uint8_t * address;
} usb_descriptor_list_t;

struct usb_ep_buffer_t;

typedef struct usb_ep_buffer_t{
	uint32_t count; 
	uint32_t index;
	uint32_t status;
	struct usb_ep_buffer_t * next;
	union {
		uint8_t data8[64];
		uint32_t data32[16];
	};
} usb_ep_buffer_t;

typedef struct usb_ep_config_t{
	struct usb_ep_buffer_t * buffer;
	void (* callback)(void);
} usb_ep_config_t;

extern usb_dev_state_type usb_device_state;
extern uint32_t  usb_configuration_number;
extern uint32_t  usb_interface_number;

extern usb_descriptor_list_t descriptor_table[];
extern int descriptor_table_length;

extern usb_ep_buffer_t ep_buffers[];
extern usb_ep_config_t config_ep_in[];
extern usb_ep_config_t config_ep_out[];

/* MCU specific functions */
void usb_init(void);

void usb_tx_ep0(uint8_t * buf,uint32_t size);
uint32_t usb_rxdata_ep0(uint8_t * buf,uint32_t size);

void usb_send_status_IN();
void usb_read_status_OUT();
void usb_send_status_stall();

void usb_ep0_stall();

int usb_tx_ep_idle(uint32_t epnum);
void usb_tx_epn(uint32_t epnum);

void usb_set_address(uint32_t address);

void usb_enable_ep(uint32_t epnum,uint32_t eptype,uint32_t epdir);

/* Generic funtions */
void usb_control(USB_SETUP_PACKET * request);

#ifdef __cplusplus
}
#endif

#endif
