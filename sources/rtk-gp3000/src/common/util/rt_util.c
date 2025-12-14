/*
 * Copyright (C) 2009 Realtek Semiconductor Corp. 
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated, 
 * modified or distributed under the authorized license from Realtek. 
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER 
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED. 
 *
 * Purpose : Define the utility macro and function in the SDK.
 *
 * Feature : SDK common utility
 *
 */

/*  
 * Include Files 
 */
#include <common/util/rt_util.h>
#include <common/rt_error.h>



/* 
 * Symbol Definition 
 */


/* 
 * Data Declaration 
 */


/*
 * Macro Definition
 */

/* 
 * Function Declaration 
 */
/* Function Name:
 *      rt_util_macCmp
 * Description:
 *      Compare two mac address
 * Input:
 *      mac1    - mac address 1
 *      mac2    - mac address 2
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - two address is same
 *      RT_ERR_FAILED       - two address is different
 * Note:
 */
int32
rt_util_macCmp(const uint8 *mac1, const uint8 *mac2)
{
    if (memcmp(mac1, mac2, ETHER_ADDR_LEN) == 0)
        return RT_ERR_OK;
    else
        return RT_ERR_FAILED;

}

#ifndef CONFIG_SDK_KERNEL_LINUX
#include <stdio.h>
#include <math.h>
double __log10_subfunction(double cz, int n)
{
	int i;
	double temp2;
	temp2=cz;
	for(i=0;i<n;i++)
		temp2*=cz;
	return temp2/(n+1);
}

#define __log10_pecision	30
double __log10(double z)
{
	int i,count=0;
	double temp = 0,cz;

	if(z==0.0)
		return -INFINITY;

	if(z==1)
		return 0;

	while(z<1)
	{
		z*=10;
		count--;
	}

	while(z>1)
	{
		z/=10;
		count++;
	}

	cz=(z-1.0)/(z+1.0);

	for(i=1;i<__log10_pecision;i+=2)
		temp+=__log10_subfunction(cz,i-1);

	return (temp*2/M_LN10)+count;
}

void
_get_data_by_type(rtk_transceiver_parameter_type_t type, rtk_transceiver_data_t *pSrcData, rtk_transceiver_data_t *pDstData)
{
    double tmp = 0;
	int size = 0;
    switch(type)
    {
		case RTK_TRANSCEIVER_PARA_TYPE_VENDOR_NAME:
		case RTK_TRANSCEIVER_PARA_TYPE_SN:
		case RTK_TRANSCEIVER_PARA_TYPE_VENDOR_PART_NUM:
			pSrcData->buf[TRANSCEIVER_LEN-1]='\0';
			osal_memcpy(pDstData, pSrcData, sizeof(rtk_transceiver_data_t));
			break;
		case RTK_TRANSCEIVER_PARA_TYPE_TEMPERATURE:
			if (128 >= pSrcData->buf[0]) //MSB: pSrcData->buf[0]; LSB: pSrcData->buf[1]
			{
				tmp = (-1)*((~(pSrcData->buf[0]))+1)+((double)(pSrcData->buf[1])*1/256);
				size = snprintf(NULL, 0, "%f C", tmp);
				snprintf(pDstData->buf,  size+1, "%f C", tmp);
			}else{
				tmp = (char)pSrcData->buf[0]+((double)(pSrcData->buf[1])*1/256);
				size = snprintf(NULL, 0, "%f C", tmp);
				snprintf(pDstData->buf, size+1, "%f C", tmp);
			}
			break;
		case RTK_TRANSCEIVER_PARA_TYPE_VOLTAGE:
			tmp = (double)((pSrcData->buf[0] << 8) | pSrcData->buf[1])*1/10000;
			size = snprintf(NULL, 0, "%f V", tmp);
			snprintf(pDstData->buf, size+1, "%f V", tmp);
			break;
		case RTK_TRANSCEIVER_PARA_TYPE_BIAS_CURRENT:
			tmp = (double)((pSrcData->buf[0] << 8) | pSrcData->buf[1])*2/1000;
			size = snprintf(NULL, 0, "%f mA", tmp);
			snprintf(pDstData->buf, size+1, "%f mA", tmp);
			break;
		case RTK_TRANSCEIVER_PARA_TYPE_TX_POWER:
		case RTK_TRANSCEIVER_PARA_TYPE_RX_POWER:
			tmp = __log10(((double)((pSrcData->buf[0] << 8) | pSrcData->buf[1])*(pSrcData->tx_pwr_scale)/10000))*10;
			size = snprintf(NULL, 0, "%f  dBm", tmp);
			snprintf(pDstData->buf, size+1, "%f  dBm", tmp);
			break;
		default:
			snprintf(pDstData->buf, strlen("N/A") + 1, "N/A");
	}

	return;
}
#endif

#if defined(CONFIG_OPENWRT_DIAG)
#include <osal/sem.h>
/* Function Name:
 *      osal_sem_mutex_create
 * Description:
 *      Create one mutex.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      Mutex id
 * Note:
 *      It is NOT a reentrant mutex.
 */
osal_mutex_t
osal_sem_mutex_create(void)
{
    return 0;
} /* end of osal_sem_mutex_create */
#endif
