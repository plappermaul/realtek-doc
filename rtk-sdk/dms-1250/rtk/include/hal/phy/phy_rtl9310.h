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
 * $Revision$
 * $Date$
 *
 * Purpose : MAC 9310 internal PHY Driver APIs.
 *
 * Feature : MAC 9310 internal PHY Driver APIs
 *
 */

#ifndef __HAL_PHY_PHY_RTL9310_H__
#define __HAL_PHY_PHY_RTL9310_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/sds.h>

/*
 * Symbol Definition
 */
#define PORT_NUM_IN_9310SDS     (1)

/*
 * Data Declaration
 */
extern rt_phyInfo_t phy_rtl9310sds_info;

typedef enum phy_rtl9310_dfeType_e
{
    PHY_RTL9310_DFE_VTH,
    PHY_RTL9310_DFE_TAP0,
    PHY_RTL9310_DFE_TAP1EVEN,
    PHY_RTL9310_DFE_TAP1ODD,
    PHY_RTL9310_DFE_TAP2EVEN,
    PHY_RTL9310_DFE_TAP2ODD,
    PHY_RTL9310_DFE_TAP3EVEN,
    PHY_RTL9310_DFE_TAP3ODD,
    PHY_RTL9310_DFE_TAP4EVEN,
    PHY_RTL9310_DFE_TAP4ODD,
    PHY_RTL9310_DFE_FGCAL_OFST,
    PHY_RTL9310_DFE_END,
} phy_rtl9310_dfeType_t;

typedef struct phy_rtl9310_dfe_s
{
    uint32                  coefNum;
    uint32                  endBit, startBit;
    uint32                  signBit;
    int32                   val;
    phy_rtl9310_dfeType_t   type;
    char                    name[16];
} phy_rtl9310_dfe_t;

/*
 * Function Declaration
 */

/* Function Name:
 *      phy_rtl9310_linkSts_get
 * Description:
 *      Get the SerDes link status.
 * Input:
 *      unit      - unit id
 *      sds       - user SerDes id
 * Output:
 *      info      - link status information
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
phy_rtl9310_linkSts_get(uint32 unit, uint32 sds, rtk_sds_linkSts_t *info);

/* Function Name:
 *      phy_rtl9310_rx_rst
 * Description:
 *      Reset Serdes Rx and original patch are kept.
 * Input:
 *      unit    - unit id
 *      sds     - user serdes id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_OUT_OF_RANGE - Serdes index is not support.
 * Note:
 *      None
 */
extern int32
phy_rtl9310_rx_rst(uint32  unit, uint32 sds);

/* Function Name:
 *      phy_rtl9310_sds_rst
 * Description:
 *      Reset Serdes.
 * Input:
 *      unit    - unit id
 *      sds     - user serdes id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_OUT_OF_RANGE - Serdes index is not support.
 * Note:
 *      None
 */
extern int32
phy_rtl9310_sds_rst(uint32  unit, uint32 sds);

/* Function Name:
 *      phy_rtl9310_sds_mode_set
 * Description:
 *      Set 9310 serdes mode.
 * Input:
 *      unit     - unit id
 *      sdsId   - serdes id
 *      mode   - serdes mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_INPUT
 *      RT_ERR_CHIP_NOT_SUPPORTED
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
phy_rtl9310_sds_mode_set(uint32 unit, uint32 sdsId, rt_serdesMode_t mode);

/* Function Name:
 *      phy_rtl9310_leq_get
 * Description:
 *      Get SerDes LEQ
 * Input:
 *      unit - unit id
 *      sds  - user SerDes id
 * Output:
 *      pLeq - LEQ configuration
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      No semaphore
 */
extern int32
phy_rtl9310_leq_get(uint32 unit, uint32 sds, rtk_sds_leq_t *pLeq);

/* Function Name:
 *      phy_rtl9310_leq_set
 * Description:
 *      Set SerDes LEQ
 * Input:
 *      unit - unit id
 *      sds  - user SerDes id
 *      pLeq  - LEQ configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      No semaphore
 */
extern int32
phy_rtl9310_leq_set(uint32 unit, uint32 sds, rtk_sds_leq_t *pLeq);

/* Function Name:
 *      phy_rtl9310_dfe_set
 * Description:
 *      Set SerDes DFE
 * Input:
 *      unit - unit id
 *      sds  - analog SerDes id
 *      type - DFE type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      None
 */
extern int32
phy_rtl9310_dfe_set(uint32 unit, uint32 sds, phy_rtl9310_dfeType_t type, int32 val);

/* Function Name:
 *      phy_rtl9310_rxCali
 * Description:
 *      SerDes Rx calibration
 * Input:
 *      unit - unit id
 *      sds  - SerDes id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      None
 */
extern int32
phy_rtl9310_rxCali(uint32 unit, uint32 sds);

/* Function Name:
 *      phy_rtl9310_serdes_ge_mapperInit
 * Description:
 *      Initialize PHY 9310 serdes.
 * Input:
 *      pPhydrv - pointer of phy driver
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void phy_rtl9310_serdes_mapperInit(rt_phydrv_t *pPhydrv);

/* Function Name:
 *      phy_rtl9310_sds_mode_get
 * Description:
 *      Get 9310 serdes current mode.
 * Input:
 *      unit     - unit id
 *      sdsId   - serdes id
 *      mode   - serdes mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_INPUT
 *      RT_ERR_CHIP_NOT_SUPPORTED
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32 phy_rtl9310_sds_mode_get(uint32 unit, uint32 sdsId, rt_serdesMode_t *mode);

/* Function Name:
 *      phy_rtl9310_sds_mode_set
 * Description:
 *      Set 9310 serdes mode.
 * Input:
 *      unit     - unit id
 *      sdsId   - serdes id
 *      mode   - serdes mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_INPUT
 *      RT_ERR_CHIP_NOT_SUPPORTED
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32  phy_rtl9310_sds_mode_set(uint32 unit, uint32 sdsId, rt_serdesMode_t mode);

/* Function Name:
 *      phy_rtl9310_symErr_clear
 * Description:
 *      Clear the SerDes symbol error count.
 * Input:
 *      unit      - unit id
 *      sds       - user SerDes id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
phy_rtl9310_symErr_clear(uint32 unit, uint32 sds);

/* Function Name:
 *      phy_rtl9310_symErr_get
 * Description:
 *      Get the SerDes symbol error count.
 * Input:
 *      unit      - unit id
 *      sds       - user SerDes id
 * Output:
 *      info      - symbol error count information
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
phy_rtl9310_symErr_get(uint32 unit, uint32 sds, rtk_sds_symErr_t *info);

/* Function Name:
 *      phy_rtl9310_10gmedia_get
 * Description:
 *      Get 9310 10 serdes PHY media type.
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
extern int32
phy_rtl9310_10gmedia_get(uint32 unit, rtk_port_t port, rtk_port_10gMedia_t *pMedia);

/* Function Name:
 *      phy_rtl9310_10gMedia_set
 * Description:
 *
 * Input:
 *      unit - unit id
 *      port - port id
 *      media
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_INPUT
 *      RT_ERR_CHIP_NOT_SUPPORTED
 *      RT_ERR_TIMEOUT
 *      RT_ERR_FAILED
 * Note:
 *      None
 */

extern int32
phy_rtl9310_10gMedia_set(uint32 unit, rtk_port_t port, rtk_port_10gMedia_t media);

/* Function Name:
 *      phy_rtl9310_pcb_adapt
 * Description:
 *      SerDes PCB adapt
 * Input:
 *      unit - unit id
 *      sds  - SerDes id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      None
 */
extern int32
phy_rtl9310_pcb_adapt(uint32 unit, uint32 sds);

/* Function Name:
 *      phy_rtl9310_sdsRxCaliEnable_get
 * Description:
 *      Get the SerDes rx cali enable status.
 * Input:
 *      unit      - unit id
 *      sds       - SerDes id
 * Output:
 *      pEnable      - pointer to the sds enable status of rx calibration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
phy_rtl9310_sdsRxCaliEnable_get(uint32 unit, uint32 sds, rtk_enable_t *pEnable);

/* Function Name:
 *      phy_rtl9310_sdsRxCaliEnable_set
 * Description:
 *      Set the SerDes rx cali status.
 * Input:
 *      unit      - unit id
 *      sds       - SerDes id
 *      enable    - status of rx calibration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 * Note:
 *      None
 */
extern int32
phy_rtl9310_sdsRxCaliEnable_set(uint32 unit, uint32 sds, rtk_enable_t enable);

/* Function Name:
 *      phy_rtl9310_sdsRxCaliDbgEnable_set
 * Description:
 *      Set debug msg status for SerDes rx calibration
 * Input:
 *      enable      - enable print debug msg
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
phy_rtl9310_sdsRxCaliDbgEnable_set(rtk_enable_t enable);

/* Function Name:
 *      phy_rtl9310_sdsRxCaliConf_get
 * Description:
 *      Get the SerDes rx calibration configration.
 * Input:
 *      unit      - unit id
 *      sds       - SerDes id
 * Output:
 *      pConf      - pointer to the sds rx calibration configuration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
phy_rtl9310_sdsRxCaliConf_get(uint32 unit, uint32 sds, rtk_sds_rxCaliConf_t *pConf);

/* Function Name:
 *      phy_rtl9310_sdsRxCaliConf_set
 * Description:
 *      Config the SerDes rx calibration.
 * Input:
 *      unit      - unit id
 *      sds       - SerDes id
 *      conf      - rx calibration conf
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 * Note:
 *      None
 */
extern int32
phy_rtl9310_sdsRxCaliConf_set(uint32 unit, uint32 sds, rtk_sds_rxCaliConf_t conf);

/* Function Name:
 *      phy_rtl9310_sdsTxParam_get
 * Description:
 *      Get SerDes Tx parameter
 * Input:
 *      unit - unit id
 *      sds  - SerDes id
 * Output:
 *      pParam - eye parameter
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      None
 */
extern int32
phy_rtl9310_sdsTxParam_get(uint32 unit, uint32 sds, rtk_sds_eyeParam_t *pParam);

/* Function Name:
 *      phy_rtl9310_sdsTxParam_set
 * Description:
 *      Set SerDes Tx parameters
 * Input:
 *      unit - unit id
 *      sds  - SerDes id
 *      param  - eye parameter value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      None
 */
extern int32
phy_rtl9310_sdsTxParam_set(uint32 unit, uint32 sds, rtk_sds_eyeParam_t param);

/* Function Name:
 *      phy_rtl9310_sdsCustConfig_init
 * Description:
 *      Serdes Customer Configuration
 * Input:
 *      unit - unit id
 *      sds  - SerDes id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
phy_rtl9310_sdsCustConfig_init(uint32 unit, uint32 sds, rtk_port_10gMedia_t media);

extern int32
phy_rtl9310_init_leq_dfe(uint32 unit, uint32 sds);

/* Function Name:
 *      phy_rtl9310_sdsRxCaliStatus_get
 * Description:
 *      Get the SerDes rx calibration status.
 * Input:
 *      unit      - unit id
 *      sds       - SerDes id
 * Output:
 *      pStatus      - pointer to  status of rx calibration
 * Return:
 *      RT_ERR_OK
 * Note:
 *      None
 */
extern int32
phy_rtl9310_sdsRxCaliStatus_get(uint32 unit, uint32 sds, rtk_port_phySdsRxCaliStatus_t *pStatus);

/* Function Name:
 *      phy_rtl9310_linkSts_chk
 * Description:
 *      Check the SerDes link status.
 * Input:
 *      unit      - unit id
 *      sds       - user SerDes id
 * Output:
 *      pSts      - check status information
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
phy_rtl9310_linkSts_chk(uint32 unit, uint32 sds, uint8 *pSts);

/* Function Name:
 *      phy_rtl9310_sdsInfo_get
 * Description:
 *      Get SerDes information
 * Input:
 *      unit - unit id
 *      sds  - SerDes id
 * Output:
 *      pInfo   - SerDes information
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      None
 */
extern int32
phy_rtl9310_sdsInfo_get(uint32 unit, uint32 sds, rtk_sds_info_t *pInfo);

/* Function Name:
 *      phy_rtl9310_sds_loopback_get
 * Description:
 *      Get serdes Loopback status of the specific port
 * Input:
 *      unit                - unit id
 *      port                - port id
 * Output:
 *      pEnable           -loopback mode status;
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
phy_rtl9310_sds_loopback_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      phy_rtl9310_sds_loopback_set
 * Description:
 *      Get serdes Loopback status of the specific port
 * Input:
 *      unit                - unit id
 *      port                - port id
 *      enable           -loopback mode status;
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
phy_rtl9310_sds_loopback_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      phy_rtl9310_remoteFault_handle
 * Description:
 *      Handle fiber remote fault
 * Input:
 *      unit      - unit id
 *      port      - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 * Note:
 *      None
 */
extern int32
phy_rtl9310_remoteFault_handle(uint32 unit, rtk_port_t port);

/* Function Name:
 *      phy_9310_linkChange_process
 * Description:
 *      Link change handling process
 * Input:
 *      unit          - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
extern int32
phy_9310_linkChange_process(uint32 unit, rtk_port_t port, rtk_port_linkStatus_t linkSts);

#endif /* __HAL_PHY_PHY_RTL9310_H__ */
