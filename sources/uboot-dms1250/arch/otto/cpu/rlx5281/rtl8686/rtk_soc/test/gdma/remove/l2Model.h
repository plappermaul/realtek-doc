/*
* Copyright c                  Realtek Semiconductor Corporation, 2002  
* All rights reserved.
* 
* Program : Header File for Layer2 Model Code
* Abstract : 
* Author : Louis Yung-Chieh Lo (yjlou@realtek.com.tw)               
* $Id: l2Model.h,v 1.22 2006-08-09 06:40:07 yjlou Exp $
*/

#ifndef _L2_MODEL_
#define _L2_MODEL_

enum MODEL_RETURN_VALUE model802_1dAddress( hsb_param_t* hsb, hsa_param_t* hsa,ale_data_t *ale ) ;
enum MODEL_RETURN_VALUE modelSpanningTree( hsb_param_t* hsb, hsa_param_t* hsa,ale_data_t *ale );
enum MODEL_RETURN_VALUE modelIngressCheck( hsb_param_t* hsb, hsa_param_t* hsa, ale_data_t *ale );
enum MODEL_RETURN_VALUE modelEgressCheck( hsb_param_t* hsb, hsa_param_t* hsa , ale_data_t *ale);
enum MODEL_RETURN_VALUE modelIngress802_1X( hsb_param_t* hsb, hsa_param_t* hsa ,ale_data_t *ale);
enum MODEL_RETURN_VALUE modelEgress802_1X( hsb_param_t* hsb, hsa_param_t* hsa , ale_data_t *ale);
enum MODEL_RETURN_VALUE modelLayer2Switching( hsb_param_t* hsb, hsa_param_t* hsa ,ale_data_t*);
enum MODEL_RETURN_VALUE modelsrcMacUnauthRouting(void);
void modelCPUport(hsb_param_t* hsb,hsa_param_t* hsa,ale_data_t *ale);
void modelDrop(hsb_param_t* hsb,hsa_param_t* hsa,ale_data_t *ale);
void modelAddCPUport(hsb_param_t* hsb,hsa_param_t* hsa);
int32 modelGetSrcVlanID(void);
int32 modelSetPriroity(uint32 idx,uint8 priority,ale_data_t *ale);
int32 modelGetNetIf(void *);
enum MODEL_RETURN_VALUE modelDpcCounter( hsb_param_t* hsb, hsa_param_t* hsa );
enum MODEL_RETURN_VALUE modelCheckIgnore1QTag(hsb_param_t*hsb,hsa_param_t*hsa,ale_data_t *ale);
extern uint8 fidHash[];

typedef struct {
    macaddr_t       mac;
    uint16          isStatic    : 1;
    uint16          hPriority   : 1;
    uint16          toCPU       : 1;
    uint16          srcBlock    : 1;
    uint16          nxtHostFlag : 1;
    uint16          reserv0     : 11;
    uint32          memberPort;
    uint32          agingTime;
} rtl_l2_param_t;


enum MODEL_8021D_STATUS
{
	M1D_DISABLE=0,
	M1D_BLOCKING,
	M1D_LEARNING,
	M1D_FORWARD,
};
#define   PRI_PBP		0
#define 	PRI_1Q		1
#define   PRI_DSCP	2
#define   PRI_ACL		3
#define   PRI_NAPT	4
#endif
