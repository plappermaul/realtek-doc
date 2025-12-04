#ifndef __EUROPA_EXPORT_H__
#define __EUROPA_EXPORT_H__

extern int32 rtk_europa_tx_power_get(uint32 voltage_v0, uint32 *i_mpd);

extern int32 rtk_europa_rx_power_get(uint32 rssi_v0, uint32 *v_rssi,uint32 *i_rssi);

#endif

