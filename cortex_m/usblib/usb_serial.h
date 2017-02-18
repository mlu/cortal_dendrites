#ifndef _USB_SERIAL_H_
#define _USB_SERIAL_H_

#include <stdint.h>
#include "usb_cdc.h"

#ifdef __cplusplus
extern "C" {
#endif


/****************  Utilities *************/

uint8_t dtr(void);
uint8_t rts(void);

/***************  C interface ************/

int usb_serial_getchar(void);
int usb_serial_peekchar(void);
size_t usb_serial_available(void);
size_t usb_serial_read(void *buffer, uint32_t size);

size_t usb_serial_putchar(uint8_t c);
size_t usb_serial_write(const uint8_t *buffer, uint32_t size);

/**********************************/

void   usb_cdcacm_putc(char ch);
uint32_t usb_cdcacm_tx(const uint8_t* buf, uint32_t len);
uint32_t usb_cdcacm_rx(uint8_t* buf, uint32_t len);
uint32_t usb_cdcacm_peek(uint8_t* buf, uint32_t len);

uint32_t usb_cdcacm_data_available(void); /* in RX buffer */
uint16_t usb_cdcacm_get_pending(void);
uint8_t usb_cdcacm_is_transmitting(void);

uint8_t usb_cdcacm_get_dtr(void);
uint8_t usb_cdcacm_get_rts(void);

/* Line coding conveniences. */
int usb_cdcacm_get_baud(void);        /* dwDTERate */
int usb_cdcacm_get_stop_bits(void);   /* bCharFormat */
int usb_cdcacm_get_parity(void);      /* bParityType */
int usb_cdcacm_get_n_data_bits(void); /* bDataBits */

#ifdef __cplusplus
}
#endif

#endif
