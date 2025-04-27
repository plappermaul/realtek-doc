/*
 * Copyright (C) 2009-2016 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 75479 $
 * $Date: 2017-01-20 15:17:16 +0800 (Fri, 20 Jan 2017) $
 *
 * Purpose : PHY 8218D Driver APIs.
 *
 * Feature : PHY 8218D Driver APIs
 *
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/debug/rt_log.h>
#include <soc/type.h>
#include <hal/common/halctrl.h>
#include <hal/mac/miim_common_drv.h>
#include <hal/phy/phy_cust1.h>

#include <AQ_RegMacro.h>
#include <AQ_User.h>
#include <AQ_API.h>
#include <AQ_PhyInterface.h>
#include <AQ_PlatformRoutines.h>
#include <AQ_Flash_Internal.h>
#include <AQ_RegMaps.h>
#include <AQ_RegGroupMaxSizes.h>



/*
 * Symbol Definition
 */
#undef PHY_CUST_BOOTLOAD_ENABLE
#undef PROV_TABLE
#define PHY_CUST_PHY_IMAGE_FILE             "/lib/modules/PHY_image"
#define PHY_CUST_PHY_PROV_TABLE_FILE        "/lib/modules/PHY_prov_table"
#define PHY_CUST_AQ_PHY_PORT_MAX            4
#define PORT_NUM_IN_AQR407                  4

#ifdef __KERNEL__
  #define PHY_CUST_SERDESEYE_KERNEL_PRINT_RAWDATA     1
#endif
#define AQ_API_SERDESEYE_GRIDSIZE_X 32
#define AQ_API_SERDESEYE_GRIDSIZE_Y 64
#define AQ_API_SERDESEYE_FILENAME_LEN   64
#define AQ_API_SERDESEYE_STRBUFF_SIZE   32


#define PHY_CUST1_AQ_GEN2           2
#define PHY_CUST1_AQ_GEN3           3

typedef struct phy_cust1_db_s
{
    uint32          aqGen[RTK_MAX_PORT_PER_UNIT];
} phy_cust1_db_t;


phy_cust1_db_t   *pPhyCust1Db[RTK_MAX_NUM_OF_UNIT] = { NULL };


/*
 * Data Declaration
 */
rt_phyInfo_t phy_cust1_info =
{
    .phy_num    = PORT_NUM_IN_AQR407,
    .eth_type   = HWP_XGE,
    .isComboPhy = {0, 0, 0, 0, 0, 0, 0, 0},
};


/*
 * Macro Declaration
 */
#define PHY_AQR_LOG(fmt, args...)      RT_LOG(LOG_INFO, (MOD_HAL|MOD_PHY), "%s:%d:"fmt, __FUNCTION__, __LINE__, ##args)
#define PHY_AQR_ERR(fmt, args...)      RT_LOG(LOG_WARNING, (MOD_HAL|MOD_PHY), "WARN.%s:%d:"fmt, __FUNCTION__, __LINE__, ##args)


/*
 * Function Declaration
 */
void _phy_aqr407_rtkPort_to_aqPort(uint32 unit, rtk_port_t port, AQ_API_Port *paq_port);


void
_aqApi_dump_GetConnectionStatus(AQ_API_Port *paq_port)
{
    AQ_Retcode  aq_ret;
    AQ_API_ConnectionStatus connStatus;

    phy_osal_printf("AQ_API_GetConnectionStatus:\n");
    phy_osal_memset(&connStatus, 0, sizeof(AQ_API_ConnectionStatus));
    if (AQ_RET_OK != (aq_ret = AQ_API_GetConnectionStatus(paq_port, &connStatus)))
    {
        PHY_AQR_ERR("U%u P%u AQ_API_GetConnectionStatus error aq_ret=%u", paq_port->PHY_ID.unit, paq_port->PHY_ID.port, aq_ret);
        return;
    }

    phy_osal_printf("  state:%u\n", connStatus.state);
    phy_osal_printf("  rxLinkUp:%u\n", connStatus.rxLinkUp);
    phy_osal_printf("  txReady:%u\n", connStatus.txReady);
    phy_osal_printf("  sysIntfCurrentRate:%u\n", connStatus.sysIntfCurrentRate);
    phy_osal_printf("  sysIntfCurrentMode:%u\n", connStatus.sysIntfCurrentMode);
    phy_osal_printf("  masterSlaveStatus:%u\n", connStatus.masterSlaveStatus);
    phy_osal_printf("  MDI_MDI_X:%u\n", connStatus.MDI_MDI_X);
    phy_osal_printf("  duplex:%u\n", connStatus.duplex);
    phy_osal_printf("  numberOfConnectionAttempts:%u\n", connStatus.numberOfConnectionAttempts);
    phy_osal_printf("  energyOnLine:%u\n", connStatus.energyOnLine);
    phy_osal_printf("  farEndDeviceExists:%u\n", connStatus.farEndDeviceExists);
}


void
_aqApi_dump_Get100M_1G_ConnectionStatus(AQ_API_Port *paq_port)
{
    AQ_Retcode  aq_ret;
    AQ_API_100M_1G_ConnectionStatus connStatus;


    phy_osal_printf("AQ_API_Get100M_1G_ConnectionStatus:\n");
    phy_osal_memset(&connStatus, 0, sizeof(AQ_API_100M_1G_ConnectionStatus));
    if (AQ_RET_OK != (aq_ret = AQ_API_Get100M_1G_ConnectionStatus(paq_port, &connStatus)))
    {
        PHY_AQR_ERR("U%u P%u AQ_API_Get100M_1G_ConnectionStatus error aq_ret=%u", paq_port->PHY_ID.unit, paq_port->PHY_ID.port, aq_ret);
        return;
    }
    phy_osal_printf("  connected:%u\n", connStatus.connected);
    phy_osal_printf("  sysIntfGoodTxEthernetFrames:%u\n", connStatus.sysIntfGoodTxEthernetFrames);
    phy_osal_printf("  sysIntfGoodRxEthernetFrames:%u\n", connStatus.sysIntfGoodRxEthernetFrames);
    phy_osal_printf("  sysIntfBadTxEthernetFrames:%u\n", connStatus.sysIntfBadTxEthernetFrames);
    phy_osal_printf("  sysIntfBadRxEthernetFrames:%u\n", connStatus.sysIntfBadRxEthernetFrames);

    phy_osal_printf("  sgmiiRxFalseCarrierEvents:%u\n", connStatus.sgmiiRxFalseCarrierEvents);
    phy_osal_printf("  sgmiiCollisionEvents:%u\n", connStatus.sgmiiCollisionEvents);
    phy_osal_printf("  rxFalseCarrierEvents:%u\n", connStatus.rxFalseCarrierEvents);
    phy_osal_printf("  sysIntfBadRxEthernetFrames:%u\n", connStatus.sysIntfBadRxEthernetFrames);
    phy_osal_printf("  collisionEvents:%u\n", connStatus.collisionEvents);
    phy_osal_printf("  frameAlignmentEvents:%u\n", connStatus.frameAlignmentEvents);
    phy_osal_printf("  runtFrameEvents:%u\n", connStatus.runtFrameEvents);
}


void
_aqApi_dump_Get10G_ConnectionStatus(AQ_API_Port *paq_port)
{
    AQ_Retcode  aq_ret;
    AQ_API_10G_ConnectionStatus connStatus;

    phy_osal_printf("AQ_API_Get10G_ConnectionStatus:\n");
    phy_osal_memset(&connStatus, 0, sizeof(AQ_API_10G_ConnectionStatus));
    if (AQ_RET_OK != (aq_ret = AQ_API_Get10G_ConnectionStatus(paq_port, &connStatus)))
    {
        PHY_AQR_ERR("U%u P%u AQ_API_Get10G_ConnectionStatus error aq_ret=%u", paq_port->PHY_ID.unit, paq_port->PHY_ID.port, aq_ret);
        return;
    }
    phy_osal_printf("  connected:%u\n", connStatus.connected);
    phy_osal_printf("  erroredLDPC_Frames:%u\n", connStatus.erroredLDPC_Frames);
    phy_osal_printf("  erroredCRC_Frames:%u\n", connStatus.erroredCRC_Frames);
    phy_osal_printf("  Iteration1_CorrectedLDPC_Frames:%u\n", connStatus.Iteration1_CorrectedLDPC_Frames);
    phy_osal_printf("  Iteration2_CorrectedLDPC_Frames:%u\n", connStatus.Iteration2_CorrectedLDPC_Frames);
    phy_osal_printf("  Iteration3_CorrectedLDPC_Frames:%u\n", connStatus.Iteration3_CorrectedLDPC_Frames);
    phy_osal_printf("  Iteration4_CorrectedLDPC_Frames:%u\n", connStatus.Iteration4_CorrectedLDPC_Frames);
    phy_osal_printf("  Iteration5_CorrectedLDPC_Frames:%u\n", connStatus.Iteration5_CorrectedLDPC_Frames);
    phy_osal_printf("  Iteration6_CorrectedLDPC_Frames:%u\n", connStatus.Iteration6_CorrectedLDPC_Frames);
    phy_osal_printf("  Iteration7_CorrectedLDPC_Frames:%u\n", connStatus.Iteration7_CorrectedLDPC_Frames);
    phy_osal_printf("  Iteration8_CorrectedLDPC_Frames:%u\n", connStatus.Iteration8_CorrectedLDPC_Frames);
    phy_osal_printf("  goodLDPC_Frames:%u\n", connStatus.goodLDPC_Frames);
    phy_osal_printf("  averageNumberOfIterations:%u\n", connStatus.averageNumberOfIterations);
    phy_osal_printf("  transmitPower:%u\n", connStatus.transmitPower);
    phy_osal_printf("  linkPartnerTransmitPower:%u\n", connStatus.linkPartnerTransmitPower);
    phy_osal_printf("  netIntfGoodTxEthernetFrames:%u\n", connStatus.netIntfGoodTxEthernetFrames);
    phy_osal_printf("  netIntfGoodRxEthernetFrames:%u\n", connStatus.netIntfGoodRxEthernetFrames);
    phy_osal_printf("  netIntfBadTxEthernetFrames:%u\n", connStatus.netIntfBadTxEthernetFrames);
    phy_osal_printf("  netIntfBadRxEthernetFrames:%u\n", connStatus.netIntfBadRxEthernetFrames);
    phy_osal_printf("  sysIntfGoodTxEthernetFrames:%u\n", connStatus.sysIntfGoodTxEthernetFrames);
    phy_osal_printf("  sysIntfGoodRxEthernetFrames:%u\n", connStatus.sysIntfGoodRxEthernetFrames);
    phy_osal_printf("  sysIntfBadTxEthernetFrames:%u\n", connStatus.sysIntfBadTxEthernetFrames);
    phy_osal_printf("  sysIntfBadRxEthernetFrames:%u\n", connStatus.sysIntfBadRxEthernetFrames);
    phy_osal_printf("  pairA_OperatingMargin:%u\n", connStatus.pairA_OperatingMargin);
    phy_osal_printf("  pairB_OperatingMargin:%u\n", connStatus.pairB_OperatingMargin);
    phy_osal_printf("  pairC_OperatingMargin:%u\n", connStatus.pairC_OperatingMargin);
    phy_osal_printf("  pairD_OperatingMargin:%u\n", connStatus.pairD_OperatingMargin);
    phy_osal_printf("  pairA_LogicalConnectivity:%u\n", connStatus.pairA_LogicalConnectivity);
    phy_osal_printf("  pairB_LogicalConnectivity:%u\n", connStatus.pairB_LogicalConnectivity);
    phy_osal_printf("  pairC_LogicalConnectivity:%u\n", connStatus.pairC_LogicalConnectivity);
    phy_osal_printf("  pairD_LogicalConnectivity:%u\n", connStatus.pairD_LogicalConnectivity);
    phy_osal_printf("  pairB_RelativeSkew:%u\n", connStatus.pairB_RelativeSkew);
    phy_osal_printf("  pairC_RelativeSkew:%u\n", connStatus.pairC_RelativeSkew);
    phy_osal_printf("  pairD_RelativeSkew:%u\n", connStatus.pairD_RelativeSkew);
    phy_osal_printf("  transmitPower:%u\n", connStatus.transmitPower);

}

void
_aqApi_dump_GetStaticConfiguration(AQ_API_Port *paq_port)
{
    AQ_Retcode  aq_ret;
    AQ_API_StaticConfiguration connStatus;

    phy_osal_printf("AQ_API_GetStaticConfiguration:\n");
    phy_osal_memset(&connStatus, 0, sizeof(AQ_API_StaticConfiguration));
    if (AQ_RET_OK != (aq_ret = AQ_API_GetStaticConfiguration(paq_port, &connStatus)))
    {
        PHY_AQR_ERR("U%u P%u AQ_API_GetStaticConfiguration error aq_ret=%u", paq_port->PHY_ID.unit, paq_port->PHY_ID.port, aq_ret);
        return;
    }
    phy_osal_printf("  OUI:%u\n", connStatus.OUI);
    phy_osal_printf("  IEEE_ModelNumber:%u\n", connStatus.IEEE_ModelNumber);
    phy_osal_printf("  IEEE_RevisionNumber:%u\n", connStatus.IEEE_RevisionNumber);
}

void
_aqApi_dump_GetAutonegotiationControl(AQ_API_Port *paq_port)
{
    AQ_Retcode  aq_ret;
    AQ_API_AutonegotiationControl   autoNegoCtrl;

    phy_osal_printf("AQ_API_GetAutonegotiationControl:\n");
    if (AQ_RET_OK != (aq_ret = AQ_API_GetAutonegotiationControl(paq_port, &autoNegoCtrl)))
    {
        PHY_AQR_ERR("U%u P%u AQ_API_GetAutonegotiationControl error aq_ret=%u", paq_port->PHY_ID.unit, paq_port->PHY_ID.port, aq_ret);
        return;
    }

    phy_osal_printf("  advertise10GBASE_T:%u\n", autoNegoCtrl.advertise10GBASE_T);
    phy_osal_printf("  shortReach:%u\n", autoNegoCtrl.shortReach);
    phy_osal_printf("  advertise5G:%u\n", autoNegoCtrl.advertise5G);
    phy_osal_printf("  advertise2_5G:%u\n", autoNegoCtrl.advertise2_5G);
    phy_osal_printf("  advertise1000BASE_T_FullDuplex:%u\n", autoNegoCtrl.advertise1000BASE_T_FullDuplex);
    phy_osal_printf("  advertise1000BASE_T_HalfDuplex:%u\n", autoNegoCtrl.advertise1000BASE_T_HalfDuplex);
    phy_osal_printf("  advertise100BASE_TX_FullDuplex:%u\n", autoNegoCtrl.advertise100BASE_TX_FullDuplex);
    phy_osal_printf("  advertise100BASE_TX_HalfDuplex:%u\n", autoNegoCtrl.advertise100BASE_TX_HalfDuplex);
    phy_osal_printf("  advertise10BASE_T_FullDuplex:%u\n", autoNegoCtrl.advertise10BASE_T_FullDuplex);
    phy_osal_printf("  advertise10BASE_T_HalfDuplex:%u\n", autoNegoCtrl.advertise10BASE_T_HalfDuplex);
    phy_osal_printf("  advertiseSymmetricPAUSE:%u\n", autoNegoCtrl.advertiseSymmetricPAUSE);
    phy_osal_printf("  advertiseAsymmetricPAUSE:%u\n", autoNegoCtrl.advertiseAsymmetricPAUSE);
    phy_osal_printf("  enableAutomaticDownshift:%u\n", autoNegoCtrl.enableAutomaticDownshift);
    phy_osal_printf("  automaticDownshiftRetryAttempts:%u\n", autoNegoCtrl.automaticDownshiftRetryAttempts);
    phy_osal_printf("  masterSlaveControl:%u\n", autoNegoCtrl.masterSlaveControl);
    phy_osal_printf("  portType:%u\n", autoNegoCtrl.portType);
    phy_osal_printf("  MDI_MDI_X_Control:%u\n", autoNegoCtrl.MDI_MDI_X_Control);
    phy_osal_printf("  forceConnRate:%u\n", autoNegoCtrl.forceConnRate);
    phy_osal_printf("  bzCapabilities:_2_5G:%u\n", autoNegoCtrl.bzCapabilities._2_5G);
    phy_osal_printf("      _5G:%u\n", autoNegoCtrl.bzCapabilities._5G);
    phy_osal_printf("      _2_5G_EEE:%u\n", autoNegoCtrl.bzCapabilities._2_5G_EEE);
    phy_osal_printf("      _5G_EEE:%u\n", autoNegoCtrl.bzCapabilities._5G_EEE);
    phy_osal_printf("      _2_5G_fastRetrain:%u\n", autoNegoCtrl.bzCapabilities._2_5G_fastRetrain);
    phy_osal_printf("      _5G_fastRetrain:%u\n", autoNegoCtrl.bzCapabilities._5G_fastRetrain);
    phy_osal_printf("      _2_5G_THP_BypassRequest:%u\n", autoNegoCtrl.bzCapabilities._2_5G_THP_BypassRequest);
    phy_osal_printf("      _5G_THP_BypassRequest:%u\n", autoNegoCtrl.bzCapabilities._5G_THP_BypassRequest);
}


void
_aqApi_dump_GetStatus(AQ_API_Port *paq_port)
{
    AQ_Retcode  aq_ret;
    AQ_API_Status   status;

    phy_osal_printf("AQ_API_GetStatus:\n");
    phy_osal_memset(&status, 0, sizeof(AQ_API_Status));
    if (AQ_RET_OK != (aq_ret = AQ_API_GetStatus(paq_port, &status)))
    {
        PHY_AQR_ERR("U%u P%u AQ_API_GetStatus error aq_ret=%u", paq_port->PHY_ID.unit, paq_port->PHY_ID.port, aq_ret);
        return;
    }

    phy_osal_printf("  programmed:%u\n", status.loadedImageInformation.programmed);
    phy_osal_printf("  imageID_String:%s\n", status.loadedImageInformation.imageID_String);
    phy_osal_printf("  imageMajorRevisionNumber:%u\n", (uint32)status.loadedImageInformation.imageMajorRevisionNumber);
    phy_osal_printf("  imageMinorRevisionNumber:%u\n", (uint32)status.loadedImageInformation.imageMinorRevisionNumber);
    phy_osal_printf("  imageROM_ID_Number:%u\n", (uint32)status.loadedImageInformation.imageROM_ID_Number);

}


void
_aqApi_dump(AQ_API_Port *paq_port)
{
    _aqApi_dump_GetConnectionStatus(paq_port);

    _aqApi_dump_Get100M_1G_ConnectionStatus(paq_port);

    _aqApi_dump_Get10G_ConnectionStatus(paq_port);

    _aqApi_dump_GetStaticConfiguration(paq_port);

    _aqApi_dump_GetAutonegotiationControl(paq_port);

    _aqApi_dump_GetStatus(paq_port);
}

/* Function Name:
 *      _phy_aqr407_rtkPort_to_aqPort
 * Description:
 *      Transfert rtk unit/port to AQ_API_Port for Gen2 PHY: 407/408/409
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      paq_port    AQ API port struct
 * Return:
 *      None
 * Note:
 *      None
 */
void
_phy_aqr407_rtkPort_to_aqPort(uint32 unit, rtk_port_t port, AQ_API_Port *paq_port)
{
    phy_osal_memset(paq_port, 0, sizeof(AQ_API_Port));
    paq_port->PHY_ID.unit = unit;
    paq_port->PHY_ID.port = port;

    if (pPhyCust1Db[unit] == NULL)
    {
        RT_ERR(RT_ERR_MEM_ALLOC, (MOD_HAL), "unit=%u,port=%u,database not init!", unit, port);
        return;
    }

    switch(pPhyCust1Db[unit]->aqGen[port])
    {
      case PHY_CUST1_AQ_GEN2:
        paq_port->device = AQ_DEVICE_EUR;
        break;
      case PHY_CUST1_AQ_GEN3:
        paq_port->device = AQ_DEVICE_EUR; /* AQ FAE suggest use GEN2 driver for current stage,
                                           * it shall chage to correct DEVICE when the driver supported!!!
                                           */
        break;
      default:
        RT_ERR(RT_ERR_MEM_ALLOC, (MOD_HAL), "unit=%u,port=%u,unknown GEN%u!", unit, port, pPhyCust1Db[unit]->aqGen[port]);
        return;
    }

}


/* Function Name:
 *      _phy_aqr407_aqSysIntfMode_to_rtkSdsMode
 * Description:
 *      Transfert AQ System Interface Mode to RTK SerDes Mode
 * Input:
 *      aqSysIntfOpMode - AQ System Interface Mode
 * Output:
 *      prtkSerdesMode  - rtk serdes mode
 * Return:
 *      None
 * Note:
 *      None
 */
int32
_phy_aqr407_aqSysIntfMode_to_rtkSdsMode(AQ_API_SysIntfOpMode aqSysIntfOpMode, rt_serdesMode_t *prtkSerdesMode)
{
    switch (aqSysIntfOpMode)
    {
      case AQ_API_SIOM_USXGMII:
        *prtkSerdesMode = RTK_MII_USXGMII_10GQXGMII;
        break;
      case AQ_API_SIOM_XFI:
        *prtkSerdesMode = RTK_MII_10GR;
        break;
      case AQ_API_SIOM_Other:
        *prtkSerdesMode = RTK_MII_2500Base_X;
        break;
      case AQ_API_SIOM_SGMII:
        *prtkSerdesMode = RTK_MII_SGMII;
        break;
      case AQ_API_SIOM_OFF:
        *prtkSerdesMode = RTK_MII_DISABLE;
        break;
      default:
        *prtkSerdesMode = RTK_MII_DISABLE;
        return RT_ERR_FAILED;
    }


    return RT_ERR_OK;

}

/* Function Name:
 *      _phy_cust1_swMacPollPhyStatusTxRxPause_get
 * Description:
 *      Get tx_pause and rx_pause status
 * Input:
 *      unit - unit id
 *      port - port id
 *      paq_port - aq port struct
 * Output:
 *      ptx_pause - tx pause status
 *      prx_pause - rx pause status
 * Return:
 *      None
 * Note:
 *      None
 */
static int32
_phy_cust1_swMacPollPhyStatusTxRxPause_get(uint32 unit, uint32 port, AQ_API_Port *paq_port, uint32 *ptx_pause, uint32 *prx_pause)
{
    AQ_Retcode  aq_ret;
    AQ_API_LinkPartnerStatus    linkPartnerStatus;
    AQ_API_AutonegotiationControl   autoNegoCtrl;

    *ptx_pause = 0;
    *prx_pause = 0;

    phy_osal_memset(&linkPartnerStatus, 0, sizeof(AQ_API_LinkPartnerStatus));
    if (AQ_RET_OK != (aq_ret = AQ_API_GetLinkPartnerStatus(paq_port, &linkPartnerStatus)))
    {
        PHY_AQR_ERR("U%u P%u AQ_API_GetLinkPartnerStatus error aq_ret=%u", unit, port, aq_ret);
        return RT_ERR_FAILED;
    }

    phy_osal_memset(&autoNegoCtrl, 0, sizeof(AQ_API_AutonegotiationControl));
    if (AQ_RET_OK != (aq_ret = AQ_API_GetAutonegotiationControl(paq_port, &autoNegoCtrl)))
    {
        PHY_AQR_ERR("U%u P%u AQ_API_GetLinkPartnerStatus error aq_ret=%u", unit, port, aq_ret);
        return RT_ERR_FAILED;
    }

    if ((autoNegoCtrl.advertiseSymmetricPAUSE == False) &&
        (autoNegoCtrl.advertiseAsymmetricPAUSE == True) &&
        (linkPartnerStatus.symmetricPAUSE == True) &&
        (linkPartnerStatus.asymmetricPAUSE == True))
    {
        *ptx_pause = 1;
    }
    else if ((autoNegoCtrl.advertiseSymmetricPAUSE == True) &&
        (autoNegoCtrl.advertiseAsymmetricPAUSE == False) &&
        (linkPartnerStatus.symmetricPAUSE == True))
    {
        *ptx_pause = 1;
        *prx_pause = 1;
    }
    else if ((autoNegoCtrl.advertiseSymmetricPAUSE == True) &&
        (autoNegoCtrl.advertiseAsymmetricPAUSE == True) &&
        (linkPartnerStatus.symmetricPAUSE == False) &&
        (linkPartnerStatus.asymmetricPAUSE == True))
    {
        *prx_pause = 1;
    }
    else if ((autoNegoCtrl.advertiseSymmetricPAUSE == True) &&
        (autoNegoCtrl.advertiseAsymmetricPAUSE == True) &&
        (linkPartnerStatus.symmetricPAUSE == True))
    {
        *ptx_pause = 1;
        *prx_pause = 1;
    }
    return RT_ERR_OK;
}

/* Function Name:
 *      phy_cust1_macIntfSerdesMode_get
 * Description:
 *      Get serdes mode
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pserdesMode - serdes mode
 * Return:
 *      None
 * Note:
 *      None
 */
int32
phy_cust1_macIntfSerdesMode_get(uint32 unit, rtk_port_t port, rt_serdesMode_t *pserdesMode)
{
    int32       ret;
    AQ_Retcode  aq_ret;
    AQ_API_Port aq_port;
    AQ_API_ConnectionStatus connStatus;

    _phy_aqr407_rtkPort_to_aqPort(unit, port, &aq_port);

    phy_osal_memset(&connStatus, 0, sizeof(AQ_API_ConnectionStatus));
    if (AQ_RET_OK != (aq_ret = AQ_API_GetConnectionStatus(&aq_port, &connStatus)))
    {
        PHY_AQR_ERR("U%u P%u AQ_API_GetConnectionStatus error aq_ret=%u", unit, port, aq_ret);
        return RT_ERR_FAILED;
    }

    //PHY_AQR_LOG("U%u P%u AQ sysIntfMode:%u sysRate:%u connState:%u", unit, port, connStatus.sysIntfCurrentMode, connStatus.sysIntfCurrentRate, connStatus.state);
    if (RT_ERR_OK != (ret = _phy_aqr407_aqSysIntfMode_to_rtkSdsMode(connStatus.sysIntfCurrentMode, pserdesMode)))
    {
        PHY_AQR_ERR("U%u P%u unknown AQ sysIntfMode:%u (sysRate:%u connState:%u)", unit, port, connStatus.sysIntfCurrentMode, connStatus.sysIntfCurrentRate, connStatus.state);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      phy_cust1_swMacPollPhyStatus_get
 * Description:
 *      Get PHY status
 * Input:
 *      unit    - unit ID
 *      port    - port ID
 * Output:
 *      pphyStatus  - PHY status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_cust1_swMacPollPhyStatus_get(uint32 unit, uint32 port, rtk_port_swMacPollPhyStatus_t *pphyStatus)
{
    AQ_Retcode  aq_ret;
    AQ_API_Port aq_port;
    AQ_API_ConnectionStatus     connStatus;
    uint32      reslStatus = 0, speed5_4, speed10_9, tx_pause, rx_pause, duplex;
    uint32      isConnected;

    _phy_aqr407_rtkPort_to_aqPort(unit, port, &aq_port);

    phy_osal_memset(&connStatus, 0, sizeof(AQ_API_ConnectionStatus));
    if (AQ_RET_OK != (aq_ret = AQ_API_GetConnectionStatus(&aq_port, &connStatus)))
    {
        PHY_AQR_ERR("U%u P%u AQ_API_GetConnectionStatus error aq_ret=%u", unit, port, aq_ret);
        return RT_ERR_FAILED;
    }


    PHY_AQR_LOG("U%u P%u state=%u duplex=%u sysIntfCurrentMode=%u sysIntfCurrentRate=%u", unit, port, connStatus.state, connStatus.duplex, connStatus.sysIntfCurrentMode, connStatus.sysIntfCurrentRate);

    switch (connStatus.state)
    {
      case AQ_API_ConnS_10G_Connected:
        PHY_AQR_LOG("U%u P%u link 10G", unit, port);
        isConnected = TRUE;
        speed5_4 = 0x0;
        speed10_9 = 0x1;
        break;
      case AQ_API_ConnS_5G_Connected:
        PHY_AQR_LOG("U%u P%u link 5G", unit, port);
        isConnected = TRUE;
        speed5_4 = 0x2;
        speed10_9 = 0x1;
        break;
      case AQ_API_ConnS_2_5G_Connected:
        PHY_AQR_LOG("U%u P%u link 2.5G", unit, port);
        isConnected = TRUE;
        speed5_4 = 0x1;
        speed10_9 = 0x1;
        break;
      case AQ_API_ConnS_1G_Connected:
        PHY_AQR_LOG("U%u P%u link 1G", unit, port);
        isConnected = TRUE;
        speed5_4 = 0x2;
        speed10_9 = 0x0;
        break;
      case AQ_API_ConnS_100M_Connected:
        PHY_AQR_LOG("U%u P%u link 100M", unit, port);
        isConnected = TRUE;
        speed5_4 = 0x1;
        speed10_9 = 0x0;
        break;
      case AQ_API_ConnS_LoopbackMode:
        if (connStatus.sysIntfCurrentRate == AQ_API_SIR_10G)
        {
            isConnected = TRUE;
            PHY_AQR_LOG("U%u P%u loopback 10G", unit, port);
            speed5_4 = 0x0;
            speed10_9 = 0x1;
        }
        else if (connStatus.sysIntfCurrentRate ==  AQ_API_SIR_1G)
        {
            PHY_AQR_LOG("U%u P%u loopback 1G", unit, port);
            isConnected = TRUE;
            speed5_4 = 0x2;
            speed10_9 = 0x0;
        }
        else
        {
            PHY_AQR_LOG("U%u P%u loopback", unit, port);
            isConnected = FALSE;
            speed5_4 = 0x3;
            speed10_9 = 0x3;
        }
        break;
      default:
        isConnected = FALSE;
        /* link is down or port is inactive... */
        if (connStatus.rxLinkUp == True)
        {
            PHY_AQR_LOG("U%u P%u link unknown", unit, port);
        }
        speed5_4 = 0x3;
        speed10_9 = 0x3;
        break;
    }

    //if (connStatus.rxLinkUp == True)
    if (isConnected == TRUE)
    {
        _phy_cust1_swMacPollPhyStatusTxRxPause_get(unit, port, &aq_port, &tx_pause, &rx_pause);
        duplex = (connStatus.duplex == True) ? 1 : 0;
    }
    else
    {
        duplex = 1;
        tx_pause = 0;
        rx_pause = 0;
    }

    reslStatus = REG32_FIELD_SET(reslStatus, 0, PHY_RESL_REG_MDI_OFFSET, PHY_RESL_REG_MDI_MASK);
    //reslStatus = REG32_FIELD_SET(reslStatus, ((connStatus.rxLinkUp == True) ? 1 : 0), PHY_RESL_REG_LINK_OFFSET, PHY_RESL_REG_LINK_MASK);
    reslStatus = REG32_FIELD_SET(reslStatus, ((isConnected == TRUE) ? 1 : 0), PHY_RESL_REG_LINK_OFFSET, PHY_RESL_REG_LINK_MASK);
    reslStatus = REG32_FIELD_SET(reslStatus, duplex, PHY_RESL_REG_DUPLEX_OFFSET, PHY_RESL_REG_DUPLEX_MASK);
    reslStatus = REG32_FIELD_SET(reslStatus, speed5_4, PHY_RESL_REG_SPEED0_OFFSET, PHY_RESL_REG_SPEED0_MASK);
    reslStatus = REG32_FIELD_SET(reslStatus, ((tx_pause) ? 1 : 0), PHY_RESL_REG_TX_PAUSE_OFFSET, PHY_RESL_REG_TX_PAUSE_MASK);
    reslStatus = REG32_FIELD_SET(reslStatus, ((rx_pause) ? 1 : 0), PHY_RESL_REG_RX_PAUSE_OFFSET, PHY_RESL_REG_RX_PAUSE_MASK);
    reslStatus = REG32_FIELD_SET(reslStatus, speed10_9, PHY_RESL_REG_SPEED1_OFFSET, PHY_RESL_REG_SPEED1_MASK);
    reslStatus = REG32_FIELD_SET(reslStatus, ((connStatus.masterSlaveStatus == AQ_API_MSS_Master) ? 1 : 0), PHY_RESL_REG_MASTER_SLAVE_OFFSET, PHY_RESL_REG_MASTER_SLAVE_MASK);

    pphyStatus->reslStatus = reslStatus;
    pphyStatus->media = PORT_MEDIA_COPPER;

    return RT_ERR_OK;
}


/* Function Name:
 *      _phy_identify_aqrGen2
 * Description:
 *      Identify the port is AQ Gen2 PHY or not?
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      reg1d3   - data of register 1.3
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - is intra serdes PHY
 *      RT_ERR_FAILED           - access failure or others
 *      RT_ERR_PHY_NOT_MATCH    - is not intra serdes PHY
 * Note:
 *      None
 */
int32
_phy_identify_aqGen2(uint32 unit, rtk_port_t port, uint32 reg1d3)
{
    if ((reg1d3 == 0xB550) || (reg1d3 == 0xB551) || (reg1d3 == 0xB552))  /* AQR407 */
    {
        PHY_AQR_LOG("U%u P%u match AQR407", unit, port);
        return RT_ERR_OK;
    }

    if ((reg1d3 == 0xB560) || (reg1d3 == 0xB561) || (reg1d3 == 0xB562)) /* AQR408 */
    {
        PHY_AQR_LOG("U%u P%u match AQR408", unit, port);
        return RT_ERR_OK;
    }

    if ((reg1d3 == 0xB570) || (reg1d3 == 0xB571) || (reg1d3 == 0xB572)) /* AQR409 */
    {
        PHY_AQR_LOG("U%u P%u match AQR409", unit, port);
        return RT_ERR_OK;
    }

    if ((reg1d3 == 0xB4E2) || (reg1d3 == 0xB4E1) || (reg1d3 == 0xB4E0)) /* AQR107 */
    {
        PHY_AQR_LOG("U%u P%u match AQR107", unit, port);
        return RT_ERR_OK;
    }

    if ((reg1d3 == 0xB4F2) || (reg1d3 == 0xB4F1) || (reg1d3 == 0xB4F0)) /* AQR108 */
    {
        PHY_AQR_LOG("U%u P%u match AQR108", unit, port);
        return RT_ERR_OK;
    }

    if ((reg1d3 == 0xB502) || (reg1d3 == 0xB501) || (reg1d3 == 0xB500)) /* AQR109 */
    {
        PHY_AQR_LOG("U%u P%u match AQR109", unit, port);
        return RT_ERR_OK;
    }

    return RT_ERR_PHY_NOT_MATCH;
}


/* Function Name:
 *      _phy_identify_aqrGen2
 * Description:
 *      Identify the port is AQ Gen2 PHY or not?
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      reg1d3   - data of register 1.3
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - is intra serdes PHY
 *      RT_ERR_FAILED           - access failure or others
 *      RT_ERR_PHY_NOT_MATCH    - is not intra serdes PHY
 * Note:
 *      None
 */
int32
_phy_identify_aqGen3(uint32 unit, rtk_port_t port, uint32 reg1d3)
{
    if ((reg1d3 == 0xB700) || (reg1d3 == 0xB6E0)) /* AQR411 */
    {
        PHY_AQR_LOG("U%u P%u match AQR411", unit, port);
        return RT_ERR_OK;
    }

    if ((reg1d3 == 0xB710) || (reg1d3 == 0xB6F0)) /* AQR412 */
    {
        PHY_AQR_LOG("U%u P%u match AQR412", unit, port);
        return RT_ERR_OK;
    }

    if (reg1d3 == 0xB610) /* AQR111 */
    {
        PHY_AQR_LOG("U%u P%u match AQR111", unit, port);
        return RT_ERR_OK;
    }

    if (reg1d3 == 0xB660) /* AQR112 */
    {
        PHY_AQR_LOG("U%u P%u match AQR112", unit, port);
        return RT_ERR_OK;
    }

    return RT_ERR_PHY_NOT_MATCH;

}


/* Function Name:
 *      phy_identify_cust1
 * Description:
 *      Identify the port is CUST1  PHY or not?
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      model_id - model id
 *      rev_id   - revision id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - is intra serdes PHY
 *      RT_ERR_FAILED           - access failure or others
 *      RT_ERR_PHY_NOT_MATCH    - is not intra serdes PHY
 * Note:
 *      None
 */
int32
phy_identify_cust1(uint32 unit, rtk_port_t port, uint32 model_id, uint32 rev_id)
{
    int32           ret;
    uint32          reg1d3 = 0, aqGen = 0;
    hal_control_t   *pHalCtrl = NULL;
    drv_smi_mdxProtoSel_t   mdxProto;

    PHY_AQR_LOG("U%u P%u", unit, port);

    RT_PARAM_CHK((NULL == (pHalCtrl = hal_ctrlInfo_get(unit))), RT_ERR_FAILED);

    if (hal_miim_portSmiMdxProto_get(unit, port, &mdxProto) != RT_ERR_OK)
    {
        return RT_ERR_PHY_NOT_MATCH;
    }

    if (mdxProto != DRV_SMI_MDX_PROTO_C45)
    {
        PHY_AQR_LOG("U%u P%u change SMI to C45", unit, port);
        /* change to C45 for probing AQ PHY */
        if (hal_miim_portSmiMdxProto_set(unit, port, DRV_SMI_MDX_PROTO_C45) != RT_ERR_OK)
        {
            PHY_AQR_LOG("U%u P%u change SMI to C45 fail", unit, port);
            /* PHY can only be accessed through C45 MDC/MDIO format. if SMI is not able to using C45, then return there is no match. */
            return RT_ERR_PHY_NOT_MATCH;
        }
    }

    /* PMA/PMD device identifier (Registers 1.3) */
    MACDRV(pHalCtrl)->fMdrv_miim_mmd_read(unit, port, 1, 3, &reg1d3);
    PHY_AQR_LOG("U%u P%u reg3 0x%x", unit, port, reg1d3);

    if ((ret = _phy_identify_aqGen3(unit, port, reg1d3)) == RT_ERR_OK)
    {
        /* at this point, remain the SMI as C45 */
        aqGen = PHY_CUST1_AQ_GEN3;
        goto EXIT_OK;
    }

    if ((ret = _phy_identify_aqGen2(unit, port, reg1d3)) == RT_ERR_OK)
    {
        /* at this point, remain the SMI as C45 */
        aqGen = PHY_CUST1_AQ_GEN2;
        goto EXIT_OK;
    }


    /* recover SMI setting */
    if (mdxProto != DRV_SMI_MDX_PROTO_C45)
    {
        PHY_AQR_LOG("U%u P%u restore SMI to C22", unit, port);
        hal_miim_portSmiMdxProto_set(unit, port, mdxProto);
    }

    return RT_ERR_PHY_NOT_MATCH;

  EXIT_OK:
    if (pPhyCust1Db[unit] == NULL)
    {
        if ((pPhyCust1Db[unit] = phy_osal_alloc(sizeof(phy_cust1_db_t))) == NULL)
            return RT_ERR_MEM_ALLOC;
        phy_osal_memset(pPhyCust1Db[unit], 0, sizeof(phy_cust1_db_t));
    }

    PHY_AQR_LOG("U%u P%u is GEN%u", unit, port, aqGen);
    pPhyCust1Db[unit]->aqGen[port] = aqGen;
    return RT_ERR_OK;
}


int32
_aqApi_autoNegoEnable_get(AQ_API_Port *paq_port, rtk_enable_t *pEnable)
{
    AQ_Retcode      aq_ret;
    AQ_API_AutonegotiationControl   autoNegoCtrl;

    if (AQ_RET_OK != (aq_ret = AQ_API_GetAutonegotiationControl(paq_port, &autoNegoCtrl)))
    {
        PHY_AQR_ERR("U%u P%u AQ_API_GetAutonegotiationControl error aq_ret=%u", paq_port->PHY_ID.unit, paq_port->PHY_ID.port, aq_ret);
        return RT_ERR_FAILED;
    }

    if (autoNegoCtrl.forceConnRate == AQ_API_ForceConnRate_Autonegotiate)
    {
        *pEnable = ENABLED;
    }
    else
    {
        *pEnable = DISABLED;
    }

    return RT_ERR_OK;
}

int32
_aqApi_autoNegoEnable_set(AQ_API_Port *paq_port, rtk_enable_t enable)
{
    AQ_Retcode      aq_ret;
    AQ_API_AutonegotiationControl   autoNegoCtrl;

    if (AQ_RET_OK != (aq_ret = AQ_API_GetAutonegotiationControl(paq_port, &autoNegoCtrl)))
    {
        PHY_AQR_ERR("U%u P%u AQ_API_GetAutonegotiationControl error aq_ret=%u", paq_port->PHY_ID.unit, paq_port->PHY_ID.port, aq_ret);
        return RT_ERR_FAILED;
    }

    if (enable == ENABLED)
    {
        autoNegoCtrl.forceConnRate = AQ_API_ForceConnRate_Autonegotiate;
    }
    else
    {
        autoNegoCtrl.forceConnRate = AQ_API_ForceConnRate_100M;
    }

    if (AQ_RET_OK != (aq_ret = AQ_API_SetAutonegotiationControl(paq_port, &autoNegoCtrl)))
    {
        PHY_AQR_ERR("U%u P%u AQ_API_SetAutonegotiationControl error aq_ret=%u", paq_port->PHY_ID.unit, paq_port->PHY_ID.port, aq_ret);
        return RT_ERR_FAILED;
    }

    if (AQ_RET_OK != (aq_ret = AQ_API_RestartAutonegotiation(paq_port)))
    {
        PHY_AQR_ERR("U%u P%u AQ_API_RestartAutonegotiation error aq_ret=%u", paq_port->PHY_ID.unit, paq_port->PHY_ID.port, aq_ret);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

int32
_aqApi_autoNegoAbility_get(AQ_API_Port *paq_port, rtk_port_phy_ability_t *pAbility)
{
    AQ_Retcode      aq_ret;
    AQ_API_AutonegotiationControl   autoNegoCtrl;

    if (AQ_RET_OK != (aq_ret = AQ_API_GetAutonegotiationControl(paq_port, &autoNegoCtrl)))
    {
        PHY_AQR_ERR("U%u P%u AQ_API_GetAutonegotiationControl error aq_ret=%u", paq_port->PHY_ID.unit, paq_port->PHY_ID.port, aq_ret);
        return RT_ERR_FAILED;
    }

    if (autoNegoCtrl.advertise10GBASE_T == True)
        pAbility->adv_10GBase_T = 1;

    if (autoNegoCtrl.advertise1000BASE_T_FullDuplex == True)
        pAbility->Full_1000 = 1;

    if (autoNegoCtrl.advertise1000BASE_T_HalfDuplex == True)
        pAbility->Half_1000 = 1;

    if (autoNegoCtrl.advertise100BASE_TX_FullDuplex == True)
        pAbility->Full_100 = 1;

    if (autoNegoCtrl.advertise100BASE_TX_HalfDuplex == True)
        pAbility->Half_100 = 1;

    if (autoNegoCtrl.advertise10BASE_T_FullDuplex == True)
        pAbility->Full_10 = 1;

    if (autoNegoCtrl.advertise10BASE_T_HalfDuplex == True)
        pAbility->Half_10 = 1;

    if (autoNegoCtrl.advertise5G == True)
        pAbility->adv_5G = 1;

    if (autoNegoCtrl.advertise2_5G == True)
        pAbility->adv_2_5G = 1;



    if (autoNegoCtrl.advertiseSymmetricPAUSE == True)
        pAbility->FC = 1;

    if (autoNegoCtrl.advertiseAsymmetricPAUSE == True)
        pAbility->AsyFC = 1;

    return RT_ERR_OK;
}

int32
_aqApi_autoNegoAbility_set(AQ_API_Port *paq_port, rtk_port_phy_ability_t *pAbility)
{
    AQ_Retcode      aq_ret;
    AQ_API_AutonegotiationControl   autoNegoCtrl;

    phy_osal_memset(&autoNegoCtrl, 0, sizeof(AQ_API_AutonegotiationControl));
    if (AQ_RET_OK != (aq_ret = AQ_API_GetAutonegotiationControl(paq_port, &autoNegoCtrl)))
    {
        PHY_AQR_ERR("U%u P%u AQ_API_GetAutonegotiationControl error aq_ret=%u", paq_port->PHY_ID.unit, paq_port->PHY_ID.port, aq_ret);
        return RT_ERR_FAILED;
    }

    autoNegoCtrl.advertise10GBASE_T = (pAbility->adv_10GBase_T) ? True : False;

    autoNegoCtrl.advertise1000BASE_T_FullDuplex = (pAbility->Full_1000) ? True : False;

    autoNegoCtrl.advertise1000BASE_T_HalfDuplex = (pAbility->Half_1000) ? True : False;

    autoNegoCtrl.advertise100BASE_TX_FullDuplex = (pAbility->Full_100) ? True : False;

    autoNegoCtrl.advertise100BASE_TX_HalfDuplex = (pAbility->Half_100) ? True : False;

    autoNegoCtrl.advertise10BASE_T_FullDuplex = (pAbility->Full_10) ? True : False;

    autoNegoCtrl.advertise10BASE_T_HalfDuplex = (pAbility->Half_10) ? True : False;

    autoNegoCtrl.advertise5G = autoNegoCtrl.bzCapabilities._5G = (pAbility->adv_5G) ? True : False;

    autoNegoCtrl.advertise2_5G = autoNegoCtrl.bzCapabilities._2_5G = (pAbility->adv_2_5G) ? True : False;

    autoNegoCtrl.advertiseSymmetricPAUSE = (pAbility->FC) ? True : False;

    autoNegoCtrl.advertiseAsymmetricPAUSE = (pAbility->AsyFC) ? True : False;

    if (AQ_RET_OK != (aq_ret = AQ_API_SetAutonegotiationControl(paq_port, &autoNegoCtrl)))
    {
        PHY_AQR_ERR("U%u P%u AQ_API_SetAutonegotiationControl error aq_ret=%u", paq_port->PHY_ID.unit, paq_port->PHY_ID.port, aq_ret);
        return RT_ERR_FAILED;
    }

    if (AQ_RET_OK != (aq_ret = AQ_API_RestartAutonegotiation(paq_port)))
    {
        PHY_AQR_ERR("U%u P%u AQ_API_RestartAutonegotiation error aq_ret=%u", paq_port->PHY_ID.unit, paq_port->PHY_ID.port, aq_ret);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

int32
_aqApi_duplex_get(AQ_API_Port *paq_port, uint32 *pDuplex)
{
    AQ_Retcode  aq_ret;
    AQ_API_ConnectionStatus connStatus;

    phy_osal_memset(&connStatus, 0, sizeof(AQ_API_ConnectionStatus));
    if (AQ_RET_OK != (aq_ret = AQ_API_GetConnectionStatus(paq_port, &connStatus)))
    {
        PHY_AQR_ERR("U%u P%u AQ_API_GetConnectionStatus error aq_ret=%u", paq_port->PHY_ID.unit, paq_port->PHY_ID.port, aq_ret);
        return RT_ERR_FAILED;
    }

    *pDuplex = (connStatus.duplex == True) ? 1 : 0;
    return RT_ERR_OK;
}


int32
_aqApi_duplex_set(AQ_API_Port *paq_port, uint32 duplex)
{
    if (duplex)
        return RT_ERR_OK;
    else
        return RT_ERR_FAILED;   /* not support */
}

int32
_aqApi_speed_get(AQ_API_Port *paq_port, uint32 *pSpeed)
{
    AQ_Retcode  aq_ret;
    AQ_API_ConnectionStatus connStatus;
    AQ_API_AutonegotiationControl   autoNegoCtrl;

    phy_osal_memset(&connStatus, 0, sizeof(AQ_API_ConnectionStatus));
    if (AQ_RET_OK != (aq_ret = AQ_API_GetConnectionStatus(paq_port, &connStatus)))
    {
        PHY_AQR_ERR("U%u P%u AQ_API_GetConnectionStatus error aq_ret=%u", paq_port->PHY_ID.unit, paq_port->PHY_ID.port, aq_ret);
        return RT_ERR_FAILED;
    }

    //PHY_AQR_LOG("U%u P%u aq state:%u", unit, port, connStatus.state);

    switch (connStatus.state)
    {
      case AQ_API_ConnS_10G_Connected:
        *pSpeed = PORT_SPEED_10G;
        return RT_ERR_OK;
      case AQ_API_ConnS_5G_Connected:
        *pSpeed = PORT_SPEED_5G;
        return RT_ERR_OK;
      case AQ_API_ConnS_2_5G_Connected:
        *pSpeed = PORT_SPEED_2_5G;
        return RT_ERR_OK;
      case AQ_API_ConnS_1G_Connected:
        *pSpeed = PORT_SPEED_1000M;
        return RT_ERR_OK;
      case AQ_API_ConnS_100M_Connected:
        *pSpeed = PORT_SPEED_100M;
        return RT_ERR_OK;
      default:
        //*pSpeed = PORT_SPEED_10G;
        //PHY_AQR_ERR("U%u P%u aq state:%u unreconized", paq_port->PHY_ID.unit, paq_port->PHY_ID.port, connStatus.state);
        //return RT_ERR_FAILED;
        break;
    }

    phy_osal_memset(&autoNegoCtrl, 0, sizeof(AQ_API_AutonegotiationControl));
    if (AQ_RET_OK != (aq_ret = AQ_API_GetAutonegotiationControl(paq_port, &autoNegoCtrl)))
    {
        PHY_AQR_ERR("U%u P%u AQ_API_GetAutonegotiationControl error aq_ret=%u", paq_port->PHY_ID.unit, paq_port->PHY_ID.port, aq_ret);
        return RT_ERR_FAILED;
    }

    if (autoNegoCtrl.forceConnRate == AQ_API_ForceConnRate_100M)
    {
        *pSpeed = PORT_SPEED_100M;
        return RT_ERR_OK;
    }

    return RT_ERR_FAILED;
}


int32
_aqApi_speed_set(AQ_API_Port *paq_port, uint32 speed)
{
    AQ_Retcode  aq_ret;
    AQ_API_AutonegotiationControl   autoNegoCtrl;

    phy_osal_memset(&autoNegoCtrl, 0, sizeof(AQ_API_AutonegotiationControl));
    if (AQ_RET_OK != (aq_ret = AQ_API_GetAutonegotiationControl(paq_port, &autoNegoCtrl)))
    {
        PHY_AQR_ERR("U%u P%u AQ_API_GetAutonegotiationControl error aq_ret=%u", paq_port->PHY_ID.unit, paq_port->PHY_ID.port, aq_ret);
        return RT_ERR_FAILED;
    }

    switch(speed)
    {
      case PORT_SPEED_100M:
        autoNegoCtrl.forceConnRate = AQ_API_ForceConnRate_100M;
        break;
      case PORT_SPEED_10M:
      case PORT_SPEED_1000M:
      case PORT_SPEED_500M:
      case PORT_SPEED_2G:
      case PORT_SPEED_10G:
      default:
        return RT_ERR_INPUT;
    }

    if (AQ_RET_OK != (aq_ret = AQ_API_SetAutonegotiationControl(paq_port, &autoNegoCtrl)))
    {
        PHY_AQR_ERR("U%u P%u AQ_API_SetAutonegotiationControl error aq_ret=%u", paq_port->PHY_ID.unit, paq_port->PHY_ID.port, aq_ret);
        return RT_ERR_FAILED;
    }

    if (AQ_RET_OK != (aq_ret = AQ_API_RestartAutonegotiation(paq_port)))
    {
        PHY_AQR_ERR("U%u P%u AQ_API_RestartAutonegotiation error aq_ret=%u", paq_port->PHY_ID.unit, paq_port->PHY_ID.port, aq_ret);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

int32
_aqApi_enable_set(AQ_API_Port *paq_port, rtk_enable_t enable)
{
    AQ_Retcode      aq_ret;

    if (enable == ENABLED)
    {
        if (AQ_RET_OK != (aq_ret = AQ_API_SetActive(paq_port)))
        {
            PHY_AQR_ERR("U%u P%u AQ_API_SetActive error aq_ret=%u", paq_port->PHY_ID.unit, paq_port->PHY_ID.port, aq_ret);
            return RT_ERR_FAILED;
        }
    }
    else
    {
        if (AQ_RET_OK != (aq_ret = AQ_API_SetLowPower(paq_port)))
        {
            PHY_AQR_ERR("U%u P%u AQ_API_SetLowPower error aq_ret=%u", paq_port->PHY_ID.unit, paq_port->PHY_ID.port, aq_ret);
            return RT_ERR_FAILED;
        }
    }

    return RT_ERR_OK;
}

int32
_aqApi_eeeEnable_get(AQ_API_Port *paq_port, rtk_enable_t *pEnable)
{
    AQ_Retcode  aq_ret;
    AQ_API_AutonegotiationControl   autoNegoCtrl;

    phy_osal_memset(&autoNegoCtrl, 0, sizeof(AQ_API_AutonegotiationControl));
    if (AQ_RET_OK != (aq_ret = AQ_API_GetAutonegotiationControl(paq_port, &autoNegoCtrl)))
    {
        PHY_AQR_ERR("U%u P%u AQ_API_GetAutonegotiationControl error aq_ret=%u", paq_port->PHY_ID.unit, paq_port->PHY_ID.port, aq_ret);
        return RT_ERR_FAILED;
    }

    *pEnable = (autoNegoCtrl.bzCapabilities._5G_EEE == True) ? ENABLED : DISABLED;

    return RT_ERR_OK;
}

int32
_aqApi_eeeEnable_set(AQ_API_Port *paq_port, rtk_enable_t enable)
{
    AQ_Retcode  aq_ret;
    AQ_API_AutonegotiationControl   autoNegoCtrl;
    AQ_boolean  aq_val;

    phy_osal_memset(&autoNegoCtrl, 0, sizeof(AQ_API_AutonegotiationControl));
    if (AQ_RET_OK != (aq_ret = AQ_API_GetAutonegotiationControl(paq_port, &autoNegoCtrl)))
    {
        PHY_AQR_ERR("U%u P%u AQ_API_GetAutonegotiationControl error aq_ret=%u", paq_port->PHY_ID.unit, paq_port->PHY_ID.port, aq_ret);
        return RT_ERR_FAILED;
    }

    switch (enable)
    {
      case ENABLED:
        aq_val = True;
        break;
      case DISABLED:
        aq_val = False;
        break;
      default:
        PHY_AQR_ERR("U%u P%u invalid input %d", paq_port->PHY_ID.unit, paq_port->PHY_ID.port, enable);
        return RT_ERR_INPUT;
    }

    autoNegoCtrl.bzCapabilities._5G_EEE   = aq_val;
    autoNegoCtrl.bzCapabilities._2_5G_EEE = aq_val;

    if (AQ_RET_OK != (aq_ret = AQ_API_SetAutonegotiationControl(paq_port, &autoNegoCtrl)))
    {
        PHY_AQR_ERR("U%u P%u AQ_API_SetAutonegotiationControl error aq_ret=%u", paq_port->PHY_ID.unit, paq_port->PHY_ID.port, aq_ret);
        return RT_ERR_FAILED;
    }

    if (AQ_RET_OK != (aq_ret = AQ_API_RestartAutonegotiation(paq_port)))
    {
        PHY_AQR_ERR("U%u P%u AQ_API_RestartAutonegotiation error aq_ret=%u", paq_port->PHY_ID.unit, paq_port->PHY_ID.port, aq_ret);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

int32
_aqApi_crossOverMode_get(AQ_API_Port *paq_port, rtk_port_crossOver_mode_t *pMode)
{
    AQ_Retcode  aq_ret;
    AQ_API_AutonegotiationControl   autoNegoCtrl;

    phy_osal_memset(&autoNegoCtrl, 0, sizeof(AQ_API_AutonegotiationControl));
    if (AQ_RET_OK != (aq_ret = AQ_API_GetAutonegotiationControl(paq_port, &autoNegoCtrl)))
    {
        PHY_AQR_ERR("U%u P%u AQ_API_GetAutonegotiationControl error aq_ret=%u", paq_port->PHY_ID.unit, paq_port->PHY_ID.port, aq_ret);
        return RT_ERR_FAILED;
    }

    switch (autoNegoCtrl.MDI_MDI_X_Control)
    {
      case AQ_API_MDC_Automatic:
        *pMode = PORT_CROSSOVER_MODE_AUTO;
        break;
      case AQ_API_MDC_MDI:
        *pMode = PORT_CROSSOVER_MODE_MDI;
        break;
      case AQ_API_MDC_MDI_X:
        *pMode = PORT_CROSSOVER_MODE_MDIX;
        break;
      default:
        PHY_AQR_ERR("U%u P%u AQ_API_GetAutonegotiationControl unknown config %d", paq_port->PHY_ID.unit, paq_port->PHY_ID.port, autoNegoCtrl.MDI_MDI_X_Control);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

int32
_aqApi_crossOverMode_set(AQ_API_Port *paq_port, rtk_port_crossOver_mode_t mode)
{
    AQ_Retcode  aq_ret;
    AQ_API_AutonegotiationControl   autoNegoCtrl;

    phy_osal_memset(&autoNegoCtrl, 0, sizeof(AQ_API_AutonegotiationControl));
    if (AQ_RET_OK != (aq_ret = AQ_API_GetAutonegotiationControl(paq_port, &autoNegoCtrl)))
    {
        PHY_AQR_ERR("U%u P%u AQ_API_GetAutonegotiationControl error aq_ret=%u", paq_port->PHY_ID.unit, paq_port->PHY_ID.port, aq_ret);
        return RT_ERR_FAILED;
    }

    switch (mode)
    {
      case PORT_CROSSOVER_MODE_AUTO:
        autoNegoCtrl.MDI_MDI_X_Control = AQ_API_MDC_Automatic;
        break;
      case PORT_CROSSOVER_MODE_MDI:
        autoNegoCtrl.MDI_MDI_X_Control = AQ_API_MDC_MDI;
        break;
      case PORT_CROSSOVER_MODE_MDIX:
        autoNegoCtrl.MDI_MDI_X_Control = AQ_API_MDC_MDI_X;
        break;
      default:
        return RT_ERR_INPUT;
    }

    if (AQ_RET_OK != (aq_ret = AQ_API_SetAutonegotiationControl(paq_port, &autoNegoCtrl)))
    {
        PHY_AQR_ERR("U%u P%u AQ_API_SetAutonegotiationControl error aq_ret=%u", paq_port->PHY_ID.unit, paq_port->PHY_ID.port, aq_ret);
        return RT_ERR_FAILED;
    }

    if (AQ_RET_OK != (aq_ret = AQ_API_RestartAutonegotiation(paq_port)))
    {
        PHY_AQR_ERR("U%u P%u AQ_API_RestartAutonegotiation error aq_ret=%u", paq_port->PHY_ID.unit, paq_port->PHY_ID.port, aq_ret);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

int32
_aqApi_crossOverStatus_get(AQ_API_Port *paq_port, rtk_port_crossOver_status_t *pStatus)
{
    AQ_Retcode  aq_ret;
    AQ_API_ConnectionStatus connStatus;

    phy_osal_memset(&connStatus, 0, sizeof(AQ_API_ConnectionStatus));
    if (AQ_RET_OK != (aq_ret = AQ_API_GetConnectionStatus(paq_port, &connStatus)))
    {
        PHY_AQR_ERR("U%u P%u AQ_API_GetConnectionStatus error aq_ret=%u", paq_port->PHY_ID.unit, paq_port->PHY_ID.port, aq_ret);
        return RT_ERR_FAILED;
    }

    switch(connStatus.MDI_MDI_X)
    {
      case AQ_API_MS_MDI:
        *pStatus = PORT_CROSSOVER_STATUS_MDI;
        break;
      case AQ_API_MS_MDI_X:
        *pStatus = PORT_CROSSOVER_STATUS_MDIX;
        break;
      case AQ_API_MS_None:
        PHY_AQR_ERR("U%u P%u AQ_API_GetConnectionStatus MDI_MDI_X %d not resolved", paq_port->PHY_ID.unit, paq_port->PHY_ID.port, connStatus.MDI_MDI_X);
        return RT_ERR_FAILED;
        break;
      default:
        PHY_AQR_ERR("U%u P%u AQ_API_GetConnectionStatus unknown MDI_MDI_X %d", paq_port->PHY_ID.unit, paq_port->PHY_ID.port, connStatus.MDI_MDI_X);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

int32
_aqApi_masterSlave_get(AQ_API_Port *paq_port, rtk_port_masterSlave_t *pMasterSlaveCfg, rtk_port_masterSlave_t *pMasterSlaveActual)
{
    AQ_Retcode  aq_ret;
    AQ_API_ConnectionStatus connStatus;
    AQ_API_AutonegotiationControl   autoNegoCtrl;

    phy_osal_memset(&autoNegoCtrl, 0, sizeof(AQ_API_AutonegotiationControl));
    if (AQ_RET_OK != (aq_ret = AQ_API_GetAutonegotiationControl(paq_port, &autoNegoCtrl)))
    {
        PHY_AQR_ERR("U%u P%u AQ_API_GetAutonegotiationControl error aq_ret=%u", paq_port->PHY_ID.unit, paq_port->PHY_ID.port, aq_ret);
        return RT_ERR_FAILED;
    }

    switch (autoNegoCtrl.masterSlaveControl)
    {
      case AQ_API_AN_Automatic:
        *pMasterSlaveCfg = PORT_AUTO_MODE;
        break;
      case AQ_API_AN_Slave:
        *pMasterSlaveCfg = PORT_SLAVE_MODE;
        break;
      case AQ_API_AN_Master:
        *pMasterSlaveCfg = PORT_MASTER_MODE;
        break;
      default:
        PHY_AQR_ERR("U%u P%u AQ_API_GetAutonegotiationControl unknown config %d", paq_port->PHY_ID.unit, paq_port->PHY_ID.port, autoNegoCtrl.masterSlaveControl);
        return RT_ERR_FAILED;
    }

    phy_osal_memset(&connStatus, 0, sizeof(AQ_API_ConnectionStatus));
    if (AQ_RET_OK != (aq_ret = AQ_API_GetConnectionStatus(paq_port, &connStatus)))
    {
        PHY_AQR_ERR("U%u P%u AQ_API_GetConnectionStatus error aq_ret=%u", paq_port->PHY_ID.unit, paq_port->PHY_ID.port, aq_ret);
        return RT_ERR_FAILED;
    }

    switch (connStatus.masterSlaveStatus)
    {
      case AQ_API_MSS_Master:
        *pMasterSlaveActual = PORT_MASTER_MODE;
        break;
      case AQ_API_MSS_Slave:
        *pMasterSlaveActual = PORT_SLAVE_MODE;
        break;
      default:
        PHY_AQR_ERR("U%u P%u AQ_API_GetConnectionStatus unknown config %d", paq_port->PHY_ID.unit, paq_port->PHY_ID.port, connStatus.masterSlaveStatus);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

int32
_aqApi_masterSlave_set(AQ_API_Port *paq_port, rtk_port_masterSlave_t masterSlave)
{
    AQ_Retcode  aq_ret;
    AQ_API_AutonegotiationControl   autoNegoCtrl;

    phy_osal_memset(&autoNegoCtrl, 0, sizeof(AQ_API_AutonegotiationControl));
    if (AQ_RET_OK != (aq_ret = AQ_API_GetAutonegotiationControl(paq_port, &autoNegoCtrl)))
    {
        PHY_AQR_ERR("U%u P%u AQ_API_GetAutonegotiationControl error aq_ret=%u", paq_port->PHY_ID.unit, paq_port->PHY_ID.port, aq_ret);
        return RT_ERR_FAILED;
    }

    switch (masterSlave)
    {
      case PORT_AUTO_MODE:
        autoNegoCtrl.masterSlaveControl = AQ_API_AN_Automatic;
        break;
      case PORT_SLAVE_MODE:
        autoNegoCtrl.masterSlaveControl = AQ_API_AN_Slave;
        break;
      case PORT_MASTER_MODE:
        autoNegoCtrl.masterSlaveControl = AQ_API_AN_Master;
        break;
      default:
        return RT_ERR_INPUT;
    }

    if (AQ_RET_OK != (aq_ret = AQ_API_SetAutonegotiationControl(paq_port, &autoNegoCtrl)))
    {
        PHY_AQR_ERR("U%u P%u AQ_API_SetAutonegotiationControl error aq_ret=%u", paq_port->PHY_ID.unit, paq_port->PHY_ID.port, aq_ret);
        return RT_ERR_FAILED;
    }

    if (AQ_RET_OK != (aq_ret = AQ_API_RestartAutonegotiation(paq_port)))
    {
        PHY_AQR_ERR("U%u P%u AQ_API_RestartAutonegotiation error aq_ret=%u", paq_port->PHY_ID.unit, paq_port->PHY_ID.port, aq_ret);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;

}

int32
_aqApi_loopback_get(AQ_API_Port *paq_port, rtk_enable_t *pEnable)
{
    AQ_Retcode  aq_ret;
    AQ_API_FWManagedLoopbackControl loopback;
    AQ_API_FWManagedLoopbackRate    rate;

    phy_osal_memset(&loopback, 0, sizeof(AQ_API_FWManagedLoopbackControl));
    phy_osal_memset(&rate, 0, sizeof(AQ_API_FWManagedLoopbackRate));
    if (AQ_RET_OK != (aq_ret = AQ_API_GetFWManagedLoopbackControl(paq_port, &loopback, &rate)))
    {
        PHY_AQR_ERR("U%u P%u AQ_API_GetFWManagedLoopbackControl error aq_ret=%u", paq_port->PHY_ID.unit, paq_port->PHY_ID.port, aq_ret);
        return RT_ERR_FAILED;
    }

    if (loopback == AQ_FWMLC_No_LB)
        *pEnable = DISABLED;
    else
        *pEnable = ENABLED;

    return RT_ERR_OK;
}

int32
_aqApi_loopback_set(AQ_API_Port *paq_port, rtk_enable_t enable)
{
    AQ_Retcode  aq_ret;
    AQ_API_FWManagedLoopbackControl loopback;
    AQ_API_FWManagedLoopbackRate    rate;

    phy_osal_memset(&loopback, 0, sizeof(AQ_API_FWManagedLoopbackControl));
    phy_osal_memset(&rate, 0, sizeof(AQ_API_FWManagedLoopbackRate));
    if (AQ_RET_OK != (aq_ret = AQ_API_GetFWManagedLoopbackControl(paq_port, &loopback, &rate)))
    {
        PHY_AQR_ERR("U%u P%u AQ_API_GetFWManagedLoopbackControl error aq_ret=%u", paq_port->PHY_ID.unit, paq_port->PHY_ID.port, aq_ret);
        return RT_ERR_FAILED;
    }

    switch (enable)
    {
      case DISABLED:
        loopback = AQ_FWMLC_No_LB;
        break;
      case ENABLED:
        //loopback = AQ_FWMLC_NetIntf_SysLB;
        loopback = AQ_FWMLC_CrossConnect_SysLB;
        break;
      default:
        return RT_ERR_INPUT;
    }

    rate = AQ_FWMLRate_10G;

    /* API Design Guide
     * Compatibility: 28nm PHYs should prefer to use AQ_API_GetFWManagedLoopbackControl and AQ_API_SetFWManagedLoopbackControl, rather than
     * AQ_API_GetLoopbackControl and AQ_API_SetLoopbackControl.
     */
    if (AQ_RET_OK != (aq_ret = AQ_API_SetFWManagedLoopbackControl(paq_port, loopback, rate)))
    {
        PHY_AQR_ERR("U%u P%u AQ_API_SetFWManagedLoopbackControl error aq_ret=%u", paq_port->PHY_ID.unit, paq_port->PHY_ID.port, aq_ret);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

void
_aqApi_basicCableDiagResults_to_rtkRtctChannelStatus(AQ_API_BasicPairResult *pairResult, rtk_rtctChannelStatus_t *channelStatus)
{
    switch (pairResult->status)
    {
      case AQ_API_BasicPairStatus_OK:
        break;
      case AQ_API_BasicPairStatus_Short:
        channelStatus->channelShort = TRUE;
        break;
      case AQ_API_BasicPairStatus_LowMismatch:
        channelStatus->channelLowMismatch = TRUE;
        break;
      case AQ_API_BasicPairStatus_HighMismatch:
        channelStatus->channelHighMismatch = TRUE;
        break;
      case AQ_API_BasicPairStatus_Open:
        channelStatus->channelOpen = TRUE;
        break;
      case AQ_API_BasicPairStatus_CrossoverA:
        channelStatus->channelCrossoverA = TRUE;
        break;
      case AQ_API_BasicPairStatus_CrossoverB:
        channelStatus->channelCrossoverB = TRUE;
        break;
      case AQ_API_BasicPairStatus_CrossoverC:
        channelStatus->channelCrossoverC = TRUE;
        break;
      case AQ_API_BasicPairStatus_CrossoverD:
        channelStatus->channelCrossoverD = TRUE;
        break;
      default:
        break;
    }
    channelStatus->channelLen = pairResult->reflection_1_Distance * 100;
}

int32
_aqApi_rtctResult_get(AQ_API_Port *paq_port, rtk_rtctResult_t *pRtctResult)
{
    AQ_Retcode  aq_ret;
    AQ_API_BasicCableDiagResults    results;

    phy_osal_memset(&results, 0, sizeof(AQ_API_BasicCableDiagResults));
    if (AQ_RET_OK != (aq_ret = AQ_API_GetBasicCableDiagsResults(paq_port, &results)))
    {
        PHY_AQR_ERR("U%u P%u AQ_API_GetFWManagedLoopbackControl error aq_ret=%u", paq_port->PHY_ID.unit, paq_port->PHY_ID.port, aq_ret);
        return RT_ERR_FAILED;
    }

    pRtctResult->linkType = PORT_SPEED_10G;
    _aqApi_basicCableDiagResults_to_rtkRtctChannelStatus(&results.pairAResult, &pRtctResult->un.channels_result.a);
    _aqApi_basicCableDiagResults_to_rtkRtctChannelStatus(&results.pairAResult, &pRtctResult->un.channels_result.b);
    _aqApi_basicCableDiagResults_to_rtkRtctChannelStatus(&results.pairAResult, &pRtctResult->un.channels_result.c);
    _aqApi_basicCableDiagResults_to_rtkRtctChannelStatus(&results.pairAResult, &pRtctResult->un.channels_result.d);
    return RT_ERR_OK;
}

int32
_aqApi_rtct_start(AQ_API_Port *paq_port)
{
    AQ_Retcode  aq_ret;

    if (AQ_RET_OK != (aq_ret = AQ_API_RunBasicCableDiags(paq_port)))
    {
        PHY_AQR_ERR("U%u P%u AQ_API_RunBasicCableDiags error aq_ret=%u", paq_port->PHY_ID.unit, paq_port->PHY_ID.port, aq_ret);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

int32
_aqApi_ieeeTestMode_set(AQ_API_Port *paq_port, rtk_port_phyTestMode_t *pTestMode)
{
    AQ_Retcode  aq_ret;
    AQ_API_10G_LineTestPattern      testPattern10G = AQ_API_10G_LTP_None;
    AQ_API_1G_LineTestPattern       testPattern1G = AQ_API_1G_LTP_None;
    AQ_API_100M_LineTestPattern     testPattern100M = AQ_API_100M_LTP_None;
    uint32                          testType;

    switch (pTestMode->mode)
    {
      case RTK_PORT_PHY_10G_TEST_MODE_NONE:
        testPattern10G = AQ_API_10G_LTP_None;
        testType = PORT_SPEED_10G;
        break;
      case RTK_PORT_PHY_10G_TEST_MODE1:
        testPattern10G = AQ_API_10G_LTP_10G_TestMode1;
        testType = PORT_SPEED_10G;
        break;
      case RTK_PORT_PHY_10G_TEST_MODE2:
        testPattern10G = AQ_API_10G_LTP_10G_TestMode2;
        testType = PORT_SPEED_10G;
        break;
      case RTK_PORT_PHY_10G_TEST_MODE3:
        testPattern10G = AQ_API_10G_LTP_10G_TestMode3;
        testType = PORT_SPEED_10G;
        break;
      case RTK_PORT_PHY_10G_TEST_MODE4_1:
        testPattern10G = AQ_API_10G_LTP_10G_TestMode4_1;
        testType = PORT_SPEED_10G;
        break;
      case RTK_PORT_PHY_10G_TEST_MODE4_2:
        testPattern10G = AQ_API_10G_LTP_10G_TestMode4_2;
        testType = PORT_SPEED_10G;
        break;
      case RTK_PORT_PHY_10G_TEST_MODE4_4:
        testPattern10G = AQ_API_10G_LTP_10G_TestMode4_4;
        testType = PORT_SPEED_10G;
        break;
      case RTK_PORT_PHY_10G_TEST_MODE4_5:
        testPattern10G = AQ_API_10G_LTP_10G_TestMode4_5;
        testType = PORT_SPEED_10G;
        break;
      case RTK_PORT_PHY_10G_TEST_MODE4_6:
        testPattern10G = AQ_API_10G_LTP_10G_TestMode4_6;
        testType = PORT_SPEED_10G;
        break;
      case RTK_PORT_PHY_10G_TEST_MODE5:
        testPattern10G = AQ_API_10G_LTP_10G_TestMode5;
        testType = PORT_SPEED_10G;
        break;
      case RTK_PORT_PHY_10G_TEST_MODE6:
        testPattern10G = AQ_API_10G_LTP_10G_TestMode6;
        testType = PORT_SPEED_10G;
        break;
      case RTK_PORT_PHY_10G_TEST_MODE7:
        testPattern10G = AQ_API_10G_LTP_10G_TestMode7;
        testType = PORT_SPEED_10G;
        break;
      case RTK_PORT_PHY_1G_TEST_MODE1:
        testPattern1G = AQ_API_1G_LTP_TestMode1;
        testType = PORT_SPEED_1000M;
        break;
      case RTK_PORT_PHY_1G_TEST_MODE2:
        testPattern1G = AQ_API_1G_LTP_TestMode2;
        testType = PORT_SPEED_1000M;
        break;
      case RTK_PORT_PHY_1G_TEST_MODE3:
        testPattern1G = AQ_API_1G_LTP_TestMode3;
        testType = PORT_SPEED_1000M;
        break;
      case RTK_PORT_PHY_1G_TEST_MODE4:
        testPattern1G = AQ_API_1G_LTP_TestMode4;
        testType = PORT_SPEED_1000M;
        break;
      case RTK_PORT_PHY_100M_TEST_MODE_NONE:
        testPattern10G = AQ_API_100M_LTP_None;
        testType = PORT_SPEED_100M;
        break;
      case RTK_PORT_PHY_100M_TEST_MODE_IEEE:
        testPattern100M = AQ_API_100M_LTP_IEEE_TestMode;
        testType = PORT_SPEED_100M;
        break;
      case RTK_PORT_PHY_100M_TEST_MODE_ANSI_JITTER:
        testPattern100M = AQ_API_100M_LTP_ANSI_Jitter_TestMode;
        testType = PORT_SPEED_100M;
        break;
      case RTK_PORT_PHY_100M_TEST_MODE_ANSI_DROOP:
        testPattern100M = AQ_API_100M_LTP_ANSI_Droop_TestMode;
        testType = PORT_SPEED_100M;
        break;
      default:
        return RT_ERR_PORT_NOT_SUPPORTED;
    }

    if (testType == PORT_SPEED_10G)
    {
        PHY_AQR_LOG("U%u P%u AQ_API_Set10G_LineTestPattern", paq_port->PHY_ID.unit, paq_port->PHY_ID.port);
        if (AQ_RET_OK != (aq_ret = AQ_API_Set10G_LineTestPattern(paq_port, testPattern10G)))
        {
            PHY_AQR_ERR("U%u P%u AQ_API_Set10G_LineTestPattern error aq_ret=%u", paq_port->PHY_ID.unit, paq_port->PHY_ID.port, aq_ret);
            return RT_ERR_FAILED;
        }
    }
    else if (testType == PORT_SPEED_1000M)
    {
        PHY_AQR_LOG("U%u P%u AQ_API_Set1G_LineTestPattern", paq_port->PHY_ID.unit, paq_port->PHY_ID.port);
        if (AQ_RET_OK != (aq_ret = AQ_API_Set1G_LineTestPattern(paq_port, testPattern1G)))
        {
            PHY_AQR_ERR("U%u P%u AQ_API_Set1G_LineTestPattern error aq_ret=%u", paq_port->PHY_ID.unit, paq_port->PHY_ID.port, aq_ret);
            return RT_ERR_FAILED;
        }
    }
    else if (testType == PORT_SPEED_100M)
    {
        PHY_AQR_LOG("U%u P%u AQ_API_Set100M_LineTestPattern", paq_port->PHY_ID.unit, paq_port->PHY_ID.port);
        if (AQ_RET_OK != (aq_ret = AQ_API_Set100M_LineTestPattern(paq_port, testPattern100M)))
        {
            PHY_AQR_ERR("U%u P%u AQ_API_Set100M_LineTestPattern error aq_ret=%u", paq_port->PHY_ID.unit, paq_port->PHY_ID.port, aq_ret);
            return RT_ERR_FAILED;
        }
    }
    else
    {
        PHY_AQR_ERR("U%u P%u unknown test type %u", paq_port->PHY_ID.unit, paq_port->PHY_ID.port, testType);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}


#ifdef __KERNEL__
#include <linux/fs.h>
#include <asm/segment.h>
#include <asm/uaccess.h>
#include <linux/buffer_head.h>

#include <linux/path.h>
#include <linux/namei.h>
#include <linux/fs.h>

int32
_kfile_sizeGet(char *filepath, uint32 *fileSize)
{
    struct path p;
    struct kstat ks;
    int ret;

    *fileSize = 0;
    if ((ret = kern_path(filepath, 0, &p)) != 0)
    {
        PHY_AQR_ERR("kern_path err %d", ret);
        return -1;
    }
    vfs_getattr(&p, &ks);

    *fileSize = (uint32)ks.size;
    return 0;
}

struct file *
_kfile_open(const char *path, int flags, int rights)
{
    struct file *filp = NULL;
    mm_segment_t oldfs;
    int err = 0;

    PHY_AQR_LOG("file open %s", path);
    oldfs = get_fs();
    set_fs(get_ds());
    filp = filp_open(path, flags, rights);
    set_fs(oldfs);
    if (IS_ERR(filp)) {
        err = PTR_ERR(filp);
        return NULL;
    }
    return filp;
}

int
_kfile_read(struct file *file, unsigned long long offset, unsigned char *data, unsigned int size)
{
    mm_segment_t oldfs;
    int ret;

    oldfs = get_fs();
    set_fs(get_ds());
    ret = vfs_read(file, data, size, &offset);
    set_fs(oldfs);
    return ret;
}

int
_kfile_write(struct file *file, unsigned long long offset, unsigned char *data, unsigned int size)
{
    mm_segment_t oldfs;
    int ret;

    oldfs = get_fs();
    set_fs(get_ds());

    ret = vfs_write(file, data, size, &offset);

    set_fs(oldfs);
    return ret;
}

void
_kfile_close(struct file *file)
{
    filp_close(file, NULL);
}
#endif


#if !defined(__BOOTLOADER__)
void
_aqApi_SerdesRxEyeDiagram(AQ_API_Port *paq_port, uint8 lane)
{
#ifdef __KERNEL__
    struct file *f;
    unsigned long long  offset = 0;
    char    buff[AQ_API_SERDESEYE_STRBUFF_SIZE];
    int32   len;
#else
    FILE    *f;
#endif
    char    eyeDiagFileName[AQ_API_SERDESEYE_FILENAME_LEN];
    uint8_t   x, y;
    uint16_t  numErrors = 0, maxErrors = 0;
    AQ_Retcode  resultCode;

#if defined(__KERNEL__) && defined(PHY_CUST_SERDESEYE_KERNEL_PRINT_RAWDATA)
    phy_osal_snprintf(eyeDiagFileName, AQ_API_SERDESEYE_FILENAME_LEN, "eyediag_p%ul%u.raw", paq_port->PHY_ID.port, (uint32)lane);
#else
    phy_osal_snprintf(eyeDiagFileName, AQ_API_SERDESEYE_FILENAME_LEN, "eyediag_p%ul%u.csv", paq_port->PHY_ID.port, (uint32)lane);
#endif

#ifdef __KERNEL__
    f = _kfile_open(eyeDiagFileName, (O_RDWR|O_CREAT), 0644);
#else
    f = fopen(eyeDiagFileName, "w");
#endif
    if (f == NULL)
    {
        phy_osal_printf("U%u P%u open file %s faile\n", paq_port->PHY_ID.unit, paq_port->PHY_ID.port, eyeDiagFileName);
        return;
    }

    phy_osal_printf("U%u P%u Lane %u SERDESRxEyeDiagram file %s\n", paq_port->PHY_ID.unit, paq_port->PHY_ID.port, (uint32)lane, eyeDiagFileName);
    resultCode = AQ_API_BeginSERDESRxEyeDiagram(paq_port, lane);
    if (resultCode == AQ_RET_OK)
    {
        for (y = 0; y < AQ_API_SERDESEYE_GRIDSIZE_Y; y += 1)
        {
            for (x = 0; x < AQ_API_SERDESEYE_GRIDSIZE_X; x += 1)
            {
                resultCode = AQ_API_GetSERDESRxEyeMeasurement(paq_port, lane, x, y, &numErrors, &maxErrors);
                if (resultCode == AQ_RET_OK)
                {
                    #ifdef __KERNEL__
                      #ifdef PHY_CUST_SERDESEYE_KERNEL_PRINT_RAWDATA
                        /* for kernel that does not support float */
                        len = sprintf(buff, "%u %u,", numErrors, maxErrors);
                      #else
                        len = sprintf(buff, "%f,", (double)numErrors / (double)maxErrors);
                      #endif
                        offset += _kfile_write(f, offset, buff, len);
                    #else
                        fprintf(f, "%f,", (double)numErrors / (double)maxErrors);
                    #endif
                }
                else
                {
                    phy_osal_printf("U%u P%u Lane %u AQ_API_GetSERDESRxEyeMeasurement X=%u Y=%u resultCode: %i\n", paq_port->PHY_ID.unit, paq_port->PHY_ID.port, (uint32)lane, x, y, resultCode);
                    #if __KERNEL__
                        len = sprintf(buff, "%s", "error,");
                        offset += _kfile_write(f, offset, buff, len);
                    #else
                        fprintf(f, "error,");
                    #endif
                }
            }
            #if __KERNEL__
                sprintf(buff, "%s", "\n");
                offset += _kfile_write(f, offset, buff, 1);
            #else
                fprintf(f, "\n");
            #endif
        }
    }
    else
    {
        phy_osal_printf("U%u P%u Lane %u AQ_API_BeginSERDESRxEyeDiagram resultCode: %i\n", paq_port->PHY_ID.unit, paq_port->PHY_ID.port, (uint32)lane, resultCode);
    }
    resultCode = AQ_API_EndSERDESRxEyeDiagram(paq_port, lane);
    if (resultCode != AQ_RET_OK)
    {
        phy_osal_printf("U%u P%u Lane %u AQ_API_EndSERDESRxEyeDiagram resultCode: %i\n", paq_port->PHY_ID.unit, paq_port->PHY_ID.port, (uint32)lane, resultCode);
    }

    #if __KERNEL__
        _kfile_close(f);
    #else
        fclose(f);
    #endif
    return;
}
#endif/* #if !defined(__BOOTLOADER__) */

#if !defined(__BOOTLOADER__)
int32
_aqApi_portEyeMonitor_start(AQ_API_Port *paq_port)
{
    hal_control_t   *pHalCtrl;
    uint32      pollEnable = DISABLED;
    uint8       lane;

    if ((pHalCtrl = hal_ctrlInfo_get(paq_port->PHY_ID.unit)) == NULL)
        return RT_ERR_FAILED;

    MACDRV(pHalCtrl)->fMdrv_miim_pollingEnable_get(paq_port->PHY_ID.unit, paq_port->PHY_ID.port, &pollEnable);
    MACDRV(pHalCtrl)->fMdrv_miim_pollingEnable_set(paq_port->PHY_ID.unit, paq_port->PHY_ID.port, DISABLED);

    for (lane = 0; lane <= 3; lane++)
    {
        phy_osal_printf("U%u P%u Lane %u SERDESRxEyeDiagram\n", paq_port->PHY_ID.unit, paq_port->PHY_ID.port, (uint32)lane);
        _aqApi_SerdesRxEyeDiagram(paq_port, lane);
    }

    MACDRV(pHalCtrl)->fMdrv_miim_pollingEnable_set(paq_port->PHY_ID.unit, paq_port->PHY_ID.port, pollEnable);
    return RT_ERR_OK;
}
#endif/* #if !defined(__BOOTLOADER__) */


#if defined(PHY_CUST_BOOTLOAD_ENABLE)
#if defined(CONFIG_SDK_KERNEL_LINUX)
#ifdef __KERNEL__
int32
_aqApi_bootLoadImage_getk(char *fileName, uint8_t **buff, int32 buffSize, int32 *actualSize)
{
    int32       ret, readSize;
    uint32 file_length = 0;
    struct file *pFile = NULL;

    PHY_AQR_LOG("bootLoadImage_get %s", fileName);
    *actualSize = 0;
    if (_kfile_sizeGet(fileName, &file_length) < 0)
    {
        PHY_AQR_ERR("bootLoadImage_get file size get failed");
        ret = RT_ERR_FAILED;
        goto EXIT;
    }

    if (file_length > buffSize)
    {
        PHY_AQR_ERR("bootLoadImage_get file size %u exceed buff size %u", file_length, buffSize);
        ret = RT_ERR_FAILED;
        goto EXIT;
    }

    PHY_AQR_LOG("bootLoadImage_get file size %u", file_length);
    if ((pFile = _kfile_open(fileName, (O_RDWR), 0644)) == NULL)
    {
        PHY_AQR_ERR("bootLoadImage_get file open failed");
        ret = RT_ERR_FAILED;
        goto EXIT;
    }

    readSize = _kfile_read(pFile, 0, *buff, file_length);
    if (readSize != file_length)
    {
        PHY_AQR_ERR("bootLoadImage_get readSize %d not match file_len %u", readSize, file_length);
        ret = RT_ERR_FAILED;
        goto EXIT;
    }

    *actualSize = file_length;
    ret = RT_ERR_OK;

  EXIT:
    if (pFile != NULL)
    {
        _kfile_close(pFile);
    }

    return ret;

}
#else
int32
_aqApi_bootLoadImage_getu(char *fileName, uint8_t **buff, int32 buffSize, int32 *actualSize)
{
    int32   ret;
    uint32 file_length = 0;
    FILE *pFile = NULL;

    PHY_AQR_LOG("bootLoadImage_get %s", fileName);

    *actualSize = 0;
    pFile = fopen(fileName, "rb" );
    if (pFile == NULL)
    {
        PHY_AQR_ERR("bootLoadImage_get open fail");
        return RT_ERR_FAILED;
    }

    if(fseek(pFile, 0, SEEK_END))
    {
        PHY_AQR_ERR("bootLoadImage_get Seek fail");
        ret = RT_ERR_FAILED;
        goto EXIT;
    }

    file_length = ftell(pFile);
    if (file_length > buffSize)
    {
        PHY_AQR_ERR("bootLoadImage_get file size %u exceed buff size %u", file_length, buffSize);
        ret = RT_ERR_FAILED;
        goto EXIT;
    }
    PHY_AQR_LOG("bootLoadImage_get file size %u", file_length);

    fseek(pFile, 0, SEEK_SET);
    fread(*buff, file_length, sizeof(uint8), pFile);
    *actualSize = file_length;
    ret = RT_ERR_OK;

  EXIT:
    if (pFile != NULL)
    {
        fclose(pFile);
    }

    return ret;
}
#endif /* __KERNEL__ */
#endif /* CONFIG_SDK_KERNEL_LINUX */

int32
_aqApi_bootLoadImage_get(char *fileName, uint8_t **buff, int32 buffSize, int32 *actualSize)
{
#if defined(CONFIG_SDK_KERNEL_LINUX)
  #ifdef __KERNEL__
    return _aqApi_bootLoadImage_getk(fileName, buff, buffSize, actualSize);
  #else
    return _aqApi_bootLoadImage_getu(fileName, buff, buffSize, actualSize);
  #endif
#else
    return RT_ERR_FAILED;
#endif
}
#endif /* PHY_CUST_BOOTLOAD_ENABLE */

#if defined(PHY_CUST_BOOTLOAD_ENABLE)
void
_aqApi_bootLoadImage_write(AQ_API_Port *paq_port, int32 phyPortNum)
{
    uint8_t  *image = NULL;
    uint32_t imageSize = 0;
#ifdef PROV_TABLE
    uint8_t     *provTableImage = NULL;
    uint32_t    provTableImageSize = 0;
#endif
    unsigned int provisioningAddresses[1] = {0};
    uint8_t     gangload_MDIO_address;
    AQ_API_Port* targetPorts[1];
    AQ_API_Port broadcastPort;
    AQ_Retcode aq_ret;
    AQ_Retcode resultCodes[4];

    PHY_AQR_LOG("U%u P%u bootLoadImage_write", paq_port->PHY_ID.unit, paq_port->PHY_ID.port);

    image = (uint8*)phy_osal_alloc(RTK_PORT_FLASHIMG_SIZE_MAX * sizeof(uint8));
    if (image == NULL)
    {
        PHY_AQR_ERR("U%u P%u bootLoadImage_write: no memory for image %u.", paq_port->PHY_ID.unit, paq_port->PHY_ID.port, (RTK_PORT_FLASHIMG_SIZE_MAX * sizeof(uint8)));
        goto EXIT;
    }

#ifdef PROV_TABLE
    provTableImage = (uint8*)phy_osal_alloc(RTK_PORT_FLASHIMG_SIZE_MAX * sizeof(uint8));
    if (provTableImage == NULL)
    {
        PHY_AQR_ERR("U%u P%u bootLoadImage_write: no memory for provTableImage %u.", paq_port->PHY_ID.unit, paq_port->PHY_ID.port, (RTK_PORT_FLASHIMG_SIZE_MAX * sizeof(uint8)));
        goto EXIT;
    }
#endif

    if (_aqApi_bootLoadImage_get(PHY_CUST_PHY_IMAGE_FILE, &image, RTK_PORT_FLASHIMG_SIZE_MAX, &imageSize) != RT_ERR_OK)
    {
        goto EXIT;
    }

#ifdef PROV_TABLE
    if (_aqApi_bootLoadImage_get(PHY_CUST_PHY_PROV_TABLE_FILE, &provTableImage, RTK_PORT_FLASHIMG_SIZE_MAX, &provTableImageSize) != RT_ERR_OK)
    {
        goto EXIT;
    }
#endif

    gangload_MDIO_address = HWP_PHY_ADDR(paq_port->PHY_ID.unit, paq_port->PHY_ID.port);
    provisioningAddresses[0] = HWP_PHY_ADDR(paq_port->PHY_ID.unit, paq_port->PHY_ID.port);
    targetPorts[0] = paq_port;
    phy_osal_memcpy(&broadcastPort, targetPorts[0], sizeof(AQ_API_Port));
#ifdef PROV_TABLE
    aq_ret = AQ_API_WriteBootLoadImageWithProvTable(targetPorts, 1,
            provisioningAddresses, resultCodes, &imageSize, image, gangload_MDIO_address,
            &broadcastPort, &provTableImageSize, provTableImage);
#else
    aq_ret = AQ_API_WriteBootLoadImage(targetPorts, 1, provisioningAddresses,
            resultCodes, &imageSize, image, gangload_MDIO_address, &broadcastPort);
#endif

    if (aq_ret != AQ_RET_OK)
    {
        PHY_AQR_ERR("U%u P%u AQ_API_WriteBootLoadImage error aq_ret=%u", paq_port->PHY_ID.unit, paq_port->PHY_ID.port, aq_ret);
    }

    if (resultCodes[0] != AQ_RET_OK)
    {
        PHY_AQR_ERR("U%u P%u AQ_API_WriteBootLoadImage resultCodes[0]=%u", paq_port->PHY_ID.unit, paq_port->PHY_ID.port, resultCodes[0]);
    }

  EXIT:
    if (image)
    {
        phy_osal_free(image);
    }
#ifdef PROV_TABLE
    if (provTableImage)
    {
        phy_osal_free(provTableImage);
    }
#endif
    return;
}
#endif /* PHY_CUST_BOOTLOAD_ENABLE */

#if defined(PHY_CUST_BOOTLOAD_ENABLE)
void
_aqApi_bootLoadImage_load(AQ_API_Port *paq_port, int32 phyPortNum)
{
    AQ_Retcode  aq_ret;
    AQ_API_Status       status;

    PHY_AQR_LOG("U%u P%u bootLoadImage_check", paq_port->PHY_ID.unit, paq_port->PHY_ID.port);
    phy_osal_memset(&status, 0, sizeof(AQ_API_Status));
    if (AQ_RET_OK != (aq_ret = AQ_API_GetStatus(paq_port, &status)))
    {
        PHY_AQR_ERR("U%u P%u AQ_API_GetStatus error aq_ret=%u", paq_port->PHY_ID.unit, paq_port->PHY_ID.port, aq_ret);
        return;
    }

    /* check if the image was loaded */
    if (status.loadedImageInformation.programmed == True)
    {
        PHY_AQR_LOG("image was loaded: programmed=%u imageID_String=%s. Skip init load.", status.loadedImageInformation.programmed, status.loadedImageInformation.imageID_String);
        return;
    }

    /* load image */
    _aqApi_bootLoadImage_write(paq_port, phyPortNum);

    /* get image information */
    phy_osal_memset(&status, 0, sizeof(AQ_API_Status));
    if (AQ_RET_OK != (aq_ret = AQ_API_GetStatus(paq_port, &status)))
    {
        PHY_AQR_ERR("U%u P%u AQ_API_GetStatus error aq_ret=%u", paq_port->PHY_ID.unit, paq_port->PHY_ID.port, aq_ret);
        return;
    }

    if (status.loadedImageInformation.programmed != True)
    {
        PHY_AQR_ERR("U%u P%u image-programmed is False", paq_port->PHY_ID.unit, paq_port->PHY_ID.port);
        phy_osal_printf("    unit %u port %u PHY image load failed", paq_port->PHY_ID.unit, paq_port->PHY_ID.port);
    }
    else
    {
        PHY_AQR_LOG("programmed=%u imageID_String=%s", status.loadedImageInformation.programmed, status.loadedImageInformation.imageID_String);
        phy_osal_printf("    unit %u port %u PHY image loaded", paq_port->PHY_ID.unit, paq_port->PHY_ID.port);
    }

    return;
}
#endif /* PHY_CUST_BOOTLOAD_ENABLE */


/* Function Name:
 *      phy_aqr407_media_get
 * Description:
 *      Get PHY media type.
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      pMedia - pointer buffer of phy media type
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - invalid parameter
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      None
 */
int32
phy_aqr407_media_get(uint32 unit, rtk_port_t port, rtk_port_media_t *pMedia)
{
    *pMedia = PORT_MEDIA_COPPER;
    return RT_ERR_OK;
}


/* Function Name:
 *      phy_aqr407_autoNegoEnable_get
 * Description:
 *      Get autonegotiation enable status of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_aqr407_autoNegoEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32           ret;
    AQ_API_Port     aq_port;

    _phy_aqr407_rtkPort_to_aqPort(unit, port, &aq_port);

    ret = _aqApi_autoNegoEnable_get(&aq_port, pEnable);

    return ret;
}

/* Function Name:
 *      phy_aqr407_autoNegoEnable_set
 * Description:
 *      Set autonegotiation enable status of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_aqr407_autoNegoEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    AQ_API_Port     aq_port;

    PHY_AQR_LOG("U%u P%u cust1 autoNegoEnable_set", unit, port);

    _phy_aqr407_rtkPort_to_aqPort(unit, port, &aq_port);

    return _aqApi_autoNegoEnable_set(&aq_port, enable);
}

/* Function Name:
 *      phy_aqr407_autoNegoAbility_get
 * Description:
 *      Get ability advertisement for auto negotiation of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pAbility - pointer to PHY auto negotiation ability
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_aqr407_autoNegoAbility_get(uint32 unit, rtk_port_t port, rtk_port_phy_ability_t *pAbility)
{
    AQ_API_Port     aq_port;

    _phy_aqr407_rtkPort_to_aqPort(unit, port, &aq_port);

    return _aqApi_autoNegoAbility_get(&aq_port, pAbility);
}

/* Function Name:
 *      phy_aqr407_autoNegoAbility_set
 * Description:
 *      Set ability advertisement for auto negotiation of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 *      pAbility  - auto negotiation ability that is going to set to PHY
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_aqr407_autoNegoAbility_set(uint32 unit, rtk_port_t port, rtk_port_phy_ability_t *pAbility)
{
    AQ_API_Port     aq_port;

    _phy_aqr407_rtkPort_to_aqPort(unit, port, &aq_port);

    PHY_AQR_LOG("U%u P%u cust1 autoNegoAbility_set", unit, port);

    return _aqApi_autoNegoAbility_set(&aq_port, pAbility);
}

/* Function Name:
 *      phy_aqr407_duplex_get
 * Description:
 *      Set duplex mode status of the specific port
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      duplex        - duplex mode of the port, full or half
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_aqr407_duplex_get(uint32 unit, rtk_port_t port, uint32 *pDuplex)
{
    AQ_API_Port aq_port;

    _phy_aqr407_rtkPort_to_aqPort(unit, port, &aq_port);

    return _aqApi_duplex_get(&aq_port, pDuplex);
}

/* Function Name:
 *      phy_aqr407_duplex_set
 * Description:
 *      Set duplex mode status of the specific port
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      duplex        - duplex mode of the port, full or half
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_aqr407_duplex_set(uint32 unit, rtk_port_t port, uint32 duplex)
{
    AQ_API_Port aq_port;

    _phy_aqr407_rtkPort_to_aqPort(unit, port, &aq_port);

    return _aqApi_duplex_set(&aq_port, duplex);
}

/* Function Name:
 *      phy_aqr407_speed_get
 * Description:
 *      Get link speed status of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pSpeed - pointer to PHY link speed
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_aqr407_speed_get(uint32 unit, rtk_port_t port, uint32 *pSpeed)
{
    AQ_API_Port aq_port;

    _phy_aqr407_rtkPort_to_aqPort(unit, port, &aq_port);

    return _aqApi_speed_get(&aq_port, pSpeed);
}

/* Function Name:
 *      phy_aqr407_speed_set
 * Description:
 *      Set speed mode status of the specific port
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      speed         - link speed status 100
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - invalid parameter
 * Note:
 *      None
 */
int32
phy_aqr407_speed_set(uint32 unit, rtk_port_t port, uint32 speed)
{
    AQ_API_Port aq_port;

    _phy_aqr407_rtkPort_to_aqPort(unit, port, &aq_port);

    return _aqApi_speed_set(&aq_port, speed);
}

/* Function Name:
 *      phy_aqr407_enable_set
 * Description:
 *      Set PHY interface status of the specific port
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      enable        - admin configuration of PHY interface
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_aqr407_enable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    AQ_API_Port     aq_port;

    PHY_AQR_LOG("U%u P%u enable:%d", unit, port, enable);

    _phy_aqr407_rtkPort_to_aqPort(unit, port, &aq_port);

    return _aqApi_enable_set(&aq_port, enable);
}


/* Function Name:
 *      phy_aqr407_eeeEnable_get
 * Description:
 *      Get enable status of EEE function in the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to enable status of EEE
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
phy_aqr407_eeeEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    AQ_API_Port     aq_port;

    _phy_aqr407_rtkPort_to_aqPort(unit, port, &aq_port);

    return _aqApi_eeeEnable_get(&aq_port, pEnable);
}


/* Function Name:
 *      phy_aqr407_eeeEnable_set
 * Description:
 *      Set enable status of EEE function in the specified port.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - enable status of EEE
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
phy_aqr407_eeeEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    AQ_API_Port     aq_port;

    PHY_AQR_LOG("U%u P%u enable:%d", unit, port, enable);

    _phy_aqr407_rtkPort_to_aqPort(unit, port, &aq_port);

    return _aqApi_eeeEnable_set(&aq_port, enable);
}



/* Function Name:
 *      phy_aqr407_crossOverMode_get
 * Description:
 *      Get cross over mode in the specified port.
 * Input:
 *      unit  - unit id
 *      port  - port id
 * Output:
 *      pMode - pointer to cross over mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Following value is valid
 *      - PORT_CROSSOVER_MODE_AUTO
 *      - PORT_CROSSOVER_MODE_MDI
 *      - PORT_CROSSOVER_MODE_MDIX
 */
int32
phy_aqr407_crossOverMode_get(uint32 unit, rtk_port_t port, rtk_port_crossOver_mode_t *pMode)
{
    AQ_API_Port aq_port;

    _phy_aqr407_rtkPort_to_aqPort(unit, port, &aq_port);

    return _aqApi_crossOverMode_get(&aq_port, pMode);
}


/* Function Name:
 *      phy_aqr407_crossOverMode_set
 * Description:
 *      Set cross over mode in the specified port.
 * Input:
 *      unit - unit id
 *      port - port id
 *      mode - cross over mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 *      RT_ERR_PORT_ID - invalid port id
 *      RT_ERR_INPUT   - invalid input parameter
 * Note:
 *      Following value is valid
 *      - PORT_CROSSOVER_MODE_AUTO
 *      - PORT_CROSSOVER_MODE_MDI
 *      - PORT_CROSSOVER_MODE_MDIX
 */
int32
phy_aqr407_crossOverMode_set(uint32 unit, rtk_port_t port, rtk_port_crossOver_mode_t mode)
{
    AQ_API_Port     aq_port;

    PHY_AQR_LOG("U%u P%u crossOverMode:%d", unit, port, mode);

    _phy_aqr407_rtkPort_to_aqPort(unit, port, &aq_port);

    return _aqApi_crossOverMode_set(&aq_port, mode);
}

/* Function Name:
 *      phy_aqr407_crossOverStatus_get
 * Description:
 *      Get cross over status in the specified port.
 * Input:
 *      unit  - unit id
 *      port  - port id
 * Output:
 *      pMode - pointer to cross over mode status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_PHY_FIBER_LINKUP - This feature is not supported in this mode
 * Note:
 *      Following value is valid
 *      - PORT_CROSSOVER_STATUS_MDI
 *      - PORT_CROSSOVER_STATUS_MDIX
 */
int32
phy_aqr407_crossOverStatus_get(uint32 unit, rtk_port_t port, rtk_port_crossOver_status_t *pStatus)
{
    AQ_API_Port aq_port;

    _phy_aqr407_rtkPort_to_aqPort(unit, port, &aq_port);

    return _aqApi_crossOverStatus_get(&aq_port, pStatus);
}


/* Function Name:
 *      phy_aqr407_reg_mmd_set
 * Description:
 *      Get PHY registers.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      mmdAddr - mmd device address
 *      mmdReg  - mmd reg id
 * Output:
 *      pData   - pointer buffer of read data
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      None
 */
int32
phy_aqr407_reg_mmd_get(uint32 unit, rtk_port_t port, uint32 mmdAddr, uint32 mmdReg, uint32 *pData)
{
    int32       ret;

    if ((ret = hal_miim_mmd_read(unit, port, mmdAddr, mmdReg, pData)) != RT_ERR_OK)
    {
        PHY_AQR_ERR("U%u P%u mmd 0x%X reg 0x%X get fail %x", unit, port, mmdAddr, mmdReg, ret);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_common_reg_mmd_set
 * Description:
 *      Set PHY registers.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      mmdAddr - mmd device address
 *      mmdReg  - mmd reg id
 *      data    - write data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      None
 */
int32
phy_aqr407_reg_mmd_set(uint32 unit, rtk_port_t port, uint32 mmdAddr, uint32 mmdReg, uint32 data)
{
    int32       ret;

    if ((ret = hal_miim_mmd_write(unit, port, mmdAddr, mmdReg, data)) != RT_ERR_OK)
    {
        PHY_AQR_ERR("U%u P%u mmd 0x%X reg 0x%X set fail %x", unit, port, mmdAddr, mmdReg, ret);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      phy_aqr407_masterSlave_get
 * Description:
 *      Get PHY configuration of master/slave mode of the specific port
 * Input:
 *      unit                - unit id
 *      port                - port id
 * Output:
 *      pMasterSlaveCfg     - pointer to the PHY master slave configuration
 *      pMasterSlaveActual  - pointer to the PHY master slave actual link status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      This function only works on giga/ 10g port to get its master/slave mode configuration.
 */
int32
phy_aqr407_masterSlave_get(
    uint32              unit,
    rtk_port_t          port,
    rtk_port_masterSlave_t   *pMasterSlaveCfg,
    rtk_port_masterSlave_t   *pMasterSlaveActual)
{
    AQ_API_Port     aq_port;

    //PHY_AQR_LOG("U%u P%u ", unit, port);

    _phy_aqr407_rtkPort_to_aqPort(unit, port, &aq_port);

    return _aqApi_masterSlave_get(&aq_port, pMasterSlaveCfg, pMasterSlaveActual);

}

/* Function Name:
 *      phy_aqr407_masterSlave_set
 * Description:
 *      Set PHY configuration of master/slave mode of the specific port
 * Input:
 *      unit                - unit id
 *      port                - port id
 *      masterSlave         - PHY master slave configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_aqr407_masterSlave_set(
    uint32              unit,
    rtk_port_t          port,
    rtk_port_masterSlave_t   masterSlave)
{
    AQ_API_Port     aq_port;

    PHY_AQR_LOG("U%u P%u masterSlave:%d", unit, port, masterSlave);

    _phy_aqr407_rtkPort_to_aqPort(unit, port, &aq_port);

    return _aqApi_masterSlave_set(&aq_port, masterSlave);

}

/* Function Name:
 *      phy_aqr407_loopback_get
 * Description:
 *      Get PHY Loopback mode of the specific port
 * Input:
 *      unit                - unit id
 *      port                - port id
 *      pEnable           -loopback mode status;
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_aqr407_loopback_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    AQ_API_Port     aq_port;

    _phy_aqr407_rtkPort_to_aqPort(unit, port, &aq_port);

    return _aqApi_loopback_get(&aq_port, pEnable);
}

/* Function Name:
 *      phy_aqr407_loopback_set
 * Description:
 *      Set PHY Loopback mode of the specific port
 * Input:
 *      unit                - unit id
 *      port                - port id
 *      enable              - ENABLED: Enable loopback;
 *                            DISABLED: Disable loopback. PHY back to normal operation.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_aqr407_loopback_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    AQ_API_Port     aq_port;

    PHY_AQR_LOG("U%u P%u enable:%d", unit, port, enable);

    _phy_aqr407_rtkPort_to_aqPort(unit, port, &aq_port);

    return _aqApi_loopback_set(&aq_port, enable);
}


/* Function Name:
 *      phy_aqr407_rtctResult_get
 * Description:
 *      Get test result of RTCT.
 * Input:
 *      unit        - unit id
 *      port        - the port for retriving RTCT test result
 * Output:
 *      pRtctResult - RTCT result
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_PHY_RTCT_NOT_FINISH   - RTCT not finish. Need to wait a while.
 *      RT_ERR_TIMEOUT      - RTCT test timeout in this port.
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The result unit is cm
 */
int32
phy_aqr407_rtctResult_get(uint32 unit, rtk_port_t port, rtk_rtctResult_t *pRtctResult)
{
    AQ_API_Port     aq_port;

    PHY_AQR_LOG("U%u P%u rtctResult_get", unit, port);

    _phy_aqr407_rtkPort_to_aqPort(unit, port, &aq_port);

    return _aqApi_rtctResult_get(&aq_port, pRtctResult);
}

/* Function Name:
 *      phy_aqr407_rtct_start
 * Description:
 *      Start PHY interface RTCT test of the specific port
 * Input:
 *      unit          - unit id
 *      port          - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 *      RT_ERR_CHIP_NOT_SUPPORTED - chip not supported
 * Note:
 *      None
 */
int32
phy_aqr407_rtct_start(uint32 unit, rtk_port_t port)
{
    AQ_API_Port     aq_port;

    PHY_AQR_LOG("U%u P%u rtctResult_get", unit, port);

    _phy_aqr407_rtkPort_to_aqPort(unit, port, &aq_port);

    return _aqApi_rtct_start(&aq_port);
}


/* Function Name:
 *      phy_aqr407_ieeeTestMode_set
 * Description:
 *      Set test mode for PHY transmitter test
 * Input:
 *      unit  - unit id
 *      port  - port id
 *      pTestMode->mode  - test mode RTK_PORT_PHY_1G_TEST_MODEx which is defined in IEEE 40.6.1.1.2 for 1G.
 *                         RTK_PORT_PHY_10G_TEST_MODEx is for 10G test.
 *                         RTK_PORT_PHY_100M_TEST_MODE_xx is for 100M test.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_NOT_ALLOWED - The operation is not allowed
 *      RT_ERR_PORT_NOT_SUPPORTED - test mode is not supported
 * Note:
 *      RTK_PORT_PHYTESTMODE_FLAG_ALL_PHY_PORTS is not supported
 */
int32
phy_aqr407_ieeeTestMode_set(uint32 unit, rtk_port_t port, rtk_port_phyTestMode_t *pTestMode)
{
    AQ_API_Port     aq_port;

    PHY_AQR_LOG("U%u P%u ieeeTestMode_set", unit, port);

    _phy_aqr407_rtkPort_to_aqPort(unit, port, &aq_port);

    return _aqApi_ieeeTestMode_set(&aq_port, pTestMode);
}

#if !defined(__BOOTLOADER__)
int32
phy_aqr407_portEyeMonitor_start(uint32 unit, rtk_port_t port, uint32 sdsId, uint32 frameNum)
{
    AQ_API_Port     aq_port;

    PHY_AQR_LOG("U%u P%u portEyeMonitor_start", unit, port);

    _phy_aqr407_rtkPort_to_aqPort(unit, port, &aq_port);

    return _aqApi_portEyeMonitor_start(&aq_port);
}
#endif

#if !defined(__BOOTLOADER__)
int32
phy_aqr407_polar_get(uint32 unit, rtk_port_t port, rtk_port_phyPolarCtrl_t *pPolarCtrl)
{
    AQ_API_Port     aq_port;

    _phy_aqr407_rtkPort_to_aqPort(unit, port, &aq_port);

    //FOR DEBUG
    _aqApi_dump(&aq_port);

    return RT_ERR_PORT_NOT_SUPPORTED;
}
#endif

/* Function Name:
 *      phy_aqr407_init
 * Description:
 *      Initialize PHY.
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_aqr407_init(uint32 unit, rtk_port_t port)
{
    AQ_API_Port     aq_port;

    PHY_AQR_LOG("U%u P%u init", unit, port);

    _phy_aqr407_rtkPort_to_aqPort(unit, port, &aq_port);

#if defined(PHY_CUST_BOOTLOAD_ENABLE)
    _aqApi_bootLoadImage_load(&aq_port, HWP_PHY_BASE_PHY_MAX(unit, port));
#endif /* PHY_CUST_BOOTLOAD_ENABLE */


    return RT_ERR_OK;
}

/* Writes the full image to the flash. Reads image back and verifies */
AQ_Retcode AQ_API_BlockWriteAndVerifyFlashImageOfKnownFLASH
(
	AQ_API_Port* port,
	AQ_API_FLASH flashType,
	const uint32_t imageSize,
	const uint8_t* image,
	uint32_t blockSize,
	uint32_t blockRetryCount
)
{
	AQ_API_Variable(AQ_GlobalNvrInterface)
	AQ_API_DeclareLocalStruct(AQ_GlobalControl, globalControl)
	uint32_t i, j;
	uint32_t addressPointer, programSize;
	uint32_t writtenSize;
	AQ_Retcode retval;
	/* FLASH parameters and command words for use with the PHY's FLASH
	* interface registers */
	AQ_API_FlashParams flashParams;
	uint8_t* blockReadBack;

	/*
	* Determine FLASH parameters and instructions
	*/
	if (AQ_API_GetFlashParametersForKnownFLASH(
		port,
		flashType,
		&flashParams) == AQ_RET_FLASH_TYPE_BAD) return AQ_RET_FLASH_TYPE_BAD;

	/* check to make sure the image size minus the 2 CRC bytes is not greater than the size of the FLASH */
	if (imageSize > flashParams.flashSize) return AQ_RET_FLASH_IMAGE_TOO_LARGE;


	/*
	*  Erase the FLASH
	*/
	retval = AQ_API_ChipErase(port, &flashParams);
	if (retval != AQ_RET_OK)
		return retval;

#ifdef AQ_VERBOSE
	phy_osal_printf("Bytes in file 0x%X\n", imageSize);
#endif

	addressPointer = 0;
	blockReadBack = (uint8_t*)(phy_osal_alloc(blockSize * sizeof(uint8_t)));
	while (addressPointer < imageSize)
	{
#ifdef AQ_VERBOSE
		phy_osal_printf("address pointer               0x%X\n", addressPointer);
#endif
		if (imageSize - addressPointer < blockSize)
			programSize = imageSize - addressPointer;
		else
			programSize = blockSize;

		retval = AQ_RET_FLASH_IMAGE_MISMATCH;
		for (i = 0; (retval != AQ_RET_OK) && (i < blockRetryCount); ++i)
		{
			/*
			*  Write the FLASH
			*/

			retval = AQ_API_ProgramKnownFLASH(port, &flashParams, NULL, addressPointer, programSize, image + addressPointer, &writtenSize);
			if (retval != AQ_RET_OK)
			{
				phy_osal_free(blockReadBack);
				return retval;
			}

			/*
			* Verify the written data. If the verification fails, return the
			* failure code.
			*/
			retval = AQ_API_ReadFlashImageOfKnownFLASH(port, flashType, addressPointer, programSize, blockReadBack, &writtenSize, False);
			for (j = 0; j < programSize; ++j)
			{
				if (blockReadBack[j] != image[addressPointer + j])
				{
					phy_osal_printf("  Mismatch on byte 0x%X: Read 0x%X - Should be: 0x%X\n", (addressPointer + j), blockReadBack[j], image[addressPointer + j]);
					retval = AQ_RET_FLASH_IMAGE_MISMATCH;
				}
			}
		}

		if (retval != AQ_RET_OK)
		{
			phy_osal_free(blockReadBack);
			return retval;
		}
		/*Workaround for CRC check failed.*/


		addressPointer += programSize;
	}
	phy_osal_free(blockReadBack);

	/* reset the processor to force a reload */
	AQ_API_AssignWordOfLocalStruct(globalControl, 1, 0x0000);
	/* REGDOC: Assign to local representation of bitfield (APPIA/HHD/EUR: 1E.C001.6) */
	AQ_API_AssignBitfieldOfLocalStruct_DeviceRestricted(APPIA_HHD_EUR, AQ_GlobalControl, globalControl, upRunStallOverride, 1);
	/* REGDOC: Assign to local representation of bitfield (HHD/APPIA/EUR: 1E.C001.0) */
	AQ_API_AssignBitfieldOfLocalStruct(AQ_GlobalControl, globalControl, upRunStall, 1);
	/* REGDOC: Write register (HHD/APPIA/EUR: 1E.C000 + 1) */
	AQ_API_WriteRegister(port->PHY_ID, AQ_GlobalControl, 1,
		AQ_API_WordOfLocalStruct(globalControl, 1));
	/* REGDOC: Assign to local representation of bitfield (HHD/APPIA/EUR: 1E.C001.F) */
	AQ_API_AssignBitfieldOfLocalStruct(AQ_GlobalControl, globalControl, upReset, 1);
	/* REGDOC: Write register (HHD/APPIA/EUR: 1E.C000 + 1) */
	AQ_API_WriteRegister(port->PHY_ID, AQ_GlobalControl, 1,
		AQ_API_WordOfLocalStruct(globalControl, 1));

	/* wait 100us:  given a 20 MHz MDIO with preamble suppression and a preset address pointer, one write delays
	a minimum of 32 * (1/20 MHz) = 1.6 us = ~64 writes.  Use the global scratchpad as it does nothing */
	for (i = 0; i < 64; i++)
		/* REGDOC: Write register (HHD/APPIA/EUR: 1E.0300) */
		AQ_API_WriteRegister(port->PHY_ID, AQ_GlobalMicroprocessorScratchPad, 0, 0x0000);

	/* REGDOC: Assign to local representation of bitfield (HHD/APPIA/EUR: 1E.C001.F) */
	AQ_API_AssignBitfieldOfLocalStruct(AQ_GlobalControl, globalControl, upReset, 0);
	/* REGDOC: Assign to local representation of bitfield (HHD/APPIA/EUR: 1E.C001.0) */
	AQ_API_AssignBitfieldOfLocalStruct(AQ_GlobalControl, globalControl, upRunStall, 0);
	/* REGDOC: Write register (HHD/APPIA/EUR: 1E.C000 + 1) */
	AQ_API_WriteRegister(port->PHY_ID, AQ_GlobalControl, 1,
		AQ_API_WordOfLocalStruct(globalControl, 1));

	return AQ_RET_OK;
}

/* Function Name:
 *      phy_cust1_imageFlash_load
 * Description:
 *      load Aquantia PHY image to flash
 * Input:
 *      unit - unit id
 *      port - port id
 *      size - image size
 *      image - image
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_cust1_imageFlash_load(uint32 unit, rtk_port_t port, uint32 size, uint8 *image)
{
    AQ_API_Port aq_port;
    uint32 ret = RT_ERR_OK;
    static uint32 complete_size = 0;
    static uint8 *buff = NULL;

    if (size > RTK_PORT_FLASHIMG_SIZE_MAX)
    {
        phy_osal_printf("Image is too large to load\n");
        return RT_ERR_FAILED;
    }

    if (buff == NULL)
    {
        buff = (uint8*)phy_osal_alloc(RTK_PORT_FLASHIMG_SIZE_MAX * sizeof(uint8));
        if (buff == NULL)
        {
            phy_osal_printf("Failed to alloc memory\n");
            return RT_ERR_FAILED;
        }
    }

    if ((complete_size + RTK_PORT_FLASHIMG_SIZE) > RTK_PORT_FLASHIMG_SIZE_MAX)
    {
      phy_osal_free(buff);
      return RT_ERR_FAILED;
    }


    /*Need to transfer many times.*/
    if ((complete_size + RTK_PORT_FLASHIMG_SIZE) < size)
    {
        phy_osal_memcpy((buff+complete_size), image, sizeof(uint8) * RTK_PORT_FLASHIMG_SIZE);
        complete_size += RTK_PORT_FLASHIMG_SIZE;
        return RT_ERR_OK;
    }
    else
    {
        phy_osal_memcpy((buff + complete_size), image, sizeof(uint8) * (size - complete_size));
        _phy_aqr407_rtkPort_to_aqPort(unit, port, &aq_port);
        complete_size += (size - complete_size);
        ret = AQ_API_BlockWriteAndVerifyFlashImageOfKnownFLASH(&aq_port, AQ_API_F_AT45DB041E, size, buff, 256, 50);
        //ret = AQ_API_WriteAndVerifyFlashImage(&aq_port, size, buff);
        phy_osal_free(buff);
        complete_size = 0;
        buff = NULL;
    }
    return ret;
}

/* Function Name:
 *      phy_cust1drv_mapperInit
 * Description:
 *      Initialize PHY driver.
 * Input:
 *      pPhydrv - pointer of phy driver
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void
phy_cust1drv_mapperInit(rt_phydrv_t *pPhydrv)
{
    pPhydrv->phydrv_index = RT_PHYDRV_CUST1;
    pPhydrv->fPhydrv_init = phy_aqr407_init;
    pPhydrv->fPhydrv_media_get = phy_aqr407_media_get;
    pPhydrv->fPhydrv_autoNegoEnable_get = phy_aqr407_autoNegoEnable_get;
    pPhydrv->fPhydrv_autoNegoEnable_set = phy_aqr407_autoNegoEnable_set;
    pPhydrv->fPhydrv_autoNegoAbility_get = phy_aqr407_autoNegoAbility_get;
    pPhydrv->fPhydrv_autoNegoAbility_set = phy_aqr407_autoNegoAbility_set;
    pPhydrv->fPhydrv_duplex_get = phy_aqr407_duplex_get;
    pPhydrv->fPhydrv_duplex_set = phy_aqr407_duplex_set;
    pPhydrv->fPhydrv_speed_get = phy_aqr407_speed_get;
    pPhydrv->fPhydrv_speed_set = phy_aqr407_speed_set;
    pPhydrv->fPhydrv_enable_set = phy_aqr407_enable_set;

    pPhydrv->fPhydrv_reg_mmd_get = phy_aqr407_reg_mmd_get;
    pPhydrv->fPhydrv_reg_mmd_set = phy_aqr407_reg_mmd_set;
    pPhydrv->fPhydrv_portimageFlash_load = phy_cust1_imageFlash_load;

#if !defined(__BOOTLOADER__)
    pPhydrv->fPhydrv_eeeEnable_get = phy_aqr407_eeeEnable_get;
    pPhydrv->fPhydrv_eeeEnable_set = phy_aqr407_eeeEnable_set;
    pPhydrv->fPhydrv_crossOverMode_get = phy_aqr407_crossOverMode_get;
    pPhydrv->fPhydrv_crossOverMode_set = phy_aqr407_crossOverMode_set;
    pPhydrv->fPhydrv_crossOverStatus_get = phy_aqr407_crossOverStatus_get;
    pPhydrv->fPhydrv_masterSlave_get = phy_aqr407_masterSlave_get;
    pPhydrv->fPhydrv_masterSlave_set = phy_aqr407_masterSlave_set;
    pPhydrv->fPhydrv_loopback_get = phy_aqr407_loopback_get;
    pPhydrv->fPhydrv_loopback_set = phy_aqr407_loopback_set;
    pPhydrv->fPhydrv_rtctResult_get = phy_aqr407_rtctResult_get;
    pPhydrv->fPhydrv_rtct_start =  phy_aqr407_rtct_start;
    pPhydrv->fPhydrv_ieeeTestMode_set = phy_aqr407_ieeeTestMode_set;
    pPhydrv->fPhydrv_portEyeMonitor_start = phy_aqr407_portEyeMonitor_start;
    pPhydrv->fPhydrv_polar_get = phy_aqr407_polar_get;
    pPhydrv->fPhydrv_macIntfSerdesMode_get = phy_cust1_macIntfSerdesMode_get;
#endif
}


/*! Provides generic synchronous PHY register write functionality. It is the
 * responsibility of the system designer to provide the specific MDIO address
 * pointer updates, etc. in order to accomplish this write operation.
 * It will be assumed that the write has been completed by the time this
 * function returns.*/
void AQ_API_MDIO_Write (
  AQ_Port PHY_ID,
  unsigned int MMD,
  unsigned int address,
  unsigned int data
)
{
    hal_control_t   *pHalCtrl;
    int32           ret;
    uint32          unit = PHY_ID.unit, port = PHY_ID.port;

    PHY_AQR_LOG("unit=%d, port=%d %u.%u data=%x", unit, port, MMD, address, data);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
    {
        PHY_AQR_ERR("U%u P%u pHalCtrl get fail", unit, port);
        return;
    }

    if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_mmd_write(unit, port, MMD, address, data)) != RT_ERR_OK)
    {
        PHY_AQR_LOG("U%u P%u mmd 0x%X reg 0x%X write fail %x", unit, port, MMD, address, ret);
    }

    return;
}


/*! Provides generic synchronous PHY register read functionality. It is the
 * responsibility of the system designer to provide the specific MDIO address
 * pointer updates, etc. in order to accomplish this read operation.*/
unsigned int AQ_API_MDIO_Read (
  AQ_Port PHY_ID,
  unsigned int MMD,
  unsigned int address
)
{
    hal_control_t   *pHalCtrl;
    uint32          data = 0;
    int32           ret;
    uint32          unit = PHY_ID.unit, port = PHY_ID.port;



    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
    {
        PHY_AQR_ERR("U%u P%u pHalCtrl get fail", unit, port);
        return 0;
    }

    if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_mmd_read(unit, port, MMD, address, &data)) != RT_ERR_OK)
    {
        PHY_AQR_ERR("U%u P%u mmd 0x%X reg 0x%X read fail %x", unit, port, MMD, address, ret);
        return 0;
    }

    //PHY_AQR_LOG("unit=%d, port=%d %u.%u data=%x", unit, port, MMD, address, data);

    return data;
}


/*! Returns after at least milliseconds have elapsed.  This must be implemented
 * in a platform-approriate way. AQ_API functions will call this function to
 * block for the specified period of time. If necessary, PHY register reads
 * may be performed on port to busy-wait. */
void AQ_API_Wait (
  uint32_t milliseconds,
  AQ_API_Port* port
)
{
    phy_osal_time_mdelay(milliseconds);
    return;
}

