
#ifndef _WLAN_CTRL_H
#define _WLAN_CTRL_H

/*--------------------Define -------------------------------------------*/

/*--------------------Define Enum---------------------------------------*/
enum WLAN_FWDL_TYPE {
    WLAN_FWDL_TYPE_ROM = 0,
    WLAN_FWDL_TYPE_FW = 1
};

/*--------------------Define MACRO--------------------------------------*/

/*--------------------Define Struct-------------------------------------*/
struct rtk_priv {
	struct mac_ax_ops *mac_ops;
	struct mac_ax_adapter *mac_adapter;
};

/*--------------------Export global variable----------------------------*/

/*--------------------Function declaration------------------------------*/
void WLAN_init(void);
void WLAN_fill_WD(u8* buf);
void WLAN_FWDL(u32 type, u32 Len);

#endif

