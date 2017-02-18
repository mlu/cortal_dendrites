#include <usb_midi_device.h>

#include <usb.h>

/* usb_lib headers */
#include <stdint.h>
#include "usb_dev.h"

#define USB_MIDI_RX_BUFFERSIZE	64
#define USB_MIDI_RX_BUFFERMASK	0x3F

/* Received data is saved in ring buffer */
uint32_t usbmidiBufferRx[USB_MIDI_RX_BUFFERSIZE];
/* Read index into vcomBufferRx */
uint32_t rx_head = 0;
/* Write index into vcomBufferRx */
uint32_t rx_tail = 0;


/*
 * MIDI interface
 */

void usb_midi_send_noteon(uint32_t note, uint32_t velocity, uint32_t channel) {
	USB_MIDI_Event_Packet mp;
	mp.packet.CN_CIN = USBMIDI_CIN_NOTEON;
	mp.packet.MIDI_0 = MIDI_NOTEON | (0xF&(channel-1));
	mp.packet.MIDI_1 = 0x7F & note;
	mp.packet.MIDI_2 = 0x7F & velocity;
	usb_midi_putpacket(mp.data32);
};

void usb_midi_send_noteoff(uint32_t note, uint32_t velocity, uint32_t channel) {
	USB_MIDI_Event_Packet mp;
	mp.packet.CN_CIN = USBMIDI_CIN_NOTEOFF;
	mp.packet.MIDI_0 = MIDI_NOTEOFF | (0xF&(channel-1));
	mp.packet.MIDI_1 = 0x7F & note;
	mp.packet.MIDI_2 = 0x7F & velocity;
	usb_midi_putpacket(mp.data32);
};

void usb_midi_sendPolyPressure(uint32_t note, uint32_t pressure, uint32_t channel);
void usb_midi_sendControlChange(uint32_t control, uint32_t value, uint32_t channel);
void usb_midi_sendProgramChange(uint32_t program, uint32_t channel);
void usb_midi_sendAfterTouch(uint32_t pressure, uint32_t channel);
void usb_midi_sendPitchBend(uint32_t value, uint32_t channel);

/***************************************************************************
*
*    Receiver callback forward declaration
*
****************************************************************************/

static void usbmidiDataRxCb(void);

/************************************************************
 * 
 * Same API as Teensy 3 ??
 * 
 * **********************************************************/

int usb_midi_available(void) {
	return (rx_tail-rx_head)&USB_MIDI_RX_BUFFERMASK;
};

uint32_t usb_midi_getpacket(void) {
	uint32_t avail = usb_midi_available();
	uint32_t c = 0;
	if (avail) {
		c = usbmidiBufferRx[rx_head];
		rx_head = (rx_head+1)&USB_MIDI_RX_BUFFERMASK;
	}
	return c;
};

uint32_t usb_midi_peekpacket(void) {
	if (usb_midi_available()) return usbmidiBufferRx[rx_head];
	else return 0;
};

int usb_midi_read(void *buffer, uint32_t size)
{
	uint32_t count = size;
	uint32_t avail = usb_midi_available();
	int c = -1;
	if (avail) {
		uint8_t * buf8 = buffer; 
		int i;
		if (avail < count) count = avail;
		while (buf8<(uint8_t *)buffer+count) {
			*buf8++ = usbmidiBufferRx[rx_head];
			rx_head = (rx_head+1)&USB_MIDI_RX_BUFFERMASK;
		}
	}
	return count;

}

int usb_midi_putpacket(uint32_t mp)
{
	return usb_midi_write(&mp, 1);
}

int usb_midi_write(const uint32_t *buffer, uint32_t size)
{
	uint32_t i, count;
	usb_ep_buffer_t * epb = config_ep_in[USB_MIDI_TX_ENDP].buffer;
	/* Lock buffer while writing to it */
	epb->status |= USB_BUF_STATUS_LOCKED;
	count = 16 - epb->count;
	if (size<count) count=size;
	for (i = 0; i<count; i++) {
		epb->data32[i+(epb->count/4)] = buffer[i];
	}
	epb->count += 4*count;
	epb->status &= ~USB_BUF_STATUS_LOCKED;
	return count;	
}

/************************************************************
 * 
 * Callbacks from IRQ
 * 
 * **********************************************************/

static void usbmidiDataRxCb(void) {
	uint32_t freespace = (rx_head-rx_tail-1)&USB_MIDI_RX_BUFFERMASK;
	usb_ep_buffer_t * epb = config_ep_out[USB_MIDI_RX_ENDP].buffer;
	uint32_t tail = rx_tail;
    uint32_t count32 = (epb->count - epb->index)/4;
    uint32_t * pbuf = (uint32_t *)&epb->data8[epb->index];
    if (freespace < count32) count32 = freespace; /* OVERFLOW !! */
    uint32_t * ebuf = (uint32_t *)&epb->data8[epb->index+4*count32];
    
    while (pbuf<ebuf) {
		usbmidiBufferRx[tail] = *(pbuf++);
		tail = (tail+1)&USB_MIDI_RX_BUFFERMASK;
	}
	epb->index+=4*count32;
	rx_tail = tail;
	/* Reenable reception of OUT packets if rx buffer is empty */
	if (epb->count == epb->index) {   /* (epb->count == epb->index) means buffer is idle, all data has been read */
		usb_rx_epn(USB_MIDI_RX_ENDP);
	}
}

/************************************************************
 * 
 * Configuration and setup
 * 
 * **********************************************************/

void usb_set_configuration(uint32_t wValue) {
	usb_configuration_number = wValue;
	usb_enable_ep(USB_MIDI_TX_ENDP,USB_EP_TYPE_BULK, EP_DIR_IN);
	usb_enable_ep(USB_MIDI_RX_ENDP,USB_EP_TYPE_BULK, EP_DIR_OUT);
	config_ep_out[USB_MIDI_RX_ENDP].callback = usbmidiDataRxCb;
	usb_device_state = USB_CONFIGURED;
	rx_head = 0;
	rx_tail = 0;
}

void usb_class_request(USB_SETUP_PACKET * request) {
	/* No supported Class Requests */
	usb_send_status_stall();
}

