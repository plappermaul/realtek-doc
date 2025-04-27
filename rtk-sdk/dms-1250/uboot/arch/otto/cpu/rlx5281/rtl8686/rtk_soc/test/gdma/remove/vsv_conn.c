
#include <stdio.h>
#include <time.h>
#include "virtualMac.h"
#include "vsv_conn.h"
#include "rtl_glue.h"
#include "vsv.h"

void* conn_client;
void* conn_server;

/*
 * C model as a server and support below function
 */
void vsv_readExtMem(vsvArgT *returnVal, int numArgs, vsvArgT *args){
	uint32 *p;
	int i;

	for(i=0;i<1000000;i++) ;
	p = (uint32*) args[0].data.integerVal;
	/* printf("vsv_readExtMem: addr=%p, \n",p ); */
#if 1 /* tysu: we swapped the cluster packet content */
	returnVal->data.integerVal = (*p);	
#else
	returnVal->data.integerVal = htonl(*p);
#endif
	/* printf("   data = 0x%08x\n", returnVal->data.integerVal ); */
}/*end function vsv_readExtMem*/

void vsv_writeExtMem(int numArgs, vsvArgT *args){
	uint32 *p;
	int i;
	
	for(i=0;i<1000000;i++) ;
	p = (uint32*) args[0].data.integerVal;
#if 1 /* tysu: we swapped the cluster packet content */
	*p = (args[1].data.integerVal);	
#else
	*p = ntohl(args[1].data.integerVal);
#endif
	/* printf("vsv_writeExtMem:addr=%p ; data=0x%x\n",p,*p); */
}/*end vsv_writeExtMem*/

void vsv_error_occur(int numArgs, vsvArgT *args){
  printf("vsv_eror_occur:: %s\n",args[0].data.stringVal);
}


/*
 * C model as a client, can use below function to request server
 */ 
void build_connection(void){
  char server[100];
	FILE * fp;
	vsvArgT argStart;
	vsvArgT returnVal,argread[1],argwrite[2];
  argStart.vtype = VSV_NONVAR_ARG;
  argStart.dtype = VSV_INTEGER_ARG;

  model_setTestTarget(IC_TYPE_REAL);

	fp = fopen("865xC/c_server.tmp","w");
	if ( fp==NULL) printf("file open error\n");
	printf("Please enter C server workstation host name or IP address ('.' for localhost)\n");
	printf(">");
	scanf("%s",server);
	if ( strcmp( server, "." )==0 ) strcpy( server, "localhost" );
	printf("C Server: %s\n",server);
	fprintf(fp,"%s\n",server);
	fclose(fp);
	conn_server = vsv_MakeServer(1111,0);
	
	printf("Please enter VERA server workstation host name or IP address ('.' for localhost)\n");
	printf(">");
	scanf("%s",server);
	if ( strcmp( server, "." )==0 ) strcpy( server, "localhost" );
	printf("VERA Server: %s\n",server);
	conn_client = vsv_MakeClient(server, 1234, 0);/*host, port, auth*/

	if( conn_client == NULL){
		printf("Null conn_client\n");
		exit(1);
		return ;
	}
	if ( conn_server == NULL){
		printf("Null conn_server\n");
		exit(1);
		return;
	}

	returnVal.vtype = VSV_VAR_ARG;
	returnVal.dtype = VSV_INTEGER_ARG;
	argread[0].vtype = VSV_NONVAR_ARG;
	argread[0].dtype = VSV_INTEGER_ARG;
	argwrite[0].vtype = VSV_NONVAR_ARG;
	argwrite[0].dtype = VSV_INTEGER_ARG;
	argwrite[1].vtype = VSV_NONVAR_ARG;
	argwrite[1].dtype = VSV_INTEGER_ARG;

	if( vsv_RegisterFunc(conn_server,"vsv_readExtMem",&returnVal,1,argread,vsv_readExtMem) )
		perror(vsv_GetErrorMesg());
	if( vsv_RegisterTask(conn_server,"vsv_writeExtMem",2,argwrite,vsv_writeExtMem) )
		perror(vsv_GetErrorMesg());
	
	printf("Model: wait connection\n");
  if ( vsv_UpConnections(10*60) ){
    printf("connect time out\n");
	exit(1);
    return ;
  }
	printf("Model: socket connect up\n");

#if 0
	if ( vsv_AnswerCalls(conn_server,VSV_LOOP_FOREVER) )
		perror(vsv_GetErrorMesg());
#endif

	vsv_CallTask(conn_client, "vsv_start", VSV_WAIT, 0, &argStart);
/* return conn_client; */
}/*end of build_clientConn*/



void vsv_endConn(void *conn_client){
  vsvArgT arg;

  arg.vtype = VSV_NONVAR_ARG;
  arg.dtype = VSV_INTEGER_ARG;
	
  vsv_CallTask(conn_client, "vsv_closeConn", VSV_WAIT, 0, &arg);
  vsv_CloseConn(&conn_client);
	vsv_CloseConn(&conn_server);
}


void vsv_clearAllTable(void *conn_client){
  vsvArgT arg;

  arg.vtype = VSV_NONVAR_ARG;
  arg.dtype = VSV_INTEGER_ARG;
  vsv_CallTask(conn_client, "vsv_resetAllTable", VSV_WAIT, 0, &arg);
}


void vsv_waitCycle(void *conn_client, int numCycle){
	vsvArgT arg;

	arg.vtype = VSV_NONVAR_ARG;
	arg.dtype = VSV_INTEGER_ARG;
	arg.data.integerVal = numCycle;

	vsv_CallTask(conn_client, "vsv_waitCycle",VSV_WAIT, 1,&arg);
}


uint32 vsv_registerMem(int baseAddr, int range){
  vsvArgT arg[2],returnVal;
	uint32  data;

	arg[0].vtype = VSV_NONVAR_ARG;
	arg[0].dtype = VSV_INTEGER_ARG;
	arg[0].data.integerVal = baseAddr;
	arg[1].vtype = VSV_NONVAR_ARG;
	arg[1].dtype = VSV_INTEGER_ARG;
	arg[1].data.integerVal = range;
	returnVal.vtype = VSV_NONVAR_ARG;
	returnVal.dtype = VSV_INTEGER_ARG;

	vsv_CallFunc(conn_client, "vsv_registerMem", &returnVal, 2, arg);

	data = returnVal.data.integerVal;
	if ( data < 0 )
		return FAILED;
	else
		return SUCCESS;
}//end vsv_registerMem

void vsv_freeRegMem(int baseAddr){
  vsvArgT arg;

	arg.vtype = VSV_NONVAR_ARG;
	arg.dtype = VSV_INTEGER_ARG;
	arg.data.integerVal = baseAddr;

	vsv_CallTask(conn_client, "vsv_freeRegMem", VSV_WAIT, 1, &arg);
	
}//vsv_freeRegMem



uint32 vsv_readMem32(void* conn_client, uint32 addr){
  vsvArgT  returnVal;
  vsvArgT  arg;
  int  data;

  returnVal.vtype = VSV_VAR_ARG;
  returnVal.dtype = VSV_INTEGER_ARG;
  arg.vtype = VSV_NONVAR_ARG;
  arg.dtype = VSV_INTEGER_ARG;
  arg.data.integerVal = (int) addr;

  vsv_CallFunc(conn_client, "vsv_readMem32", &returnVal, 1, &arg);

  data = returnVal.data.integerVal;
  return (uint32)data;  

}/*end read_mem32*/


uint16 vsv_readMem16(void* conn_client, uint32 addr){
  vsvArgT  returnVal;
  vsvArgT  arg;
  int  data;

  returnVal.vtype = VSV_VAR_ARG;
  returnVal.dtype = VSV_INTEGER_ARG;
  arg.vtype = VSV_NONVAR_ARG;
  arg.dtype = VSV_INTEGER_ARG;
  arg.data.integerVal = (int) addr;

  vsv_CallFunc(conn_client, "vsv_readMem16", &returnVal, 1, &arg);

  data = returnVal.data.integerVal;
  return (uint16)data;

}/*end read_mem16*/


uint8 vsv_readMem8(void* conn_client, uint32 addr){
  vsvArgT  returnVal;
  vsvArgT  arg;
  int  data;

  returnVal.vtype = VSV_VAR_ARG;
  returnVal.dtype = VSV_INTEGER_ARG;
  arg.vtype = VSV_NONVAR_ARG;
  arg.dtype = VSV_INTEGER_ARG;
  arg.data.integerVal = (int) addr;

  vsv_CallFunc(conn_client, "vsv_readMem8", &returnVal, 1, &arg);

  data = returnVal.data.integerVal;
  return (uint8)data;

}/*end read_mem8*/


void vsv_writeMem32(void *conn_client, uint32 addr, uint32 value){
  vsvArgT arg[2];

  arg[0].vtype = VSV_NONVAR_ARG;
  arg[0].dtype = VSV_INTEGER_ARG;
  arg[0].data.integerVal = (int) addr;
  arg[1].vtype = VSV_NONVAR_ARG;
  arg[1].dtype = VSV_INTEGER_ARG;
  arg[1].data.integerVal = (int) value;

  vsv_CallTask(conn_client,"vsv_writeMem32", VSV_WAIT, 2, arg);

}/*end write_mem32*/

void vsv_write_pktStr(int port_id, int pkt_len, unsigned char * pkt_load){
  vsvArgT arg[3];
	arg[0].vtype = VSV_NONVAR_ARG;
	arg[0].dtype = VSV_INTEGER_ARG;
	arg[0].data.integerVal = port_id;
	arg[1].vtype = VSV_NONVAR_ARG;
	arg[1].dtype = VSV_INTEGER_ARG;
	arg[1].data.integerVal = pkt_len;
	arg[2].vtype = VSV_NONVAR_ARG;
	arg[2].dtype = VSV_STRING_ARG;
	arg[2].data.stringVal = pkt_load;
	
	vsv_CallTask(conn_client,"vsv_write_pkt",VSV_WAIT,3,arg);

}

void vsv_writePkt2Q(int port_id, int pkt_len, unsigned char *pkt_load, int ipg){
  vsvArgT arg3[3],arg1[1],arg2[2];
	int num,i,len_tmp;
	uint32 data32;
	uint32 data8[4]={0,0,0,0};

#if 0 /* for debug */
printf("-----------  vsv_writePkt2Q -----------\n");
printf("port:%2d, pktlen:%3d, ipg:%2d\n",port_id,pkt_len,ipg);
printf("p_load: ");
for(i=0;i<pkt_len;i++)
printf("%x ",(uint8)pkt_load[i]);
printf("\n-----------  vsv_writePkt2Q -----------\n");
#endif

	/* printf("vsv_writePkt2Q: port:%2d, pktlen:%3d, ipg:%2d\n",port_id,pkt_len,ipg); */
	len_tmp = pkt_len;
	arg3[0].vtype = VSV_NONVAR_ARG;
	arg3[0].dtype = VSV_INTEGER_ARG;
	arg3[0].data.integerVal = port_id;
	arg3[1].vtype = VSV_NONVAR_ARG;
	arg3[1].dtype = VSV_INTEGER_ARG;
	arg3[1].data.integerVal = pkt_len;
	arg3[2].vtype = VSV_NONVAR_ARG;
	arg3[2].dtype = VSV_INTEGER_ARG;
	arg3[2].data.integerVal = ipg;
	vsv_CallTask(conn_client,"vsv_writePkt_start",VSV_WAIT,3,arg3);

	arg1[0].vtype = VSV_NONVAR_ARG;
	arg1[0].dtype = VSV_INTEGER_ARG;
	for ( num=0; num<pkt_len-4; num+=4){
		data32 = (uint32) (pkt_load[num+3]<<24) | (pkt_load[num+2]<<16) |
		                  (pkt_load[num+1]<< 8) | (pkt_load[num]);
		arg1[0].data.integerVal = (int)data32;
		vsv_CallTask(conn_client,"vsv_writePkt_int",VSV_WAIT,1,arg1);
		len_tmp = len_tmp-4;
	}
	
	arg2[0].vtype = VSV_NONVAR_ARG;
  arg2[0].dtype = VSV_INTEGER_ARG;
	for ( i=0; i<len_tmp; i++){
		/*data8[i] = atoi(pkt_load[num+i]);*/
		data8[i] = (uint8)pkt_load[num+i];
	}
	data32 = (data8[3]<<24) | (data8[2]<<16) | (data8[1]<<8) | data8[0];
	arg2[0].data.integerVal = (int)data32;
	arg2[1].vtype = VSV_NONVAR_ARG;
	arg2[1].dtype = VSV_INTEGER_ARG;
	arg2[1].data.integerVal = len_tmp;
	vsv_CallTask(conn_client,"vsv_writePkt_last2Q",VSV_WAIT,2,arg2);

}/*end vsv_writePkt2Q*/

void vsv_schOutputPkt(void){
	vsvArgT arg;

  arg.vtype = VSV_NONVAR_ARG;
  arg.dtype = VSV_INTEGER_ARG;
  vsv_CallTask(conn_client, "vsv_schOutputPkt", VSV_WAIT, 0, &arg);
}



void vsv_writePkt(int port_id, int pkt_len, unsigned char *pkt_load, int ipg){
  vsvArgT arg3[3],arg1[1],arg2[2];
	int num,i,len_tmp;
	uint32 data32;
	uint32 data8[4]={0,0,0,0};

#if 0 /* for debug */
printf("-----------  vsv_writePkt -----------\n");
printf("port:%2d, pktlen:%3d, ipg:%2d\n",port_id,pkt_len,ipg);
printf("p_load: ");
for(i=0;i<pkt_len;i++)
printf("%x ",(uint8)pkt_load[i]);
printf("\n-----------  vsv_writePkt -----------\n");
#endif

	/* printf("vsv_writePkt: port:%2d, pktlen:%3d, ipg:%2d\n",port_id,pkt_len,ipg); */
	len_tmp = pkt_len;
	arg3[0].vtype = VSV_NONVAR_ARG;
	arg3[0].dtype = VSV_INTEGER_ARG;
	arg3[0].data.integerVal = port_id;
	arg3[1].vtype = VSV_NONVAR_ARG;
	arg3[1].dtype = VSV_INTEGER_ARG;
	arg3[1].data.integerVal = pkt_len;
	arg3[2].vtype = VSV_NONVAR_ARG;
	arg3[2].dtype = VSV_INTEGER_ARG;
	arg3[2].data.integerVal = ipg;
	vsv_CallTask(conn_client,"vsv_writePkt_start",VSV_WAIT,3,arg3);

	arg1[0].vtype = VSV_NONVAR_ARG;
	arg1[0].dtype = VSV_INTEGER_ARG;
	for ( num=0; num<pkt_len-4; num+=4){
		data32 = (uint32) (pkt_load[num+3]<<24) | (pkt_load[num+2]<<16) |
		                  (pkt_load[num+1]<< 8) | (pkt_load[num]);
		arg1[0].data.integerVal = (int)data32;
		vsv_CallTask(conn_client,"vsv_writePkt_int",VSV_WAIT,1,arg1);
		len_tmp = len_tmp-4;
	}
	
	arg2[0].vtype = VSV_NONVAR_ARG;
  arg2[0].dtype = VSV_INTEGER_ARG;
	for ( i=0; i<len_tmp; i++){
		/*data8[i] = atoi(pkt_load[num+i]);*/
		data8[i] = (uint8)pkt_load[num+i];
	}
	data32 = (data8[3]<<24) | (data8[2]<<16) | (data8[1]<<8) | data8[0];
	arg2[0].data.integerVal = (int)data32;
	arg2[1].vtype = VSV_NONVAR_ARG;
	arg2[1].dtype = VSV_INTEGER_ARG;
	arg2[1].data.integerVal = len_tmp;
	vsv_CallTask(conn_client,"vsv_writePkt_last",VSV_WAIT,2,arg2);

}/*end vsv_writePkt*/


/*
 * return value: pkt len, if <0, error occur, queue empty
 * ASSUME: pkt_load must be pointed to a buffer with 16KB for jumbo.
*/
int vsv_readPkt(int port_id, unsigned char* pkt_load){
	int i,pkt_len,num,len_tmp;
	vsvArgT arg1[1], returnVal;
	
	/* rtlglue_printf( "vsv_readPkt(port_id=%d)\n", port_id ); */
	arg1[0].vtype = VSV_NONVAR_ARG;
	arg1[0].dtype = VSV_INTEGER_ARG;
	arg1[0].data.integerVal = port_id;
	returnVal.vtype = VSV_VAR_ARG;
	returnVal.dtype = VSV_INTEGER_ARG;
	vsv_CallFunc(conn_client,"vsv_readPkt_start",&returnVal,1,arg1);

	if ( returnVal.data.integerVal < 0 ){
		/* printf("vsv_readPkt : queue empty\n"); */
		return -1;
	}else{
		 printf("vsv_readPkt : p len= %2d\n",returnVal.data.integerVal); 
		pkt_len = returnVal.data.integerVal;
	}

	/* pkt_load = (char *) rtlglue_malloc(sizeof(char)*pkt_len); */
	len_tmp = pkt_len;
	for(num=0; num<pkt_len-4; num+=4){
		arg1[0].vtype = VSV_NONVAR_ARG;
		arg1[0].dtype = VSV_INTEGER_ARG;
		arg1[0].data.integerVal = 4;
		vsv_CallFunc(conn_client,"vsv_readPkt_int",&returnVal,1,arg1);
		for (i=0; i<4; i++)
			pkt_load[num+i] = (char) ((returnVal.data.integerVal >> (i*8)) & 0xff);
		len_tmp = len_tmp-4;
	}
	
	arg1[0].data.integerVal =len_tmp;
	vsv_CallFunc(conn_client,"vsv_readPkt_int",&returnVal,1,arg1);
	for (i=0;i<len_tmp; i++)
		pkt_load[num+i] = (char) ((returnVal.data.integerVal>>(i*8)) & 0xff);

	return pkt_len;
}/*end vsv_readPkt*/




