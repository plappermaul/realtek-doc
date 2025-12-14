#ifndef _MAC_AX_ADDRINFO_T_H_
#define _MAC_AX_ADDRINFO_T_H_

/* dword0 */
#define AX_ADDRINFO_T_LENGTH_SH		    0
#define AX_ADDRINFO_T_LENGTH_MSK		0xffff
#define AX_ADDRINFO_T_INFO_NUM_SH		16
#define AX_ADDRINFO_T_INFO_NUM_MSK		0xff
#define AX_ADDRINFO_T_LS		        BIT(30)
#define AX_ADDRINFO_T_MSDU_LS		    BIT(31)

/* dword1 */
#define AX_ADDRINFO_T_PHY_ADDR_L_SH		0
#define AX_ADDRINFO_T_PHY_ADDR_L_MSK	0xffffffff

#endif // _MAC_AX_ADDRINFO_T_H_