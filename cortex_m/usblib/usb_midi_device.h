#ifndef __USB_MIDI_DEVICE_H_
#define __USB_MIDI_DEVICE_H_

#include <stdint.h>
#include <usb.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MIDI_MISC	    		0x0
#define MIDI_SYSTEM_COMMON2		0x2
#define MIDI_SYSTEM_COMMON3		0x3
#define MIDI_NOTEOFF  			0x80
#define MIDI_NOTEON  			0x90
#define MIDI_POLY_KEYPRESS		0xA0
#define MIDI_CONTROL_CHANGE  		0xB0
#define MIDI_PROGRAM_CHANGE  		0xC0
#define MIDI_CHANNEL_PRESSURE		0xD0


typedef struct {
	uint8_t CN_CIN;
	uint8_t MIDI_0;
	uint8_t MIDI_1;	
	uint8_t MIDI_2;	
} __attribute__((packed)) MIDI_EVENT_PACKET_t;

typedef union {
    uint8_t  bytes[4];
    uint32_t data32;
    MIDI_EVENT_PACKET_t packet;
} USB_MIDI_Event_Packet;

#define USBMIDI_CIN_MISC	    		0x0
#define USBMIDI_CIN_SYSTEM_COMMON2		0x2
#define USBMIDI_CIN_SYSTEM_COMMON3		0x3
#define USBMIDI_CIN_NOTEOFF  			0x8
#define USBMIDI_CIN_NOTEON  			0x9
#define USBMIDI_CIN_POLY_KEYPRESS		0xA
#define USBMIDI_CIN_CONTROL_CHANGE  		0xB
#define USBMIDI_CIN_PROGRAM_CHANGE  		0xC
#define USBMIDI_CIN_CHANNEL_PRESSURE		0xD

/*
 * USB MIDI Requests
 */

/*
 * USB Device configuration
 */

#define LUFA_ID_VENDOR     		0x03EB 
#define LUFA_ID_MIDI_DEVICE     0x2048

#define MAX_POWER (100 >> 1)
 
/*
 * Endpoint configuration
 */

#define USB_MIDI_CTRL_ENDP            0
#define USB_MIDI_CTRL_EPSIZE          0x40

#define USB_MIDI_TX_ENDP              1
#define USB_MIDI_TX_EPSIZE            0x40

#define USB_MIDI_RX_ENDP              1
#define USB_MIDI_RX_EPSIZE            0x40

/*
 * 
 * MIDI interface
 * 
 */

void usb_midi_send_noteon(uint32_t note, uint32_t velocity, uint32_t channel);
void usb_midi_send_noteoff(uint32_t note, uint32_t velocity, uint32_t channel);
void usb_midi_sendPolyPressure(uint32_t note, uint32_t pressure, uint32_t channel);
void usb_midi_sendControlChange(uint32_t control, uint32_t value, uint32_t channel);
void usb_midi_sendProgramChange(uint32_t program, uint32_t channel);
void usb_midi_sendAfterTouch(uint32_t pressure, uint32_t channel);
void usb_midi_sendPitchBend(uint32_t value, uint32_t channel);

/*
 * 
 * USB MIDI interface
 * 
 */


int usb_midi_available(void); /* in RX buffer */
uint32_t usb_midi_getpacket(void);
uint32_t usb_midi_peekpacket(void);
int usb_midi_putpacket(uint32_t mp);

#ifdef __cplusplus
}
#endif

#endif
