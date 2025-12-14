#ifndef _MAC_AX_RXBD_SEPT_H_
#define _MAC_AX_RXBD_SEPT_H_

/* dword0 */
#define AX_RXBD_SEPT_RXP2BUFFSIZE_SH		0
#define AX_RXBD_SEPT_RXP2BUFFSIZE_MSK		0x3fff
#define AX_RXBD_SEPT_RXP1BUFFSIZE_SH		16
#define AX_RXBD_SEPT_RXP1BUFFSIZE_MSK		0x3fff

/* dword1 */
#define AX_RXBD_SEPT_PHY_ADDR_P1_L_SH		0
#define AX_RXBD_SEPT_PHY_ADDR_P1_L_MSK		0xffffffff

/* dword2 */
#define AX_RXBD_SEPT_PHY_ADDR_P2_L_SH		0
#define AX_RXBD_SEPT_PHY_ADDR_P2_L_MSK		0xffffffff

#endif // _MAC_AX_RXBD_SEPT_H_