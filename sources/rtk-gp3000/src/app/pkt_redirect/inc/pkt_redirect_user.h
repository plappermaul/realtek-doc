#ifndef __PKT_REDIRECT_USER_H__
#define __PKT_REDIRECT_USER_H__

#define PKT_REDIRECT_OPTYPE_NONE            0x00
#define PKT_REDIRECT_OPTYPE_REGISTER        0x01
#define PKT_REDIRECT_OPTYPE_SEND_MSG        0x02 /* Send message between kernel/user */

#define PKT_REDIRECT_REGTYPE_REGISTER       0x01
#define PKT_REDIRECT_REGTYPE_DEREGISTER     0x02

#define PR_DATA(prData)     (((unsigned char *)prData) + sizeof(pr_data_t))


enum {
    PR_USER_UID_EPONOAM = 0,
    PR_USER_UID_GPONOMCI = 1,
    PR_USER_UID_EPONDYINGGASP = 2,
    PR_USER_UID_GPONOMCICTRL=3,
	PR_USER_UID_IGMPCTRL=4,
    PR_USER_UID_SFPCTRL=5,
    PR_USER_UID_IGMPMLD=6,
    PR_USER_UID_RLDP = 7,
    PR_USER_UID_PTPMASTER = 8,
    PR_USER_UID_PTPSLAVE = 9,
    PR_USER_UID_FTTROMCI = 10,
    PR_USER_UID_END
};

enum {
    PR_KERNEL_UID_GMAC = 0,
    PR_KERNEL_UID_GPONOMCI = 1,
    PR_KERNEL_UID_EPONDYINGGASP = 2,
    PR_KERNEL_UID_GPONOMCICTRL = 3,
	PR_KERNEL_UID_IGMPCTRL=4,
	PR_KERNEL_UID_BCASTER=5,
    PR_KERNEL_UID_TR142=6,
    PR_KERNEL_UID_SFPCTRL=7,
    PR_KERNEL_UID_IGMPMLD=8,
    PR_KERNEL_UID_RLDP = 9,
    PR_KERNEL_UID_PTP = 10,
    PR_KERNEL_UID_FTTROMCI = 11,
    PR_KERNEL_UID_END
};


/* Define the communication structures */
/* Data type for user -> kernel */
typedef struct pr_identifier_s {
    unsigned short prOpType;   /* Packet redirect operation type */
    unsigned short uid;        /* User ID - assigned by kernel/user program itself */
    int            pid;        /* Process ID - always 0 for kernel modules */
} pr_identifier_t;

typedef struct pr_data_s {
    unsigned short prOpType;   /* Packet redirect operation type */
    unsigned short uid;        /* User ID */
    int            isUser;
    /* ---------------------- */
    unsigned int   flag;       /* Flag */
    unsigned int   dataLen;    /* length */
    /* ---------------------- */
    /* Data */
} pr_data_t;

typedef struct pr_regUserApp_s {
    unsigned short prOpType;   /* Packet redirect operation type */
    unsigned short ownUid;     /* User ID */
    int            ownPid;     /* Process ID */
    /* ---------------------- */
    unsigned char  regType;    /* Register type */
    unsigned short mtu;        /* Maximum data size */
} pr_regUserApp_t;


int ptk_redirect_userApp_reg(
    int sock_fd,
    unsigned short uid,
    unsigned short mtu);

int ptk_redirect_userApp_dereg(
    int sock_fd,
    unsigned short uid);

int ptk_redirect_userApp_sendPkt(
    int sock_fd,
    unsigned short uid,
    unsigned int   flag,
    unsigned short dataLen,
    unsigned char *data);

int ptk_redirect_userApp_recvPkt(
    int sock_fd,
    unsigned short mtu,
    unsigned short *dataLen,
    unsigned char *payload);

#endif /* __PKT_REDIRECT_USER_H__ */


