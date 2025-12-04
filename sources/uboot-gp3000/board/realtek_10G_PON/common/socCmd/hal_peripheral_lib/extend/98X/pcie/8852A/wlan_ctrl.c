
#include "halmac/pltfm_cfg.h"
#include "halmac/mac_ax.h"
#include "halmac/mac_ax/init.h"
#include "pltfm_cb.h"
#include "wlan_ctrl.h"

extern int printf(const char* fmt, ...);
extern void mdelay(unsigned long);

struct rtk_priv rtkpriv;

static u32 wmac_pwr_on(struct rtk_priv *rtkpriv)
{
	u32 ret;
	struct mac_ax_adapter *halmac = rtkpriv->mac_adapter;
	struct mac_ax_ops *ops = rtkpriv->mac_ops;

	if (!halmac || !ops)
		return -1;

	ret = ops->pwr_switch(halmac, 1);
	if (ret)
		return ret;

	return 0;
}

static u32 wmac_init_mac(struct rtk_priv *rtkpriv)
{
	u32 ret;
	struct mac_ax_adapter *halmac = rtkpriv->mac_adapter;
	struct mac_ax_ops *ops = rtkpriv->mac_ops;
    struct mac_ax_role_info role_info;

	if (!halmac || !ops)
		return -1;

	ret = ops->sys_init(halmac);
	if (ret)
		return ret;

	ret = ops->trx_init(halmac, MAC_AX_TRX_LOOPBACK);
	if (ret)
		return ret;

	/* !!move to ieee80211_ops->sta_add later */
	role_info.band = 0;
	role_info.macid = 0;
	role_info.wmm = 0;
	ret = ops->add_role(halmac, &role_info);

	return 0;
}


static u32 wmac_init_hal(struct rtk_priv *rtkpriv)
{
	u32 ret;

	ret = wmac_pwr_on(rtkpriv);
	if (ret){
        printf("power on fail\n");
        return ret;
    }

	ret = wmac_init_mac(rtkpriv);
	if (ret) {
		printf("wmac_init_mac!!\n");
		return ret;
	}

	return 0;
}

void WLAN_init()
{
    enum mac_ax_intf mac_intf;
    struct rtk_priv *rtk_priv;
    u32 ret;

    rtk_priv = &rtkpriv;
    mac_intf = MAC_AX_INTF_PCIE;

    ret = mac_ax_ops_init(rtk_priv, &pltfm_cb, mac_intf,
			      &(rtk_priv->mac_adapter), &(rtk_priv->mac_ops));

	if (ret) {
		printf("init mac api : %d\n", ret);
		return;
	}

    wmac_init_hal(rtk_priv);

}

void WLAN_fill_WD(u8 *buf)
{
    struct mac_ax_adapter *adapter = rtkpriv.mac_adapter;
    struct mac_ax_ops *api = rtkpriv.mac_ops;
    struct mac_ax_txpkt_info info;
    u32 txdesc_size;

    info.type = MAC_AX_PKT_DATA;
    info.pktsize = 0x40;

    info.u.data.agg_en = 0;
    info.u.data.userate = 1;
    info.u.data.data_rate = 139;
    info.u.data.data_bw = 1;
    info.u.data.data_gi_ltf = 0;
    info.u.data.data_er = 0;
    info.u.data.data_dcm = 0;
    info.u.data.data_stbc = 0;
    info.u.data.data_ldpc = 0;
    info.u.data.dis_data_fb = 1;
    info.u.data.dis_rts_fb = 1;
    info.u.data.max_agg_num = 0;
    info.u.data.lifetime_sel = 0;
    info.u.data.ampdu_density = 0;

    info.u.data.ch = 0;
    info.u.data.hdr_len = 26;
    info.u.data.macid = 0;
    txdesc_size = api->txdesc_len(adapter, &info);

    api->build_txdesc(adapter, &info, buf, txdesc_size);

    return;
}


void WLAN_FWDL(u32 type, u32 Len)
{
    struct mac_ax_adapter *adapter = rtkpriv.mac_adapter;
    u8 *ROM, *fw;
    u32 ROMAddr;

    switch(type)
    {
        case WLAN_FWDL_TYPE_ROM:
            ROM = (u8 *)0x00600000;
            ROMAddr = 0x18900000;
            Len = 0x70000;
            mac_romdl(adapter, ROM, ROMAddr, Len);
            break;
        case WLAN_FWDL_TYPE_FW:
            fw = (u8 *)0x00700000;
            mac_fwdl(adapter, fw, Len);
            break;
        default:
            return;
    }
}

