
#include <linux/phy.h>
#include <linux/ethtool.h>
#include <linux/ethtool_netlink.h>
#include "rtk_phylib.h"
#include "rtk_phy.h"
#include "rtk_phylib_rtl8224.h"
#include "rtk_phy_rtl8224.h"
#include "phy_rtl8224_patch.h"
#include "error.h"
#include "rtk_osal.h"

//#define RTL8224_INTERNAL_INFO                1

#ifdef RTL8224_INTERNAL_INFO
#define RTL8224_INTERNAL_PORT_CHECK(port)\
                            do{\
                                printk("\n[%s] addr(%d)\n",__FUNCTION__,port->mdio.addr);\
                            }while(0)
#else
#define RTL8224_INTERNAL_PORT_CHECK(port)\
                            do{\
                            }while(0)
#endif

static int _phy_8224_cable_test_report_trans(uint32 pair, rtk_rtctResult_t *result)
{
    switch(pair)
    {
        case 0:
            if(result->un.ge_result.channelAShort)
                return ETHTOOL_A_CABLE_RESULT_CODE_CROSS_SHORT;
            if(result->un.ge_result.channelAOpen)
                return ETHTOOL_A_CABLE_RESULT_CODE_OPEN;
            if(result->un.ge_result.channelAMismatch)
                return ETHTOOL_A_CABLE_RESULT_CODE_CROSS_SHORT;
            if(result->un.ge_result.channelAPairBusy)
                return ETHTOOL_A_CABLE_RESULT_CODE_SAME_SHORT;
            break;
        case 1:
            if(result->un.ge_result.channelBShort)
                return ETHTOOL_A_CABLE_RESULT_CODE_CROSS_SHORT;
            if(result->un.ge_result.channelBOpen)
                return ETHTOOL_A_CABLE_RESULT_CODE_OPEN;
            if(result->un.ge_result.channelBMismatch)
                return ETHTOOL_A_CABLE_RESULT_CODE_CROSS_SHORT;
            if(result->un.ge_result.channelBPairBusy)
                return ETHTOOL_A_CABLE_RESULT_CODE_SAME_SHORT;
            break;
        case 2:
            if(result->un.ge_result.channelCShort)
                return ETHTOOL_A_CABLE_RESULT_CODE_CROSS_SHORT;
            if(result->un.ge_result.channelCOpen)
                return ETHTOOL_A_CABLE_RESULT_CODE_OPEN;
            if(result->un.ge_result.channelCMismatch)
                return ETHTOOL_A_CABLE_RESULT_CODE_CROSS_SHORT;
            if(result->un.ge_result.channelCPairBusy)
                return ETHTOOL_A_CABLE_RESULT_CODE_SAME_SHORT;
            break;
        case 3:
            if(result->un.ge_result.channelDShort)
                return ETHTOOL_A_CABLE_RESULT_CODE_CROSS_SHORT;
            if(result->un.ge_result.channelDOpen)
                return ETHTOOL_A_CABLE_RESULT_CODE_OPEN;
            if(result->un.ge_result.channelDMismatch)
                return ETHTOOL_A_CABLE_RESULT_CODE_CROSS_SHORT;
            if(result->un.ge_result.channelDPairBusy)
                return ETHTOOL_A_CABLE_RESULT_CODE_SAME_SHORT;

            break;
        default:
            return ETHTOOL_A_CABLE_RESULT_CODE_UNSPEC;

    }
    return ETHTOOL_A_CABLE_RESULT_CODE_OK;
}

static int _phy_8224_cable_test_length_get(uint32 pair, rtk_rtctResult_t *result, uint32 *length_cm)
{
    switch(pair)
    {
        case 0:
            if(result->un.ge_result.channelAShort)
                return -1;
            if(result->un.ge_result.channelAOpen)
                return -1;
            if(result->un.ge_result.channelAMismatch)
                return -1;
            if(result->un.ge_result.channelAPairBusy)
                return -1;
            *length_cm = result->un.ge_result.channelALen;
            break;
        case 1:
            if(result->un.ge_result.channelBShort)
                return -1;
            if(result->un.ge_result.channelBOpen)
                return -1;
            if(result->un.ge_result.channelBMismatch)
                return -1;
            if(result->un.ge_result.channelBPairBusy)
                return -1;
            *length_cm = result->un.ge_result.channelBLen;
            break;
        case 2:
            if(result->un.ge_result.channelCShort)
                return -1;
            if(result->un.ge_result.channelCOpen)
                return -1;
            if(result->un.ge_result.channelCMismatch)
                return -1;
            if(result->un.ge_result.channelCPairBusy)
                return -1;
            *length_cm = result->un.ge_result.channelCLen;
            break;
        case 3:
            if(result->un.ge_result.channelDShort)
                return -1;
            if(result->un.ge_result.channelDOpen)
                return -1;
            if(result->un.ge_result.channelDMismatch)
                return -1;
            if(result->un.ge_result.channelDPairBusy)
                return -1;
            *length_cm = result->un.ge_result.channelDLen;
            break;
        default:
            return -1;

    }
    return 0;
}

static int _phy_8224_dbCompleted_check(struct phy_device *phydev)
{
    struct rtl8224_priv_info *priv = (struct rtl8224_priv_info *)phydev->priv;
    struct phy_device *base_phydev = NULL;
    struct rtl8224_priv_info *base_priv = NULL;
    struct phy_device *tmp_phydev = NULL;
    struct rtl8224_priv_info *tmp_priv = NULL;
    int phy_addr, loop, ret = 0;


    base_phydev = priv->basePort;
    base_priv = (struct rtl8224_priv_info *)base_phydev->priv;
    phy_addr = base_phydev->mdio.addr;

    for(loop = 0; loop < RTL8224_PORT_NUM; loop++)
    {
        tmp_phydev = mdiobus_get_phy(base_phydev->mdio.bus, (phy_addr + loop));
        tmp_priv = (struct rtl8224_priv_info *)tmp_phydev->priv;

        if((loop == 0) && (tmp_phydev != base_phydev))
        {
            PR_ERR("[%d][loop %d] base port is WRONG!!!",__LINE__,loop);
            ret = -1;
            goto db_not_ready;
        }

        if(tmp_phydev != base_priv->memberPort[loop])
        {
            PR_ERR("[%d][loop %d] member port is WRONG!!!",__LINE__,loop);
            ret = -2;
            goto db_not_ready;
        }

        if(tmp_priv->port_offset != loop)
        {
            PR_ERR("[%d][loop %d] port offset is WRONG!!!",__LINE__,loop);
            ret = -3;
            goto db_not_ready;
        }

        if((tmp_priv->is_basePort != 0) && (loop != 0))
        {
            PR_ERR("[%d][loop %d] is_basePort is WRONG!!!",__LINE__,loop);
            ret = -4;
            goto db_not_ready;
        }

    }

    return ret;
db_not_ready:
    PR_ERR("[%d][loop %d] dbCompleted is NOT Ready!!!",__LINE__,loop);

    return ret;
}

/* updated base port db*/
static struct phy_device *phy_8224_basePort_db_updated(struct phy_device *phydev, int *reg_data1, int *reg_data2)
{
    struct rtl8224_priv_info *phy_priv = (struct rtl8224_priv_info *)phydev->priv;

    *reg_data1 =  phy_read_mmd(phydev, 30, 0x4);
    if (*reg_data1 < 0)
        goto bus_read_error;
    *reg_data2 =  phy_read_mmd(phydev, 30, 0x5);
    if (*reg_data2 < 0)
        goto bus_read_error;

    if(*reg_data2 == 0x8224) /* base port match */
    {
        phy_priv->basePort = phydev;
        phy_priv->is_basePort = 1;
        phy_priv->port_offset = 0;
        phy_priv->memberPort[RTL8224_BASE_PORT_OFFSET]= phydev;
        if(*reg_data1 == 0x0)
            phy_priv->phytype = RTK_PHYLIB_RTL8224;
        else if(*reg_data1 == 0x7000)
            phy_priv->phytype = RTK_PHYLIB_RTL8224N;
        else
            goto basePort_match_error;
    }
    else
    {
        phy_priv->is_basePort = 0;
        return NULL;
    }
    return phydev;
bus_read_error:
    PR_ERR("\n[%s] read error!\n",__FUNCTION__);
    return NULL;
basePort_match_error:
    PR_ERR("\n[%s] base port match error!\n",__FUNCTION__);
    return NULL;
}

/* updated member port db*/
int phy_8224_member_db_updated(struct phy_device *phydev)
{
    struct rtl8224_priv_info *phy_priv = (struct rtl8224_priv_info *)phydev->priv;
    struct phy_device *tmp_phydev = NULL;
    struct rtl8224_priv_info *temp_priv;
    int index = RTL8224_PORT_NUM;
    int port_offset = 0;
    int phy_addr = phydev->mdio.addr;

    /* base port's phy address is the first always, */
    /* and 4 phy addresses are continuous */
    while(index > 0)
    {
        phy_addr--;
        index --;
        tmp_phydev = mdiobus_get_phy(phydev->mdio.bus, phy_addr); /* get other port's phy device by phy address*/
        temp_priv = (struct rtl8224_priv_info *)tmp_phydev->priv;
        if(tmp_phydev == NULL)
        {
            /* to find next one */
            continue;
        }
        else
        {
            if(temp_priv->is_basePort == 1)
            {
                /* Record base port*/
                phy_priv->basePort = tmp_phydev;
                /* Record member port*/
                phy_priv->memberPort[RTL8224_BASE_PORT_OFFSET] = tmp_phydev;
                /* port offset */
                port_offset = (phydev->mdio.addr - tmp_phydev->mdio.addr);
                phy_priv->memberPort[port_offset] = phydev;
                phy_priv->port_offset = port_offset;
                /* updated base port's member port db */
                temp_priv->memberPort[port_offset] = phydev;

                return 0;
            }
        }
    }
    PR_ERR("\n[%s](PHY addr %d) FAILED!!!\n",__FUNCTION__,phydev->mdio.addr);
    return -1;
}

int rtl8224_probe(struct phy_device *phydev)
{
    struct rtl8224_priv_info *priv = NULL;
    int reg_data1 = 0, reg_data2 = 0, db_ret = 0, tmp_base = 1;
    struct phy_device *tmp_phydev = NULL;
    struct rtl8224_priv_info *tmp_priv;

    RTL8224_INTERNAL_PORT_CHECK(phydev);

    priv = kmalloc(sizeof(struct rtl8224_priv_info), GFP_KERNEL);
    if (!priv)
    {
        return -ENOMEM;
    }
    memset(priv, 0, sizeof(struct rtl8224_priv_info));
    phydev->priv = priv;

    if (phydev->drv->phy_id == REALTEK_PHY_ID_RTL8224)
    {
        tmp_phydev = phy_8224_basePort_db_updated(phydev, &reg_data1, &reg_data2);
        /* updated member port db */
        if(tmp_phydev == NULL)
        {
             tmp_base = 0;
             db_ret = phy_8224_member_db_updated(phydev);
             if(db_ret)
                goto match_error;
        }
    }
    else
    {
        goto match_error;
    }
    tmp_priv = (struct rtl8224_priv_info *)phydev->priv;

    phydev->priv = priv;
    return 0;
match_error:
    PR_ERR("[%s] probe miss MATCH!!!\n",__FUNCTION__);
    devm_kfree(&phydev->mdio.dev, priv);
    return -ENXIO;
}


int rtl8224_get_features(struct phy_device *phydev)
{
    int ret;
    RTL8224_INTERNAL_PORT_CHECK(phydev);

    ret = genphy_c45_pma_read_abilities(phydev);
    if (ret)
        return ret;

    linkmode_or(phydev->supported, phydev->supported, PHY_BASIC_FEATURES);

    linkmode_set_bit(ETHTOOL_LINK_MODE_2500baseT_Full_BIT,
                       phydev->supported);

    /* not support 10M modes */
    linkmode_clear_bit(ETHTOOL_LINK_MODE_10baseT_Half_BIT,
                       phydev->supported);
    linkmode_clear_bit(ETHTOOL_LINK_MODE_10baseT_Full_BIT,
                       phydev->supported);

    return 0;
}


int rtl8224_config_init(struct phy_device *phydev)
{
    struct rtl8224_priv_info *priv = (struct rtl8224_priv_info *)phydev->priv;
    struct rtl8224_priv_info *tmp_priv = NULL;
    int ret = 0, loop = 0;
    int32   unit = 0;
    rtk_port_t port;
    RTL8224_INTERNAL_PORT_CHECK(phydev);

    ret = _phy_8224_dbCompleted_check(phydev);
    if(ret)
        PR_ERR("\n PHY data base is NOT completed\n");

    if(priv->is_basePort == 1)
    {
        PR_INFO("\n Base port (phy_addr %d), execute patch...\n",phydev->mdio.addr);

        for (loop = 0; loop < RTL8224_PORT_NUM; loop++)
        {
            if ((port = _phy_8224_get_phydevice_by_offset(unit, phydev, loop)) == NULL)
            {
                PR_ERR("\n Offset(%d) PHY device cannot get\n",loop);
                return -1;
            }
            tmp_priv = (struct rtl8224_priv_info *)port->priv;

            phy_rtl8224_patch(unit, port, loop);
            _phy_8224_mdi_reverse_set(port, RTL8224_MDI_PIN_SWAP);

            if ((RTL8224_MDI_PAIR_SWAP) != 0)
            {
                _phy_8224_tx_polarity_swap_set(port, RTL8224_MDI_PAIR_SWAP);
            }
            _phy_8224_interrupt_init(unit, port);
        }
        PR_INFO("\n Base port (phy_addr %d), path completed!\n",phydev->mdio.addr);
    }
    else
    {
        PR_INFO("\n(phy_addr %d) is member port\n", phydev->mdio.addr);
    }

    return ret;
}

int
rtl8224_cable_test_start(struct phy_device *phydev)
{
    int32   ret = RT_ERR_OK;
    uint32  unit = 0;
    uint32  phyData = 0, speed = 0, duplex = 0;
    uint32  tryTime = 1000;
    rtk_enable_t ena;
    rtk_port_t port = phydev;
    RTL8224_INTERNAL_PORT_CHECK(phydev);

    if ((ret = _phy_8224_common_c45_enable_get(unit, port, &ena)) != RT_ERR_OK)
        return ret;

    if (ena == DISABLED)
    {
        return RT_ERR_OPER_DENIED;
    }

    /* Check the port is link up or not?
     * Due to bit 2 is LL(latching low), need to read twice to get current status
     */
    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_PMAPMD, 0x1, &phyData)) != RT_ERR_OK)
        return ret;
    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_PMAPMD, 0x1, &phyData)) != RT_ERR_OK)
        return ret;

    _phy_8224_common_c45_speedDuplexStatusResReg_get(unit, port, &speed, &duplex);
    if (phyData & BIT_2)
    {
        if (speed == PORT_SPEED_10M)
        {
            return RT_ERR_PORT_NOT_SUPPORTED;
        }

        return RT_ERR_OK;
    }
    else
    {
	    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA400, &phyData)) != RT_ERR_OK)
	        return ret;
	    phyData |= (BIT_9); //[9]=1
	    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xA400, phyData)) != RT_ERR_OK)
	        return ret;

	    osal_time_mdelay(500);

	    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA422, &phyData)) != RT_ERR_OK)
	        return ret;
	    phyData &= (~BIT_15); //[15]=0
	    phyData |= (BIT_1); //[1]=1
	    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xA422, phyData)) != RT_ERR_OK)
	        return ret;

	    phyData |= (BIT_4 | BIT_5 | BIT_6 | BIT_7); //[7:4]=0xf
	    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xA422, phyData)) != RT_ERR_OK)
	        return ret;

	    phyData |= (BIT_0); //[0]=0x1
	    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xA422, phyData)) != RT_ERR_OK)
	        return ret;

	    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA422, &phyData)) != RT_ERR_OK)
	        return ret;

	    while((phyData & BIT_15) == 0)
	    {
	        osal_time_mdelay(10);
	        if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA422, &phyData)) != RT_ERR_OK)
	            return ret;
	        tryTime--;
	        if(tryTime == 0)
	            return RT_ERR_NOT_FINISH;
	    }
    }
    return ret;
}

int rtl8224_cable_test_get_status(struct phy_device *phydev, bool *finished)
{
    int32 ret = 0;
    uint32 pair = 0, unit = 0, length_cm;
    rtk_rtctResult_t result;
    RTL8224_INTERNAL_PORT_CHECK(phydev);

    *finished = false;
    ret = _phy_8224_rtctResult_get(unit, phydev, &result);
    if(ret == 0)
        *finished = true;

    for (pair = 0; pair < 4; pair++)
    {
        ethnl_cable_test_result(phydev, pair, _phy_8224_cable_test_report_trans(pair, &result));

        if(_phy_8224_cable_test_length_get(pair, &result, &length_cm))
            ethnl_cable_test_fault_length(phydev, pair, length_cm);

    }
    return 0;
}

int rtl8224_get_tunable(struct phy_device *phydev, struct ethtool_tunable *tuna, void *data)
{
    int32 ret = 0;
    rtk_enable_t val = 0;
    uint32 unit = 0;
    RTL8224_INTERNAL_PORT_CHECK(phydev);

    switch (tuna->id)
    {
        case ETHTOOL_PHY_EDPD:
            RTK_PHYLIB_ERR_CHK(phy_8224_linkDownPowerSavingEnable_get(unit, phydev, &val));
            *(u16 *)data = (val == DISABLED) ? ETHTOOL_PHY_EDPD_DISABLE : ETHTOOL_PHY_EDPD_DFLT_TX_MSECS;
            return 0;

        default:
            return -EOPNOTSUPP;
    }
}

int rtl8224_set_tunable(struct phy_device *phydev, struct ethtool_tunable *tuna, const void *data)
{
    int32 ret = 0;
    rtk_enable_t val = DISABLED;
    uint32 unit = 0;
    RTL8224_INTERNAL_PORT_CHECK(phydev);

    switch (tuna->id)
    {
        case ETHTOOL_PHY_EDPD:
            switch (*(const u16 *)data)
            {
                case ETHTOOL_PHY_EDPD_DISABLE:
                    val = DISABLED;
                    break;
                default:
                    val = ENABLED;
                    break;
            }
            RTK_PHYLIB_ERR_CHK(phy_8224_linkDownPowerSavingEnable_set(unit, phydev, val));
            return 0;

        default:
            return -EOPNOTSUPP;
    }
}

int rtl8224_config_intr(struct phy_device *phydev)
{
    int32 ret = 0;

    RTL8224_INTERNAL_PORT_CHECK(phydev);

    RTK_PHYLIB_ERR_CHK(_phy_8224_intr_enable(phydev, (phydev->interrupts == PHY_INTERRUPT_ENABLED)? 1 : 0));

    return ret;
}

int rtl8224_ack_intr(struct phy_device *phydev)
{
    int32 ret = 0;
    uint32 status = 0;

    RTL8224_INTERNAL_PORT_CHECK(phydev);

    RTK_PHYLIB_ERR_CHK(_phy_8224_intr_read_clear(phydev, &status));

    return ret;
}

irqreturn_t rtl8224_handle_intr(struct phy_device *phydev)
{
    irqreturn_t ret;
    uint32 status = 0;

    RTL8224_INTERNAL_PORT_CHECK(phydev);

    RTK_PHYLIB_ERR_CHK(_phy_8224_intr_read_clear(phydev, &status));
    if (status & BIT_4)
    {
        PR_INFO("[%s,%d] RTK_PHY_INTR_LINK_CHANGE\n", __FUNCTION__, __LINE__);
        phy_mac_interrupt(phydev);
    }

    return IRQ_HANDLED;
}

