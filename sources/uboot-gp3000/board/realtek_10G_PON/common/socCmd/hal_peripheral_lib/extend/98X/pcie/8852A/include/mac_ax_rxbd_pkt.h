#ifndef _MAC_AX_RXBD_PKT_H_
#define _MAC_AX_RXBD_PKT_H_

/* dword0 */
#define AX_RXBD_PKT_RXBUFFSIZE_SH		0
#define AX_RXBD_PKT_RXBUFFSIZE_MSK		0x3fff

/* dword1 */
#define AX_RXBD_PKT_PHY_ADDR_L_SH		0
#define AX_RXBD_PKT_PHY_ADDR_L_MSK		0xffffffff

/* dword2 */
#define AX_RXBD_PKT_PHY_ADDR_H_SH		0
#define AX_RXBD_PKT_PHY_ADDR_H_MSK		0xffffffff

#endif // _MAC_AX_RXBD_PKT_H_