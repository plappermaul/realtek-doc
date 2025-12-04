/*
 * Include Files
 */

#include <common/debug/rt_log.h>
#include <scfg.h>
#include <dal/rtl8198f/dal_rtl8198f_rtl83xx.h>
#include <dal/rtl8198f/dal_rtl8226_status.h>

uint32 rtl8198f_board;

int32
rtk_rtl8226_port_status_get(rtk_port_t port,
			 rtk_port_linkStatus_t *pLinkStatus,
			 rtk_port_speed_t *pSpeed,
			 rtk_port_duplex_t *pDuplex)
{
	int linksp, linkst;

	RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH), "%s", __FUNCTION__);

	/* parameter check */
	RT_PARAM_CHK((port == CAERR_PORT_ID), RT_ERR_PORT_ID);
	RT_PARAM_CHK((NULL == pLinkStatus), RT_ERR_NULL_POINTER);
	RT_PARAM_CHK((NULL == pSpeed), RT_ERR_NULL_POINTER);
	RT_PARAM_CHK((NULL == pDuplex), RT_ERR_NULL_POINTER);


	/* function body */
	if ((rtl8198f_board & 0xF00) == 0x500) {
		linksp = rtl8226_get_link_speed(RTL8226_PORT1_PHY);
		linkst = rtl8226_get_link_status(RTL8226_PORT1_PHY);
	} else if ((rtl8198f_board & 0xF00) == 0x600) {
		linksp = rtl8226_get_link_speed(RTL8226_PORT0_PHY - port);
		linkst = rtl8226_get_link_status(RTL8226_PORT0_PHY - port);
	} else
		printk("ERROR :%s Unknow board type!!!\n", __func__);

	switch (linksp) {
	case RTL8226_LINK_DOWN:
		*pLinkStatus = PORT_LINKDOWN;
		*pSpeed = 0;
		*pDuplex = 0;
		break;
	case RTL8226_LINK_UP_2G5:
		*pLinkStatus = PORT_LINKUP;
		*pSpeed = PORT_SPEED_2500M;
		*pDuplex = 1;
		break;
	case RTL8226_LINK_UP_2G5LITE:
		*pLinkStatus = PORT_LINKUP;
		*pSpeed = PORT_SPEED_1000M;
		*pDuplex = 1;
		break;
	case RTL8226_LINK_UP_1G:
		*pLinkStatus = PORT_LINKUP;
		*pSpeed = PORT_SPEED_1000M;
		*pDuplex = 1;
		break;
	case RTL8226_LINK_UP_100M:
		*pLinkStatus = PORT_LINKUP;
		*pSpeed = PORT_SPEED_100M;
		*pDuplex = linkst & RTL8226_PORT_DUPLEX;
		break;
	case RTL8226_LINK_UP_10M:
		*pLinkStatus = PORT_LINKUP;
		*pSpeed = PORT_SPEED_10M;
		*pDuplex = linkst & RTL8226_PORT_DUPLEX;
		break;
	default:
		printk("ERROR :%s Unknow link status!!!\n", __func__);
		break;
	}

	return RT_ERR_OK;
} /*end of dal_rtl8198f_switch_maxPktLenByPort_get*/

int32
rtk_rtl8226_port_flowctrl_get(rtk_port_t port,
			 rtk_enable_t *pTxFlowctrl,
			 rtk_enable_t *pRxFlowctrl)
{

	int linkst;

	RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH), "%s", __FUNCTION__);

	/* parameter check */
	RT_PARAM_CHK((port == CAERR_PORT_ID), RT_ERR_PORT_ID);
	RT_PARAM_CHK((NULL == pTxFlowctrl), RT_ERR_NULL_POINTER);
	RT_PARAM_CHK((NULL == pRxFlowctrl), RT_ERR_NULL_POINTER);

	/* function body */
	if ((rtl8198f_board & 0xF00) == 0x500)
		linkst = rtl8226_get_link_status(RTL8226_PORT1_PHY);
	else if ((rtl8198f_board & 0xF00) == 0x600)
		linkst = rtl8226_get_link_status(RTL8226_PORT0_PHY - port);
	else
		printk("ERROR :%s Unknow board type!!!\n", __func__);

	if (linkst & RTL8226_PORT_TXFC)
		*pTxFlowctrl = 1;
	if (linkst & RTL8226_PORT_RXFC)
		*pRxFlowctrl = 1;

	return RT_ERR_OK;
}

int32
rtk_rtl8226_port_power_get(rtk_port_t port, rtk_enable_t *enable)
{
	unsigned int regdata = 0;
	int ret = RT_ERR_FAILED;

	if ((rtl8198f_board & 0xF00) == 0x500){
		ret = rtl8226_phy_reg_get(RTL8226_PORT1_PHY, 1, 0x0, &regdata);
		if(ret)
			return ret;

		if((regdata & (1<<11)) == 0)
			*enable = 1;
		else
			*enable = 0;
	}
	else if ((rtl8198f_board & 0xF00) == 0x600){
		ret = rtl8226_phy_reg_get(RTL8226_PORT0_PHY - port, 1, 0x0, &regdata);
		if(ret)
			return ret;

		if((regdata & (1<<11)) == 0)
			*enable = 1;
		else
			*enable = 0;
	}

	return ret;	
}

int32
rtk_rtl8226_port_power_set(rtk_port_t port, rtk_enable_t enable)
{
	unsigned short regdata = 0;
	int ret = RT_ERR_FAILED;

	if ((rtl8198f_board & 0xF00) == 0x500){
		ret = rtl8226_phy_reg_get(RTL8226_PORT1_PHY, 1, 0x0, &regdata);
		if (ret)
			return ret;

		if (enable == ENABLED)
			regdata &= ~(1<<11);
		else
			regdata |= (1<<11);

		ret = rtl8226_phy_reg_set(RTL8226_PORT1_PHY, 1, 0x0, regdata);
		if (ret)
			return ret;

	}
	else if ((rtl8198f_board & 0xF00) == 0x600){
		ret = rtl8226_phy_reg_get(RTL8226_PORT0_PHY - port, 1, 0x0, &regdata);
		if (ret)
			return ret;

		if (enable == ENABLED)
			regdata &= ~(1<<11);
		else
			regdata |= (1<<11);

		ret = rtl8226_phy_reg_set(RTL8226_PORT0_PHY - port, 1, 0x0, regdata);
		if (ret)
			return ret;
	}

	return ret;	
}

int32
rtk_rtl8226_port_phy_an_ability_get(rtk_port_t port, rtk_port_phy_ability_t *pAbility)
{
	unsigned int regdata = 0;
	int ret = RT_ERR_FAILED;

	if ((rtl8198f_board & 0xF00) == 0x500){
		ret = rtl8226_phy_reg_get(RTL8226_PORT1_PHY, 7, 0x20, &regdata);
		if (ret)
			return ret;

		if (regdata & RTL8226_PORT_2500F)
			pAbility->Full_2500 = 1;

		ret = rtl8226_phy_reg_get(RTL8226_PORT1_PHY, 31, 0xA412, &regdata);
		if (ret)
			return ret;

		if (regdata & RTL8226_PORT_1000F)
			pAbility->Full_1000 = 1;

		ret = rtl8226_phy_reg_get(RTL8226_PORT1_PHY, 7, 0x10, &regdata);
		if (ret)
			return ret;

		if (regdata & RTL8226_PORT_100F)
			pAbility->Full_100 = 1;
		if (regdata & RTL8226_PORT_100H)
			pAbility->Half_100 = 1;
		if (regdata & RTL8226_PORT_10F)
			pAbility->Full_10 = 1;
		if (regdata & RTL8226_PORT_10H)
			pAbility->Half_10 = 1;
		if (regdata & RTL8226_PORT_FC)
			pAbility->FC = 1;
		if (regdata & RTL8226_PORT_ASY_FC)
			pAbility->AsyFC = 1;

	}
	else if ((rtl8198f_board & 0xF00) == 0x600){
		ret = rtl8226_phy_reg_get(RTL8226_PORT0_PHY - port, 7, 0x20, &regdata);
		if (ret)
			return ret;

		if (regdata & RTL8226_PORT_2500F)
			pAbility->Full_2500 = 1;

		ret = rtl8226_phy_reg_get(RTL8226_PORT0_PHY - port, 31, 0xA412, &regdata);
		if (ret)
			return ret;

		if (regdata & RTL8226_PORT_1000F)
			pAbility->Full_1000 = 1;

		ret = rtl8226_phy_reg_get(RTL8226_PORT0_PHY - port, 7, 0x10, &regdata);
		if (ret)
			return ret;

		if (regdata & RTL8226_PORT_100F)
			pAbility->Full_100 = 1;
		if (regdata & RTL8226_PORT_100H)
			pAbility->Half_100 = 1;
		if (regdata & RTL8226_PORT_10F)
			pAbility->Full_10 = 1;
		if (regdata & RTL8226_PORT_10H)
			pAbility->Half_10 = 1;
		if (regdata & RTL8226_PORT_FC)
			pAbility->FC = 1;
		if (regdata & RTL8226_PORT_ASY_FC)
			pAbility->AsyFC = 1;
	}

	return ret;
}

int32
rtk_rtl8226_port_phy_an_ability_set(rtk_port_t port, rtk_port_phy_ability_t *pAbility)
{
	unsigned int regdata = 0;
	int ret = RT_ERR_FAILED;

	if ((rtl8198f_board & 0xF00) == 0x500){
		ret = rtl8226_phy_reg_get(RTL8226_PORT1_PHY, 7, 0x20, &regdata);
		if (ret)
			return ret;
		regdata &= ~RTL8226_PORT_2500F;
		if (pAbility->Full_2500)
			regdata |= RTL8226_PORT_2500F;
		ret = rtl8226_phy_reg_set(RTL8226_PORT1_PHY, 7, 0x20, regdata);
		if(ret)
			return ret;

		ret = rtl8226_phy_reg_get(RTL8226_PORT1_PHY, 31, 0xA412, &regdata);
		if (ret)
			return ret;
		regdata &= ~RTL8226_PORT_1000F;
		if (pAbility->Full_1000)
			regdata |= RTL8226_PORT_1000F;
		ret = rtl8226_phy_reg_set(RTL8226_PORT1_PHY, 31, 0xA412, regdata);
		if(ret)
			return ret;

		ret = rtl8226_phy_reg_get(RTL8226_PORT1_PHY, 7, 0x10, &regdata);
		if (ret)
			return ret;
		regdata &= ~(RTL8226_PORT_ASY_FC | RTL8226_PORT_FC | \
			RTL8226_PORT_100F|RTL8226_PORT_100H|RTL8226_PORT_10F|RTL8226_PORT_10H);
		if (pAbility->Full_100) {
			regdata |= RTL8226_PORT_100F;
		}
		if (pAbility->Half_100) {
			regdata |= RTL8226_PORT_100H;
		}
		if (pAbility->Full_10) {
			regdata |= RTL8226_PORT_10F;
		}
		if (pAbility->Half_10) {
			regdata |= RTL8226_PORT_10H;
		}
		if (pAbility->AsyFC) {
			regdata |= RTL8226_PORT_ASY_FC;
		}
		if (pAbility->FC) {
			regdata |= RTL8226_PORT_FC;
		}
		ret = rtl8226_phy_reg_set(RTL8226_PORT1_PHY, 7, 0x10, regdata);
		if(ret)
			return ret;

		// Restart AN
		ret = rtl8226_phy_reg_get(RTL8226_PORT1_PHY, 7, 0, &regdata);
		if (ret)
			return ret;
		regdata |= RTL8226_PORT_RE_AN;
		ret = rtl8226_phy_reg_set(RTL8226_PORT1_PHY, 7, 0, regdata);
		if (ret)
			return ret;
	}
	else if ((rtl8198f_board & 0xF00) == 0x600){
		ret = rtl8226_phy_reg_get(RTL8226_PORT0_PHY - port, 7, 0x20, &regdata);
		if (ret)
			return ret;
		regdata &= ~RTL8226_PORT_2500F;
		if (pAbility->Full_2500)
			regdata |= RTL8226_PORT_2500F;

		ret = rtl8226_phy_reg_set(RTL8226_PORT0_PHY - port, 7, 0x20, regdata);
		if (ret)
			return ret;


		ret = rtl8226_phy_reg_get(RTL8226_PORT0_PHY - port, 31, 0xA412, &regdata);
		if (ret)
			return ret;
		regdata &= ~RTL8226_PORT_1000F;
		if (pAbility->Full_1000)
			regdata |= RTL8226_PORT_1000F;

		ret = rtl8226_phy_reg_set(RTL8226_PORT0_PHY - port, 31, 0xA412, regdata);
		if (ret)
			return ret;

		ret = rtl8226_phy_reg_get(RTL8226_PORT0_PHY - port, 7, 0x10, &regdata);
		if (ret)
			return ret;
		regdata &= ~(RTL8226_PORT_ASY_FC | RTL8226_PORT_FC | \
			RTL8226_PORT_100F|RTL8226_PORT_100H|RTL8226_PORT_10F|RTL8226_PORT_10H);
		if (pAbility->Full_100) {
			regdata |= RTL8226_PORT_100F;
		}
		if (pAbility->Half_100) {
			regdata |= RTL8226_PORT_100H;
		}
		if (pAbility->Full_10) {
			regdata |= RTL8226_PORT_10F;
		}
		if (pAbility->Half_10) {
			regdata |= RTL8226_PORT_10H;
		}
		if (pAbility->AsyFC) {
			regdata |= RTL8226_PORT_ASY_FC;
		}
		if (pAbility->FC) {
			regdata |= RTL8226_PORT_FC;
		}

		ret = rtl8226_phy_reg_set(RTL8226_PORT0_PHY - port, 7, 0x10, regdata);
		if (ret)
			return ret;

		// Restart AN
		ret = rtl8226_phy_reg_get(RTL8226_PORT0_PHY - port, 7, 0, &regdata);
		if (ret)
			return ret;
		regdata |= RTL8226_PORT_RE_AN;
		ret = rtl8226_phy_reg_set(RTL8226_PORT0_PHY - port, 7, 0, regdata);
		if (ret)
			return ret;
	}

	return ret;
}

rtk_port_t dal_rtl8198f_lspid_mapping(rtk_port_t port)
{
	rtl8198f_board = rtl_get_board_type();

	if (((rtl8198f_board & 0xF00) == 0x500) && port == 0)
		return port + 1;
	else if (((rtl8198f_board & 0xF00) == 0x600) && port > 1)
		return port - 2;
	else
		return port;
}

int32 dal_rtl8226_port_link_get(rtk_port_t port,
				rtk_port_linkStatus_t *pLinkStatus)
{
	rtk_port_speed_t speed;
	rtk_port_duplex_t duplex;

	rtl8198f_board = rtl_get_board_type();

	if ((rtl8198f_board & 0xF00) == 0x500) {
		if (port == 1) {
			rtk_rtl8226_port_status_get(port, pLinkStatus, &speed, &duplex);
			return RT_ERR_OK;
		}
	} else if ((rtl8198f_board & 0xF00) == 0x600) {
		if (port < 2) {
			rtk_rtl8226_port_status_get(port, pLinkStatus, &speed, &duplex);
			return RT_ERR_OK;
		}
	}

	return RT_ERR_PORT_ID;
}

int32 dal_rtl8226_port_mac_get(rtk_port_t port,
				rtk_port_mac_ability_t *pPortStatus)
{
	rtk_port_linkStatus_t status;

	rtl8198f_board = rtl_get_board_type();
	if ((rtl8198f_board & 0xF00) == 0x500) {
		if (port == 1) {
			rtk_rtl8226_port_status_get(port, &status,
				&(pPortStatus->speed),
				&(pPortStatus->duplex));

			rtk_rtl8226_port_flowctrl_get(port, &(pPortStatus->txpause),
				&(pPortStatus->rxpause));

			return RT_ERR_OK;
		}
	} else if ((rtl8198f_board & 0xF00) == 0x600) {
		if (port < 2) {
			rtk_rtl8226_port_status_get(port, &status,
				&(pPortStatus->speed),
				&(pPortStatus->duplex));

			rtk_rtl8226_port_flowctrl_get(port, &(pPortStatus->txpause),
				&(pPortStatus->rxpause));

			return RT_ERR_OK;
		}
	}

	return RT_ERR_PORT_ID;
}

int32 dal_rtl8226_port_stat_get(rtk_port_t port, rtk_stat_port_cntr_t *pPortCntrs)
{
	rtl8198f_board = rtl_get_board_type();
	if ((rtl8198f_board & 0xF00) == 0x500) {
		if (port == 1) {
			dal_rtl8198f_stat_embedded_port_get(port, pPortCntrs);
			return RT_ERR_OK;
		}
	} else if ((rtl8198f_board & 0xF00) == 0x600) {
		if (port < 2) {
			dal_rtl8198f_stat_embedded_port_get(port, pPortCntrs);
			return RT_ERR_OK;
		}
	}

	return RT_ERR_PORT_ID;
}

int32 dal_rtl8226_port_power_get(rtk_port_t port,rtk_enable_t *enable)
{
	rtl8198f_board = rtl_get_board_type();

	if ((rtl8198f_board & 0xF00) == 0x500) {
		if (port == 1) {
			rtk_rtl8226_port_power_get(port, enable);
			return RT_ERR_OK;
		}
	} else if ((rtl8198f_board & 0xF00) == 0x600) {
		if (port < 2) {
			rtk_rtl8226_port_power_get(port, enable);
			return RT_ERR_OK;
		}
	}

	return RT_ERR_PORT_ID;
}


int32 dal_rtl8226_port_power_set(rtk_port_t port,rtk_enable_t enable)
{
	rtl8198f_board = rtl_get_board_type();

	if ((rtl8198f_board & 0xF00) == 0x500) {
		if (port == 1) {
			rtk_rtl8226_port_power_set(port, enable);
			return RT_ERR_OK;
		}
	} else if ((rtl8198f_board & 0xF00) == 0x600) {
		if (port < 2) {
			rtk_rtl8226_port_power_set(port, enable);
			return RT_ERR_OK;
		}
	}

	return RT_ERR_PORT_ID;
}

int32 dal_rtl8226_port_phy_an_ability_get(rtk_port_t port,rtk_port_phy_ability_t *pAbility)
{
	rtl8198f_board = rtl_get_board_type();

	if ((rtl8198f_board & 0xF00) == 0x500) {
		if (port == 1) {
			rtk_rtl8226_port_phy_an_ability_get(port, pAbility);
			return RT_ERR_OK;
		}
	} else if ((rtl8198f_board & 0xF00) == 0x600) {
		if (port < 2) {
			rtk_rtl8226_port_phy_an_ability_get(port, pAbility);
			return RT_ERR_OK;
		}
	}

	return RT_ERR_PORT_ID;
}

int32 dal_rtl8226_port_phy_an_ability_set(rtk_port_t port,rtk_port_phy_ability_t *pAbility)
{
	rtl8198f_board = rtl_get_board_type();

	if ((rtl8198f_board & 0xF00) == 0x500) {
		if (port == 1) {
			rtk_rtl8226_port_phy_an_ability_set(port, pAbility);
			return RT_ERR_OK;
		}
	} else if ((rtl8198f_board & 0xF00) == 0x600) {
		if (port < 2) {
			rtk_rtl8226_port_phy_an_ability_set(port, pAbility);
			return RT_ERR_OK;
		}
	}

	return RT_ERR_PORT_ID;
}

