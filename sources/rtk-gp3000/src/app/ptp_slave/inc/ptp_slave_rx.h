#ifndef PTP_SLAVE_RX_H
#define PTP_SLAVE_RX_H

void *ptp_slave_rxThread(void *argu);
void ptp_slave_rx_exit(void);
void ptp_slave_pkt_send(unsigned short dataLen,unsigned char *data,unsigned int portmask);
#endif