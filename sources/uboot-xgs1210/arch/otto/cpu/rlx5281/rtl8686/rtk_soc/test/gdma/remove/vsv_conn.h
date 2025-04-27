/*
* Copyright c                  Realtek Semiconductor Corporation, 2002
* All rights reserved.
*
* Program : Header File for VSV Connection
* Abstract :
* Author : Eng-Ching Khor (eckhor@realtek.com.tw)
* $Id: vsv_conn.h,v 1.5 2006-08-30 05:51:58 yjlou Exp $
*/

#ifndef _VSV_CONN_H_
#define _VSV_CONN_H_

extern void* conn_client;
extern void* conn_server;
void build_connection(void);
void vsv_endConn(void *connection);
void vsv_clearAllTable(void *connection);
void vsv_waitCycle(void *connection, int num);
uint32 vsv_registerMem(int baseAddr, int range);
void vsv_freeRegMem(int baseAddr);

uint32 vsv_readMem32(void* connection, uint32 addr);
void vsv_writeMem32(void *connection, uint32 addr, uint32 value);

void vsv_writePkt2Q(int port_id, int pkt_len, unsigned char* pkt_load, int ipg);
void vsv_schOutputPkt(void);
void vsv_writePkt(int port_id, int pkt_len, unsigned char* pkt_load, int ipg);
int vsv_readPkt(int port_id, unsigned char* pkt_load);

//void vsv_writeExtMem(int numArgs, vsvArgT *args);
//void vsv_readExtMem(vsvArgT *returnVal, int numArgs, vsvArgT *args);

uint16 vsv_readMem16(void* connection, uint32 addr);
uint8 vsv_readMem8(void* connection, uint32 addr);
void vsv_write_pktStr(int port_id, int pkt_len, unsigned char* pkt_load);
#endif
