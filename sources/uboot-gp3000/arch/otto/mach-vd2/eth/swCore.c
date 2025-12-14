/*
* ----------------------------------------------------------------
* Copyright c                  Realtek Semiconductor Corporation, 2002
* All rights reserved.
*
*
* Abstract: Switch core driver source code.
*
* $Author: yachang $
*
* ---------------------------------------------------------------
*/
#include <common.h>

#include "asicregs.h"
#include "efuse.h"
#include "swCore.h"

//#define CONFIG_EXT_PHY

#define WRITE_MEM32(addr, val)   (*(volatile unsigned int *) (addr)) = (val)
#define WRITE_MEM16(addr, val)   (*(volatile unsigned short *) (addr)) = (val)
#define READ_MEM32(addr)         (*(volatile unsigned int *) (addr))

DECLARE_GLOBAL_DATA_PTR;

extern char eth0_mac[6];

//------------------------------------------------------------------------
static void _rtl8651_clearSpecifiedAsicTable(uint32_t type, uint32_t count)
{
    extern int swTable_addEntry(uint32_t tableType, uint32_t eidx, void *entryContent_P);
    struct {
        uint32_t _content[8];
    } entry;
    uint32_t idx;

    memset(&entry, 0, sizeof(entry));
    for (idx=0; idx<count; idx++)// Write into hardware
        swTable_addEntry(type, idx, &entry);
}

static void FullAndSemiReset( void )
{
    /* Perform full-reset for sw-core. */
    REG32(SIRR) |= FULL_RST;
    mdelay(50);

    /* Enable TRXRDY */
    REG32(SIRR) |= TRXRDY;
}

static int rtl8651_getAsicEthernetPHYReg(uint32_t phyId, uint32_t regId, uint32_t *rData)
{
    uint32_t status;

    WRITE_MEM32( MDCIOCR, COMMAND_READ | ( phyId << PHYADD_OFFSET ) | ( regId << REGADD_OFFSET ) );

#ifdef RTL865X_TEST
    status = READ_MEM32( MDCIOSR );
#else
#if defined(CONFIG_RTL8196C)
    mdelay(10);   //wei add, for 8196C_test chip patch. mdio data read will delay 1 mdc clock.
#endif
    do {
        status = READ_MEM32( MDCIOSR );
    } while ( ( status & STATUS ) != 0 );
#endif

    status &= 0xffff;
    *rData = status;

    return SUCCESS;
}

static int rtl8651_setAsicEthernetPHYReg(uint32_t phyId, uint32_t regId, uint32_t wData)
{
    WRITE_MEM32( MDCIOCR, COMMAND_WRITE | ( phyId << PHYADD_OFFSET ) | ( regId << REGADD_OFFSET ) | wData );

#ifdef RTL865X_TEST
#else
    while( ( READ_MEM32( MDCIOSR ) & STATUS ) != 0 );		/* wait until command complete */
#endif

    return SUCCESS;
}

static int rtl8651_restartAsicEthernetPHYNway(uint32_t port, uint32_t phyid)
{
    uint32_t statCtrlReg0;

    /* read current PHY reg 0 */
    rtl8651_getAsicEthernetPHYReg( phyid, 0, &statCtrlReg0 );

    /* enable 'restart Nway' bit */
    statCtrlReg0 |= RESTART_AUTONEGO;

    /* write PHY reg 0 */
    rtl8651_setAsicEthernetPHYReg( phyid, 0, statCtrlReg0 );

    return SUCCESS;
}

//====================================================================
#ifdef CONFIG_EXT_PHY
int rtl865xC_setAsicEthernetMIIMode(uint32_t port, uint32_t mode)
{
    if ( port != 0 && port != RTL8651_MII_PORTNUMBER )
        return FAILED;
    if ( mode != LINK_RGMII && mode != LINK_MII_MAC && mode != LINK_MII_PHY )
        return FAILED;

    if ( port == 0 ) {
        /* MII port MAC interface mode configuration */
        WRITE_MEM32( P0GMIICR, ( READ_MEM32( P0GMIICR ) & ~CFG_GMAC_MASK ) | ( mode << LINKMODE_OFFSET ) );
    } else {
        /* MII port MAC interface mode configuration */
        WRITE_MEM32( P5GMIICR, ( READ_MEM32( P5GMIICR ) & ~CFG_GMAC_MASK ) | ( mode << LINKMODE_OFFSET ) );
    }
    return SUCCESS;

}

int rtl865xC_setAsicEthernetRGMIITiming(uint32_t port, uint32_t Tcomp, uint32_t Rcomp)
{
    if ( port != 0 && port != RTL8651_MII_PORTNUMBER )
        return FAILED;
    if ( Tcomp < RGMII_TCOMP_0NS || Tcomp > RGMII_TCOMP_2NS || Rcomp < RGMII_RCOMP_0NS || Rcomp > RGMII_RCOMP_3DOT5NS )
        return FAILED;

    if ( port == 0 ) {
        WRITE_MEM32(P0GMIICR, ( ( ( READ_MEM32(P0GMIICR) & ~RGMII_TCOMP_MASK ) | Tcomp ) & ~RGMII_RCOMP_MASK ) | Rcomp );
    } else {
        WRITE_MEM32(P5GMIICR, ( ( ( READ_MEM32(P5GMIICR) & ~RGMII_TCOMP_MASK ) | Tcomp ) & ~RGMII_RCOMP_MASK ) | Rcomp );
    }

    WRITE_MEM32(P0GMIICR, READ_MEM32(P0GMIICR)|Conf_done);

    return SUCCESS;
}

int RTL8367B_probe(unsigned int phyid)
{
    extern int RL6000_write(unsigned int addr, unsigned int data);
    extern int RL6000_read(unsigned int addr, unsigned int *data);
    extern void RL6000_RGMII(void);
    extern void RL6000_cpu_tag(int enable);
    extern short rtk_switch_init(void);

    unsigned int id = 0;

    //set port0 RGMII mode
    rtl865xC_setAsicEthernetMIIMode(0, LINK_RGMII);
    //set Port 0 Interface Type Configuration to GMII/MII/RGMII interface
    WRITE_MEM32(PITCR, (READ_MEM32(PITCR)&0xfffffffc)|Port0_TypeCfg_GMII_MII_RGMII);
    //set external phyid
    WRITE_MEM32(PCRP0, (READ_MEM32(PCRP0)&(~ExtPHYID_MASK))|(phyid << ExtPHYID_OFFSET));

    //chip reset, must wait more than 0.9 sec
    RL6000_write(0x1322, 0x1);
    mdelay(1000);

    RL6000_write(0x13C2, 0x0249);
    RL6000_read(0x1300, &id);

    if (!id || (0xffff == (id & 0xffff)))
        goto probe_fail;

    rtl865xC_setAsicEthernetRGMIITiming(0, RGMII_TCOMP_2NS, RGMII_RCOMP_2NS);
    WRITE_MEM32(MACCR,(READ_MEM32(MACCR)&0xffffcfff)|0x01<<12);//select 100Mhz system clk

    //disable auto-polling
    WRITE_MEM32(PCRP0, (READ_MEM32(PCRP0)&~PollLinkStatus));
    WRITE_MEM32(PCRP1, (READ_MEM32(PCRP1)&~PollLinkStatus));
    WRITE_MEM32(PCRP2, (READ_MEM32(PCRP2)&~PollLinkStatus));
    WRITE_MEM32(PCRP3, (READ_MEM32(PCRP3)&~PollLinkStatus));
    WRITE_MEM32(PCRP4, (READ_MEM32(PCRP4)&~PollLinkStatus));

    //set port0 force mode
    WRITE_MEM32(PCRP0, (READ_MEM32(PCRP0)&(~ForceSpeedMask)&(~ExtPHYID_MASK))|ForceSpeed1000M|EnForceMode);
    mdelay(10);

    //port0 force link
    WRITE_MEM32(PCRP0, READ_MEM32(PCRP0)|ForceLink);

    rtk_switch_init();
    RL6000_write(0x1b03, 0x0222);	//LED setting, LED0/LED1/LED2:Link/Act
    RL6000_RGMII();
    RL6000_cpu_tag(0);

    printf("RTL8367B is detected! \n\r");
    return SUCCESS;

probe_fail:
    FullAndSemiReset();
    rtl8651_setAsicEthernetPHYReg(0, 31, 0);	//select back to page 0
    return FAILED;
}

int RTL8211E_probe(unsigned int phyid)
{
    unsigned int uid, tmp;

    //set port0 RGMII mode
    rtl865xC_setAsicEthernetMIIMode(0, LINK_RGMII);
    //set Port 0 Interface Type Configuration to GMII/MII/RGMII interface
    WRITE_MEM32(PITCR, (READ_MEM32(PITCR)&0xfffffffc)|Port0_TypeCfg_GMII_MII_RGMII);
    //set external phyid
    WRITE_MEM32(PCRP0, (READ_MEM32(PCRP0)&(~ExtPHYID_MASK))|(phyid << ExtPHYID_OFFSET));

    rtl8651_getAsicEthernetPHYReg(phyid, 2, &tmp);
    uid = tmp << 16;
    rtl8651_getAsicEthernetPHYReg(phyid, 3, &tmp);
    uid = uid | tmp;

    if (uid != 0x001cc915)
        goto probe_fail;

    /* LED setting */
    //select page 7
    rtl8651_setAsicEthernetPHYReg(phyid, 31, 7);
    //extension page 44
    rtl8651_setAsicEthernetPHYReg(phyid, 30, 0x2c);
    //Register 28 bit[2:0] = 111 (set LED 0 On when 10/100/1000Link)
    rtl8651_getAsicEthernetPHYReg(phyid, 28, &tmp);
    tmp |= 0x7;
    rtl8651_setAsicEthernetPHYReg(phyid, 28, tmp);
    //Register 26 bit[4] = 1 (set LED 0 blinking when Activity)
    rtl8651_getAsicEthernetPHYReg(phyid, 26, &tmp);
    tmp |= 0x10;
    rtl8651_setAsicEthernetPHYReg(phyid, 26, tmp);
    //select page 0
    rtl8651_setAsicEthernetPHYReg(phyid, 31, 0);

    rtl865xC_setAsicEthernetRGMIITiming(0, RGMII_TCOMP_2NS, RGMII_RCOMP_1NS);
    WRITE_MEM32(MACCR,(READ_MEM32(MACCR)&0xffffcfff)|0x01<<12);//select 100Mhz system clk

    printf("RTL8211E is detected! \n\r");
    return SUCCESS;

probe_fail:
    FullAndSemiReset();
    return FAILED;
}

//cathy, interface with phyid 0 (ex: RTL8367B) should be probed at the last in the list !!!!
static struct external_giga extGigaList[] = {
    {RTL8211E, RTL8211E_PHYID, RTL8211E_probe},
    {RTL8367B, RTL8367B_PHYID, RTL8367B_probe},
    {-1, -1, NULL}
};

int probeExtGiga(void)
{
    struct external_giga *extGiga;
    int extInf = EXT_NONE;

    /* fix pointer */
    if (gd->reloc_off) {
        ulong addr;
        for (extGiga = &extGigaList[0]; extGiga->probeFn != NULL; extGiga++) {
            addr = (ulong) extGiga->probeFn + gd->reloc_off;
            extGiga->probeFn =  (int(*)(unsigned int))addr;
        }
    }


    for (extGiga = &extGigaList[0]; extGiga->probeFn != NULL; extGiga++) {
        if (SUCCESS == extGiga->probeFn(extGiga->extPhyId)) {
            extInf = extGiga->inf;
            break;

        }
    }

    return extInf;
}
#endif //CONFIG_EXT_PHY

int rtk_set_gphy(unsigned int port)
{
    unsigned short efuse_val = 0x0;
    unsigned int phyID=0;

    phyID = (REG32(EMB_PHY_ID) >> (port*5)) & 0x1f;

    /* =================== GPHY: RC  ===================*/
    efuse_val = efuse_read(((3*port)+8));
   	//printf("[%s(port %d)]RC K_val=0x%X, phyID=0x%X\n\r",__func__,port,efuse_val,phyID);

    //select page 0xbcd
    if(efuse_val == 0) {
        /* There is No value in Efuse, use the default value */
        rtl8651_setAsicEthernetPHYReg(phyID, 31, 0xbcd);
        rtl8651_setAsicEthernetPHYReg(phyID, 22, 0xeeee);
        rtl8651_setAsicEthernetPHYReg(phyID, 23, 0xeeee);
    } else {
        rtl8651_setAsicEthernetPHYReg(phyID, 31, 0xbcd);
        rtl8651_setAsicEthernetPHYReg(phyID, 22, efuse_val);
        rtl8651_setAsicEthernetPHYReg(phyID, 23, efuse_val);
    }

    /*===================  GPHY: R  ===================*/
    efuse_val = efuse_read(((3*port)+9));
    //printf("[%s(port %d)]R K_val=0x%x, phyID=0x%X\n\r",__func__,port,efuse_val,phyID);
    //select page 0xbce
    if(efuse_val == 0) {
        /* There is No value in Efuse, use the default value */
        rtl8651_setAsicEthernetPHYReg(phyID, 31, 0xbce);
        rtl8651_setAsicEthernetPHYReg(phyID, 16, 0x7777);
        rtl8651_setAsicEthernetPHYReg(phyID, 17, 0x7777);
    } else {
        rtl8651_setAsicEthernetPHYReg(phyID, 31, 0xbce);
        rtl8651_setAsicEthernetPHYReg(phyID, 16, efuse_val);
        rtl8651_setAsicEthernetPHYReg(phyID, 17, efuse_val);
    }
    /*===================   GPHY: AMP value =================== */
    efuse_val = efuse_read(((3*port)+10));
    //printf("[%s(port %d)]AMP K_val=0x%x, phyID=0x%X\n\r",__func__,port,efuse_val,phyID);

    //select page 0xbca
    if(efuse_val == 0) {
        /* There is No value in Efuse, use the default value */
        rtl8651_setAsicEthernetPHYReg(phyID, 31, 0xbca);
        rtl8651_setAsicEthernetPHYReg(phyID, 22, 0x7777);
    } else {
        rtl8651_setAsicEthernetPHYReg(phyID, 31, 0xbca);
        rtl8651_setAsicEthernetPHYReg(phyID, 22, efuse_val);
    }

    efuse_val = efuse_get_phy_cap(port);
    //printf("[%s(port %d)]phy cap=0x%x, phyID=0x%X\n\r",__func__,port,efuse_val,phyID);

    rtl8651_setAsicEthernetPHYReg(phyID, 31, 0x0a47);
    rtl8651_setAsicEthernetPHYReg(phyID, 17, efuse_val);

	return 0;
}

int swCore_init(void)
{
    int port = 0;
    //int extInf;

    /* Full reset and semreset */
    FullAndSemiReset();

#ifdef CONFIG_EXT_PHY
    extInf = probeExtGiga();
    if (EXT_NONE != extInf)
        port = 1;
    else
        printf("Ext. phy is not found. \n\r");
#endif

    /* rtl8651_clearAsicAllTable */
    REG32(0xbb804234) = 0;
    REG32(0xbb804234) = 0x7f;	//initialize all hwnat tables excluding multicast and net interface table

    _rtl8651_clearSpecifiedAsicTable(TYPE_MULTICAST_TABLE, RTL8651_IPMULTICASTTBL_SIZE);
    _rtl8651_clearSpecifiedAsicTable(TYPE_NETINTERFACE_TABLE, RTL865XC_NETINTERFACE_NUMBER);

    REG32(PCRP0) |= AcptMaxLen_16K | MacSwReset;
    REG32(PCRP1) |= (1 << ExtPHYID_OFFSET) | AcptMaxLen_16K | MacSwReset;
    REG32(PCRP2) |= (2 << ExtPHYID_OFFSET) | AcptMaxLen_16K | MacSwReset;
    REG32(PCRP3) |= (3 << ExtPHYID_OFFSET) | AcptMaxLen_16K | MacSwReset;
    REG32(PCRP4) |= (4 << ExtPHYID_OFFSET) | AcptMaxLen_16K | MacSwReset;


    /* Set PVID of all ports to 8 */
    REG32(PVCR0) = (0x8 << 16) | 0x8;
    REG32(PVCR1) = (0x8 << 16) | 0x8;
    REG32(PVCR2) = (0x8 << 16) | 0x8;
    REG32(PVCR3) = (0x8 << 16) | 0x8;


    /* Enable L2 lookup engine and spanning tree functionality */
    // REG32(MSCR) = EN_L2 | EN_L3 | EN_L4 | EN_IN_ACL;
    REG32(MSCR) = EN_L2;
    REG32(QNUMCR) = P0QNum_1 | P1QNum_1 | P2QNum_1 | P3QNum_1 | P4QNum_1;

    //move TXRX RDY to rtl8676 init
    //REG32(SIRR) |= TRXRDY;

    /*PHY FlowControl. Default enable*/
    for (; port<MAX_PORT_NUMBER; port++) {
        /* Set Flow Control capability. */
        rtl8651_restartAsicEthernetPHYNway(port+1, port);

        if(rtk_set_gphy(port)) {
            printf("Set GPHY parameters failed\n\r");
        }
    }

    //printf("%s(%d): no l2 table\n",__FUNCTION__,__LINE__);
    //rtl8651_setAsicL2Table((ether_addr_t*)(&eth0_mac), 0);
    //printf("%s(%d)\n",__FUNCTION__,__LINE__);
    REG32(FFCR) = EN_UNUNICAST_TOCPU | EN_UNMCAST_TOCPU; // rx broadcast and unicast packet

    return 0;
}



