/**
  ******************************************************************************
  * @file    usb_regs.h
  * @author  MCD Application Team
  * @version V2.1.0
  * @date    19-March-2012
  * @brief   hardware registers
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_REGS_H__
#define __USB_REGS_H__

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

#define USB_HS_BASE_ADDR                 0x40040000
#define USB_FS_BASE_ADDR                 0x50000000

#define USB_CORE_GLOBAL_REGS_OFFSET      0x000
#define USB_DEV_GLOBAL_REG_OFFSET        0x800
#define USB_DEV_IN_EP_REG_OFFSET         0x900
#define USB_EP_REG_OFFSET                0x20
#define USB_DEV_OUT_EP_REG_OFFSET        0xB00
#define USB_HOST_GLOBAL_REG_OFFSET       0x400
#define USB_HOST_PORT_REGS_OFFSET        0x440
#define USB_HOST_CHAN_REGS_OFFSET        0x500
#define USB_CHAN_REGS_OFFSET             0x20
#define USB_PCGCCTL_OFFSET               0xE00
#define USB_DATA_FIFO_OFFSET             0x1000
#define USB_DATA_FIFO_SIZE               0x1000

#define USB_MAX_TX_FIFOS                 15

#define USB_HS_MAX_PACKET_SIZE           512
#define USB_FS_MAX_PACKET_SIZE           64
#define USB_MAX_EP0_SIZE                 64
/**
  * @}
  */ 

/** @defgroup USB_REGS_Exported_Types
  * @{
  */ 

/** @defgroup __USB_Core_register
  * @{
  */
typedef struct _USB_GREGS  //000h
{
  __IO uint32_t GOTGCTL;      /* USB_OTG Control and Status Register    000h*/
  __IO uint32_t GOTGINT;      /* USB_OTG Interrupt Register             004h*/
  __IO uint32_t GAHBCFG;      /* Core AHB Configuration Register    008h*/
  __IO uint32_t GUSBCFG;      /* Core USB Configuration Register    00Ch*/
  __IO uint32_t GRSTCTL;      /* Core Reset Register                010h*/
  __IO uint32_t GINTSTS;      /* Core Interrupt Register            014h*/
  __IO uint32_t GINTMSK;      /* Core Interrupt Mask Register       018h*/
  __IO uint32_t GRXSTSR;      /* Receive Sts Q Read Register        01Ch*/
  __IO uint32_t GRXSTSP;      /* Receive Sts Q Read & POP Register  020h*/
  __IO uint32_t GRXFSIZ;      /* Receive FIFO Size Register         024h*/
  __IO uint32_t DIEPTXF0_HNPTXFSIZ;   /* EP0 / Non Periodic Tx FIFO Size Register 028h*/
  __IO uint32_t HNPTXSTS;     /* Non Periodic Tx FIFO/Queue Sts reg 02Ch*/
  uint32_t Reserved30[2];     /* Reserved                           030h*/
  __IO uint32_t GCCFG;        /* General Purpose IO Register        038h*/
  __IO uint32_t CID;          /* User ID Register                   03Ch*/
  uint32_t  Reserved40[48];   /* Reserved                      040h-0FFh*/
  __IO uint32_t HPTXFSIZ; /* Host Periodic Tx FIFO Size Reg     100h*/
  __IO uint32_t DIEPTXF[USB_MAX_TX_FIFOS];/* dev Periodic Transmit FIFO */
}
USB_GREGS_Type;
/**
  * @}
  */


/** @defgroup __device_Registers
  * @{
  */
typedef struct _USB_DREGS // 800h
{
  __IO uint32_t DCFG;         /* dev Configuration Register   800h*/
  __IO uint32_t DCTL;         /* dev Control Register         804h*/
  __IO uint32_t DSTS;         /* dev Status Register (RO)     808h*/
  uint32_t Reserved0C;           /* Reserved                     80Ch*/
  __IO uint32_t DIEPMSK;   /* dev IN Endpoint Mask         810h*/
  __IO uint32_t DOEPMSK;  /* dev OUT Endpoint Mask        814h*/
  __IO uint32_t DAINT;     /* dev All Endpoints Itr Reg    818h*/
  __IO uint32_t DAINTMSK; /* dev All Endpoints Itr Mask   81Ch*/
  uint32_t  Reserved20;          /* Reserved                     820h*/
  uint32_t Reserved9;       /* Reserved                     824h*/
  __IO uint32_t DVBUSDIS;    /* dev VBUS discharge Register  828h*/
  __IO uint32_t DVBUSPULSE;  /* dev VBUS Pulse Register      82Ch*/
  __IO uint32_t DTHRCTL;     /* dev thr                      830h*/
  __IO uint32_t DIEPEMPMSK; /* dev empty msk             834h*/
  __IO uint32_t DEACHINT;    /* dedicated EP interrupt       838h*/
  __IO uint32_t DEACHMSK;    /* dedicated EP msk             83Ch*/  
  uint32_t Reserved40;      /* dedicated EP mask           840h*/
  __IO uint32_t DINEP1MSK;  /* dedicated EP mask           844h*/
  uint32_t  Reserved44[15];      /* Reserved                 844-87Ch*/
  __IO uint32_t DOUTEP1MSK; /* dedicated EP msk            884h*/   
}
USB_DREGS_Type;
/**
  * @}
  */


/** @defgroup __IN_Endpoint-Specific_Register
  * @{
  */
typedef struct _USB_INEPREGS
{
  __IO uint32_t DIEPCTL;			/* dev IN Endpoint Control Reg 900h + (ep_num * 20h) + 00h*/
  uint32_t Reserved04;				/* Reserved                       900h + (ep_num * 20h) + 04h*/
  __IO uint32_t DIEPINT;			/* dev IN Endpoint Itr Reg     900h + (ep_num * 20h) + 08h*/
  uint32_t Reserved0C;				/* Reserved                       900h + (ep_num * 20h) + 0Ch*/
  __IO uint32_t DIEPTSIZ;			/* IN Endpoint Txfer Size   900h + (ep_num * 20h) + 10h*/
  __IO uint32_t DIEPDMA;			/* IN Endpoint DMA Address Reg    900h + (ep_num * 20h) + 14h*/
  __IO uint32_t DTXFSTS;			/*IN Endpoint Tx FIFO Status Reg 900h + (ep_num * 20h) + 18h*/
  uint32_t Reserved18;				/* Reserved  900h+(ep_num*20h)+1Ch-900h+ (ep_num * 20h) + 1Ch*/
}
USB_INEPREGS_Type;
/**
  * @}
  */


/** @defgroup __OUT_Endpoint-Specific_Registers
  * @{
  */
typedef struct _USB_OUTEPREGS
{
  __IO uint32_t DOEPCTL;       /* dev OUT Endpoint Control Reg  B00h + (ep_num * 20h) + 00h*/
  uint32_t Reserved04;         /* Reserved                      B00h + (ep_num * 20h) + 04h*/
  __IO uint32_t DOEPINT;       /* dev OUT Endpoint Itr Reg      B00h + (ep_num * 20h) + 08h*/
  uint32_t Reserved0C;         /* Reserved                      B00h + (ep_num * 20h) + 0Ch*/
  __IO uint32_t DOEPTSIZ;      /* dev OUT Endpoint Txfer Size   B00h + (ep_num * 20h) + 10h*/
  __IO uint32_t DOEPDMA;       /* dev OUT Endpoint DMA Address  B00h + (ep_num * 20h) + 14h*/
  uint32_t Reserved18[2];      /* Reserved B00h + (ep_num * 20h) + 18h - B00h + (ep_num * 20h) + 1Ch*/
}
USB_OUTEPREGS_Type;
/**
  * @}
  */


/** @defgroup __Host_Mode_Register_Structures
  * @{
  */
typedef struct _USB_HREGS
{
  __IO uint32_t HCFG;             /* Host Configuration Register    400h*/
  __IO uint32_t HFIR;      /* Host Frame Interval Register   404h*/
  __IO uint32_t HFNUM;         /* Host Frame Nbr/Frame Remaining 408h*/
  uint32_t Reserved40C;                   /* Reserved                       40Ch*/
  __IO uint32_t HPTXSTS;   /* Host Periodic Tx FIFO/ Queue Status 410h*/
  __IO uint32_t HAINT;   /* Host All Channels Interrupt Register 414h*/
  __IO uint32_t HAINTMSK;   /* Host All Channels Interrupt Mask 418h*/
}
USB_HREGS;
/**
  * @}
  */


/** @defgroup __Host_Channel_Specific_Registers
  * @{
  */
typedef struct _USB_HC_REGS
{
  __IO uint32_t HCCHAR;
  __IO uint32_t HCSPLT;
  __IO uint32_t HCINT;
  __IO uint32_t HCINTMSK;
  __IO uint32_t HCTSIZ;
  __IO uint32_t HCDMA;
  uint32_t Reserved[2];
}
USB_HC_REGS;
/**
  * @}
  */


/** @defgroup __otg_Core_registers
  * @{
  */
typedef struct USB_core_regs //000h
{
  USB_GREGS_Type         *GREGS;
  USB_DREGS_Type         *DREGS;
  USB_HREGS         *HREGS;
  USB_INEPREGS_Type      *INEP_REGS[USB_MAX_TX_FIFOS];
  USB_OUTEPREGS_Type     *OUTEP_REGS[USB_MAX_TX_FIFOS];
  USB_HC_REGS       *HC_REGS[USB_MAX_TX_FIFOS];
  __IO uint32_t         *HPRT0;
  __IO uint32_t         *DFIFO[USB_MAX_TX_FIFOS];
  __IO uint32_t         *PCGCCTL;
}
USB_CORE_REGS , *PUSB_CORE_REGS;

#define USB_FS_GREGS            ((USB_GREGS_Type *)(USB_FS_BASE_ADDR + USB_CORE_GLOBAL_REGS_OFFSET))
#define USB_FS_DREGS            ((USB_DREGS_Type *)(USB_FS_BASE_ADDR + USB_DEV_GLOBAL_REG_OFFSET))
#define USB_FS_INEP             ((USB_INEPREGS_Type *)(USB_FS_BASE_ADDR + USB_DEV_IN_EP_REG_OFFSET))
#define USB_FS_OUTEP            ((USB_OUTEPREGS_Type *)(USB_FS_BASE_ADDR + USB_DEV_OUT_EP_REG_OFFSET))

typedef union _USB_GOTGCTL_TypeDef 
{
  uint32_t d32;
  struct
  {
uint32_t sesreqscs :
    1;
uint32_t sesreq :
    1;
uint32_t Reserved2_7 :
    6;
uint32_t hstnegscs :
    1;
uint32_t hnpreq :
    1;
uint32_t hstsethnpen :
    1;
uint32_t devhnpen :
    1;
uint32_t Reserved12_15 :
    4;
uint32_t conidsts :
    1;
uint32_t dbct :
    1;
uint32_t asesvld :
    1;
uint32_t bsesvld :
    1;
uint32_t Reserved20_31 :
    12;
  }
  b;
} USB_GOTGCTL_TypeDef ;

typedef union _USB_GOTGINT_TypeDef 
{
  uint32_t d32;
  struct
  {
uint32_t Reserved0_1 :
    2;
uint32_t sesenddet :
    1;
uint32_t Reserved3_7 :
    5;
uint32_t sesreqsucstschng :
    1;
uint32_t hstnegsucstschng :
    1;
uint32_t reserver10_16 :
    7;
uint32_t hstnegdet :
    1;
uint32_t adevtoutchng :
    1;
uint32_t debdone :
    1;
uint32_t Reserved31_20 :
    12;
  }
  b;
} USB_GOTGINT_TypeDef ;

/* --------------------------------  USB_GAHBCFG  -------------------------------- */

#define USB_GAHBCFG_GINTMSK_Pos       0				/* Global interrupt mask  				*/
#define USB_GAHBCFG_GINTMSK_Msk       (1<<0)
#define USB_GAHBCFG_TXFELVL_Pos       7				/* TxFIFO empty level  					*/
#define USB_GAHBCFG_TXFELVL_Msk       (1<<7)
#define USB_GAHBCFG_PTXFELVL_Pos      8				/* Periodic TxFIFO empty level  		*/
#define USB_GAHBCFG_PTXFELVL_Msk      (1<<8)

/* --------------------------------  USB_GUSBCFG  -------------------------------- */

#define USB_GUSBCFG_TOUTCAL_Pos       0
#define USB_GUSBCFG_TOUTCAL_Msk       (0x7<<0)
#define USB_GUSBCFG_PHYSEL_Pos        6
#define USB_GUSBCFG_PHYSEL_Msk        (1<<6)
#define USB_GUSBCFG_SRPCAP_Pos        8
#define USB_GUSBCFG_SRPCAP_Msk        (1<<8)
#define USB_GUSBCFG_HNPCAP_Pos        9
#define USB_GUSBCFG_HNPCAP_Msk        (1<<9)
#define USB_GUSBCFG_TRDT_Pos          10
#define USB_GUSBCFG_TRDT_Msk          (0xF<<10)
#define USB_GUSBCFG_FHMOD_Pos         29
#define USB_GUSBCFG_FHMOD_Msk         (1<<29)
#define USB_GUSBCFG_FDMOD_Pos         30
#define USB_GUSBCFG_FDMOD_Msk         (1<<30)
#define USB_GUSBCFG_CTXPKT_Pos        31
#define USB_GUSBCFG_CTXPKT_Msk        (1<<31)

/* --------------------------------  USB_GRSTCTL  -------------------------------- */

#define USB_GRSTCTL_CSRST_Pos         0					/* Core soft reset  */
#define USB_GRSTCTL_CSRST_Msk         (1<<USB_GRSTCTL_CSRST_Pos)
#define USB_GRSTCTL_HSRST_Pos         1					/* HCLK soft reset   */
#define USB_GRSTCTL_HSRST_Msk         (1<<USB_GRSTCTL_HSRST_Pos)
#define USB_GRSTCTL_FCRST_Pos         2					/* Host frame counter reset   */
#define USB_GRSTCTL_FCRST_Msk         (1<<USB_GRSTCTL_FCRST_Pos)
#define USB_GRSTCTL_RXFFLSH_Pos       4					/* RxFIFO flush   */
#define USB_GRSTCTL_RXFFLSH_Msk       (1<<USB_GRSTCTL_RXFFLSH_Pos)
#define USB_GRSTCTL_TXFFLSH_Pos       5 				/* TxFIFO flush   */
#define USB_GRSTCTL_TXFFLSH_Msk       (1<<USB_GRSTCTL_TXFFLSH_Pos)
#define USB_GRSTCTL_TXFNUM_Pos        6	 				/* TxFIFO number   */
#define USB_GRSTCTL_TXFNUM_Msk        (0x1F<<USB_GRSTCTL_TXFNUM_Pos)
#define USB_GRSTCTL_AHBIDL_Pos        31				/* AHB master idle   */
#define USB_GRSTCTL_AHBIDL_Msk        (1<<USB_GRSTCTL_AHBIDL_Pos)

/* --------------------------------  USB_GINTMSK  -------------------------------- */

#define USB_GINTMSK_MMISM_Pos         1
#define USB_GINTMSK_MMISM_Msk         (1<<1)
#define USB_GINTMSK_OTGINT_Pos        2
#define USB_GINTMSK_OTGINT_Msk        (1<<2)
#define USB_GINTMSK_SOFM_Pos          3
#define USB_GINTMSK_SOFM_Msk          (1<<3)
#define USB_GINTMSK_RFXLVLM_Pos       4
#define USB_GINTMSK_RXFLVLM_Msk       (1<<4)
#define USB_GINTMSK_NPTXFEM_Pos       5
#define USB_GINTMSK_NPTXFEM_Msk       (1<<5)
#define USB_GINTMSK_GINAKEFFM_Pos     6
#define USB_GINTMSK_GINAKEFFM_Msk     (1<<6)
#define USB_GINTMSK_GONAKEFFM_Pos     7
#define USB_GINTMSK_GONAKEFFM_Msk     (1<<7)
#define USB_GINTMSK_ESUSPM_Pos        10
#define USB_GINTMSK_ESUSPM_Msk        (1<<10)
#define USB_GINTMSK_USBSUSPM_Pos      11
#define USB_GINTMSK_USBSUSPM_Msk      (1<<11)
#define USB_GINTMSK_USBRST_Pos        12
#define USB_GINTMSK_USBRST_Msk        (1<<12)
#define USB_GINTMSK_ENUMDNEM_Pos      13
#define USB_GINTMSK_ENUMDNEM_Msk      (1<<13)
#define USB_GINTMSK_ISOODRPM_Pos      14
#define USB_GINTMSK_ISOODRPM_Msk      (1<<14)
#define USB_GINTMSK_EOPFM_Pos         15
#define USB_GINTMSK_EOPFM_Msk         (1<<15)
#define USB_GINTMSK_EPMISM_Pos        17
#define USB_GINTMSK_EPMISM_Msk        (1<<17)
#define USB_GINTMSK_IEPINT_Pos        18
#define USB_GINTMSK_IEPINT_Msk        (1<<18)
#define USB_GINTMSK_OEPINT_Pos        19
#define USB_GINTMSK_OEPINT_Msk        (1<<19)
#define USB_GINTMSK_PTXFEM_Pos        26				/* Periodic TxFIFO empty mask        */
#define USB_GINTMSK_PTXFEM_Msk        (1<<26)
#define USB_GINTMSK_CIDSCHGM_Pos      28				/* Connector ID status change mask        */
#define USB_GINTMSK_CIDSCHGM_Msk      (1<<28)
#define USB_GINTMSK_DISCINT_Pos       29				/* DISCINTDisconnect detected interrupt mask        */
#define USB_GINTMSK_DISCINT_Msk       (1<<29)
#define USB_GINTMSK_SRQIM_Pos         30				/* Session request/new session detected interrupt mask        */
#define USB_GINTMSK_SRQIM_Msk         (1<<30)
#define USB_GINTMSK_WUIM_Pos          31				/* Resume/remote wakeup detected interrupt mask        */
#define USB_GINTMSK_WUIM_Msk          (1<<31)

/* --------------------------------  USB_GINTSTS  -------------------------------- */

#define USB_GINTSTS_MMIS_Pos          1
#define USB_GINTSTS_MMIS_Msk          (1<<1)
#define USB_GINTSTS_OTGINT_Pos        2
#define USB_GINTSTS_OTGINT_Msk        (1<<2)
#define USB_GINTSTS_SOF_Pos           3
#define USB_GINTSTS_SOF_Msk           (1<<3)
#define USB_GINTSTS_RFXLVL_Pos        4
#define USB_GINTSTS_RXFLVL_Msk        (1<<4)
#define USB_GINTSTS_NPTXFE_Pos        5
#define USB_GINTSTS_NPTXFE_Msk        (1<<5)
#define USB_GINTSTS_GINAKEFF_Pos      6
#define USB_GINTSTS_GINAKEFF_Msk      (1<<6)
#define USB_GINTSTS_GONAKEFF_Pos      7
#define USB_GINTSTS_GONAKEFF_Msk      (1<<7)
#define USB_GINTSTS_ESUSP_Pos         10
#define USB_GINTSTS_ESUSP_Msk         (1<<10)
#define USB_GINTSTS_USBSUSP_Pos       11
#define USB_GINTSTS_USBSUSP_Msk       (1<<11)
#define USB_GINTSTS_USBRST_Pos        12
#define USB_GINTSTS_USBRST_Msk        (1<<12)
#define USB_GINTSTS_ENUMDNE_Pos       13
#define USB_GINTSTS_ENUMDNE_Msk       (1<<13)
#define USB_GINTSTS_ISOODRP_Pos       14
#define USB_GINTSTS_ISOODRP_Msk       (1<<14)
#define USB_GINTSTS_EOPF_Pos          15
#define USB_GINTSTS_EOPF_Msk          (1<<15)
#define USB_GINTSTS_EPMIS_Pos         17
#define USB_GINTSTS_EPMIS_Msk         (1<<17)
#define USB_GINTSTS_IEPINT_Pos        18
#define USB_GINTSTS_IEPINT_Msk        (1<<18)
#define USB_GINTSTS_OEPINT_Pos        19
#define USB_GINTSTS_OEPINT_Msk        (1<<19)
#define USB_GINTSTS_PTXFE_Pos         26				/* Periodic TxFIFO empty mask        */
#define USB_GINTSTS_PTXFE_Msk         (1<<26)
#define USB_GINTSTS_CIDSCHG_Pos       28				/* Connector ID status change mask        */
#define USB_GINTSTS_CIDSCHG_Msk       (1<<28)
#define USB_GINTSTS_DISCINT_Pos       29				/* DISCINTDisconnect detected interrupt mask        */
#define USB_GINTSTS_DISCINT_Msk       (1<<29)
#define USB_GINTSTS_SRQI_Pos          30				/* Session request/new session detected interrupt mask        */
#define USB_GINTSTS_SRQI_Msk          (1<<30)
#define USB_GINTSTS_WUI_Pos           31				/* Resume/remote wakeup detected interrupt mask        */
#define USB_GINTSTS_WUI_Msk           (1<<31)

typedef union _USB_DRXSTS_TypeDef 
{
  uint32_t d32;
  struct
  {
uint32_t epnum :
    4;
uint32_t bcnt :
    11;
uint32_t dpid :
    2;
uint32_t pktsts :
    4;
uint32_t fn :
    4;
uint32_t Reserved :
    7;
  }
  b;
} USB_DRXSTS_TypeDef ;
typedef union _USB_GRXSTS_TypeDef 
{
  uint32_t d32;
  struct
  {
uint32_t chnum :
    4;
uint32_t bcnt :
    11;
uint32_t dpid :
    2;
uint32_t pktsts :
    4;
uint32_t Reserved :
    11;
  }
  b;
} USB_GRXFSTS_TypeDef ;

/* ---------------------------------  USB_GRXFSIZ  -------------------------------- */

#define USB_GRXFSIZ_RXFD_Pos          0				/* RxFIFO depth in DWORDS				*/
#define USB_GRXFSIZ_RXFD_Msk          (0xFFFF<<0)

/* --------------------------------  USB_DIEPTXF  -------------------------------- */

#define USB_DIEPTXF_INEPTXSA_Pos      0				/* IN endpoint FIFOx transmit RAM start address		*/
#define USB_DIEPTXF_INEPTXSA_Msk      (0xFFFF<<0)
#define USB_DIEPTXF_INEPTXFD_Pos      16			/* IN endpoint TxFIFO depth	in DWORDS			*/
#define USB_DIEPTXF_INEPTXFD_Msk      (0xFFFF<<16)

typedef union _USB_FSIZ_TypeDef 
{
  uint32_t d32;
  struct
  {
uint32_t startaddr :
    16;
uint32_t depth :
    16;
  }
  b;
} USB_FSIZ_TypeDef ;
typedef union _USB_HNPTXSTS_TypeDef 
{
  uint32_t d32;
  struct
  {
    uint32_t nptxfspcavail :
      16;
    uint32_t nptxqspcavail :
      8;
      struct
        {
          uint32_t terminate :
            1;
          uint32_t token :
            2;
          uint32_t chnum :
            4; 
         } nptxqtop;
     uint32_t Reserved :
        1;
  }
  b;
} USB_HNPTXSTS_TypeDef ;

/* --------------------------------  USB_DTXFSTS  -------------------------------- */

#define USB_DTXFSTS_INEPTFSAV_Pos      0				/* IN endpoint TxFIFO space available		*/
#define USB_DTXFSTS_INEPTFSAV_Msk      (0xFFFF<<0)

/* --------------------------------  USB_GCCFG  -------------------------------- */

/********************  Bit definition for USB_OTG_GCCFG register  ********************/
#define USB_OTG_GCCFG_PWRDWN                 ((uint32_t)0x00010000)              /*!< Power down */
#define USB_OTG_GCCFG_VBDEN                  ((uint32_t)0x00200000)              /*!< USB VBUS Detection Enable */


/* --------------------------------  USB_DCFG  -------------------------------- */

#define USB_DCFG_DSPD_Pos          0				/* Device speed  				*/
#define USB_DCFG_DSPD_Msk          (0x3<<0)
#define USB_DCFG_NZLSOHSK_Pos      2				/* Non-zero-length status OUT handshake 				*/
#define USB_DCFG_NZLSOHSK_Msk      (1<<2)
#define USB_DCFG_DAD_Pos           4				/* Device address 				*/
#define USB_DCFG_DAD_Msk           (0x7F<<4)
#define USB_DCFG_PFIVL_Pos         11				/* Periodic frame interval 		*/
#define USB_DCFG_PFIVL_Msk         (0x3<<11)

/* --------------------------------  USB_DCTL  -------------------------------- */

#define USB_DCTL_RWUSIG_Pos       0				/* Remote wakeup signaling  				*/
#define USB_DCTL_RWUSIG_Msk       (1<<0)
#define USB_DCTL_SDIS_Pos         1				/* Soft disconnect  				*/
#define USB_DCTL_SDIS_Msk         (1<<1)
#define USB_DCTL_GINSTS_Pos       2				/* Global IN NAK status  				*/
#define USB_DCTL_GINSTS_Msk       (1<<2)
#define USB_DCTL_GONSTS_Pos       3				/* Global OUT NAK status  				*/
#define USB_DCTL_GONSTS_Msk       (1<<3)
#define USB_DCTL_TCTL_Pos         4				/* Test control  				*/
#define USB_DCTL_TCTL_Msk         (0x7<<4)
#define USB_DCTL_SGINAK_Pos       7				/* Set global IN NAK  				*/
#define USB_DCTL_SGINAK_Msk       (1<<7)
#define USB_DCTL_CGINAK_Pos       8				/* Clear global IN NAK  				*/
#define USB_DCTL_CGINAK_Msk       (1<<8)
#define USB_DCTL_SGONAK_Pos       9				/* Set global OUT NAK  				*/
#define USB_DCTL_SGONAK_Msk       (1<<9)
#define USB_DCTL_CGONAK_Pos       10			/* Clear global OUT NAK  				*/
#define USB_DCTL_CGONAK_Msk       (1<<10)
#define USB_DCTL_POPRGDNE_Pos     11			/* POPRGDNEPower-on programming done  				*/
#define USB_DCTL_POPRGDNE_Msk     (1<<11)

typedef union _USB_DSTS_TypeDef 
{
  uint32_t d32;
  struct
  {
uint32_t suspsts :
    1;
uint32_t enumspd :
    2;
uint32_t errticerr :
    1;
uint32_t Reserved4_7:
    4;
uint32_t soffn :
    14;
uint32_t Reserved22_31 :
    10;
  }
  b;
} USB_DSTS_TypeDef ;


#define USB_DOEPINT_XFRC_Pos                 0            /* Transfer completed interrupt        */
#define USB_DOEPINT_XFRC_Msk                 (1 << 0)
#define USB_DOEPINT_EPDISD_Pos               1            /* Endpoint disabled interrupt       */
#define USB_DOEPINT_EPDISD_Msk               (1 << 1)
#define USB_DOEPINT_STUP_Pos                 3            /* SETUP phase done        */
#define USB_DOEPINT_STUP_Msk                 (1 << 3)
#define USB_DOEPINT_OTEPDIS_Pos              4            /* OUT token received when endpoint disabled        */
#define USB_DOEPINT_OTEPDIS_Msk              (1 << 4)
#define USB_DOEPINT_B2BSTUP_Pos              6            /* Back-to-back SETUP packets received       */
#define USB_DOEPINT_B2BSTUP_Msk              (1 << 6)

#define USB_DIEPINT_XFRC_Pos                 0            /* Transfer completed interrupt        */
#define USB_DIEPINT_XFRC_Msk                 (1 << 0)
#define USB_DIEPINT_EPDISD_Pos               1            /* Endpoint disabled interrupt       */
#define USB_DIEPINT_EPDISD_Msk               (1 << 1)
#define USB_DIEPINT_TOC_Pos                  3            /* Timeout condition    */
#define USB_DIEPINT_TOC_Msk                  (1 << 3)
#define USB_DIEPINT_ITTXFE_Pos               4            /* IN token received when TxFIFO is empty       */
#define USB_DIEPINT_ITTXFE_Msk               (1 << 4)
#define USB_DIEPINT_INEPNE_Pos               6            /* IN endpoint NAK effective      */
#define USB_DIEPINT_INEPNE_Msk               (1 << 6)
#define USB_DIEPINT_TXFE_Pos                 7            /* Transmit FIFO empty      */
#define USB_DIEPINT_TXFE_Msk                 (1 << 7)

/* ----------------------------------  USB_DAINT  --------------------------------- */

#define USB_DAINT_IEPINT_Pos                 0                                                       /* InEpInt Position             */
#define USB_DAINT_IEPINT_Msk                 (0x0000ffffUL << 0)                 /* InEpInt Mask                 */
#define USB_DAINT_OEPINT_Pos                 16                                                      /* OutEPInt Position            */
#define USB_DAINT_OEPINT_Msk                 (0x0000ffffUL << 16)                /* OutEPInt Mask                */

/* --------------------------------  USB_DAINTMSK  -------------------------------- */

#define USB_DAINTMSK_IEPM_Pos              0                                                       /* InEpMsk Position          */
#define USB_DAINTMSK_IEPM_Msk              (0x0000ffffUL << 0s)              /* InEpMsk Mask              */
#define USB_DAINTMSK_OEPM_Pos             16                                                      /* OutEpMsk Position         */
#define USB_DAINTMSK_OEPM_Msk             (0x0000ffffUL << 16)             /* OutEpMsk Mask             */

typedef union _USB_DTHRCTL_TypeDef 
{
  uint32_t d32;
  struct
  {
uint32_t non_iso_thr_en :
    1;
uint32_t iso_thr_en :
    1;
uint32_t tx_thr_len :
    9;
uint32_t Reserved11_15 :
    5;
uint32_t rx_thr_en :
    1;
uint32_t rx_thr_len :
    9;
uint32_t Reserved26 : 
    1;
uint32_t arp_en :  
    1;
uint32_t Reserved28_31 :
    4;   
  }
  b;
} USB_DTHRCTL_TypeDef ;

#define USB_DEPCTL_MPSIZ_Pos       	0				/* MPSIZMaximum packet size 		*/
#define USB_DEPCTL_MPSIZ_Msk       	(0x3FF<<0)
#define USB_DEPCTL_USBAEP_Pos       15				/* USB active endpoint 				*/
#define USB_DEPCTL_USBAEP_Msk       (1<<15)
#define USB_DEPCTL_DPID_Pos         16				/* Endpoint data PID 				*/
#define USB_DEPCTL_DPID_Msk         (1<<16)
#define USB_DEPCTL_EPTYP_Pos       	18				/* Endpoint type				*/
#define USB_DEPCTL_EPTYP_Msk       	(0x3<<18)
#define USB_DEPCTL_STALL_Pos       	21				/* STALL handshake 				*/
#define USB_DEPCTL_STALL_Msk       	(1<<21)
#define USB_DEPCTL_TXFNUM_Pos      	22				/* TxFIFO number				*/
#define USB_DEPCTL_TXFNUM_Msk      	(0xF<<22)
#define USB_DEPCTL_CNAK_Pos       	26				/* Clear NAK 				*/
#define USB_DEPCTL_CNAK_Msk       	(1<<26)
#define USB_DEPCTL_SNAK_Pos       	27				/* Set NAK 				*/
#define USB_DEPCTL_SNAK_Msk       	(1<<27)
#define USB_DEPCTL_SD0PID_Pos      	28				/* Set DATA0 PID		*/
#define USB_DEPCTL_SD0PID_Msk      	(1<<28)
#define USB_DEPCTL_EPENA_Pos       	31				/* Endpoint enable			*/
#define USB_DEPCTL_EPENA_Msk       	(1<<31)

typedef union _USB_DEPXFRSIZ_TypeDef 
{
  uint32_t d32;
  struct
  {
uint32_t xfersize :
    19;
uint32_t pktcnt :
    10;
uint32_t mc :
    2;
uint32_t Reserved :
    1;
  }
  b;
} USB_DEPXFRSIZ_TypeDef ;
typedef union _USB_DEP0XFRSIZ_TypeDef 
{
  uint32_t d32;
  struct
  {
uint32_t xfersize :
    7;
uint32_t Reserved7_18 :
    12;
uint32_t pktcnt :
    2;
uint32_t Reserved20_28 :
    9;
uint32_t supcnt :
    2;
    uint32_t Reserved31;
  }
  b;
} USB_DEP0XFRSIZ_TypeDef ;

/*************** OTG_FS device OUT endpoint 0 transfer size register (OTG_FS_DOEPTSIZ0)   0xB10 *************/

#define USB_DOEPTSIZ0_XFRSIZ_Pos       0				/* Transfer size 				*/
#define USB_DOEPTSIZ0_XFRSIZ_Msk       (0x7F<<0)
#define USB_DOEPTSIZ0_PKTCNT_Pos       19				/* Packet count 				*/
#define USB_DOEPTSIZ0_PKTCNT_Msk       (1<<19)
#define USB_DOEPTSIZ0_STUPCNT_Pos      29				/* SETUP packet count 				*/
#define USB_DOEPTSIZ0_STUPCNT_Msk      (0x3<<29)

/*************** OTG_FS device OUT endpoint 0 transfer size register (OTG_FS_DOEPTSIZ)   *************/

#define USB_DOEPTSIZ_XFRSIZ_Pos        0				/* Transfer size 				*/
#define USB_DOEPTSIZ_XFRSIZ_Msk        (0x3FF<<0)
#define USB_DOEPTSIZ_PKTCNT_Pos        19				/* Packet count 				*/
#define USB_DOEPTSIZ_PKTCNT_Msk        (0x7<<19)

/*************** OTG_FS device OUT endpoint 0 transfer size register (OTG_FS_DIEPTSIZ0)    *************/

#define USB_DIEPTSIZ0_XFRSIZ_Pos       0				/* Transfer size 				*/
#define USB_DIEPTSIZ0_XFRSIZ_Msk       (0x7F<<0)
#define USB_DIEPTSIZ0_PKTCNT_Pos       19				/* Packet count 				*/
#define USB_DIEPTSIZ0_PKTCNT_Msk       (3<<19)

/*************** OTG_FS device OUT endpoint 0 transfer size register (OTG_FS_DIEPTSIZ)   *************/

#define USB_DIEPTSIZ_XFRSIZ_Pos        0				/* Transfer size 				*/
#define USB_DIEPTSIZ_XFRSIZ_Msk        (0x3FF<<0)
#define USB_DIEPTSIZ_PKTCNT_Pos        19				/* Packet count 				*/
#define USB_DIEPTSIZ_PKTCNT_Msk        (0x7<<19)



typedef union _USB_PCGCCTL_TypeDef 
{
  uint32_t d32;
  struct
  {
uint32_t stoppclk :
    1;
uint32_t gatehclk :
    1;
uint32_t Reserved2_3 :
    2;
uint32_t phy_susp :
    1;    
uint32_t Reserved5_31 :
    27;
  }
  b;
} USB_PCGCCTL_TypeDef ;



#endif //__USB_REGS_H__


