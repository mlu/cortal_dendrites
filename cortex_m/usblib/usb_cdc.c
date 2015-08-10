#include <stddef.h>
#include "usb_serial.h"
#include "usb_cdc.h"
#include "usb_dev.h"

#define USB_CDC_RX_BUFFERSIZE	256
#define USB_CDC_RX_BUFFERMASK	0xFF

/* Received data is saven in ring buffer */
uint8_t vcomBufferRx[USB_CDC_RX_BUFFERSIZE];
/* Read index into vcomBufferRx */
uint32_t rx_head = 0;
/* Write index into vcomBufferRx */
uint32_t rx_tail = 0;

/* Transmit data is written to EP USB device buffer */

/* Other state (line coding, DTR/RTS) */
static volatile usb_cdcacm_line_coding line_coding = {
    /* This default is 115200 baud, 8N1. */
    .dwDTERate   = 115200,
    .bCharFormat = USB_CDC_STOP_BITS_1,
    .bParityType = USB_CDC_PARITY_NONE,
    .bDataBits   = 8,
};

/* DTR in bit 0, RTS in bit 1. */
static volatile uint8_t usb_cdc_line_rtsdtr;


static void vcomDataRxCb(void);

/*
 * CDC ACM interface
 */

void usb_cdc_enable() {
    /* Present ourselves to the host. Writing 0 to "disc" pin must
     * pull USB_DP pin up while leaving USB_DM pulled down by the
     * transceiver. See USB 2.0 spec, section 7.1.7.3. */
}

void usb_cdc_disable() {

}

void usb_cdcacm_get_line_coding(usb_cdcacm_line_coding *ret) {
    ret->dwDTERate = line_coding.dwDTERate;
    ret->bCharFormat = line_coding.bCharFormat;
    ret->bParityType = line_coding.bParityType;
    ret->bDataBits = line_coding.bDataBits;
}

int usb_cdcacm_get_baud(void) {
    return line_coding.dwDTERate;
}

int usb_cdcacm_get_stop_bits(void) {
    return line_coding.bCharFormat;
}

int usb_cdcacm_get_parity(void) {
    return line_coding.bParityType;
}

int usb_cdcacm_get_n_data_bits(void) {
    return line_coding.bDataBits;
}

int _write(int file, char *ptr, int len)
{
	int count = 0;
	while (count<len) count += usb_serial_write(ptr+count, len-count);
	return 0;
}

/************************************************************
 * 
 * Teensy 3/Arduino USerial class functions
 * 
 * **********************************************************/

uint8_t dtr(void) { return (usb_cdc_line_rtsdtr & USB_CDC_CONTROL_LINE_DTR) ? 1 : 0; }

uint8_t rts(void) { return (usb_cdc_line_rtsdtr & USB_CDC_CONTROL_LINE_RTS) ? 1 : 0; }

/************************************************************
 * 
 * Same API as Teensy 3
 * 
 * **********************************************************/

int usb_serial_available(void) {
	return (rx_tail-rx_head)&USB_CDC_RX_BUFFERMASK;
};

int usb_serial_getchar(void) {
	uint32_t avail = usb_serial_available();
	int c = -1;
	if (avail) {
		c = vcomBufferRx[rx_head];
		rx_head = (rx_head+1)&USB_CDC_RX_BUFFERMASK;
	}
	if (avail<64) 
		if (config_ep_out[USB_CDC_RX_ENDP].buffer->count != config_ep_out[USB_CDC_RX_ENDP].buffer->index)  /* More data to read in the buffer */
			vcomDataRxCb();
	return c;
};

int usb_serial_peekchar(void) {
	if (usb_serial_available()) return vcomBufferRx[rx_head];
	else return -1;
};

int usb_serial_read(void *buffer, uint32_t size)
{
	uint32_t count = size;
	uint32_t avail = usb_serial_available();
	int c = -1;
	if (avail) {
		uint8_t * buf8 = buffer; 
		int i;
		if (avail < count) count = avail;
		while (buf8<(uint8_t *)buffer+count) {
			*buf8++ = vcomBufferRx[rx_head];
			rx_head = (rx_head+1)&USB_CDC_RX_BUFFERMASK;
		}
	}
	return count;
}

int usb_serial_putchar(uint8_t c)
{
	return usb_serial_write(&c, 1);
}

int usb_serial_write(const uint8_t *buffer, uint32_t size)
{
	uint32_t i, count;
	usb_ep_buffer_t * epb = config_ep_in[USB_CDC_TX_ENDP].buffer;
	/* Lock buffer while writing to it */
	epb->status |= USB_BUF_STATUS_LOCKED;
	count = 64 - epb->count;
	if (size<count) count=size;
	for (i = 0; i<count; i++) {
		epb->data8[epb->count++] = buffer[i];
	}
	epb->status &= ~USB_BUF_STATUS_LOCKED;
	epb->status &= ~USB_BUF_STATUS_LOCKED;
	return count;	
}

/************************************************************
 * 
 * Callbacks from IRQ
 * 
 * **********************************************************/

static void vcomDataTxCb(void) {
}

/* CDC Receive callback, only called from IRQ */
static void vcomDataRxCb(void) {
	uint32_t available = (rx_head-rx_tail-1)&USB_CDC_RX_BUFFERMASK;
	usb_ep_buffer_t * epb = config_ep_out[USB_CDC_RX_ENDP].buffer;
	uint32_t tail = rx_tail;
    uint32_t count = epb->count - epb->index;
    uint8_t * pbuf = &epb->data8[epb->index];
    if (available < count) count = available; /* OVERFLOW !! */
    uint8_t * ebuf = pbuf+count;
    
    while (pbuf<ebuf) {
		vcomBufferRx[tail] = *(pbuf++);
		tail = (tail+1)&USB_CDC_RX_BUFFERMASK;
	}
	epb->index+=count;
	rx_tail = tail;
	/* Reenable reception of OUT packets if rx buffer is empty */
	if (epb->count == epb->index) {   /* (epb->count == epb->index) means buffer is idle, all data has been read */
		usb_rx_epn(USB_CDC_RX_ENDP);
	}
}

/************************************************************
 * 
 * Configuration and setup
 * 
 * **********************************************************/

void usb_set_configuration(uint32_t wValue) {
	usb_configuration_number = wValue;
	usb_enable_ep(1,USB_EP_TYPE_BULK,EP_DIR_IN | EP_DIR_OUT);
	config_ep_out[1].callback = vcomDataRxCb;
	usb_enable_ep(2,USB_EP_TYPE_INTERRUPT,EP_DIR_IN);
	usb_device_state = USB_CONFIGURED;
	/* TODO, perhaps FIFO configuration should be done here, BUT it feels too processor specific */
	rx_head = 0;
	rx_tail = 0;
}

void usb_class_request(USB_SETUP_PACKET * request) {
	if (request->bRequest == USB_CDC_SET_LINE_CODING) {
		usb_rxdata_ep0((uint8_t*)&line_coding, sizeof(line_coding));
		usb_send_status_IN();
		return;
	}
	if (request->bRequest == USB_CDC_GET_LINE_CODING) {
		usb_send_status_stall();
		return;
	}
	if (request->bRequest == USB_CDC_SET_COMM_FEATURE) {
		usb_send_status_stall();
		return;
	}
	if (request->bRequest == USB_CDC_SET_CONTROL_LINE_STATE) {
		usb_cdc_line_rtsdtr = request->wValue;
		usb_send_status_IN();
		return;
	}	
	if (request->bRequest == USB_CDC_SEND_BREAK) {
		usb_send_status_IN();
		return;
	}
	usb_send_status_stall();
}


