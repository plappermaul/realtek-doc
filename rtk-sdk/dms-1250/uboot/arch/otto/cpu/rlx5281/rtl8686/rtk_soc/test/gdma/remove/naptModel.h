/*
* Copyright c                  Realtek Semiconductor Corporation, 2002  
* All rights reserved.
* 
* Program : Header File for Layer2 Model Code
* Abstract : 
* Author : Louis Yung-Chieh Lo (yjlou@realtek.com.tw)               
* $Id: naptModel.h,v 1.5 2006-09-07 09:10:24 evinlien Exp $
*/

#ifndef _NAPT_MODEL_H_
#define _NAPT_MODEL_H_

enum MODEL_RETURN_VALUE modelLayer4NaptLanWanProcessing( hsb_param_t* hsb, hsa_param_t* hsa,  ale_data_t *ale);
enum MODEL_RETURN_VALUE modelLayer4NaptWanLanProcessing( hsb_param_t* hsb, hsa_param_t* hsa, ale_data_t *ale);
int16 cvtHostType2PHReason(ale_data_t *ale, hsb_param_t* hsb);
#endif /* _NAPT_MODEL_H_ */

