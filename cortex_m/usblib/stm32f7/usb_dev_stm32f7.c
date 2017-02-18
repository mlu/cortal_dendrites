#include <stm32f7xx.h>
#include "usb_regs.h"
#include "usb_dev.h"

/* Global RX FIFO Packet status defines */
#define GRXSTSP_PKTSTS_SETUP  0x6

#define USE_DEBUGLOG
#ifdef USE_DEBUGLOG
__attribute__ ((section(".systemram"))) uint32_t dbix, debugdata[1000];
#define DEBUGLOG(id,data) if (dbix<1000) {debugdata[dbix]=id;debugdata[dbix+1]=data;dbix+=2; };
#define DEBUGLOGCORE(id,data) if (dbix<1000) {debugdata[dbix]=id;debugdata[dbix+1]=data;dbix+=2;  };
#else
__attribute__ ((section(".systemram"))) uint32_t dbix, uint32_t debugdata[1000];
#define DEBUGLOG(id,data) {;};
#define DEBUGLOGCORE(id,data) if (dbix<1000) {debugdata[dbix++]=id;debugdata[dbix++]=data; };
#endif

uint32_t ep0_setup[2];

uint32_t ep0_tx_count;
uint32_t * ep0_tx_buf;

int usb_tx_ep_idle(uint32_t epnum) {
	/* Test if endpoint is active and not enabled */
	return (USB_FS_INEP[epnum].DIEPCTL & (USB_DEPCTL_EPENA_Msk | USB_DEPCTL_USBAEP_Msk))==USB_DEPCTL_USBAEP_Msk;
}

/* Transmit a single packet */
void usb_tx_epn(uint32_t epnum)
{
	volatile uint32_t * dfifon = (uint32_t *) (USB_FS_BASE_ADDR + USB_DATA_FIFO_OFFSET + epnum*USB_DATA_FIFO_SIZE);
	usb_ep_buffer_t * epbuf = config_ep_in[epnum].buffer;
	uint32_t wcount = (epbuf->count+3)/4;
	DEBUGLOG(0x99,epbuf->count)
	/* Transfer size */
	USB_FS_INEP[epnum].DIEPTSIZ = (1<<USB_DIEPTSIZ_PKTCNT_Pos) | ((epbuf->count)<<USB_DIEPTSIZ_XFRSIZ_Pos);
	/* Enable EPn IN */
	USB_FS_INEP[epnum].DIEPCTL |= USB_DEPCTL_EPENA_Msk | USB_DEPCTL_CNAK_Msk;
	/* Prepare to receive more transfer requests */
	USB_FS_DREGS->DIEPMSK |= USB_DIEPINT_ITTXFE_Msk;
	/* Write to FIFO */
	int i;
	for (i=0; i<wcount; i++) {
		*(dfifon) = epbuf->data32[i];
	}
	if (epbuf->count == 64) epbuf->status |= USB_BUF_STATUS_MLP;
	else epbuf->status &= ~USB_BUF_STATUS_MLP;
	epbuf->count = 0;
};

void usb_tx_ep0(uint8_t * buf,uint32_t size) {
	/* Write packet into EP0 Tx FIFO */
	uint32_t dtxfsts = USB_FS_INEP[0].DTXFSTS;
	ep0_tx_buf =(uint32_t *)buf;
	ep0_tx_count = (size+3)/4;
	uint32_t pktcount = (ep0_tx_count+15)/16;
	uint32_t txcount = ep0_tx_count;
	/* Transfer size */
	USB_FS_INEP[0].DIEPTSIZ = (pktcount<<USB_DIEPTSIZ_PKTCNT_Pos) | (size<<USB_DIEPTSIZ_XFRSIZ_Pos);
	/* Enable EP0 IN */
	USB_FS_INEP[0].DIEPCTL = USB_DEPCTL_EPENA_Msk | USB_DEPCTL_CNAK_Msk;
	DEBUGLOG(9,size)
	if (txcount>16) txcount = 16;
	if ( dtxfsts == 16 ) {
		volatile uint32_t * dfifo0 = (uint32_t *) (USB_FS_BASE_ADDR + USB_DATA_FIFO_OFFSET);
		int i;
		/* Write data to FIFO */
		for (i=0; i<txcount; i++) {
			*(dfifo0) = *(ep0_tx_buf++);
		}
		ep0_tx_count -= txcount;	
	}
	if (ep0_tx_count>0) {
		USB_FS_DREGS->DIEPEMPMSK = 1;
	}
}

void usb_tx_more_ep0() {
	/* Write packet into EP0 Tx FIFO */
	uint32_t dtxfsts = USB_FS_INEP[0].DTXFSTS;
	uint32_t txcount = ep0_tx_count;
	if (txcount>16) txcount = 16;
	DEBUGLOG(9,ep0_tx_count)
	while (USB_FS_INEP[0].DTXFSTS < 16) ; /* This might hang the application */
	if ( dtxfsts == 16 ) {
		volatile uint32_t * dfifo0 = (uint32_t *) (USB_FS_BASE_ADDR + USB_DATA_FIFO_OFFSET);
		int i;
		/* Write data to FIFO */
		for (i=0; i<txcount; i++) {
			*(dfifo0) = *(ep0_tx_buf++);
		}
		ep0_tx_count -= txcount;	
	}
	if (ep0_tx_count==0) {
		USB_FS_DREGS->DIEPEMPMSK = 0; /* ???????????? Check this */
	}
}

void usb_rx_ep0(uint32_t bcount) {
	/* Read packet from EP0 Rx FIFO */
	if (bcount>0) {
		int i;
		uint32_t count = (bcount+3)/4;
		volatile uint32_t * dfifo0 = (uint32_t *) (USB_FS_BASE_ADDR + USB_DATA_FIFO_OFFSET);
		for (i=0; i<count; i++) {
			config_ep_out[0].buffer->data32[i] = *(dfifo0);
		}
		config_ep_out[0].buffer->count = bcount;
		config_ep_out[0].buffer->index = 0;
	}
}

void usb_rxsetup_ep0(uint32_t bcount) {
	/* Read setup packet from EP0 Rx FIFO */
	if (bcount==8) {
		int i;
		volatile uint32_t * dfifo0 = (uint32_t *) (USB_FS_BASE_ADDR + USB_DATA_FIFO_OFFSET);
		for (i=0; i<2; i++) {
			ep0_setup[i] = *(dfifo0);
		}
	}
	else
	{
	/* ERROR */	
	}
}

void usb_rxfifo_epn(uint32_t ep, uint32_t bcount) {
	if (bcount>0) {
		int i;
		uint32_t count = (bcount+3)/4;
		volatile uint32_t * dfifo0 = (uint32_t *) (USB_FS_BASE_ADDR + USB_DATA_FIFO_OFFSET);
		uint32_t * pbuf = config_ep_out[ep].buffer->data32;
		uint32_t * ebuf = pbuf+count;		
		while (pbuf < ebuf) {
			*(pbuf++) = *(dfifo0);
		}
		config_ep_out[ep].buffer->count = bcount;
		config_ep_out[ep].buffer->index = 0;
		if (config_ep_out[ep].callback) config_ep_out[ep].callback();
		/* SNAK the EP until buffer is free */
	}
}

void usb_rxflvl() {
	/* Pop the Rx FIFO status */
	uint32_t grxstsp = USB_FS_GREGS->GRXSTSP;
	uint32_t bcount = (grxstsp>>4)&0x7FF;
	uint32_t pktsts = (grxstsp>>17)&0xF;
	uint32_t epnum = (grxstsp)&0xF;
	DEBUGLOG(4,grxstsp)
	if (pktsts==GRXSTSP_PKTSTS_SETUP) usb_rxsetup_ep0(bcount);
	else if (bcount>0) {
		if (epnum==0) usb_rx_ep0(bcount);
		else usb_rxfifo_epn(epnum, bcount);
	}
}

/* Enable OUT EP to receive data */
void usb_rx_epn(uint32_t epnum) {
	USB_FS_OUTEP[epnum].DOEPTSIZ = (1<<USB_DOEPTSIZ_PKTCNT_Pos) | (64<<USB_DOEPTSIZ_XFRSIZ_Pos);
	USB_FS_OUTEP[epnum].DOEPCTL |= USB_DEPCTL_EPENA_Msk | USB_DEPCTL_CNAK_Msk;
}

void usb_send_status_IN() {
	USB_FS_INEP[0].DIEPTSIZ = (1<<USB_DIEPTSIZ0_PKTCNT_Pos);
	USB_FS_INEP[0].DIEPCTL = USB_DEPCTL_EPENA_Msk | USB_DEPCTL_CNAK_Msk;
}

void usb_send_status_stall() {
	USB_FS_INEP[0].DIEPCTL = USB_DEPCTL_EPENA_Msk | USB_DEPCTL_STALL_Msk;
}

void usb_read_status_OUT() {
	USB_FS_OUTEP[0].DOEPTSIZ = (1<<USB_DOEPTSIZ0_PKTCNT_Pos) | (64<<USB_DOEPTSIZ0_XFRSIZ_Pos);
	USB_FS_OUTEP[0].DOEPCTL |= USB_DEPCTL_EPENA_Msk | USB_DEPCTL_CNAK_Msk;
}

/* Blocking read of DATA OUT packets on EP0 */
uint32_t usb_rxdata_ep0(uint8_t * buf,uint32_t size) {
	int i;
	uint8_t * epbuf = config_ep_out[0].buffer->data8;
	USB_FS_OUTEP[0].DOEPTSIZ = (1<<USB_DOEPTSIZ0_PKTCNT_Pos) | (size<<USB_DOEPTSIZ0_XFRSIZ_Pos);
	USB_FS_OUTEP[0].DOEPCTL |= USB_DEPCTL_EPENA_Msk | USB_DEPCTL_CNAK_Msk;
	while ((USB_FS_GREGS->GINTSTS & USB_GINTSTS_RXFLVL_Msk)==0) ; /* wait */
	usb_rxflvl();
	for (i=0;i<size;i++) buf[i] = epbuf[i];
};

/***********************************************************************
 * 
 * Initialisation and Configuration
 * 
 * 
 * *********************************************************************/
 
void usb_reset() {
	int i;
	DEBUGLOG(2,0)

	/* Flush FIFO */
	USB_FS_GREGS->GRSTCTL = USB_GRSTCTL_RXFFLSH_Msk;
	while (USB_FS_GREGS->GRSTCTL & USB_GRSTCTL_RXFFLSH_Msk) ;
	USB_FS_GREGS->GRSTCTL = USB_GRSTCTL_TXFFLSH_Msk | (0x10 << USB_GRSTCTL_TXFNUM_Pos);
	while (USB_FS_GREGS->GRSTCTL & USB_GRSTCTL_TXFFLSH_Msk) ;
	
	for (i=0; i<USB_EPBUF_NUM; i++) {
		ep_buffers[i].index = 0;
		ep_buffers[i].count = 0;
		ep_buffers[i].status = 0;
	}

	USB_FS_DREGS->DCFG = ( 3 << USB_DCFG_DSPD_Pos) | ( 0 << USB_DCFG_NZLSOHSK_Pos)  ;
	USB_FS_OUTEP[0].DOEPCTL = USB_DEPCTL_SNAK_Msk;
	USB_FS_OUTEP[1].DOEPCTL = USB_DEPCTL_SNAK_Msk;
	USB_FS_OUTEP[2].DOEPCTL = USB_DEPCTL_SNAK_Msk;
	USB_FS_OUTEP[3].DOEPCTL = USB_DEPCTL_SNAK_Msk;
	/* Unmask EP0 interrupts */
	USB_FS_DREGS->DAINTMSK = (1<<USB_DAINTMSK_OEPM_Pos) | (1<<USB_DAINTMSK_IEPM_Pos);
	USB_FS_DREGS->DOEPMSK = USB_DOEPINT_STUP_Msk | USB_DOEPINT_XFRC_Msk;
	USB_FS_DREGS->DIEPMSK = USB_DIEPINT_XFRC_Msk | USB_DIEPINT_TOC_Msk;
	//USB_FS_DREGS->DIEPMSK |=  USB_DIEPINT_ITTXFE_Msk; /* Not needed when device controls sending of data */
	/* Configre reception of setup packets */
	USB_FS_OUTEP[0].DOEPTSIZ = (3<<USB_DOEPTSIZ0_STUPCNT_Pos);
	
}

void usb_enum_done() {
	uint32_t dsts = USB_FS_DREGS->DSTS;
	DEBUGLOG(3,dsts)
	if ((dsts&0x6)==0x6) {
		USB_FS_INEP[0].DIEPCTL = 0;
		USB_FS_OUTEP[0].DOEPCTL = 0x80000000;
	}
}

void usb_set_address(uint32_t address) {
	USB_FS_DREGS->DCFG |= (address & 0x7F)<<USB_DCFG_DAD_Pos;
};

void usb_enable_ep(uint32_t epnum,uint32_t eptype,uint32_t epdir) {
	if (epnum==0) return;
	if (epnum>3) return;
	if (epdir & EP_DIR_OUT) {
		USB_FS_OUTEP[epnum].DOEPCTL = USB_DEPCTL_USBAEP_Msk | USB_DEPCTL_SD0PID_Msk | 64;
		USB_FS_OUTEP[epnum].DOEPCTL |= (eptype<<USB_DEPCTL_EPTYP_Pos);
		USB_FS_DREGS->DAINTMSK |= (1<<(USB_DAINTMSK_OEPM_Pos+epnum));
		config_ep_out[epnum].buffer->count=0;
		config_ep_out[epnum].buffer->index=0;
		config_ep_out[epnum].buffer->status=0;
		usb_rx_epn(epnum);
	}
	if (epdir & EP_DIR_IN) {
		uint32_t diepctl = USB_DEPCTL_USBAEP_Msk | USB_DEPCTL_SD0PID_Msk | 64;
		diepctl |= (eptype<<USB_DEPCTL_EPTYP_Pos);
		diepctl |= epnum<<USB_DEPCTL_TXFNUM_Pos;
		diepctl |= USB_DEPCTL_CNAK_Msk;
		USB_FS_INEP[epnum].DIEPCTL = diepctl;
		USB_FS_DREGS->DAINTMSK |= (1<<(USB_DAINTMSK_IEPM_Pos+epnum));
		USB_FS_DREGS->DIEPMSK |= USB_DIEPINT_ITTXFE_Msk;
		config_ep_in[epnum].buffer->count=0;
		config_ep_in[epnum].buffer->index=0;
		config_ep_in[epnum].buffer->status=0;
	}
};

void usb_init_device(void)
{
	int i;
	
	/* Device mode initialisation 16.8 */	
	USB_FS_GREGS->GINTMSK |= USB_GINTMSK_USBRST_Msk | USB_GINTMSK_ENUMDNEM_Msk | 
	                             USB_GINTMSK_ESUSPM_Msk | USB_GINTMSK_USBSUSPM_Msk;
	USB_FS_GREGS->GINTMSK |= USB_GINTMSK_WUIM_Msk;
	USB_FS_GREGS->GINTMSK |= USB_GINTMSK_RXFLVLM_Msk;
	USB_FS_GREGS->GINTMSK |= USB_GINTMSK_OEPINT_Msk;
	USB_FS_GREGS->GINTMSK |= USB_GINTMSK_IEPINT_Msk;
	USB_FS_GREGS->GINTMSK |= USB_GINTMSK_SOFM_Msk;
	
	/* Power up the tranciever and enable VBUS sensing */
	USB_FS_GREGS->GCCFG = USB_OTG_GCCFG_PWRDWN | USB_OTG_GCCFG_VBDEN;
	
	USB_FS_DREGS->DCFG = ( 3 << USB_DCFG_DSPD_Pos) | ( 0 << USB_DCFG_NZLSOHSK_Pos)  ;

	/* Force B session valid */
	//USB_FS_GREGS->GOTGCTL = USB_OTG_GOTGCTL_BVALOEN | USB_OTG_GOTGCTL_BVALOVAL;

	/* Clear the DCTL.SftDiscon */
	USB_FS_DREGS->DCTL &= ~USB_DCTL_SDIS_Msk;

	/* FIFO RAM Allocation - this really depends on device specific EP configuration */
	USB_FS_GREGS->GRXFSIZ = 0x40;  			/* RXFIFO 64 DWORDS = 256 bytes */
	USB_FS_GREGS->DIEPTXF0_HNPTXFSIZ = 0x00100040;  /* 16 DWORDS EP0 */
	USB_FS_GREGS->DIEPTXF[0]  = 0x00400050;   		/* 4*16 DWORDS EP1 IN */
	USB_FS_GREGS->DIEPTXF[1]  = 0x00400090;   		/* 4*16 DWORDS EP2 IN */
	USB_FS_GREGS->DIEPTXF[2]  = 0x004000D0;   		/* 4*16 DWORDS EP3 IN */
}

void usb_init(void)
{
//#ifdef USE_DEBUGLOG
	int i;
	dbix = 0;
	for (i=0; i<sizeof(debugdata)/4;i++) debugdata[i]=0;
//#endif
	/* USB Clock and module enable */
	RCC->AHB2ENR |= RCC_AHB2ENR_OTGFSEN;
	/* Configure peripherial pins */
	/* OTG_FS_VBUS is PA9 */
	/* OTG_FS_DM is PA11 */
	/* OTG_FS_DP is PA12 */
	/* Alternate function mode for PA9, PA11 and PA12 */
	uint32_t moder = GPIOA->MODER;
	//moder &= ~(0x02880000);
	//moder |= (0x02880000);
	moder &= ~(0x03F00000);
	moder |=  (0x02800000);
	GPIOA->MODER = moder;
	uint32_t afrh = GPIOA->AFR[1];
	//afrh &= (0xFFF00FFF);
	//afrh |= (0x000AA000);
	afrh &= (0xFFF000FF);
	afrh |= (0x000AAA00);
	GPIOA->AFR[1] = afrh;
	
	/* Core initialisation 16.3.2 */
	USB_FS_DREGS->DCTL = USB_DCTL_SDIS_Msk;
	
	USB_FS_GREGS->GAHBCFG = USB_GAHBCFG_GINTMSK_Msk;

	//USB_FS_GREGS->GUSBCFG = (5 << USB_GUSBCFG_TOUTCAL_Pos) | (5 << USB_GUSBCFG_TRDT_Pos);;
	USB_FS_GREGS->GUSBCFG = (5 << USB_GUSBCFG_TOUTCAL_Pos) | (9 << USB_GUSBCFG_TRDT_Pos);;
	USB_FS_GREGS->GUSBCFG |= USB_GUSBCFG_FDMOD_Msk;
	//USB_FS_GREGS->GUSBCFG |= USB_GUSBCFG_SRPCAP_Msk;

	USB_FS_GREGS->GINTSTS = 0xFFFFFFFF;
	USB_FS_GREGS->GINTMSK = USB_GINTMSK_MMISM_Msk | USB_GINTMSK_OTGINT_Msk;

    /* Enable OTG_FS_IRQ interrupt */
    NVIC_EnableIRQ(OTG_FS_IRQn);
    
    usb_init_device();
	
}

/***********************************************************************
 * 
 * Core interrupt handler
 * 
 * *********************************************************************/


void OTG_FS_IRQHandler() {
//	PORT1->OMR = 0x30000 | 0x03;
	uint32_t gintsts = USB_FS_GREGS->GINTSTS;
	if (gintsts&0xF0000000) {DEBUGLOG(1,gintsts);}
	USB_FS_GREGS->GINTSTS = gintsts;
	if (gintsts & (USB_GINTSTS_SRQI_Msk | USB_GINTSTS_USBRST_Msk | 
				   USB_GINTSTS_ENUMDNE_Msk | USB_GINTSTS_MMIS_Msk | 
				   USB_GINTSTS_CIDSCHG_Msk | USB_GINTSTS_WUI_Msk |
				   USB_GINTSTS_USBSUSP_Msk | USB_GINTMSK_OTGINT_Msk)) {
		DEBUGLOGCORE(0xF0000000,gintsts);	
		if (gintsts & USB_GINTSTS_SRQI_Msk) { /* Session Request */
			
		}
		if (gintsts & USB_GINTSTS_USBSUSP_Msk) { /* Suspend */
			
		}
		if (gintsts & USB_GINTSTS_WUI_Msk) { /* Wakeup */
			
		}
		if (gintsts & USB_GINTSTS_USBRST_Msk) { /* USB Reset */
			usb_reset();
		}
		if (gintsts & USB_GINTSTS_ENUMDNE_Msk) { /* USB Enumeration done */
			usb_enum_done();
		}
		if (gintsts & USB_GINTSTS_MMIS_Msk) { 	/* Mode mismatch - shouldnt happen */
			
		}
		if (gintsts & USB_GINTSTS_OTGINT_Msk) { 	/* OTG Event */
			uint32_t gotgint = USB_FS_GREGS->GOTGINT;
			USB_FS_GREGS->GOTGINT = gotgint;
			DEBUGLOGCORE(0xE0000000,gotgint)
		}
	}
	if (gintsts & USB_GINTSTS_SOF_Msk) { 	/* Handle SOF */
		if (config_ep_out[1].buffer->count != config_ep_out[1].buffer->index)  /* More data to read in the buffer */
			if (config_ep_out[1].callback) config_ep_out[1].callback();
	}
	if (gintsts & USB_GINTSTS_RXFLVL_Msk) { 	/* RxFIFO nonempty */
		usb_rxflvl(); 
	}
	if (gintsts & USB_GINTMSK_OEPINT_Msk) { 	/* Out EP */
		uint32_t daint = USB_FS_DREGS->DAINT;
		if (daint & 0x10000) {
			uint32_t doepint = USB_FS_OUTEP[0].DOEPINT;
			DEBUGLOG(6,doepint)
			USB_FS_OUTEP[0].DOEPINT = doepint;
			if (doepint & USB_DOEPINT_STUP_Msk) {
				/* Handle control transfer setup packet */
				DEBUGLOGCORE(0xE0C0E0C0,0x0)
				DEBUGLOGCORE(ep0_setup[0],ep0_setup[1])
				usb_control((USB_SETUP_PACKET *) ep0_setup);
				USB_FS_OUTEP[0].DOEPTSIZ |= (3<<USB_DOEPTSIZ0_STUPCNT_Pos);
			}
			if (doepint & USB_DOEPINT_OTEPDIS_Msk) {
				/* Host wants to send data */
			}
			if (doepint & USB_DOEPINT_XFRC_Msk) {
				/* Transfer complete - move to application, prepare for more data */
			}
		}
		if (daint & 0x20000) {
			uint32_t doepint = USB_FS_OUTEP[1].DOEPINT;
			DEBUGLOG(0x16,doepint)
			USB_FS_OUTEP[1].DOEPINT = doepint;
			if (doepint & USB_DOEPINT_OTEPDIS_Msk) {
				/* Check for available packet buffer */
				usb_ep_buffer_t * epb = config_ep_out[1].buffer;
				if (epb->count == epb->index) {
					usb_rx_epn(1);
				}
			}
			if (doepint & USB_DOEPINT_XFRC_Msk) {
				/* Transfer complete - move to application, prepare for more data */
				usb_ep_buffer_t * epb = config_ep_out[1].buffer;
				if (epb->count == epb->index) {
					usb_rx_epn(1);
				}
			}
		}
	}
	if (gintsts & USB_GINTMSK_IEPINT_Msk) { 	/* In EP */
		uint32_t daint = USB_FS_DREGS->DAINT;
		if (daint & 1) {
			uint32_t diepint = USB_FS_INEP[0].DIEPINT;
			DEBUGLOG(8,diepint)
			USB_FS_INEP[0].DIEPINT = diepint;
			if ((diepint & USB_DIEPINT_TXFE_Msk) && (USB_FS_DREGS->DIEPEMPMSK & 1)) {
				usb_tx_more_ep0();
			}
			if (diepint & USB_DIEPINT_ITTXFE_Msk) {
				/* Host wants to receive data */
			}
			if (diepint & USB_DIEPINT_XFRC_Msk) {
				/* Transfer complete - prepare application to send more data */
			}
		}
		if (daint & 2) {
			uint32_t diepint = USB_FS_INEP[1].DIEPINT;
			DEBUGLOG(0x18,diepint)
			USB_FS_INEP[1].DIEPINT = diepint;
			if (diepint & (USB_DIEPINT_ITTXFE_Msk | USB_DIEPINT_XFRC_Msk)) {
				/* Write pending TX data to FIFO */
				if (!(config_ep_in[1].buffer->status&USB_BUF_STATUS_LOCKED)) {
					if ((config_ep_in[1].buffer->count) || (config_ep_in[1].buffer->status & USB_BUF_STATUS_MLP)) {
						if (usb_tx_ep_idle(1)) usb_tx_epn(1);
						DEBUGLOG(0x101,USB_FS_INEP[1].DIEPCTL)
					}
				}
			}
		}
		if (daint & 4) {
			uint32_t diepint = USB_FS_INEP[2].DIEPINT;
			USB_FS_INEP[2].DIEPINT = diepint;
		}		
	}
//	PORT1->OMR = 0x30000 | 0x03;
}

