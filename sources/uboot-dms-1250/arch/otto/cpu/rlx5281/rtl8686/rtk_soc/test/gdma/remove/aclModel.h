/*
* Copyright c                  Realtek Semiconductor Corporation, 2002  
* All rights reserved.
* 
* Program : Header File for Layer3/4 Model Code
* Abstract : 
* Author : Louis Yung-Chieh Lo (yjlou@realtek.com.tw)               
* $Id: aclModel.h,v 1.7 2006-07-13 15:58:51 chenyl Exp $
*/

#ifndef _ACL_MODEL_
#define _ACL_MODEL_
enum MODEL_RETURN_VALUE modelIngressACLCheck( hsb_param_t* hsb, hsa_param_t* hsa, ale_data_t *ale );
enum MODEL_RETURN_VALUE modelAfterL34IngressACLCheck( hsb_param_t* hsb, hsa_param_t* hsa, ale_data_t *ale );
enum MODEL_RETURN_VALUE modelEgressACLCheck( hsb_param_t* hsb, hsa_param_t* hsa, ale_data_t *ale );
#endif

