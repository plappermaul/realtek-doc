#ifndef _MAC_AX_TXBD_H_
#define _MAC_AX_TXBD_H_

/* dword0 */
#define AX_TXBD_LENGTH_SH		0
#define AX_TXBD_LENGTH_MSK		0xffff
#define AX_TXBD_LS		BIT(30)

/* dword1 */
#define AX_TXBD_PHY_ADDR_L_SH		0
#define AX_TXBD_PHY_ADDR_L_MSK		0xffffffff

/* dword2 */
#define AX_TXBD_PHY_ADDR_H_SH		0
#define AX_TXBD_PHY_ADDR_H_MSK		0xffffffff

#endif // _MAC_AX_TXBD_H_