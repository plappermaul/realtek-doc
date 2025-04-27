/*
* Copyright c                  Realtek Semiconductor Corporation, 2002  
* All rights reserved.
* 
* Program : Header File for Layer3/4 Model Code
* Abstract : 
* Author : Louis Yung-Chieh Lo (yjlou@realtek.com.tw)               
* $Id: l34Model.h,v 1.20 2006-08-09 07:52:09 yjlou Exp $
*/

#ifndef _L34_MODEL_
#define _L34_MODEL_

enum MODEL_RETURN_VALUE modelLayer34Switching( hsb_param_t* hsb, hsa_param_t* hsa, ale_data_t *ale );
enum MODEL_ACTION_VALUE modelLayer4Switching( hsb_param_t* hsb, hsa_param_t* hsa ,uint8, ale_data_t *ale);
enum MODEL_ACTION_VALUE
{
	/* 0*/	MACT_FORWARD = 0,
	/* 1*/	MACT_L3ROUTING,
	/* 2*/	MACT_TOCPU,
	/* 3*/    MACT_DROP,
};
enum SERVERP_PROTO_VALUE
{
		SERVERP_INVALID=0,
		SERVERP_PROTTCP,
		SERVERP_PROTUDP,
		SERVERP_BOTH,
};
enum HSA_PPPOE_INFO
{
	 HSA_PPPOE_INTACT,
	 HSA_PPPOE_TAGGING,
	 HSA_PPPOE_REMOVE_TAG,
	 HSA_PPPOE_MODIFY,
};

enum NEXTHOPALGO
{
	 ALGO_RR_BASE,
	 ALGO_SESSION_BASE,
	 ALGO_SOURCE_BASE,
};

enum MODEL_ACTION_VALUE modelMatchInternalServerPort( hsb_param_t* hsb, hsa_param_t* hsa, ale_data_t *ale);
enum MODEL_ACTION_VALUE modelMatchExternalServerPort( hsb_param_t* hsb, hsa_param_t* hsa, ale_data_t *ale);
enum MODEL_RETURN_VALUE modelAlgCheck( hsb_param_t* hsb, hsa_param_t* hsa,int, ale_data_t *ale  );
enum MODEL_BOOLEAN_VALUE modelIgnoreEgressCheck(void );
enum MODEL_RETURN_VALUE modelHandleNexthop( hsb_param_t* hsb, hsa_param_t* hsa, ale_data_t *ale );
enum MODEL_RETURN_VALUE modelHandleRedirect( hsb_param_t* hsb, hsa_param_t* hsa, ale_data_t *ale ,int idx);
int32  modelLayer3TTL(hsb_param_t*,hsa_param_t*);
#endif

