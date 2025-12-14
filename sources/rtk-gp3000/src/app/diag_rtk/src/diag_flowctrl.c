/*
* This program is the proprietary software of Realtek Semiconductor
* Corporation and/or its licensors, and only be used, duplicated,
* modified or distributed under the authorized license from Realtek.
*
* ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
* THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
*
* $Revision$
* $Date$
*
* Purpose : Definition those XXX command and APIs in the SDK diagnostic shell.
*
* Feature : The file have include the following module and sub-modules
*
*/

/*
 * Include Files
 */

#include <stdio.h>
#include <string.h>
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <diag_util.h>
#include <diag_str.h>
#include <parser/cparser_priv.h>
//#include <dal/apollo/raw/apollo_raw_flowctrl.h>
#include <dal/apollomp/raw/apollomp_raw_flowctrl.h>
#include <dal/rtl9601b/dal_rtl9601b_flowctrl.h>
#include <dal/rtl9602c/dal_rtl9602c_flowctrl.h>
#include <dal/rtl9607c/dal_rtl9607c_flowctrl.h>
#include <dal/rtl9603cvd/dal_rtl9603cvd_flowctrl.h>

#if 0
#if defined(CONFIG_SDK_RTL9601B)
#include <dal/rtl9601b/dal_rtl9601b_pbo.h>
#define CID rtl9601b
#define UPPER_CASE_CID RTL9601B
#endif

#if defined(CONFIG_SDK_RTL9602C)
#include <dal/rtl9602c/dal_rtl9602c_pbo.h>
#define CID  rtl9602c
#define UPPER_CASE_CID RTL9602C
#endif

#if defined(CONFIG_SDK_RTL9607C)
#include <dal/rtl9607c/dal_rtl9607c_pbo.h>
#define CID  rtl9607c
#define UPPER_CASE_CID RTL9607C
#endif

#if defined(CONFIG_SDK_RTL9603CVD)
#include <dal/rtl9603cvd/dal_rtl9603cvd_pbo.h>
#define CID  rtl9603cvd
#define UPPER_CASE_CID RTL9603CVD
#endif

#if CONFIG_GPON_VERSION < 2
#include <rtk/gpon.h>
#else
#include <module/gpon/gpon.h>
#endif

#if defined(CONFIG_SDK_APOLLO)
#define CID apollo
#define UPPER_CASE_CID APOLLO
#endif

#if defined(CONFIG_SDK_APOLLOMP)
#define CID apollomp
#define UPPER_CASE_CID APOLLOMP
#endif

#define USED_PAGE_MAX_LEN 9

static int32 unsupported(void)
{
    return RT_ERR_FEATURE_NOT_SUPPORTED;
}

#ifndef CID
#define _unsupported(arg,args...) unsupported()
#define FC_GLUE(x,y) _unsupported 
#else 
#define _FC_GLUE(x, y) x##y 
#define FC_GLUE(x, y) _FC_GLUE(x, y) 
#endif 
#else 
#include <dal/rtl9601b/dal_rtl9601b_pbo.h>
#include <dal/rtl9602c/dal_rtl9602c_pbo.h>
#include <dal/rtl9607c/dal_rtl9607c_pbo.h>
#include <dal/rtl9603cvd/dal_rtl9603cvd_pbo.h>
#if CONFIG_GPON_VERSION < 2
#include <rtk/gpon.h>
#else
#include <module/gpon/gpon.h>
#endif
#define USED_PAGE_MAX_LEN 9
#endif

typedef struct diag_fc_glue_s
{
	uint32  family_id;
	int32   (*_raw_flowctrl_ponUsedPage_get)(uint32, uint32 *, uint32 *);
    int32   (*_raw_pbo_usUsedPageCount_get)(uint32 *, uint32 *);
    int32   (*_raw_pbo_usMaxUsedPageCount_get)(uint32 *, uint32 *);
	int32   (*_raw_pbo_dsUsedPageCount_get)(uint32 *, uint32 *);
    int32   (*_raw_pbo_dsMaxUsedPageCount_get)(uint32 *, uint32 *);
 	int32   (*_raw_pbo_dsQUsedPageCount_get)(uint32, uint32 *);
    int32   (*_raw_pbo_dsQMaxUsedPageCount_get)(uint32, uint32 *);
    int32   (*_raw_flowctrl_totalUsedPageCnt_get)(uint32 *, uint32 *);
	int32   (*_raw_flowctrl_publicUsedPageCnt_get)(uint32 *, uint32 *);
    int32   (*_raw_flowctrl_publicFcoffUsedPageCnt_get)(uint32 *, uint32 *);
    int32   (*_raw_flowctrl_publicJumboUsedPageCnt_get)(uint32 *, uint32 *);
	int32   (*_raw_flowctrl_portUsedPageCnt_get)(uint32, uint32 *, uint32 *);
    int32   (*_raw_flowctrl_swpboPortUsedUsedPage_get)(uint32 *, uint32 *);
    int32   (*_raw_flowctrl_portEgressUsedPageCnt_get)(uint32, uint32 *, uint32 *);
	int32   (*_raw_flowctrl_queueEgressUsedPageCnt_get)(uint32, uint32, uint32 *, uint32 *);
    int32   (*_raw_flowctrl_swpboEgressUsedPage_get)(uint32 *, uint32 *);
    int32   (*_raw_flowctrl_swpboEgressQueueUsedPage_get)(uint32 *, uint32 *);

} diag_fc_glue_t;


static diag_fc_glue_t diag_fc_glue_mapper[] =
{
#if defined(CONFIG_SDK_APOLLO)
	{
	APOLLO_CHIP_ID,	
	._raw_flowctrl_ponUsedPage_get 				= apollo_raw_flowctrl_ponUsedPage_get,
    ._raw_pbo_usUsedPageCount_get 				= NULL,
    ._raw_pbo_usMaxUsedPageCount_get 			= NULL,
	._raw_pbo_dsUsedPageCount_get 				= NULL,
    ._raw_pbo_dsMaxUsedPageCount_get 			= NULL,
 	._raw_pbo_dsQUsedPageCount_get 				= NULL,
    ._raw_pbo_dsQMaxUsedPageCount_get 			= NULL,
    ._raw_flowctrl_totalUsedPageCnt_get 		= apollo_raw_flowctrl_totalUsedPageCnt_get,
	._raw_flowctrl_publicUsedPageCnt_get 		= apollo_raw_flowctrl_publicUsedPageCnt_get,
    ._raw_flowctrl_publicFcoffUsedPageCnt_get 	= apollo_raw_flowctrl_publicFcoffUsedPageCnt_get,
    ._raw_flowctrl_publicJumboUsedPageCnt_get 	= apollo_raw_flowctrl_publicJumboUsedPageCnt_get,
	._raw_flowctrl_portUsedPageCnt_get 			= apollo_raw_flowctrl_portUsedPageCnt_get, 
    ._raw_flowctrl_swpboPortUsedUsedPage_get 	= NULL, 
    ._raw_flowctrl_portEgressUsedPageCnt_get 	= apollo_raw_flowctrl_portEgressUsedPageCnt_get,
	._raw_flowctrl_queueEgressUsedPageCnt_get 	= apollo_raw_flowctrl_queueEgressUsedPageCnt_get, 
    ._raw_flowctrl_swpboEgressUsedPage_get 		= NULL,
    ._raw_flowctrl_swpboEgressQueueUsedPage_get = NULL,

	},
	
#endif

#if defined(CONFIG_SDK_APOLLOMP)
	{
	APOLLOMP_CHIP_ID,	
	._raw_flowctrl_ponUsedPage_get 				= apollomp_raw_flowctrl_ponUsedPage_get,
    ._raw_pbo_usUsedPageCount_get 				= NULL,
    ._raw_pbo_usMaxUsedPageCount_get 			= NULL,
	._raw_pbo_dsUsedPageCount_get 				= NULL,
    ._raw_pbo_dsMaxUsedPageCount_get 			= NULL,
 	._raw_pbo_dsQUsedPageCount_get 				= NULL,
    ._raw_pbo_dsQMaxUsedPageCount_get 			= NULL,
    ._raw_flowctrl_totalUsedPageCnt_get 		= apollomp_raw_flowctrl_totalUsedPageCnt_get,
	._raw_flowctrl_publicUsedPageCnt_get 		= apollomp_raw_flowctrl_publicUsedPageCnt_get,
    ._raw_flowctrl_publicFcoffUsedPageCnt_get 	= apollomp_raw_flowctrl_publicFcoffUsedPageCnt_get,
    ._raw_flowctrl_publicJumboUsedPageCnt_get 	= apollomp_raw_flowctrl_publicJumboUsedPageCnt_get,
	._raw_flowctrl_portUsedPageCnt_get 			= apollomp_raw_flowctrl_portUsedPageCnt_get, 
    ._raw_flowctrl_swpboPortUsedUsedPage_get 	= NULL, 
    ._raw_flowctrl_portEgressUsedPageCnt_get 	= apollomp_raw_flowctrl_portEgressUsedPageCnt_get,
	._raw_flowctrl_queueEgressUsedPageCnt_get 	= apollomp_raw_flowctrl_queueEgressUsedPageCnt_get, 
    ._raw_flowctrl_swpboEgressUsedPage_get 		= NULL,
    ._raw_flowctrl_swpboEgressQueueUsedPage_get = NULL,

	},
#endif

#if defined(CONFIG_SDK_RTL9601B)
	{
	RTL9601B_CHIP_ID,
	._raw_flowctrl_ponUsedPage_get 				= rtl9601b_raw_flowctrl_ponUsedPage_get,
    ._raw_pbo_usUsedPageCount_get 				= NULL,
    ._raw_pbo_usMaxUsedPageCount_get 			= NULL,
	._raw_pbo_dsUsedPageCount_get 				= NULL,
    ._raw_pbo_dsMaxUsedPageCount_get 			= NULL,
 	._raw_pbo_dsQUsedPageCount_get 				= NULL,
    ._raw_pbo_dsQMaxUsedPageCount_get 			= NULL,
    ._raw_flowctrl_totalUsedPageCnt_get 		= rtl9601b_raw_flowctrl_totalUsedPageCnt_get,
	._raw_flowctrl_publicUsedPageCnt_get 		= rtl9601b_raw_flowctrl_publicUsedPageCnt_get,
    ._raw_flowctrl_publicFcoffUsedPageCnt_get 	= rtl9601b_raw_flowctrl_publicFcoffUsedPageCnt_get,
    ._raw_flowctrl_publicJumboUsedPageCnt_get 	= NULL,
	._raw_flowctrl_portUsedPageCnt_get 			= rtl9601b_raw_flowctrl_portUsedPageCnt_get, 
    ._raw_flowctrl_swpboPortUsedUsedPage_get 	= NULL, 
    ._raw_flowctrl_portEgressUsedPageCnt_get 	= rtl9601b_raw_flowctrl_portEgressUsedPageCnt_get,
	._raw_flowctrl_queueEgressUsedPageCnt_get 	= rtl9601b_raw_flowctrl_queueEgressUsedPageCnt_get, 
    ._raw_flowctrl_swpboEgressUsedPage_get 		= NULL,
    ._raw_flowctrl_swpboEgressQueueUsedPage_get = NULL,

	},
#endif

#if defined(CONFIG_SDK_RTL9602C)
	{
	RTL9602C_CHIP_ID,
	._raw_flowctrl_ponUsedPage_get 				= rtl9602c_raw_flowctrl_ponUsedPage_get,
    ._raw_pbo_usUsedPageCount_get 				= rtl9602c_raw_pbo_usUsedPageCount_get,
    ._raw_pbo_usMaxUsedPageCount_get 			= rtl9602c_raw_pbo_usMaxUsedPageCount_get,
	._raw_pbo_dsUsedPageCount_get 				= rtl9602c_raw_pbo_dsUsedPageCount_get,
    ._raw_pbo_dsMaxUsedPageCount_get 			= rtl9602c_raw_pbo_dsMaxUsedPageCount_get,
 	._raw_pbo_dsQUsedPageCount_get 				= NULL,
    ._raw_pbo_dsQMaxUsedPageCount_get 			= NULL,
    ._raw_flowctrl_totalUsedPageCnt_get 		= rtl9602c_raw_flowctrl_totalUsedPageCnt_get,
	._raw_flowctrl_publicUsedPageCnt_get 		= rtl9602c_raw_flowctrl_publicUsedPageCnt_get,
    ._raw_flowctrl_publicFcoffUsedPageCnt_get 	= rtl9602c_raw_flowctrl_publicFcoffUsedPageCnt_get,
    ._raw_flowctrl_publicJumboUsedPageCnt_get 	= NULL,
	._raw_flowctrl_portUsedPageCnt_get 			= rtl9602c_raw_flowctrl_portUsedPageCnt_get, 
    ._raw_flowctrl_swpboPortUsedUsedPage_get 	= NULL, 
    ._raw_flowctrl_portEgressUsedPageCnt_get 	= rtl9602c_raw_flowctrl_portEgressUsedPageCnt_get,
	._raw_flowctrl_queueEgressUsedPageCnt_get 	= rtl9602c_raw_flowctrl_queueEgressUsedPageCnt_get, 
    ._raw_flowctrl_swpboEgressUsedPage_get 		= NULL,
    ._raw_flowctrl_swpboEgressQueueUsedPage_get = NULL,

	},
#endif

#if defined(CONFIG_SDK_RTL9607C)
    {
	RTL9607C_CHIP_ID,
	._raw_flowctrl_ponUsedPage_get 				= rtl9607c_raw_flowctrl_ponUsedPage_get,
    ._raw_pbo_usUsedPageCount_get 				= rtl9607c_raw_pbo_usUsedPageCount_get,
    ._raw_pbo_usMaxUsedPageCount_get 			= rtl9607c_raw_pbo_usMaxUsedPageCount_get,
	._raw_pbo_dsUsedPageCount_get 				= rtl9607c_raw_pbo_dsUsedPageCount_get,
    ._raw_pbo_dsMaxUsedPageCount_get 			= rtl9607c_raw_pbo_dsMaxUsedPageCount_get,
 	._raw_pbo_dsQUsedPageCount_get 				= rtl9607c_raw_pbo_dsQUsedPageCount_get,
    ._raw_pbo_dsQMaxUsedPageCount_get 			= rtl9607c_raw_pbo_dsQMaxUsedPageCount_get,
    ._raw_flowctrl_totalUsedPageCnt_get 		= rtl9607c_raw_flowctrl_totalUsedPageCnt_get,
	._raw_flowctrl_publicUsedPageCnt_get 		= rtl9607c_raw_flowctrl_publicUsedPageCnt_get,
    ._raw_flowctrl_publicFcoffUsedPageCnt_get 	= rtl9607c_raw_flowctrl_publicFcoffUsedPageCnt_get,
    ._raw_flowctrl_publicJumboUsedPageCnt_get 	= NULL,
	._raw_flowctrl_portUsedPageCnt_get 			= rtl9607c_raw_flowctrl_portUsedPageCnt_get, 
    ._raw_flowctrl_swpboPortUsedUsedPage_get 	= rtl9607c_raw_flowctrl_swpboPortUsedUsedPage_get, 
    ._raw_flowctrl_portEgressUsedPageCnt_get 	= rtl9607c_raw_flowctrl_portEgressUsedPageCnt_get,
	._raw_flowctrl_queueEgressUsedPageCnt_get 	= rtl9607c_raw_flowctrl_queueEgressUsedPageCnt_get, 
    ._raw_flowctrl_swpboEgressUsedPage_get 		= rtl9607c_raw_flowctrl_swpboEgressUsedPage_get,
    ._raw_flowctrl_swpboEgressQueueUsedPage_get = rtl9607c_raw_flowctrl_swpboEgressQueueUsedPage_get,

	},
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
	{
	RTL9603CVD_CHIP_ID,
	._raw_flowctrl_ponUsedPage_get 				= rtl9603cvd_raw_flowctrl_ponUsedPage_get,
    ._raw_pbo_usUsedPageCount_get 				= rtl9603cvd_raw_pbo_usUsedPageCount_get,
    ._raw_pbo_usMaxUsedPageCount_get 			= rtl9603cvd_raw_pbo_usMaxUsedPageCount_get,
	._raw_pbo_dsUsedPageCount_get 				= rtl9603cvd_raw_pbo_dsUsedPageCount_get,
    ._raw_pbo_dsMaxUsedPageCount_get 			= rtl9603cvd_raw_pbo_dsMaxUsedPageCount_get,
 	._raw_pbo_dsQUsedPageCount_get 				= rtl9603cvd_raw_pbo_dsQUsedPageCount_get,
    ._raw_pbo_dsQMaxUsedPageCount_get 			= rtl9603cvd_raw_pbo_dsQMaxUsedPageCount_get,
    ._raw_flowctrl_totalUsedPageCnt_get 		= rtl9603cvd_raw_flowctrl_totalUsedPageCnt_get,
	._raw_flowctrl_publicUsedPageCnt_get 		= rtl9603cvd_raw_flowctrl_publicUsedPageCnt_get,
    ._raw_flowctrl_publicFcoffUsedPageCnt_get 	= rtl9603cvd_raw_flowctrl_publicFcoffUsedPageCnt_get,
    ._raw_flowctrl_publicJumboUsedPageCnt_get 	= NULL,
	._raw_flowctrl_portUsedPageCnt_get 			= rtl9603cvd_raw_flowctrl_portUsedPageCnt_get, 
    ._raw_flowctrl_swpboPortUsedUsedPage_get 	= rtl9603cvd_raw_flowctrl_swpboPortUsedUsedPage_get, 
    ._raw_flowctrl_portEgressUsedPageCnt_get 	= rtl9603cvd_raw_flowctrl_portEgressUsedPageCnt_get,
	._raw_flowctrl_queueEgressUsedPageCnt_get 	= rtl9603cvd_raw_flowctrl_queueEgressUsedPageCnt_get, 
    ._raw_flowctrl_swpboEgressUsedPage_get 		= rtl9603cvd_raw_flowctrl_swpboEgressUsedPage_get,
    ._raw_flowctrl_swpboEgressQueueUsedPage_get = rtl9603cvd_raw_flowctrl_swpboEgressQueueUsedPage_get,

	},
#endif
	{
	UNKNOWN_CHIP_ID,
	._raw_flowctrl_ponUsedPage_get 				= NULL,
    ._raw_pbo_usUsedPageCount_get 				= NULL,
    ._raw_pbo_usMaxUsedPageCount_get 			= NULL,
	._raw_pbo_dsUsedPageCount_get 				= NULL,
    ._raw_pbo_dsMaxUsedPageCount_get 			= NULL,
 	._raw_pbo_dsQUsedPageCount_get 				= NULL,
    ._raw_pbo_dsQMaxUsedPageCount_get 			= NULL,
    ._raw_flowctrl_totalUsedPageCnt_get 		= NULL,
	._raw_flowctrl_publicUsedPageCnt_get 		= NULL,
    ._raw_flowctrl_publicFcoffUsedPageCnt_get 	= NULL,
    ._raw_flowctrl_publicJumboUsedPageCnt_get 	= NULL,
	._raw_flowctrl_portUsedPageCnt_get 			= NULL, 
    ._raw_flowctrl_swpboPortUsedUsedPage_get 	= NULL, 
    ._raw_flowctrl_portEgressUsedPageCnt_get 	= NULL,
	._raw_flowctrl_queueEgressUsedPageCnt_get 	= NULL, 
    ._raw_flowctrl_swpboEgressUsedPage_get 		= NULL,
    ._raw_flowctrl_swpboEgressQueueUsedPage_get = NULL,

	},
};

#define FC_GLUE_MAPPER_SIZE (sizeof(diag_fc_glue_mapper)/sizeof(diag_fc_glue_t))

static uint32 diag_fc_glue_mapper_idx_get(void)
{
	uint32  mapper_index;
	for (mapper_index = 0; mapper_index < FC_GLUE_MAPPER_SIZE; mapper_index++)
	{
		if (DIAG_UTIL_CHIP_TYPE == diag_fc_glue_mapper[mapper_index].family_id)
		{
			return mapper_index;
		}
	}
	return FC_GLUE_MAPPER_SIZE;
}

static uint32 diag_fc_glue_pbo_dsQ_type_normal0_get(void)
{
	uint32 ret;
	switch(DIAG_UTIL_CHIP_TYPE)
    {
		case RTL9607C_CHIP_ID:
			ret = RTL9607C_PBO_DSQUEUE_TYPE_NORMAL0;
			break;
		case RTL9603CVD_CHIP_ID:
			ret = RTL9603CVD_PBO_DSQUEUE_TYPE_NORMAL0;
			break;
		default:
			ret = UNKNOWN_CHIP_ID;  
	}
	return ret;
}

static uint32 diag_fc_glue_pbo_dsQ_type_normal1_get(void)
{
	uint32 ret;
	switch (DIAG_UTIL_CHIP_TYPE)
	{
		case RTL9607C_CHIP_ID:
			ret = RTL9607C_PBO_DSQUEUE_TYPE_NORMAL1;
			break;
		case RTL9603CVD_CHIP_ID:
			ret = RTL9603CVD_PBO_DSQUEUE_TYPE_NORMAL1; 
			break;
		default:
			ret = UNKNOWN_CHIP_ID;
	}
	return ret;
}

static uint32 diag_fc_glue_pbo_dsQ_type_high_get(void)
{
	uint32 ret;
	switch (DIAG_UTIL_CHIP_TYPE)
	{
		case RTL9607C_CHIP_ID:
			ret = RTL9607C_PBO_DSQUEUE_TYPE_HIGH;
			break;
		case RTL9603CVD_CHIP_ID:
			ret = RTL9603CVD_PBO_DSQUEUE_TYPE_HIGH;
			break;
		default:
			ret = UNKNOWN_CHIP_ID;
	}
	return ret;
}

#define FC_GLUE_MAPPER_INDEX (diag_fc_glue_mapper_idx_get())
#define FC_GLUE_PBO_DSQUEUE_TYPE_NORMAL0 (diag_fc_glue_pbo_dsQ_type_normal0_get())
#define FC_GLUE_PBO_DSQUEUE_TYPE_NORMAL1 (diag_fc_glue_pbo_dsQ_type_normal1_get())
#define FC_GLUE_PBO_DSQUEUE_TYPE_HIGH (diag_fc_glue_pbo_dsQ_type_high_get())

typedef struct _used_page_record_s
{
    char title[32];
    uint32 current;
    uint32 maximum;
    struct _used_page_record_s *next;
} _used_page_record_t;

static _used_page_record_t *p_used_page_rec_head = NULL;

static void _used_page_record_remove(void)
{
    _used_page_record_t *ptr = NULL, *pTmp = NULL;

    if (!p_used_page_rec_head)
        return;

    ptr = p_used_page_rec_head;
    while(ptr)
    {
        pTmp = ptr;
        ptr = ptr->next;
		osal_free(pTmp);
        pTmp = NULL;
    }
    p_used_page_rec_head = NULL;

}

static uint32 _used_page_record_add(_used_page_record_t *pRec)
{
    _used_page_record_t *ptr = NULL;

    if (!pRec)
        return RT_ERR_FAILED;

    if (!p_used_page_rec_head)
    {
        p_used_page_rec_head = pRec;
        p_used_page_rec_head->next = NULL;
    }
    else
    {
        ptr = p_used_page_rec_head;
        while(ptr->next != NULL)
            ptr = ptr->next;

        pRec->next = NULL;
        ptr->next = pRec;
    }

    return RT_ERR_OK;
}

static void _used_page_record_dump(uint32 num)
{
    _used_page_record_t *ptr = NULL;
    _used_page_record_t **ppTmpHead = NULL;
    uint32 idx, row_max, row_idx, col_max = 16;
    uint32  rem, mid;

    if (!p_used_page_rec_head || 0 == num)
        return;

    row_max = (num / col_max) + ((0 == (num % col_max) ? 0 : 1));

    ppTmpHead = (_used_page_record_t**)osal_alloc(sizeof(_used_page_record_t *) * row_max);
    if (!ppTmpHead)
        return;

    ppTmpHead[0] = p_used_page_rec_head;

    for (row_idx = 0; row_idx < row_max; row_idx++)
    {
        ptr = ppTmpHead[row_idx];
        idx = 0;
        rem = 0;
        // print title
        while (ptr && idx < col_max)
        {
            if (USED_PAGE_MAX_LEN < strlen(ptr->title))
                mid = 0;
            else
                mid = ((USED_PAGE_MAX_LEN - strlen(ptr->title)) / 2) + (0 == (USED_PAGE_MAX_LEN - strlen(ptr->title)) % 2  ? 0 : 1);

            diag_util_printf("%*s", (rem + mid + strlen(ptr->title)), ptr->title);

            rem = USED_PAGE_MAX_LEN + 1 - (mid + strlen(ptr->title));
            ptr = ptr->next;
            idx++;
        }

        if (col_max == idx && row_idx+1 < row_max)
        {
            ppTmpHead[row_idx+1] = ptr;
        }
        diag_util_printf("\n");

        ptr = ppTmpHead[row_idx];
        idx = 0;
        // print current / maximum
        while (ptr && idx < col_max)
        {
            diag_util_printf("%4u/%-4u ",  ptr->current, ptr->maximum);

            ptr = ptr->next;
            idx++;
        }
        diag_util_printf("\n");

    }
    diag_util_printf("\n");

    _used_page_record_remove();

    if (ppTmpHead)
    {
        osal_free(ppTmpHead);
        ppTmpHead = NULL;
    }
}

static void _sid_used_page_dump(void)
{
    rtk_qid_t               queue,idx, tmp_queue;
    uint32                  qcurrent[8] = {0}, qmaximum[8] = {0}, flow[8] = {0};
    rtk_enable_t            enable;
    uint32                  flow_id, flow_cnt = 0;
    rtk_gpon_usFlow_attr_t  flow_attr;
    rtk_ponmac_queue_t      ponQ;
    rtk_ponmac_queueCfg_t   queueCfg;
	uint32					m_idx = FC_GLUE_MAPPER_INDEX;

    diag_util_printf("\nPON Egress SID Used Page Count (Current/Maximum)\n");

    for (flow_id=0; flow_id < HAL_MAX_NUM_OF_GPON_FLOW(); flow_id++)
    {
        if (RT_ERR_OK != rtk_gpon_usFlow_get(flow_id, &flow_attr))
            continue;

        if (RT_ERR_OK !=  rtk_ponmac_flow2Queue_get(flow_id ,&ponQ))
            continue;

        if (DIAG_UTIL_CHIP_TYPE !=  APOLLOMP_CHIP_ID &&
            DIAG_UTIL_CHIP_TYPE !=  APOLLO_CHIP_ID &&
           DIAG_UTIL_CHIP_TYPE !=   RTL9601B_CHIP_ID)
        {
            if (RT_ERR_OK != rtk_ponmac_sidValid_get(flow_id, &enable))
                continue;

            if (ENABLED != enable)
                continue;
        }

        if (RT_ERR_OK != rtk_ponmac_queue_get(&ponQ,&queueCfg))
            continue;

        idx = (flow_cnt % 8);
        flow[idx] = flow_id;


		#if 0
        if (RT_ERR_OK != FC_GLUE(CID, _raw_flowctrl_ponUsedPage_get)(flow[idx], &qcurrent[idx], &qmaximum[idx]))
            continue;
		#else
		if (m_idx < FC_GLUE_MAPPER_SIZE && 
			diag_fc_glue_mapper[m_idx]._raw_flowctrl_ponUsedPage_get)
		{
			if (RT_ERR_OK != diag_fc_glue_mapper[m_idx]._raw_flowctrl_ponUsedPage_get(flow[idx], &qcurrent[idx], &qmaximum[idx]))
				continue;
		}
		#endif
        if (7 == idx)
        {
            for (queue = 0; queue < 8; queue++)
            {
                diag_util_printf("%3u:%4u/%-4u  ", flow[queue], qcurrent[queue],  qmaximum[queue]);
            }
            diag_util_printf("\n");

        }
        flow_cnt++;
    }
    tmp_queue = flow_cnt % 8;
    if (flow_cnt != 0 && 0 !=  tmp_queue)
    {
        for (queue = 0; queue < tmp_queue; queue++)
        {
            diag_util_printf("%3u:%4u/%-4u  ", flow[queue], qcurrent[queue],  qmaximum[queue]);
        }
        diag_util_printf("\n");
    }

}

static int32 _pbo_used_page_dump(void)
{
    int32 ret;
    uint32 cur_sram[2], cur_dram[2];
    uint32 max_sram[2], max_dram[2];
    uint32 cur_queue[3], max_queue[3];
    uint32 mid =0, rem = 0;

    diag_util_printf("\nPBO Used Page Count (Current/Maximum)\n");

    osal_memset(cur_sram, 0, sizeof(cur_sram));
    osal_memset(cur_dram, 0, sizeof(cur_dram));
    osal_memset(max_sram, 0, sizeof(max_sram));
    osal_memset(max_dram, 0, sizeof(max_dram));
    osal_memset(cur_queue, 0, sizeof(cur_queue));
    osal_memset(max_queue, 0, sizeof(max_queue));

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9601B)
    case RTL9601B_CHIP_ID:
        DIAG_UTIL_ERR_CHK(rtl9601b_raw_pbo_usedPageCount_get(&cur_sram[0], &cur_dram[0]), ret);
        DIAG_UTIL_ERR_CHK(rtl9601b_raw_pbo_maxUsedPageCount_get(&max_sram[0], &max_dram[0]), ret);

        if (USED_PAGE_MAX_LEN < strlen("US_SRAM"))
            mid = 0;
        else
            mid = ((USED_PAGE_MAX_LEN - strlen("US_SRAM")) / 2) + (0 == (USED_PAGE_MAX_LEN - strlen("US_SRAM")) % 2  ? 0 : 1);

        diag_util_printf("%*s", (rem + mid + strlen("US_SRAM")), "US_SRAM");

        rem = USED_PAGE_MAX_LEN + 1 - (mid + strlen("US_SRAM"));

        diag_util_printf("%*s", (rem + mid + strlen("US_DRAM")), "US_DRAM");

        diag_util_printf( "\n%4u/%-4u ", cur_sram[0],  max_sram[0]);
        diag_util_printf( "%4u/%-4u \n", cur_dram[0], max_dram[0]);

        break;
#endif
#if defined(CONFIG_SDK_RTL9602C)
    case RTL9602C_CHIP_ID:

        DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_usUsedPageCount_get(&cur_sram[0], &cur_dram[0]), ret);
        DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_usMaxUsedPageCount_get(&max_sram[0], &max_dram[0]), ret);
        DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_dsUsedPageCount_get(&cur_sram[1], &cur_dram[1]), ret);
        DIAG_UTIL_ERR_CHK(rtl9602c_raw_pbo_dsMaxUsedPageCount_get(&max_sram[1], &max_dram[1]), ret);

        if (USED_PAGE_MAX_LEN < strlen("US_SRAM"))
            mid = 0;
        else
            mid = ((USED_PAGE_MAX_LEN - strlen("US_SRAM")) / 2) + (0 == (USED_PAGE_MAX_LEN - strlen("US_SRAM")) % 2  ? 0 : 1);

        diag_util_printf("%*s", (rem + mid + strlen("US_SRAM")), "US_SRAM");

        rem = USED_PAGE_MAX_LEN + 1 - (mid + strlen("US_SRAM"));

        diag_util_printf("%*s", (rem + mid + strlen("US_DRAM")), "US_DRAM");
        diag_util_printf("%*s", (rem + mid + strlen("DS_SRAM")), "DS_SRAM");
        diag_util_printf("%*s", (rem + mid + strlen("DS_DRAM")), "DS_DRAM");

        diag_util_printf( "\n%4u/%-4u ", cur_sram[0], max_sram[0]);
        diag_util_printf( "%4u/%-4u ", cur_dram[0], max_dram[0]);
        diag_util_printf( "%4u/%-4u ", cur_sram[1], max_sram[1]);
        diag_util_printf( "%4u/%-4u \n", cur_dram[1], max_dram[1]);
        break;
#endif
#if defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
    case RTL9607C_CHIP_ID:
    case RTL9603CVD_CHIP_ID:
    {
		uint32 m_idx = FC_GLUE_MAPPER_INDEX;
		uint32 normal0_type = FC_GLUE_PBO_DSQUEUE_TYPE_NORMAL0;
		uint32 normal1_type = FC_GLUE_PBO_DSQUEUE_TYPE_NORMAL1;
		uint32 high_type = FC_GLUE_PBO_DSQUEUE_TYPE_HIGH;
		#if 1
		if (m_idx >= FC_GLUE_MAPPER_SIZE ||
			normal0_type == UNKNOWN_CHIP_ID ||
			normal1_type == UNKNOWN_CHIP_ID ||
			high_type == UNKNOWN_CHIP_ID)
			return CPARSER_NOT_OK;

		if (diag_fc_glue_mapper[m_idx]._raw_pbo_usUsedPageCount_get)
			DIAG_UTIL_ERR_CHK(diag_fc_glue_mapper[m_idx]._raw_pbo_usUsedPageCount_get(&cur_sram[0], &cur_dram[0]), ret);
		if (diag_fc_glue_mapper[m_idx]._raw_pbo_usMaxUsedPageCount_get)
			DIAG_UTIL_ERR_CHK(diag_fc_glue_mapper[m_idx]._raw_pbo_usMaxUsedPageCount_get(&max_sram[0], &max_dram[0]), ret);
		if (diag_fc_glue_mapper[m_idx]._raw_pbo_dsUsedPageCount_get)
			DIAG_UTIL_ERR_CHK(diag_fc_glue_mapper[m_idx]._raw_pbo_dsUsedPageCount_get(&cur_sram[1], &cur_dram[1]), ret);
		if (diag_fc_glue_mapper[m_idx]._raw_pbo_dsMaxUsedPageCount_get)
			DIAG_UTIL_ERR_CHK(diag_fc_glue_mapper[m_idx]._raw_pbo_dsMaxUsedPageCount_get(&max_sram[1], &max_dram[1]), ret);
		if (diag_fc_glue_mapper[m_idx]._raw_pbo_dsQUsedPageCount_get)
			DIAG_UTIL_ERR_CHK(diag_fc_glue_mapper[m_idx]._raw_pbo_dsQUsedPageCount_get(normal0_type, &cur_queue[0]), ret);
		if (diag_fc_glue_mapper[m_idx]._raw_pbo_dsQUsedPageCount_get)
			DIAG_UTIL_ERR_CHK(diag_fc_glue_mapper[m_idx]._raw_pbo_dsQUsedPageCount_get(normal1_type, &cur_queue[1]), ret);
		if (diag_fc_glue_mapper[m_idx]._raw_pbo_dsQUsedPageCount_get)
			DIAG_UTIL_ERR_CHK(diag_fc_glue_mapper[m_idx]._raw_pbo_dsQUsedPageCount_get(high_type, &cur_queue[2]), ret);
		if (diag_fc_glue_mapper[m_idx]._raw_pbo_dsQMaxUsedPageCount_get)
			DIAG_UTIL_ERR_CHK(diag_fc_glue_mapper[m_idx]._raw_pbo_dsQMaxUsedPageCount_get(normal0_type, &max_queue[0]), ret);
		if (diag_fc_glue_mapper[m_idx]._raw_pbo_dsQMaxUsedPageCount_get)
			DIAG_UTIL_ERR_CHK(diag_fc_glue_mapper[m_idx]._raw_pbo_dsQMaxUsedPageCount_get(normal1_type, &max_queue[1]), ret);
		if (diag_fc_glue_mapper[m_idx]._raw_pbo_dsQMaxUsedPageCount_get)
			DIAG_UTIL_ERR_CHK(diag_fc_glue_mapper[m_idx]._raw_pbo_dsQMaxUsedPageCount_get(high_type, &max_queue[2]), ret);
		#else
        DIAG_UTIL_ERR_CHK(FC_GLUE(CID, _raw_pbo_usUsedPageCount_get)(&cur_sram[0], &cur_dram[0]), ret);
        DIAG_UTIL_ERR_CHK(FC_GLUE(CID, _raw_pbo_usMaxUsedPageCount_get)(&max_sram[0], &max_dram[0]), ret);
        DIAG_UTIL_ERR_CHK(FC_GLUE(CID, _raw_pbo_dsUsedPageCount_get)(&cur_sram[1], &cur_dram[1]), ret);
        DIAG_UTIL_ERR_CHK(FC_GLUE(CID, _raw_pbo_dsMaxUsedPageCount_get)(&max_sram[1], &max_dram[1]), ret);
        DIAG_UTIL_ERR_CHK(FC_GLUE(CID, _raw_pbo_dsQUsedPageCount_get)(FC_GLUE(UPPER_CASE_CID, _PBO_DSQUEUE_TYPE_NORMAL0), &cur_queue[0]), ret);
        DIAG_UTIL_ERR_CHK(FC_GLUE(CID, _raw_pbo_dsQUsedPageCount_get)(FC_GLUE(UPPER_CASE_CID, _PBO_DSQUEUE_TYPE_NORMAL1), &cur_queue[1]), ret);
        DIAG_UTIL_ERR_CHK(FC_GLUE(CID, _raw_pbo_dsQUsedPageCount_get)(FC_GLUE(UPPER_CASE_CID, _PBO_DSQUEUE_TYPE_HIGH), &cur_queue[2]), ret);
        DIAG_UTIL_ERR_CHK(FC_GLUE(CID, _raw_pbo_dsQMaxUsedPageCount_get)(FC_GLUE(UPPER_CASE_CID, _PBO_DSQUEUE_TYPE_NORMAL0), &max_queue[0]), ret);
        DIAG_UTIL_ERR_CHK(FC_GLUE(CID, _raw_pbo_dsQMaxUsedPageCount_get)(FC_GLUE(UPPER_CASE_CID, _PBO_DSQUEUE_TYPE_NORMAL1), &max_queue[1]), ret);
        DIAG_UTIL_ERR_CHK(FC_GLUE(CID, _raw_pbo_dsQMaxUsedPageCount_get)(FC_GLUE(UPPER_CASE_CID, _PBO_DSQUEUE_TYPE_HIGH), &max_queue[2]), ret);
		#endif
        if (USED_PAGE_MAX_LEN < strlen("US_SRAM"))
            mid = 0;
        else
            mid = ((USED_PAGE_MAX_LEN - strlen("US_SRAM")) / 2) + (0 == (USED_PAGE_MAX_LEN - strlen("US_SRAM")) % 2  ? 0 : 1);

        diag_util_printf("%*s", (rem + mid + strlen("US_SRAM")), "US_SRAM");

        rem = USED_PAGE_MAX_LEN + 1 - (mid + strlen("US_SRAM"));

        diag_util_printf("%*s", (rem + mid + strlen("US_DRAM")), "US_DRAM");
        diag_util_printf("%*s", (rem + mid + strlen("DS_SRAM")), "DS_SRAM");
        diag_util_printf("%*s", (rem + mid + strlen("DS_DRAM")), "DS_DRAM");

        if (USED_PAGE_MAX_LEN <= strlen("Normal_Q0"))
            mid = 0;
        else
            mid = ((USED_PAGE_MAX_LEN - strlen("Normal_Q0")) / 2) + (0 == (USED_PAGE_MAX_LEN - strlen("Normal_Q0")) % 2  ? 0 : 1);

        diag_util_printf("%*s", (rem + mid + strlen("Normal_Q0")), "Normal_Q0");

        rem = USED_PAGE_MAX_LEN + 1 - (mid + strlen("Normal_Q0"));

        diag_util_printf("%*s", (rem + mid + strlen("Normal_Q1")), "Normal_Q1");


        if (USED_PAGE_MAX_LEN <= strlen("High_Q"))
            mid = 0;
        else
            mid = ((USED_PAGE_MAX_LEN - strlen("High_Q")) / 2);

        diag_util_printf("%*s", (rem + mid + strlen("High_Q")), "High_Q");

        diag_util_printf( "\n%4u/%-4u ", cur_sram[0], max_sram[0]);
        diag_util_printf( "%4u/%-4u ", cur_dram[0], max_dram[0]);
        diag_util_printf( "%4u/%-4u ", cur_sram[1], max_sram[1]);
        diag_util_printf( "%4u/%-4u ", cur_dram[1], max_dram[1]);
        diag_util_printf( "%4u/%-4u ", cur_queue[0], max_queue[0]);
        diag_util_printf( "%4u/%-4u ", cur_queue[1], max_queue[1]);
        diag_util_printf( "%4u/%-4u \n", cur_queue[2], max_queue[2]);
        break;
    }
#endif
    default:
            return  CPARSER_OK;
    }
    return CPARSER_OK;
}

/*
 * flowctrl dump threshold ( switch | pon | swpbo )
 */
cparser_result_t
cparser_cmd_flowctrl_dump_threshold_switch_pon_swpbo(
                                              cparser_context_t *context)
{
    rtk_enable_t enable;
    uint32 threshold, onThresh, offThresh;
    diag_portlist_t portlist;
    rtk_port_t port;
    diag_mask_t  queuelist;
    rtk_qid_t queue;
    int32 ret = RT_ERR_FAILED;
    diag_mask_t  idxlist;
    uint32 idx;
    uint32 index[8];

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    if (!osal_strcmp(TOKEN_STR(3),"switch")) {
        switch (DIAG_UTIL_CHIP_TYPE) {
#ifdef CONFIG_SDK_APOLLO
            case APOLLO_CHIP_ID:
                {
                    apollo_raw_flowctrl_type_t type;
                    apollo_raw_flowctrl_jumbo_size_t size;
                    /*call API*/
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_type_get(&type), ret);
                    diag_util_printf("Flow Control Type is  %s\n",diagStr_flowCtrlType[type]);
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_jumboMode_get(&enable), ret);
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_jumboSize_get(&size), ret);
                    diag_util_printf("Jumbo Mode is %s\n",diagStr_enable[enable]);
                    diag_util_printf("Jumbo Size is %s\n",diagStr_flowCtrlJumboSize[size]);
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_dropAllThreshold_get(&threshold), ret);
                    diag_util_printf("Drop All Threshold is %u\n", threshold);
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_pauseAllThreshold_get(&threshold), ret);
                    diag_util_printf("Pause All Threshold is %u\n", threshold);

                    diag_util_printf("\nFlow Control On-Off Threshold\n");
                    diag_util_printf("                        On Threshold     Off-Threshold\n");
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_globalFconHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Global High FC-On            %-4u              %-4u\n",onThresh,offThresh);
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_globalFconLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Global Low FC-On             %-4u              %-4u\n",onThresh,offThresh);
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_portFconHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Port High FC-On              %-4u              %-4u\n",onThresh,offThresh);
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_portFconLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Port Low FC-On               %-4u              %-4u\n",onThresh,offThresh);
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_globalFcoffHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Global High FC-Off           %-4u              %-4u\n",onThresh,offThresh);
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_globalFcoffLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Global Low FC-Off            %-4u              %-4u\n",onThresh,offThresh);
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_portFcoffHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Port High FC-Off             %-4u              %-4u\n",onThresh,offThresh);
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_portFcoffLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Port Low FC-Off              %-4u              %-4u\n",onThresh,offThresh);
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_jumboGlobalHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Jumbo Global High            %-4u              %-4u\n",onThresh,offThresh);
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_jumboGlobalLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Jumbo Global Low             %-4u              %-4u\n",onThresh,offThresh);
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_jumboPortHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Jumbo Port High              %-4u              %-4u\n",onThresh,offThresh);
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_jumboPortLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Jumbo Port Low               %-4u              %-4u\n",onThresh,offThresh);
                    diag_util_printf("\n");
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_portEegressGapThreshold_get(&threshold), ret);
                    diag_util_printf("Port Egress Gap Threshold is %u \n", threshold);
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_queueEegressGapThreshold_get(&threshold), ret);
                    diag_util_printf("Queue Egress Gap Threshold is %u \n", threshold);

                    portlist.min =0;
                    portlist.max = 6;
                    portlist.portmask.bits[0] = 0x7F;
                    diag_util_printf("Egress-Port       Drop-Threshold \n");
                    diag_util_printf("-----------------------------\n");
                    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
                    {
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_portEegressDropThreshold_get(port, &threshold), ret);
                        diag_util_printf("%-18u  %u\n", port, threshold);
                    }
                    diag_util_printf("\n");
                    queuelist.min = 0;
                    queuelist.max =  APOLLO_QUEUENO -1;
                    queuelist.mask.bits[0] = 0xFF;
                    diag_util_printf("Egress-Queue       Drop-Threshold \n");
                    diag_util_printf("---------------------------------\n");
                    DIAG_UTIL_MASK_SCAN(queuelist, queue)
                    {
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_queueEegressDropThreshold_get(queue, &threshold), ret);
                        diag_util_printf("%-18u  %u\n", queue, threshold);
                    }

                    diag_util_printf("ueue Egress Drop Enable \n");
                    diag_util_printf("            ");
                    for (queue=0;queue<8; queue++) {
                        diag_util_printf("Q%d    ",queue);
                    }
                    diag_util_printf("\n");
                    diag_util_printf("---------------------------------------------------------\n");
                    for (port=0;port<7; port++) {
                        diag_util_printf("Port %2u ", port);
                        for (queue=0;queue<8; queue++) {
                            DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_egressDropEnable_get(port, queue, &enable), ret);
                            diag_util_printf("   %3s",enable?"En":"Dis");
                        }
                        diag_util_printf("\n");
                    }
                }
                break;
#endif
#ifdef CONFIG_SDK_APOLLOMP
            case APOLLOMP_CHIP_ID:
                {
                    apollomp_raw_flowctrl_type_t type;
                    apollomp_raw_flowctrl_jumbo_size_t size;
                    /*call API*/
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_type_get(&type), ret);
                    diag_util_printf("Flow Control Type is  %s\n",diagStr_flowCtrlType[type]);
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_jumboMode_get(&enable), ret);
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_jumboSize_get(&size), ret);
                    diag_util_printf("Jumbo Mode is %s\n",diagStr_enable[enable]);
                    diag_util_printf("Jumbo Size is %s\n",diagStr_flowCtrlJumboSize[size]);
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_dropAllThreshold_get(&threshold), ret);
                    diag_util_printf("Drop All Threshold is %u\n", threshold);
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_pauseAllThreshold_get(&threshold), ret);
                    diag_util_printf("Pause All Threshold is %u\n", threshold);

                    diag_util_printf("\nFlow Control On-Off Threshold\n");
                    diag_util_printf("                        On Threshold     Off-Threshold\n");
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_globalFconHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Global High FC-On            %-4u              %-4u\n",onThresh,offThresh);
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_globalFconLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Global Low FC-On             %-4u              %-4u\n",onThresh,offThresh);
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_portFconHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Port High FC-On              %-4u              %-4u\n",onThresh,offThresh);
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_portFconLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Port Low FC-On               %-4u              %-4u\n",onThresh,offThresh);
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_globalFcoffHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Global High FC-Off           %-4u              %-4u\n",onThresh,offThresh);
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_globalFcoffLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Global Low FC-Off            %-4u              %-4u\n",onThresh,offThresh);
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_portFcoffHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Port High FC-Off             %-4u              %-4u\n",onThresh,offThresh);
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_portFcoffLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Port Low FC-Off              %-4u              %-4u\n",onThresh,offThresh);
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_jumboGlobalHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Jumbo Global High            %-4u              %-4u\n",onThresh,offThresh);
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_jumboGlobalLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Jumbo Global Low             %-4u              %-4u\n",onThresh,offThresh);
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_jumboPortHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Jumbo Port High              %-4u              %-4u\n",onThresh,offThresh);
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_jumboPortLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Jumbo Port Low               %-4u              %-4u\n",onThresh,offThresh);
                    diag_util_printf("\n");
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_portEegressGapThreshold_get(&threshold), ret);
                    diag_util_printf("Port Egress Gap Threshold is %u \n", threshold);
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_queueEegressGapThreshold_get(&threshold), ret);
                    diag_util_printf("Queue Egress Gap Threshold is %u \n", threshold);

                    portlist.min =0;
                    portlist.max = 6;
                    portlist.portmask.bits[0] = 0x7F;
                    diag_util_printf("Egress-Port       Drop-Threshold \n");
                    diag_util_printf("-----------------------------\n");
                    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
                    {
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_portEegressDropThreshold_get(port, &threshold), ret);
                        diag_util_printf("%-18u  %u\n", port, threshold);
                    }
                    diag_util_printf("\n");
                    queuelist.min = 0;
                    queuelist.max =  APOLLO_QUEUENO -1;
                    queuelist.mask.bits[0] = 0xFF;
                    diag_util_printf("Egress-Queue       Drop-Threshold \n");
                    diag_util_printf("---------------------------------\n");
                    DIAG_UTIL_MASK_SCAN(queuelist, queue)
                    {
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_queueEegressDropThreshold_get(queue, &threshold), ret);
                        diag_util_printf("%-18u  %u\n", queue, threshold);
                    }

                    diag_util_printf("Queue Egress Drop Enable \n");
                    diag_util_printf("            ");
                    for (queue=0;queue<8; queue++) {
                        diag_util_printf("Q%d    ",queue);
                    }
                    diag_util_printf("\n");
                    diag_util_printf("---------------------------------------------------------\n");
                    for (port=0;port<7; port++) {
                        diag_util_printf("Port %2u ", port);
                        for (queue=0;queue<8; queue++) {
                            DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_egressDropEnable_get(port, queue, &enable), ret);
                            diag_util_printf("   %3s",enable?"En":"Dis");
                        }
                        diag_util_printf("\n");
                    }
                }
                break;
#endif
#ifdef CONFIG_SDK_RTL9601B
            case RTL9601B_CHIP_ID:
                {
                    /*call API*/
                    DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_dropAllThreshold_get(&threshold), ret);
                    diag_util_printf("Drop All Threshold is %u\n", threshold);
                    DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_pauseAllThreshold_get(&threshold), ret);
                    diag_util_printf("Pause All Threshold is %u\n", threshold);

                    diag_util_printf("\nFlow Control On-Off Threshold\n");
                    diag_util_printf("                        On Threshold     Off-Threshold\n");
                    DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_globalFconHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Global High FC-On            %-4u              %-4u\n",onThresh,offThresh);
                    DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_globalFconLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Global Low FC-On             %-4u              %-4u\n",onThresh,offThresh);
                    DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_portFconHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Port High FC-On              %-4u              %-4u\n",onThresh,offThresh);
                    DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_portFconLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Port Low FC-On               %-4u              %-4u\n",onThresh,offThresh);
                    DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_globalFcoffHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Global High FC-Off           %-4u              %-4u\n",onThresh,offThresh);
                    DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_globalFcoffLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Global Low FC-Off            %-4u              %-4u\n",onThresh,offThresh);
                    DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_portFcoffHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Port High FC-Off             %-4u              %-4u\n",onThresh,offThresh);
                    DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_portFcoffLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Port Low FC-Off              %-4u              %-4u\n",onThresh,offThresh);
                    diag_util_printf("\n");
                    DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_portEegressGapThreshold_get(&threshold), ret);
                    diag_util_printf("Port Egress Gap Threshold is %u \n", threshold);
                    DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_queueEegressGapThreshold_get(&threshold), ret);
                    diag_util_printf("Queue Egress Gap Threshold is %u \n", threshold);

                    portlist.min =0;
                    portlist.max = 6;
                    portlist.portmask.bits[0] = 0x7;
                    diag_util_printf("Egress-Port       Drop-Threshold \n");
                    diag_util_printf("-----------------------------\n");
                    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
                    {
                        DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_portEegressDropThreshold_get(port, &threshold), ret);
                        diag_util_printf("%-18u  %u\n", port, threshold);
                    }
                    diag_util_printf("\n");
                    queuelist.min = 0;
                    queuelist.max =  7;
                    queuelist.mask.bits[0] = 0xFF;
                    diag_util_printf("Egress-Queue       Drop-Threshold \n");
                    diag_util_printf("---------------------------------\n");
                    DIAG_UTIL_MASK_SCAN(queuelist, queue)
                    {
                        DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_queueEegressDropThreshold_get(queue, &threshold), ret);
                        diag_util_printf("%-18u  %u\n", queue, threshold);
                    }

                    diag_util_printf("Queue Egress Drop Enable \n");
                    diag_util_printf("            ");
                    for (queue=0;queue<8; queue++) {
                        diag_util_printf("Q%d    ",queue);
                    }
                    diag_util_printf("\n");
                    diag_util_printf("---------------------------------------------------------\n");
                    for (port=0;port<=2; port++) {
                        diag_util_printf("Port %2u ", port);
                        for (queue=0;queue<8; queue++) {
                            DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_egressDropEnable_get(port, queue, &enable), ret);
                            diag_util_printf("   %3s",enable?"En":"Dis");
                        }
                        diag_util_printf("\n");
                    }
                }
                break;
#endif
#ifdef CONFIG_SDK_RTL9602C
            case RTL9602C_CHIP_ID:
                {
                    /*call API*/
                    DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_dropAllThreshold_get(&threshold), ret);
                    diag_util_printf("Drop All Threshold is %u\n", threshold);
                    DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_pauseAllThreshold_get(&threshold), ret);
                    diag_util_printf("Pause All Threshold is %u\n", threshold);

                    diag_util_printf("\nFlow Control On-Off Threshold\n");
                    diag_util_printf("                        On Threshold     Off-Threshold\n");
                    DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_globalFconHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Global High FC-On            %-4u              %-4u\n",onThresh,offThresh);
                    DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_globalFconLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Global Low FC-On             %-4u              %-4u\n",onThresh,offThresh);
                    DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_portFconHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Port High FC-On              %-4u              %-4u\n",onThresh,offThresh);
                    DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_portFconLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Port Low FC-On               %-4u              %-4u\n",onThresh,offThresh);
                    DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_globalFcoffHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Global High FC-Off           %-4u              %-4u\n",onThresh,offThresh);
                    DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_globalFcoffLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Global Low FC-Off            %-4u              %-4u\n",onThresh,offThresh);
                    DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_portFcoffHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Port High FC-Off             %-4u              %-4u\n",onThresh,offThresh);
                    DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_portFcoffLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Port Low FC-Off              %-4u              %-4u\n",onThresh,offThresh);
                    diag_util_printf("\n");

                    portlist.min = HAL_GET_MIN_PORT();
                    portlist.max = HAL_GET_MAX_PORT();
                    HAL_GET_ALL_PORTMASK(portlist.portmask);
                    diag_util_printf("Egress-Port       Drop-Threshold \n");
                    diag_util_printf("-----------------------------\n");
                    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
                    {
                        DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_portEegressDropThreshold_get(port, &threshold), ret);
                        diag_util_printf("%-18u  %u\n", port, threshold);
                    }
                    diag_util_printf("\n");
                    queuelist.min = 0;
                    queuelist.max =  7;
                    queuelist.mask.bits[0] = 0xFF;
                    diag_util_printf("Egress-Queue       Drop-Threshold \n");
                    diag_util_printf("---------------------------------\n");
                    DIAG_UTIL_MASK_SCAN(queuelist, queue)
                    {
                        DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_queueEegressDropThreshold_get(queue, &threshold), ret);
                        diag_util_printf("%-18u  %u\n", queue, threshold);
                    }

                    diag_util_printf("Queue Egress Drop Enable \n");
                    diag_util_printf("            ");
                    DIAG_UTIL_MASK_SCAN(queuelist, queue)
                    {
                        diag_util_printf("Q%d    ",queue);
                    }
                    diag_util_printf("\n");
                    diag_util_printf("---------------------------------------------------------\n");
                    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
                    {
                        diag_util_printf("Port %2u ", port);
                        DIAG_UTIL_MASK_SCAN(queuelist, queue)
                        {
                            DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_egressDropEnable_get(port, queue, &enable), ret);
                            diag_util_printf("   %3s",enable?"En":"Dis");
                        }
                        diag_util_printf("\n");
                    }

                    diag_util_printf("\nQueue Egress Drop Enable for PON\n");
                    diag_util_printf("            ");
                    DIAG_UTIL_MASK_SCAN(queuelist, queue)
                    {
                        diag_util_printf("Q%d    ",queue);
                    }
                    diag_util_printf("\n");
                    diag_util_printf("---------------------------------------------------------\n");
                    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
                    {
                        diag_util_printf("Port %2u ", port);
                        DIAG_UTIL_MASK_SCAN(queuelist, queue)
                        {
                            DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_ponEgressDropEnable_get(port, queue, &enable), ret);
                            diag_util_printf("   %3s",enable?"En":"Dis");
                        }
                        diag_util_printf("\n");
                    }
                }
                break;
#endif
#ifdef CONFIG_SDK_RTL9607C
            case RTL9607C_CHIP_ID:
                {
                    /*call API*/
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_dropAllThreshold_get(&threshold), ret);
                    diag_util_printf("Drop All Threshold is %u\n", threshold);
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_pauseAllThreshold_get(&threshold), ret);
                    diag_util_printf("Pause All Threshold is %u\n", threshold);

                    diag_util_printf("\nFlow Control On-Off Threshold\n");
                    diag_util_printf("                        On Threshold     Off-Threshold\n");
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_globalFconHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Global High FC-On            %-4u              %-4u\n",onThresh,offThresh);
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_globalFconLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Global Low FC-On             %-4u              %-4u\n",onThresh,offThresh);
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_portFconHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Port High FC-On              %-4u              %-4u\n",onThresh,offThresh);
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_portFconLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Port Low FC-On               %-4u              %-4u\n",onThresh,offThresh);
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_globalFcoffHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Global High FC-Off           %-4u              %-4u\n",onThresh,offThresh);
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_globalFcoffLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Global Low FC-Off            %-4u              %-4u\n",onThresh,offThresh);
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_portFcoffHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Port High FC-Off             %-4u              %-4u\n",onThresh,offThresh);
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_portFcoffLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Port Low FC-Off              %-4u              %-4u\n",onThresh,offThresh);
                    diag_util_printf("\n");

                    portlist.min = HAL_GET_MIN_PORT();
                    portlist.max = HAL_GET_MAX_PORT();
                    HAL_GET_ALL_PORTMASK(portlist.portmask);
                    diag_util_printf("Egress-Port       Drop-Threshold \n");
                    diag_util_printf("-----------------------------\n");
                    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
                    {
                        DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_portEegressDropThreshold_get(port, &threshold), ret);
                        diag_util_printf("%-18u  %u\n", port, threshold);
                    }
                    diag_util_printf("\n");
                    queuelist.min = HAL_MIN_NUM_OF_QUEUE() - 1;
                    queuelist.max =  HAL_MAX_NUM_OF_QUEUE() - 1;
                    queuelist.mask.bits[0] = 0xFF;
                    //DIAG_UTIL_EXTRACT_QUEUEMASK(queuelist, 7);
                    diag_util_printf("Egress-Queue       Drop-Threshold \n");
                    diag_util_printf("---------------------------------\n");
                    DIAG_UTIL_MASK_SCAN(queuelist, queue)
                    {
                        DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_queueEegressDropThreshold_get(queue, &threshold), ret);
                        diag_util_printf("%-18u  %u\n", queue, threshold);
                    }

                    diag_util_printf("Queue Egress Drop Enable \n");
                    diag_util_printf("            ");
                    DIAG_UTIL_MASK_SCAN(queuelist, queue)
                    {
                        diag_util_printf("Q%d    ",queue);
                    }
                    diag_util_printf("\n");
                    diag_util_printf("---------------------------------------------------------\n");
                    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
                    {
                        diag_util_printf("Port %2u ", port);
                        DIAG_UTIL_MASK_SCAN(queuelist, queue)
                        {
                            DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_egressDropEnable_get(port, queue, &enable), ret);
                            diag_util_printf("   %3s",enable?"En":"Dis");
                        }
                        diag_util_printf("\n");
                    }

                    diag_util_printf("\nQueue Egress Drop Enable for PON\n");
                    diag_util_printf("            ");
                    DIAG_UTIL_MASK_SCAN(queuelist, queue)
                    {
                        diag_util_printf("Q%d    ",queue);
                    }
                    diag_util_printf("\n");
                    diag_util_printf("---------------------------------------------------------\n");
                    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
                    {
                        diag_util_printf("Port %2u ", port);
                        DIAG_UTIL_MASK_SCAN(queuelist, queue)
                        {
                            DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_ponEgressDropEnable_get(port, queue, &enable), ret);
                            diag_util_printf("   %3s",enable?"En":"Dis");
                        }
                        diag_util_printf("\n");
                    }
                }
                break;
#endif
#ifdef CONFIG_SDK_RTL9603CVD
                case RTL9603CVD_CHIP_ID:
                {
                    /*call API*/
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_dropAllThreshold_get(&threshold), ret);
                    diag_util_printf("Drop All Threshold is %u\n", threshold);
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_pauseAllThreshold_get(&threshold), ret);
                    diag_util_printf("Pause All Threshold is %u\n", threshold);

                    diag_util_printf("\nFlow Control On-Off Threshold\n");
                    diag_util_printf("                        On Threshold     Off-Threshold\n");
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_globalFconHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Global High FC-On            %-4u              %-4u\n",onThresh,offThresh);
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_globalFconLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Global Low FC-On             %-4u              %-4u\n",onThresh,offThresh);
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_portFconHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Port High FC-On              %-4u              %-4u\n",onThresh,offThresh);
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_portFconLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Port Low FC-On               %-4u              %-4u\n",onThresh,offThresh);
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_globalFcoffHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Global High FC-Off           %-4u              %-4u\n",onThresh,offThresh);
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_globalFcoffLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Global Low FC-Off            %-4u              %-4u\n",onThresh,offThresh);
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_portFcoffHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Port High FC-Off             %-4u              %-4u\n",onThresh,offThresh);
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_portFcoffLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Port Low FC-Off              %-4u              %-4u\n",onThresh,offThresh);
                    diag_util_printf("\n");

                    portlist.min = HAL_GET_MIN_PORT();
                    portlist.max = HAL_GET_MAX_PORT();
                    HAL_GET_ALL_PORTMASK(portlist.portmask);
                    diag_util_printf("Egress-Port       Drop-Threshold \n");
                    diag_util_printf("-----------------------------\n");
                    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
                    {
                        DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_portEegressDropThreshold_get(port, &threshold), ret);
                        diag_util_printf("%-18u  %u\n", port, threshold);
                    }
                    diag_util_printf("\n");
                    queuelist.min = HAL_MIN_NUM_OF_QUEUE() - 1;
                    queuelist.max =  HAL_MAX_NUM_OF_QUEUE() - 1;
                    queuelist.mask.bits[0] = 0xFF;
                    //DIAG_UTIL_EXTRACT_QUEUEMASK(queuelist, 7);
                    diag_util_printf("Egress-Queue       Drop-Threshold \n");
                    diag_util_printf("---------------------------------\n");
                    DIAG_UTIL_MASK_SCAN(queuelist, queue)
                    {
                        DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_queueEegressDropThreshold_get(queue, &threshold), ret);
                        diag_util_printf("%-18u  %u\n", queue, threshold);
                    }

                    diag_util_printf("Queue Egress Drop Enable \n");
                    diag_util_printf("            ");
                    DIAG_UTIL_MASK_SCAN(queuelist, queue)
                    {
                        diag_util_printf("Q%d    ",queue);
                    }
                    diag_util_printf("\n");
                    diag_util_printf("---------------------------------------------------------\n");
                    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
                    {
                        diag_util_printf("Port %2u ", port);
                        DIAG_UTIL_MASK_SCAN(queuelist, queue)
                        {
                            DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_egressDropEnable_get(port, queue, &enable), ret);
                            diag_util_printf("   %3s",enable?"En":"Dis");
                        }
                        diag_util_printf("\n");
                    }

                    diag_util_printf("\nQueue Egress Drop Enable for PON\n");
                    diag_util_printf("            ");
                    DIAG_UTIL_MASK_SCAN(queuelist, queue)
                    {
                        diag_util_printf("Q%d    ",queue);
                    }
                    diag_util_printf("\n");
                    diag_util_printf("---------------------------------------------------------\n");
                    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
                    {
                        diag_util_printf("Port %2u ", port);
                        DIAG_UTIL_MASK_SCAN(queuelist, queue)
                        {
                            DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_ponEgressDropEnable_get(port, queue, &enable), ret);
                            diag_util_printf("   %3s",enable?"En":"Dis");
                        }
                        diag_util_printf("\n");
                    }
                }
                break;
#endif


            default:
                diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
                return CPARSER_NOT_OK;
                break;
        }
    } else if (!osal_strcmp(TOKEN_STR(3),"pon")) {
        switch (DIAG_UTIL_CHIP_TYPE) {
#if defined(CONFIG_SDK_APOLLO)
            case APOLLO_CHIP_ID:
                {
                    diag_util_printf("\nPON Flow Control On-Off Threshold\n");
                    diag_util_printf("                        On Threshold     Off-Threshold\n");
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_ponGlobalHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Global High                  %-4u              %-4u\n",onThresh,offThresh);
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_ponGlobalLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Global Low                   %-4u              %-4u\n",onThresh,offThresh);
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_ponPortHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Port High                    %-4u              %-4u\n",onThresh,offThresh);
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_ponPortLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Port Low                     %-4u              %-4u\n",onThresh,offThresh);

                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_ponQueueEegressGapThreshold_get(&threshold), ret);
                    diag_util_printf("PON MAC Egress Queue Gap Threshold is %u \n", threshold);
                    diag_util_printf("\n");
                    idxlist.min = 0;
                    idxlist.max =  APOLLO_PON_QUEUE_INDEX_MAX;
                    idxlist.mask.bits[0] = 0xFF;
                    diag_util_printf("PON  Egress Queue Drop Index Threshold  \n");
                    diag_util_printf("Index           Drop-Threshold \n");
                    diag_util_printf("---------------------------------\n");
                    DIAG_UTIL_MASK_SCAN(idxlist, idx)
                    {
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_ponQueueEegressDropThreshold_get(idx, &threshold), ret);
                        diag_util_printf("%-18u  %u\n", idx, threshold);
                    }

                    diag_util_printf("Queue Egress Drop Threshold Index \n");
                    diag_util_printf("--------------------------------------------------\n");
                    for (queue=0;queue<8; queue++) {
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_ponQueueEegressDropIndex_get(queue, &index[(queue%8)]), ret);
                    }

                    diag_util_printf("QID:       0     1     2     3     4     5     6     7    \n");

                    diag_util_printf("Index:     ");
                    for (queue=0;queue<8; queue++) {
                        diag_util_printf("%-6u", index[queue]);
                        if (queue==7)
                            diag_util_printf("\n");
                    }

                    diag_util_printf("---------------------------------------------------\n");

                    for (queue=8;queue<16; queue++) {
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_ponQueueEegressDropIndex_get(queue, &index[(queue%8)]), ret);
                    }

                    diag_util_printf("QID:       8     9    10    11    12    13    14    15    \n");

                    diag_util_printf("Index:     ");
                    for (queue=0;queue<8; queue++) {
                        diag_util_printf("%-6u", index[queue]);
                        if (queue==7)
                            diag_util_printf("\n");
                    }

                    diag_util_printf("---------------------------------------------------\n");

                    for (queue=16;queue<24; queue++) {
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_ponQueueEegressDropIndex_get(queue, &index[(queue%8)]), ret);
                    }

                    diag_util_printf("QID:      16    17    18    19    20    21    22    23    \n");

                    diag_util_printf("Index:     ");
                    for (queue=0;queue<8; queue++) {
                        diag_util_printf("%-6u", index[queue]);
                        if (queue==7)
                            diag_util_printf("\n");
                    }

                    diag_util_printf("---------------------------------------------------\n");

                    for (queue=24;queue<32; queue++) {
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_ponQueueEegressDropIndex_get(queue, &index[(queue%8)]), ret);
                    }

                    diag_util_printf("QID:      24    25    26    27    28    29    30    31    \n");

                    diag_util_printf("Index:     ");
                    for (queue=0;queue<8; queue++) {
                        diag_util_printf("%-6u", index[queue]);
                        if (queue==7)
                            diag_util_printf("\n");
                    }

                    diag_util_printf("---------------------------------------------------\n");

                    for (queue=32;queue<40; queue++) {
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_ponQueueEegressDropIndex_get(queue, &index[(queue%8)]), ret);
                    }

                    diag_util_printf("QID:      32    33    34    35    36    37    38    39    \n");

                    diag_util_printf("Index:     ");
                    for (queue=0;queue<8; queue++) {
                        diag_util_printf("%-6u", index[queue]);
                        if (queue==7)
                            diag_util_printf("\n");
                    }

                    diag_util_printf("---------------------------------------------------\n");

                    for (queue=40;queue<48; queue++) {
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_ponQueueEegressDropIndex_get(queue, &index[(queue%8)]), ret);
                    }

                    diag_util_printf("QID:      40    41    42    43    44    45    46    47    \n");

                    diag_util_printf("Index:     ");
                    for (queue=0;queue<8; queue++) {
                        diag_util_printf("%-6u", index[queue]);
                        if (queue==7)
                            diag_util_printf("\n");
                    }

                    diag_util_printf("---------------------------------------------------\n");

                    for (queue=48;queue<56; queue++) {
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_ponQueueEegressDropIndex_get(queue, &index[(queue%8)]), ret);
                    }

                    diag_util_printf("QID:      48    49    50    51    52    53    54    55    \n");

                    diag_util_printf("Index:     ");
                    for (queue=0;queue<8; queue++) {
                        diag_util_printf("%-6u", index[queue]);
                        if (queue==7)
                            diag_util_printf("\n");
                    }

                    diag_util_printf("---------------------------------------------------\n");

                    for (queue=56;queue<64; queue++) {
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_ponQueueEegressDropIndex_get(queue, &index[(queue%8)]), ret);
                    }

                    diag_util_printf("QID:      56    57    58    59    60    61    62    63    \n");

                    diag_util_printf("Index:     ");
                    for (queue=0;queue<8; queue++) {
                        diag_util_printf("%-6u", index[queue]);
                        if (queue==7)
                            diag_util_printf("\n");
                    }

                    diag_util_printf("---------------------------------------------------\n");

                    for (queue=64;queue<72; queue++) {
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_ponQueueEegressDropIndex_get(queue, &index[(queue%8)]), ret);
                    }

                    diag_util_printf("QID:      64    65    66    67    68    69    70    71    \n");

                    diag_util_printf("Index:     ");
                    for (queue=0;queue<8; queue++) {
                        diag_util_printf("%-6u", index[queue]);
                        if (queue==7)
                            diag_util_printf("\n");
                    }

                    diag_util_printf("---------------------------------------------------\n");

                    for (queue=72;queue<80; queue++) {
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_ponQueueEegressDropIndex_get(queue, &index[(queue%8)]), ret);
                    }

                    diag_util_printf("QID:      72    73    74    75    76    77    78    79    \n");

                    diag_util_printf("Index:     ");
                    for (queue=0;queue<8; queue++) {
                        diag_util_printf("%-6u", index[queue]);
                        if (queue==7)
                            diag_util_printf("\n");
                    }

                    diag_util_printf("---------------------------------------------------\n");

                    for (queue=80;queue<88; queue++) {
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_ponQueueEegressDropIndex_get(queue, &index[(queue%8)]), ret);
                    }

                    diag_util_printf("QID:      80    81    82    83    84    85    86    87    \n");

                    diag_util_printf("Index:     ");
                    for (queue=0;queue<8; queue++) {
                        diag_util_printf("%-6u", index[queue]);
                        if (queue==7)
                            diag_util_printf("\n");
                    }

                    diag_util_printf("---------------------------------------------------\n");

                    for (queue=88;queue<96; queue++) {
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_ponQueueEegressDropIndex_get(queue, &index[(queue%8)]), ret);
                    }

                    diag_util_printf("QID:      88    89    90    91    92    93    94    95    \n");

                    diag_util_printf("Index:     ");
                    for (queue=0;queue<8; queue++) {
                        diag_util_printf("%-6u", index[queue]);
                        if (queue==7)
                            diag_util_printf("\n");
                    }

                    diag_util_printf("---------------------------------------------------\n");

                    for (queue=96;queue<104; queue++) {
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_ponQueueEegressDropIndex_get(queue, &index[(queue%8)]), ret);
                    }

                    diag_util_printf("QID:      96    97    98    99    100   101   102   103    \n");

                    diag_util_printf("Index:     ");
                    for (queue=0;queue<8; queue++) {
                        diag_util_printf("%-6u", index[queue]);
                        if (queue==7)
                            diag_util_printf("\n");
                    }

                    diag_util_printf("---------------------------------------------------\n");

                    for (queue=104;queue<112; queue++) {
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_ponQueueEegressDropIndex_get(queue, &index[(queue%8)]), ret);
                    }

                    diag_util_printf("QID:      104   105   106   107   108   109   110   111    \n");

                    diag_util_printf("Index:     ");
                    for (queue=0;queue<8; queue++) {
                        diag_util_printf("%-6u", index[queue]);
                        if (queue==7)
                            diag_util_printf("\n");
                    }

                    diag_util_printf("---------------------------------------------------\n");

                    for (queue=112;queue<120; queue++) {
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_ponQueueEegressDropIndex_get(queue, &index[(queue%8)]), ret);
                    }

                    diag_util_printf("QID:      112   113   114   115   116   117   118   119    \n");

                    diag_util_printf("Index:     ");
                    for (queue=0;queue<8; queue++) {
                        diag_util_printf("%-6u", index[queue]);
                        if (queue==7)
                            diag_util_printf("\n");
                    }

                    diag_util_printf("---------------------------------------------------\n");

                    for (queue=120;queue<128; queue++) {
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_ponQueueEegressDropIndex_get(queue, &index[(queue%8)]), ret);
                    }

                    diag_util_printf("QID:      120   121   122   123   124   125   126   127    \n");

                    diag_util_printf("Index:     ");
                    for (queue=0;queue<8; queue++) {
                        diag_util_printf("%-6u", index[queue]);
                        if (queue==7)
                            diag_util_printf("\n");
                    }
                }
                break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
            case APOLLOMP_CHIP_ID:
                {
                    diag_util_printf("\nPON Flow Control On-Off Threshold\n");
                    diag_util_printf("                        On Threshold     Off-Threshold\n");
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_ponGlobalHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Global High                  %-4u              %-4u\n",onThresh,offThresh);
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_ponGlobalLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Global Low                   %-4u              %-4u\n",onThresh,offThresh);
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_ponPortHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Port High                    %-4u              %-4u\n",onThresh,offThresh);
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_ponPortLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Port Low                     %-4u              %-4u\n",onThresh,offThresh);

                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_ponQueueEegressGapThreshold_get(&threshold), ret);
                    diag_util_printf("PON MAC Egress Queue Gap Threshold is %u \n", threshold);
                    diag_util_printf("\n");
                    idxlist.min = 0;
                    idxlist.max =  APOLLO_PON_QUEUE_INDEX_MAX;
                    idxlist.mask.bits[0] = 0xFF;
                    diag_util_printf("PON  Egress Queue Drop Index Threshold  \n");
                    diag_util_printf("Index           Drop-Threshold \n");
                    diag_util_printf("---------------------------------\n");
                    DIAG_UTIL_MASK_SCAN(idxlist, idx)
                    {
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_ponQueueEegressDropThreshold_get(idx, &threshold), ret);
                        diag_util_printf("%-18u  %u\n", idx, threshold);
                    }

                    diag_util_printf("Queue Egress Drop Threshold Index \n");
                    diag_util_printf("--------------------------------------------------\n");
                    for (queue=0;queue<8; queue++) {
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_ponQueueEegressDropIndex_get(queue, &index[(queue%8)]), ret);
                    }

                    diag_util_printf("QID:       0     1     2     3     4     5     6     7    \n");

                    diag_util_printf("Index:     ");
                    for (queue=0;queue<8; queue++) {
                        diag_util_printf("%-6u", index[queue]);
                        if (queue==7)
                            diag_util_printf("\n");
                    }

                    diag_util_printf("---------------------------------------------------\n");

                    for (queue=8;queue<16; queue++) {
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_ponQueueEegressDropIndex_get(queue, &index[(queue%8)]), ret);
                    }

                    diag_util_printf("QID:       8     9    10    11    12    13    14    15    \n");

                    diag_util_printf("Index:     ");
                    for (queue=0;queue<8; queue++) {
                        diag_util_printf("%-6u", index[queue]);
                        if (queue==7)
                            diag_util_printf("\n");
                    }

                    diag_util_printf("---------------------------------------------------\n");

                    for (queue=16;queue<24; queue++) {
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_ponQueueEegressDropIndex_get(queue, &index[(queue%8)]), ret);
                    }

                    diag_util_printf("QID:      16    17    18    19    20    21    22    23    \n");

                    diag_util_printf("Index:     ");
                    for (queue=0;queue<8; queue++) {
                        diag_util_printf("%-6u", index[queue]);
                        if (queue==7)
                            diag_util_printf("\n");
                    }

                    diag_util_printf("---------------------------------------------------\n");

                    for (queue=24;queue<32; queue++) {
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_ponQueueEegressDropIndex_get(queue, &index[(queue%8)]), ret);
                    }

                    diag_util_printf("QID:      24    25    26    27    28    29    30    31    \n");

                    diag_util_printf("Index:     ");
                    for (queue=0;queue<8; queue++) {
                        diag_util_printf("%-6u", index[queue]);
                        if (queue==7)
                            diag_util_printf("\n");
                    }

                    diag_util_printf("---------------------------------------------------\n");

                    for (queue=32;queue<40; queue++) {
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_ponQueueEegressDropIndex_get(queue, &index[(queue%8)]), ret);
                    }

                    diag_util_printf("QID:      32    33    34    35    36    37    38    39    \n");

                    diag_util_printf("Index:     ");
                    for (queue=0;queue<8; queue++) {
                        diag_util_printf("%-6u", index[queue]);
                        if (queue==7)
                            diag_util_printf("\n");
                    }

                    diag_util_printf("---------------------------------------------------\n");

                    for (queue=40;queue<48; queue++) {
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_ponQueueEegressDropIndex_get(queue, &index[(queue%8)]), ret);
                    }

                    diag_util_printf("QID:      40    41    42    43    44    45    46    47    \n");

                    diag_util_printf("Index:     ");
                    for (queue=0;queue<8; queue++) {
                        diag_util_printf("%-6u", index[queue]);
                        if (queue==7)
                            diag_util_printf("\n");
                    }

                    diag_util_printf("---------------------------------------------------\n");

                    for (queue=48;queue<56; queue++) {
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_ponQueueEegressDropIndex_get(queue, &index[(queue%8)]), ret);
                    }

                    diag_util_printf("QID:      48    49    50    51    52    53    54    55    \n");

                    diag_util_printf("Index:     ");
                    for (queue=0;queue<8; queue++) {
                        diag_util_printf("%-6u", index[queue]);
                        if (queue==7)
                            diag_util_printf("\n");
                    }

                    diag_util_printf("---------------------------------------------------\n");

                    for (queue=56;queue<64; queue++) {
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_ponQueueEegressDropIndex_get(queue, &index[(queue%8)]), ret);
                    }

                    diag_util_printf("QID:      56    57    58    59    60    61    62    63    \n");

                    diag_util_printf("Index:     ");
                    for (queue=0;queue<8; queue++) {
                        diag_util_printf("%-6u", index[queue]);
                        if (queue==7)
                            diag_util_printf("\n");
                    }

                    diag_util_printf("---------------------------------------------------\n");

                    for (queue=64;queue<72; queue++) {
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_ponQueueEegressDropIndex_get(queue, &index[(queue%8)]), ret);
                    }

                    diag_util_printf("QID:      64    65    66    67    68    69    70    71    \n");

                    diag_util_printf("Index:     ");
                    for (queue=0;queue<8; queue++) {
                        diag_util_printf("%-6u", index[queue]);
                        if (queue==7)
                            diag_util_printf("\n");
                    }

                    diag_util_printf("---------------------------------------------------\n");

                    for (queue=72;queue<80; queue++) {
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_ponQueueEegressDropIndex_get(queue, &index[(queue%8)]), ret);
                    }

                    diag_util_printf("QID:      72    73    74    75    76    77    78    79    \n");

                    diag_util_printf("Index:     ");
                    for (queue=0;queue<8; queue++) {
                        diag_util_printf("%-6u", index[queue]);
                        if (queue==7)
                            diag_util_printf("\n");
                    }

                    diag_util_printf("---------------------------------------------------\n");

                    for (queue=80;queue<88; queue++) {
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_ponQueueEegressDropIndex_get(queue, &index[(queue%8)]), ret);
                    }

                    diag_util_printf("QID:      80    81    82    83    84    85    86    87    \n");

                    diag_util_printf("Index:     ");
                    for (queue=0;queue<8; queue++) {
                        diag_util_printf("%-6u", index[queue]);
                        if (queue==7)
                            diag_util_printf("\n");
                    }

                    diag_util_printf("---------------------------------------------------\n");

                    for (queue=88;queue<96; queue++) {
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_ponQueueEegressDropIndex_get(queue, &index[(queue%8)]), ret);
                    }

                    diag_util_printf("QID:      88    89    90    91    92    93    94    95    \n");

                    diag_util_printf("Index:     ");
                    for (queue=0;queue<8; queue++) {
                        diag_util_printf("%-6u", index[queue]);
                        if (queue==7)
                            diag_util_printf("\n");
                    }

                    diag_util_printf("---------------------------------------------------\n");

                    for (queue=96;queue<104; queue++) {
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_ponQueueEegressDropIndex_get(queue, &index[(queue%8)]), ret);
                    }

                    diag_util_printf("QID:      96    97    98    99    100   101   102   103    \n");

                    diag_util_printf("Index:     ");
                    for (queue=0;queue<8; queue++) {
                        diag_util_printf("%-6u", index[queue]);
                        if (queue==7)
                            diag_util_printf("\n");
                    }

                    diag_util_printf("---------------------------------------------------\n");

                    for (queue=104;queue<112; queue++) {
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_ponQueueEegressDropIndex_get(queue, &index[(queue%8)]), ret);
                    }

                    diag_util_printf("QID:      104   105   106   107   108   109   110   111    \n");

                    diag_util_printf("Index:     ");
                    for (queue=0;queue<8; queue++) {
                        diag_util_printf("%-6u", index[queue]);
                        if (queue==7)
                            diag_util_printf("\n");
                    }

                    diag_util_printf("---------------------------------------------------\n");

                    for (queue=112;queue<120; queue++) {
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_ponQueueEegressDropIndex_get(queue, &index[(queue%8)]), ret);
                    }

                    diag_util_printf("QID:      112   113   114   115   116   117   118   119    \n");

                    diag_util_printf("Index:     ");
                    for (queue=0;queue<8; queue++) {
                        diag_util_printf("%-6u", index[queue]);
                        if (queue==7)
                            diag_util_printf("\n");
                    }

                    diag_util_printf("---------------------------------------------------\n");

                    for (queue=120;queue<128; queue++) {
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_ponQueueEegressDropIndex_get(queue, &index[(queue%8)]), ret);
                    }

                    diag_util_printf("QID:      120   121   122   123   124   125   126   127    \n");

                    diag_util_printf("Index:     ");
                    for (queue=0;queue<8; queue++) {
                        diag_util_printf("%-6u", index[queue]);
                        if (queue==7)
                            diag_util_printf("\n");
                    }
                }
                break;
#endif
#if defined(CONFIG_SDK_RTL9601B)
            case RTL9601B_CHIP_ID:
                {
                    diag_util_printf("\nPON Flow Control On-Off Threshold\n");
                    diag_util_printf("                        On Threshold     Off-Threshold\n");
                    DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_ponGlobalHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Global High                  %-4u              %-4u\n",onThresh,offThresh);
                    DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_ponGlobalLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Global Low                   %-4u              %-4u\n",onThresh,offThresh);
                    DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_ponPortHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Port High                    %-4u              %-4u\n",onThresh,offThresh);
                    DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_ponPortLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Port Low                     %-4u              %-4u\n",onThresh,offThresh);

                    DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_ponQueueEegressGapThreshold_get(&threshold), ret);
                    diag_util_printf("PON MAC Egress Queue Gap Threshold is %u \n", threshold);
                    diag_util_printf("\n");
                    idxlist.min = 0;
                    idxlist.max =  RTL9601B_PON_QUEUE_INDEX_MAX;
                    idxlist.mask.bits[0] = 0xFF;
                    diag_util_printf("PON  Egress Queue Drop Index Threshold  \n");
                    diag_util_printf("Index           Drop-Threshold \n");
                    diag_util_printf("---------------------------------\n");
                    DIAG_UTIL_MASK_SCAN(idxlist, idx)
                    {
                        DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_ponQueueEegressDropThreshold_get(idx, &threshold), ret);
                        diag_util_printf("%-18u  %u\n", idx, threshold);
                    }
                }
                break;
#endif
#if defined(CONFIG_SDK_RTL9602C)
            case RTL9602C_CHIP_ID:
                {
                    diag_util_printf("\nPON Flow Control On-Off Threshold\n");
                    diag_util_printf("                        On Threshold     Off-Threshold\n");
                    DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_ponGlobalHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Global High                  %-4u              %-4u\n",onThresh,offThresh);
                    DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_ponGlobalLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Global Low                   %-4u              %-4u\n",onThresh,offThresh);
                    DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_ponPortHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Port High                    %-4u              %-4u\n",onThresh,offThresh);
                    DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_ponPortLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Port Low                     %-4u              %-4u\n",onThresh,offThresh);
                }
                break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
            case RTL9607C_CHIP_ID:
                {
                    diag_util_printf("\nPON Flow Control On-Off Threshold\n");
                    diag_util_printf("                        On Threshold     Off-Threshold\n");
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_ponGlobalHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Global High                       %-4u              %-4u\n",onThresh,offThresh);
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_ponGlobalLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Global Low                        %-4u              %-4u\n",onThresh,offThresh);
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_ponPortHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Port High                         %-4u              %-4u\n",onThresh,offThresh);
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_ponPortLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Port Low                          %-4u              %-4u\n",onThresh,offThresh);
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_ponGlobalDropHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Global Drop High                  %-4u              %-4u\n",onThresh,offThresh);
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_ponGlobalDropLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Global Drop Low                   %-4u              %-4u\n",onThresh,offThresh);
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_ponPortDropHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Port Drop High                    %-4u              %-4u\n",onThresh,offThresh);
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_ponPortDropLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Port Drop Low                     %-4u              %-4u\n",onThresh,offThresh);
                }
                break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
                case RTL9603CVD_CHIP_ID:
                {
                    diag_util_printf("\nPON Flow Control On-Off Threshold\n");
                    diag_util_printf("                        On Threshold     Off-Threshold\n");
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_ponGlobalHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Global High                       %-4u              %-4u\n",onThresh,offThresh);
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_ponGlobalLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Global Low                        %-4u              %-4u\n",onThresh,offThresh);
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_ponPortHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Port High                         %-4u              %-4u\n",onThresh,offThresh);
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_ponPortLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Port Low                          %-4u              %-4u\n",onThresh,offThresh);
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_ponGlobalDropHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Global Drop High                  %-4u              %-4u\n",onThresh,offThresh);
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_ponGlobalDropLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Global Drop Low                   %-4u              %-4u\n",onThresh,offThresh);
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_ponPortDropHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Port Drop High                    %-4u              %-4u\n",onThresh,offThresh);
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_ponPortDropLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_printf("Port Drop Low                     %-4u              %-4u\n",onThresh,offThresh);
                }
                break;
#endif

            default:
                diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
                return CPARSER_NOT_OK;
                break;
        }

    } else if (!osal_strcmp(TOKEN_STR(3),"swpbo")) {
            switch (DIAG_UTIL_CHIP_TYPE) {
#if defined(CONFIG_SDK_RTL9607C)
                case RTL9607C_CHIP_ID:
                    {
                        diag_util_printf("\nSWPBO Flow Control On-Off Threshold\n");
                        diag_util_printf("                        On Threshold     Off-Threshold\n");
                        DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_swpboGlobalHighThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_printf("Global High                       %-4u              %-4u\n",onThresh,offThresh);
                        DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_swpboGlobalLowThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_printf("Global Low                        %-4u              %-4u\n",onThresh,offThresh);
                        DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_swpboPortHighThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_printf("Port High                         %-4u              %-4u\n",onThresh,offThresh);
                        DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_swpboPortLowThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_printf("Port Low                          %-4u              %-4u\n",onThresh,offThresh);
                        diag_util_printf("\nEgress-Port       Drop-Threshold\n");
                        diag_util_printf("------------------------------------\n");
                        DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_swpboEgressDropThreshold_get(&threshold), ret);
                        diag_util_printf("Swpbo               %-4u\n", threshold);
                    }
                    break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
                case RTL9603CVD_CHIP_ID:
                    {
                        diag_util_printf("\nSWPBO Flow Control On-Off Threshold\n");
                        diag_util_printf("                        On Threshold     Off-Threshold\n");
                        DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_swpboGlobalHighThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_printf("Global High                       %-4u              %-4u\n",onThresh,offThresh);
                        DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_swpboGlobalLowThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_printf("Global Low                        %-4u              %-4u\n",onThresh,offThresh);
                        DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_swpboPortHighThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_printf("Port High                         %-4u              %-4u\n",onThresh,offThresh);
                        DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_swpboPortLowThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_printf("Port Low                          %-4u              %-4u\n",onThresh,offThresh);
                        diag_util_printf("\nEgress-Port       Drop-Threshold\n");
                        diag_util_printf("------------------------------------\n");
                        DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_swpboEgressDropThreshold_get(&threshold), ret);
                        diag_util_printf("Swpbo               %-4u\n", threshold);
                    }
                    break;
#endif


                default:
                    diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
                    return CPARSER_NOT_OK;
                    break;
            }

    }

    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_dump_threshold_switch_pon */

/*
 * flowctrl dump used-page
 */
cparser_result_t
cparser_cmd_flowctrl_dump_used_page(
                                              cparser_context_t *context)
{
    uint32                    record_cnt =0, port_len =0;
    diag_portlist_t      portlist;
    rtk_port_t              port, tmp_port;
    uint32                    current = 0, maximum = 0;
    diag_mask_t            queuelist;
    rtk_qid_t               queue;
    uint32                    qcurrent[8], qmaximum[8];
    int32                      ret = RT_ERR_FAILED;
	uint32 					m_idx = FC_GLUE_MAPPER_INDEX;

    rtk_port_linkStatus_t   linkStatus = PORT_LINKDOWN;
    rtk_port_macAbility_t   portAbility;
    _used_page_record_t *pRec = NULL;

    osal_memset(qcurrent,0,sizeof(qcurrent));
    osal_memset(qmaximum,0,sizeof(qmaximum));

    DIAG_UTIL_PARAM_CHK();
	
	if (m_idx >= FC_GLUE_MAPPER_SIZE)
	{
		diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
        return CPARSER_NOT_OK;
	}

    switch (DIAG_UTIL_CHIP_TYPE)
    {
        case APOLLO_CHIP_ID:
        case APOLLOMP_CHIP_ID:
        case RTL9601B_CHIP_ID:
        case RTL9607C_CHIP_ID:
        case RTL9602C_CHIP_ID:
        case RTL9603CVD_CHIP_ID:
            diag_util_printf("\nSystem and Ingress Page Count (Current/Maximum) \n");

            //_used_page_record_t *pRec = NULL;

            pRec = (_used_page_record_t*)osal_alloc(sizeof(_used_page_record_t));
            /*if (!pRec)
                return return CPARSER_NOT_OK;*/
			#if 0
            if (RT_ERR_OK == FC_GLUE(CID, _raw_flowctrl_totalUsedPageCnt_get)(&(pRec->current), &(pRec->maximum)))
                record_cnt++;
			#else
			if (diag_fc_glue_mapper[m_idx]._raw_flowctrl_totalUsedPageCnt_get && 
				RT_ERR_OK == diag_fc_glue_mapper[m_idx]._raw_flowctrl_totalUsedPageCnt_get(&(pRec->current), &(pRec->maximum)))
			{
				record_cnt++;
			}		
			#endif
            snprintf(pRec->title, strlen("Total") + 1, "Total");
            _used_page_record_add(pRec);


            pRec = (_used_page_record_t*)osal_alloc(sizeof(_used_page_record_t));

			#if 0
            if (RT_ERR_OK == FC_GLUE(CID, _raw_flowctrl_publicUsedPageCnt_get)(&(pRec->current), &(pRec->maximum)))
                record_cnt++;
			#else
			if (diag_fc_glue_mapper[m_idx]._raw_flowctrl_publicUsedPageCnt_get &&
				RT_ERR_OK == diag_fc_glue_mapper[m_idx]._raw_flowctrl_publicUsedPageCnt_get(&(pRec->current), &(pRec->maximum)))
			{
				record_cnt++;
			}
			#endif
            snprintf(pRec->title, strlen("Pub-FC") + 1, "Pub-FC");
            _used_page_record_add(pRec);


            pRec = (_used_page_record_t*)osal_alloc(sizeof(_used_page_record_t));
			
			#if 0
            if (RT_ERR_OK == FC_GLUE(CID, _raw_flowctrl_publicFcoffUsedPageCnt_get)(&(pRec->current), &(pRec->maximum)))
                record_cnt++;
			#else
			if (diag_fc_glue_mapper[m_idx]._raw_flowctrl_publicFcoffUsedPageCnt_get &&
				RT_ERR_OK == diag_fc_glue_mapper[m_idx]._raw_flowctrl_publicFcoffUsedPageCnt_get(&(pRec->current), &(pRec->maximum)))
			{
				record_cnt++;
			}
			#endif
            snprintf(pRec->title, strlen("Pub-FC-OFF") + 1, "Pub-FC-OFF");
             _used_page_record_add(pRec);

            #if defined(CONFIG_SDK_APOLLOMP)

                pRec = (_used_page_record_t*)osal_alloc(sizeof(_used_page_record_t));

				#if 0
                if (RT_ERR_OK == FC_GLUE(CID, _raw_flowctrl_publicJumboUsedPageCnt_get)(&(pRec->current), &(pRec->maximum)))
                    record_cnt++;
				#else
				if (diag_fc_glue_mapper[m_idx]._raw_flowctrl_publicJumboUsedPageCnt_get &&
					RT_ERR_OK == diag_fc_glue_mapper[m_idx]._raw_flowctrl_publicJumboUsedPageCnt_get(&(pRec->current), &(pRec->maximum)))
				{
					record_cnt++;
				}
				#endif
                snprintf(pRec->title, strlen("Jumbo") + 1, "Jumbo");
                 _used_page_record_add(pRec);

            #endif

            portlist.min = HAL_GET_MIN_PORT();
            portlist.max = HAL_GET_MAX_PORT();
            HAL_GET_ALL_PORTMASK(portlist.portmask);

            DIAG_UTIL_PORTMASK_SCAN(portlist, port)
            {
                if (RT_ERR_OK != rtk_port_link_get(port, &linkStatus))
                    continue;
                if (PORT_LINKDOWN == linkStatus)
                    continue;

                pRec = (_used_page_record_t*)osal_alloc(sizeof(_used_page_record_t));

				#if 0
                if (RT_ERR_OK == FC_GLUE(CID, _raw_flowctrl_portUsedPageCnt_get)(port, &(pRec->current), &(pRec->maximum)))
                    record_cnt++;
				#else
				if (diag_fc_glue_mapper[m_idx]._raw_flowctrl_portUsedPageCnt_get &&
					RT_ERR_OK == diag_fc_glue_mapper[m_idx]._raw_flowctrl_portUsedPageCnt_get(port, &(pRec->current), &(pRec->maximum)))
				{
					record_cnt++;
				}
				#endif
                snprintf(pRec->title, sizeof(pRec->title), " P%u", port);
                 _used_page_record_add(pRec);

            }


            #if defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
            if (RT_ERR_OK == rtk_port_macForceAbility_get(HAL_GET_SWPBOLB_PORT(), &portAbility) &&
                PORT_LINKUP == portAbility.linkStatus)
            {
                pRec = (_used_page_record_t*)osal_alloc(sizeof(_used_page_record_t));

				#if 0
                if (RT_ERR_OK == FC_GLUE(CID, _raw_flowctrl_swpboPortUsedUsedPage_get)(&(pRec->current), &(pRec->maximum)))
                    record_cnt++;
				#else
				if (diag_fc_glue_mapper[m_idx]._raw_flowctrl_swpboPortUsedUsedPage_get &&
					RT_ERR_OK == diag_fc_glue_mapper[m_idx]._raw_flowctrl_swpboPortUsedUsedPage_get(&(pRec->current), &(pRec->maximum)))
				{
					record_cnt++;
				}
				#endif
                snprintf(pRec->title, sizeof(pRec->title), " P%u", HAL_GET_SWPBOLB_PORT());
                _used_page_record_add(pRec);
            }
            #endif
            _used_page_record_dump(record_cnt);

            queuelist.min = HAL_MIN_NUM_OF_QUEUE() - 1;
            queuelist.max =  HAL_MAX_NUM_OF_QUEUE() - 1;
            queuelist.mask.bits[0] = 0xFF;

            diag_util_printf("Egress Page Count (Current/Maximum)\n");

            diag_util_printf("Port%9s%10s%10s%10s%10s%10s%10s%10s%10s\n", "EP", "Q0","Q1","Q2","Q3","Q4","Q5","Q6","Q7");
            DIAG_UTIL_PORTMASK_SCAN(portlist, port)
            {
                if (RT_ERR_OK != rtk_port_link_get(port, &linkStatus))
                    continue;
                if (PORT_LINKDOWN == linkStatus)
                    continue;

                diag_util_printf("P%u", port);

                if ((DIAG_UTIL_CHIP_TYPE != APOLLOMP_CHIP_ID &&
                    DIAG_UTIL_CHIP_TYPE != APOLLO_CHIP_ID) ||
                    (port < HAL_GET_PON_PORT()))
                {
                    tmp_port = port;
                }
                else
                {
                    tmp_port = port  - 1;
                }
                port_len = snprintf(NULL, 0, "P%u", port);
                port_len = port_len - 1;

				#if 0
                DIAG_UTIL_ERR_CHK(FC_GLUE(CID, _raw_flowctrl_portEgressUsedPageCnt_get)(tmp_port, &current, &maximum), ret);
				#else
				if (diag_fc_glue_mapper[m_idx]._raw_flowctrl_portEgressUsedPageCnt_get)
				{
					DIAG_UTIL_ERR_CHK(diag_fc_glue_mapper[m_idx]._raw_flowctrl_portEgressUsedPageCnt_get(tmp_port, &current, &maximum), ret);
				}
				#endif
                diag_util_printf("%*u/", (6 - port_len + 4), current);
                diag_util_printf("%-4u ", maximum);

                DIAG_UTIL_MASK_SCAN(queuelist, queue)
                {
					#if 1
					if (diag_fc_glue_mapper[m_idx]._raw_flowctrl_queueEgressUsedPageCnt_get)
					{
						DIAG_UTIL_ERR_CHK(diag_fc_glue_mapper[m_idx]._raw_flowctrl_queueEgressUsedPageCnt_get(tmp_port, queue, &qcurrent[queue], &qmaximum[queue]), ret);
					}
					#else
                    DIAG_UTIL_ERR_CHK(FC_GLUE(CID, _raw_flowctrl_queueEgressUsedPageCnt_get)(tmp_port, queue, &qcurrent[queue], &qmaximum[queue]), ret);
					#endif
                }

                if ((DIAG_UTIL_CHIP_TYPE != APOLLOMP_CHIP_ID &&
                    DIAG_UTIL_CHIP_TYPE != APOLLO_CHIP_ID) ||
                    (!HAL_IS_PON_PORT(port)))
                {
                    DIAG_UTIL_MASK_SCAN(queuelist, queue)
                    {
                        diag_util_printf("%4u/%-4u ", qcurrent[queue], qmaximum[queue]);
                    }
                }
                diag_util_printf("\n");
            }

            #if defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
                if (PORT_LINKUP == portAbility.linkStatus)
                {
                    diag_util_printf("P%u", HAL_GET_SWPBOLB_PORT());
                    port_len = snprintf(NULL, 0, "P%u", HAL_GET_SWPBOLB_PORT());
                    port_len = port_len - 1;
					#if 0
                    DIAG_UTIL_ERR_CHK(FC_GLUE(CID, _raw_flowctrl_swpboEgressUsedPage_get)(&current, &maximum), ret);
					#else
					if (diag_fc_glue_mapper[m_idx]._raw_flowctrl_swpboEgressUsedPage_get)
					{
						DIAG_UTIL_ERR_CHK(diag_fc_glue_mapper[m_idx]._raw_flowctrl_swpboEgressUsedPage_get(&current, &maximum), ret);
					}
					#endif
                    diag_util_printf("%*u/", (6 - port_len + 4) , current);
                    diag_util_printf("%-4u ", maximum);
					#if 0
                    DIAG_UTIL_ERR_CHK(FC_GLUE(CID, _raw_flowctrl_swpboEgressQueueUsedPage_get)(&qcurrent[0], &qmaximum[0]), ret);
					#else
					if (diag_fc_glue_mapper[m_idx]._raw_flowctrl_swpboEgressQueueUsedPage_get)
					{
						DIAG_UTIL_ERR_CHK(diag_fc_glue_mapper[m_idx]._raw_flowctrl_swpboEgressQueueUsedPage_get(&qcurrent[0], &qmaximum[0]), ret);
					}
					#endif
                    diag_util_printf("%4u/%-4u ", qcurrent[0], qmaximum[0]);
                    diag_util_printf("\n");
                }
            #endif
            _sid_used_page_dump();
            _pbo_used_page_dump();

            break;
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_dump_used_page */

/*
 * flowctrl dump used-page ( switch | pon )
 */
cparser_result_t
cparser_cmd_flowctrl_dump_used_page_switch_pon(
                                              cparser_context_t *context)
{
    //apollo_raw_flowctrl_type_t type;
    rtk_enable_t enable;
    //apollo_raw_flowctrl_jumbo_size_t size;
    uint32 threshold, current, maximum;
    diag_portlist_t portlist;
    rtk_port_t port, tmp_port;
    diag_mask_t  queuelist;
    rtk_qid_t queue,idx, tmp_queue;
    uint32 qcurrent[8], qmaximum[8];
    int32 ret = RT_ERR_FAILED;

    osal_memset(qcurrent,0,sizeof(qcurrent));
    osal_memset(qmaximum,0,sizeof(qmaximum));

    DIAG_UTIL_PARAM_CHK();

    if (!osal_strcmp(TOKEN_STR(3),"switch")) {
        switch (DIAG_UTIL_CHIP_TYPE) {
#if defined(CONFIG_SDK_APOLLO)
            case APOLLO_CHIP_ID:
                DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_totalUsedPageCnt_get(&current, &maximum), ret);
                diag_util_printf("Total page Count: Current  %u maximum  %u\n", current, maximum);
                diag_util_printf("\n");
                DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_publicUsedPageCnt_get(&current, &maximum), ret);
                diag_util_printf("Public page Count: Current  %u maximum  %u\n", current, maximum);
                diag_util_printf("\n");
                DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_publicFcoffUsedPageCnt_get(&current, &maximum), ret);
                diag_util_printf("Public page Count for FC-off: Current  %u maximum  %u\n", current, maximum);
                diag_util_printf("\n");
                DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_publicJumboUsedPageCnt_get(&current, &maximum), ret);
                diag_util_printf("Public page Count for jumbo frame: Current  %u maximum  %u\n", current, maximum);

                diag_util_printf("ingress-port     current-page      maximum-page\n");
                diag_util_printf("-----------------------------------------------------\n");
                for (port=0;port<APOLLO_PORTNO; port++) {
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_portUsedPageCnt_get(port, &current, &maximum), ret);
                    diag_util_printf("    %-18u %-18u%u\n", port, current, maximum);
                }

                diag_util_printf("egress-port     current-page      maximum-page\n");
                diag_util_printf("-----------------------------------------------------\n");
                for (port=0;port<APOLLO_PORTNO; port++) {
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_portEgressUsedPageCnt_get(port, &current, &maximum), ret);
                    diag_util_printf("    %-18u %-18u%u\n", port, current, maximum);
                }

                diag_util_printf("Egress Page Count \n");
                for (port=0;port<APOLLO_PORTNO; port++) {
                    if (HAL_GET_PON_PORT() != port) {
                        if (port>HAL_GET_PON_PORT() )
                            tmp_port = port-1;
                        else
                            tmp_port = port;

                        diag_util_printf("          P%u     Q0     Q1     Q2     Q3     Q4     Q5     Q6     Q7  \n", port);
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_portEgressUsedPageCnt_get(tmp_port, &current, &maximum), ret);

                        for (queue=0;queue<APOLLO_QUEUENO; queue++) {
                            DIAG_UTIL_ERR_CHK( apollo_raw_flowctrl_queueEgressUsedPageCnt_get(tmp_port, queue, &qcurrent[queue], &qmaximum[queue]), ret);
                        }
                        diag_util_printf("Current : %-5u  ", current);

                        for (queue=0;queue<APOLLO_QUEUENO; queue++) {
                            if (queue == 7)
                                diag_util_printf("%-5u\n", qcurrent[queue]);
                            else
                                diag_util_printf("%-5u  ", qcurrent[queue]);
                        }

                        diag_util_printf("Maximum : %-5u  ", maximum);

                        for (queue=0;queue<APOLLO_QUEUENO; queue++) {
                            if (queue == 7)
                                diag_util_printf("%-5u\n", qmaximum[queue]);
                            else
                                diag_util_printf("%-5u  ", qmaximum[queue]);
                        }
                        diag_util_printf("-------------------------------------------------------------------\n");
                    }

                }
                break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
            case APOLLOMP_CHIP_ID:
                DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_totalUsedPageCnt_get(&current, &maximum), ret);
                diag_util_printf("Total page Count: Current  %u maximum  %u\n", current, maximum);
                diag_util_printf("\n");
                DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_publicUsedPageCnt_get(&current, &maximum), ret);
                diag_util_printf("Public page Count: Current  %u maximum  %u\n", current, maximum);
                diag_util_printf("\n");
                DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_publicFcoffUsedPageCnt_get(&current, &maximum), ret);
                diag_util_printf("Public page Count for FC-off: Current  %u maximum  %u\n", current, maximum);
                diag_util_printf("\n");
                DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_publicJumboUsedPageCnt_get(&current, &maximum), ret);
                diag_util_printf("Public page Count for jumbo frame: Current  %u maximum  %u\n", current, maximum);

                diag_util_printf("ingress-port     current-page      maximum-page\n");
                diag_util_printf("-----------------------------------------------------\n");
                for (port=0;port<APOLLO_PORTNO; port++) {
                    if (!HAL_IS_PORT_EXIST(port))
                        continue;
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_portUsedPageCnt_get(port, &current, &maximum), ret);
                    diag_util_printf("    %-18u %-18u%u\n", port, current, maximum);
                }

                diag_util_printf("egress-port     current-page      maximum-page\n");
                diag_util_printf("-----------------------------------------------------\n");
                for (port=0;port<APOLLO_PORTNO; port++) {
                    if (!HAL_IS_PORT_EXIST(port))
                        continue;
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_portEgressUsedPageCnt_get(port, &current, &maximum), ret);
                    diag_util_printf("    %-18u %-18u%u\n", port, current, maximum);
                }

                diag_util_printf("Egress Page Count \n");
                for (port=0;port<APOLLO_PORTNO; port++) {
                    if (!HAL_IS_PORT_EXIST(port))
                        continue;
                    if (HAL_GET_PON_PORT() != port) {
                        if (port>HAL_GET_PON_PORT() )
                            tmp_port = port-1;
                        else
                            tmp_port = port;

                        diag_util_printf("          P%u     Q0     Q1     Q2     Q3     Q4     Q5     Q6     Q7  \n", port);
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_portEgressUsedPageCnt_get(port, &current, &maximum), ret);

                        for (queue=0;queue<APOLLO_QUEUENO; queue++) {
                            DIAG_UTIL_ERR_CHK( apollomp_raw_flowctrl_queueEgressUsedPageCnt_get(tmp_port, queue, &qcurrent[queue], &qmaximum[queue]), ret);
                        }
                        diag_util_printf("Current : %-5u  ", current);

                        for (queue=0;queue<APOLLO_QUEUENO; queue++) {
                            if (queue == 7)
                                diag_util_printf("%-5u\n", qcurrent[queue]);
                            else
                                diag_util_printf("%-5u  ", qcurrent[queue]);
                        }

                        diag_util_printf("Maximum : %-5u  ", maximum);

                        for (queue=0;queue<APOLLO_QUEUENO; queue++) {
                            if (queue == 7)
                                diag_util_printf("%-5u\n", qmaximum[queue]);
                            else
                                diag_util_printf("%-5u  ", qmaximum[queue]);
                        }
                        diag_util_printf("-------------------------------------------------------------------\n");
                    } else {
                        diag_util_printf("          P%u\n", port);
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_portEgressUsedPageCnt_get(HAL_GET_PON_PORT(), &current, &maximum), ret);

                        diag_util_printf("Current : %-5u\n", current);
                        diag_util_printf("Maximum : %-5u\n", maximum);
                        diag_util_printf("-------------------------------------------------------------------\n");
                    }

                }
                break;
#endif
#if defined(CONFIG_SDK_RTL9601B)
            case RTL9601B_CHIP_ID:
                DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_totalUsedPageCnt_get(&current, &maximum), ret);
                diag_util_printf("Total page Count: Current  %u maximum  %u\n", current, maximum);
                diag_util_printf("\n");
                DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_publicUsedPageCnt_get(&current, &maximum), ret);
                diag_util_printf("Public page Count: Current  %u maximum  %u\n", current, maximum);
                diag_util_printf("\n");
                DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_publicFcoffUsedPageCnt_get(&current, &maximum), ret);
                diag_util_printf("Public page Count for FC-off: Current  %u maximum  %u\n", current, maximum);
                diag_util_printf("\n");

                diag_util_printf("ingress-port     current-page      maximum-page\n");
                diag_util_printf("-----------------------------------------------------\n");
                for (port=0;port<=2; port++) {
                    DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_portUsedPageCnt_get(port, &current, &maximum), ret);
                    diag_util_printf("    %-18u %-18u%u\n", port, current, maximum);
                }

                diag_util_printf("egress-port     current-page      maximum-page\n");
                diag_util_printf("-----------------------------------------------------\n");
                for (port=0;port<=2; port++) {
                    DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_portEgressUsedPageCnt_get(port, &current, &maximum), ret);
                    diag_util_printf("    %-18u %-18u%u\n", port, current, maximum);
                }

                diag_util_printf("Egress Page Count \n");
                for (port=0;port<=2; port++) {
                    diag_util_printf("          P%u     Q0     Q1     Q2     Q3     Q4     Q5     Q6     Q7  \n", port);
                    DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_portEgressUsedPageCnt_get(port, &current, &maximum), ret);

                    for (queue=0;queue<RTL9601B_QUEUENO; queue++) {
                        DIAG_UTIL_ERR_CHK( rtl9601b_raw_flowctrl_queueEgressUsedPageCnt_get(port, queue, &qcurrent[queue], &qmaximum[queue]), ret);
                    }
                    diag_util_printf("Current : %-5u  ", current);

                    for (queue=0;queue<RTL9601B_QUEUENO; queue++) {
                        if (queue == 7)
                            diag_util_printf("%-5u\n", qcurrent[queue]);
                        else
                            diag_util_printf("%-5u  ", qcurrent[queue]);
                    }

                    diag_util_printf("Maximum : %-5u  ", maximum);

                    for (queue=0;queue<RTL9601B_QUEUENO; queue++) {
                        if (queue == 7)
                            diag_util_printf("%-5u\n", qmaximum[queue]);
                        else
                            diag_util_printf("%-5u  ", qmaximum[queue]);
                    }
                    diag_util_printf("-------------------------------------------------------------------\n");

                }
                break;
#endif
#if defined(CONFIG_SDK_RTL9602C)
            case RTL9602C_CHIP_ID:
                DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_totalUsedPageCnt_get(&current, &maximum), ret);
                diag_util_printf("Total page Count: Current  %u maximum  %u\n", current, maximum);
                diag_util_printf("\n");
                DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_publicUsedPageCnt_get(&current, &maximum), ret);
                diag_util_printf("Public page Count: Current  %u maximum  %u\n", current, maximum);
                diag_util_printf("\n");
                DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_publicFcoffUsedPageCnt_get(&current, &maximum), ret);
                diag_util_printf("Public page Count for FC-off: Current  %u maximum  %u\n", current, maximum);
                diag_util_printf("\n");

                diag_util_printf("ingress-port     current-page      maximum-page\n");
                diag_util_printf("-----------------------------------------------------\n");
                portlist.min = HAL_GET_MIN_PORT();
                portlist.max = HAL_GET_MAX_PORT();
                HAL_GET_ALL_PORTMASK(portlist.portmask);
                DIAG_UTIL_PORTMASK_SCAN(portlist, port)
                {
                    DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_portUsedPageCnt_get(port, &current, &maximum), ret);
                    diag_util_printf("    %-18u %-18u%u\n", port, current, maximum);
                }

                diag_util_printf("egress-port     current-page      maximum-page\n");
                diag_util_printf("-----------------------------------------------------\n");
                DIAG_UTIL_PORTMASK_SCAN(portlist, port)
                {
                    DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_portEgressUsedPageCnt_get(port, &current, &maximum), ret);
                    diag_util_printf("    %-18u %-18u%u\n", port, current, maximum);
                }
                queuelist.min = 0;
                queuelist.max =  7;
                queuelist.mask.bits[0] = 0xFF;

                diag_util_printf("Egress Page Count \n");
                DIAG_UTIL_PORTMASK_SCAN(portlist, port)
                {
                    diag_util_printf("          P%u     Q0     Q1     Q2     Q3     Q4     Q5     Q6     Q7  \n", port);
                    DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_portEgressUsedPageCnt_get(port, &current, &maximum), ret);

                    DIAG_UTIL_MASK_SCAN(queuelist, queue)
                    {
                        DIAG_UTIL_ERR_CHK( rtl9602c_raw_flowctrl_queueEgressUsedPageCnt_get(port, queue, &qcurrent[queue], &qmaximum[queue]), ret);
                    }
                    diag_util_printf("Current : %-5u  ", current);

                    DIAG_UTIL_MASK_SCAN(queuelist, queue)
                    {
                        if (queue == 7)
                            diag_util_printf("%-5u\n", qcurrent[queue]);
                        else
                            diag_util_printf("%-5u  ", qcurrent[queue]);
                    }

                    diag_util_printf("Maximum : %-5u  ", maximum);

                    DIAG_UTIL_MASK_SCAN(queuelist, queue)
                    {
                        if (queue == 7)
                            diag_util_printf("%-5u\n", qmaximum[queue]);
                        else
                            diag_util_printf("%-5u  ", qmaximum[queue]);
                    }
                    diag_util_printf("-------------------------------------------------------------------\n");

                }
                break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
            case RTL9607C_CHIP_ID:
                DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_totalUsedPageCnt_get(&current, &maximum), ret);
                diag_util_printf("Total page Count: Current  %u maximum  %u\n", current, maximum);
                diag_util_printf("\n");
                DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_publicUsedPageCnt_get(&current, &maximum), ret);
                diag_util_printf("Public page Count: Current  %u maximum  %u\n", current, maximum);
                diag_util_printf("\n");
                DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_publicFcoffUsedPageCnt_get(&current, &maximum), ret);
                diag_util_printf("Public page Count for FC-off: Current  %u maximum  %u\n", current, maximum);
                diag_util_printf("\n");

                diag_util_printf("ingress-port     current-page      maximum-page\n");
                diag_util_printf("-----------------------------------------------------\n");

                portlist.min = HAL_GET_MIN_PORT();
                portlist.max = HAL_GET_MAX_PORT();
                HAL_GET_ALL_PORTMASK(portlist.portmask);

                DIAG_UTIL_PORTMASK_SCAN(portlist, port)
                {
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_portUsedPageCnt_get(port, &current, &maximum), ret);
                    diag_util_printf("    %-18u %-18u%u\n", port, current, maximum);
                }
                DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_swpboPortUsedUsedPage_get(&current, &maximum), ret);
                diag_util_printf("    %-18u %-18u%u\n", HAL_GET_SWPBOLB_PORT(), current, maximum);

                diag_util_printf("egress-port     current-page      maximum-page\n");
                diag_util_printf("-----------------------------------------------------\n");
                DIAG_UTIL_PORTMASK_SCAN(portlist, port)
                {
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_portEgressUsedPageCnt_get(port, &current, &maximum), ret);
                    diag_util_printf("    %-18u %-18u%u\n", port, current, maximum);
                }
                DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_swpboEgressUsedPage_get(&current, &maximum), ret);
                diag_util_printf("    %-18u %-18u%u\n", HAL_GET_SWPBOLB_PORT(), current, maximum);

                queuelist.min = HAL_MIN_NUM_OF_QUEUE() - 1;
                queuelist.max =  HAL_MAX_NUM_OF_QUEUE() - 1;
                queuelist.mask.bits[0] = 0xFF;
                //DIAG_UTIL_EXTRACT_QUEUEMASK(queuelist, 7);

                diag_util_printf("Egress Page Count \n");
                DIAG_UTIL_PORTMASK_SCAN(portlist, port)
                {
                    diag_util_printf("          P%u     Q0     Q1     Q2     Q3     Q4     Q5     Q6     Q7  \n", port);
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_portEgressUsedPageCnt_get(port, &current, &maximum), ret);

                    DIAG_UTIL_MASK_SCAN(queuelist, queue)
                    {
                        DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_queueEgressUsedPageCnt_get(port, queue, &qcurrent[queue], &qmaximum[queue]), ret);
                    }
                    diag_util_printf("Current : %-5u  ", current);

                    DIAG_UTIL_MASK_SCAN(queuelist, queue)
                    {
                        if (queue >= 7)
                        {   
                            queue = 7;
                            diag_util_printf("%-5u\n", qcurrent[queue]);
                            break;
                        }    
                        else
                            diag_util_printf("%-5u  ", qcurrent[queue]);
                    }

                    diag_util_printf("Maximum : %-5u  ", maximum);

                    DIAG_UTIL_MASK_SCAN(queuelist, queue)
                    {
                        if (queue >= 7)
                        {                          
                            queue = 7;
                            diag_util_printf("%-5u\n", qmaximum[queue]);
                            break;
                        }    
                        else
                            diag_util_printf("%-5u  ", qmaximum[queue]);
                    }
                    diag_util_printf("-------------------------------------------------------------------\n");

                }

                diag_util_printf("          P%u     Q0  \n", HAL_GET_SWPBOLB_PORT());
                DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_swpboEgressUsedPage_get(&current, &maximum), ret);
                diag_util_printf("Current : %-5u  ", current);
                DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_swpboEgressQueueUsedPage_get(&qcurrent[0], &qmaximum[0]), ret);
                diag_util_printf("%-5u\n", qcurrent[0]);
                diag_util_printf("Maximum : %-5u  ", maximum);
                diag_util_printf("%-5u\n", qmaximum[0]);
                diag_util_printf("-------------------------------------------------------------------\n");
                break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
            case RTL9603CVD_CHIP_ID:
                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_totalUsedPageCnt_get(&current, &maximum), ret);
                diag_util_printf("Total page Count: Current  %u maximum  %u\n", current, maximum);
                diag_util_printf("\n");
                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_publicUsedPageCnt_get(&current, &maximum), ret);
                diag_util_printf("Public page Count: Current  %u maximum  %u\n", current, maximum);
                diag_util_printf("\n");
                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_publicFcoffUsedPageCnt_get(&current, &maximum), ret);
                diag_util_printf("Public page Count for FC-off: Current  %u maximum  %u\n", current, maximum);
                diag_util_printf("\n");

                diag_util_printf("ingress-port     current-page      maximum-page\n");
                diag_util_printf("-----------------------------------------------------\n");

                portlist.min = HAL_GET_MIN_PORT();
                portlist.max = HAL_GET_MAX_PORT();
                HAL_GET_ALL_PORTMASK(portlist.portmask);

                DIAG_UTIL_PORTMASK_SCAN(portlist, port)
                {
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_portUsedPageCnt_get(port, &current, &maximum), ret);
                    diag_util_printf("    %-18u %-18u%u\n", port, current, maximum);
                }
                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_swpboPortUsedUsedPage_get(&current, &maximum), ret);
                diag_util_printf("    %-18u %-18u%u\n", HAL_GET_SWPBOLB_PORT(), current, maximum);

                queuelist.min = HAL_MIN_NUM_OF_QUEUE() - 1;
                queuelist.max =  HAL_MAX_NUM_OF_QUEUE() - 1;
                queuelist.mask.bits[0] = 0xFF;
                //DIAG_UTIL_EXTRACT_QUEUEMASK(queuelist, 7);

                diag_util_printf("Egress Page Count \n");
                DIAG_UTIL_PORTMASK_SCAN(portlist, port)
                {
                    diag_util_printf("          P%u     Q0     Q1     Q2     Q3     Q4     Q5     Q6     Q7  \n", port);
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_portEgressUsedPageCnt_get(port, &current, &maximum), ret);

                    DIAG_UTIL_MASK_SCAN(queuelist, queue)
                    {
                        DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_queueEgressUsedPageCnt_get(port, queue, &qcurrent[queue], &qmaximum[queue]), ret);
                    }
                    diag_util_printf("Current : %-5u  ", current);

                    DIAG_UTIL_MASK_SCAN(queuelist, queue)
                    {
                        if (queue >= 7)
                        {                          
                            queue = 7;
                            diag_util_printf("%-5u\n", qcurrent[queue]);
                            break;
                        }    
                        else
                            diag_util_printf("%-5u  ", qcurrent[queue]);
                    }

                    diag_util_printf("Maximum : %-5u  ", maximum);

                    DIAG_UTIL_MASK_SCAN(queuelist, queue)
                    {
                        if (queue == 7)
                        {                          
                            queue = 7;
                            diag_util_printf("%-5u\n", qmaximum[queue]);
                            break;
                        }    
                        else
                            diag_util_printf("%-5u  ", qmaximum[queue]);
                    }
                    diag_util_printf("-------------------------------------------------------------------\n");

                }

                diag_util_printf("          P%u     Q0  \n", HAL_GET_SWPBOLB_PORT());
                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_swpboEgressUsedPage_get(&current, &maximum), ret);
                diag_util_printf("Current : %-5u  ", current);
                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_swpboEgressQueueUsedPage_get(&qcurrent[0], &qmaximum[0]), ret);
                diag_util_printf("%-5u\n", qcurrent[0]);
                diag_util_printf("Maximum : %-5u  ", maximum);
                diag_util_printf("%-5u\n", qmaximum[0]);
                diag_util_printf("-------------------------------------------------------------------\n");
                break;
#endif

            default:
                diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
                return CPARSER_NOT_OK;
                break;
        }
    } else if (!osal_strcmp(TOKEN_STR(3),"pon")) {
        switch (DIAG_UTIL_CHIP_TYPE) {
#if defined(CONFIG_SDK_APOLLO)
            case APOLLO_CHIP_ID:
                {
                    diag_util_printf("Get PON  Egress Queue Used Page Count\n");
                    diag_util_printf("-------------------------------------------------------------\n");

                    for (idx=0;idx<16;idx++) {
                        diag_util_printf("QID:          ");
                        for (queue=0;queue<8; queue++) {
                            tmp_queue = (idx<<3) | queue;
                            DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_ponUsedPage_get(tmp_queue, &qcurrent[(tmp_queue%8)], &qmaximum[(tmp_queue%8)]), ret);
                            if (queue == 7)
                                diag_util_printf("%-5u\n", tmp_queue);
                            else
                                diag_util_printf("%-5u   ", tmp_queue);
                        }
                        diag_util_printf("Current :     ");
                        for (queue=0;queue<APOLLO_QUEUENO; queue++) {
                            tmp_queue = (idx<<3) | queue;
                            if (queue == 7)
                                diag_util_printf("%-5u\n", qcurrent[queue]);
                            else
                                diag_util_printf("%-5u   ", qcurrent[queue]);
                        }
                        diag_util_printf("Maximum :     ");
                        for (queue=0;queue<APOLLO_QUEUENO; queue++) {
                            if (queue == 7)
                                diag_util_printf("%-5u\n", qmaximum[queue]);
                            else
                                diag_util_printf("%-5u   ", qmaximum[queue]);
                        }
                        diag_util_printf("-------------------------------------------------------------------\n");

                    }
                }
                break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
            case APOLLOMP_CHIP_ID:
                {
                    diag_util_printf("Get PON  Egress Queue Used Page Count\n");
                    diag_util_printf("-------------------------------------------------------------\n");

                    for (idx=0;idx<16;idx++) {
                        diag_util_printf("QID:          ");
                        for (queue=0;queue<8; queue++) {
                            tmp_queue = (idx<<3) | queue;
                            DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_ponUsedPage_get(tmp_queue, &qcurrent[(tmp_queue%8)], &qmaximum[(tmp_queue%8)]), ret);
                            if (queue == 7)
                                diag_util_printf("%-5u\n", tmp_queue);
                            else
                                diag_util_printf("%-5u   ", tmp_queue);
                        }
                        diag_util_printf("Current :     ");
                        for (queue=0;queue<APOLLO_QUEUENO; queue++) {
                            tmp_queue = (idx<<3) | queue;
                            if (queue == 7)
                                diag_util_printf("%-5u\n", qcurrent[queue]);
                            else
                                diag_util_printf("%-5u   ", qcurrent[queue]);
                        }
                        diag_util_printf("Maximum :     ");
                        for (queue=0;queue<APOLLO_QUEUENO; queue++) {
                            if (queue == 7)
                                diag_util_printf("%-5u\n", qmaximum[queue]);
                            else
                                diag_util_printf("%-5u   ", qmaximum[queue]);
                        }
                        diag_util_printf("-------------------------------------------------------------------\n");

                    }
                }
                break;
#endif
#if defined(CONFIG_SDK_RTL9601B)
            case RTL9601B_CHIP_ID:
                {
                    diag_util_printf("Get PON  Egress SID Used Page Count\n");
                    diag_util_printf("-------------------------------------------------------------\n");

                    for (idx=0;idx<5;idx++) {
                        diag_util_printf("SID:          ");
                        for (queue=0;queue<8; queue++) {
                            if (idx==4&&queue>0)
                                break;

                            tmp_queue = (idx<<3) | queue;
                            DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_ponUsedPage_get(tmp_queue, &qcurrent[queue], &qmaximum[queue]), ret);
                            if ((queue == 7) || (idx==4 && queue==0))
                                diag_util_printf("%-5u\n", tmp_queue);
                            else
                                diag_util_printf("%-5u   ", tmp_queue);
                        }
                        diag_util_printf("Current :     ");
                        for (queue=0;queue<RTL9601B_QUEUENO; queue++) {
                            if (idx==4&&queue>0)
                                break;

                            if ((queue == 7) || (idx==4 && queue==0))
                                diag_util_printf("%-5u\n", qcurrent[queue]);
                            else
                                diag_util_printf("%-5u   ", qcurrent[queue]);
                        }
                        diag_util_printf("Maximum :     ");
                        for (queue=0;queue<RTL9601B_QUEUENO; queue++) {
                            if (idx==4&&queue>0)
                                break;

                            if ((queue == 7) || (idx==4 && queue==0))
                                diag_util_printf("%-5u\n", qmaximum[queue]);
                            else
                                diag_util_printf("%-5u   ", qmaximum[queue]);
                        }
                        diag_util_printf("-------------------------------------------------------------------\n");

                    }
                }
                break;
#endif
#if defined(CONFIG_SDK_RTL9602C)
            case RTL9602C_CHIP_ID:
                {
                    diag_util_printf("Get PON  Egress SID Used Page Count\n");
                    diag_util_printf("-------------------------------------------------------------\n");

                    for (idx=0;idx<9;idx++) {
                        diag_util_printf("SID:          ");
                        for (queue=0;queue<8; queue++) {
                            if (idx==8&&queue>0)
                                break;

                            tmp_queue = (idx<<3) | queue;
                            DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_ponUsedPage_get(tmp_queue, &qcurrent[queue], &qmaximum[queue]), ret);
                            if ((queue == 7) || (idx==8 && queue==0))
                                diag_util_printf("%-5u\n", tmp_queue);
                            else
                                diag_util_printf("%-5u   ", tmp_queue);
                        }
                        diag_util_printf("Current :     ");
                        for (queue=0;queue<8; queue++) {
                            if (idx==8&&queue>0)
                                break;

                            if ((queue == 7) || (idx==8 && queue==0))
                                diag_util_printf("%-5u\n", qcurrent[queue]);
                            else
                                diag_util_printf("%-5u   ", qcurrent[queue]);
                        }
                        diag_util_printf("Maximum :     ");
                        for (queue=0;queue<8; queue++) {
                            if (idx==8&&queue>0)
                                break;

                            if ((queue == 7) || (idx==8 && queue==0))
                                diag_util_printf("%-5u\n", qmaximum[queue]);
                            else
                                diag_util_printf("%-5u   ", qmaximum[queue]);
                        }
                        diag_util_printf("-------------------------------------------------------------------\n");

                    }
                }
                break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
            case RTL9607C_CHIP_ID:
                {
                    diag_util_printf("Get PON  Egress SID Used Page Count\n");
                    diag_util_printf("-------------------------------------------------------------\n");
                    rtk_qid_t idx_end = (HAL_CLASSIFY_SID_NUM() / 8) + ((0 == (HAL_CLASSIFY_SID_NUM() % 8)) ? 0 : 1);

                    for (idx = 0; idx < idx_end; idx++) {
                        diag_util_printf("SID:          ");
                        for (queue=0;queue<8; queue++) {
                            if (idx == idx_end &&queue>0)
                                break;

                            tmp_queue = (idx<<3) | queue;
                            DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_ponUsedPage_get(tmp_queue, &qcurrent[queue], &qmaximum[queue]), ret);
                            if ((queue == 7) || (idx == idx_end && queue==0))
                                diag_util_printf("%-5u\n", tmp_queue);
                            else
                                diag_util_printf("%-5u   ", tmp_queue);
                        }
                        diag_util_printf("Current :     ");
                        for (queue=0;queue<8; queue++) {
                            if (idx == idx_end && queue>0)
                                break;

                            if ((queue == 7) || (idx == idx_end && queue==0))
                                diag_util_printf("%-5u\n", qcurrent[queue]);
                            else
                                diag_util_printf("%-5u   ", qcurrent[queue]);
                        }
                        diag_util_printf("Maximum :     ");
                        for (queue=0;queue<8; queue++) {
                            if (idx== idx_end && queue > 0)
                                break;

                            if ((queue == 7) || (idx== idx_end && queue==0))
                                diag_util_printf("%-5u\n", qmaximum[queue]);
                            else
                                diag_util_printf("%-5u   ", qmaximum[queue]);
                        }
                        diag_util_printf("-------------------------------------------------------------------\n");

                    }
                }
                break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
            case RTL9603CVD_CHIP_ID:
                {
                    diag_util_printf("Get PON  Egress SID Used Page Count\n");
                    diag_util_printf("-------------------------------------------------------------\n");
                    rtk_qid_t idx_end = (HAL_CLASSIFY_SID_NUM() / 8) + ((0 == (HAL_CLASSIFY_SID_NUM() % 8)) ? 0 : 1);

                    for (idx = 0; idx < idx_end; idx++) {
                        diag_util_printf("SID:          ");
                        for (queue=0;queue<8; queue++) {
                            if (idx == idx_end &&queue>0)
                                break;

                            tmp_queue = (idx<<3) | queue;
                            DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_ponUsedPage_get(tmp_queue, &qcurrent[queue], &qmaximum[queue]), ret);
                            if ((queue == 7) || (idx == idx_end && queue==0))
                                diag_util_printf("%-5u\n", tmp_queue);
                            else
                                diag_util_printf("%-5u   ", tmp_queue);
                        }
                        diag_util_printf("Current :     ");
                        for (queue=0;queue<8; queue++) {
                            if (idx == idx_end && queue>0)
                                break;

                            if ((queue == 7) || (idx == idx_end && queue==0))
                                diag_util_printf("%-5u\n", qcurrent[queue]);
                            else
                                diag_util_printf("%-5u   ", qcurrent[queue]);
                        }
                        diag_util_printf("Maximum :     ");
                        for (queue=0;queue<8; queue++) {
                            if (idx== idx_end && queue > 0)
                                break;

                            if ((queue == 7) || (idx== idx_end && queue==0))
                                diag_util_printf("%-5u\n", qmaximum[queue]);
                            else
                                diag_util_printf("%-5u   ", qmaximum[queue]);
                        }
                        diag_util_printf("-------------------------------------------------------------------\n");

                    }
                }
                break;
#endif

            default:
                diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
                return CPARSER_NOT_OK;
                break;
        }


    }

    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_dump_used_page_switch_pon */

/*
 * flowctrl set type ( ingress | egress )
 */
cparser_result_t
cparser_cmd_flowctrl_set_type_ingress_egress(
                                            cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    switch (DIAG_UTIL_CHIP_TYPE) {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            {
                apollo_raw_flowctrl_type_t type;
                if ('i'==TOKEN_CHAR(3,0))
                    type = RAW_FLOWCTRL_TYPE_INGRESS;
                else if ('e'==TOKEN_CHAR(3,0))
                    type = RAW_FLOWCTRL_TYPE_EGRESS;
                DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_type_set(type), ret);
                diag_util_printf("Set Flow Control Type to  %s\n",diagStr_flowCtrlType[type]);
            }
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            {
                apollomp_raw_flowctrl_type_t type;
                if ('i'==TOKEN_CHAR(3,0))
                    type = APOLLOMP_FLOWCTRL_TYPE_INGRESS;
                else if ('e'==TOKEN_CHAR(3,0))
                    type = APOLLOMP_FLOWCTRL_TYPE_EGRESS;
                DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_type_set(type), ret);
                diag_util_printf("Set Flow Control Type to  %s\n",diagStr_flowCtrlType[type]);
            }
            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_set_type_ingress_egress */

/*
 * flowctrl get type
 */
cparser_result_t
cparser_cmd_flowctrl_get_type(
                             cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch (DIAG_UTIL_CHIP_TYPE) {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            {
                apollo_raw_flowctrl_type_t type;
                DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_type_get(&type), ret);
                diag_util_mprintf("Flow Control Type is  %s\n",diagStr_flowCtrlType[type]);
            }
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            {
                apollomp_raw_flowctrl_type_t type;
                DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_type_get(&type), ret);
                diag_util_mprintf("Flow Control Type is  %s\n",diagStr_flowCtrlType[type]);
            }
            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_get_type */

/*
 * flowctrl set drop-all threshold <UINT:threshold>
 */
cparser_result_t
cparser_cmd_flowctrl_set_drop_all_threshold_threshold(
                                                     cparser_context_t *context,
                                                     uint32_t  *threshold_ptr)
{
    int32 ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();

    switch (DIAG_UTIL_CHIP_TYPE) {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_dropAllThreshold_set(*threshold_ptr), ret);
                diag_util_mprintf("Set Drop All Threshold to %u\n", *threshold_ptr);
            }
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_dropAllThreshold_set(*threshold_ptr), ret);
                diag_util_mprintf("Set Drop All Threshold to %u\n", *threshold_ptr);
            }
            break;
#endif

#if defined(CONFIG_SDK_RTL9601B)
        case RTL9601B_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_dropAllThreshold_set(*threshold_ptr), ret);
                diag_util_mprintf("Set Drop All Threshold to %u\n", *threshold_ptr);
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_dropAllThreshold_set(*threshold_ptr), ret);
                diag_util_mprintf("Set Drop All Threshold to %u\n", *threshold_ptr);
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_dropAllThreshold_set(*threshold_ptr), ret);
                diag_util_mprintf("Set Drop All Threshold to %u\n", *threshold_ptr);
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_dropAllThreshold_set(*threshold_ptr), ret);
                diag_util_mprintf("Set Drop All Threshold to %u\n", *threshold_ptr);
            }
            break;
#endif

        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_set_drop_all_threshold_threshold */

/*
 * flowctrl get drop-all threshold
 */
cparser_result_t
cparser_cmd_flowctrl_get_drop_all_threshold(
                                           cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint32 threshold;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch (DIAG_UTIL_CHIP_TYPE) {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_dropAllThreshold_get(&threshold), ret);
                diag_util_mprintf("Drop All Threshold is %u\n", threshold);
            }
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_dropAllThreshold_get(&threshold), ret);
                diag_util_mprintf("Drop All Threshold is %u\n", threshold);
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9601B)
        case RTL9601B_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_dropAllThreshold_get(&threshold), ret);
                diag_util_mprintf("Drop All Threshold is %u\n", threshold);
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_dropAllThreshold_get(&threshold), ret);
                diag_util_mprintf("Drop All Threshold is %u\n", threshold);
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_dropAllThreshold_get(&threshold), ret);
                diag_util_mprintf("Drop All Threshold is %u\n", threshold);
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_dropAllThreshold_get(&threshold), ret);
                diag_util_mprintf("Drop All Threshold is %u\n", threshold);
            }
            break;
#endif

        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_get_drop_all_threshold */

/*
 * flowctrl set pause-all threshold <UINT:threshold>
 */
cparser_result_t
cparser_cmd_flowctrl_set_pause_all_threshold_threshold(
                                                      cparser_context_t *context,
                                                      uint32_t  *threshold_ptr)
{
    int32 ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();

    switch (DIAG_UTIL_CHIP_TYPE) {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_pauseAllThreshold_set(*threshold_ptr), ret);
                diag_util_mprintf("Set Pause All Threshold to %u\n", *threshold_ptr);
            }
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_pauseAllThreshold_set(*threshold_ptr), ret);
                diag_util_mprintf("Set Pause All Threshold to %u\n", *threshold_ptr);
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9601B)
        case RTL9601B_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_pauseAllThreshold_set(*threshold_ptr), ret);
                diag_util_mprintf("Set Pause All Threshold to %u\n", *threshold_ptr);
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_pauseAllThreshold_set(*threshold_ptr), ret);
                diag_util_mprintf("Set Pause All Threshold to %u\n", *threshold_ptr);
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_pauseAllThreshold_set(*threshold_ptr), ret);
                diag_util_mprintf("Set Pause All Threshold to %u\n", *threshold_ptr);
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_pauseAllThreshold_set(*threshold_ptr), ret);
                diag_util_mprintf("Set Pause All Threshold to %u\n", *threshold_ptr);
            }
            break;
#endif

        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }


    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_set_pause_all_threshold_threshold */

/*
 * flowctrl get pause-all threshold
 */
cparser_result_t
cparser_cmd_flowctrl_get_pause_all_threshold(
                                            cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint32 threshold;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch (DIAG_UTIL_CHIP_TYPE) {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_pauseAllThreshold_get(&threshold), ret);
            }
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_pauseAllThreshold_get(&threshold), ret);
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9601B)
        case RTL9601B_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_pauseAllThreshold_get(&threshold), ret);
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_pauseAllThreshold_get(&threshold), ret);
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_pauseAllThreshold_get(&threshold), ret);
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_pauseAllThreshold_get(&threshold), ret);
            }
            break;
#endif

        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }
#if defined(CONFIG_SDK_APOLLOMP) || defined(CONFIG_SDK_RTL9601B) || defined(CONFIG_SDK_RTL9602C) || defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
    diag_util_mprintf("Pause All Threshold is %u\n", threshold);
#endif
    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_get_pause_all_threshold */

/*
 * flowctrl set ingress system ( flowctrl-threshold | drop-threshold ) ( high-off | high-on | low-off | low-on ) threshold <UINT:threshold>
 */
cparser_result_t
cparser_cmd_flowctrl_set_ingress_system_flowctrl_threshold_drop_threshold_high_off_high_on_low_off_low_on_threshold_threshold(
                                                                                                                             cparser_context_t *context,
                                                                                                                             uint32_t  *threshold_ptr)
{
    uint32 onThresh, offThresh;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    switch (DIAG_UTIL_CHIP_TYPE) {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            {
                if ('d'==TOKEN_CHAR(4,0)) {
                    if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_globalFcoffHighThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_globalFcoffHighThreshold_set(onThresh, *threshold_ptr), ret);
                        diag_util_printf("Set Flow Control Global FC-Off High Off Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_globalFcoffHighThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_globalFcoffHighThreshold_set(*threshold_ptr, offThresh), ret);
                        diag_util_printf("Set Flow Control Global FC-Off High On Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_globalFcoffLowThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_globalFcoffLowThreshold_set(onThresh, *threshold_ptr), ret);
                        diag_util_printf("Set Flow Control Global FC-Off Low Off Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_globalFcoffLowThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_globalFcoffLowThreshold_set(*threshold_ptr, offThresh), ret);
                        diag_util_printf("Set Flow Control Global FC-Off Low On Threshold to %u\n",*threshold_ptr);
                    }
                } else if ('f'==TOKEN_CHAR(4,0)) {
                    if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_globalFconHighThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_globalFconHighThreshold_set(onThresh, *threshold_ptr), ret);
                        diag_util_printf("Set Flow Control Global FC-On High Off Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_globalFconHighThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_globalFconHighThreshold_set(*threshold_ptr, offThresh), ret);
                        diag_util_printf("Set Flow Control Global FC-On High On Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_globalFconLowThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_globalFconLowThreshold_set(onThresh, *threshold_ptr), ret);
                        diag_util_printf("Set Flow Control Global FC-On Low Off Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_globalFconLowThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_globalFconLowThreshold_set(*threshold_ptr, offThresh), ret);
                        diag_util_printf("Set Flow Control Global FC-On Low On Threshold to %u\n",*threshold_ptr);
                    }
                }
            }
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            {
                if ('d'==TOKEN_CHAR(4,0)) {
                    if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_globalFcoffHighThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_globalFcoffHighThreshold_set(onThresh, *threshold_ptr), ret);
                        diag_util_printf("Set Flow Control Global FC-Off High Off Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_globalFcoffHighThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_globalFcoffHighThreshold_set(*threshold_ptr, offThresh), ret);
                        diag_util_printf("Set Flow Control Global FC-Off High On Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_globalFcoffLowThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_globalFcoffLowThreshold_set(onThresh, *threshold_ptr), ret);
                        diag_util_printf("Set Flow Control Global FC-Off Low Off Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_globalFcoffLowThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_globalFcoffLowThreshold_set(*threshold_ptr, offThresh), ret);
                        diag_util_printf("Set Flow Control Global FC-Off Low On Threshold to %u\n",*threshold_ptr);
                    }
                } else if ('f'==TOKEN_CHAR(4,0)) {
                    if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_globalFconHighThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_globalFconHighThreshold_set(onThresh, *threshold_ptr), ret);
                        diag_util_printf("Set Flow Control Global FC-On High Off Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_globalFconHighThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_globalFconHighThreshold_set(*threshold_ptr, offThresh), ret);
                        diag_util_printf("Set Flow Control Global FC-On High On Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_globalFconLowThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_globalFconLowThreshold_set(onThresh, *threshold_ptr), ret);
                        diag_util_printf("Set Flow Control Global FC-On Low Off Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_globalFconLowThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_globalFconLowThreshold_set(*threshold_ptr, offThresh), ret);
                        diag_util_printf("Set Flow Control Global FC-On Low On Threshold to %u\n",*threshold_ptr);
                    }
                }

            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9601B)
        case RTL9601B_CHIP_ID:
            {
                if ('d'==TOKEN_CHAR(4,0)) {
                    if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                        DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_globalFcoffHighThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_globalFcoffHighThreshold_set(onThresh, *threshold_ptr), ret);
                        diag_util_printf("Set Flow Control Global FC-Off High Off Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                        DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_globalFcoffHighThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_globalFcoffHighThreshold_set(*threshold_ptr, offThresh), ret);
                        diag_util_printf("Set Flow Control Global FC-Off High On Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                        DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_globalFcoffLowThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_globalFcoffLowThreshold_set(onThresh, *threshold_ptr), ret);
                        diag_util_printf("Set Flow Control Global FC-Off Low Off Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                        DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_globalFcoffLowThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_globalFcoffLowThreshold_set(*threshold_ptr, offThresh), ret);
                        diag_util_printf("Set Flow Control Global FC-Off Low On Threshold to %u\n",*threshold_ptr);
                    }
                } else if ('f'==TOKEN_CHAR(4,0)) {
                    if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                        DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_globalFconHighThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_globalFconHighThreshold_set(onThresh, *threshold_ptr), ret);
                        diag_util_printf("Set Flow Control Global FC-On High Off Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                        DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_globalFconHighThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_globalFconHighThreshold_set(*threshold_ptr, offThresh), ret);
                        diag_util_printf("Set Flow Control Global FC-On High On Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                        DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_globalFconLowThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_globalFconLowThreshold_set(onThresh, *threshold_ptr), ret);
                        diag_util_printf("Set Flow Control Global FC-On Low Off Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                        DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_globalFconLowThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_globalFconLowThreshold_set(*threshold_ptr, offThresh), ret);
                        diag_util_printf("Set Flow Control Global FC-On Low On Threshold to %u\n",*threshold_ptr);
                    }
                }

            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
            {
                if ('d'==TOKEN_CHAR(4,0)) {
                    if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                        DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_globalFcoffHighThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_globalFcoffHighThreshold_set(onThresh, *threshold_ptr), ret);
                        diag_util_printf("Set Flow Control Global FC-Off High Off Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                        DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_globalFcoffHighThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_globalFcoffHighThreshold_set(*threshold_ptr, offThresh), ret);
                        diag_util_printf("Set Flow Control Global FC-Off High On Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                        DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_globalFcoffLowThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_globalFcoffLowThreshold_set(onThresh, *threshold_ptr), ret);
                        diag_util_printf("Set Flow Control Global FC-Off Low Off Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                        DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_globalFcoffLowThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_globalFcoffLowThreshold_set(*threshold_ptr, offThresh), ret);
                        diag_util_printf("Set Flow Control Global FC-Off Low On Threshold to %u\n",*threshold_ptr);
                    }
                } else if ('f'==TOKEN_CHAR(4,0)) {
                    if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                        DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_globalFconHighThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_globalFconHighThreshold_set(onThresh, *threshold_ptr), ret);
                        diag_util_printf("Set Flow Control Global FC-On High Off Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                        DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_globalFconHighThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_globalFconHighThreshold_set(*threshold_ptr, offThresh), ret);
                        diag_util_printf("Set Flow Control Global FC-On High On Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                        DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_globalFconLowThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_globalFconLowThreshold_set(onThresh, *threshold_ptr), ret);
                        diag_util_printf("Set Flow Control Global FC-On Low Off Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                        DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_globalFconLowThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_globalFconLowThreshold_set(*threshold_ptr, offThresh), ret);
                        diag_util_printf("Set Flow Control Global FC-On Low On Threshold to %u\n",*threshold_ptr);
                    }
                }

            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
            {
                if ('d'==TOKEN_CHAR(4,0)) {
                    if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                        DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_globalFcoffHighThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_globalFcoffHighThreshold_set(onThresh, *threshold_ptr), ret);
                        diag_util_printf("Set Flow Control Global FC-Off High Off Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                        DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_globalFcoffHighThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_globalFcoffHighThreshold_set(*threshold_ptr, offThresh), ret);
                        diag_util_printf("Set Flow Control Global FC-Off High On Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                        DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_globalFcoffLowThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_globalFcoffLowThreshold_set(onThresh, *threshold_ptr), ret);
                        diag_util_printf("Set Flow Control Global FC-Off Low Off Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                        DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_globalFcoffLowThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_globalFcoffLowThreshold_set(*threshold_ptr, offThresh), ret);
                        diag_util_printf("Set Flow Control Global FC-Off Low On Threshold to %u\n",*threshold_ptr);
                    }
                } else if ('f'==TOKEN_CHAR(4,0)) {
                    if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                        DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_globalFconHighThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_globalFconHighThreshold_set(onThresh, *threshold_ptr), ret);
                        diag_util_printf("Set Flow Control Global FC-On High Off Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                        DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_globalFconHighThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_globalFconHighThreshold_set(*threshold_ptr, offThresh), ret);
                        diag_util_printf("Set Flow Control Global FC-On High On Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                        DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_globalFconLowThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_globalFconLowThreshold_set(onThresh, *threshold_ptr), ret);
                        diag_util_printf("Set Flow Control Global FC-On Low Off Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                        DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_globalFconLowThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_globalFconLowThreshold_set(*threshold_ptr, offThresh), ret);
                        diag_util_printf("Set Flow Control Global FC-On Low On Threshold to %u\n",*threshold_ptr);
                    }
                }

            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
            {
                if ('d'==TOKEN_CHAR(4,0)) {
                    if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                        DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_globalFcoffHighThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_globalFcoffHighThreshold_set(onThresh, *threshold_ptr), ret);
                        diag_util_printf("Set Flow Control Global FC-Off High Off Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                        DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_globalFcoffHighThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_globalFcoffHighThreshold_set(*threshold_ptr, offThresh), ret);
                        diag_util_printf("Set Flow Control Global FC-Off High On Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                        DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_globalFcoffLowThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_globalFcoffLowThreshold_set(onThresh, *threshold_ptr), ret);
                        diag_util_printf("Set Flow Control Global FC-Off Low Off Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                        DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_globalFcoffLowThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_globalFcoffLowThreshold_set(*threshold_ptr, offThresh), ret);
                        diag_util_printf("Set Flow Control Global FC-Off Low On Threshold to %u\n",*threshold_ptr);
                    }
                } else if ('f'==TOKEN_CHAR(4,0)) {
                    if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                        DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_globalFconHighThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_globalFconHighThreshold_set(onThresh, *threshold_ptr), ret);
                        diag_util_printf("Set Flow Control Global FC-On High Off Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                        DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_globalFconHighThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_globalFconHighThreshold_set(*threshold_ptr, offThresh), ret);
                        diag_util_printf("Set Flow Control Global FC-On High On Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                        DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_globalFconLowThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_globalFconLowThreshold_set(onThresh, *threshold_ptr), ret);
                        diag_util_printf("Set Flow Control Global FC-On Low Off Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                        DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_globalFconLowThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_globalFconLowThreshold_set(*threshold_ptr, offThresh), ret);
                        diag_util_printf("Set Flow Control Global FC-On Low On Threshold to %u\n",*threshold_ptr);
                    }
                }

            }
            break;
#endif

        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_set_ingress_system_flowctrl_threshold_drop_threshold_high_off_high_on_low_off_low_on_threshold_threshold */

/*
 * flowctrl get ingress system (  flowctrl-threshold | drop-threshold ) ( high-off | high-on | low-off | low-on ) threshold
 */
cparser_result_t
cparser_cmd_flowctrl_get_ingress_system_flowctrl_threshold_drop_threshold_high_off_high_on_low_off_low_on_threshold(
                                                                                                                   cparser_context_t *context)
{
    uint32 onThresh, offThresh;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch (DIAG_UTIL_CHIP_TYPE) {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            {
                if ('d'==TOKEN_CHAR(4,0)) {
                    if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_globalFcoffHighThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_printf("Flow Control Global FC-Off High Off Threshold is %u\n",offThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_globalFcoffHighThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_printf("Flow Control Global FC-Off High On Threshold is %u\n",onThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_globalFcoffLowThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_printf("Flow Control Global FC-Off Low Off Threshold is %u\n",offThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_globalFcoffLowThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_printf("Flow Control Global FC-Off Low On Threshold is %u\n",onThresh);
                    }
                } else if ('f'==TOKEN_CHAR(4,0)) {
                    if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_globalFconHighThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_printf("Flow Control Global FC-On High Off Threshold is %u\n",offThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_globalFconHighThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_printf("Flow Control Global FC-On High On Threshold is %u\n",onThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_globalFconLowThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_printf("Flow Control Global FC-On Low Off Threshold is %u\n",offThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_globalFconLowThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_printf("Flow Control Global FC-On Low On Threshold is %u\n",onThresh);
                    }
                }
            }
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            {
                if ('d'==TOKEN_CHAR(4,0)) {
                    if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_globalFcoffHighThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_printf("Flow Control Global FC-Off High Off Threshold is %u\n",offThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_globalFcoffHighThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_printf("Flow Control Global FC-Off High On Threshold is %u\n",onThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_globalFcoffLowThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_printf("Flow Control Global FC-Off Low Off Threshold is %u\n",offThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_globalFcoffLowThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_printf("Flow Control Global FC-Off Low On Threshold is %u\n",onThresh);
                    }
                } else if ('f'==TOKEN_CHAR(4,0)) {
                    if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_globalFconHighThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_printf("Flow Control Global FC-On High Off Threshold is %u\n",offThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_globalFconHighThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_printf("Flow Control Global FC-On High On Threshold is %u\n",onThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_globalFconLowThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_printf("Flow Control Global FC-On Low Off Threshold is %u\n",offThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_globalFconLowThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_printf("Flow Control Global FC-On Low On Threshold is %u\n",onThresh);
                    }
                }

            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9601B)
        case RTL9601B_CHIP_ID:
            {
                if ('d'==TOKEN_CHAR(4,0)) {
                    if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                        DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_globalFcoffHighThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_printf("Flow Control Global FC-Off High Off Threshold is %u\n",offThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                        DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_globalFcoffHighThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_printf("Flow Control Global FC-Off High On Threshold is %u\n",onThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                        DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_globalFcoffLowThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_printf("Flow Control Global FC-Off Low Off Threshold is %u\n",offThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                        DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_globalFcoffLowThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_printf("Flow Control Global FC-Off Low On Threshold is %u\n",onThresh);
                    }
                } else if ('f'==TOKEN_CHAR(4,0)) {
                    if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                        DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_globalFconHighThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_printf("Flow Control Global FC-On High Off Threshold is %u\n",offThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                        DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_globalFconHighThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_printf("Flow Control Global FC-On High On Threshold is %u\n",onThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                        DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_globalFconLowThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_printf("Flow Control Global FC-On Low Off Threshold is %u\n",offThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                        DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_globalFconLowThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_printf("Flow Control Global FC-On Low On Threshold is %u\n",onThresh);
                    }
                }

            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
            {
                if ('d'==TOKEN_CHAR(4,0)) {
                    if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                        DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_globalFcoffHighThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_printf("Flow Control Global FC-Off High Off Threshold is %u\n",offThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                        DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_globalFcoffHighThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_printf("Flow Control Global FC-Off High On Threshold is %u\n",onThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                        DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_globalFcoffLowThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_printf("Flow Control Global FC-Off Low Off Threshold is %u\n",offThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                        DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_globalFcoffLowThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_printf("Flow Control Global FC-Off Low On Threshold is %u\n",onThresh);
                    }
                } else if ('f'==TOKEN_CHAR(4,0)) {
                    if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                        DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_globalFconHighThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_printf("Flow Control Global FC-On High Off Threshold is %u\n",offThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                        DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_globalFconHighThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_printf("Flow Control Global FC-On High On Threshold is %u\n",onThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                        DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_globalFconLowThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_printf("Flow Control Global FC-On Low Off Threshold is %u\n",offThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                        DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_globalFconLowThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_printf("Flow Control Global FC-On Low On Threshold is %u\n",onThresh);
                    }
                }

            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
            {
                if ('d'==TOKEN_CHAR(4,0)) {
                    if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                        DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_globalFcoffHighThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_printf("Flow Control Global FC-Off High Off Threshold is %u\n",offThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                        DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_globalFcoffHighThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_printf("Flow Control Global FC-Off High On Threshold is %u\n",onThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                        DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_globalFcoffLowThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_printf("Flow Control Global FC-Off Low Off Threshold is %u\n",offThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                        DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_globalFcoffLowThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_printf("Flow Control Global FC-Off Low On Threshold is %u\n",onThresh);
                    }
                } else if ('f'==TOKEN_CHAR(4,0)) {
                    if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                        DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_globalFconHighThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_printf("Flow Control Global FC-On High Off Threshold is %u\n",offThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                        DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_globalFconHighThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_printf("Flow Control Global FC-On High On Threshold is %u\n",onThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                        DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_globalFconLowThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_printf("Flow Control Global FC-On Low Off Threshold is %u\n",offThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                        DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_globalFconLowThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_printf("Flow Control Global FC-On Low On Threshold is %u\n",onThresh);
                    }
                }

            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
            {
                if ('d'==TOKEN_CHAR(4,0)) {
                    if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                        DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_globalFcoffHighThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_printf("Flow Control Global FC-Off High Off Threshold is %u\n",offThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                        DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_globalFcoffHighThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_printf("Flow Control Global FC-Off High On Threshold is %u\n",onThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                        DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_globalFcoffLowThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_printf("Flow Control Global FC-Off Low Off Threshold is %u\n",offThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                        DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_globalFcoffLowThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_printf("Flow Control Global FC-Off Low On Threshold is %u\n",onThresh);
                    }
                } else if ('f'==TOKEN_CHAR(4,0)) {
                    if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                        DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_globalFconHighThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_printf("Flow Control Global FC-On High Off Threshold is %u\n",offThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                        DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_globalFconHighThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_printf("Flow Control Global FC-On High On Threshold is %u\n",onThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                        DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_globalFconLowThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_printf("Flow Control Global FC-On Low Off Threshold is %u\n",offThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                        DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_globalFconLowThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_printf("Flow Control Global FC-On Low On Threshold is %u\n",onThresh);
                    }
                }

            }
            break;
#endif

        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_get_ingress_system_flowctrl_threshold_drop_threshold_high_off_high_on_low_off_low_on_threshold */

/*
 * flowctrl set ingress port ( flowctrl-threshold | drop-threshold ) ( high-off | high-on | low-off | low-on ) threshold <UINT:threshold>
 */
cparser_result_t
cparser_cmd_flowctrl_set_ingress_port_flowctrl_threshold_drop_threshold_high_off_high_on_low_off_low_on_threshold_threshold(
                                                                                                                           cparser_context_t *context,
                                                                                                                           uint32_t  *threshold_ptr)
{
    uint32 onThresh, offThresh;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    switch (DIAG_UTIL_CHIP_TYPE) {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            {
                if ('d'==TOKEN_CHAR(4,0)) {
                    if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_portFcoffHighThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_portFcoffHighThreshold_set(onThresh, *threshold_ptr), ret);
                        diag_util_printf("Set Flow Control Port FC-Off High Off Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_portFcoffHighThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_portFcoffHighThreshold_set(*threshold_ptr, offThresh), ret);
                        diag_util_printf("Set Flow Control Port FC-Off High On Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_portFcoffLowThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_portFcoffLowThreshold_set(onThresh, *threshold_ptr), ret);
                        diag_util_printf("Set Flow Control Port FC-Off Low Off Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_portFcoffLowThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_portFcoffLowThreshold_set(*threshold_ptr, offThresh), ret);
                        diag_util_printf("Set Flow Control Port FC-Off Low On Threshold to %u\n",*threshold_ptr);
                    }
                } else if ('f'==TOKEN_CHAR(4,0)) {
                    if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_portFconHighThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_portFconHighThreshold_set(onThresh, *threshold_ptr), ret);
                        diag_util_printf("Set Flow Control Port FC-On High Off Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_portFconHighThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_portFconHighThreshold_set(*threshold_ptr, offThresh), ret);
                        diag_util_printf("Set Flow Control Port FC-On High On Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_portFconLowThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_portFconLowThreshold_set(onThresh, *threshold_ptr), ret);
                        diag_util_printf("Set Flow Control Port FC-On Low Off Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_portFconLowThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_portFconLowThreshold_set(*threshold_ptr, offThresh), ret);
                        diag_util_printf("Set Flow Control Port FC-On Low On Threshold to %u\n",*threshold_ptr);
                    }
                }
            }
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            {
                if ('d'==TOKEN_CHAR(4,0)) {
                    if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_portFcoffHighThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_portFcoffHighThreshold_set(onThresh, *threshold_ptr), ret);
                        diag_util_printf("Set Flow Control Port FC-Off High Off Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_portFcoffHighThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_portFcoffHighThreshold_set(*threshold_ptr, offThresh), ret);
                        diag_util_printf("Set Flow Control Port FC-Off High On Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_portFcoffLowThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_portFcoffLowThreshold_set(onThresh, *threshold_ptr), ret);
                        diag_util_printf("Set Flow Control Port FC-Off Low Off Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_portFcoffLowThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_portFcoffLowThreshold_set(*threshold_ptr, offThresh), ret);
                        diag_util_printf("Set Flow Control Port FC-Off Low On Threshold to %u\n",*threshold_ptr);
                    }
                } else if ('f'==TOKEN_CHAR(4,0)) {
                    if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_portFconHighThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_portFconHighThreshold_set(onThresh, *threshold_ptr), ret);
                        diag_util_printf("Set Flow Control Port FC-On High Off Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_portFconHighThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_portFconHighThreshold_set(*threshold_ptr, offThresh), ret);
                        diag_util_printf("Set Flow Control Port FC-On High On Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_portFconLowThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_portFconLowThreshold_set(onThresh, *threshold_ptr), ret);
                        diag_util_printf("Set Flow Control Port FC-On Low Off Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_portFconLowThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_portFconLowThreshold_set(*threshold_ptr, offThresh), ret);
                        diag_util_printf("Set Flow Control Port FC-On Low On Threshold to %u\n",*threshold_ptr);
                    }
                }
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9601B)
        case RTL9601B_CHIP_ID:
            {
                if ('d'==TOKEN_CHAR(4,0)) {
                    if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                        DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_portFcoffHighThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_portFcoffHighThreshold_set(onThresh, *threshold_ptr), ret);
                        diag_util_printf("Set Flow Control Port FC-Off High Off Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                        DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_portFcoffHighThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_portFcoffHighThreshold_set(*threshold_ptr, offThresh), ret);
                        diag_util_printf("Set Flow Control Port FC-Off High On Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                        DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_portFcoffLowThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_portFcoffLowThreshold_set(onThresh, *threshold_ptr), ret);
                        diag_util_printf("Set Flow Control Port FC-Off Low Off Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                        DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_portFcoffLowThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_portFcoffLowThreshold_set(*threshold_ptr, offThresh), ret);
                        diag_util_printf("Set Flow Control Port FC-Off Low On Threshold to %u\n",*threshold_ptr);
                    }
                } else if ('f'==TOKEN_CHAR(4,0)) {
                    if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                        DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_portFconHighThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_portFconHighThreshold_set(onThresh, *threshold_ptr), ret);
                        diag_util_printf("Set Flow Control Port FC-On High Off Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                        DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_portFconHighThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_portFconHighThreshold_set(*threshold_ptr, offThresh), ret);
                        diag_util_printf("Set Flow Control Port FC-On High On Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                        DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_portFconLowThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_portFconLowThreshold_set(onThresh, *threshold_ptr), ret);
                        diag_util_printf("Set Flow Control Port FC-On Low Off Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                        DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_portFconLowThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_portFconLowThreshold_set(*threshold_ptr, offThresh), ret);
                        diag_util_printf("Set Flow Control Port FC-On Low On Threshold to %u\n",*threshold_ptr);
                    }
                }
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
            {
                if ('d'==TOKEN_CHAR(4,0)) {
                    if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                        DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_portFcoffHighThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_portFcoffHighThreshold_set(onThresh, *threshold_ptr), ret);
                        diag_util_printf("Set Flow Control Port FC-Off High Off Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                        DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_portFcoffHighThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_portFcoffHighThreshold_set(*threshold_ptr, offThresh), ret);
                        diag_util_printf("Set Flow Control Port FC-Off High On Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                        DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_portFcoffLowThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_portFcoffLowThreshold_set(onThresh, *threshold_ptr), ret);
                        diag_util_printf("Set Flow Control Port FC-Off Low Off Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                        DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_portFcoffLowThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_portFcoffLowThreshold_set(*threshold_ptr, offThresh), ret);
                        diag_util_printf("Set Flow Control Port FC-Off Low On Threshold to %u\n",*threshold_ptr);
                    }
                } else if ('f'==TOKEN_CHAR(4,0)) {
                    if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                        DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_portFconHighThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_portFconHighThreshold_set(onThresh, *threshold_ptr), ret);
                        diag_util_printf("Set Flow Control Port FC-On High Off Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                        DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_portFconHighThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_portFconHighThreshold_set(*threshold_ptr, offThresh), ret);
                        diag_util_printf("Set Flow Control Port FC-On High On Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                        DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_portFconLowThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_portFconLowThreshold_set(onThresh, *threshold_ptr), ret);
                        diag_util_printf("Set Flow Control Port FC-On Low Off Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                        DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_portFconLowThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_portFconLowThreshold_set(*threshold_ptr, offThresh), ret);
                        diag_util_printf("Set Flow Control Port FC-On Low On Threshold to %u\n",*threshold_ptr);
                    }
                }
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
            {
                if ('d'==TOKEN_CHAR(4,0)) {
                    if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                        DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_portFcoffHighThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_portFcoffHighThreshold_set(onThresh, *threshold_ptr), ret);
                        diag_util_printf("Set Flow Control Port FC-Off High Off Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                        DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_portFcoffHighThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_portFcoffHighThreshold_set(*threshold_ptr, offThresh), ret);
                        diag_util_printf("Set Flow Control Port FC-Off High On Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                        DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_portFcoffLowThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_portFcoffLowThreshold_set(onThresh, *threshold_ptr), ret);
                        diag_util_printf("Set Flow Control Port FC-Off Low Off Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                        DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_portFcoffLowThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_portFcoffLowThreshold_set(*threshold_ptr, offThresh), ret);
                        diag_util_printf("Set Flow Control Port FC-Off Low On Threshold to %u\n",*threshold_ptr);
                    }
                } else if ('f'==TOKEN_CHAR(4,0)) {
                    if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                        DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_portFconHighThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_portFconHighThreshold_set(onThresh, *threshold_ptr), ret);
                        diag_util_printf("Set Flow Control Port FC-On High Off Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                        DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_portFconHighThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_portFconHighThreshold_set(*threshold_ptr, offThresh), ret);
                        diag_util_printf("Set Flow Control Port FC-On High On Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                        DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_portFconLowThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_portFconLowThreshold_set(onThresh, *threshold_ptr), ret);
                        diag_util_printf("Set Flow Control Port FC-On Low Off Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                        DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_portFconLowThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_portFconLowThreshold_set(*threshold_ptr, offThresh), ret);
                        diag_util_printf("Set Flow Control Port FC-On Low On Threshold to %u\n",*threshold_ptr);
                    }
                }
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
            {
                if ('d'==TOKEN_CHAR(4,0)) {
                    if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                        DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_portFcoffHighThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_portFcoffHighThreshold_set(onThresh, *threshold_ptr), ret);
                        diag_util_printf("Set Flow Control Port FC-Off High Off Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                        DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_portFcoffHighThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_portFcoffHighThreshold_set(*threshold_ptr, offThresh), ret);
                        diag_util_printf("Set Flow Control Port FC-Off High On Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                        DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_portFcoffLowThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_portFcoffLowThreshold_set(onThresh, *threshold_ptr), ret);
                        diag_util_printf("Set Flow Control Port FC-Off Low Off Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                        DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_portFcoffLowThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_portFcoffLowThreshold_set(*threshold_ptr, offThresh), ret);
                        diag_util_printf("Set Flow Control Port FC-Off Low On Threshold to %u\n",*threshold_ptr);
                    }
                } else if ('f'==TOKEN_CHAR(4,0)) {
                    if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                        DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_portFconHighThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_portFconHighThreshold_set(onThresh, *threshold_ptr), ret);
                        diag_util_printf("Set Flow Control Port FC-On High Off Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                        DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_portFconHighThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_portFconHighThreshold_set(*threshold_ptr, offThresh), ret);
                        diag_util_printf("Set Flow Control Port FC-On High On Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                        DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_portFconLowThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_portFconLowThreshold_set(onThresh, *threshold_ptr), ret);
                        diag_util_printf("Set Flow Control Port FC-On Low Off Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                        DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_portFconLowThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_portFconLowThreshold_set(*threshold_ptr, offThresh), ret);
                        diag_util_printf("Set Flow Control Port FC-On Low On Threshold to %u\n",*threshold_ptr);
                    }
                }
            }
            break;
#endif

        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_set_ingress_port_flowctrl_threshold_drop_threshold_high_off_high_on_low_off_low_on_threshold_threshold */

/*
 * flowctrl get ingress port ( flowctrl-threshold | drop-threshold ) ( high-off | high-on | low-off | low-on ) threshold
 */
cparser_result_t
cparser_cmd_flowctrl_get_ingress_port_flowctrl_threshold_drop_threshold_high_off_high_on_low_off_low_on_threshold(
                                                                                                                 cparser_context_t *context)
{
    uint32 onThresh, offThresh;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch (DIAG_UTIL_CHIP_TYPE) {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            {
                if ('d'==TOKEN_CHAR(4,0)) {
                    if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_portFcoffHighThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_mprintf("Flow Control Port FC-Off High Off Threshold is %u\n",offThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_portFcoffHighThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_mprintf("Flow Control Port FC-Off High On Threshold is %u\n",onThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_portFcoffLowThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_mprintf("Flow Control Port FC-Off Low Off Threshold is %u\n",offThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_portFcoffLowThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_mprintf("Flow Control Port FC-Off Low On Threshold is %u\n",onThresh);
                    }
                } else if ('f'==TOKEN_CHAR(4,0)) {
                    if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_portFconHighThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_mprintf("Flow Control Port FC-On High Off Threshold is %u\n",offThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_portFconHighThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_mprintf("Flow Control Port FC-On High On Threshold is %u\n",onThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_portFconLowThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_mprintf("Flow Control Port FC-On Low Off Threshold is %u\n",offThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_portFconLowThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_mprintf("Flow Control Port FC-On Low On Threshold is %u\n",onThresh);
                    }
                }
            }
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            {
                if ('d'==TOKEN_CHAR(4,0)) {
                    if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_portFcoffHighThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_mprintf("Flow Control Port FC-Off High Off Threshold is %u\n",offThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_portFcoffHighThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_mprintf("Flow Control Port FC-Off High On Threshold is %u\n",onThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_portFcoffLowThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_mprintf("Flow Control Port FC-Off Low Off Threshold is %u\n",offThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_portFcoffLowThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_mprintf("Flow Control Port FC-Off Low On Threshold is %u\n",onThresh);
                    }
                } else if ('f'==TOKEN_CHAR(4,0)) {
                    if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_portFconHighThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_mprintf("Flow Control Port FC-On High Off Threshold is %u\n",offThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_portFconHighThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_mprintf("Flow Control Port FC-On High On Threshold is %u\n",onThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_portFconLowThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_mprintf("Flow Control Port FC-On Low Off Threshold is %u\n",offThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_portFconLowThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_mprintf("Flow Control Port FC-On Low On Threshold is %u\n",onThresh);
                    }
                }
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9601B)
        case RTL9601B_CHIP_ID:
            {
                if ('d'==TOKEN_CHAR(4,0)) {
                    if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                        DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_portFcoffHighThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_mprintf("Flow Control Port FC-Off High Off Threshold is %u\n",offThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                        DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_portFcoffHighThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_mprintf("Flow Control Port FC-Off High On Threshold is %u\n",onThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                        DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_portFcoffLowThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_mprintf("Flow Control Port FC-Off Low Off Threshold is %u\n",offThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                        DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_portFcoffLowThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_mprintf("Flow Control Port FC-Off Low On Threshold is %u\n",onThresh);
                    }
                } else if ('f'==TOKEN_CHAR(4,0)) {
                    if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                        DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_portFconHighThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_mprintf("Flow Control Port FC-On High Off Threshold is %u\n",offThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                        DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_portFconHighThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_mprintf("Flow Control Port FC-On High On Threshold is %u\n",onThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                        DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_portFconLowThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_mprintf("Flow Control Port FC-On Low Off Threshold is %u\n",offThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                        DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_portFconLowThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_mprintf("Flow Control Port FC-On Low On Threshold is %u\n",onThresh);
                    }
                }
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
            {
                if ('d'==TOKEN_CHAR(4,0)) {
                    if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                        DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_portFcoffHighThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_mprintf("Flow Control Port FC-Off High Off Threshold is %u\n",offThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                        DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_portFcoffHighThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_mprintf("Flow Control Port FC-Off High On Threshold is %u\n",onThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                        DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_portFcoffLowThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_mprintf("Flow Control Port FC-Off Low Off Threshold is %u\n",offThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                        DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_portFcoffLowThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_mprintf("Flow Control Port FC-Off Low On Threshold is %u\n",onThresh);
                    }
                } else if ('f'==TOKEN_CHAR(4,0)) {
                    if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                        DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_portFconHighThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_mprintf("Flow Control Port FC-On High Off Threshold is %u\n",offThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                        DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_portFconHighThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_mprintf("Flow Control Port FC-On High On Threshold is %u\n",onThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                        DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_portFconLowThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_mprintf("Flow Control Port FC-On Low Off Threshold is %u\n",offThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                        DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_portFconLowThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_mprintf("Flow Control Port FC-On Low On Threshold is %u\n",onThresh);
                    }
                }
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
            {
                if ('d'==TOKEN_CHAR(4,0)) {
                    if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                        DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_portFcoffHighThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_mprintf("Flow Control Port FC-Off High Off Threshold is %u\n",offThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                        DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_portFcoffHighThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_mprintf("Flow Control Port FC-Off High On Threshold is %u\n",onThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                        DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_portFcoffLowThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_mprintf("Flow Control Port FC-Off Low Off Threshold is %u\n",offThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                        DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_portFcoffLowThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_mprintf("Flow Control Port FC-Off Low On Threshold is %u\n",onThresh);
                    }
                } else if ('f'==TOKEN_CHAR(4,0)) {
                    if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                        DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_portFconHighThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_mprintf("Flow Control Port FC-On High Off Threshold is %u\n",offThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                        DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_portFconHighThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_mprintf("Flow Control Port FC-On High On Threshold is %u\n",onThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                        DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_portFconLowThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_mprintf("Flow Control Port FC-On Low Off Threshold is %u\n",offThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                        DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_portFconLowThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_mprintf("Flow Control Port FC-On Low On Threshold is %u\n",onThresh);
                    }
                }
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
            {
                if ('d'==TOKEN_CHAR(4,0)) {
                    if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                        DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_portFcoffHighThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_mprintf("Flow Control Port FC-Off High Off Threshold is %u\n",offThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                        DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_portFcoffHighThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_mprintf("Flow Control Port FC-Off High On Threshold is %u\n",onThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                        DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_portFcoffLowThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_mprintf("Flow Control Port FC-Off Low Off Threshold is %u\n",offThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                        DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_portFcoffLowThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_mprintf("Flow Control Port FC-Off Low On Threshold is %u\n",onThresh);
                    }
                } else if ('f'==TOKEN_CHAR(4,0)) {
                    if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                        DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_portFconHighThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_mprintf("Flow Control Port FC-On High Off Threshold is %u\n",offThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                        DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_portFconHighThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_mprintf("Flow Control Port FC-On High On Threshold is %u\n",onThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                        DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_portFconLowThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_mprintf("Flow Control Port FC-On Low Off Threshold is %u\n",offThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                        DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_portFconLowThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_mprintf("Flow Control Port FC-On Low On Threshold is %u\n",onThresh);
                    }
                }
            }
            break;
#endif

        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_get_ingress_port_flowctrl_threshold_drop_threshold_high_off_high_on_low_off_low_on_threshold */

/*
 * flowctrl set ingress egress-drop port ( <PORT_LIST:ports> | all ) threshold <UINT:threshold>
 */
cparser_result_t
cparser_cmd_flowctrl_set_ingress_egress_drop_port_ports_all_threshold_threshold(
                                                                               cparser_context_t *context,
                                                                               char * *ports_ptr,
                                                                               uint32_t  *threshold_ptr)
{
    diag_portlist_t portlist;
    rtk_port_t port;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    diag_util_printf("Set Ingress Flow Control Port Egress Drop Threshold \n");
    diag_util_printf("Egress-Port       Drop-Threshold \n");
    diag_util_printf("-----------------------------\n");

    switch (DIAG_UTIL_CHIP_TYPE) 
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            DIAG_UTIL_PORTMASK_SCAN(portlist, port)
            {
                DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_portEegressDropThreshold_set(port, *threshold_ptr), ret);
                diag_util_mprintf("%-18u  %u\n", port, *threshold_ptr);
            }
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_PORTMASK_SCAN(portlist, port)
            {
                DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_portEegressDropThreshold_set(port, *threshold_ptr), ret);
                diag_util_mprintf("%-18u  %u\n", port, *threshold_ptr);
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9601B)
        case RTL9601B_CHIP_ID:
            DIAG_UTIL_PORTMASK_SCAN(portlist, port)
            {
                DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_portEegressDropThreshold_set(port, *threshold_ptr), ret);
                diag_util_mprintf("%-18u  %u\n", port, *threshold_ptr);
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
            DIAG_UTIL_PORTMASK_SCAN(portlist, port)
            {
                DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_portEegressDropThreshold_set(port, *threshold_ptr), ret);
                diag_util_mprintf("%-18u  %u\n", port, *threshold_ptr);
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
            DIAG_UTIL_PORTMASK_SCAN(portlist, port)
            {
                DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_portEegressDropThreshold_set(port, *threshold_ptr), ret);
                diag_util_mprintf("%-18u  %u\n", port, *threshold_ptr);
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
            DIAG_UTIL_PORTMASK_SCAN(portlist, port)
            {
                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_portEegressDropThreshold_set(port, *threshold_ptr), ret);
                diag_util_mprintf("%-18u  %u\n", port, *threshold_ptr);
            }
            break;
#endif

        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_set_ingress_egress_drop_port_ports_all_threshold_threshold */

/*
 * flowctrl get ingress egress-drop port ( <PORT_LIST:ports> | all ) threshold
 */
cparser_result_t
cparser_cmd_flowctrl_get_ingress_egress_drop_port_ports_all_threshold(
                                                                     cparser_context_t *context,
                                                                     char * *ports_ptr)
{
    diag_portlist_t portlist;
    rtk_port_t port;
    uint32 threshold;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    diag_util_mprintf("Get Ingress Flow Control Port Egress Drop Threshold \n");
    diag_util_mprintf("Egress-Port       Drop-Threshold \n");
    diag_util_mprintf("-----------------------------\n");

    switch (DIAG_UTIL_CHIP_TYPE) 
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            DIAG_UTIL_PORTMASK_SCAN(portlist, port)
            {
                DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_portEegressDropThreshold_get(port, &threshold), ret);
                diag_util_mprintf("%-18u  %u\n", port, threshold);
            }
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_PORTMASK_SCAN(portlist, port)
            {
                DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_portEegressDropThreshold_get(port, &threshold), ret);
                diag_util_mprintf("%-18u  %u\n", port, threshold);
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9601B)
        case RTL9601B_CHIP_ID:
            DIAG_UTIL_PORTMASK_SCAN(portlist, port)
            {
                DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_portEegressDropThreshold_get(port, &threshold), ret);
                diag_util_mprintf("%-18u  %u\n", port, threshold);
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
            DIAG_UTIL_PORTMASK_SCAN(portlist, port)
            {
                DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_portEegressDropThreshold_get(port, &threshold), ret);
                diag_util_mprintf("%-18u  %u\n", port, threshold);
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
            DIAG_UTIL_PORTMASK_SCAN(portlist, port)
            {
                DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_portEegressDropThreshold_get(port, &threshold), ret);
                diag_util_mprintf("%-18u  %u\n", port, threshold);
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
            DIAG_UTIL_PORTMASK_SCAN(portlist, port)
            {
                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_portEegressDropThreshold_get(port, &threshold), ret);
                diag_util_mprintf("%-18u  %u\n", port, threshold);
            }
            break;
#endif

        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }


    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_get_ingress_egress_drop_port_ports_all_threshold */

/*
 * flowctrl set ingress egress-drop queue-id ( <MASK_LIST:qid> | all ) threshold <UINT:threshold>
 */
cparser_result_t
cparser_cmd_flowctrl_set_ingress_egress_drop_queue_id_qid_all_threshold_threshold(
                                                                                 cparser_context_t *context,
                                                                                 char * *qid_ptr,
                                                                                 uint32_t  *threshold_ptr)
{
    diag_mask_t  queuelist;
    rtk_qid_t queue;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_QUEUEMASK(queuelist, 5), ret);

    diag_util_printf("Set Ingress Flow Control Queue Egress Drop Threshold \n");
    diag_util_printf("Egress-Queue       Drop-Threshold \n");
    diag_util_printf("---------------------------------\n");

    switch (DIAG_UTIL_CHIP_TYPE) 
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            DIAG_UTIL_MASK_SCAN(queuelist, queue)
            {
                DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_queueEegressDropThreshold_set(queue, *threshold_ptr), ret);
                diag_util_printf("%-18u  %u\n", queue, *threshold_ptr);
            }
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_MASK_SCAN(queuelist, queue)
            {
                DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_queueEegressDropThreshold_set(queue, *threshold_ptr), ret);
                diag_util_printf("%-18u  %u\n", queue, *threshold_ptr);
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9601B)
        case RTL9601B_CHIP_ID:
            DIAG_UTIL_MASK_SCAN(queuelist, queue)
            {
                DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_queueEegressDropThreshold_set(queue, *threshold_ptr), ret);
                diag_util_printf("%-18u  %u\n", queue, *threshold_ptr);
            }
            break;
#endif

#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
            DIAG_UTIL_MASK_SCAN(queuelist, queue)
            {
                DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_queueEegressDropThreshold_set(queue, *threshold_ptr), ret);
                diag_util_printf("%-18u  %u\n", queue, *threshold_ptr);
            }
            break;
#endif

#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
            DIAG_UTIL_MASK_SCAN(queuelist, queue)
            {
                DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_queueEegressDropThreshold_set(queue, *threshold_ptr), ret);

                diag_util_printf("%-18u  %u\n", queue, *threshold_ptr);
            }
            break;
#endif

#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
            DIAG_UTIL_MASK_SCAN(queuelist, queue)
            {
                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_queueEegressDropThreshold_set(queue, *threshold_ptr), ret);
                diag_util_printf("%-18u  %u\n", queue, *threshold_ptr);
            }
            break;
#endif

        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }


    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_set_ingress_egress_drop_queue_id_qid_all_threshold_threshold */

/*
 * flowctrl get ingress egress-drop queue-id ( <MASK_LIST:qid> | all ) threshold
 */
cparser_result_t
cparser_cmd_flowctrl_get_ingress_egress_drop_queue_id_qid_all_threshold(
                                                                       cparser_context_t *context,
                                                                       char * *qid_ptr)
{
    diag_mask_t  queuelist;
    rtk_qid_t queue;
    uint32 threshold;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_QUEUEMASK(queuelist, 5), ret);

    diag_util_mprintf("Get Ingress Flow Control Queue Egress Drop Threshold \n");
    diag_util_mprintf("Egress-Queue       Drop-Threshold \n");
    diag_util_mprintf("---------------------------------\n");

    switch (DIAG_UTIL_CHIP_TYPE) 
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            DIAG_UTIL_MASK_SCAN(queuelist, queue)
            {
                DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_queueEegressDropThreshold_get(queue, &threshold), ret);
                diag_util_mprintf("%-18u  %u\n", queue, threshold);
            }
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_MASK_SCAN(queuelist, queue)
            {
                DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_queueEegressDropThreshold_get(queue, &threshold), ret);
                diag_util_mprintf("%-18u  %u\n", queue, threshold);
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9601B)
        case RTL9601B_CHIP_ID:
            DIAG_UTIL_MASK_SCAN(queuelist, queue)
            {
                DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_queueEegressDropThreshold_get(queue, &threshold), ret);
                diag_util_mprintf("%-18u  %u\n", queue, threshold);
            }
            break;
#endif

#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
            DIAG_UTIL_MASK_SCAN(queuelist, queue)
            {
                DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_queueEegressDropThreshold_get(queue, &threshold), ret);
                diag_util_mprintf("%-18u  %u\n", queue, threshold);
            }
            break;
#endif

#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
            DIAG_UTIL_MASK_SCAN(queuelist, queue)
            {
                DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_queueEegressDropThreshold_get(queue, &threshold), ret);
                diag_util_mprintf("%-18u  %u\n", queue, threshold);
            }
            break;
#endif

#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
            DIAG_UTIL_MASK_SCAN(queuelist, queue)
            {
                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_queueEegressDropThreshold_get(queue, &threshold), ret);
                diag_util_mprintf("%-18u  %u\n", queue, threshold);
            }
            break;
#endif

        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_get_ingress_egress_drop_queue_id_qid_all_threshold */

/*
 * flowctrl set ingress egress-drop ( port-gap | queue-gap ) threshold <UINT:threshold>
 */
cparser_result_t
cparser_cmd_flowctrl_set_ingress_egress_drop_port_gap_queue_gap_threshold_threshold(
                                                                                   cparser_context_t *context,
                                                                                   uint32_t  *threshold_ptr)
{
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    switch (DIAG_UTIL_CHIP_TYPE) {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            {
                if ('p'==TOKEN_CHAR(4,0)) {
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_portEegressGapThreshold_set(*threshold_ptr), ret);
                    diag_util_printf("Set Ingress Flow Control Port Egress Gap Threshold to %u \n", *threshold_ptr);
                } else if ('q'==TOKEN_CHAR(4,0)) {
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_queueEegressGapThreshold_set(*threshold_ptr), ret);
                    diag_util_printf("Set Ingress Flow Control Queue Egress Gap Threshold to %u \n", *threshold_ptr);
                } else
                    return RT_ERR_FAILED;
            }
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            {
                if ('p'==TOKEN_CHAR(4,0)) {
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_portEegressGapThreshold_set(*threshold_ptr), ret);
                    diag_util_printf("Set Ingress Flow Control Port Egress Gap Threshold to %u \n", *threshold_ptr);
                } else if ('q'==TOKEN_CHAR(4,0)) {
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_queueEegressGapThreshold_set(*threshold_ptr), ret);
                    diag_util_printf("Set Ingress Flow Control Queue Egress Gap Threshold to %u \n", *threshold_ptr);
                } else
                    return RT_ERR_FAILED;
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9601B)
        case RTL9601B_CHIP_ID:
            {
                if ('p'==TOKEN_CHAR(4,0)) {
                    DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_portEegressGapThreshold_set(*threshold_ptr), ret);
                    diag_util_printf("Set Ingress Flow Control Port Egress Gap Threshold to %u \n", *threshold_ptr);
                } else if ('q'==TOKEN_CHAR(4,0)) {
                    DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_queueEegressGapThreshold_set(*threshold_ptr), ret);
                    diag_util_printf("Set Ingress Flow Control Queue Egress Gap Threshold to %u \n", *threshold_ptr);
                } else
                    return RT_ERR_FAILED;
            }
            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_set_ingress_egress_drop_port_gap_queue_gap_threshold_threshold */

/*
 * flowctrl get ingress egress-drop ( port-gap | queue-gap ) threshold
 */
cparser_result_t
cparser_cmd_flowctrl_get_ingress_egress_drop_port_gap_queue_gap_threshold(
                                                                         cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint32 threshold;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch (DIAG_UTIL_CHIP_TYPE) {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            {
                if ('p'==TOKEN_CHAR(4,0)) {
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_portEegressGapThreshold_get(&threshold), ret);
                    diag_util_mprintf("Ingress Flow Control Port Egress Gap Threshold is %u \n", threshold);
                } else if ('q'==TOKEN_CHAR(4,0)) {
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_queueEegressGapThreshold_get(&threshold), ret);
                    diag_util_mprintf("Ingress Flow Control Queue Egress Gap Threshold is %u \n", threshold);
                } else
                    return RT_ERR_FAILED;
            }
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            {
                if ('p'==TOKEN_CHAR(4,0)) {
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_portEegressGapThreshold_get(&threshold), ret);
                    diag_util_mprintf("Ingress Flow Control Port Egress Gap Threshold is %u \n", threshold);
                } else if ('q'==TOKEN_CHAR(4,0)) {
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_queueEegressGapThreshold_get(&threshold), ret);
                    diag_util_mprintf("Ingress Flow Control Queue Egress Gap Threshold is %u \n", threshold);
                } else
                    return RT_ERR_FAILED;
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9601B)
        case RTL9601B_CHIP_ID:
            {
                if ('p'==TOKEN_CHAR(4,0)) {
                    DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_portEegressGapThreshold_get(&threshold), ret);
                    diag_util_mprintf("Ingress Flow Control Port Egress Gap Threshold is %u \n", threshold);
                } else if ('q'==TOKEN_CHAR(4,0)) {
                    DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_queueEegressGapThreshold_get(&threshold), ret);
                    diag_util_mprintf("Ingress Flow Control Queue Egress Gap Threshold is %u \n", threshold);
                } else
                    return RT_ERR_FAILED;
            }
            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_get_ingress_egress_drop_port_gap_queue_gap_threshold */

/*
 * flowctrl set ingress egress-drop port ( <PORT_LIST:ports> | all ) queue-id ( <MASK_LIST:qid> | all ) drop ( enable | disable )
 */
cparser_result_t
cparser_cmd_flowctrl_set_ingress_egress_drop_port_ports_all_queue_id_qid_all_drop_enable_disable(
                                                                                                cparser_context_t *context,
                                                                                                char * *ports_ptr,
                                                                                                char * *qid_ptr)
{
    diag_portlist_t portlist;
    diag_mask_t  queuelist;
    rtk_port_t port;
    rtk_qid_t queue;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_QUEUEMASK(queuelist, 7), ret);

    diag_util_printf("Set Ingress Flow Control Queue Egress Drop Enable \n");
    diag_util_printf("            ");
    DIAG_UTIL_MASK_SCAN(queuelist, queue)
    {
        diag_util_printf("Q%d    ",queue);
    }
    diag_util_printf("\n");
    diag_util_printf("---------------------------------------------------------\n");
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        diag_util_printf("Port%2u  ", port);
        DIAG_UTIL_MASK_SCAN(queuelist, queue)
        {
            switch (DIAG_UTIL_CHIP_TYPE) {
#if defined(CONFIG_SDK_APOLLO)
                case APOLLO_CHIP_ID:
                    {
                        if ('e'==TOKEN_CHAR(9,0)) {
                            DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_egressDropEnable_set(port, queue, ENABLED), ret);
                            diag_util_printf("    En");
                        } else if ('d'==TOKEN_CHAR(9,0)) {
                            DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_egressDropEnable_set(port, queue, DISABLED), ret);
                            diag_util_printf("   Dis");
                        } else
                            return RT_ERR_FAILED;
                    }
                    break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
                case APOLLOMP_CHIP_ID:
                    {
                        if ('e'==TOKEN_CHAR(9,0)) {
                            DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_egressDropEnable_set(port, queue, ENABLED), ret);
                            diag_util_printf("    En");
                        } else if ('d'==TOKEN_CHAR(9,0)) {
                            DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_egressDropEnable_set(port, queue, DISABLED), ret);
                            diag_util_printf("   Dis");
                        } else
                            return RT_ERR_FAILED;
                    }
                    break;
#endif
#if defined(CONFIG_SDK_RTL9601B)
                case RTL9601B_CHIP_ID:
                    {
                        if ('e'==TOKEN_CHAR(9,0)) {
                            DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_egressDropEnable_set(port, queue, ENABLED), ret);
                            diag_util_printf("    En");
                        } else if ('d'==TOKEN_CHAR(9,0)) {
                            DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_egressDropEnable_set(port, queue, DISABLED), ret);
                            diag_util_printf("   Dis");
                        } else
                            return RT_ERR_FAILED;
                    }
                    break;
#endif

#if defined(CONFIG_SDK_RTL9602C)
                case RTL9602C_CHIP_ID:
                    {
                        if ('e'==TOKEN_CHAR(9,0)) {
                            DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_egressDropEnable_set(port, queue, ENABLED), ret);
                            diag_util_printf("    En");
                        } else if ('d'==TOKEN_CHAR(9,0)) {
                            DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_egressDropEnable_set(port, queue, DISABLED), ret);
                            diag_util_printf("   Dis");
                        } else
                            return RT_ERR_FAILED;
                    }
                    break;
#endif

#if defined(CONFIG_SDK_RTL9607C)
                case RTL9607C_CHIP_ID:
                    {
                        if ('e'==TOKEN_CHAR(9,0)) {
                            DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_egressDropEnable_set(port, queue, ENABLED), ret);
                            diag_util_printf("    En");
                        } else if ('d'==TOKEN_CHAR(9,0)) {
                            DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_egressDropEnable_set(port, queue, DISABLED), ret);
                            diag_util_printf("   Dis");
                        } else
                            return RT_ERR_FAILED;
                    }
                    break;
#endif

#if defined(CONFIG_SDK_RTL9603CVD)
                case RTL9603CVD_CHIP_ID:
                    {
                        if ('e'==TOKEN_CHAR(9,0)) {
                            DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_egressDropEnable_set(port, queue, ENABLED), ret);
                            diag_util_printf("    En");
                        } else if ('d'==TOKEN_CHAR(9,0)) {
                            DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_egressDropEnable_set(port, queue, DISABLED), ret);
                            diag_util_printf("   Dis");
                        } else
                            return RT_ERR_FAILED;
                    }
                    break;
#endif
                default:
                    diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
                    return CPARSER_NOT_OK;
                    break;
            }
        }
        diag_util_printf("\n");
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_set_ingress_egress_drop_port_ports_all_queue_id_qid_all_drop_enable_disable */

/*
 * flowctrl get ingress egress-drop port ( <PORT_LIST:ports> | all ) queue-id ( <MASK_LIST:qid> | all ) drop
 */
cparser_result_t
cparser_cmd_flowctrl_get_ingress_egress_drop_port_ports_all_queue_id_qid_all_drop(
                                                                                 cparser_context_t *context,
                                                                                 char * *ports_ptr,
                                                                                 char * *qid_ptr)
{
    diag_portlist_t portlist;
    diag_mask_t  queuelist;
    rtk_port_t port;
    rtk_qid_t queue;
    rtk_enable_t enable;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_QUEUEMASK(queuelist, 7), ret);

    diag_util_mprintf("Get Ingress Flow Control Queue Egress Drop Enable \n");
    DIAG_UTIL_MASK_SCAN(queuelist, queue)
    {
        diag_util_printf("Q%d    ",queue);
    }
    diag_util_mprintf("\n");
    diag_util_mprintf("---------------------------------------------------------\n");
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        diag_util_printf("Port %2u ", port);
        DIAG_UTIL_MASK_SCAN(queuelist, queue)
        {
            switch (DIAG_UTIL_CHIP_TYPE) {
#if defined(CONFIG_SDK_APOLLO)
                case APOLLO_CHIP_ID:
                    {
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_egressDropEnable_get(port, queue, &enable), ret);
                    }
                    break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
                case APOLLOMP_CHIP_ID:
                    {
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_egressDropEnable_get(port, queue, &enable), ret);
                    }
                    break;
#endif
#if defined(CONFIG_SDK_RTL9601B)
                case RTL9601B_CHIP_ID:
                    {
                        DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_egressDropEnable_get(port, queue, &enable), ret);
                    }
                    break;
#endif

#if defined(CONFIG_SDK_RTL9602C)
                case RTL9602C_CHIP_ID:
                    {
                        DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_egressDropEnable_get(port, queue, &enable), ret);
                    }
                    break;
#endif

#if defined(CONFIG_SDK_RTL9607C)
                case RTL9607C_CHIP_ID:
                    {
                        DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_egressDropEnable_get(port, queue, &enable), ret);
                    }
                    break;
#endif

#if defined(CONFIG_SDK_RTL9603CVD)
                case RTL9603CVD_CHIP_ID:
                    {
                        DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_egressDropEnable_get(port, queue, &enable), ret);
                    }
                    break;
#endif

                default:
                    diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
                    return CPARSER_NOT_OK;
                    break;
            }
#if defined(CONFIG_SDK_APOLLOMP) || defined(CONFIG_SDK_RTL9601B) || defined(CONFIG_SDK_RTL9602C) || defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
            diag_util_printf("   %3s",enable?"En":"Dis");
#endif
        }
#if defined(CONFIG_SDK_APOLLOMP) || defined(CONFIG_SDK_RTL9601B) || defined(CONFIG_SDK_RTL9602C) || defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
        diag_util_mprintf("\n");
#endif
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_get_ingress_egress_drop_port_ports_all_queue_id_qid_all_drop */

/*
 * flowctrl set ingress pon system ( high-off | high-on | low-off | low-on | drop-high-off | drop-high-on | drop-low-off | drop-low-on) threshold <UINT:threshold>
*/
cparser_result_t
cparser_cmd_flowctrl_set_ingress_pon_system_high_off_high_on_low_off_low_on_drop_high_off_drop_high_on_drop_low_off_drop_low_on_threshold_threshold(
                                                                                               cparser_context_t *context,
                                                                                               uint32_t  *threshold_ptr)
{
    uint32 onThresh, offThresh;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch (DIAG_UTIL_CHIP_TYPE) {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            {
                if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_ponGlobalHighThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_ponGlobalHighThreshold_set(onThresh, *threshold_ptr), ret);
                    diag_util_printf("Set PON MAC Global High Off Threshold to %u\n",*threshold_ptr);
                } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_ponGlobalHighThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_ponGlobalHighThreshold_set(*threshold_ptr, offThresh), ret);
                    diag_util_printf("Set PON MAC Global High On Threshold to %u\n",*threshold_ptr);
                } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_ponGlobalLowThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_ponGlobalLowThreshold_set(onThresh, *threshold_ptr), ret);
                    diag_util_printf("Set PON MAC Global Low Off Threshold to %u\n",*threshold_ptr);
                } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_ponGlobalLowThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_ponGlobalLowThreshold_set(*threshold_ptr, offThresh), ret);
                    diag_util_printf("Set PON MAC Global Low On Threshold to %u\n",*threshold_ptr);
                }
            }
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            {
                if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_ponGlobalHighThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_ponGlobalHighThreshold_set(onThresh, *threshold_ptr), ret);
                    diag_util_printf("Set PON MAC Global High Off Threshold to %u\n",*threshold_ptr);
                } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_ponGlobalHighThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_ponGlobalHighThreshold_set(*threshold_ptr, offThresh), ret);
                    diag_util_printf("Set PON MAC Global High On Threshold to %u\n",*threshold_ptr);
                } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_ponGlobalLowThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_ponGlobalLowThreshold_set(onThresh, *threshold_ptr), ret);
                    diag_util_printf("Set PON MAC Global Low Off Threshold to %u\n",*threshold_ptr);
                } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_ponGlobalLowThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_ponGlobalLowThreshold_set(*threshold_ptr, offThresh), ret);
                    diag_util_printf("Set PON MAC Global Low On Threshold to %u\n",*threshold_ptr);
                }
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9601B)
        case RTL9601B_CHIP_ID:
            {
                if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                    DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_ponGlobalHighThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_ponGlobalHighThreshold_set(onThresh, *threshold_ptr), ret);
                    diag_util_printf("Set PON MAC Global High Off Threshold to %u\n",*threshold_ptr);
                } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                    DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_ponGlobalHighThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_ponGlobalHighThreshold_set(*threshold_ptr, offThresh), ret);
                    diag_util_printf("Set PON MAC Global High On Threshold to %u\n",*threshold_ptr);
                } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                    DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_ponGlobalLowThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_ponGlobalLowThreshold_set(onThresh, *threshold_ptr), ret);
                    diag_util_printf("Set PON MAC Global Low Off Threshold to %u\n",*threshold_ptr);
                } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                    DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_ponGlobalLowThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_ponGlobalLowThreshold_set(*threshold_ptr, offThresh), ret);
                    diag_util_printf("Set PON MAC Global Low On Threshold to %u\n",*threshold_ptr);
                }
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
            {
                if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                    DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_ponGlobalHighThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_ponGlobalHighThreshold_set(onThresh, *threshold_ptr), ret);
                    diag_util_printf("Set PON MAC Global High Off Threshold to %u\n",*threshold_ptr);
                } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                    DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_ponGlobalHighThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_ponGlobalHighThreshold_set(*threshold_ptr, offThresh), ret);
                    diag_util_printf("Set PON MAC Global High On Threshold to %u\n",*threshold_ptr);
                } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                    DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_ponGlobalLowThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_ponGlobalLowThreshold_set(onThresh, *threshold_ptr), ret);
                    diag_util_printf("Set PON MAC Global Low Off Threshold to %u\n",*threshold_ptr);
                } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                    DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_ponGlobalLowThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_ponGlobalLowThreshold_set(*threshold_ptr, offThresh), ret);
                    diag_util_printf("Set PON MAC Global Low On Threshold to %u\n",*threshold_ptr);
                }
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
            {
                if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_ponGlobalHighThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_ponGlobalHighThreshold_set(onThresh, *threshold_ptr), ret);
                    diag_util_printf("Set PON MAC Global High Off Threshold to %u\n",*threshold_ptr);
                } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_ponGlobalHighThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_ponGlobalHighThreshold_set(*threshold_ptr, offThresh), ret);
                    diag_util_printf("Set PON MAC Global High On Threshold to %u\n",*threshold_ptr);
                } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_ponGlobalLowThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_ponGlobalLowThreshold_set(onThresh, *threshold_ptr), ret);
                    diag_util_printf("Set PON MAC Global Low Off Threshold to %u\n",*threshold_ptr);
                } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_ponGlobalLowThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_ponGlobalLowThreshold_set(*threshold_ptr, offThresh), ret);
                    diag_util_printf("Set PON MAC Global Low On Threshold to %u\n",*threshold_ptr);
                }

                if (!osal_strcmp(TOKEN_STR(5),"drop-high-off")) {
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_ponGlobalDropHighThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_ponGlobalDropHighThreshold_set(onThresh, *threshold_ptr), ret);
                    diag_util_printf("Set PON MAC Global Drop High Off Threshold to %u\n",*threshold_ptr);
                } else if (!osal_strcmp(TOKEN_STR(5),"drop-high-on")) {
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_ponGlobalDropHighThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_ponGlobalDropHighThreshold_set(*threshold_ptr, offThresh), ret);
                    diag_util_printf("Set PON MAC Global Drop High On Threshold to %u\n",*threshold_ptr);
                } else if (!osal_strcmp(TOKEN_STR(5),"drop-low-off")) {
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_ponGlobalDropLowThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_ponGlobalDropLowThreshold_set(onThresh, *threshold_ptr), ret);
                    diag_util_printf("Set PON MAC Global Drop Low Off Threshold to %u\n",*threshold_ptr);
                } else if (!osal_strcmp(TOKEN_STR(5),"drop-low-on")) {
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_ponGlobalDropLowThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_ponGlobalDropLowThreshold_set(*threshold_ptr, offThresh), ret);
                    diag_util_printf("Set PON MAC Global Drop Low On Threshold to %u\n",*threshold_ptr);
                }
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
            {
                if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_ponGlobalHighThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_ponGlobalHighThreshold_set(onThresh, *threshold_ptr), ret);
                    diag_util_printf("Set PON MAC Global High Off Threshold to %u\n",*threshold_ptr);
                } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_ponGlobalHighThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_ponGlobalHighThreshold_set(*threshold_ptr, offThresh), ret);
                    diag_util_printf("Set PON MAC Global High On Threshold to %u\n",*threshold_ptr);
                } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_ponGlobalLowThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_ponGlobalLowThreshold_set(onThresh, *threshold_ptr), ret);
                    diag_util_printf("Set PON MAC Global Low Off Threshold to %u\n",*threshold_ptr);
                } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_ponGlobalLowThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_ponGlobalLowThreshold_set(*threshold_ptr, offThresh), ret);
                    diag_util_printf("Set PON MAC Global Low On Threshold to %u\n",*threshold_ptr);
                }

                if (!osal_strcmp(TOKEN_STR(5),"drop-high-off")) {
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_ponGlobalDropHighThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_ponGlobalDropHighThreshold_set(onThresh, *threshold_ptr), ret);
                    diag_util_printf("Set PON MAC Global Drop High Off Threshold to %u\n",*threshold_ptr);
                } else if (!osal_strcmp(TOKEN_STR(5),"drop-high-on")) {
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_ponGlobalDropHighThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_ponGlobalDropHighThreshold_set(*threshold_ptr, offThresh), ret);
                    diag_util_printf("Set PON MAC Global Drop High On Threshold to %u\n",*threshold_ptr);
                } else if (!osal_strcmp(TOKEN_STR(5),"drop-low-off")) {
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_ponGlobalDropLowThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_ponGlobalDropLowThreshold_set(onThresh, *threshold_ptr), ret);
                    diag_util_printf("Set PON MAC Global Drop Low Off Threshold to %u\n",*threshold_ptr);
                } else if (!osal_strcmp(TOKEN_STR(5),"drop-low-on")) {
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_ponGlobalDropLowThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_ponGlobalDropLowThreshold_set(*threshold_ptr, offThresh), ret);
                    diag_util_printf("Set PON MAC Global Drop Low On Threshold to %u\n",*threshold_ptr);
                }
            }
            break;
#endif

        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_set_ingress_pon_system_high_off_high_on_low_off_low_on_drop_high_off_drop_high_on_drop_low_off_drop_low_on_threshold_threshold */

/*
 * flowctrl get ingress pon system ( high-off | high-on | low-off | low-on | drop-high-off | drop-high-on | drop-low-off | drop-low-on ) threshold
 */
cparser_result_t
cparser_cmd_flowctrl_get_ingress_pon_system_high_off_high_on_low_off_low_on_drop_high_off_drop_high_on_drop_low_off_drop_low_on_threshold(
                                                                                     cparser_context_t *context)
{
    uint32 onThresh, offThresh;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch (DIAG_UTIL_CHIP_TYPE) {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            {
                if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_ponGlobalHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("PON MAC Global High Off Threshold is %u\n",offThresh);
                } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_ponGlobalHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("PON MAC Global High On Threshold is %u\n",onThresh);
                } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_ponGlobalLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("PON MAC Global Low Off Threshold is %u\n",offThresh);
                } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_ponGlobalLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("PON MAC Global Low On Threshold is %u\n",onThresh);
                }
            }
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            {
                if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_ponGlobalHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("PON MAC Global High Off Threshold is %u\n",offThresh);
                } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_ponGlobalHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("PON MAC Global High On Threshold is %u\n",onThresh);
                } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_ponGlobalLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("PON MAC Global Low Off Threshold is %u\n",offThresh);
                } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_ponGlobalLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("PON MAC Global Low On Threshold is %u\n",onThresh);
                }
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9601B)
        case RTL9601B_CHIP_ID:
            {
                if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                    DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_ponGlobalHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("PON MAC Global High Off Threshold is %u\n",offThresh);
                } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                    DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_ponGlobalHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("PON MAC Global High On Threshold is %u\n",onThresh);
                } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                    DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_ponGlobalLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("PON MAC Global Low Off Threshold is %u\n",offThresh);
                } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                    DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_ponGlobalLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("PON MAC Global Low On Threshold is %u\n",onThresh);
                }
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
            {
                if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                    DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_ponGlobalHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("PON MAC Global High Off Threshold is %u\n",offThresh);
                } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                    DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_ponGlobalHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("PON MAC Global High On Threshold is %u\n",onThresh);
                } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                    DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_ponGlobalLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("PON MAC Global Low Off Threshold is %u\n",offThresh);
                } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                    DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_ponGlobalLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("PON MAC Global Low On Threshold is %u\n",onThresh);
                }
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
            {
                if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_ponGlobalHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("PON MAC Global High Off Threshold is %u\n",offThresh);
                } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_ponGlobalHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("PON MAC Global High On Threshold is %u\n",onThresh);
                } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_ponGlobalLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("PON MAC Global Low Off Threshold is %u\n",offThresh);
                } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_ponGlobalLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("PON MAC Global Low On Threshold is %u\n",onThresh);
                }
                if (!osal_strcmp(TOKEN_STR(5),"drop-high-off")) {
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_ponGlobalDropHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("PON MAC Global Drop High Off Threshold is %u\n",offThresh);
                } else if (!osal_strcmp(TOKEN_STR(5),"drop-high-on")) {
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_ponGlobalDropHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("PON MAC Global Drop High On Threshold is %u\n",onThresh);
                } else if (!osal_strcmp(TOKEN_STR(5),"drop-low-off")) {
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_ponGlobalDropLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("PON MAC Global Drop Low Off Threshold is %u\n",offThresh);
                } else if (!osal_strcmp(TOKEN_STR(5),"drop-low-on")) {
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_ponGlobalDropLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("PON MAC Global Drop Low On Threshold is %u\n",onThresh);
                }
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
            {
                if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_ponGlobalHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("PON MAC Global High Off Threshold is %u\n",offThresh);
                } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_ponGlobalHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("PON MAC Global High On Threshold is %u\n",onThresh);
                } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_ponGlobalLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("PON MAC Global Low Off Threshold is %u\n",offThresh);
                } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_ponGlobalLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("PON MAC Global Low On Threshold is %u\n",onThresh);
                }
                if (!osal_strcmp(TOKEN_STR(5),"drop-high-off")) {
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_ponGlobalDropHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("PON MAC Global Drop High Off Threshold is %u\n",offThresh);
                } else if (!osal_strcmp(TOKEN_STR(5),"drop-high-on")) {
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_ponGlobalDropHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("PON MAC Global Drop High On Threshold is %u\n",onThresh);
                } else if (!osal_strcmp(TOKEN_STR(5),"drop-low-off")) {
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_ponGlobalDropLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("PON MAC Global Drop Low Off Threshold is %u\n",offThresh);
                } else if (!osal_strcmp(TOKEN_STR(5),"drop-low-on")) {
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_ponGlobalDropLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("PON MAC Global Drop Low On Threshold is %u\n",onThresh);
                }
            }
            break;
#endif

        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_get_ingress_pon_system_high_off_high_on_low_off_low_on_drop_high_off_drop_high_on_drop_low_off_drop_low_on_threshold */

/*
 * flowctrl set ingress pon port ( high-off | high-on | low-off | low-on | drop-high-off | drop-high-on | drop-low-off | drop-low-on) threshold <UINT:threshold>
 */
cparser_result_t
cparser_cmd_flowctrl_set_ingress_pon_port_high_off_high_on_low_off_low_on_drop_high_off_drop_high_on_drop_low_off_drop_low_on_threshold_threshold(
                                                                                             cparser_context_t *context,
                                                                                             uint32_t  *threshold_ptr)
{
    uint32 onThresh, offThresh;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch (DIAG_UTIL_CHIP_TYPE) {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            {
                if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_ponPortHighThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_ponPortHighThreshold_set(onThresh, *threshold_ptr), ret);
                    diag_util_printf("Set PON MAC Port High Off Threshold to %u\n",*threshold_ptr);
                } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_ponPortHighThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_ponPortHighThreshold_set(*threshold_ptr, offThresh), ret);
                    diag_util_printf("Set PON MAC Port High On Threshold to %u\n",*threshold_ptr);
                } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_ponPortLowThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_ponPortLowThreshold_set(onThresh, *threshold_ptr), ret);
                    diag_util_printf("Set PON MAC Port Low Off Threshold to %u\n",*threshold_ptr);
                } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_ponPortLowThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_ponPortLowThreshold_set(*threshold_ptr, offThresh), ret);
                    diag_util_printf("Set PON MAC Port Low On Threshold to %u\n",*threshold_ptr);
                }
            }
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            {
                if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_ponPortHighThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_ponPortHighThreshold_set(onThresh, *threshold_ptr), ret);
                    diag_util_printf("Set PON MAC Port High Off Threshold to %u\n",*threshold_ptr);
                } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_ponPortHighThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_ponPortHighThreshold_set(*threshold_ptr, offThresh), ret);
                    diag_util_printf("Set PON MAC Port High On Threshold to %u\n",*threshold_ptr);
                } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_ponPortLowThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_ponPortLowThreshold_set(onThresh, *threshold_ptr), ret);
                    diag_util_printf("Set PON MAC Port Low Off Threshold to %u\n",*threshold_ptr);
                } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_ponPortLowThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_ponPortLowThreshold_set(*threshold_ptr, offThresh), ret);
                    diag_util_printf("Set PON MAC Port Low On Threshold to %u\n",*threshold_ptr);
                }
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9601B)
        case RTL9601B_CHIP_ID:
            {
                if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                    DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_ponPortHighThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_ponPortHighThreshold_set(onThresh, *threshold_ptr), ret);
                    diag_util_printf("Set PON MAC Port High Off Threshold to %u\n",*threshold_ptr);
                } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                    DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_ponPortHighThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_ponPortHighThreshold_set(*threshold_ptr, offThresh), ret);
                    diag_util_printf("Set PON MAC Port High On Threshold to %u\n",*threshold_ptr);
                } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                    DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_ponPortLowThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_ponPortLowThreshold_set(onThresh, *threshold_ptr), ret);
                    diag_util_printf("Set PON MAC Port Low Off Threshold to %u\n",*threshold_ptr);
                } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                    DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_ponPortLowThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_ponPortLowThreshold_set(*threshold_ptr, offThresh), ret);
                    diag_util_printf("Set PON MAC Port Low On Threshold to %u\n",*threshold_ptr);
                }
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
            {
                if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                    DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_ponPortHighThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_ponPortHighThreshold_set(onThresh, *threshold_ptr), ret);
                    diag_util_printf("Set PON MAC Port High Off Threshold to %u\n",*threshold_ptr);
                } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                    DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_ponPortHighThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_ponPortHighThreshold_set(*threshold_ptr, offThresh), ret);
                    diag_util_printf("Set PON MAC Port High On Threshold to %u\n",*threshold_ptr);
                } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                    DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_ponPortLowThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_ponPortLowThreshold_set(onThresh, *threshold_ptr), ret);
                    diag_util_printf("Set PON MAC Port Low Off Threshold to %u\n",*threshold_ptr);
                } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                    DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_ponPortLowThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_ponPortLowThreshold_set(*threshold_ptr, offThresh), ret);
                    diag_util_printf("Set PON MAC Port Low On Threshold to %u\n",*threshold_ptr);
                }
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
            {
                if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_ponPortHighThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_ponPortHighThreshold_set(onThresh, *threshold_ptr), ret);
                    diag_util_printf("Set PON MAC Port High Off Threshold to %u\n",*threshold_ptr);
                } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_ponPortHighThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_ponPortHighThreshold_set(*threshold_ptr, offThresh), ret);
                    diag_util_printf("Set PON MAC Port High On Threshold to %u\n",*threshold_ptr);
                } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_ponPortLowThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_ponPortLowThreshold_set(onThresh, *threshold_ptr), ret);
                    diag_util_printf("Set PON MAC Port Low Off Threshold to %u\n",*threshold_ptr);
                } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_ponPortLowThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_ponPortLowThreshold_set(*threshold_ptr, offThresh), ret);
                    diag_util_printf("Set PON MAC Port Low On Threshold to %u\n",*threshold_ptr);
                }
                if (!osal_strcmp(TOKEN_STR(5),"drop-high-off")) {
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_ponPortDropHighThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_ponPortDropHighThreshold_set(onThresh, *threshold_ptr), ret);
                    diag_util_printf("Set PON MAC Port Drop High Off Threshold to %u\n",*threshold_ptr);
                } else if (!osal_strcmp(TOKEN_STR(5),"drop-high-on")) {
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_ponPortDropHighThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_ponPortDropHighThreshold_set(*threshold_ptr, offThresh), ret);
                    diag_util_printf("Set PON MAC Port Drop High On Threshold to %u\n",*threshold_ptr);
                } else if (!osal_strcmp(TOKEN_STR(5),"drop-low-off")) {
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_ponPortDropLowThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_ponPortDropLowThreshold_set(onThresh, *threshold_ptr), ret);
                    diag_util_printf("Set PON MAC Port Drop Low Off Threshold to %u\n",*threshold_ptr);
                } else if (!osal_strcmp(TOKEN_STR(5),"drop-low-on")) {
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_ponPortDropLowThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_ponPortDropLowThreshold_set(*threshold_ptr, offThresh), ret);
                    diag_util_printf("Set PON MAC Port Drop Low On Threshold to %u\n",*threshold_ptr);
				}
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
            {
                if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_ponPortHighThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_ponPortHighThreshold_set(onThresh, *threshold_ptr), ret);
                    diag_util_printf("Set PON MAC Port High Off Threshold to %u\n",*threshold_ptr);
                } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_ponPortHighThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_ponPortHighThreshold_set(*threshold_ptr, offThresh), ret);
                    diag_util_printf("Set PON MAC Port High On Threshold to %u\n",*threshold_ptr);
                } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_ponPortLowThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_ponPortLowThreshold_set(onThresh, *threshold_ptr), ret);
                    diag_util_printf("Set PON MAC Port Low Off Threshold to %u\n",*threshold_ptr);
                } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_ponPortLowThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_ponPortLowThreshold_set(*threshold_ptr, offThresh), ret);
                    diag_util_printf("Set PON MAC Port Low On Threshold to %u\n",*threshold_ptr);
                }
                if (!osal_strcmp(TOKEN_STR(5),"drop-high-off")) {
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_ponPortDropHighThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_ponPortDropHighThreshold_set(onThresh, *threshold_ptr), ret);
                    diag_util_printf("Set PON MAC Port Drop High Off Threshold to %u\n",*threshold_ptr);
                } else if (!osal_strcmp(TOKEN_STR(5),"drop-high-on")) {
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_ponPortDropHighThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_ponPortDropHighThreshold_set(*threshold_ptr, offThresh), ret);
                    diag_util_printf("Set PON MAC Port Drop High On Threshold to %u\n",*threshold_ptr);
                } else if (!osal_strcmp(TOKEN_STR(5),"drop-low-off")) {
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_ponPortDropLowThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_ponPortDropLowThreshold_set(onThresh, *threshold_ptr), ret);
                    diag_util_printf("Set PON MAC Port Drop Low Off Threshold to %u\n",*threshold_ptr);
                } else if (!osal_strcmp(TOKEN_STR(5),"drop-low-on")) {
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_ponPortDropLowThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_ponPortDropLowThreshold_set(*threshold_ptr, offThresh), ret);
                    diag_util_printf("Set PON MAC Port Drop Low On Threshold to %u\n",*threshold_ptr);
                }
            }
            break;
#endif

        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_set_ingress_pon_port_high_off_high_on_low_off_low_on_drop_high_off_drop_high_on_drop_low_off_drop_low_on_threshold_threshold */

/*
 * flowctrl get ingress pon port ( high-off | high-on | low-off | low-on | drop-high-off | drop-high-on | drop-low-off | drop-low-on ) threshold
 */
cparser_result_t
cparser_cmd_flowctrl_get_ingress_pon_port_high_off_high_on_low_off_low_on_drop_high_off_drop_high_on_drop_low_off_drop_low_on_threshold(
                                                                                   cparser_context_t *context)
{
    uint32 onThresh, offThresh;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch (DIAG_UTIL_CHIP_TYPE) {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            {
                if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_ponPortHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("PON MAC Port High Off Threshold is %u\n",offThresh);
                } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_ponPortHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("PON MAC Port High On Threshold is %u\n",onThresh);
                } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_ponPortLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("PON MAC Port Low Off Threshold is %u\n",offThresh);
                } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_ponPortLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("PON MAC Port Low On Threshold is %u\n",onThresh);
                }
            }
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            {
                if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_ponPortHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("PON MAC Port High Off Threshold is %u\n",offThresh);
                } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_ponPortHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("PON MAC Port High On Threshold is %u\n",onThresh);
                } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_ponPortLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("PON MAC Port Low Off Threshold is %u\n",offThresh);
                } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_ponPortLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("PON MAC Port Low On Threshold is %u\n",onThresh);
                }
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9601B)
        case RTL9601B_CHIP_ID:
            {
                if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                    DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_ponPortHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("PON MAC Port High Off Threshold is %u\n",offThresh);
                } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                    DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_ponPortHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("PON MAC Port High On Threshold is %u\n",onThresh);
                } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                    DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_ponPortLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("PON MAC Port Low Off Threshold is %u\n",offThresh);
                } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                    DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_ponPortLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("PON MAC Port Low On Threshold is %u\n",onThresh);
                }
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
            {
                if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                    DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_ponPortHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("PON MAC Port High Off Threshold is %u\n",offThresh);
                } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                    DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_ponPortHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("PON MAC Port High On Threshold is %u\n",onThresh);
                } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                    DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_ponPortLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("PON MAC Port Low Off Threshold is %u\n",offThresh);
                } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                    DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_ponPortLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("PON MAC Port Low On Threshold is %u\n",onThresh);
                }
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
            {
                if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_ponPortHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("PON MAC Port High Off Threshold is %u\n",offThresh);
                } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_ponPortHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("PON MAC Port High On Threshold is %u\n",onThresh);
                } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_ponPortLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("PON MAC Port Low Off Threshold is %u\n",offThresh);
                } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_ponPortLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("PON MAC Port Low On Threshold is %u\n",onThresh);
                }
                if (!osal_strcmp(TOKEN_STR(5),"drop-high-off")) {
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_ponPortDropHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("PON MAC Port Drop High Off Threshold is %u\n",offThresh);
                } else if (!osal_strcmp(TOKEN_STR(5),"drop-high-on")) {
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_ponPortDropHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("PON MAC Port Drop High On Threshold is %u\n",onThresh);
                } else if (!osal_strcmp(TOKEN_STR(5),"drop-low-off")) {
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_ponPortDropLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("PON MAC Port Drop Low Off Threshold is %u\n",offThresh);
                } else if (!osal_strcmp(TOKEN_STR(5),"drop-low-on")) {
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_ponPortDropLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("PON MAC Port Drop Low On Threshold is %u\n",onThresh);
                }
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
            {
                if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_ponPortHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("PON MAC Port High Off Threshold is %u\n",offThresh);
                } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_ponPortHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("PON MAC Port High On Threshold is %u\n",onThresh);
                } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_ponPortLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("PON MAC Port Low Off Threshold is %u\n",offThresh);
                } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_ponPortLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("PON MAC Port Low On Threshold is %u\n",onThresh);
                }
                if (!osal_strcmp(TOKEN_STR(5),"drop-high-off")) {
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_ponPortDropHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("PON MAC Port Drop High Off Threshold is %u\n",offThresh);
                } else if (!osal_strcmp(TOKEN_STR(5),"drop-high-on")) {
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_ponPortDropHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("PON MAC Port Drop High On Threshold is %u\n",onThresh);
                } else if (!osal_strcmp(TOKEN_STR(5),"drop-low-off")) {
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_ponPortDropLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("PON MAC Port Drop Low Off Threshold is %u\n",offThresh);
                } else if (!osal_strcmp(TOKEN_STR(5),"drop-low-on")) {
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_ponPortDropLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("PON MAC Port Drop Low On Threshold is %u\n",onThresh);
                }
            }
            break;
#endif

        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_get_ingress_pon_port_high_off_high_on_low_off_low_on_drop_high_off_drop_high_on_drop_low_off_drop_low_on_threshold */

/*
 * flowctrl set ingress pon sid <UINT:sid> state ( enable | disable )
 */
cparser_result_t
cparser_cmd_flowctrl_set_ingress_pon_sid_sid_state_enable_disable(
                                                                 cparser_context_t *context,
                                                                 uint32_t  *sid_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_enable_t state = RTK_ENABLE_END;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    if ('e' == TOKEN_CHAR(7,0)) {
        state = ENABLED;
    } else if ('d' == TOKEN_CHAR(7,0)) {
        state = DISABLED;
    } else {
        RT_PARAM_CHK((RTK_ENABLE_END == state), CPARSER_ERR_INVALID_PARAMS);
    }

    switch (DIAG_UTIL_CHIP_TYPE) {
#if defined(CONFIG_SDK_RTL9601B)
        case RTL9601B_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_ponSidState_set(*sid_ptr, state), ret);
                diag_util_mprintf("PON SID %u State %s \n", *sid_ptr, diagStr_enable[state]);
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_ponSidState_set(*sid_ptr, state), ret);
                diag_util_mprintf("PON SID %u State %s \n", *sid_ptr, diagStr_enable[state]);
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_ponSidState_set(*sid_ptr, state), ret);
                diag_util_mprintf("PON SID %u State %s \n", *sid_ptr, diagStr_enable[state]);
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_ponSidState_set(*sid_ptr, state), ret);
                diag_util_mprintf("PON SID %u State %s \n", *sid_ptr, diagStr_enable[state]);
            }
            break;
#endif

        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_set_ingress_pon_sid_sid_state_enable_disable */

/*
 * flowctrl get ingress pon sid <UINT:sid> state
 */
cparser_result_t
cparser_cmd_flowctrl_get_ingress_pon_sid_sid_state(
                                                  cparser_context_t *context,
                                                  uint32_t  *sid_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_enable_t state;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch (DIAG_UTIL_CHIP_TYPE) {
#if defined(CONFIG_SDK_RTL9601B)
        case RTL9601B_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_ponSidState_get(*sid_ptr, &state), ret);
                diag_util_mprintf("PON SID %u State %s \n", *sid_ptr, diagStr_enable[state]);
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_ponSidState_get(*sid_ptr, &state), ret);
                diag_util_mprintf("PON SID %u State %s \n", *sid_ptr, diagStr_enable[state]);
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_ponSidState_get(*sid_ptr, &state), ret);
                diag_util_mprintf("PON SID %u State %s \n", *sid_ptr, diagStr_enable[state]);
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_ponSidState_get(*sid_ptr, &state), ret);
                diag_util_mprintf("PON SID %u State %s \n", *sid_ptr, diagStr_enable[state]);
            }
            break;
#endif

        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_get_ingress_pon_sid_sid_state */

/*
 * flowctrl set max-page-clear egress-port ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_flowctrl_set_max_page_clear_egress_port_ports_all(
                                                             cparser_context_t *context,
                                                             char * *ports_ptr)
{
    diag_portlist_t  portlist;
    rtk_bmp_t  mask;
    rtk_port_t port;
    uint8   buf[UTIL_PORT_MASK_BUFFER_LENGTH];
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    mask.bits[0] = 0;

    switch (DIAG_UTIL_CHIP_TYPE) 
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            DIAG_UTIL_PORTMASK_SCAN(portlist, port)
            {
                DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_debugCtrl_set(port, ENABLED, mask), ret);
            }
            diag_util_lPortMask2str(buf, &portlist.portmask);
            diag_util_printf("Port:%s Egress Port Max Page Clear.\n",buf);
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_PORTMASK_SCAN(portlist, port)
            {
                DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_debugCtrl_set(port, ENABLED, mask), ret);
            }
            diag_util_lPortMask2str(buf, &portlist.portmask);
            diag_util_printf("Port:%s Egress Port Max Page Clear.\n",buf);
                break;
#endif
#if defined(CONFIG_SDK_RTL9601B)
        case RTL9601B_CHIP_ID:
            DIAG_UTIL_PORTMASK_SCAN(portlist, port)
            {
                DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_debugCtrl_set(port, ENABLED, mask), ret);
            }
            diag_util_lPortMask2str(buf, &portlist.portmask);
            diag_util_printf("Port:%s Egress Port Max Page Clear.\n",buf);
            break;
#endif

#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
            DIAG_UTIL_PORTMASK_SCAN(portlist, port)
            {
                DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_debugCtrl_set(port, ENABLED, mask), ret);
            }
            diag_util_lPortMask2str(buf, &portlist.portmask);
            diag_util_printf("Port:%s Egress Port Max Page Clear.\n",buf);
            break;
#endif

#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
            DIAG_UTIL_PORTMASK_SCAN(portlist, port)
            {
                DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_debugCtrl_set(port, ENABLED, mask), ret);
            }
            diag_util_lPortMask2str(buf, &portlist.portmask);
            diag_util_printf("Port:%s Egress Port Max Page Clear.\n",buf);
            break;
#endif

#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
            DIAG_UTIL_PORTMASK_SCAN(portlist, port)
            {
                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_debugCtrl_set(port, ENABLED, mask), ret);
            }
            diag_util_lPortMask2str(buf, &portlist.portmask);
            diag_util_printf("Port:%s Egress Port Max Page Clear.\n",buf);
            break;
#endif

        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }


    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_set_max_page_clear_egress_port_ports_all */

/*
 * flowctrl set total-pktcnt-clear
 */
cparser_result_t
cparser_cmd_flowctrl_set_total_pktcnt_clear(
                                           cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch (DIAG_UTIL_CHIP_TYPE) {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_clrTotalPktCnt_set(ENABLED), ret);
            }
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_clrTotalPktCnt_set(ENABLED), ret);
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9601B)
        case RTL9601B_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_clrTotalPktCnt_set(ENABLED), ret);
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_clrTotalPktCnt_set(ENABLED), ret);
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_clrTotalPktCnt_set(ENABLED), ret);
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_clrTotalPktCnt_set(ENABLED), ret);
            }
            break;
#endif

        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }
#if defined(CONFIG_SDK_APOLLOMP) || defined(CONFIG_SDK_RTL9601B) || defined(CONFIG_SDK_RTL9602C) || defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
    diag_util_printf("Set Total Packet Count Clear.\n");
#endif
    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_set_total_pktcnt_clear */

/*
 * flowctrl set max-page-clear
 */
cparser_result_t
cparser_cmd_flowctrl_set_max_page_clear(
                                              cparser_context_t *context)
{
    diag_portlist_t  portlist;
    diag_mask_t  queuelist;
    rtk_port_t port;
	uint32 i;
    int32 ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();

    switch (DIAG_UTIL_CHIP_TYPE) {

#if defined(CONFIG_SDK_APOLLOMP)
		case APOLLOMP_CHIP_ID:
			{
				DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_clrMaxUsedPageCnt_set(ENABLED), ret);
				DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_clrTotalPktCnt_set(ENABLED), ret);
                queuelist.mask.bits[0] = 0xFF;
                HAL_SCAN_ALL_PORT(port)
                {
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_debugCtrl_set(port, ENABLED, queuelist.mask), ret);
                }


                DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_ponLatchMaxUsedPage_set(DISABLED), ret);
                for (i=0;i<1000;i++);
                DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_ponLatchMaxUsedPage_set(ENABLED), ret);
			}
			break;
#endif
#if defined(CONFIG_SDK_RTL9601B)
        case RTL9601B_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_clrMaxUsedPageCnt_set(ENABLED), ret);
                DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_clrTotalPktCnt_set(ENABLED), ret);
                queuelist.mask.bits[0] = 0xFF;
                HAL_SCAN_ALL_PORT(port)
                {
                    DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_debugCtrl_set(port, ENABLED, queuelist.mask), ret);
                }

				for(i=0;i<HAL_MAX_NUM_OF_PON_QUEUE();i++)
                {
                	DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_ponUsedPageCtrl_set(i, ENABLED), ret);
				}
			}
            break;
#endif
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_clrMaxUsedPageCnt_set(ENABLED), ret);
                DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_clrTotalPktCnt_set(ENABLED), ret);
                queuelist.mask.bits[0] = 0xFF;
                HAL_SCAN_ALL_PORT(port)
                {
                    DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_debugCtrl_set(port, ENABLED, queuelist.mask), ret);
                }

				for(i=0;i<HAL_MAX_NUM_OF_PON_QUEUE();i++)
                {
                	DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_ponUsedPageCtrl_set(i, ENABLED), ret);
				}

            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_clrMaxUsedPageCnt_set(ENABLED), ret);
                DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_clrTotalPktCnt_set(ENABLED), ret);
                queuelist.mask.bits[0] = 0xFF;
                HAL_SCAN_ALL_PORT(port)
                {
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_debugCtrl_set(port, ENABLED, queuelist.mask), ret);
                }

				for(i=0;i<HAL_MAX_NUM_OF_PON_QUEUE();i++)
                {
                	DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_ponUsedPageCtrl_set(i, ENABLED), ret);
				}
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_clrMaxUsedPageCnt_set(ENABLED), ret);
                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_clrTotalPktCnt_set(ENABLED), ret);

                queuelist.mask.bits[0] = 0xFF;
                HAL_SCAN_ALL_PORT(port)
                {
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_debugCtrl_set(port, ENABLED, queuelist.mask), ret);
                }

                for(i=0;i<HAL_MAX_NUM_OF_PON_QUEUE();i++)
                {
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_ponUsedPageCtrl_set(i, ENABLED), ret);
                }
            }
            break;
#endif

        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }


    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_set_max_page_clear*/


/*
 * flowctrl set max-page-clear switch
 */
cparser_result_t
cparser_cmd_flowctrl_set_max_page_clear_switch(
                                              cparser_context_t *context)
{
    diag_portlist_t  portlist;
    diag_mask_t  queuelist;
    rtk_port_t port;
    int32 ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();

    switch (DIAG_UTIL_CHIP_TYPE)
	{
#if defined(CONFIG_SDK_APOLLOMP)
		case APOLLOMP_CHIP_ID:
			{
				DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_clrMaxUsedPageCnt_set(ENABLED), ret);
				DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_clrTotalPktCnt_set(ENABLED), ret);
				queuelist.mask.bits[0] = 0xFF;
				HAL_SCAN_ALL_PORT(port)
				{
					DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_debugCtrl_set(port, DISABLED, queuelist.mask), ret);
				}
			}
			break;
#endif
#if defined(CONFIG_SDK_RTL9601B)
        case RTL9601B_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_clrMaxUsedPageCnt_set(ENABLED), ret);
                DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_clrTotalPktCnt_set(ENABLED), ret);
                queuelist.mask.bits[0] = 0xFF;
                HAL_SCAN_ALL_PORT(port)
                {
                    DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_debugCtrl_set(port, DISABLED, queuelist.mask), ret);
                }
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_clrMaxUsedPageCnt_set(ENABLED), ret);
                DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_clrTotalPktCnt_set(ENABLED), ret);
                queuelist.mask.bits[0] = 0xFF;
                HAL_SCAN_ALL_PORT(port)
                {
                    DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_debugCtrl_set(port, DISABLED, queuelist.mask), ret);
                }
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_clrMaxUsedPageCnt_set(ENABLED), ret);
                DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_clrTotalPktCnt_set(ENABLED), ret);
                queuelist.mask.bits[0] = 0xFF;
                HAL_SCAN_ALL_PORT(port)
                {
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_debugCtrl_set(port, DISABLED, queuelist.mask), ret);
                }
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_clrMaxUsedPageCnt_set(ENABLED), ret);
                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_clrTotalPktCnt_set(ENABLED), ret);
                queuelist.mask.bits[0] = 0xFF;
                HAL_SCAN_ALL_PORT(port)
                {
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_debugCtrl_set(port, ENABLED, queuelist.mask), ret);
                }
            }
            break;
#endif

        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }


    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_set_max_page_clear_switch */




/*
 * flowctrl set max-page-clear used-page
 */
cparser_result_t
cparser_cmd_flowctrl_set_max_page_clear_used_page(
                                                 cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();

    switch (DIAG_UTIL_CHIP_TYPE) {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_clrMaxUsedPageCnt_set(ENABLED), ret);
                diag_util_printf("Maximum Used Paged Count is cleared\n");
            }
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_clrMaxUsedPageCnt_set(ENABLED), ret);
                diag_util_printf("Maximum Used Paged Count is cleared\n");
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9601B)
        case RTL9601B_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_clrMaxUsedPageCnt_set(ENABLED), ret);
                diag_util_printf("Maximum Used Paged Count is cleared\n");
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_clrMaxUsedPageCnt_set(ENABLED), ret);
                diag_util_printf("Maximum Used Paged Count is cleared\n");
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_clrMaxUsedPageCnt_set(ENABLED), ret);
                diag_util_printf("Maximum Used Paged Count is cleared\n");
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_clrMaxUsedPageCnt_set(ENABLED), ret);
                diag_util_printf("Maximum Used Paged Count is cleared\n");
            }
            break;
#endif

        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_set_max_page_clear_used_page */

/*
 * flowctrl set max-page-clear egress-queue port ( <PORT_LIST:ports> | all ) queue-id ( <MASK_LIST:qid> | all )
 */
cparser_result_t
cparser_cmd_flowctrl_set_max_page_clear_egress_queue_port_ports_all_queue_id_qid_all(
                                                                                    cparser_context_t *context,
                                                                                    char * *ports_ptr,
                                                                                    char * *qid_ptr)
{
    diag_portlist_t  portlist;
    diag_mask_t  queuelist;
    rtk_port_t port;
    uint32 idx;
    uint8   buf[UTIL_PORT_MASK_BUFFER_LENGTH];
    uint8   buf2[UTIL_PORT_MASK_BUFFER_LENGTH];
    int32 ret = RT_ERR_FAILED;

    osal_memset(&queuelist, 0, sizeof(diag_mask_t));
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_QUEUEMASK(queuelist, 7) , ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        switch (DIAG_UTIL_CHIP_TYPE) {
#if defined(CONFIG_SDK_APOLLO)
            case APOLLO_CHIP_ID:
                {
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_debugCtrl_set(port, DISABLED, queuelist.mask), ret);
                }
                break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
            case APOLLOMP_CHIP_ID:
                {
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_debugCtrl_set(port, DISABLED, queuelist.mask), ret);
                }
                break;
#endif
#if defined(CONFIG_SDK_RTL9601B)
            case RTL9601B_CHIP_ID:
                {
                    DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_debugCtrl_set(port, DISABLED, queuelist.mask), ret);
                }
                break;
#endif

#if defined(CONFIG_SDK_RTL9602C)
            case RTL9602C_CHIP_ID:
                {
                    DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_debugCtrl_set(port, DISABLED, queuelist.mask), ret);
                }
                break;
#endif

#if defined(CONFIG_SDK_RTL9607C)
            case RTL9607C_CHIP_ID:
                {
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_debugCtrl_set(port, DISABLED, queuelist.mask), ret);
                }
                break;
#endif

#if defined(CONFIG_SDK_RTL9603CVD)
            case RTL9603CVD_CHIP_ID:
                {
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_debugCtrl_set(port, DISABLED, queuelist.mask), ret);
                }
                break;
#endif

            default:
                diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
                return CPARSER_NOT_OK;
                break;
        }
    }
    diag_util_lPortMask2str(buf, &portlist.portmask);
    diag_util_mask2str(buf2, queuelist.mask.bits[0]);
    diag_util_printf("Port:%s Egress queue: %s Max Page Clear.\n",buf, buf2);

    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_set_max_page_clear_egress_queue_port_ports_all_queue_id_qid_all */

/*
 * flowctrl get used-page-cnt ( ingress | egress ) port ( <PORT_LIST:ports> | all | swpbo )
 */
cparser_result_t
cparser_cmd_flowctrl_get_used_page_cnt_ingress_egress_port_ports_all_swpbo(
                                                                    cparser_context_t *context,
                                                                    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t port = 0;
    uint32 current, maximum;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    if (!osal_strcmp(TOKEN_STR(5),"swpbo")) {
        switch (DIAG_UTIL_CHIP_TYPE) {
#if defined(CONFIG_SDK_RTL9607C)
            case RTL9607C_CHIP_ID:
                {
                    if (!osal_strcmp(TOKEN_STR(3),"ingress")) {
                        diag_util_mprintf("ingress-port     current-page      maximum-page\n");
                        diag_util_mprintf("-----------------------------------------------------\n");
                        DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_swpboPortUsedUsedPage_get(&current, &maximum), ret);
                        diag_util_mprintf("    %-18u %-18u%u\n", HAL_GET_SWPBOLB_PORT(), current, maximum);
                    } else if (!osal_strcmp(TOKEN_STR(3),"egress")) {
                        diag_util_mprintf("egress-port     current-page      maximum-page\n");
                        diag_util_mprintf("-----------------------------------------------------\n");
                        DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_swpboEgressUsedPage_get(&current, &maximum), ret);
                        diag_util_mprintf("    %-18u %-18u%u\n", HAL_GET_SWPBOLB_PORT(), current, maximum);
                    }
                }
                break;
#endif

#if defined(CONFIG_SDK_RTL9603CVD)
            case RTL9603CVD_CHIP_ID:
                {
                    if (!osal_strcmp(TOKEN_STR(3),"ingress")) {
                        diag_util_mprintf("ingress-port     current-page      maximum-page\n");
                        diag_util_mprintf("-----------------------------------------------------\n");
                        DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_swpboPortUsedUsedPage_get(&current, &maximum), ret);
                        diag_util_mprintf("    %-18u %-18u%u\n", HAL_GET_SWPBOLB_PORT(), current, maximum);
                    } else if (!osal_strcmp(TOKEN_STR(3),"egress")) {
                        diag_util_mprintf("egress-port     current-page      maximum-page\n");
                        diag_util_mprintf("-----------------------------------------------------\n");
                        DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_swpboEgressUsedPage_get(&current, &maximum), ret);
                        diag_util_mprintf("    %-18u %-18u%u\n", HAL_GET_SWPBOLB_PORT(), current, maximum);
                    }
                }
                break;
#endif

            default:
                diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
                return CPARSER_NOT_OK;
                break;
         }
    } else {
        DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

        if (!osal_strcmp(TOKEN_STR(3),"ingress")) {
            diag_util_mprintf("ingress-port     current-page      maximum-page\n");
            diag_util_mprintf("-----------------------------------------------------\n");

            DIAG_UTIL_PORTMASK_SCAN(portlist, port)
            {
                switch (DIAG_UTIL_CHIP_TYPE) {
#if defined(CONFIG_SDK_APOLLO)
                    case APOLLO_CHIP_ID:
                        {
                            DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_portUsedPageCnt_get(port, &current, &maximum), ret);
                        }
                        break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
                    case APOLLOMP_CHIP_ID:
                        {
                            DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_portUsedPageCnt_get(port, &current, &maximum), ret);
                        }
                        break;
#endif
#if defined(CONFIG_SDK_RTL9601B)
                    case RTL9601B_CHIP_ID:
                        {
                            DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_portUsedPageCnt_get(port, &current, &maximum), ret);
                        }
                        break;
#endif

#if defined(CONFIG_SDK_RTL9602C)
                    case RTL9602C_CHIP_ID:
                        {
                            DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_portUsedPageCnt_get(port, &current, &maximum), ret);
                        }
                        break;
#endif

#if defined(CONFIG_SDK_RTL9607C)
                    case RTL9607C_CHIP_ID:
                        {
                            DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_portUsedPageCnt_get(port, &current, &maximum), ret);
                        }
                        break;
#endif

#if defined(CONFIG_SDK_RTL9603CVD)
                    case RTL9603CVD_CHIP_ID:
                        {
                            DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_portUsedPageCnt_get(port, &current, &maximum), ret);
                        }
                        break;
#endif

                    default:
                        diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
                        return CPARSER_NOT_OK;
                        break;
                }
#if defined(CONFIG_SDK_APOLLOMP) || defined(CONFIG_SDK_RTL9601B) || defined(CONFIG_SDK_RTL9602C) || defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
                
                diag_util_mprintf("    %-18u %-18u%u\n", port, current, maximum);
#endif
            }
        } else if (!osal_strcmp(TOKEN_STR(3),"egress")) {
            diag_util_mprintf("egress-port     current-page      maximum-page\n");
            diag_util_mprintf("-----------------------------------------------------\n");
            switch (DIAG_UTIL_CHIP_TYPE) {
#if defined(CONFIG_SDK_APOLLO)
                case APOLLO_CHIP_ID:
                    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
                    {
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_portEgressUsedPageCnt_get(port, &current, &maximum), ret);
                        diag_util_mprintf("    %-18u %-18u%u\n", port, current, maximum);
                    }
                    break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
                case APOLLOMP_CHIP_ID:
                    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
                    {
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_portEgressUsedPageCnt_get(port, &current, &maximum), ret);
                        diag_util_mprintf("    %-18u %-18u%u\n", port, current, maximum);
                    }
                    break;
#endif
#if defined(CONFIG_SDK_RTL9601B)
                case RTL9601B_CHIP_ID:
                    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
                    {
                        DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_portEgressUsedPageCnt_get(port, &current, &maximum), ret);
                        diag_util_mprintf("    %-18u %-18u%u\n", port, current, maximum);
                    }
                    break;
#endif

#if defined(CONFIG_SDK_RTL9602C)
                case RTL9602C_CHIP_ID:
                    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
                    {
                        DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_portEgressUsedPageCnt_get(port, &current, &maximum), ret);
                        diag_util_mprintf("    %-18u %-18u%u\n", port, current, maximum);
                    }
                    break;
#endif

#if defined(CONFIG_SDK_RTL9607C)
                case RTL9607C_CHIP_ID:
                    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
                    {
                        DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_portEgressUsedPageCnt_get(port, &current, &maximum), ret);
                        diag_util_mprintf("    %-18u %-18u%u\n", port, current, maximum);
                    }
                    break;
#endif

#if defined(CONFIG_SDK_RTL9603CVD)
                case RTL9603CVD_CHIP_ID:
                    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
                    {
                        DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_portEgressUsedPageCnt_get(port, &current, &maximum), ret);
                        diag_util_mprintf("    %-18u %-18u%u\n", port, current, maximum);
                    }
                    break;
#endif

                default:
                    diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
                    return CPARSER_NOT_OK;
                    break;
            }
        }
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_get_used_page_cnt_ingress_egress_port_ports_all_swpbo */

/*
 * flowctrl get used-page-cnt egress-queue port ( <PORT_LIST:ports> | all | swpbo ) queue-id ( <MASK_LIST:qid> | all )
 */
cparser_result_t
cparser_cmd_flowctrl_get_used_page_cnt_egress_queue_port_ports_all_swpbo_queue_id_qid_all(
                                                                                   cparser_context_t *context,
                                                                                   char * *ports_ptr,
                                                                                   char * *qid_ptr)
{
    diag_portlist_t portlist;
    diag_mask_t  queuelist;
    rtk_port_t port;
    rtk_qid_t queue;
    uint32 current, maximum[8];
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(maximum,0,sizeof(maximum));

    diag_util_mprintf("Get Ingress Flow Control Queue Egress Drop Queue Page Count \n");
    if (!osal_strcmp(TOKEN_STR(5),"swpbo")) {
        switch (DIAG_UTIL_CHIP_TYPE) {
#if defined(CONFIG_SDK_RTL9607C)
            case RTL9607C_CHIP_ID:
                {
                    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_QUEUEMASK(queuelist, 7), ret);
                    DIAG_UTIL_MASK_SCAN(queuelist, queue)
                    {
                        if (queue == 0)
                        {
                            diag_util_printf("                     ");
                            diag_util_printf("Q%d      ",queue);
                            diag_util_mprintf("\n");
                            diag_util_mprintf("------------------------------------------------------------------------------------\n");
                            diag_util_printf("Port%2u  Current   ", HAL_GET_SWPBOLB_PORT());
                            DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_swpboEgressQueueUsedPage_get(&current, &maximum[queue]), ret);
                            diag_util_printf("%5u   ", current);
                            diag_util_mprintf("\n");
                            diag_util_printf("        Maximum   ");
                            diag_util_printf("%5u   ", maximum[queue]);
                            diag_util_mprintf("\n");
                        }
                    }
                }
                break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
            case RTL9603CVD_CHIP_ID:
                {
                    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_QUEUEMASK(queuelist, 7), ret);
                    DIAG_UTIL_MASK_SCAN(queuelist, queue)
                    {
                        if (queue == 0)
                        {
                            diag_util_printf("                     ");
                            diag_util_printf("Q%d      ",queue);
                            diag_util_mprintf("\n");
                            diag_util_mprintf("------------------------------------------------------------------------------------\n");
                            diag_util_printf("Port%2u  Current   ", HAL_GET_SWPBOLB_PORT());
                            DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_swpboEgressQueueUsedPage_get(&current, &maximum[queue]), ret);
                            diag_util_printf("%5u   ", current);
                            diag_util_mprintf("\n");
                            diag_util_printf("        Maximum   ");
                            diag_util_printf("%5u   ", maximum[queue]);
                            diag_util_mprintf("\n");
                        }
                    }
                }
                break;
#endif

            default:
                diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
                return CPARSER_NOT_OK;
                break;
         }
    } else {

        DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);
        DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_QUEUEMASK(queuelist, 7), ret);

        diag_util_printf("                     ");
        DIAG_UTIL_MASK_SCAN(queuelist, queue)
        {
            diag_util_printf("Q%d      ",queue);
        }
        diag_util_mprintf("\n");
        diag_util_mprintf("------------------------------------------------------------------------------------\n");
        DIAG_UTIL_PORTMASK_SCAN(portlist, port)
        {
            diag_util_printf("Port%2u  Current   ", port);
            DIAG_UTIL_MASK_SCAN(queuelist, queue)
            {
                switch (DIAG_UTIL_CHIP_TYPE) {
#if defined(CONFIG_SDK_APOLLO)
                    case APOLLO_CHIP_ID:
                        {
                            DIAG_UTIL_ERR_CHK( apollo_raw_flowctrl_queueEgressUsedPageCnt_get(port, queue, &current, &maximum[queue]), ret);
                        }
                        break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
                    case APOLLOMP_CHIP_ID:
                        {
                            DIAG_UTIL_ERR_CHK( apollomp_raw_flowctrl_queueEgressUsedPageCnt_get(port, queue, &current, &maximum[queue]), ret);
                        }
                        break;
#endif
#if defined(CONFIG_SDK_RTL9601B)
                    case RTL9601B_CHIP_ID:
                        {
                            DIAG_UTIL_ERR_CHK( rtl9601b_raw_flowctrl_queueEgressUsedPageCnt_get(port, queue, &current, &maximum[queue]), ret);
                        }
                        break;
#endif

#if defined(CONFIG_SDK_RTL9602C)
                    case RTL9602C_CHIP_ID:
                        {
                            DIAG_UTIL_ERR_CHK( rtl9602c_raw_flowctrl_queueEgressUsedPageCnt_get(port, queue, &current, &maximum[queue]), ret);
                        }
                        break;
#endif

#if defined(CONFIG_SDK_RTL9607C)
                    case RTL9607C_CHIP_ID:
                        {
                            DIAG_UTIL_ERR_CHK( rtl9607c_raw_flowctrl_queueEgressUsedPageCnt_get(port, queue, &current, &maximum[queue]), ret);
                        }
                        break;
#endif

#if defined(CONFIG_SDK_RTL9603CVD)
                    case RTL9603CVD_CHIP_ID:
                        {
                            DIAG_UTIL_ERR_CHK( rtl9603cvd_raw_flowctrl_queueEgressUsedPageCnt_get(port, queue, &current, &maximum[queue]), ret);
                        }
                        break;
#endif

                    default:
                        diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
                        return CPARSER_NOT_OK;
                        break;
                }
#if defined(CONFIG_SDK_APOLLOMP) || defined(CONFIG_SDK_RTL9601B) || defined(CONFIG_SDK_RTL9602C) || defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
                diag_util_printf("%5u   ", current);
#endif
            }
            diag_util_mprintf("\n");
            diag_util_printf("        Maximum   ");
            DIAG_UTIL_MASK_SCAN(queuelist, queue)
            {
                diag_util_printf("%5u   ", maximum[queue]);
            }
            diag_util_mprintf("\n");
        }
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_get_used_page_cnt_egress_queue_port_ports_all_swpbo_queue_id_qid_all */

/*
 * flowctrl get total-page-cnt
 */
cparser_result_t
cparser_cmd_flowctrl_get_total_page_cnt(
                                       cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint32 count;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    switch (DIAG_UTIL_CHIP_TYPE) {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_totalPageCnt_get(&count), ret);
                diag_util_mprintf("Total page Count for Packet: Current  %u\n", count);
            }
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_totalPageCnt_get(&count), ret);
                diag_util_mprintf("Total page Count for Packet: Current  %u\n", count);
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9601B)
        case RTL9601B_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_totalPageCnt_get(&count), ret);
                diag_util_mprintf("Total page Count for Packet: Current  %u\n", count);
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_totalPageCnt_get(&count), ret);
                diag_util_mprintf("Total page Count for Packet: Current  %u\n", count);
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_totalPageCnt_get(&count), ret);
                diag_util_mprintf("Total page Count for Packet: Current  %u\n", count);
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_totalPageCnt_get(&count), ret);
                diag_util_mprintf("Total page Count for Packet: Current  %u\n", count);
            }
            break;
#endif

        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_get_total_page_cnt */

/*
 * flowctrl get used-page-cnt ( total | public | public-off | public-jumbo )
 */
cparser_result_t
cparser_cmd_flowctrl_get_used_page_cnt_total_public_public_off_public_jumbo(
                                                                           cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint32 current, maximum;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch (DIAG_UTIL_CHIP_TYPE) {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            {
                if (!osal_strcmp(TOKEN_STR(3),"total")) {
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_totalUsedPageCnt_get(&current, &maximum), ret);
                    diag_util_mprintf("Total page Count: Current  %u maximum  %u\n", current, maximum);
                } else if (!osal_strcmp(TOKEN_STR(3),"public")) {
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_publicUsedPageCnt_get(&current, &maximum), ret);
                    diag_util_mprintf("Public page Count: Current  %u maximum  %u\n", current, maximum);
                } else if (!osal_strcmp(TOKEN_STR(3),"public-off")) {
                    diag_util_mprintf("-----------------------------------------------------\n");
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_publicFcoffUsedPageCnt_get(&current, &maximum), ret);
                    diag_util_mprintf("Public page Count for FC-off: Current  %u maximum  %u\n", current, maximum);
                } else if (!osal_strcmp(TOKEN_STR(3),"public-jumbo")) {
                    diag_util_mprintf("-----------------------------------------------------\n");
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_publicJumboUsedPageCnt_get(&current, &maximum), ret);
                    diag_util_mprintf("Public page Count for jumbo frame: Current  %u maximum  %u\n", current, maximum);
                }
            }
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            {
                if (!osal_strcmp(TOKEN_STR(3),"total")) {
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_totalUsedPageCnt_get(&current, &maximum), ret);
                    diag_util_mprintf("Total page Count: Current  %u maximum  %u\n", current, maximum);
                } else if (!osal_strcmp(TOKEN_STR(3),"public")) {
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_publicUsedPageCnt_get(&current, &maximum), ret);
                    diag_util_mprintf("Public page Count: Current  %u maximum  %u\n", current, maximum);
                } else if (!osal_strcmp(TOKEN_STR(3),"public-off")) {
                    diag_util_mprintf("-----------------------------------------------------\n");
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_publicFcoffUsedPageCnt_get(&current, &maximum), ret);
                    diag_util_mprintf("Public page Count for FC-off: Current  %u maximum  %u\n", current, maximum);
                } else if (!osal_strcmp(TOKEN_STR(3),"public-jumbo")) {
                    diag_util_mprintf("-----------------------------------------------------\n");
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_publicJumboUsedPageCnt_get(&current, &maximum), ret);
                    diag_util_mprintf("Public page Count for jumbo frame: Current  %u maximum  %u\n", current, maximum);
                }
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9601B)
        case RTL9601B_CHIP_ID:
            {
                if (!osal_strcmp(TOKEN_STR(3),"total")) {
                    DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_totalUsedPageCnt_get(&current, &maximum), ret);
                    diag_util_mprintf("Total page Count: Current  %u maximum  %u\n", current, maximum);
                } else if (!osal_strcmp(TOKEN_STR(3),"public")) {
                    DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_publicUsedPageCnt_get(&current, &maximum), ret);
                    diag_util_mprintf("Public page Count: Current  %u maximum  %u\n", current, maximum);
                } else if (!osal_strcmp(TOKEN_STR(3),"public-off")) {
                    diag_util_mprintf("-----------------------------------------------------\n");
                    DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_publicFcoffUsedPageCnt_get(&current, &maximum), ret);
                    diag_util_mprintf("Public page Count for FC-off: Current  %u maximum  %u\n", current, maximum);
                } else
                    diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
            {
                if (!osal_strcmp(TOKEN_STR(3),"total")) {
                    DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_totalUsedPageCnt_get(&current, &maximum), ret);
                    diag_util_mprintf("Total page Count: Current  %u maximum  %u\n", current, maximum);
                } else if (!osal_strcmp(TOKEN_STR(3),"public")) {
                    DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_publicUsedPageCnt_get(&current, &maximum), ret);
                    diag_util_mprintf("Public page Count: Current  %u maximum  %u\n", current, maximum);
                } else if (!osal_strcmp(TOKEN_STR(3),"public-off")) {
                    diag_util_mprintf("-----------------------------------------------------\n");
                    DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_publicFcoffUsedPageCnt_get(&current, &maximum), ret);
                    diag_util_mprintf("Public page Count for FC-off: Current  %u maximum  %u\n", current, maximum);
                } else
                    diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
            {
                if (!osal_strcmp(TOKEN_STR(3),"total")) {
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_totalUsedPageCnt_get(&current, &maximum), ret);
                    diag_util_mprintf("Total page Count: Current  %u maximum  %u\n", current, maximum);
                } else if (!osal_strcmp(TOKEN_STR(3),"public")) {
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_publicUsedPageCnt_get(&current, &maximum), ret);
                    diag_util_mprintf("Public page Count: Current  %u maximum  %u\n", current, maximum);
                } else if (!osal_strcmp(TOKEN_STR(3),"public-off")) {
                    diag_util_mprintf("-----------------------------------------------------\n");
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_publicFcoffUsedPageCnt_get(&current, &maximum), ret);
                    diag_util_mprintf("Public page Count for FC-off: Current  %u maximum  %u\n", current, maximum);
                } else
                    diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
            {
                if (!osal_strcmp(TOKEN_STR(3),"total")) {
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_totalUsedPageCnt_get(&current, &maximum), ret);
                    diag_util_mprintf("Total page Count: Current  %u maximum  %u\n", current, maximum);
                } else if (!osal_strcmp(TOKEN_STR(3),"public")) {
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_publicUsedPageCnt_get(&current, &maximum), ret);
                    diag_util_mprintf("Public page Count: Current  %u maximum  %u\n", current, maximum);
                } else if (!osal_strcmp(TOKEN_STR(3),"public-off")) {
                    diag_util_mprintf("-----------------------------------------------------\n");
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_publicFcoffUsedPageCnt_get(&current, &maximum), ret);
                    diag_util_mprintf("Public page Count for FC-off: Current  %u maximum  %u\n", current, maximum);
                } else
                    diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            }
            break;
#endif

        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_get_used_page_cnt_total_public_public_off_public_jumbo */

/*
 * flowctrl get used-page-cnt packet ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_flowctrl_get_used_page_cnt_packet_ports_all(
                                                       cparser_context_t *context,
                                                       char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t port = 0;
    uint32 count;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    diag_util_mprintf("ingress-port     page    \n");
    diag_util_mprintf("-------------------------------\n");

    switch (DIAG_UTIL_CHIP_TYPE) {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            DIAG_UTIL_PORTMASK_SCAN(portlist, port)
            {
                DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_portPacketPageCnt_get(port, &count), ret);
                diag_util_mprintf("    %-18u %u\n", port, count);
            }
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_PORTMASK_SCAN(portlist, port)
            {
                DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_portPacketPageCnt_get(port, &count), ret);
                diag_util_mprintf("    %-18u %u\n", port, count);
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9601B)
        case RTL9601B_CHIP_ID:
            DIAG_UTIL_PORTMASK_SCAN(portlist, port)
            {
                DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_portPacketPageCnt_get(port, &count), ret);
                diag_util_mprintf("    %-18u %u\n", port, count);
            }
            break;
#endif

        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_get_used_page_cnt_packet_ports_all */

/*
 * flowctrl get used-page-cnt pon queue-id ( <MASK_LIST:qid> | all )
 */
cparser_result_t
cparser_cmd_flowctrl_get_used_page_cnt_pon_queue_id_qid_all(
                                                           cparser_context_t *context,
                                                           char * *qid_ptr)
{
    diag_mask_t  queuelist;
    rtk_qid_t queue;
    uint32 current, maximum;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_MASK128(queuelist, 5), ret);
    diag_util_mprintf("PON queue        current-page      maximum-page\n");
    diag_util_mprintf("-----------------------------------------------------\n");
    DIAG_UTIL_MASK_SCAN(queuelist, queue)
    {
        switch (DIAG_UTIL_CHIP_TYPE) {
#if defined(CONFIG_SDK_APOLLO)
            case APOLLO_CHIP_ID:
                {
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_ponUsedPage_get(queue, &current, &maximum), ret);
                }
                break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
            case APOLLOMP_CHIP_ID:
                {
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_ponUsedPage_get(queue, &current, &maximum), ret);
                }
                break;
#endif
#if defined(CONFIG_SDK_RTL9601B)
            case RTL9601B_CHIP_ID:
                {
                    DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_ponUsedPage_get(queue, &current, &maximum), ret);
                }
                break;
#endif
#if defined(CONFIG_SDK_RTL9602C)
            case RTL9602C_CHIP_ID:
                {
                    DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_ponUsedPage_get(queue, &current, &maximum), ret);
                }
                break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
            case RTL9607C_CHIP_ID:
                {
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_ponUsedPage_get(queue, &current, &maximum), ret);
                }
                break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
            case RTL9603CVD_CHIP_ID:
                {
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_ponUsedPage_get(queue, &current, &maximum), ret);
                }
                break;
#endif

            default:
                diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
                return CPARSER_NOT_OK;
                break;
        }
#if defined(CONFIG_SDK_APOLLOMP) || defined(CONFIG_SDK_RTL9601B) || defined(CONFIG_SDK_RTL9602C) || defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
        diag_util_mprintf("%-18u %-18u%u\n", queue, current, maximum);
#endif
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_get_used_page_cnt_pon_queue_id_qid_all */

/*
 * flowctrl set max-page-clear pon queue-id <UINT:qid>
 */
cparser_result_t
cparser_cmd_flowctrl_set_max_page_clear_pon_queue_id_qid(
                                                        cparser_context_t *context,
                                                        uint32_t  *qid_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint32 i;

    DIAG_UTIL_PARAM_CHK();
    switch (DIAG_UTIL_CHIP_TYPE) {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_ponUsedPageCtrl_set(*qid_ptr, ENABLED), ret);
            }
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            {
#if 0 //This function has something wrong
                DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_ponUsedPageCtrl_set(ENABLED), ret);
#endif

                DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_ponLatchMaxUsedPage_set(DISABLED), ret);
                for (i=0;i<1000;i++);
                DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_ponLatchMaxUsedPage_set(ENABLED), ret);

            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9601B)
        case RTL9601B_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_ponUsedPageCtrl_set(*qid_ptr, ENABLED), ret);
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_ponUsedPageCtrl_set(*qid_ptr, ENABLED), ret);
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_ponUsedPageCtrl_set(*qid_ptr, ENABLED), ret);
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_ponUsedPageCtrl_set(*qid_ptr, ENABLED), ret);
            }
            break;
#endif

        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }
#if defined(CONFIG_SDK_APOLLOMP) || defined(CONFIG_SDK_RTL9601B) || defined(CONFIG_SDK_RTL9602C) || defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
    diag_util_printf("Maximum PON queue %u is cleared\n",*qid_ptr);
#endif
    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_set_max_page_clear_pon_queue_id_qid */

/*
 * flowctrl set prefetch threshold <UINT:threshold>
 */
cparser_result_t
cparser_cmd_flowctrl_set_prefetch_threshold_threshold(
                                                     cparser_context_t *context,
                                                     uint32_t  *threshold_ptr)
{
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    switch (DIAG_UTIL_CHIP_TYPE) {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_txPrefet_set(*threshold_ptr), ret);
            }
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_txPrefet_set(*threshold_ptr), ret);
            }
            break;
#endif
#if defined(CONFIG_SDK_RTl9601B)
        case RTL9601B_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_txPrefet_set(*threshold_ptr), ret);
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_txPrefet_set(*threshold_ptr), ret);
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_txPrefet_set(*threshold_ptr), ret);
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_txPrefet_set(*threshold_ptr), ret);
            }
            break;
#endif

        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_set_prefetch_threshold_threshold */

/*
 * flowctrl get prefetch threshold
 */
cparser_result_t
cparser_cmd_flowctrl_get_prefetch_threshold(
                                           cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint32 threshold;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_PARAM_CHK();

    switch (DIAG_UTIL_CHIP_TYPE) 
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_txPrefet_get(&threshold), ret);
                diag_util_mprintf("Prefet Threshold is %u\n", threshold);
            }
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_txPrefet_get(&threshold), ret);
                diag_util_mprintf("Prefet Threshold is %u\n", threshold);
            }
            break;
#endif
#if defined(CONFIG_SDK_RTl9601B)
        case RTL9601B_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9601b_raw_flowctrl_txPrefet_get(&threshold), ret);
                diag_util_mprintf("Prefet Threshold is %u\n", threshold);
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_txPrefet_get(&threshold), ret);
                diag_util_mprintf("Prefet Threshold is %u\n", threshold);
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_txPrefet_get(&threshold), ret);
                diag_util_mprintf("Prefet Threshold is %u\n", threshold);
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_txPrefet_get(&threshold), ret);
                diag_util_mprintf("Prefet Threshold is %u\n", threshold);
            }
            break;
#endif

        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_get_prefetch_threshold */

#if (!defined(CONFIG_REDUCED_DIAG))

/*
 * flowctrl set jumbo state ( enable | disable )
 */
cparser_result_t
cparser_cmd_flowctrl_set_jumbo_state_enable_disable(
                                                   cparser_context_t *context)
{
    rtk_enable_t enable;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    if ('e'==TOKEN_CHAR(4,0))
        enable = ENABLED;
    else if ('d'==TOKEN_CHAR(4,0))
        enable = DISABLED;

    switch (DIAG_UTIL_CHIP_TYPE) {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_jumboMode_set(enable), ret);
            }
            diag_util_printf("Set Flow Control Jumbo Mode to %s\n",diagStr_enable[enable]);
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_jumboMode_set(enable), ret);
            }
            diag_util_printf("Set Flow Control Jumbo Mode to %s\n",diagStr_enable[enable]);
            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_set_jumbo_state_enable_disable */

/*
 * flowctrl get jumbo state
 */
cparser_result_t
cparser_cmd_flowctrl_get_jumbo_state(
                                    cparser_context_t *context)
{
    rtk_enable_t enable;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch (DIAG_UTIL_CHIP_TYPE) {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_jumboMode_get(&enable), ret);
                diag_util_mprintf("Flow Control Jumbo Mode is %s\n",diagStr_enable[enable]);
            }
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_jumboMode_get(&enable), ret);
                diag_util_mprintf("Flow Control Jumbo Mode is %s\n",diagStr_enable[enable]);
            }
            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }


    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_get_jumbo_state */

/*
 * flowctrl set jumbo size ( 3k | 4k | 6k | max )
 */
cparser_result_t
cparser_cmd_flowctrl_set_jumbo_size_3k_4k_6k_max(
                                                cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    switch (DIAG_UTIL_CHIP_TYPE) {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            {
                apollo_raw_flowctrl_jumbo_size_t size;
                if ('3'==TOKEN_CHAR(4,0))
                    size = RAW_FLOWCTRL_JUMBO_3K;
                else if ('4'==TOKEN_CHAR(4,0))
                    size = RAW_FLOWCTRL_JUMBO_4K;
                else if ('6'==TOKEN_CHAR(4,0))
                    size = RAW_FLOWCTRL_JUMBO_6K;
                else if ('m'==TOKEN_CHAR(4,0))
                    size = RAW_FLOWCTRL_JUMBO_MAX;
                DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_jumboSize_set(size), ret);
                diag_util_printf("Set Flow Control Jumbo size to %s\n",diagStr_flowCtrlJumboSize[size]);
            }
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            {
                apollomp_raw_flowctrl_jumbo_size_t size;
                if ('3'==TOKEN_CHAR(4,0))
                    size = APOLLOMP_FLOWCTRL_JUMBO_3K;
                else if ('4'==TOKEN_CHAR(4,0))
                    size = APOLLOMP_FLOWCTRL_JUMBO_4K;
                else if ('6'==TOKEN_CHAR(4,0))
                    size = APOLLOMP_FLOWCTRL_JUMBO_6K;
                else if ('m'==TOKEN_CHAR(4,0))
                    size = APOLLOMP_FLOWCTRL_JUMBO_MAX;
                DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_jumboSize_set(size), ret);
                diag_util_printf("Set Flow Control Jumbo size to %s\n",diagStr_flowCtrlJumboSize[size]);
            }
            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_set_jumbo_size_3k_4k_6k_max */

/*
 * flowctrl get jumbo size
 */
cparser_result_t
cparser_cmd_flowctrl_get_jumbo_size(
                                   cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch (DIAG_UTIL_CHIP_TYPE) {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            {
                apollo_raw_flowctrl_jumbo_size_t size;
                DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_jumboSize_get(&size), ret);
                diag_util_mprintf("Flow Control Jumbo size is %s\n",diagStr_flowCtrlJumboSize[size]);
            }
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            {
                apollomp_raw_flowctrl_jumbo_size_t size;
                DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_jumboSize_get(&size), ret);
                diag_util_mprintf("Flow Control Jumbo size is %s\n",diagStr_flowCtrlJumboSize[size]);
            }
            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_get_jumbo_size */


/*
 * flowctrl set ingress jumbo-global ( high-off | high-on | low-off | low-on ) threshold <UINT:threshold>
 */
cparser_result_t
cparser_cmd_flowctrl_set_ingress_jumbo_global_high_off_high_on_low_off_low_on_threshold_threshold(
                                                                                                 cparser_context_t *context,
                                                                                                 uint32_t  *threshold_ptr)
{
    uint32 onThresh, offThresh;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    switch (DIAG_UTIL_CHIP_TYPE) {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            {
                if (!osal_strcmp(TOKEN_STR(4),"high-off")) {
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_jumboGlobalHighThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_jumboGlobalHighThreshold_set(onThresh, *threshold_ptr), ret);
                    diag_util_printf("Set Flow Control Jumbo Global High Off Threshold to %u\n",*threshold_ptr);
                } else if (!osal_strcmp(TOKEN_STR(4),"high-on")) {
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_jumboGlobalHighThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_jumboGlobalHighThreshold_set(*threshold_ptr, offThresh), ret);
                    diag_util_printf("Set Flow Control Jumbo Global High On Threshold to %u\n",*threshold_ptr);
                } else if (!osal_strcmp(TOKEN_STR(4),"low-off")) {
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_jumboGlobalLowThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_jumboGlobalLowThreshold_set(onThresh, *threshold_ptr), ret);
                    diag_util_printf("Set Flow Control Jumbo Global Low Off Threshold to %u\n",*threshold_ptr);
                } else if (!osal_strcmp(TOKEN_STR(4),"low-on")) {
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_jumboGlobalLowThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_jumboGlobalLowThreshold_set(*threshold_ptr, offThresh), ret);
                    diag_util_printf("Set Flow Control Jumbo Global Low On Threshold to %u\n",*threshold_ptr);
                }
            }
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            {
                if (!osal_strcmp(TOKEN_STR(4),"high-off")) {
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_jumboGlobalHighThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_jumboGlobalHighThreshold_set(onThresh, *threshold_ptr), ret);
                    diag_util_printf("Set Flow Control Jumbo Global High Off Threshold to %u\n",*threshold_ptr);
                } else if (!osal_strcmp(TOKEN_STR(4),"high-on")) {
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_jumboGlobalHighThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_jumboGlobalHighThreshold_set(*threshold_ptr, offThresh), ret);
                    diag_util_printf("Set Flow Control Jumbo Global High On Threshold to %u\n",*threshold_ptr);
                } else if (!osal_strcmp(TOKEN_STR(4),"low-off")) {
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_jumboGlobalLowThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_jumboGlobalLowThreshold_set(onThresh, *threshold_ptr), ret);
                    diag_util_printf("Set Flow Control Jumbo Global Low Off Threshold to %u\n",*threshold_ptr);
                } else if (!osal_strcmp(TOKEN_STR(4),"low-on")) {
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_jumboGlobalLowThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_jumboGlobalLowThreshold_set(*threshold_ptr, offThresh), ret);
                    diag_util_printf("Set Flow Control Jumbo Global Low On Threshold to %u\n",*threshold_ptr);
                }

            }
            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_set_ingress_jumbo_global_high_off_high_on_low_off_low_on_threshold_threshold */

/*
 * flowctrl get ingress jumbo-global ( high-off | high-on | low-off | low-on ) threshold
 */
cparser_result_t
cparser_cmd_flowctrl_get_ingress_jumbo_global_high_off_high_on_low_off_low_on_threshold(
                                                                                       cparser_context_t *context)
{
    uint32 onThresh, offThresh;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();


    switch (DIAG_UTIL_CHIP_TYPE) {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            {
                if (!osal_strcmp(TOKEN_STR(4),"high-off")) {
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_jumboGlobalHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("Flow Control Jumbo Global High Off Threshold is %u\n",offThresh);
                } else if (!osal_strcmp(TOKEN_STR(4),"high-on")) {
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_jumboGlobalHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("Flow Control Jumbo Global High On Threshold is %u\n",onThresh);
                } else if (!osal_strcmp(TOKEN_STR(4),"low-off")) {
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_jumboGlobalLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("Flow Control Jumbo Global Low Off Threshold is %u\n",offThresh);
                } else if (!osal_strcmp(TOKEN_STR(4),"low-on")) {
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_jumboGlobalLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("Flow Control Jumbo Global Low On Threshold is %u\n",onThresh);
                }
            }
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            {
                if (!osal_strcmp(TOKEN_STR(4),"high-off")) {
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_jumboGlobalHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("Flow Control Jumbo Global High Off Threshold is %u\n",offThresh);
                } else if (!osal_strcmp(TOKEN_STR(4),"high-on")) {
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_jumboGlobalHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("Flow Control Jumbo Global High On Threshold is %u\n",onThresh);
                } else if (!osal_strcmp(TOKEN_STR(4),"low-off")) {
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_jumboGlobalLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("Flow Control Jumbo Global Low Off Threshold is %u\n",offThresh);
                } else if (!osal_strcmp(TOKEN_STR(4),"low-on")) {
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_jumboGlobalLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("Flow Control Jumbo Global Low On Threshold is %u\n",onThresh);
                }

            }
            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_get_ingress_jumbo_global_high_off_high_on_low_off_low_on_threshold */

/*
 * flowctrl set ingress jumbo-port ( high-off | high-on | low-off | low-on ) threshold <UINT:threshold>
 */
cparser_result_t
cparser_cmd_flowctrl_set_ingress_jumbo_port_high_off_high_on_low_off_low_on_threshold_threshold(
                                                                                               cparser_context_t *context,
                                                                                               uint32_t  *threshold_ptr)
{
    uint32 onThresh, offThresh;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    switch (DIAG_UTIL_CHIP_TYPE) {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            {
                if (!osal_strcmp(TOKEN_STR(4),"high-off")) {
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_jumboPortHighThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_jumboPortHighThreshold_set(onThresh, *threshold_ptr), ret);
                    diag_util_printf("Set Flow Control Jumbo Port High Off Threshold to %u\n",*threshold_ptr);
                } else if (!osal_strcmp(TOKEN_STR(4),"high-on")) {
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_jumboPortHighThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_jumboPortHighThreshold_set(*threshold_ptr, offThresh), ret);
                    diag_util_printf("Set Flow Control Jumbo Port High On Threshold to %u\n",*threshold_ptr);
                } else if (!osal_strcmp(TOKEN_STR(4),"low-off")) {
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_jumboPortLowThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_jumboPortLowThreshold_set(onThresh, *threshold_ptr), ret);
                    diag_util_printf("Set Flow Control Jumbo Port Low Off Threshold to %u\n",*threshold_ptr);
                } else if (!osal_strcmp(TOKEN_STR(4),"low-on")) {
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_jumboPortLowThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_jumboPortLowThreshold_set(*threshold_ptr, offThresh), ret);
                    diag_util_printf("Set Flow Control Jumbo Port Low On Threshold to %u\n",*threshold_ptr);
                }
            }
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            {
                if (!osal_strcmp(TOKEN_STR(4),"high-off")) {
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_jumboPortHighThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_jumboPortHighThreshold_set(onThresh, *threshold_ptr), ret);
                    diag_util_printf("Set Flow Control Jumbo Port High Off Threshold to %u\n",*threshold_ptr);
                } else if (!osal_strcmp(TOKEN_STR(4),"high-on")) {
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_jumboPortHighThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_jumboPortHighThreshold_set(*threshold_ptr, offThresh), ret);
                    diag_util_printf("Set Flow Control Jumbo Port High On Threshold to %u\n",*threshold_ptr);
                } else if (!osal_strcmp(TOKEN_STR(4),"low-off")) {
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_jumboPortLowThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_jumboPortLowThreshold_set(onThresh, *threshold_ptr), ret);
                    diag_util_printf("Set Flow Control Jumbo Port Low Off Threshold to %u\n",*threshold_ptr);
                } else if (!osal_strcmp(TOKEN_STR(4),"low-on")) {
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_jumboPortLowThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_jumboPortLowThreshold_set(*threshold_ptr, offThresh), ret);
                    diag_util_printf("Set Flow Control Jumbo Port Low On Threshold to %u\n",*threshold_ptr);
                }
            }
            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_set_ingress_jumbo_port_high_off_high_on_low_off_low_on_threshold_threshold */

/*
 * flowctrl get ingress jumbo-port ( high-off | high-on | low-off | low-on ) threshold
 */
cparser_result_t
cparser_cmd_flowctrl_get_ingress_jumbo_port_high_off_high_on_low_off_low_on_threshold(
                                                                                     cparser_context_t *context)
{
    uint32 onThresh, offThresh;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch (DIAG_UTIL_CHIP_TYPE) {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            {
                if (!osal_strcmp(TOKEN_STR(4),"high-off")) {
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_jumboPortHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("Flow Control Jumbo Global High Off Threshold is %u\n",offThresh);
                } else if (!osal_strcmp(TOKEN_STR(4),"high-on")) {
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_jumboPortHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("Flow Control Jumbo Global High On Threshold is %u\n",onThresh);
                } else if (!osal_strcmp(TOKEN_STR(4),"low-off")) {
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_jumboPortLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("Flow Control Jumbo Global Low Off Threshold is %u\n",offThresh);
                } else if (!osal_strcmp(TOKEN_STR(4),"low-on")) {
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_jumboPortLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("Flow Control Jumbo Global Low On Threshold is %u\n",onThresh);
                }
            }
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            {
                if (!osal_strcmp(TOKEN_STR(4),"high-off")) {
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_jumboPortHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("Flow Control Jumbo Global High Off Threshold is %u\n",offThresh);
                } else if (!osal_strcmp(TOKEN_STR(4),"high-on")) {
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_jumboPortHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("Flow Control Jumbo Global High On Threshold is %u\n",onThresh);
                } else if (!osal_strcmp(TOKEN_STR(4),"low-off")) {
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_jumboPortLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("Flow Control Jumbo Global Low Off Threshold is %u\n",offThresh);
                } else if (!osal_strcmp(TOKEN_STR(4),"low-on")) {
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_jumboPortLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("Flow Control Jumbo Global Low On Threshold is %u\n",onThresh);
                }

            }
            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_get_ingress_jumbo_port_high_off_high_on_low_off_low_on_threshold */

/*
 * flowctrl set low-queue threshold <UINT:threshold>
 */
cparser_result_t
cparser_cmd_flowctrl_set_low_queue_threshold_threshold(
                                                      cparser_context_t *context,
                                                      uint32_t  *threshold_ptr)
{
    int32 ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();


    switch (DIAG_UTIL_CHIP_TYPE) {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_lowQueueThreshold_set(*threshold_ptr), ret);
                diag_util_mprintf("Set Low Queue Threshold to %u\n", *threshold_ptr);
            }
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_lowQueueThreshold_set(*threshold_ptr), ret);
                diag_util_mprintf("Set Low Queue Threshold to %u\n", *threshold_ptr);
            }
            break;
#endif

        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }


    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_set_low_queue_threshold_threshold */

/*
 * flowctrl get low-queue threshold
 */
cparser_result_t
cparser_cmd_flowctrl_get_low_queue_threshold(
                                            cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint32 threshold;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch (DIAG_UTIL_CHIP_TYPE) {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_lowQueueThreshold_get(&threshold), ret);
                diag_util_mprintf("Low Queue Threshold is %u\n", threshold);
            }
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_lowQueueThreshold_get(&threshold), ret);
                diag_util_mprintf("Low Queue Threshold is %u\n", threshold);
            }
            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_get_low_queue_threshold */

/*
 * flowctrl set egress system ( flowctrl-threshold | drop-threshold ) ( high-off | high-on | low-off | low-on ) threshold <UINT:threshold>
 */
cparser_result_t
cparser_cmd_flowctrl_set_egress_system_flowctrl_threshold_drop_threshold_high_off_high_on_low_off_low_on_threshold_threshold(
                                                                                                                            cparser_context_t *context,
                                                                                                                            uint32_t  *threshold_ptr)
{
    uint32 onThresh, offThresh;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    switch (DIAG_UTIL_CHIP_TYPE) {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            {
                if ('d'==TOKEN_CHAR(4,0)) {
                    if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_globalFcoffHighThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_globalFcoffHighThreshold_set(onThresh, *threshold_ptr), ret);
                        diag_util_printf("Set Flow Control Global FC-Off High Off Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_globalFcoffHighThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_globalFcoffHighThreshold_set(*threshold_ptr, offThresh), ret);
                        diag_util_printf("Set Flow Control Global FC-Off High On Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_globalFcoffLowThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_globalFcoffLowThreshold_set(onThresh, *threshold_ptr), ret);
                        diag_util_printf("Set Flow Control Global FC-Off Low Off Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_globalFcoffLowThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_globalFcoffLowThreshold_set(*threshold_ptr, offThresh), ret);
                        diag_util_printf("Set Flow Control Global FC-Off Low On Threshold to %u\n",*threshold_ptr);
                    }
                } else if ('f'==TOKEN_CHAR(4,0)) {
                    if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_globalFconHighThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_globalFconHighThreshold_set(onThresh, *threshold_ptr), ret);
                        diag_util_printf("Set Flow Control Global FC-On High Off Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_globalFconHighThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_globalFconHighThreshold_set(*threshold_ptr, offThresh), ret);
                        diag_util_printf("Set Flow Control Global FC-On High On Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_globalFconLowThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_globalFconLowThreshold_set(onThresh, *threshold_ptr), ret);
                        diag_util_printf("Set Flow Control Global FC-On Low Off Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_globalFconLowThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_globalFconLowThreshold_set(*threshold_ptr, offThresh), ret);
                        diag_util_printf("Set Flow Control Global FC-On Low On Threshold to %u\n",*threshold_ptr);
                    }
                }
            }
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            {
                if ('d'==TOKEN_CHAR(4,0)) {
                    if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_globalFcoffHighThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_globalFcoffHighThreshold_set(onThresh, *threshold_ptr), ret);
                        diag_util_printf("Set Flow Control Global FC-Off High Off Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_globalFcoffHighThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_globalFcoffHighThreshold_set(*threshold_ptr, offThresh), ret);
                        diag_util_printf("Set Flow Control Global FC-Off High On Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_globalFcoffLowThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_globalFcoffLowThreshold_set(onThresh, *threshold_ptr), ret);
                        diag_util_printf("Set Flow Control Global FC-Off Low Off Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_globalFcoffLowThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_globalFcoffLowThreshold_set(*threshold_ptr, offThresh), ret);
                        diag_util_printf("Set Flow Control Global FC-Off Low On Threshold to %u\n",*threshold_ptr);
                    }
                } else if ('f'==TOKEN_CHAR(4,0)) {
                    if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_globalFconHighThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_globalFconHighThreshold_set(onThresh, *threshold_ptr), ret);
                        diag_util_printf("Set Flow Control Global FC-On High Off Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_globalFconHighThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_globalFconHighThreshold_set(*threshold_ptr, offThresh), ret);
                        diag_util_printf("Set Flow Control Global FC-On High On Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_globalFconLowThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_globalFconLowThreshold_set(onThresh, *threshold_ptr), ret);
                        diag_util_printf("Set Flow Control Global FC-On Low Off Threshold to %u\n",*threshold_ptr);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_globalFconLowThreshold_get(&onThresh, &offThresh), ret);
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_globalFconLowThreshold_set(*threshold_ptr, offThresh), ret);
                        diag_util_printf("Set Flow Control Global FC-On Low On Threshold to %u\n",*threshold_ptr);
                    }
                }

            }
            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_set_egress_system_flowctrl_threshold_drop_threshold_high_off_high_on_low_off_low_on_threshold_threshold */

/*
 * flowctrl get egress system ( flowctrl-threshold | drop-threshold ) ( high-off | high-on | low-off | low-on ) threshold
 */
cparser_result_t
cparser_cmd_flowctrl_get_egress_system_flowctrl_threshold_drop_threshold_high_off_high_on_low_off_low_on_threshold(
                                                                                                                  cparser_context_t *context)
{
    uint32 onThresh, offThresh;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch (DIAG_UTIL_CHIP_TYPE) {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            {
                if ('d'==TOKEN_CHAR(4,0)) {
                    if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_globalFcoffHighThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_mprintf("Flow Control Global FC-Off High Off Threshold is %u\n",offThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_globalFcoffHighThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_mprintf("Flow Control Global FC-Off High On Threshold is %u\n",onThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_globalFcoffLowThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_mprintf("Flow Control Global FC-Off Low Off Threshold is %u\n",offThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_globalFcoffLowThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_mprintf("Flow Control Global FC-Off Low On Threshold is %u\n",onThresh);
                    }
                } else if ('f'==TOKEN_CHAR(4,0)) {
                    if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_globalFconHighThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_mprintf("Flow Control Global FC-On High Off Threshold is %u\n",offThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_globalFconHighThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_mprintf("Flow Control Global FC-On High On Threshold is %u\n",onThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_globalFconLowThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_mprintf("Flow Control Global FC-On Low Off Threshold is %u\n",offThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                        DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_globalFconLowThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_mprintf("Flow Control Global FC-On Low On Threshold is %u\n",onThresh);
                    }
                }
            }
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            {
                if ('d'==TOKEN_CHAR(4,0)) {
                    if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_globalFcoffHighThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_mprintf("Flow Control Global FC-Off High Off Threshold is %u\n",offThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_globalFcoffHighThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_mprintf("Flow Control Global FC-Off High On Threshold is %u\n",onThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_globalFcoffLowThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_mprintf("Flow Control Global FC-Off Low Off Threshold is %u\n",offThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_globalFcoffLowThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_mprintf("Flow Control Global FC-Off Low On Threshold is %u\n",onThresh);
                    }
                } else if ('f'==TOKEN_CHAR(4,0)) {
                    if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_globalFconHighThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_mprintf("Flow Control Global FC-On High Off Threshold is %u\n",offThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_globalFconHighThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_mprintf("Flow Control Global FC-On High On Threshold is %u\n",onThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_globalFconLowThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_mprintf("Flow Control Global FC-On Low Off Threshold is %u\n",offThresh);
                    } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                        DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_globalFconLowThreshold_get(&onThresh, &offThresh), ret);
                        diag_util_mprintf("Flow Control Global FC-On Low On Threshold is %u\n",onThresh);
                    }
                }

            }
            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_get_egress_system_flowctrl_threshold_drop_threshold_high_off_high_on_low_off_low_on_threshold */

/*
 * flowctrl set egress queue-id ( <MASK_LIST:qid> | all ) threshold <UINT:threshold>
 */
cparser_result_t
cparser_cmd_flowctrl_set_egress_queue_id_qid_all_threshold_threshold(
                                                                    cparser_context_t *context,
                                                                    char * *qid_ptr,
                                                                    uint32_t  *threshold_ptr)
{
    diag_mask_t  queuelist;
    rtk_qid_t queue;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_QUEUEMASK(queuelist, 4), ret);

    diag_util_printf("Set Egress Flow Control Queue  Threshold \n");
    diag_util_printf("Egress-Queue       Drop-Threshold \n");
    diag_util_printf("---------------------------------\n");
    switch (DIAG_UTIL_CHIP_TYPE) {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            DIAG_UTIL_MASK_SCAN(queuelist, queue)
            {
                DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_queueEegressDropThreshold_set(queue, *threshold_ptr), ret);
                diag_util_printf("%-18u  %u\n", queue, *threshold_ptr);
            }
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_MASK_SCAN(queuelist, queue)
            {
                DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_queueEegressDropThreshold_set(queue, *threshold_ptr), ret);
                diag_util_printf("%-18u  %u\n", queue, *threshold_ptr);
            }
            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }


    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_set_egress_queue_id_qid_all_threshold_threshold */

/*
 * flowctrl get egress queue-id ( <MASK_LIST:qid> | all ) threshold
 */
cparser_result_t
cparser_cmd_flowctrl_get_egress_queue_id_qid_all_threshold(
                                                          cparser_context_t *context,
                                                          char * *qid_ptr)
{
    diag_mask_t  queuelist;
    rtk_qid_t queue;
    uint32 threshold;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_QUEUEMASK(queuelist, 4), ret);

    diag_util_mprintf("Get Egress Flow Control Queue Threshold \n");
    diag_util_mprintf("Egress-Queue       Drop-Threshold \n");
    diag_util_mprintf("---------------------------------\n");
    
    switch (DIAG_UTIL_CHIP_TYPE) {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            DIAG_UTIL_MASK_SCAN(queuelist, queue)
            {
                DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_queueEegressDropThreshold_get(queue, &threshold), ret);
                diag_util_mprintf("%-18u  %u\n", queue, threshold);
            }
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_MASK_SCAN(queuelist, queue)
            {
                DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_queueEegressDropThreshold_get(queue, &threshold), ret);
                diag_util_mprintf("%-18u  %u\n", queue, threshold);
            }
            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }


    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_get_egress_queue_id_qid_all_threshold */

/*
 * flowctrl set egress port ( <PORT_LIST:ports> | all ) threshold <UINT:threshold>
 */
cparser_result_t
cparser_cmd_flowctrl_set_egress_port_ports_all_threshold_threshold(
                                                                  cparser_context_t *context,
                                                                  char * *ports_ptr,
                                                                  uint32_t  *threshold_ptr)
{
    diag_portlist_t portlist;
    rtk_port_t port;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    diag_util_printf("Set Egress  Flow Control Port Threshold \n");
    diag_util_printf("Egress-Port       Drop-Threshold \n");
    diag_util_printf("-----------------------------\n");
    
    switch (DIAG_UTIL_CHIP_TYPE) {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            DIAG_UTIL_PORTMASK_SCAN(portlist, port)
            {
                DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_portEegressDropThreshold_set(port, *threshold_ptr), ret);
                diag_util_printf("%-18u  %u\n", port, *threshold_ptr);
            }
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_PORTMASK_SCAN(portlist, port)
            {
                DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_portEegressDropThreshold_set(port, *threshold_ptr), ret);
                diag_util_printf("%-18u  %u\n", port, *threshold_ptr);
            }
            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_set_egress_port_ports_all_threshold_threshold */

/*
 * flowctrl get egress port ( <PORT_LIST:ports> | all ) threshold
 */
cparser_result_t
cparser_cmd_flowctrl_get_egress_port_ports_all_threshold(
                                                        cparser_context_t *context,
                                                        char * *ports_ptr)
{
    diag_portlist_t portlist;
    rtk_port_t port;
    uint32 threshold;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    diag_util_mprintf("Get Egress Flow Control Port Threshold \n");
    diag_util_mprintf("Egress-Port       Drop-Threshold \n");
    diag_util_mprintf("-----------------------------\n");
    
    switch (DIAG_UTIL_CHIP_TYPE) {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            DIAG_UTIL_PORTMASK_SCAN(portlist, port)
            {
                DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_portEegressDropThreshold_get(port, &threshold), ret);
                diag_util_mprintf("%-18u  %u\n", port, threshold);
            }
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_PORTMASK_SCAN(portlist, port)
            {
                DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_portEegressDropThreshold_get(port, &threshold), ret);
                diag_util_mprintf("%-18u  %u\n", port, threshold);
            }
            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }


    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_get_egress_port_ports_all_threshold */

/*
 * flowctrl set egress port ( <PORT_LIST:ports> | all ) queue-id ( <MASK_LIST:qid> | all ) queue-drop state ( enable | disable )
 */
cparser_result_t
cparser_cmd_flowctrl_set_egress_port_ports_all_queue_id_qid_all_queue_drop_state_enable_disable(
                                                                                               cparser_context_t *context,
                                                                                               char * *ports_ptr,
                                                                                               char * *qid_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    switch (DIAG_UTIL_CHIP_TYPE) {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            {

            }
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            {

            }
            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_set_egress_port_ports_all_queue_id_qid_all_queue_drop_state_enable_disable */

/*
 * flowctrl get egress port ( <PORT_LIST:ports> | all ) queue-id ( <MASK_LIST:qid> | all ) queue-drop state
 */
cparser_result_t
cparser_cmd_flowctrl_get_egress_port_ports_all_queue_id_qid_all_queue_drop_state(
                                                                                cparser_context_t *context,
                                                                                char * *ports_ptr,
                                                                                char * *qid_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("");
    switch (DIAG_UTIL_CHIP_TYPE) {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            {

            }
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            {

            }
            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_get_egress_port_ports_all_queue_id_qid_all_queue_drop_state */

/*
 * flowctrl set egress port-gap-threshold <UINT:threshold>
 */
cparser_result_t
cparser_cmd_flowctrl_set_egress_port_gap_threshold_threshold(
                                                            cparser_context_t *context,
                                                            uint32_t  *threshold_ptr)
{
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    switch (DIAG_UTIL_CHIP_TYPE) {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_portEegressGapThreshold_set(*threshold_ptr), ret);
                diag_util_mprintf("Set Egress Flow Control Port Gap Threshold to %u \n", *threshold_ptr);
            }
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_portEegressGapThreshold_set(*threshold_ptr), ret);
                diag_util_mprintf("Set Egress Flow Control Port Gap Threshold to %u \n", *threshold_ptr);
            }
            break;
#endif
#ifdef CONFIG_SDK_RTL9607C
        case RTL9607C_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_portEegressGapThreshold_set(*threshold_ptr), ret);
                diag_util_mprintf("Set Egress Flow Control Port Gap Threshold to %u \n", *threshold_ptr);
            }
            break;
#endif
#ifdef CONFIG_SDK_RTL9603CVD
        case RTL9603CVD_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_portEegressGapThreshold_set(*threshold_ptr), ret);
                diag_util_mprintf("Set Egress Flow Control Port Gap Threshold to %u \n", *threshold_ptr);
            }
            break;
#endif

        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_set_egress_port_gap_threshold_threshold */

/*
 * flowctrl get egress port-gap-threshold
 */
cparser_result_t
cparser_cmd_flowctrl_get_egress_port_gap_threshold(
                                                  cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint32 threshold;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch (DIAG_UTIL_CHIP_TYPE) {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_portEegressGapThreshold_get(&threshold), ret);
                diag_util_mprintf("Egress Flow Control Port  Gap Threshold is %u \n", threshold);
            }
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_portEegressGapThreshold_get(&threshold), ret);
                diag_util_mprintf("Egress Flow Control Port  Gap Threshold is %u \n", threshold);
            }
            break;
#endif
#ifdef CONFIG_SDK_RTL9607C
        case RTL9607C_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_portEegressGapThreshold_get(&threshold), ret);
                diag_util_mprintf("Egress Flow Control Port  Gap Threshold is %u \n", threshold);
            }
            break;
#endif
#ifdef CONFIG_SDK_RTL9603CVD
        case RTL9603CVD_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_portEegressGapThreshold_get(&threshold), ret);
                diag_util_mprintf("Egress Flow Control Port  Gap Threshold is %u \n", threshold);
            }
            break;
#endif

        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_get_egress_port_gap_threshold */

/*
 * flowctrl set egress queue-gap-threshold <UINT:threshold>
 */
cparser_result_t
cparser_cmd_flowctrl_set_egress_queue_gap_threshold_threshold(
                                                             cparser_context_t *context,
                                                             uint32_t  *threshold_ptr)
{
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    switch (DIAG_UTIL_CHIP_TYPE) {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_queueEegressGapThreshold_set(*threshold_ptr), ret);
                diag_util_printf("Set Egress Flow Control Queue Gap Threshold to %u \n", *threshold_ptr);
            }
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_queueEegressGapThreshold_set(*threshold_ptr), ret);
                diag_util_printf("Set Egress Flow Control Queue Gap Threshold to %u \n", *threshold_ptr);
            }
            break;
#endif
#ifdef CONFIG_SDK_RTL9607C
        case RTL9607C_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_queueEegressGapThreshold_set(*threshold_ptr), ret);
                diag_util_printf("Set Egress Flow Control Queue Gap Threshold to %u \n", *threshold_ptr);
            }
            break;
#endif
#ifdef CONFIG_SDK_RTL9603CVD
        case RTL9603CVD_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_queueEegressGapThreshold_set(*threshold_ptr), ret);
                diag_util_printf("Set Egress Flow Control Queue Gap Threshold to %u \n", *threshold_ptr);
            }
            break;
#endif

        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_set_egress_queue_gap_threshold_threshold */

/*
 * flowctrl get egress queue-gap-threshold
 */
cparser_result_t
cparser_cmd_flowctrl_get_egress_queue_gap_threshold(
                                                   cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint32 threshold;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch (DIAG_UTIL_CHIP_TYPE) {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_queueEegressGapThreshold_get(&threshold), ret);
                diag_util_mprintf("Egress Flow Control Queue Gap Threshold is %u \n", threshold);
            }
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_queueEegressGapThreshold_get(&threshold), ret);
                diag_util_mprintf("Egress Flow Control Queue Gap Threshold is %u \n", threshold);
            }
            break;
#endif
#ifdef CONFIG_SDK_RTL9607C
        case RTL9607C_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_queueEegressGapThreshold_get(&threshold), ret);
                diag_util_mprintf("Egress Flow Control Queue Gap Threshold is %u \n", threshold);
            }
            break;
#endif
#ifdef CONFIG_SDK_RTL9603CVD
        case RTL9603CVD_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_queueEegressGapThreshold_get(&threshold), ret);
                diag_util_mprintf("Egress Flow Control Queue Gap Threshold is %u \n", threshold);
            }
            break;
#endif

        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }
    
    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_get_egress_queue_gap_threshold */

/*
 * flowctrl set high-queue port ( <PORT_LIST:port> | all ) queue-mask ( <PORT_LIST:queue> | all )
 */
cparser_result_t
cparser_cmd_flowctrl_set_high_queue_port_port_all_queue_mask_queue_all(
                                                                      cparser_context_t *context,
                                                                      char * *port_ptr,
                                                                      char * *queue_ptr)
{
    int32           ret = RT_ERR_FAILED;
    rtk_port_t      port = 0;
    diag_portlist_t    portlist;
    diag_mask_t    queueMask;
    uint8   buf1[UTIL_PORT_MASK_BUFFER_LENGTH];
    uint8   buf2[UTIL_PORT_MASK_BUFFER_LENGTH];

    osal_memset(&queueMask, 0, sizeof(diag_mask_t));
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    RT_PARAM_CHK((NULL == *queue_ptr), CPARSER_ERR_INVALID_PARAMS);
    RT_PARAM_CHK((NULL == *port_ptr), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_QUEUEMASK(queueMask, 6), ret);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        switch (DIAG_UTIL_CHIP_TYPE) {
#if defined(CONFIG_SDK_APOLLO)
            case APOLLO_CHIP_ID:
                {
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_highQueueMask_set(port, queueMask.mask), ret);
                }
                break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
            case APOLLOMP_CHIP_ID:
                {
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_highQueueMask_set(port, queueMask.mask), ret);
                }
                break;
#endif
            default:
                diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
                return CPARSER_NOT_OK;
                break;
        }
    }

    diag_util_lPortMask2str(buf1, &portlist.portmask);
    diag_util_mask2str (buf2, queueMask.mask.bits[0]);
    diag_util_printf("Set High Queue Mask: Port %s Queues:%s \n",buf1,buf2);

    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_set_high_queue_port_port_all_queue_mask_queue_all */

/*
 * flowctrl get high-queue port ( <PORT_LIST:port> | all )
 */
cparser_result_t
cparser_cmd_flowctrl_get_high_queue_port_port_all(
                                                 cparser_context_t *context,
                                                 char * *port_ptr)
{
    int32               ret = RT_ERR_FAILED;
    rtk_port_t          port = 0;
    diag_portlist_t     portlist;
    rtk_bmp_t queueMask;

    DIAG_UTIL_PARAM_CHK();
    RT_PARAM_CHK((NULL == *port_ptr), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_OUTPUT_INIT();

    /* show specific port isolation info */
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    diag_util_mprintf("Get High Queue Mask\n");
    switch (DIAG_UTIL_CHIP_TYPE) {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            DIAG_UTIL_PORTMASK_SCAN(portlist, port)
            {
                DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_highQueueMask_get(port, &queueMask), ret);
                diag_util_mprintf("\tPort %2u : queue-mask = 0x%x\n", port, queueMask.bits[0]);
            }
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_PORTMASK_SCAN(portlist, port)
            {
                DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_highQueueMask_get(port, &queueMask), ret);
                diag_util_mprintf("\tPort %2u : queue-mask = 0x%x\n", port, queueMask.bits[0]);
            }
            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_get_high_queue_port_port_all */

/*
 * flowctrl set egress port ( <PORT_LIST:ports> | all ) queue-id ( <MASK_LIST:qid> | all ) queue-drop state ( enable | disable )
 */
/*
 * flowctrl set patch ( gpon-35m | fiber-35m | 20m )
 */
cparser_result_t
cparser_cmd_flowctrl_set_patch_gpon_35m_fiber_35m_20m(
                                                     cparser_context_t *context)
{
    rtk_flowctrl_patch_t type;
    int32 ret;

    switch (TOKEN_CHAR(3,0)) {
        case 'g':
            type = FLOWCTRL_PATCH_35M_GPON;
            break;
        case 'f':
            type = FLOWCTRL_PATCH_35M_FIBER;
            break;
        case '2':
            type = FLOWCTRL_PATCH_20M;
            break;
        default:
            return CPARSER_ERR_INVALID_PARAMS;
    }


    DIAG_UTIL_PARAM_CHK();
    switch (DIAG_UTIL_CHIP_TYPE) {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_patch(type), ret);
            }
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_patch(type), ret);
            }
            break;
#endif
#ifdef CONFIG_SDK_RTL9602C
		case RTL9602C_CHIP_ID:

			DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_patch(type), ret);
			break;
#endif

        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_set_patch_gpon_35m_fiber_35m_20m */


/*
 * flowctrl set pon max-page-cnt latch ( enable | disable )
 */
cparser_result_t
cparser_cmd_flowctrl_set_pon_max_page_cnt_latch_enable_disable(
                                                              cparser_context_t *context)
{
    rtk_enable_t enable;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    if ('e'==TOKEN_CHAR(5,0))
        enable = ENABLED;
    else if ('d'==TOKEN_CHAR(5,0))
        enable = DISABLED;

    switch (DIAG_UTIL_CHIP_TYPE) 
    {

#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_ponLatchMaxUsedPage_set(enable), ret);
                diag_util_mprintf("Set PON queue maximum latch to %s\n",diagStr_enable[enable]);
            }
            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_set_pon_max_page_cnt_latch_enable_disable */


/*
 * flowctrl get pon max-page-cnt latch
 */
cparser_result_t
cparser_cmd_flowctrl_get_pon_max_page_cnt_latch(
                                               cparser_context_t *context)
{
    rtk_enable_t enable;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch (DIAG_UTIL_CHIP_TYPE) {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            {

            }
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_ponLatchMaxUsedPage_get(&enable), ret);
                diag_util_mprintf("Maximum PON queue latch is %s\n",diagStr_enable[enable]);
            }
            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }


    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_get_pon_max_page_cnt_latch */

/*
 * flowctrl set ingress pon egress-drop queue-id ( <MASK_LIST:qid> | all ) threshold-index <UINT:index>
 */
cparser_result_t
cparser_cmd_flowctrl_set_ingress_pon_egress_drop_queue_id_qid_all_threshold_index_index(
                                                                                       cparser_context_t *context,
                                                                                       char * *qid_ptr,
                                                                                       uint32_t  *index_ptr)
{
    diag_mask_t  queuelist;
    rtk_qid_t queue;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_MASK128(queuelist, 6), ret);
    DIAG_UTIL_MASK_SCAN(queuelist, queue)
    {
        switch (DIAG_UTIL_CHIP_TYPE) {
#if defined(CONFIG_SDK_APOLLO)
            case APOLLO_CHIP_ID:
                {
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_ponQueueEegressDropIndex_set(queue, *index_ptr), ret);
                }
                break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
            case APOLLOMP_CHIP_ID:
                {
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_ponQueueEegressDropIndex_set(queue, *index_ptr), ret);
                }
                break;
#endif
            default:
                diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
                return CPARSER_NOT_OK;
                break;
        }
    }
    diag_util_printf("Queues:%s  Threshold index:%u\n",context->parser->tokens[(6)].buf, *index_ptr);

    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_set_ingress_pon_egress_drop_queue_id_qid_all_threshold_index_index */

/*
 * flowctrl get ingress pon egress-drop queue-id ( <MASK_LIST:qid> | all ) threshold-index
 */
cparser_result_t
cparser_cmd_flowctrl_get_ingress_pon_egress_drop_queue_id_qid_all_threshold_index(
                                                                                 cparser_context_t *context,
                                                                                 char * *qid_ptr)
{
    diag_mask_t  queuelist;
    rtk_qid_t queue;
    uint32 idx;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_MASK128(queuelist, 6), ret);

    diag_util_mprintf("Get Ingress Flow Control Queue Egress Drop Threshold Index \n");
    diag_util_mprintf("Egress-Queue       Drop-Threshold-Index \n");
    diag_util_mprintf("---------------------------------\n");

    switch (DIAG_UTIL_CHIP_TYPE) {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            DIAG_UTIL_MASK_SCAN(queuelist, queue)
            {
                DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_ponQueueEegressDropIndex_get(queue, &idx), ret);
                diag_util_mprintf("%-18u  %u\n", queue, idx);
            }
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_MASK_SCAN(queuelist, queue)
            {
                DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_ponQueueEegressDropIndex_get(queue, &idx), ret);
                diag_util_mprintf("%-18u  %u\n", queue, idx);
            }
            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_get_ingress_pon_egress_drop_queue_id_qid_all_threshold_index */

/*
 * flowctrl set ingress pon egress-drop queue-threshold-index ( <MASK_LIST:index> | all ) threshold <UINT:threshold>
 */
cparser_result_t
cparser_cmd_flowctrl_set_ingress_pon_egress_drop_queue_threshold_index_index_all_threshold_threshold(
                                                                                                    cparser_context_t *context,
                                                                                                    char * *index_ptr,
                                                                                                    uint32_t  *threshold_ptr)
{
    diag_mask_t  idxlist;
    uint32  idx;
    uint8   buf[UTIL_PORT_MASK_BUFFER_LENGTH];
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_MASK8(idxlist, 6), ret);
    DIAG_UTIL_MASK_SCAN(idxlist, idx)
    {
        switch (DIAG_UTIL_CHIP_TYPE) {
#if defined(CONFIG_SDK_APOLLO)
            case APOLLO_CHIP_ID:
                {
                    DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_ponQueueEegressDropThreshold_set(idx, *threshold_ptr), ret);
                }
                break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
            case APOLLOMP_CHIP_ID:
                {
                    DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_ponQueueEegressDropThreshold_set(idx, *threshold_ptr), ret);
                }
                break;
#endif
            default:
                diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
                return CPARSER_NOT_OK;
                break;
        }
    }
    diag_util_mask2str (buf, idxlist.mask.bits[0]);
    diag_util_printf("Threshold Index:%s Threshold:%u\n",buf, *threshold_ptr);

    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_set_ingress_pon_egress_drop_queue_threshold_index_index_all_threshold_threshold */

/*
 * flowctrl get ingress pon egress-drop queue-threshold-index ( <MASK_LIST:idx> | all ) threshold
 */
cparser_result_t
cparser_cmd_flowctrl_get_ingress_pon_egress_drop_queue_threshold_index_idx_all_threshold(
                                                                                        cparser_context_t *context,
                                                                                        char * *idx_ptr)
{
    diag_mask_t  idxlist;
    uint32 idx;
    uint32 threshold;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_MASK8(idxlist, 6), ret);

    diag_util_mprintf("Get Ingress Flow Control Queue Egress Drop Threshold  \n");
    diag_util_mprintf("Index           Drop-Threshold \n");
    diag_util_mprintf("---------------------------------\n");

    switch (DIAG_UTIL_CHIP_TYPE) {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            DIAG_UTIL_MASK_SCAN(idxlist, idx)
            {
                DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_ponQueueEegressDropThreshold_get(idx, &threshold), ret);
                diag_util_mprintf("%-18u  %u\n", idx, threshold);
            }
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_MASK_SCAN(idxlist, idx)
            {
                DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_ponQueueEegressDropThreshold_get(idx, &threshold), ret);
                diag_util_mprintf("%-18u  %u\n", idx, threshold);
            }
            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }


    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_get_ingress_pon_egress_drop_queue_threshold_index_idx_all_threshold */



/*
 * flowctrl set ingress pon egress-drop queue-gap threshold <UINT:threshold>
 */
cparser_result_t
cparser_cmd_flowctrl_set_ingress_pon_egress_drop_queue_gap_threshold_threshold(
                                                                              cparser_context_t *context,
                                                                              uint32_t  *threshold_ptr)
{
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    switch (DIAG_UTIL_CHIP_TYPE) {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_ponQueueEegressGapThreshold_set(*threshold_ptr), ret);
                diag_util_printf("Set PON MAC Queue Egress Gap Threshold to %u \n", *threshold_ptr);
            }
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_ponQueueEegressGapThreshold_set(*threshold_ptr), ret);
                diag_util_printf("Set PON MAC Queue Egress Gap Threshold to %u \n", *threshold_ptr);
            }
            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_set_ingress_pon_egress_drop_queue_gap_threshold_threshold */

/*
 * flowctrl get ingress pon egress-drop queue-gap threshold
 */
cparser_result_t
cparser_cmd_flowctrl_get_ingress_pon_egress_drop_queue_gap_threshold(
                                                                    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint32 threshold;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch (DIAG_UTIL_CHIP_TYPE) {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(apollo_raw_flowctrl_ponQueueEegressGapThreshold_get(&threshold), ret);
                diag_util_mprintf("Ingress Flow Control PON MAC Egress Gap Threshold is %u \n", threshold);
            }
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(apollomp_raw_flowctrl_ponQueueEegressGapThreshold_get(&threshold), ret);
                diag_util_mprintf("Ingress Flow Control PON MAC Egress Gap Threshold is %u \n", threshold);
            }
            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_get_ingress_pon_egress_drop_queue_gap_threshold */

/*
 * flowctrl set ingress pon egress-drop port ( <PORT_LIST:ports> | all ) queue-id ( <MASK_LIST:qid> | all ) drop ( enable | disable )
 */
cparser_result_t
cparser_cmd_flowctrl_set_ingress_pon_egress_drop_port_ports_all_queue_id_qid_all_drop_enable_disable(
                                                                                                    cparser_context_t *context,
                                                                                                    char * *ports_ptr,
                                                                                                    char * *qid_ptr)
{
    diag_portlist_t portlist;
    diag_mask_t  queuelist;
    rtk_port_t port;
    rtk_qid_t queue;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 6), ret);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_QUEUEMASK(queuelist, 8), ret);

    diag_util_printf("Set Ingress Flow Control Queue Egress Drop Enable for PON \n");
    diag_util_printf("            ");
    DIAG_UTIL_MASK_SCAN(queuelist, queue)
    {
        diag_util_printf("Q%d    ",queue);
    }
    diag_util_printf("\n");
    diag_util_printf("---------------------------------------------------------\n");
    switch (DIAG_UTIL_CHIP_TYPE) {
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
            DIAG_UTIL_PORTMASK_SCAN(portlist, port)
            {
                diag_util_printf("Port%2u  ", port);
                DIAG_UTIL_MASK_SCAN(queuelist, queue)
                {
                    if ('e'==TOKEN_CHAR(10,0)) {
                        DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_ponEgressDropEnable_set(port, queue, ENABLED), ret);
                        diag_util_printf("    En");
                    } else if ('d'==TOKEN_CHAR(10,0)) {
                        DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_ponEgressDropEnable_set(port, queue, DISABLED), ret);
                        diag_util_printf("   Dis");
                    } else
                        return RT_ERR_FAILED;
                }
                diag_util_printf("\n");
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
            DIAG_UTIL_PORTMASK_SCAN(portlist, port)
            {
                diag_util_printf("Port%2u  ", port);
                DIAG_UTIL_MASK_SCAN(queuelist, queue)
                {
                    if ('e'==TOKEN_CHAR(10,0)) {
                        DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_ponEgressDropEnable_set(port, queue, ENABLED), ret);
                        diag_util_printf("    En");
                    } else if ('d'==TOKEN_CHAR(10,0)) {
                        DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_ponEgressDropEnable_set(port, queue, DISABLED), ret);
                        diag_util_printf("   Dis");
                    } else
                        return RT_ERR_FAILED;
                }
                diag_util_printf("\n");
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
            DIAG_UTIL_PORTMASK_SCAN(portlist, port)
            {
                diag_util_printf("Port%2u  ", port);
                DIAG_UTIL_MASK_SCAN(queuelist, queue)
                {
                    if ('e'==TOKEN_CHAR(10,0)) {
                        DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_ponEgressDropEnable_set(port, queue, ENABLED), ret);
                        diag_util_printf("    En");
                    } else if ('d'==TOKEN_CHAR(10,0)) {
                        DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_ponEgressDropEnable_set(port, queue, DISABLED), ret);
                        diag_util_printf("   Dis");
                    } else
                        return RT_ERR_FAILED;
                }
                diag_util_printf("\n");
            }
            break;
#endif

        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_set_ingress_pon_egress_drop_port_ports_all_queue_id_qid_all_drop_enable_disable */

/*
 * flowctrl get ingress pon egress-drop port ( <PORT_LIST:ports> | all ) queue-id ( <MASK_LIST:qid> | all ) drop
 */
cparser_result_t
cparser_cmd_flowctrl_get_ingress_pon_egress_drop_port_ports_all_queue_id_qid_all_drop(
                                                                                     cparser_context_t *context,
                                                                                     char * *ports_ptr,
                                                                                     char * *qid_ptr)
{
    diag_portlist_t portlist;
    diag_mask_t  queuelist;
    rtk_port_t port;
    rtk_qid_t queue;
    rtk_enable_t enable;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 6), ret);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_QUEUEMASK(queuelist, 8), ret);

    diag_util_mprintf("Get Ingress Flow Control Queue Egress Drop Enable for PON \n");
    DIAG_UTIL_MASK_SCAN(queuelist, queue)
    {
        diag_util_printf("Q%d    ",queue);
    }
    diag_util_mprintf("\n");
    diag_util_mprintf("---------------------------------------------------------\n");
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        diag_util_printf("Port %2u ", port);
        DIAG_UTIL_MASK_SCAN(queuelist, queue)
        {
            switch (DIAG_UTIL_CHIP_TYPE) {

#if defined(CONFIG_SDK_RTL9602C)
                case RTL9602C_CHIP_ID:
                    {
                        DIAG_UTIL_ERR_CHK(rtl9602c_raw_flowctrl_ponEgressDropEnable_get(port, queue, &enable), ret);
                    }
                    break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
                case RTL9607C_CHIP_ID:
                    {
                        DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_ponEgressDropEnable_get(port, queue, &enable), ret);
                    }
                    break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
                case RTL9603CVD_CHIP_ID:
                    {
                        DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_ponEgressDropEnable_get(port, queue, &enable), ret);
                    }
                    break;
#endif

                default:
                    diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
                    return CPARSER_NOT_OK;
                    break;
            }
#if defined(CONFIG_SDK_RTL9602C) || defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
            diag_util_printf("   %3s",enable?"En":"Dis");
#endif
        }
#if defined(CONFIG_SDK_RTL9602C) || defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
        diag_util_mprintf("\n");
#endif
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_get_ingress_pon_egress_drop_port_ports_all_queue_id_qid_all_drop */
#endif

/*
 *flowctrl set egress-drop port ( <PORT_LIST:port> | all ) state ( enable | disable )
 */
cparser_result_t
cparser_cmd_flowctrl_set_egress_drop_port_port_all_state_enable_disable(
                                                                       cparser_context_t *context,
                                                                       char * *ports_ptr)
{
    diag_portlist_t portlist;
    rtk_port_t port;
    rtk_enable_t state;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if ('d'==TOKEN_CHAR(6,0))
        state = DISABLED;
    else if ('e'==TOKEN_CHAR(6,0))
        state = ENABLED;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        switch (DIAG_UTIL_CHIP_TYPE) {
#ifdef CONFIG_SDK_RTL9602C
            case RTL9602C_CHIP_ID:

                DIAG_UTIL_ERR_CHK(reg_array_field_write(RTL9602C_OUTPUT_DROP_ENr, port, REG_ARRAY_INDEX_NONE, RTL9602C_ENf,&state), ret);

                break;
#endif
#ifdef CONFIG_SDK_RTL9607C
            case RTL9607C_CHIP_ID:

                DIAG_UTIL_ERR_CHK(reg_array_field_write(RTL9607C_OUTPUT_DROP_ENr, port, REG_ARRAY_INDEX_NONE, RTL9607C_ENf,&state), ret);

                break;
#endif
#ifdef CONFIG_SDK_RTL9603CVD
            case RTL9603CVD_CHIP_ID:

                DIAG_UTIL_ERR_CHK(reg_array_field_write(RTL9603CVD_OUTPUT_DROP_ENr, port, REG_ARRAY_INDEX_NONE, RTL9603CVD_ENf,&state), ret);

                break;
#endif

            default:
                diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
                return CPARSER_NOT_OK;
                break;
        }
    }


    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_set_egress_drop_port_port_all_state_enable_disable */

/*
 * flowctrl set egress-drop ( broadcast | multicast | unknown-unicast ) ( select | not )
 */
cparser_result_t
cparser_cmd_flowctrl_set_egress_drop_broadcast_multicast_unknown_unicast_select_not(
                                                                                   cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_enable_t sel = DISABLED;
    uint32 bitField;
    DIAG_UTIL_PARAM_CHK();

    if ('n' == TOKEN_CHAR(4,0)) {
        sel = DISABLED;
    } else if ('s' == TOKEN_CHAR(4,0)) {
        sel = ENABLED;
    }

    switch (DIAG_UTIL_CHIP_TYPE) {
#ifdef CONFIG_SDK_RTL9602C
        case RTL9602C_CHIP_ID:
            bitField = RTL9602C_OD_BC_SELf;
            if ('b' == TOKEN_CHAR(3,0)) {
                bitField = RTL9602C_OD_BC_SELf;
            } else if ('m' == TOKEN_CHAR(3,0)) {
                bitField = RTL9602C_OD_MC_SELf;
            } else if ('u' == TOKEN_CHAR(3,0)) {
                bitField = RTL9602C_OD_UC_SELf;
            }

            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9602C_OUTPUT_DROP_CFGr, bitField,&sel), ret);

            break;
#endif
#ifdef CONFIG_SDK_RTL9607C
        case RTL9607C_CHIP_ID:
            bitField = RTL9607C_OD_BC_SELf;
            if ('b' == TOKEN_CHAR(3,0)) {
                bitField = RTL9607C_OD_BC_SELf;
            } else if ('m' == TOKEN_CHAR(3,0)) {
                bitField = RTL9607C_OD_MC_SELf;
            } else if ('u' == TOKEN_CHAR(3,0)) {
                bitField = RTL9607C_OD_UC_SELf;
            }

            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9607C_OUTPUT_DROP_CFGr, bitField,&sel), ret);

            break;
#endif
#ifdef CONFIG_SDK_RTL9603CVD
        case RTL9603CVD_CHIP_ID:
            bitField = RTL9603CVD_OD_BC_SELf;
            if ('b' == TOKEN_CHAR(3,0)) {
                bitField = RTL9603CVD_OD_BC_SELf;
            } else if ('m' == TOKEN_CHAR(3,0)) {
                bitField = RTL9603CVD_OD_MC_SELf;
            } else if ('u' == TOKEN_CHAR(3,0)) {
                bitField = RTL9603CVD_OD_UC_SELf;
            }

            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9603CVD_OUTPUT_DROP_CFGr, bitField,&sel), ret);

            break;
#endif

        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }


    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_set_egress_drop_broadcast_multicast_unknown_unicast_select_not */

/*
 * flowctrl dump egress-drop
 */
cparser_result_t
cparser_cmd_flowctrl_dump_egress_drop(
                                     cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint32 state,bcSel,mcSel,ucSel;
    diag_portlist_t portlist;
    rtk_port_t port;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch (DIAG_UTIL_CHIP_TYPE) {
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
            {
                diag_util_mprintf("Drop state:\n");

                portlist.max = HAL_GET_MAX_PORT();
                portlist.min = HAL_GET_MIN_PORT();
                HAL_GET_ALL_PORTMASK(portlist.portmask);

                DIAG_UTIL_PORTMASK_SCAN(portlist, port)
                {
                    DIAG_UTIL_ERR_CHK(reg_array_field_read(RTL9602C_OUTPUT_DROP_ENr, port, REG_ARRAY_INDEX_NONE, RTL9602C_ENf, &state), ret);

                    diag_util_mprintf("Port %d: %s\n",port,state?"Enable":"Disable");
                }

                DIAG_UTIL_ERR_CHK(reg_field_read(RTL9602C_OUTPUT_DROP_CFGr, RTL9602C_OD_BC_SELf,&bcSel), ret);


                DIAG_UTIL_ERR_CHK(reg_field_read(RTL9602C_OUTPUT_DROP_CFGr, RTL9602C_OD_MC_SELf,&mcSel), ret);


                DIAG_UTIL_ERR_CHK(reg_field_read(RTL9602C_OUTPUT_DROP_CFGr, RTL9602C_OD_UC_SELf,&ucSel), ret);

                diag_util_mprintf("Broadcast: %s, Multicast: %s, Unicast: %s\n",
                                  bcSel?"Select":"Not",
                                  mcSel?"Select":"Not",
                                  ucSel?"Select":"Not");
            }

            break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
            {
                diag_util_mprintf("Drop state:\n");

                portlist.max = HAL_GET_MAX_PORT();
                portlist.min = HAL_GET_MIN_PORT();
                HAL_GET_ALL_PORTMASK(portlist.portmask);

                DIAG_UTIL_PORTMASK_SCAN(portlist, port)
                {
                    DIAG_UTIL_ERR_CHK(reg_array_field_read(RTL9607C_OUTPUT_DROP_ENr, port, REG_ARRAY_INDEX_NONE, RTL9607C_ENf,&state), ret);

                    diag_util_mprintf("Port %d: %s\n",port,state?"Enable":"Disable");
                }

                DIAG_UTIL_ERR_CHK(reg_field_read(RTL9607C_OUTPUT_DROP_CFGr, RTL9607C_OD_BC_SELf,&bcSel), ret);

                DIAG_UTIL_ERR_CHK(reg_field_read(RTL9607C_OUTPUT_DROP_CFGr, RTL9607C_OD_MC_SELf,&mcSel), ret);

                DIAG_UTIL_ERR_CHK(reg_field_read(RTL9607C_OUTPUT_DROP_CFGr, RTL9607C_OD_UC_SELf,&ucSel), ret);

                diag_util_mprintf("Broadcast: %s, Multicast: %s, Unicast: %s\n",
                                  bcSel?"Select":"Not",
                                  mcSel?"Select":"Not",
                                  ucSel?"Select":"Not");
            }

            break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
            {
                diag_util_mprintf("Drop state:\n");

                portlist.max = HAL_GET_MAX_PORT();
                portlist.min = HAL_GET_MIN_PORT();
                HAL_GET_ALL_PORTMASK(portlist.portmask);

                DIAG_UTIL_PORTMASK_SCAN(portlist, port)
                {
                    DIAG_UTIL_ERR_CHK(reg_array_field_read(RTL9603CVD_OUTPUT_DROP_ENr, port, REG_ARRAY_INDEX_NONE, RTL9603CVD_ENf,&state), ret);

                    diag_util_mprintf("Port %d: %s\n",port,state?"Enable":"Disable");
                }

                DIAG_UTIL_ERR_CHK(reg_field_read(RTL9603CVD_OUTPUT_DROP_CFGr, RTL9603CVD_OD_BC_SELf,&bcSel), ret);

                DIAG_UTIL_ERR_CHK(reg_field_read(RTL9603CVD_OUTPUT_DROP_CFGr, RTL9603CVD_OD_MC_SELf,&mcSel), ret);

                DIAG_UTIL_ERR_CHK(reg_field_read(RTL9603CVD_OUTPUT_DROP_CFGr, RTL9603CVD_OD_UC_SELf,&ucSel), ret);

                diag_util_mprintf("Broadcast: %s, Multicast: %s, Unicast: %s\n",
                                  bcSel?"Select":"Not",
                                  mcSel?"Select":"Not",
                                  ucSel?"Select":"Not");
            }

            break;
#endif

        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }


    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_dump_egress_drop */

/*
 *flowctrl set ingress port ( <PORT_LIST:port> | all ) back-pressure state ( enable | disable )
 */
cparser_result_t
cparser_cmd_flowctrl_set_ingress_port_port_all_back_pressure_state_enable_disable(
                                                                                  cparser_context_t *context,
                                                                                  char * *ports_ptr)
{
    diag_portlist_t portlist;
    rtk_port_t port;
    rtk_enable_t state = DISABLED;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if ('d'==TOKEN_CHAR(7,0))
        state = DISABLED;
    else if ('e'==TOKEN_CHAR(7,0))
        state = ENABLED;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        switch (DIAG_UTIL_CHIP_TYPE) {
#ifdef CONFIG_SDK_RTL9607C
            case RTL9607C_CHIP_ID:
                {
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_portBackPressureEnable_set(port, state), ret);
                }
                break;
#endif
#ifdef CONFIG_SDK_RTL9603CVD
            case RTL9603CVD_CHIP_ID:
                {
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_portBackPressureEnable_set(port, state), ret);
                }
                break;
#endif

            default:
                diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
                return CPARSER_NOT_OK;
                break;
        }
    }


    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_set_ingress_port_port_all_back_pressure_state_enable_disable */

/*
 *flowctrl get ingress port ( <PORT_LIST:port> | all ) back-pressure state
 */
cparser_result_t
cparser_cmd_flowctrl_get_ingress_port_port_all_back_pressure_state(
                                                                   cparser_context_t *context,
                                                                   char * *ports_ptr)
{
    diag_portlist_t portlist;
    rtk_port_t port;
    rtk_enable_t state;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    diag_util_mprintf("Port BKPRESS_EN\n");
    diag_util_mprintf("---- ----------");

    switch (DIAG_UTIL_CHIP_TYPE) 
    {
#ifdef CONFIG_SDK_RTL9607C
        case RTL9607C_CHIP_ID:
            DIAG_UTIL_PORTMASK_SCAN(portlist, port)
            {
                DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_portBackPressureEnable_get(port, &state), ret);
                diag_util_mprintf("\n%-4d %-10s", port, diagStr_enable[state]);
            }
            diag_util_printf("\n");
            break;
#endif
#ifdef CONFIG_SDK_RTL9603CVD
        case RTL9603CVD_CHIP_ID:
            DIAG_UTIL_PORTMASK_SCAN(portlist, port)
            {
                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_portBackPressureEnable_get(port, &state), ret);
                diag_util_mprintf("\n%-4d %-10s", port, diagStr_enable[state]);
            }
            diag_util_printf("\n");
            break;
#endif

        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_get_ingress_port_port_all_back_pressure_state */

/*
 *flowctrl set ingress extra-reserve threshold <UINT:threshold>
 */
cparser_result_t
cparser_cmd_flowctrl_set_ingress_extra_reserve_threshold_threshold(
                                                                   cparser_context_t *context,
                                                                   uint32_t  *threshold_ptr)
{
    int32 ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch (DIAG_UTIL_CHIP_TYPE) {
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_rsvPageBeforeFcDrop_set(*threshold_ptr), ret);
                diag_util_mprintf("Set extra-reserve threshold: %u\n", *threshold_ptr);
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_rsvPageBeforeFcDrop_set(*threshold_ptr), ret);
                diag_util_mprintf("Set extra-reserve threshold: %u\n", *threshold_ptr);
            }
            break;
#endif

        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_set_ingress_extra_reserve_threshold_threshold */

/*
 * flowctrl get ingress extra-reserve threshold
 */
cparser_result_t
cparser_cmd_flowctrl_get_ingress_extra_reserve_threshold(
                                                         cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint32 threshold;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch (DIAG_UTIL_CHIP_TYPE) {
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_rsvPageBeforeFcDrop_get(&threshold), ret);
                diag_util_mprintf("Get extra-reserve threshold: %u\n", threshold);
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_rsvPageBeforeFcDrop_get(&threshold), ret);
                diag_util_mprintf("Get extra-reserve threshold: %u\n", threshold);
            }
            break;
#endif

        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_get_ingress_extra_reserve_threshold */

/*
 * flowctrl set change-duplex-ctrl state ( enable | disable )
 */
cparser_result_t
cparser_cmd_flowctrl_set_change_duplex_ctrl_state_enable_disable(
                                                                 cparser_context_t *context)
{
    rtk_enable_t enable = DISABLED;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    if ('e'==TOKEN_CHAR(4,0))
        enable = ENABLED;
    else if ('d'==TOKEN_CHAR(4,0))
        enable = DISABLED;

    switch (DIAG_UTIL_CHIP_TYPE) {
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_changeDuplexEnable_set(enable), ret);
                diag_util_printf("Set Change Duplex Ctrl State to %s\n",diagStr_enable[enable]);
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_changeDuplexEnable_set(enable), ret);
                diag_util_printf("Set Change Duplex Ctrl State to %s\n",diagStr_enable[enable]);
            }
            break;
#endif

        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_set_change_duplex_ctrl_state_enable_disable */

/*
 * flowctrl get change-duplex-ctrl state
 */
cparser_result_t
cparser_cmd_flowctrl_get_change_duplex_ctrl_state(
                                                  cparser_context_t *context)
{
    rtk_enable_t enable;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch (DIAG_UTIL_CHIP_TYPE) {
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_changeDuplexEnable_get(&enable), ret);
                diag_util_mprintf("Get Change Duplex Ctrl State is %s\n",diagStr_enable[enable]);
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_changeDuplexEnable_get(&enable), ret);
                diag_util_mprintf("Get Change Duplex Ctrl State is %s\n",diagStr_enable[enable]);
            }
            break;
#endif

        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
    }


    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_get_change_duplex_ctrl_state */


/*
 * flowctrl set ingress swpbo system ( high-off | high-on | low-off | low-on ) threshold <UINT:threshold>
*/
cparser_result_t
cparser_cmd_flowctrl_set_ingress_swpbo_system_high_off_high_on_low_off_low_on_threshold_threshold(
                                                                                               cparser_context_t *context,
                                                                                               uint32_t  *threshold_ptr)
{
    uint32 onThresh, offThresh;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch (DIAG_UTIL_CHIP_TYPE) {
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
            {
                if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_swpboGlobalHighThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_swpboGlobalHighThreshold_set(onThresh, *threshold_ptr), ret);
                    diag_util_printf("Set SWPBO MAC Global High Off Threshold to %u\n",*threshold_ptr);
                } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_swpboGlobalHighThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_swpboGlobalHighThreshold_set(*threshold_ptr, offThresh), ret);
                    diag_util_printf("Set SWPBO MAC Global High On Threshold to %u\n",*threshold_ptr);
                } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_swpboGlobalLowThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_swpboGlobalLowThreshold_set(onThresh, *threshold_ptr), ret);
                    diag_util_printf("Set SWPBO MAC Global Low Off Threshold to %u\n",*threshold_ptr);
                } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_swpboGlobalLowThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_swpboGlobalLowThreshold_set(*threshold_ptr, offThresh), ret);
                    diag_util_printf("Set SWPBO MAC Global Low On Threshold to %u\n",*threshold_ptr);
                }
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
            {
                if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_swpboGlobalHighThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_swpboGlobalHighThreshold_set(onThresh, *threshold_ptr), ret);
                    diag_util_printf("Set SWPBO MAC Global High Off Threshold to %u\n",*threshold_ptr);
                } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_swpboGlobalHighThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_swpboGlobalHighThreshold_set(*threshold_ptr, offThresh), ret);
                    diag_util_printf("Set SWPBO MAC Global High On Threshold to %u\n",*threshold_ptr);
                } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_swpboGlobalLowThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_swpboGlobalLowThreshold_set(onThresh, *threshold_ptr), ret);
                    diag_util_printf("Set SWPBO MAC Global Low Off Threshold to %u\n",*threshold_ptr);
                } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_swpboGlobalLowThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_swpboGlobalLowThreshold_set(*threshold_ptr, offThresh), ret);
                    diag_util_printf("Set SWPBO MAC Global Low On Threshold to %u\n",*threshold_ptr);
                }
            }
            break;
#endif

        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }
    return CPARSER_OK;
}

/*
 * flowctrl get ingress swpbo system ( high-off | high-on | low-off | low-on ) threshold
 */
cparser_result_t
cparser_cmd_flowctrl_get_ingress_swpbo_system_high_off_high_on_low_off_low_on_threshold(
                                                                                     cparser_context_t *context)
{
    uint32 onThresh, offThresh;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch (DIAG_UTIL_CHIP_TYPE) {
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
            {
                if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_swpboGlobalHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("PON SWPBO MAC Global High Off Threshold is %u\n",offThresh);
                } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_swpboGlobalHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("PON SWPBO MAC Global High On Threshold is %u\n",onThresh);
                } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_swpboGlobalLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("PON SWPBO MAC Global Low Off Threshold is %u\n",offThresh);
                } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_swpboGlobalLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("PON SWPBO MAC Global Low On Threshold is %u\n",onThresh);
                }
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
            {
                if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_swpboGlobalHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("PON SWPBO MAC Global High Off Threshold is %u\n",offThresh);
                } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_swpboGlobalHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("PON SWPBO MAC Global High On Threshold is %u\n",onThresh);
                } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_swpboGlobalLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("PON SWPBO MAC Global Low Off Threshold is %u\n",offThresh);
                } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_swpboGlobalLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("PON SWPBO MAC Global Low On Threshold is %u\n",onThresh);
                }
            }
            break;
#endif

        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_get_ingress_swpbo_system_high_off_high_on_low_off_low_on_threshold */

/*
 * flowctrl set ingress swpbo port ( high-off | high-on | low-off | low-on ) threshold <UINT:threshold>
 */
cparser_result_t
cparser_cmd_flowctrl_set_ingress_swpbo_port_high_off_high_on_low_off_low_on_threshold_threshold(
                                                                                             cparser_context_t *context,
                                                                                             uint32_t  *threshold_ptr)
{
    uint32 onThresh, offThresh;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch (DIAG_UTIL_CHIP_TYPE) {
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
            {
                if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_swpboPortHighThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_swpboPortHighThreshold_set(onThresh, *threshold_ptr), ret);
                    diag_util_printf("Set SWPBO MAC Port High Off Threshold to %u\n",*threshold_ptr);
                } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_swpboPortHighThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_swpboPortHighThreshold_set(*threshold_ptr, offThresh), ret);
                    diag_util_printf("Set SWPBO MAC Port High On Threshold to %u\n",*threshold_ptr);
                } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_swpboPortLowThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_swpboPortLowThreshold_set(onThresh, *threshold_ptr), ret);
                    diag_util_printf("Set SWPBO MAC Port Low Off Threshold to %u\n",*threshold_ptr);
                } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_swpboPortLowThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_swpboPortLowThreshold_set(*threshold_ptr, offThresh), ret);
                    diag_util_printf("Set SWPBO MAC Port Low On Threshold to %u\n",*threshold_ptr);
                }
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
            {
                if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_swpboPortHighThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_swpboPortHighThreshold_set(onThresh, *threshold_ptr), ret);
                    diag_util_printf("Set SWPBO MAC Port High Off Threshold to %u\n",*threshold_ptr);
                } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_swpboPortHighThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_swpboPortHighThreshold_set(*threshold_ptr, offThresh), ret);
                    diag_util_printf("Set SWPBO MAC Port High On Threshold to %u\n",*threshold_ptr);
                } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_swpboPortLowThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_swpboPortLowThreshold_set(onThresh, *threshold_ptr), ret);
                    diag_util_printf("Set SWPBO MAC Port Low Off Threshold to %u\n",*threshold_ptr);
                } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_swpboPortLowThreshold_get(&onThresh, &offThresh), ret);
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_swpboPortLowThreshold_set(*threshold_ptr, offThresh), ret);
                    diag_util_printf("Set SWPBO MAC Port Low On Threshold to %u\n",*threshold_ptr);
                }
            }
            break;
#endif

        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_set_ingress_swpbo_port_high_off_high_on_low_off_low_on_threshold_threshold */

/*
 * flowctrl get ingress swpbo port ( high-off | high-on | low-off | low-on  ) threshold
 */
cparser_result_t
cparser_cmd_flowctrl_get_ingress_swpbo_port_high_off_high_on_low_off_low_on_threshold(
                                                                                   cparser_context_t *context)
{
    uint32 onThresh, offThresh;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch (DIAG_UTIL_CHIP_TYPE) {
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
            {
                if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_swpboPortHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("SWPBO MAC Port High Off Threshold is %u\n",offThresh);
                } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_swpboPortHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("SWPBO MAC Port High On Threshold is %u\n",onThresh);
                } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_swpboPortLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("SWPBO MAC Port Low Off Threshold is %u\n",offThresh);
                } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_swpboPortLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("SWPBO MAC Port Low On Threshold is %u\n",onThresh);
                }
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
            {
                if (!osal_strcmp(TOKEN_STR(5),"high-off")) {
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_swpboPortHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("SWPBO MAC Port High Off Threshold is %u\n",offThresh);
                } else if (!osal_strcmp(TOKEN_STR(5),"high-on")) {
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_swpboPortHighThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("SWPBO MAC Port High On Threshold is %u\n",onThresh);
                } else if (!osal_strcmp(TOKEN_STR(5),"low-off")) {
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_swpboPortLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("SWPBO MAC Port Low Off Threshold is %u\n",offThresh);
                } else if (!osal_strcmp(TOKEN_STR(5),"low-on")) {
                    DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_swpboPortLowThreshold_get(&onThresh, &offThresh), ret);
                    diag_util_mprintf("SWPBO MAC Port Low On Threshold is %u\n",onThresh);
                }
            }
            break;
#endif

        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_get_ingress_swpbo_port_high_off_high_on_low_off_low_on_threshold */

/*
 * flowctrl set ingress egress-drop swpbo threshold <UINT:threshold>
 */
cparser_result_t
cparser_cmd_flowctrl_set_ingress_egress_drop_swpbo_threshold_threshold(
                                                                       cparser_context_t *context,
                                                                       uint32_t  *threshold_ptr)
{
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    diag_util_printf("Set Ingress Flow Control Swpbo Egress Drop Threshold \n");
    switch (DIAG_UTIL_CHIP_TYPE) {
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_swpboEgressDropThreshold_set(*threshold_ptr), ret);
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_swpboEgressDropThreshold_set(*threshold_ptr), ret);
            }
            break;
#endif

        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_set_ingress_egress_drop_swpbo_threshold_threshold */

/*
 * flowctrl get ingress egress-drop swpbo threshold
 */
cparser_result_t
cparser_cmd_flowctrl_get_ingress_egress_drop_swpbo_threshold(
                                                             cparser_context_t *context)
{
    uint32 threshold;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("Get Ingress Flow Control Swpbo Egress Drop Threshold \n");
    switch (DIAG_UTIL_CHIP_TYPE) {
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_swpboEgressDropThreshold_get(&threshold), ret);

                diag_util_mprintf("swpbo egress drop is %u\n", threshold);
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
            {
                DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_flowctrl_swpboEgressDropThreshold_get(&threshold), ret);

                diag_util_mprintf("swpbo egress drop is %u\n", threshold);
            }
            break;
#endif

        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_get_ingress_egress_drop_swpbo_threshold */

/*
 *flowctrl set ingress port ( <PORT_LIST:port> | all ) fc-drop-ctrl state ( enable | disable )
 */
cparser_result_t
cparser_cmd_flowctrl_set_ingress_port_port_all_fc_drop_ctrl_state_enable_disable(
                                                                                  cparser_context_t *context,
                                                                                  char * *ports_ptr)
{
    diag_portlist_t portlist;
    rtk_port_t port;
    rtk_enable_t state = DISABLED;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if ('d'==TOKEN_CHAR(7,0))
        state = DISABLED;
    else if ('e'==TOKEN_CHAR(7,0))
        state = ENABLED;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        switch (DIAG_UTIL_CHIP_TYPE) {
#ifdef CONFIG_SDK_RTL9607C
            case RTL9607C_CHIP_ID:
                {
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_portFcDropCtrlEnable_set(port, state), ret);
                }
                break;
#endif

            default:
                diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
                return CPARSER_NOT_OK;
                break;
        }
    }


    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_set_ingress_port_port_all_fc_drop_ctrl_state_enable_disable */

/*
 *flowctrl get ingress port ( <PORT_LIST:port> | all ) fc-drop-ctrl state
 */
cparser_result_t
cparser_cmd_flowctrl_get_ingress_port_port_all_fc_drop_ctrl_state(
                                                                   cparser_context_t *context,
                                                                   char * *ports_ptr)
{
    diag_portlist_t portlist;
    rtk_port_t port;
    rtk_enable_t state;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    diag_util_mprintf("Port FC-DROP-EN\n");
    diag_util_mprintf("---- ----------");

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        switch (DIAG_UTIL_CHIP_TYPE) {
#ifdef CONFIG_SDK_RTL9607C
            case RTL9607C_CHIP_ID:
                {
                    DIAG_UTIL_ERR_CHK(rtl9607c_raw_flowctrl_portFcDropCtrlEnable_get(port, &state), ret);
                    diag_util_mprintf("\n%-4d %-10s", port, diagStr_enable[state]);
                }
                break;
#endif

            default:
                diag_util_mprintf("\n%s\n",DIAG_STR_NOTSUPPORT);
                return CPARSER_NOT_OK;
                break;
        }
    }
    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_flowctrl_get_ingress_port_port_all_fc_drop_ctrl_state */

