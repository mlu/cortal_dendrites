#include "xmc4500.h"
#include "usb_dev.h"

/* Global RX FIFO Packet status defines */
#define GRXSTSP_PKTSTS_SETUP  0x6

//#define USE_DEBUGLOG
#ifdef USE_DEBUGLOG
__attribute__ ((section(".systemram"))) uint32_t debugdata[10000];
#define DEBUGLOG(id,data) debugdata[2*(++debugdata[0])]=id;debugdata[2*(debugdata[0])+1]=data;
#define DEBUGLOGCORE(id,data) debugdata[2*(++debugdata[0])]=id;debugdata[2*(debugdata[0])+1]=data;
#else
//__attribute__ ((section(".systemram"))) uint32_t debugdata[1000];
#define DEBUGLOG(id,data) {;};
#define DEBUGLOGCORE(id,data) {;};
//#define DEBUGLOGCORE(id,data) debugdata[2*(++debugdata[0])]=id;debugdata[2*(debugdata[0])+1]=data;
#endif

uint32_t ep0_setup[2];

uint32_t ep0_tx_count;
uint32_t * ep0_tx_buf;

int usb_tx_ep_idle(uint32_t epnum) {
	/* Test if endpoint is active and not enabled */
	USB0_EP_TypeDef * USB0_EP = (USB0_EP_TypeDef *)(USB_EP_BASE+epnum*0x20);	
	return (USB0_EP->DIEPCTL_INTBULK & (USB_EP_DIEPCTL_INTBULK_EPEna_Msk|USB_EP_DIEPCTL_INTBULK_USBActEP_Msk))==USB_EP_DIEPCTL_INTBULK_USBActEP_Msk;
}

/* Transmit a single packet */
void usb_tx_epn(uint32_t epnum)
{
	USB0_EP_TypeDef * USB0_EP = (USB0_EP_TypeDef *)(USB_EP_BASE+epnum*0x20);	
	volatile uint32_t * dfifon = (uint32_t *) (0x50041000+epnum*0x1000);
	usb_ep_buffer_t * epbuf = config_ep_in[epnum].buffer;
	uint32_t wcount = (epbuf->count+3)/4;
	DEBUGLOG(0x99,epbuf->count)
	/* Transfer size */
	USB0_EP->DIEPTSIZ = (1<<USB_EP_DIEPTSIZ_PktCnt_Pos) | ((epbuf->count)<<USB_EP_DIEPTSIZ_XferSize_Pos);
	/* Enable EPn IN */
	USB0_EP->DIEPCTL_INTBULK |= USB_EP_DIEPCTL_INTBULK_EPEna_Msk | USB_EP_DIEPCTL_INTBULK_CNAK_Msk;
	/* Prepare to receive more transfer requests */
	USB0->DIEPMSK |= USB_DIEPMSK_INTknTXFEmpMsk_Msk;
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
	uint32_t dtxfsts = USB0_EP0->DTXFSTS0;
	ep0_tx_buf =(uint32_t *)buf;
	ep0_tx_count = (size+3)/4;
	uint32_t pktcount = (ep0_tx_count+15)/16;
	uint32_t txcount = ep0_tx_count;
	/* Transfer size */
	USB0_EP0->DIEPTSIZ0 = (pktcount<<USB_EP_DIEPTSIZ0_PktCnt_Pos) | (size<<USB_EP_DIEPTSIZ0_XferSize_Pos);
	/* Enable EP0 IN */
	USB0_EP0->DIEPCTL0 = USB_EP_DIEPCTL0_EPEna_Msk | USB_EP_DIEPCTL0_CNAK_Msk;
	DEBUGLOG(9,size)
	if (txcount>16) txcount = 16;
	if ( dtxfsts == 16 ) {
		volatile uint32_t * dfifo0 = (uint32_t *) 0x50041000;
		int i;
		/* Write data to FIFO */
		for (i=0; i<txcount; i++) {
			*(dfifo0) = *(ep0_tx_buf++);
		}
		ep0_tx_count -= txcount;	
	}
	if (ep0_tx_count>0) {
		USB0->DIEPEMPMSK = 1;
	}
}

void usb_tx_more_ep0() {
	/* Write packet into EP0 Tx FIFO */
	uint32_t dtxfsts = USB0_EP0->DTXFSTS0;
	uint32_t txcount = ep0_tx_count;
	if (txcount>16) txcount = 16;
	DEBUGLOG(9,ep0_tx_count)
	while (USB0_EP0->DTXFSTS0 < 16) ; /* This might hang the application */
	if ( dtxfsts == 16 ) {
		volatile uint32_t * dfifo0 = (uint32_t *) 0x50041000;
		int i;
		/* Write data to FIFO */
		for (i=0; i<txcount; i++) {
			*(dfifo0) = *(ep0_tx_buf++);
		}
		ep0_tx_count -= txcount;	
	}
	if (ep0_tx_count==0) {
		USB0->DIEPEMPMSK = 0; /* ???????????? Check this */
	}
}

void usb_rx_ep0(uint32_t bcount) {
	/* Read packet from EP0 Rx FIFO */
	if (bcount>0) {
		int i;
		uint32_t count = (bcount+3)/4;
		volatile uint32_t * dfifo0 = (uint32_t *) 0x50041000;
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
		volatile uint32_t * dfifo0 = (uint32_t *) 0x50041000;
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
		volatile uint32_t * dfifo0 = (uint32_t *) 0x50041000;
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
	uint32_t grxstsp = USB0->GRXSTSP_DEVICEMODE;
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
	USB0_EP_TypeDef * USB0_EP = (USB0_EP_TypeDef *)(USB_EP_BASE+epnum*0x20);
	USB0_EP->DOEPTSIZ_CONTROL = (1<<USB_EP_DOEPTSIZ_CONTROL_PktCnt_Pos) | (64<<USB_EP_DOEPTSIZ_CONTROL_XferSize_Pos);
	USB0_EP->DOEPCTL_INTBULK |= USB_EP_DOEPCTL_INTBULK_EPEna_Msk | USB_EP_DOEPCTL_INTBULK_CNAK_Msk;
}

void usb_send_status_IN() {
	USB0_EP0->DIEPTSIZ0 = (1<<USB_EP_DIEPTSIZ0_PktCnt_Pos);
	USB0_EP0->DIEPCTL0 = USB_EP_DIEPCTL0_EPEna_Msk | USB_EP_DIEPCTL0_CNAK_Msk ;
}

void usb_send_status_stall() {
	USB0_EP0->DIEPCTL0 = USB_EP_DIEPCTL0_EPEna_Msk | USB_EP_DIEPCTL0_Stall_Msk ;
}

void usb_read_status_OUT() {
	USB0_EP0->DOEPTSIZ0 = (1<<USB_EP_DOEPTSIZ0_PktCnt_Pos) | (64<<USB_EP_DOEPTSIZ0_XferSize_Pos);
	USB0_EP0->DOEPCTL0 |= USB_EP_DOEPCTL0_EPEna_Msk | USB_EP_DOEPCTL0_CNAK_Msk;
}

/* Blocking read of DATA OUT packets on EP0 */
uint32_t usb_rxdata_ep0(uint8_t * buf,uint32_t size) {
	int i;
	uint8_t * epbuf = config_ep_out[0].buffer->data8;
	USB0_EP0->DOEPTSIZ0 = (1<<USB_EP_DOEPTSIZ0_PktCnt_Pos) | (size<<USB_EP_DOEPTSIZ0_XferSize_Pos);
	USB0_EP0->DOEPCTL0 |= USB_EP_DOEPCTL0_EPEna_Msk | USB_EP_DOEPCTL0_CNAK_Msk;
	while ((USB0->GINTSTS_DEVICEMODE & USB_GINTSTS_DEVICEMODE_RxFLvl_Msk)==0) ; /* wait */
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
	USB0->GRSTCTL = USB_GRSTCTL_RxFFlsh_Msk;
	while (USB0->GRSTCTL & USB_GRSTCTL_RxFFlsh_Msk) ;
	USB0->GRSTCTL = USB_GRSTCTL_TxFFlsh_Msk | (0x10 << USB_GRSTCTL_TxFNum_Pos);
	while (USB0->GRSTCTL & USB_GRSTCTL_TxFFlsh_Msk) ;
	
	for (i=0; i<USB_EPBUF_NUM; i++) {
		ep_buffers[i].index = 0;
		ep_buffers[i].count = 0;
		ep_buffers[i].status = 0;
	}

	USB0->DCFG = ( 3 << USB_DCFG_DevSpd_Pos) | ( 0 << USB_DCFG_NZStsOUTHShk_Pos)  ;
	USB0_EP0->DOEPCTL0 = USB_EP_DOEPCTL0_SNAK_Msk;
	USB0_EP1->DOEPCTL_INTBULK = USB_EP_DOEPCTL_INTBULK_SNAK_Msk;
	USB0_EP2->DOEPCTL_INTBULK = USB_EP_DOEPCTL_INTBULK_SNAK_Msk;
	USB0_EP3->DOEPCTL_INTBULK = USB_EP_DOEPCTL_INTBULK_SNAK_Msk;
	USB0_EP4->DOEPCTL_INTBULK = USB_EP_DOEPCTL_INTBULK_SNAK_Msk;
	USB0_EP5->DOEPCTL_INTBULK = USB_EP_DOEPCTL_INTBULK_SNAK_Msk;
	USB0_EP6->DOEPCTL_INTBULK = USB_EP_DOEPCTL_INTBULK_SNAK_Msk;
	/* Unmask EP0 interrupts */
	USB0->DAINTMSK = (1<<USB_DAINTMSK_OutEpMsk_Pos) | (1<<USB_DAINTMSK_InEpMsk_Pos);
	USB0->DOEPMSK = USB_DOEPMSK_SetUPMsk_Msk | USB_DOEPMSK_XferComplMsk_Msk;
	USB0->DIEPMSK = USB_DIEPMSK_XferComplMsk_Msk | USB_DIEPMSK_TimeOUTMsk_Msk;
	/* Configre reception of setup packets */
	USB0_EP0->DOEPTSIZ0 = (3<<USB_EP_DOEPTSIZ0_SUPCnt_Pos);
	
}

void usb_enum_done() {
	uint32_t dsts = USB0->DSTS;
	DEBUGLOG(3,dsts)
	if ((dsts&0x6)==0x6) {
		USB0_EP0->DIEPCTL0 = 0;
		USB0_EP0->DOEPCTL0 |= USB_EP_DOEPCTL0_EPEna_Msk;
	}
}

void usb_set_address(uint32_t address) {
	USB0->DCFG = (address & 0x7F)<<USB_DCFG_DevAddr_Pos;
};

void usb_enable_ep(uint32_t epnum,uint32_t eptype,uint32_t epdir) {
	if (epnum==0) return;
	if (epnum>6) return;
	USB0_EP_TypeDef * USB0_EP = (USB0_EP_TypeDef *)(USB_EP_BASE+epnum*0x20);
	if (epdir & EP_DIR_OUT) {
		USB0_EP->DOEPCTL_INTBULK = USB_EP_DOEPCTL_INTBULK_USBActEP_Msk | USB_EP_DOEPCTL_INTBULK_SetD0PID_Msk | 64;
		USB0_EP->DOEPCTL_INTBULK |= (eptype<<USB_EP_DOEPCTL_INTBULK_EPType_Pos);
		USB0->DAINTMSK |= (1<<(USB_DAINTMSK_OutEpMsk_Pos+epnum));
		config_ep_out[epnum].buffer->count=0;
		config_ep_out[epnum].buffer->index=0;
		config_ep_out[epnum].buffer->status=0;
		usb_rx_epn(epnum);
	}
	if (epdir & EP_DIR_IN) {
		uint32_t diepctl = USB_EP_DIEPCTL_INTBULK_USBActEP_Msk | USB_EP_DIEPCTL_INTBULK_SetD0PID_Msk | 64;
		diepctl |= (eptype<<USB_EP_DIEPCTL_INTBULK_EPType_Pos);
		diepctl |= epnum<<USB_EP_DOEPCTL_INTBULK_TxFNum_Pos;
		diepctl |= USB_EP_DIEPCTL_INTBULK_CNAK_Msk;
		USB0_EP->DIEPCTL_INTBULK = diepctl;
		USB0->DAINTMSK |= (1<<(USB_DAINTMSK_InEpMsk_Pos+epnum));
		USB0->DIEPMSK |= USB_DIEPMSK_INTknTXFEmpMsk_Msk;
		config_ep_in[epnum].buffer->count=0;
		config_ep_in[epnum].buffer->index=0;
		config_ep_in[epnum].buffer->status=0;
	}
};

void usb_init_device(void)
{
	int i;
	
	/* Device mode initialisation 16.8 */	
	USB0->GINTMSK_DEVICEMODE |= USB_GINTMSK_DEVICEMODE_USBRstMsk_Msk | USB_GINTMSK_DEVICEMODE_EnumDoneMsk_Msk | 
	                            USB_GINTMSK_DEVICEMODE_ErlySuspMsk_Msk | USB_GINTMSK_DEVICEMODE_USBSuspMsk_Msk;
	USB0->GINTMSK_DEVICEMODE |= USB_GINTMSK_DEVICEMODE_WkUpIntMsk_Msk;
	USB0->GINTMSK_DEVICEMODE |= USB_GINTMSK_DEVICEMODE_RxFLvlMsk_Msk;
	USB0->GINTMSK_DEVICEMODE |= USB_GINTMSK_DEVICEMODE_OEPIntMsk_Msk;
	USB0->GINTMSK_DEVICEMODE |= USB_GINTMSK_DEVICEMODE_IEPIntMsk_Msk;
	USB0->GINTMSK_DEVICEMODE |= USB_GINTMSK_DEVICEMODE_SofMsk_Msk;
	
	USB0->DCFG = ( 3 << USB_DCFG_DevSpd_Pos) | ( 0 << USB_DCFG_NZStsOUTHShk_Pos)  ;

	/* Clear the DCTL.SftDiscon */
	USB0->DCTL &= ~USB_DCTL_SftDiscon_Msk;

	/* FIFO RAM Allocation - this really depends on device specific EP configuration */
	USB0->GRXFSIZ = 0x40;  			/* RXFIFO 64 DWORDS = 256 bytes */
	USB0->GNPTXFSIZ_DEVICEMODE = 0x00100040;   /* 16 DWORDS */
	USB0->DIEPTXF1  = 0x00400050;   /* 4*16 DWORDS */
	USB0->DIEPTXF2  = 0x00400090;   /* 4*16 DWORDS */
	USB0->DIEPTXF3  = 0x004000D0;   /* 4*16 DWORDS */
	USB0->DIEPTXF4  = 0x00400110;   /* 4*16 DWORDS */
	USB0->DIEPTXF5  = 0x00400150;   /* 4*16 DWORDS */
	USB0->DIEPTXF6  = 0x00400190;   /* 4*16 DWORDS */

}

void usb_init(void)
{
#ifdef USE_DEBUGLOG
	int i;
	for (i=0; i<sizeof(debugdata)/4;i++) debugdata[i]=0;
#endif
	/* USB Clock and module enable */
	SCU_CLK->CLKSET = SCU_CLK_CLKSET_USBCEN_Msk;
	SCU_RESET->PRCLR2 = SCU_RESET_PRCLR2_USBRS_Msk;
	SCU_POWER->PWRSET =  SCU_POWER_PWRSET_USBOTGEN_Msk |  SCU_POWER_PWRSET_USBPHYPDQ_Msk;
	
	/* Core initialisation 16.3.2 */
	USB0->DCTL = USB_DCTL_SftDiscon_Msk;
	
	USB0->GAHBCFG = USB_GAHBCFG_GlblIntrMsk_Msk; /* DMA Mode, AHB Burst Length, Global Interrupt Mask, RxFIFO Non-Empty, */

	USB0->GUSBCFG = (5 << USB_GUSBCFG_TOutCal_Pos) | (5 << USB_GUSBCFG_USBTrdTim_Pos) ; //| USB_GUSBCFG_ForceDevMode_Msk; /* HNP Capable, SRP Capable, FS Time-Out, USB Turnaround Time */
	//USB0->GUSBCFG |= USB_GUSBCFG_ForceDevMode_Msk ;
	USB0->GUSBCFG |= USB_GUSBCFG_SRPCap_Msk ;

	USB0->GINTSTS_DEVICEMODE = 0xFFFFFFFF;
	USB0->GINTMSK_DEVICEMODE = USB_GINTMSK_DEVICEMODE_ModeMisMsk_Msk | USB_GINTMSK_DEVICEMODE_OTGIntMsk_Msk;

    /* Enable USB0_0_IRQ interrupt */
    NVIC_EnableIRQ(USB0_0_IRQn);
    
    usb_init_device();
	
}

/***********************************************************************
 * 
 * Core interrupt handler
 * 
 * *********************************************************************/

void USB0_0_IRQ() {
	PORT1->OMR = 0x30000 | 0x03;
	uint32_t gintsts = USB0->GINTSTS_DEVICEMODE;
	if (gintsts&0xF0000000) {DEBUGLOG(1,gintsts);}
	USB0->GINTSTS_DEVICEMODE = gintsts;
	if (gintsts & (USB_GINTSTS_DEVICEMODE_SessReqInt_Msk | USB_GINTSTS_DEVICEMODE_USBRst_Msk | 
				   USB_GINTSTS_DEVICEMODE_EnumDone_Msk | USB_GINTMSK_DEVICEMODE_ModeMisMsk_Msk | 
				   USB_GINTMSK_DEVICEMODE_OTGIntMsk_Msk | USB_GINTMSK_DEVICEMODE_WkUpIntMsk_Msk |
				   USB_GINTMSK_DEVICEMODE_USBSuspMsk_Msk )) {
		DEBUGLOGCORE(0xF0000000,gintsts);	
		if (gintsts & USB_GINTSTS_DEVICEMODE_SessReqInt_Msk) { /* Session Request */
			
		}
		if (gintsts & USB_GINTSTS_DEVICEMODE_USBSusp_Msk) { /* Suspend */
			
		}
		if (gintsts & USB_GINTSTS_DEVICEMODE_WkUpInt_Msk) { /* Wakeup */
			
		}
		if (gintsts & USB_GINTSTS_DEVICEMODE_USBRst_Msk) { /* USB Reset */
			usb_reset();
		}
		if (gintsts & USB_GINTSTS_DEVICEMODE_EnumDone_Msk) { /* USB Enumeration done */
			usb_enum_done();
		}
		if (gintsts & USB_GINTMSK_DEVICEMODE_ModeMisMsk_Msk) { 	/* Mode mismatch - shouldnt happen */
			
		}
		if (gintsts & USB_GINTMSK_DEVICEMODE_OTGIntMsk_Msk) { 	/* OTG Event */
			uint32_t gotgint = USB0->GOTGINT;
			USB0->GOTGINT = gotgint;
			DEBUGLOGCORE(0xE0000000,gotgint)
		}
	}
	if (gintsts & USB_GINTSTS_DEVICEMODE_Sof_Msk) { 	/* Handle SOF */
		if (config_ep_out[1].buffer->count != config_ep_out[1].buffer->index)  /* More data to read in the buffer */
			if (config_ep_out[1].callback) config_ep_out[1].callback();
	}
	if (gintsts & USB_GINTMSK_DEVICEMODE_RxFLvlMsk_Msk) { 	/* RxFIFO nonempty */
		usb_rxflvl(); 
	}
	if (gintsts & USB_GINTMSK_DEVICEMODE_OEPIntMsk_Msk) { 	/* Out EP */
		uint32_t daint = USB0->DAINT;
		if (daint & 0x10000) {
			uint32_t doepint = USB0_EP0->DOEPINT0;
			DEBUGLOG(6,doepint)
			USB0_EP0->DOEPINT0 = doepint;
			if (doepint & USB_EP_DOEPINT0_SetUp_Msk) {
				/* Handle control transfer setup packet */
				DEBUGLOGCORE(0xE0C0E0C0,0x0)
				DEBUGLOGCORE(ep0_setup[0],ep0_setup[1])
				usb_control((USB_SETUP_PACKET *) ep0_setup);
				USB0_EP0->DOEPTSIZ0 |= (3<<USB_EP_DOEPTSIZ0_SUPCnt_Pos);
			}
			if (doepint & USB_EP_DOEPINT0_OUTTknEPdis_Msk) {
				/* Host wants to send data */
			}
			if (doepint & USB_EP_DOEPINT0_XferCompl_Msk) {
				/* Transfer complete - move to application, prepare for more data */
			}
		}
		if (daint & 0x20000) {
			uint32_t doepint = USB0_EP1->DOEPINT;
			DEBUGLOG(0x16,doepint)
			USB0_EP1->DOEPINT = doepint;
			if (doepint & USB_EP_DOEPINT0_OUTTknEPdis_Msk) {
				/* Check for available packet buffer */
				usb_ep_buffer_t * epb = config_ep_out[1].buffer;
				if (epb->count == epb->index) {
					usb_rx_epn(1);
				}
			}
			if (doepint & USB_EP_DOEPINT0_XferCompl_Msk) {
				/* Transfer complete - move to application, prepare for more data */
				usb_ep_buffer_t * epb = config_ep_out[1].buffer;
				if (epb->count == epb->index) {
					usb_rx_epn(1);
				}
			}
		}
	}
	if (gintsts & USB_GINTMSK_DEVICEMODE_IEPIntMsk_Msk) { 	/* In EP */
		uint32_t daint = USB0->DAINT;
		if (daint & 1) {
			uint32_t diepint = USB0_EP0->DIEPINT0;
			DEBUGLOG(8,diepint)
			USB0_EP0->DIEPINT0 = diepint;
			if ((diepint & USB_EP_DIEPINT0_TxFEmp_Msk) && (USB0->DIEPEMPMSK & 1)) {
				usb_tx_more_ep0();
			}
			if (diepint & USB_EP_DIEPINT0_INTknTXFEmp_Msk) {
				/* Host wants to receive data */
			}
			if (diepint & USB_EP_DIEPINT0_XferCompl_Msk) {
				/* Transfer complete - prepare application to send more data */
			}
		}
		if (daint & 2) {
			uint32_t diepint = USB0_EP1->DIEPINT;
			DEBUGLOG(0x18,diepint)
			USB0_EP1->DIEPINT = diepint;
			if (diepint & (USB_EP_DIEPINT_INTknTXFEmp_Msk | USB_EP_DIEPINT_XferCompl_Msk)) {
				/* Write pending TX data to FIFO */
				if (!(config_ep_in[1].buffer->status&USB_BUF_STATUS_LOCKED)) {
					if ((config_ep_in[1].buffer->count) || (config_ep_in[1].buffer->status & USB_BUF_STATUS_MLP)) {
						if (usb_tx_ep_idle(1)) usb_tx_epn(1);
						DEBUGLOG(0x101,USB0_EP1->DIEPCTL_INTBULK)
					}
				}
			}
		}
		if (daint & 4) {
			uint32_t diepint = USB0_EP2->DIEPINT;
			USB0_EP2->DIEPINT = diepint;
		}		
	}
	PORT1->OMR = 0x30000 | 0x03;
}
