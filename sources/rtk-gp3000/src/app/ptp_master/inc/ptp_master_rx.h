#ifndef PTP_MASTER_RX_H
#define PTP_MASTER_RX_H

void *ptp_master_rxThread(void *argu);
void ptp_master_rx_exit(void);
void ptp_master_pkt_send(unsigned short dataLen,unsigned char *data,unsigned int portmask);

#endif
