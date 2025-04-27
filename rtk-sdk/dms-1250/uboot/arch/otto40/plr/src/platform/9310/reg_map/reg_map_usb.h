#ifndef _REG_MAP_USB_H
#define _REG_MAP_USB_H

/*-----------------------------------------------------
 Extraced from file_USB_OHCI.xml
-----------------------------------------------------*/
typedef union {
	struct {
		unsigned int mbz_0:24; //0x0
		unsigned int rev:8; //0x10
	} f;
	unsigned int v;
} HCREVISION_T;
#define HCREVISIONar (0xB8020000)
#define HCREVISIONdv (0x00000010)
#define HCREVISIONrv RVAL(HCREVISION)
#define RMOD_HCREVISION(...) RMOD(HCREVISION, __VA_ARGS__)
#define RIZS_HCREVISION(...) RIZS(HCREVISION, __VA_ARGS__)
#define RFLD_HCREVISION(fld) RFLD(HCREVISION, fld)

typedef union {
	struct {
		unsigned int mbz_0:21; //0x0
		unsigned int rwe:1; //0x0
		unsigned int rwc:1; //0x0
		unsigned int ir:1; //0x0
		unsigned int hcfs:2; //0x0
		unsigned int ble:1; //0x0
		unsigned int cle:1; //0x0
		unsigned int ie:1; //0x0
		unsigned int ple:1; //0x0
		unsigned int cbsr:2; //0x0
	} f;
	unsigned int v;
} HCCONTROL_T;
#define HCCONTROLar (0xB8020004)
#define HCCONTROLdv (0x00000000)
#define HCCONTROLrv RVAL(HCCONTROL)
#define RMOD_HCCONTROL(...) RMOD(HCCONTROL, __VA_ARGS__)
#define RIZS_HCCONTROL(...) RIZS(HCCONTROL, __VA_ARGS__)
#define RFLD_HCCONTROL(fld) RFLD(HCCONTROL, fld)

typedef union {
	struct {
		unsigned int mbz_0:14; //0x0
		unsigned int soc:2; //0x0
		unsigned int mbz_1:12; //0x0
		unsigned int ocr:1; //0x0
		unsigned int blf:1; //0x0
		unsigned int clf:1; //0x0
		unsigned int hcr:1; //0x0
	} f;
	unsigned int v;
} HCCOMMANDSTATUS_T;
#define HCCOMMANDSTATUSar (0xB8020008)
#define HCCOMMANDSTATUSdv (0x00000000)
#define HCCOMMANDSTATUSrv RVAL(HCCOMMANDSTATUS)
#define RMOD_HCCOMMANDSTATUS(...) RMOD(HCCOMMANDSTATUS, __VA_ARGS__)
#define RIZS_HCCOMMANDSTATUS(...) RIZS(HCCOMMANDSTATUS, __VA_ARGS__)
#define RFLD_HCCOMMANDSTATUS(fld) RFLD(HCCOMMANDSTATUS, fld)

typedef union {
	struct {
		unsigned int mbz_0:1; //0x0
		unsigned int oc:1; //0x0
		unsigned int mbz_1:23; //0x0
		unsigned int rhsc:1; //0x0
		unsigned int fno:1; //0x0
		unsigned int ue:1; //0x0
		unsigned int rd:1; //0x0
		unsigned int sf:1; //0x0
		unsigned int wdh:1; //0x0
		unsigned int so:1; //0x0
	} f;
	unsigned int v;
} HCINTERRUPTSTATUS_T;
#define HCINTERRUPTSTATUSar (0xB802000C)
#define HCINTERRUPTSTATUSdv (0x00000000)
#define HCINTERRUPTSTATUSrv RVAL(HCINTERRUPTSTATUS)
#define RMOD_HCINTERRUPTSTATUS(...) RMOD(HCINTERRUPTSTATUS, __VA_ARGS__)
#define RIZS_HCINTERRUPTSTATUS(...) RIZS(HCINTERRUPTSTATUS, __VA_ARGS__)
#define RFLD_HCINTERRUPTSTATUS(fld) RFLD(HCINTERRUPTSTATUS, fld)

typedef union {
	struct {
		unsigned int mie:1; //0x0
		unsigned int oc:1; //0x0
		unsigned int mbz_0:23; //0x0
		unsigned int rhsc:1; //0x0
		unsigned int fno:1; //0x0
		unsigned int ue:1; //0x0
		unsigned int rd:1; //0x0
		unsigned int sf:1; //0x0
		unsigned int wdh:1; //0x0
		unsigned int so:1; //0x0
	} f;
	unsigned int v;
} HCINTERRUPTENABLE_T;
#define HCINTERRUPTENABLEar (0xB8020010)
#define HCINTERRUPTENABLEdv (0x00000000)
#define HCINTERRUPTENABLErv RVAL(HCINTERRUPTENABLE)
#define RMOD_HCINTERRUPTENABLE(...) RMOD(HCINTERRUPTENABLE, __VA_ARGS__)
#define RIZS_HCINTERRUPTENABLE(...) RIZS(HCINTERRUPTENABLE, __VA_ARGS__)
#define RFLD_HCINTERRUPTENABLE(fld) RFLD(HCINTERRUPTENABLE, fld)

typedef union {
	struct {
		unsigned int mie:1; //0x0
		unsigned int oc:1; //0x0
		unsigned int mbz_0:23; //0x0
		unsigned int rhsc:1; //0x0
		unsigned int fno:1; //0x0
		unsigned int ue:1; //0x0
		unsigned int rd:1; //0x0
		unsigned int sf:1; //0x0
		unsigned int wdh:1; //0x0
		unsigned int so:1; //0x0
	} f;
	unsigned int v;
} HCINTERRUPTDISABLE_T;
#define HCINTERRUPTDISABLEar (0xB8020014)
#define HCINTERRUPTDISABLEdv (0x00000000)
#define HCINTERRUPTDISABLErv RVAL(HCINTERRUPTDISABLE)
#define RMOD_HCINTERRUPTDISABLE(...) RMOD(HCINTERRUPTDISABLE, __VA_ARGS__)
#define RIZS_HCINTERRUPTDISABLE(...) RIZS(HCINTERRUPTDISABLE, __VA_ARGS__)
#define RFLD_HCINTERRUPTDISABLE(fld) RFLD(HCINTERRUPTDISABLE, fld)

typedef union {
	struct {
		unsigned int hcca:24; //0x0
		unsigned int mbz_0:8; //0x0
	} f;
	unsigned int v;
} HCHCCA_T;
#define HCHCCAar (0xB8020018)
#define HCHCCAdv (0x00000000)
#define HCHCCArv RVAL(HCHCCA)
#define RMOD_HCHCCA(...) RMOD(HCHCCA, __VA_ARGS__)
#define RIZS_HCHCCA(...) RIZS(HCHCCA, __VA_ARGS__)
#define RFLD_HCHCCA(fld) RFLD(HCHCCA, fld)

typedef union {
	struct {
		unsigned int pced:28; //0x0
		unsigned int mbz_0:4; //0x0
	} f;
	unsigned int v;
} HCPERIODCURRENTED_T;
#define HCPERIODCURRENTEDar (0xB802001C)
#define HCPERIODCURRENTEDdv (0x00000000)
#define HCPERIODCURRENTEDrv RVAL(HCPERIODCURRENTED)
#define RMOD_HCPERIODCURRENTED(...) RMOD(HCPERIODCURRENTED, __VA_ARGS__)
#define RIZS_HCPERIODCURRENTED(...) RIZS(HCPERIODCURRENTED, __VA_ARGS__)
#define RFLD_HCPERIODCURRENTED(fld) RFLD(HCPERIODCURRENTED, fld)

typedef union {
	struct {
		unsigned int ched:28; //0x0
		unsigned int mbz_0:4; //0x0
	} f;
	unsigned int v;
} HCCONTROLHEADED_T;
#define HCCONTROLHEADEDar (0xB8020020)
#define HCCONTROLHEADEDdv (0x00000000)
#define HCCONTROLHEADEDrv RVAL(HCCONTROLHEADED)
#define RMOD_HCCONTROLHEADED(...) RMOD(HCCONTROLHEADED, __VA_ARGS__)
#define RIZS_HCCONTROLHEADED(...) RIZS(HCCONTROLHEADED, __VA_ARGS__)
#define RFLD_HCCONTROLHEADED(fld) RFLD(HCCONTROLHEADED, fld)

typedef union {
	struct {
		unsigned int cced:28; //0x0
		unsigned int mbz_0:4; //0x0
	} f;
	unsigned int v;
} HCCONTROLCURRENTED_T;
#define HCCONTROLCURRENTEDar (0xB8020024)
#define HCCONTROLCURRENTEDdv (0x00000000)
#define HCCONTROLCURRENTEDrv RVAL(HCCONTROLCURRENTED)
#define RMOD_HCCONTROLCURRENTED(...) RMOD(HCCONTROLCURRENTED, __VA_ARGS__)
#define RIZS_HCCONTROLCURRENTED(...) RIZS(HCCONTROLCURRENTED, __VA_ARGS__)
#define RFLD_HCCONTROLCURRENTED(fld) RFLD(HCCONTROLCURRENTED, fld)

typedef union {
	struct {
		unsigned int bhed:28; //0x0
		unsigned int mbz_0:4; //0x0
	} f;
	unsigned int v;
} HCBULKHEADED_T;
#define HCBULKHEADEDar (0xB8020028)
#define HCBULKHEADEDdv (0x00000000)
#define HCBULKHEADEDrv RVAL(HCBULKHEADED)
#define RMOD_HCBULKHEADED(...) RMOD(HCBULKHEADED, __VA_ARGS__)
#define RIZS_HCBULKHEADED(...) RIZS(HCBULKHEADED, __VA_ARGS__)
#define RFLD_HCBULKHEADED(fld) RFLD(HCBULKHEADED, fld)

typedef union {
	struct {
		unsigned int bced:28; //0x0
		unsigned int mbz_0:4; //0x0
	} f;
	unsigned int v;
} HCBULKCURRENTED_T;
#define HCBULKCURRENTEDar (0xB802002C)
#define HCBULKCURRENTEDdv (0x00000000)
#define HCBULKCURRENTEDrv RVAL(HCBULKCURRENTED)
#define RMOD_HCBULKCURRENTED(...) RMOD(HCBULKCURRENTED, __VA_ARGS__)
#define RIZS_HCBULKCURRENTED(...) RIZS(HCBULKCURRENTED, __VA_ARGS__)
#define RFLD_HCBULKCURRENTED(fld) RFLD(HCBULKCURRENTED, fld)

typedef union {
	struct {
		unsigned int dh:28; //0x0
		unsigned int mbz_0:4; //0x0
	} f;
	unsigned int v;
} HCDONEHEAD_T;
#define HCDONEHEADar (0xB8020030)
#define HCDONEHEADdv (0x00000000)
#define HCDONEHEADrv RVAL(HCDONEHEAD)
#define RMOD_HCDONEHEAD(...) RMOD(HCDONEHEAD, __VA_ARGS__)
#define RIZS_HCDONEHEAD(...) RIZS(HCDONEHEAD, __VA_ARGS__)
#define RFLD_HCDONEHEAD(fld) RFLD(HCDONEHEAD, fld)

typedef union {
	struct {
		unsigned int fit:1; //0x0
		unsigned int fsmps:15; //0x0
		unsigned int mbz_0:2; //0x0
		unsigned int fi:14; //0x2EDF
	} f;
	unsigned int v;
} HCFMINTERVAL_T;
#define HCFMINTERVALar (0xB8020034)
#define HCFMINTERVALdv (0x00002EDF)
#define HCFMINTERVALrv RVAL(HCFMINTERVAL)
#define RMOD_HCFMINTERVAL(...) RMOD(HCFMINTERVAL, __VA_ARGS__)
#define RIZS_HCFMINTERVAL(...) RIZS(HCFMINTERVAL, __VA_ARGS__)
#define RFLD_HCFMINTERVAL(fld) RFLD(HCFMINTERVAL, fld)

typedef union {
	struct {
		unsigned int frt:1; //0x0
		unsigned int mbz_0:17; //0x0
		unsigned int fr:14; //0x0
	} f;
	unsigned int v;
} HCFMREMAINING_T;
#define HCFMREMAININGar (0xB8020038)
#define HCFMREMAININGdv (0x00000000)
#define HCFMREMAININGrv RVAL(HCFMREMAINING)
#define RMOD_HCFMREMAINING(...) RMOD(HCFMREMAINING, __VA_ARGS__)
#define RIZS_HCFMREMAINING(...) RIZS(HCFMREMAINING, __VA_ARGS__)
#define RFLD_HCFMREMAINING(fld) RFLD(HCFMREMAINING, fld)

typedef union {
	struct {
		unsigned int mbz_0:16; //0x0
		unsigned int fn:16; //0x0
	} f;
	unsigned int v;
} HCFMNUMBER_T;
#define HCFMNUMBERar (0xB802003C)
#define HCFMNUMBERdv (0x00000000)
#define HCFMNUMBERrv RVAL(HCFMNUMBER)
#define RMOD_HCFMNUMBER(...) RMOD(HCFMNUMBER, __VA_ARGS__)
#define RIZS_HCFMNUMBER(...) RIZS(HCFMNUMBER, __VA_ARGS__)
#define RFLD_HCFMNUMBER(fld) RFLD(HCFMNUMBER, fld)

typedef union {
	struct {
		unsigned int mbz_0:18; //0x0
		unsigned int ps:14; //0x0
	} f;
	unsigned int v;
} HCPERIODICSTART_T;
#define HCPERIODICSTARTar (0xB8020040)
#define HCPERIODICSTARTdv (0x00000000)
#define HCPERIODICSTARTrv RVAL(HCPERIODICSTART)
#define RMOD_HCPERIODICSTART(...) RMOD(HCPERIODICSTART, __VA_ARGS__)
#define RIZS_HCPERIODICSTART(...) RIZS(HCPERIODICSTART, __VA_ARGS__)
#define RFLD_HCPERIODICSTART(fld) RFLD(HCPERIODICSTART, fld)

typedef union {
	struct {
		unsigned int mbz_0:20; //0x0
		unsigned int lst:12; //0x628
	} f;
	unsigned int v;
} HCLSTHRESHOLD_T;
#define HCLSTHRESHOLDar (0xB8020044)
#define HCLSTHRESHOLDdv (0x00000628)
#define HCLSTHRESHOLDrv RVAL(HCLSTHRESHOLD)
#define RMOD_HCLSTHRESHOLD(...) RMOD(HCLSTHRESHOLD, __VA_ARGS__)
#define RIZS_HCLSTHRESHOLD(...) RIZS(HCLSTHRESHOLD, __VA_ARGS__)
#define RFLD_HCLSTHRESHOLD(fld) RFLD(HCLSTHRESHOLD, fld)

typedef union {
	struct {
		unsigned int potpgt:8; //0x2
		unsigned int mbz_0:11; //0x0
		unsigned int nocp:1; //0x0
		unsigned int ocpm:1; //0x0
		unsigned int dt:1; //0x0
		unsigned int psm:1; //0x0
		unsigned int nps:1; //0x0
		unsigned int ndp:8; //0x1
	} f;
	unsigned int v;
} HCRHDESCRIPTORA_T;
#define HCRHDESCRIPTORAar (0xB8020048)
#define HCRHDESCRIPTORAdv (0x02000001)
#define HCRHDESCRIPTORArv RVAL(HCRHDESCRIPTORA)
#define RMOD_HCRHDESCRIPTORA(...) RMOD(HCRHDESCRIPTORA, __VA_ARGS__)
#define RIZS_HCRHDESCRIPTORA(...) RIZS(HCRHDESCRIPTORA, __VA_ARGS__)
#define RFLD_HCRHDESCRIPTORA(fld) RFLD(HCRHDESCRIPTORA, fld)

typedef union {
	struct {
		unsigned int ppcm:16; //0x0
		unsigned int dr:16; //0x0
	} f;
	unsigned int v;
} HCRHDESCRIPTORB_T;
#define HCRHDESCRIPTORBar (0xB802004C)
#define HCRHDESCRIPTORBdv (0x00000000)
#define HCRHDESCRIPTORBrv RVAL(HCRHDESCRIPTORB)
#define RMOD_HCRHDESCRIPTORB(...) RMOD(HCRHDESCRIPTORB, __VA_ARGS__)
#define RIZS_HCRHDESCRIPTORB(...) RIZS(HCRHDESCRIPTORB, __VA_ARGS__)
#define RFLD_HCRHDESCRIPTORB(fld) RFLD(HCRHDESCRIPTORB, fld)

typedef union {
	struct {
		unsigned int crwe:1; //0x0
		unsigned int mbz_0:13; //0x0
		unsigned int ocic:1; //0x0
		unsigned int lpsc:1; //0x0
		unsigned int drwe:1; //0x0
		unsigned int mbz_1:13; //0x0
		unsigned int oci:1; //0x0
		unsigned int lps:1; //0x0
	} f;
	unsigned int v;
} HCRHSTATUS_T;
#define HCRHSTATUSar (0xB8020050)
#define HCRHSTATUSdv (0x00000000)
#define HCRHSTATUSrv RVAL(HCRHSTATUS)
#define RMOD_HCRHSTATUS(...) RMOD(HCRHSTATUS, __VA_ARGS__)
#define RIZS_HCRHSTATUS(...) RIZS(HCRHSTATUS, __VA_ARGS__)
#define RFLD_HCRHSTATUS(fld) RFLD(HCRHSTATUS, fld)

typedef union {
	struct {
		unsigned int mbz_0:11; //0x0
		unsigned int prsc:1; //0x0
		unsigned int ocic:1; //0x0
		unsigned int pssc:1; //0x0
		unsigned int pesc:1; //0x0
		unsigned int csc:1; //0x0
		unsigned int mbz_1:6; //0x0
		unsigned int ldsa:1; //0x0
		unsigned int pps:1; //0x0
		unsigned int mbz_2:3; //0x0
		unsigned int prs:1; //0x0
		unsigned int poci:1; //0x0
		unsigned int pss:1; //0x0
		unsigned int pes:1; //0x0
		unsigned int css:1; //0x0
	} f;
	unsigned int v;
} HCRHPORTSTATUS1_T;
#define HCRHPORTSTATUS1ar (0xB8020054)
#define HCRHPORTSTATUS1dv (0x00000000)
#define HCRHPORTSTATUS1rv RVAL(HCRHPORTSTATUS1)
#define RMOD_HCRHPORTSTATUS1(...) RMOD(HCRHPORTSTATUS1, __VA_ARGS__)
#define RIZS_HCRHPORTSTATUS1(...) RIZS(HCRHPORTSTATUS1, __VA_ARGS__)
#define RFLD_HCRHPORTSTATUS1(fld) RFLD(HCRHPORTSTATUS1, fld)

/*-----------------------------------------------------
 Extraced from file_USB_EHCI.xml
-----------------------------------------------------*/
typedef union {
	struct {
		unsigned int caplength:8; //0x1
		unsigned int mbz_0:8; //0x0
		unsigned int hciversion:16; //0x10
	} f;
	unsigned int v;
} HCCAPBASE_T;
#define HCCAPBASEar (0xB8021000)
#define HCCAPBASEdv (0x01000010)
#define HCCAPBASErv RVAL(HCCAPBASE)
#define RMOD_HCCAPBASE(...) RMOD(HCCAPBASE, __VA_ARGS__)
#define RIZS_HCCAPBASE(...) RIZS(HCCAPBASE, __VA_ARGS__)
#define RFLD_HCCAPBASE(fld) RFLD(HCCAPBASE, fld)

typedef union {
	struct {
		unsigned int mbz_0:8; //0x0
		unsigned int dbg_pn:4; //0x0
		unsigned int mbz_1:3; //0x0
		unsigned int p_indicator:1; //0x0
		unsigned int n_cc:4; //0x1
		unsigned int n_pcc:4; //0x1
		unsigned int prr:1; //0x0
		unsigned int mbz_2:2; //0x0
		unsigned int ppc:1; //0x1
		unsigned int n_ports:4; //0x1
	} f;
	unsigned int v;
} HCSPARAMS_T;
#define HCSPARAMSar (0xB8021004)
#define HCSPARAMSdv (0x00001111)
#define HCSPARAMSrv RVAL(HCSPARAMS)
#define RMOD_HCSPARAMS(...) RMOD(HCSPARAMS, __VA_ARGS__)
#define RIZS_HCSPARAMS(...) RIZS(HCSPARAMS, __VA_ARGS__)
#define RFLD_HCSPARAMS(fld) RFLD(HCSPARAMS, fld)

typedef union {
	struct {
		unsigned int mbz_0:16; //0x0
		unsigned int eecp:8; //0xA0
		unsigned int ist:4; //0x1
		unsigned int mbz_1:1; //0x0
		unsigned int aspc:1; //0x1
		unsigned int pflf:1; //0x1
		unsigned int ac:1; //0x0
	} f;
	unsigned int v;
} HCCPARAMS_T;
#define HCCPARAMSar (0xB8021008)
#define HCCPARAMSdv (0x0000A016)
#define HCCPARAMSrv RVAL(HCCPARAMS)
#define RMOD_HCCPARAMS(...) RMOD(HCCPARAMS, __VA_ARGS__)
#define RIZS_HCCPARAMS(...) RIZS(HCCPARAMS, __VA_ARGS__)
#define RFLD_HCCPARAMS(fld) RFLD(HCCPARAMS, fld)

typedef union {
	struct {
		unsigned int mbz_0:8; //0x0
		unsigned int itc:8; //0x8
		unsigned int mbz_1:4; //0x0
		unsigned int aspmen:1; //0x1
		unsigned int mbz_2:1; //0x0
		unsigned int aspmcount:2; //0x3
		unsigned int lhcreset:1; //0x0
		unsigned int ioaad:1; //0x0
		unsigned int asen:1; //0x0
		unsigned int psen:1; //0x0
		unsigned int flsize:2; //0x0
		unsigned int hcreset:1; //0x0
		unsigned int rs:1; //0x0
	} f;
	unsigned int v;
} USBCMD_T;
#define USBCMDar (0xB8021010)
#define USBCMDdv (0x00080B00)
#define USBCMDrv RVAL(USBCMD)
#define RMOD_USBCMD(...) RMOD(USBCMD, __VA_ARGS__)
#define RIZS_USBCMD(...) RIZS(USBCMD, __VA_ARGS__)
#define RFLD_USBCMD(fld) RFLD(USBCMD, fld)

typedef union {
	struct {
		unsigned int mbz_0:16; //0x0
		unsigned int asstatus:1; //0x0
		unsigned int psstatus:1; //0x0
		unsigned int recl:1; //0x0
		unsigned int hch:1; //0x0
		unsigned int mbz_1:6; //0x0
		unsigned int ioaa:1; //0x0
		unsigned int hserr:1; //0x0
		unsigned int flr:1; //0x0
		unsigned int pcd:1; //0x0
		unsigned int usberrint:1; //0x0
		unsigned int usbint:1; //0x0
	} f;
	unsigned int v;
} USBSTS_T;
#define USBSTSar (0xB8021014)
#define USBSTSdv (0x00000000)
#define USBSTSrv RVAL(USBSTS)
#define RMOD_USBSTS(...) RMOD(USBSTS, __VA_ARGS__)
#define RIZS_USBSTS(...) RIZS(USBSTS, __VA_ARGS__)
#define RFLD_USBSTS(fld) RFLD(USBSTS, fld)

typedef union {
	struct {
		unsigned int mbz_0:26; //0x0
		unsigned int ioaaen:1; //0x0
		unsigned int hserren:1; //0x0
		unsigned int flren:1; //0x0
		unsigned int pcinten:1; //0x0
		unsigned int usberrint_e:1; //0x0
		unsigned int usbint_e:1; //0x0
	} f;
	unsigned int v;
} USBINTR_T;
#define USBINTRar (0xB8021018)
#define USBINTRdv (0x00000000)
#define USBINTRrv RVAL(USBINTR)
#define RMOD_USBINTR(...) RMOD(USBINTR, __VA_ARGS__)
#define RIZS_USBINTR(...) RIZS(USBINTR, __VA_ARGS__)
#define RFLD_USBINTR(fld) RFLD(USBINTR, fld)

typedef union {
	struct {
		unsigned int mbz_0:18; //0x0
		unsigned int fidx:14; //0x0
	} f;
	unsigned int v;
} FRINDEX_T;
#define FRINDEXar (0xB802101C)
#define FRINDEXdv (0x00000000)
#define FRINDEXrv RVAL(FRINDEX)
#define RMOD_FRINDEX(...) RMOD(FRINDEX, __VA_ARGS__)
#define RIZS_FRINDEX(...) RIZS(FRINDEX, __VA_ARGS__)
#define RFLD_FRINDEX(fld) RFLD(FRINDEX, fld)

typedef union {
	struct {
		unsigned int cdss:32; //0x0
	} f;
	unsigned int v;
} CTRLDSSEGMENT_T;
#define CTRLDSSEGMENTar (0xB8021020)
#define CTRLDSSEGMENTdv (0x00000000)
#define CTRLDSSEGMENTrv RVAL(CTRLDSSEGMENT)
#define RMOD_CTRLDSSEGMENT(...) RMOD(CTRLDSSEGMENT, __VA_ARGS__)
#define RIZS_CTRLDSSEGMENT(...) RIZS(CTRLDSSEGMENT, __VA_ARGS__)
#define RFLD_CTRLDSSEGMENT(fld) RFLD(CTRLDSSEGMENT, fld)

typedef union {
	struct {
		unsigned int baseaddr:20; //0x0
		unsigned int mbz_0:12; //0x0
	} f;
	unsigned int v;
} PERIODICLISTBASE_T;
#define PERIODICLISTBASEar (0xB8021024)
#define PERIODICLISTBASEdv (0x00000000)
#define PERIODICLISTBASErv RVAL(PERIODICLISTBASE)
#define RMOD_PERIODICLISTBASE(...) RMOD(PERIODICLISTBASE, __VA_ARGS__)
#define RIZS_PERIODICLISTBASE(...) RIZS(PERIODICLISTBASE, __VA_ARGS__)
#define RFLD_PERIODICLISTBASE(fld) RFLD(PERIODICLISTBASE, fld)

typedef union {
	struct {
		unsigned int lpl:27; //0x0
		unsigned int mbz_0:5; //0x0
	} f;
	unsigned int v;
} ASYNCLISTADDR_T;
#define ASYNCLISTADDRar (0xB8021028)
#define ASYNCLISTADDRdv (0x00000000)
#define ASYNCLISTADDRrv RVAL(ASYNCLISTADDR)
#define RMOD_ASYNCLISTADDR(...) RMOD(ASYNCLISTADDR, __VA_ARGS__)
#define RIZS_ASYNCLISTADDR(...) RIZS(ASYNCLISTADDR, __VA_ARGS__)
#define RFLD_ASYNCLISTADDR(fld) RFLD(ASYNCLISTADDR, fld)

typedef union {
	struct {
		unsigned int mbz_0:31; //0x0
		unsigned int cf:1; //0x0
	} f;
	unsigned int v;
} CONFIGFLAG_T;
#define CONFIGFLAGar (0xB8021050)
#define CONFIGFLAGdv (0x00000000)
#define CONFIGFLAGrv RVAL(CONFIGFLAG)
#define RMOD_CONFIGFLAG(...) RMOD(CONFIGFLAG, __VA_ARGS__)
#define RIZS_CONFIGFLAG(...) RIZS(CONFIGFLAG, __VA_ARGS__)
#define RFLD_CONFIGFLAG(fld) RFLD(CONFIGFLAG, fld)

typedef union {
	struct {
		unsigned int mbz_0:9; //0x0
		unsigned int wkoc_e:1; //0x0
		unsigned int wkdscnnt_e:1; //0x0
		unsigned int wkcnnt_e:1; //0x0
		unsigned int ptc:4; //0x0
		unsigned int pic:2; //0x0
		unsigned int po:1; //0x1
		unsigned int pp:1; //0x0
		unsigned int ls:2; //0x0
		unsigned int mbz_1:1; //0x0
		unsigned int preset:1; //0x0
		unsigned int suspend:1; //0x0
		unsigned int fpr:1; //0x0
		unsigned int occ:1; //0x0
		unsigned int oca:1; //0x0
		unsigned int pedc:1; //0x0
		unsigned int ped:1; //0x0
		unsigned int csc:1; //0x0
		unsigned int ccs:1; //0x0
	} f;
	unsigned int v;
} PORTSC_T;
#define PORTSCar (0xB8021054)
#define PORTSCdv (0x00002000)
#define PORTSCrv RVAL(PORTSC)
#define RMOD_PORTSC(...) RMOD(PORTSC, __VA_ARGS__)
#define RIZS_PORTSC(...) RIZS(PORTSC, __VA_ARGS__)
#define RFLD_PORTSC(fld) RFLD(PORTSC, fld)

typedef union {
	struct {
		unsigned int mbz_0:18; //0x0
		unsigned int pmbc:13; //0x0
		unsigned int en:1; //0x0
	} f;
	unsigned int v;
} INSNREG00_T;
#define INSNREG00ar (0xB8021090)
#define INSNREG00dv (0x00000000)
#define INSNREG00rv RVAL(INSNREG00)
#define RMOD_INSNREG00(...) RMOD(INSNREG00, __VA_ARGS__)
#define RIZS_INSNREG00(...) RIZS(INSNREG00, __VA_ARGS__)
#define RFLD_INSNREG00(fld) RFLD(INSNREG00, fld)

typedef union {
	struct {
		unsigned int ot:16; //0x0
		unsigned int it:16; //0x0
	} f;
	unsigned int v;
} INSNREG01_T;
#define INSNREG01ar (0xB8021094)
#define INSNREG01dv (0x00000000)
#define INSNREG01rv RVAL(INSNREG01)
#define RMOD_INSNREG01(...) RMOD(INSNREG01, __VA_ARGS__)
#define RIZS_INSNREG01(...) RIZS(INSNREG01, __VA_ARGS__)
#define RFLD_INSNREG01(fld) RFLD(INSNREG01, fld)

typedef union {
	struct {
		unsigned int mbz_0:20; //0x0
		unsigned int buf_dep:12; //0x0
	} f;
	unsigned int v;
} INSNREG02_T;
#define INSNREG02ar (0xB8021098)
#define INSNREG02dv (0x00000000)
#define INSNREG02rv RVAL(INSNREG02)
#define RMOD_INSNREG02(...) RMOD(INSNREG02, __VA_ARGS__)
#define RIZS_INSNREG02(...) RIZS(INSNREG02, __VA_ARGS__)
#define RFLD_INSNREG02(fld) RFLD(INSNREG02, fld)

typedef union {
	struct {
		unsigned int mbz_0:23; //0x0
		unsigned int buf_dep:8; //0x0
		unsigned int br_mem_xfer:1; //0x0
	} f;
	unsigned int v;
} INSNREG03_T;
#define INSNREG03ar (0xB802109C)
#define INSNREG03dv (0x00000000)
#define INSNREG03rv RVAL(INSNREG03)
#define RMOD_INSNREG03(...) RMOD(INSNREG03, __VA_ARGS__)
#define RIZS_INSNREG03(...) RIZS(INSNREG03, __VA_ARGS__)
#define RFLD_INSNREG03(fld) RFLD(INSNREG03, fld)

typedef union {
	struct {
		unsigned int mbz_0:27; //0x0
		unsigned int buf_dep:1; //0x0
		unsigned int mbz_1:1; //0x0
		unsigned int down_pe_time:1; //0x0
		unsigned int hccparams_w:1; //0x0
		unsigned int hcsparams_w:1; //0x0
	} f;
	unsigned int v;
} INSNREG04_T;
#define INSNREG04ar (0xB80210A0)
#define INSNREG04dv (0x00000000)
#define INSNREG04rv RVAL(INSNREG04)
#define RMOD_INSNREG04(...) RMOD(INSNREG04, __VA_ARGS__)
#define RIZS_INSNREG04(...) RIZS(INSNREG04, __VA_ARGS__)
#define RFLD_INSNREG04(fld) RFLD(INSNREG04, fld)

typedef union {
	struct {
		unsigned int mbz_0:14; //0x0
		unsigned int vbusy:1; //0x0
		unsigned int vport:4; //0x0
		unsigned int vclm:1; //0x0
		unsigned int vctrl:4; //0x0
		unsigned int vstatus:8; //0x0
	} f;
	unsigned int v;
} INSNREG05_T;
#define INSNREG05ar (0xB80210A4)
#define INSNREG05dv (0x00000000)
#define INSNREG05rv RVAL(INSNREG05)
#define RMOD_INSNREG05(...) RMOD(INSNREG05, __VA_ARGS__)
#define RIZS_INSNREG05(...) RIZS(INSNREG05, __VA_ARGS__)
#define RFLD_INSNREG05(fld) RFLD(INSNREG05, fld)

/*-----------------------------------------------------
 Extraced from file_USB_EHCI_EXT.xml
-----------------------------------------------------*/
typedef union {
	struct {
		unsigned int r_ena_incr16:1; //0x1
		unsigned int r_ena_incr8:1; //0x1
		unsigned int r_ena_incr4:1; //0x1
		unsigned int r_ena_incrx_align:1; //0x1
		unsigned int mbz_0:6; //0x0
		unsigned int app_start_clk_i:1; //0x0
		unsigned int usb2_gating:1; //0x0
		unsigned int mbz_1:2; //0x0
		unsigned int ms_endian_host:1; //0x1
		unsigned int ss_endian_host:1; //0x0
		unsigned int mbz_2:1; //0x0
		unsigned int usb_speed_led_sel:2; //0x3
		unsigned int en_usb_led_n:1; //0x0
		unsigned int usb_led_bs:1; //0x0
		unsigned int mbz_3:1; //0x0
		unsigned int s_suspend_host_n:1; //0x1
		unsigned int s_suspend_sel:1; //0x0
		unsigned int mbz_4:3; //0x0
		unsigned int s_usbhost_burst_sel:1; //0x1
		unsigned int usbhost_pwr_edge:1; //0x0
		unsigned int mbz_5:2; //0x0
		unsigned int s_pgbndry:1; //0x1
	} f;
	unsigned int v;
} CFG1_REG_T;
#define CFG1_REGar (0xB8140200)
#define CFG1_REGdv (0xF0026211)
#define CFG1_REGrv RVAL(CFG1_REG)
#define RMOD_CFG1_REG(...) RMOD(CFG1_REG, __VA_ARGS__)
#define RIZS_CFG1_REG(...) RIZS(CFG1_REG, __VA_ARGS__)
#define RFLD_CFG1_REG(fld) RFLD(CFG1_REG, fld)

typedef union {
	struct {
		unsigned int nak_cnt:8; //0x0
		unsigned int timeout_cnt:8; //0x0
		unsigned int babble_cnt:8; //0x0
		unsigned int crcerr_cnt:8; //0x0
	} f;
	unsigned int v;
} DBG1_REG_T;
#define DBG1_REGar (0xB8140204)
#define DBG1_REGdv (0x00000000)
#define DBG1_REGrv RVAL(DBG1_REG)
#define RMOD_DBG1_REG(...) RMOD(DBG1_REG, __VA_ARGS__)
#define RIZS_DBG1_REG(...) RIZS(DBG1_REG, __VA_ARGS__)
#define RFLD_DBG1_REG(fld) RFLD(DBG1_REG, fld)

typedef union {
	struct {
		unsigned int mbz_0:5; //0x0
		unsigned int dbg_sel:3; //0x0
		unsigned int rsvd:11; //0x1
		unsigned int prt_state:5; //0x0
		unsigned int xfer_resp:4; //0x0
		unsigned int txrx_state:4; //0x0
	} f;
	unsigned int v;
} DBG2_REG_T;
#define DBG2_REGar (0xB8140208)
#define DBG2_REGdv (0x00002000)
#define DBG2_REGrv RVAL(DBG2_REG)
#define RMOD_DBG2_REG(...) RMOD(DBG2_REG, __VA_ARGS__)
#define RIZS_DBG2_REG(...) RIZS(DBG2_REG, __VA_ARGS__)
#define RFLD_DBG2_REG(fld) RFLD(DBG2_REG, fld)

typedef union {
	struct {
		unsigned int mbz_0:24; //0x0
		unsigned int vstatus_out:8; //0x0
	} f;
	unsigned int v;
} PHY0_REG_T;
#define PHY0_REGar (0xB814020C)
#define PHY0_REGdv (0x00000000)
#define PHY0_REGrv RVAL(PHY0_REG)
#define RMOD_PHY0_REG(...) RMOD(PHY0_REG, __VA_ARGS__)
#define RIZS_PHY0_REG(...) RIZS(PHY0_REG, __VA_ARGS__)
#define RFLD_PHY0_REG(fld) RFLD(PHY0_REG, fld)

typedef union {
	struct {
		unsigned int mbz_0:26; //0x0
		unsigned int r_fdiscon:1; //0x1
		unsigned int r_sdiscon:1; //0x0
		unsigned int r_fprtpwr:1; //0x1
		unsigned int r_sprtpwr:1; //0x0
		unsigned int r_fovrcur:1; //0x1
		unsigned int r_sovercur:1; //0x0
	} f;
	unsigned int v;
} PHY_FM_REG_T;
#define PHY_FM_REGar (0xB8140210)
#define PHY_FM_REGdv (0x0000002A)
#define PHY_FM_REGrv RVAL(PHY_FM_REG)
#define RMOD_PHY_FM_REG(...) RMOD(PHY_FM_REG, __VA_ARGS__)
#define RIZS_PHY_FM_REG(...) RIZS(PHY_FM_REG, __VA_ARGS__)
#define RFLD_PHY_FM_REG(fld) RFLD(PHY_FM_REG, fld)

#endif // _REG_MAP_USB_H
