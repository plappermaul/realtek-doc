/*
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 *
 * *
 * $Revision:  $
 * $Date: 2012-08-07
 *
 * Purpose : GPON MAC register access APIs
 *
 * Feature : Provide the APIs to access GPON MAC
 *
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/port.h>
#include <rtk/rt/rt_gpon.h>
#include <dal/ca8279/dal_ca8279.h>
#include <dal/ca8279/dal_rt_ca8279_gpon.h>
#include <dal/ca8279/dal_ca8279_l2.h>
#include <dal/ca8279/dal_ca8279_ponmac.h>


#include <osal/time.h>

#include <soc/cortina/registers.h>
#include <cortina-api/include/gpon.h>
#include <cortina-api/include/xgpon.h>
#include <cortina-api/include/xgpon_aes.h>
#include <cortina-api/include/port.h>
#include <cortina-api/include/vlan.h>
#include <cortina-api/include/special_packet.h>
#include <cortina-api/include/pkt_tx.h>

#include <aal_l2_vlan.h>
#include <aal_pon.h>
#include <aal_xgpon.h>
#include <aal_xgpon_actp.h>
#include <aal_xgpon_fec.h>
#include <aal_puc.h>
#include <aal_pdc.h>
#include <aal_ploam.h>
#include <aal_l2_qos.h>
#include "ni-drv/ca_ni.h"
#include "ca-ne-rtk/ca_ext.h"
#include "aal_port.h"
#include "aal_l2_specpkt.h"

//#include "aal/include/aal_gpon.h"
//#include "scfg.h"

static uint32 gpon_init = {INIT_NOT_COMPLETED};
static rt_enable_t  gOmciTrapEnable = DISABLED; //Default disable trap OMCI packet
static rt_enable_t  gOmciMirrorEnable = DISABLED; //Default disable mirror OMCI packet
static rt_port_t gMirroringPort;

static rt_gpon_omci_rx_callback gOmciRxCallback = NULL;
extern int drv_nic_register_rxhook(int portmask,int priority,p2rfunc_t rx);

/****************************************************************/
/* Symbol Definition                                            */
/****************************************************************/
#define SATURN_DEV_ID       (0)
#define SATURN_ANI_PORT_ID  (0)
#define SATURN_VLAN_DEV_ID  (1)
#define SATURN_VEIP_SRC_PORT     CA_PORT_ID(CA_PORT_TYPE_ETHERNET, CA_PORT_ID_NI6)
#define SATURN_WAN_PORT          CA_PORT_ID(CA_PORT_TYPE_GPON,     CA_PORT_ID_NI7)


#define SATURN_MAX_TCONT        (15)        /*0 for OMCC, 1~16 for data*/
#define SATURN_MAX_TCONT_QUEUE  (8)         /*Only support 0~7*/
#define SATURN_MAX_ALLOC_VAL    (0xFFFF)
#define SATURN_MAX_GEM_FLOW     (120)       /* 0~7 for OMCC, 8~127 for data*/
#define SATURN_MAX_PON_QUEUE     (120)       /* 0~7 for OMCC, 8~127 for data*/

#define SATURN_QUEUE_NUM_PER_TCONT  (8)

#define TCONT_ALLOC_ID_984_987_RESERVED (0xFFFF)

#define ETYPE_OFFSET    12
#define ETH_HEAD_LEN    16

static uint8 omci_eth[] = {0xff, 0xf1};
static uint8 vlan_eth[] = {0x81, 0x00};

static uint8 omciHeader[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x02,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
    0x88, 0xb5 };


#ifdef CONFIG_CORTINA_BOARD_SATURN_SFU_TITAN
#define AAL_FOR_ALL_USR_XGEM_DO(gem) \
        for ((gem) = 1; (gem) <= AAL_XGPON_MAX_GEM_PORT_IDX; (gem)++)

#define AAL_FOR_ALL_OMCI_XGEM_DO(gem) \
        for ((gem) = 0; (gem) <= 0; (gem)++)
#else
#define AAL_FOR_ALL_USR_XGEM_DO(gem) \
        for ((gem) = 8; (gem) <= AAL_XGPON_MAX_GEM_PORT_IDX; (gem)++)

#define AAL_FOR_ALL_OMCI_XGEM_DO(gem) \
        for ((gem) = 0; (gem) <= 7; (gem)++)
#endif
/****************************************************************/
/* Type Definition                                              */
/****************************************************************/

// to maintain streamId to gem_idx
typedef struct {
	unsigned short	gemIdx;
	unsigned short	cfIndex;//maintain saturn CF rule index
    unsigned char   isValid;
    unsigned char   aesEnable;
} dal_ca8279_flow2GemIdx_t;


typedef enum
{
    GEM_FLOW_DIR_US = 0x1,
    GEM_FLOW_DIR_DS = 0x2,
} GEM_FLOW_DIR;


/****************************************************************/
/* Macro Declaration                                            */
/****************************************************************/
//Tcont ID 0 for OMCC channel use
#define LOGIC_TCONID_TO_CA8279_TCONID(_logicTcontId) ((_logicTcontId)+1)
#define CA8279_TCONID_TO_LOGIC_TCONID(_logicTcontId) ((_logicTcontId)-1)

/*CA 8279 Queue0~7 for CMCC use */
#define LOGIC_QID_TO_CA8279_QID(_logicQid) ((_logicQid)+8)

#define GEM_IDX_TO_FLOW(_gem_idx)   (((_gem_idx) >> 0 )&0xFF)
#define GEM_IDX_TO_TQID(_gem_idx)   (((_gem_idx) >> 8 )&0x07)
#define GEM_IDX_TO_TCID(_gem_idx)   (((_gem_idx) >> 11)&0x3f)

#define TO_GEM_IDX(_flow, _tcid, _tqid)  ((((_tcid) & 0x1f) << 11 ) | ((_tqid) << 8) | (_flow& 0xff))

#define RATE_8KBPS_TO_KBPS(_8kbps)  ((_8kbps)*8)
#define RATE_KBPS_TO_8KBPS(_kbps)  ((_kbps)/8)
#define RATE_KBPS_TO_VOQ_RATE_M(_kbps)  ((_kbps)/1000)
#define RATE_KBPS_TO_VOQ_RATE_K(_kbps)  ((_kbps)%1000)


/*CA 8279 GEM flow 0~7 for OMCC use */
#define LOGIC_FLOW_TO_CA8279_FLOW(_gemFlow) ((_gemFlow)+8)


#define IS_OMCC_TCONT(_tcontId) (_tcontId==0)


#define CA8279_LOG_ERR(fmt, arg...)  printk(KERN_ERR "[ca8279]:\t" fmt "\n",## arg)
#define CA8279_LOG_DBG(fmt, arg...)  printk(KERN_INFO "[ca8279]:\t" fmt "\n",## arg) 



/****************************************************************/
/* Data Declaration                                             */
/****************************************************************/
static dal_ca8279_flow2GemIdx_t gGemUsFlowMap[SATURN_MAX_GEM_FLOW],gGemDsFlowMap[SATURN_MAX_GEM_FLOW];
static uint32 gTcontAllocId[SATURN_MAX_TCONT];


/*Defien local function*/
static int32 
_dal_ca8279_gpon_db_init(void)
{
    unsigned short gemFlow,tcontId;

    for(gemFlow = 0;gemFlow<SATURN_MAX_GEM_FLOW;gemFlow++)
    {
        gGemUsFlowMap[gemFlow].isValid = FALSE;
        gGemUsFlowMap[gemFlow].cfIndex = 0x0;
        gGemUsFlowMap[gemFlow].gemIdx  = 0x0;
		gGemUsFlowMap[gemFlow].aesEnable = 0x0;
    }

    for(gemFlow = 0;gemFlow<SATURN_MAX_GEM_FLOW;gemFlow++)
    {
        gGemDsFlowMap[gemFlow].isValid = FALSE;
        gGemDsFlowMap[gemFlow].cfIndex = 0x0;
        gGemDsFlowMap[gemFlow].gemIdx  = 0x0;
		gGemDsFlowMap[gemFlow].aesEnable  = 0x0;
    }

    for(tcontId=0;tcontId<SATURN_MAX_TCONT;tcontId++)
    {
        gTcontAllocId[tcontId] = TCONT_ALLOC_ID_984_987_RESERVED;
    }

    return RT_ERR_OK;
}


static int 
_AssignNonUsedTcontId(unsigned int allocId, unsigned int *pTcontId)
{
    unsigned int i;
    for(i=0;i<SATURN_MAX_TCONT;i++)
    {
        if(gTcontAllocId[i] == allocId)
        {
            *pTcontId = i;
            return RT_ERR_OK;
        }
    }

    for(i=0;i<SATURN_MAX_TCONT;i++)
    {
        if(gTcontAllocId[i] == TCONT_ALLOC_ID_984_987_RESERVED)
        {
            gTcontAllocId[i] = allocId;
            *pTcontId = i;
            return RT_ERR_OK;
        }
    }
    return RT_ERR_FAILED;
}

static int 
_GetTcontIdByAllocId(unsigned int allocId, unsigned int *pTcontId)
{
    unsigned int i;
       
    for(i=0;i<SATURN_MAX_TCONT;i++)
    {
        if(gTcontAllocId[i] == allocId)
        {
            *pTcontId = i;
            return RT_ERR_OK;
        }

    }    
    return RT_ERR_FAILED;
}

static int 
_GetAllocIdByTcontId(unsigned int tcontId, unsigned int *pAllocId)
{
    if(TCONT_ALLOC_ID_984_987_RESERVED != gTcontAllocId[tcontId])
    {
        *pAllocId = gTcontAllocId[tcontId];
        return RT_ERR_OK;
    }    
	
    return RT_ERR_FAILED;
}

static int 
_GetNonUsedTcontId(unsigned int *pTcontId)
{
    unsigned int i;
   
    for(i=0;i<SATURN_MAX_TCONT;i++)
    {
        if(TCONT_ALLOC_ID_984_987_RESERVED == gTcontAllocId[i])
        {
            *pTcontId = i;
            return RT_ERR_OK;
        }

    }
    return RT_ERR_FAILED;
}

static int 
_DelTcontId(unsigned int tcontId)
{


    RT_PARAM_CHK((tcontId >= SATURN_MAX_TCONT), RT_ERR_INPUT);
    
    gTcontAllocId[tcontId] = TCONT_ALLOC_ID_984_987_RESERVED;
        
    return RT_ERR_OK;
}


static int32 
gpon_usFlowMap_add(unsigned short gemFlowId, unsigned short gemIdx, unsigned int *pRuleIndex)
{
    unsigned int                   priority = 0;
    ca_classifier_key_t            key;
    ca_classifier_key_mask_t       key_mask;
    ca_classifier_action_t         action;
    ca_status_t     ret           = CA_E_OK;
    
	memset(&key, 0, sizeof(ca_classifier_key_t));
	memset(&key_mask, 0, sizeof(ca_classifier_key_mask_t));
	memset(&action, 0, sizeof(ca_classifier_action_t));


    ret = ca_l2_vlan_create(SATURN_VLAN_DEV_ID, gemFlowId);
    if ((CA_E_OK != ret) && (CA_E_EXISTS != ret))
    {
        CA8279_LOG_ERR("%s %d vlan create fail vid:%d ret:%d",
            __FUNCTION__, __LINE__, gemFlowId, ret);
        RT_ERR(ret, (MOD_DAL|MOD_GPON), "");
        return RT_ERR_FAILED;
    }

    key.src_port = SATURN_VEIP_SRC_PORT;

    key.l2.vlan_otag.vlan_max.vid = gemFlowId;
    key.l2.vlan_otag.vlan_min.vid = gemFlowId;
    key.l2.vlan_count = 1;

    key_mask.src_port = 1;
    key_mask.l2 = 1;
    key_mask.l2_mask.vlan_otag = 1;/*only filter outer tag */
    key_mask.l2_mask.vlan_otag_mask.vid =1;


    action.forward = CA_CLASSIFIER_FORWARD_PORT;
    action.dest.port = SATURN_WAN_PORT;
    action.options.masks.action_handle = 1;
    action.options.action_handle.gem_index = gemIdx;

    action.options.outer_vlan_act       = CA_CLASSIFIER_VLAN_ACTION_POP;
    action.options.masks.outer_vlan_act = 1;

    if (CA_E_OK != ca_classifier_rule_add(SATURN_DEV_ID, priority, &key, &key_mask, &action, pRuleIndex))
    {
        CA8279_LOG_ERR("%s %d CF add gemFlowId:%d gemIdx:0x%x fail",
            __FUNCTION__,__LINE__,gemFlowId,gemIdx);

        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}



static int32
gpon_usFlowMap_del(unsigned short index)
{
    if (CA_E_OK != ca_classifier_rule_delete(SATURN_DEV_ID, index))
    {
        CA8279_LOG_ERR("%s %d CF delete gemFlowId:%d fail",
            __FUNCTION__,__LINE__,index);

        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

static unsigned char 
__tcont_tbl_is_empty (unsigned short ignoreGemFlowId)
{
    unsigned short gemFlowId;
    unsigned int tcontId = GEM_IDX_TO_TCID(gGemUsFlowMap[ignoreGemFlowId].gemIdx);
    

    for (gemFlowId = 0; gemFlowId < SATURN_MAX_GEM_FLOW; gemFlowId++) 
    {
        if(ignoreGemFlowId == gemFlowId)
            continue;
        
        if (gGemUsFlowMap[gemFlowId].isValid) 
        {
            if(GEM_IDX_TO_TCID(gGemUsFlowMap[gemFlowId].gemIdx) == tcontId)
                return FALSE;
        }
    }

    return (TRUE);
}


static  int _SetGemFlowMap(
    GEM_FLOW_DIR dir, 
    unsigned short gemFlowId, 
    unsigned short gemIdx,
    unsigned char aesEnable)

{
    unsigned int ruleIdx;
    
    /* parameter check */
    RT_PARAM_CHK((gemFlowId >= SATURN_MAX_GEM_FLOW), RT_ERR_INPUT);

    if(GEM_FLOW_DIR_DS == dir)
    {
        gGemDsFlowMap[gemFlowId].isValid = TRUE; 
        gGemDsFlowMap[gemFlowId].gemIdx = gemIdx;
    }

    else
    {
        if((TRUE == gGemUsFlowMap[gemFlowId].isValid) && gGemUsFlowMap[gemFlowId].gemIdx == gemIdx)
        {
            /*Use same CF rule,only update aes data base*/
            gGemUsFlowMap[gemFlowId].aesEnable  = aesEnable;

            CA8279_LOG_ERR("%s %d gemFlowId:%d same gemIdx:0x%x",
                __FUNCTION__, __LINE__, gemFlowId, gemIdx);
            return RT_ERR_OK;             
        }

        if((TRUE == gGemUsFlowMap[gemFlowId].isValid) && gGemUsFlowMap[gemFlowId].gemIdx != gemIdx)
        {        
            if (RT_ERR_OK != gpon_usFlowMap_del(gGemUsFlowMap[gemFlowId].cfIndex))
            {
                CA8279_LOG_ERR("%s %d gemFlowId:%d fail",
                    __FUNCTION__,__LINE__,gemFlowId);
                
                return RT_ERR_FAILED;
            }
        }


        if (RT_ERR_OK != gpon_usFlowMap_add(gemFlowId,gemIdx,&ruleIdx))
        {
            CA8279_LOG_ERR("%s %d CF add gemFlowId:%d gemIdx:0x%x fail",
                __FUNCTION__,__LINE__,gemFlowId,gemIdx);

            return RT_ERR_FAILED;
        }
        
        gGemUsFlowMap[gemFlowId].isValid = TRUE;
        gGemUsFlowMap[gemFlowId].gemIdx = gemIdx; 
        gGemUsFlowMap[gemFlowId].cfIndex = ruleIdx;
		gGemUsFlowMap[gemFlowId].aesEnable  = aesEnable;
    }
    return RT_ERR_OK;

}


static unsigned int 
_DeleteGemFlowMap(unsigned short gemFlowId,GEM_FLOW_DIR dir)
{   
    /* parameter check */
    RT_PARAM_CHK((gemFlowId >= SATURN_MAX_GEM_FLOW), RT_ERR_INPUT);

    if(GEM_FLOW_DIR_DS == dir)
    {
        gGemDsFlowMap[gemFlowId].isValid = FALSE;
        gGemDsFlowMap[gemFlowId].gemIdx = 0x0;
    } 
    else
    {
        if (RT_ERR_OK != gpon_usFlowMap_del(gGemUsFlowMap[gemFlowId].cfIndex))
        {
            CA8279_LOG_ERR("%s %d gemFlowId:%d",
                __FUNCTION__,__LINE__,gemFlowId);
            
            return RT_ERR_FAILED;
        }

        gGemUsFlowMap[gemFlowId].isValid = FALSE;
        gGemUsFlowMap[gemFlowId].gemIdx = 0x0;
        gGemUsFlowMap[gemFlowId].cfIndex = 0x0;
		gGemUsFlowMap[gemFlowId].aesEnable  = 0x0;
    }  
    return RT_ERR_OK;
;

}

/*Direct call AAL API*/
static int 
_ca8279_gpon_aal_tcont_set(unsigned int tcontId, unsigned int allocId)
{
    aal_xgpon_bwmp_alloc_id_cfg_msk_t mask;
    aal_xgpon_bwmp_alloc_id_cfg_t alloc_id_cfg;

    if(IS_OMCC_TCONT(tcontId)){
        CA8279_LOG_ERR("%s:%d::ERROR: TCONT %d only used for OMCC!",__FUNCTION__,__LINE__,tcontId);
        return RT_ERR_FAILED;
    }

    if (allocId > 0xFFFF ){
        CA8279_LOG_ERR("%s:%d:: ERROR alloc_id is :%d",__FUNCTION__,__LINE__,allocId);
        return RT_ERR_FAILED;
    }

    mask.u32 = 0;
    mask.s.alloc_id =1;
    mask.s.omci_en =1;
    mask.s.ploam_en =1;
    alloc_id_cfg.alloc_id = allocId;
    alloc_id_cfg.omci_en = 1;
    alloc_id_cfg.ploam_en = 1;

    if (CA_E_OK !=aal_xgpon_bwmp_tcont_set(SATURN_DEV_ID, AAL_XGPON_WAVE_CHANNEL_DEFAULT, tcontId, mask, &alloc_id_cfg))
    {
        CA8279_LOG_ERR("%s:%d::create tcont_id:%d fail",__FUNCTION__,__LINE__,tcontId);
        return RT_ERR_FAILED;
    }

    CA8279_LOG_DBG("Add TCONT %u Alloc-ID %u",tcontId,  allocId);

    return RT_ERR_OK;

}


static int 
_ca8279_gpon_aal_tcont_del(unsigned int tcontId)
{
    aal_xgpon_bwmp_alloc_id_cfg_msk_t mask;
    aal_xgpon_bwmp_alloc_id_cfg_t alloc_id_cfg;

    if(IS_OMCC_TCONT(tcontId)){
        CA8279_LOG_ERR("%s:%d::ERROR: TCONT %d only used for OMCC!",__FUNCTION__,__LINE__,tcontId);
        return RT_ERR_FAILED;
    }


    ca_mdelay(1); /* wait for USBG burst finish */

    if (CA_E_OK != aal_xgpon_tcont_flush(SATURN_DEV_ID, AAL_XGPON_WAVE_CHANNEL_DEFAULT, tcontId))
    {
        CA8279_LOG_ERR("%s:%d::ERROR delete tcont_id:%d fail",__FUNCTION__,__LINE__,tcontId);
        return RT_ERR_FAILED;
    }

    mask.u32 = 0;
    mask.s.alloc_id =1;
    mask.s.omci_en =1;
    mask.s.ploam_en =1;
    alloc_id_cfg.alloc_id = 0x3aad;
    alloc_id_cfg.omci_en = 0;
    alloc_id_cfg.ploam_en = 0;


    if (CA_E_OK != aal_xgpon_bwmp_tcont_set(SATURN_DEV_ID, AAL_XGPON_WAVE_CHANNEL_DEFAULT, tcontId, mask, &alloc_id_cfg))
    {
        CA8279_LOG_ERR("%s:%d::delete tcont_id:%d fail",__FUNCTION__,__LINE__,tcontId);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;

}


static int 
_ca8279_gpon_aal_usGem_set(unsigned int usFlowId, const rt_gpon_usFlow_t *pUsFlow, unsigned short *pGemIdx)
{
    aal_xgpon_usbg_gem_config_msk_t us_mask;
    aal_xgpon_usbg_gem_config_t us_xgem_cfg;
    unsigned int internal_gem_idx;

    /* parameter check */
    RT_PARAM_CHK((NULL == pUsFlow), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((usFlowId >= SATURN_MAX_GEM_FLOW), RT_ERR_INPUT);

    if(IS_OMCC_TCONT(pUsFlow->tcontId)){
        CA8279_LOG_ERR("%s:%d::ERROR: TCONT %d only used for OMCC!",__FUNCTION__,__LINE__,pUsFlow->tcontId);
        return RT_ERR_FAILED;
    }

    internal_gem_idx = LOGIC_FLOW_TO_CA8279_FLOW(usFlowId);
       
    /*add us gem table*/
    us_mask.u32 = ~0U;
    
    us_xgem_cfg.port_en     = 1;
    us_xgem_cfg.xgem_id     = pUsFlow->gemPortId;
    us_xgem_cfg.aes_mode    = pUsFlow->aesEn;
    us_xgem_cfg.mc_flag     = 0;

    if (CA_E_OK != aal_xgpon_usbg_xgem_cfg_set(SATURN_DEV_ID, internal_gem_idx, us_mask, &us_xgem_cfg))
    {
        CA8279_LOG_ERR("%s:%d::ERROR set usbg xgem fail",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    

    /*Enable Tcont*/
    if (CA_E_OK != aal_puc_pvtbl_enable_set(SATURN_DEV_ID, pUsFlow->tcontId, TRUE))
    {
        CA8279_LOG_ERR("%s:%d::ERROR set pvtbl fail",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    
    *pGemIdx = TO_GEM_IDX(internal_gem_idx, pUsFlow->tcontId, pUsFlow->tcQueueId);

    return RT_ERR_OK;
}


static int 
_ca8279_gpon_aal_usGem_get(unsigned int usFlowId, rt_gpon_usFlow_t *pUsFlow)
{
    aal_xgpon_usbg_gem_config_t us_xgem_cfg;
    unsigned int internal_gem_idx;

    /* parameter check */
    RT_PARAM_CHK((NULL == pUsFlow), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((usFlowId >= SATURN_MAX_GEM_FLOW), RT_ERR_INPUT);

    internal_gem_idx = LOGIC_FLOW_TO_CA8279_FLOW(usFlowId);
    if (CA_E_OK != aal_xgpon_usbg_xgem_cfg_get(SATURN_DEV_ID, internal_gem_idx, &us_xgem_cfg))
    {
        CA8279_LOG_ERR("%s:%d::ERROR get usbg xgem fail",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    pUsFlow->gemPortId = us_xgem_cfg.xgem_id;
    pUsFlow->aesEn = us_xgem_cfg.aes_mode;
    return RT_ERR_OK;
}


static int 
_ca8279_gpon_aal_dsGem_set(unsigned int dsFlowId, const rt_gpon_dsFlow_t *pDsFlow, unsigned short *pGemIdx)
{
    aal_xgpon_dsfp_xgem_cfg_msk_t ds_mask;
    aal_xgpon_dsfp_xgem_cfg_t   ds_xgem_cfg;
    unsigned int internal_gem_idx;


    /* parameter check */
    RT_PARAM_CHK((NULL == pDsFlow), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((dsFlowId >= SATURN_MAX_GEM_FLOW), RT_ERR_INPUT);


    internal_gem_idx = LOGIC_FLOW_TO_CA8279_FLOW(dsFlowId);

    ds_mask.u32 = 0xffffffff;

    ds_xgem_cfg.gem_vld = 1;
    ds_xgem_cfg.gem_id = pDsFlow->gemPortId;
    ds_xgem_cfg.gem_mc = pDsFlow->isMcGem;
    if (CA_E_OK != aal_xgpon_dsfp_xgem_set(SATURN_DEV_ID, internal_gem_idx, ds_mask, &ds_xgem_cfg))
    {
        CA8279_LOG_ERR("%s:%d::set ds gem fail",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }


#if 1
    (void)aal_xgpon_dsfp_xgem_get(SATURN_DEV_ID, 0, &ds_xgem_cfg);

    if (internal_gem_idx > 0 && pDsFlow->gemPortId != ds_xgem_cfg.gem_id) {
        /* once OMCI configures GEM port: PDC_CTRL.pdc_map_mem_en = 1,
         * setup PDC_PDC_MAP_MEM:
         * PDC_PDC_MAP_MEM_DATA policer_id = internal GEM ID, PDC_PDC_MAP_MEM_DATA0.
         * no-drop = 0 for GEM port id != 0;
         */
        aal_pdc_ctrl_mask_t             pdc_ctrl_mask;
        aal_pdc_ctrl_t                  pdc_ctrl;
        aal_pdc_pdc_map_mem_data_mask_t pdc_map_mask;
        aal_pdc_pdc_map_mem_data_t      pdc_map_cfg;

        memset(&pdc_ctrl, 0, sizeof(pdc_ctrl));
        pdc_ctrl_mask.wrd               = 0;
#if defined(CONFIG_CORTINA_BOARD_SATURN_SFU_TITAN)
        pdc_ctrl_mask.bf.pdc_map_mem_en = 1;
        //pdc_ctrl.pdc_map_mem_en         = 1; // TODO: should always be 0 after test, should confirm with Donggun&ASIC

        ret = aal_pdc_ctrl_set(SATURN_DEV_ID, pdc_ctrl_mask, &pdc_ctrl);
        CA_ERR_GOTO_FUNC_EXIT(SATURN_DEV_ID, ret);
#endif
        pdc_map_mask.wrd        = 0;
        memset(&pdc_map_cfg, 0, sizeof(pdc_map_cfg));

        pdc_map_mask.bf.no_drop = 1;
        pdc_map_mask.bf.pol_en  = 1;
        pdc_map_mask.bf.pol_id  = 1;
        pdc_map_mask.bf.lspid   = 1;

        pdc_map_cfg.no_drop     = 0;
        pdc_map_cfg.pol_en      = 1;
        pdc_map_cfg.pol_id      = internal_gem_idx; /* internal GEM ID */
        /******12 5th 2017, Jianguang add below codes to support gem_index to src port map*******/
        pdc_map_cfg.lspid       = aal_pdc_free_lspid_get(SATURN_DEV_ID,internal_gem_idx);
        /**end of Jianguang add**/
        if (CA_E_OK != aal_pdc_map_mem_set(SATURN_DEV_ID, internal_gem_idx, pdc_map_mask, &pdc_map_cfg))
        {
            CA8279_LOG_ERR("%s:%d::set pdc map fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

    }

#endif

    *pGemIdx = TO_GEM_IDX(internal_gem_idx, 0x1F, 0x7);

    return RT_ERR_OK;

}


static int 
_ca8279_gpon_aal_dsGem_get(unsigned int dsFlowId, rt_gpon_dsFlow_t *pDsFlow)
{
    aal_xgpon_dsfp_xgem_cfg_t   ds_xgem_cfg;
    unsigned int internal_gem_idx;

    /* parameter check */
    RT_PARAM_CHK((NULL == pDsFlow), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((dsFlowId >= SATURN_MAX_GEM_FLOW), RT_ERR_INPUT);

    internal_gem_idx = LOGIC_FLOW_TO_CA8279_FLOW(dsFlowId);
    if (CA_E_OK != aal_xgpon_dsfp_xgem_get(SATURN_DEV_ID, internal_gem_idx, &ds_xgem_cfg))
    {
        CA8279_LOG_ERR("%s:%d::get ds gem fail",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    pDsFlow->gemPortId = ds_xgem_cfg.gem_id;
    pDsFlow->aesEn = 0; //downstream encryption is decided by OLT in runtime, so no such information

    return RT_ERR_OK;
}


static int 
_ca8279_gpon_aal_usGem_del(unsigned int usFlowId)
{
    aal_xgpon_usbg_gem_config_msk_t us_mask;
    aal_xgpon_usbg_gem_config_t us_xgem_cfg;
    unsigned int internal_gem_idx;

    RT_PARAM_CHK((usFlowId >= SATURN_MAX_GEM_FLOW), RT_ERR_INPUT);

    internal_gem_idx = LOGIC_FLOW_TO_CA8279_FLOW(usFlowId);

    
    memset(&us_xgem_cfg, 0, sizeof(aal_xgpon_usbg_gem_config_t));
    us_mask.u32 = 0xffffffff;

    //us_xgem_cfg.xgem_id = pUsFlow->gemPortId;
    us_xgem_cfg.port_en = FALSE;

    
    if (CA_E_OK != aal_xgpon_usbg_xgem_cfg_set(SATURN_DEV_ID, internal_gem_idx, us_mask, &us_xgem_cfg))
    {
        CA8279_LOG_ERR("%s:%d::Error Disable usbg gem fail",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }


    if (__tcont_tbl_is_empty(usFlowId)) 
    {
        aal_xgpon_bwmp_alloc_id_cfg_t  aal_bwmp_cfg;
        aal_xgpon_dsfp_xgem_cfg_t      aal_dsfp_cfg;
        unsigned int tcontId = GEM_IDX_TO_TCID(gGemUsFlowMap[usFlowId].gemIdx);

        memset(&aal_bwmp_cfg, 0, sizeof(aal_bwmp_cfg));

        if (CA_E_OK != aal_xgpon_bwmp_tcont_get(SATURN_DEV_ID, 0, tcontId, &aal_bwmp_cfg))
        {
            CA8279_LOG_ERR("%s:%d::Error bwmp get fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }


        if (CA_E_OK != aal_xgpon_dsfp_xgem_get(SATURN_DEV_ID, 0, &aal_dsfp_cfg))
        {
            CA8279_LOG_ERR("%s:%d::Error dsfp get fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if (CA_E_OK != aal_xgpon_tcont_flush(SATURN_DEV_ID, 0, tcontId))
        {
            CA8279_LOG_ERR("%s:%d::Error tcont:%d flush fail",__FUNCTION__,__LINE__,tcontId);
            return RT_ERR_FAILED;
        }


        if (aal_bwmp_cfg.alloc_id != aal_dsfp_cfg.gem_id) 
        {
            if (CA_E_OK != aal_puc_pvtbl_enable_set(SATURN_DEV_ID, tcontId, FALSE))
            {
                CA8279_LOG_ERR("%s:%d::Error pus pvtbl set fail",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
        }
    }
    
    return RT_ERR_OK;
}

static int 
_ca8279_gpon_aal_dsGem_del(unsigned int dsFlowId)
{
    aal_xgpon_dsfp_xgem_cfg_msk_t ds_mask;
    aal_xgpon_dsfp_xgem_cfg_t   ds_xgem_cfg;
    aal_pdc_pdc_map_mem_data_mask_t pdc_map_mask;
    aal_pdc_pdc_map_mem_data_t      pdc_map_cfg;
    unsigned short internal_gem_idx = 0;


    internal_gem_idx = LOGIC_FLOW_TO_CA8279_FLOW(dsFlowId);

    ds_mask.u32 = 0xffffffff;
    memset(&ds_xgem_cfg, 0, sizeof(aal_xgpon_dsfp_xgem_cfg_t));
    
    ds_xgem_cfg.gem_vld = 0;
    //ds_xgem_cfg.gem_id = pDsFlow->gemPortId;

    if (CA_E_OK != aal_xgpon_dsfp_xgem_set(SATURN_DEV_ID, internal_gem_idx, ds_mask, &ds_xgem_cfg))
    {
        CA8279_LOG_ERR("%s:%d::Disable ds gem fail",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

#if 1
    /******12 5th 2017, Jianguang add below codes to support gem_index to src port map*******/
    pdc_map_mask.wrd = 0;
    memset(&pdc_map_cfg,0,sizeof(aal_pdc_pdc_map_mem_data_t));

    if (CA_E_OK != aal_pdc_map_mem_get(SATURN_DEV_ID,internal_gem_idx,&pdc_map_cfg))
    {
        CA8279_LOG_ERR("%s:%d::pdc map get fail",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }


    if (CA_E_OK != aal_pdc_lspid_gem_index_set(SATURN_DEV_ID,pdc_map_cfg.lspid,0xff))
    {
        CA8279_LOG_ERR("%s:%d::pdc lspid get fail",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    pdc_map_cfg.lspid = 7;
    pdc_map_mask.bf.lspid = 1;
    if (CA_E_OK != aal_pdc_map_mem_set(SATURN_DEV_ID,internal_gem_idx,pdc_map_mask,&pdc_map_cfg))
    {
        CA8279_LOG_ERR("%s:%d::pdc set fail",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    /******end of add new function **************/
#endif

    return RT_ERR_OK;

}

static int _ca8279_gpon_aal_pon_queue_add(
    unsigned int tcontId, 
    unsigned int pri, 
    unsigned int queue_id, 
    unsigned int weight
)

{
    aal_puc_queue_cfg_t queue_cfg;
    aal_puc_voq_sche_cfg_t voq_sche_config;
    aal_puc_voq_drop_cfg_msk_t voq_drp_mask;
    aal_puc_voq_drop_cfg_t voq_drp;
    aal_puc_queue_cfg_msk_t queue_cfg_mask;
    ca_uint32_t tbc = 0;

    CA8279_LOG_DBG("%s:%d::tcontId:%d pri:%d queue_id:%d weight:%d",
        __FUNCTION__, 
        __LINE__,
        tcontId,
        pri,
        queue_id,
        weight);


    if(IS_OMCC_TCONT(tcontId)){
        CA8279_LOG_ERR("%s:%d::ERROR: TCONT %d only used for OMCC!",__FUNCTION__,__LINE__,tcontId);
        return RT_ERR_FAILED;
    }

    /*Disable queue drop*/
    voq_drp_mask.u32      = 0;
    voq_drp_mask.s.voqid  = 1;
    voq_drp_mask.s.enable = 1;

    memset(&voq_drp, 0, sizeof(voq_drp));
    voq_drp.enable = 0;
    voq_drp.voqid = queue_id;
    if (CA_E_OK != aal_puc_voq_drop_set(SATURN_DEV_ID, voq_drp_mask, &voq_drp))
    {
        CA8279_LOG_ERR("%s:%d::set pus voq drop fail",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*Set Tcont Pri queue mapping*/
    queue_cfg_mask.u32 = ~0U;    
    queue_cfg.queue_id = queue_id;
    queue_cfg.queue_weight = weight;
    queue_cfg.queue_pri = pri;
	queue_cfg.queue_ena = 1;
    if (CA_E_OK != aal_puc_port_voq_map_set(SATURN_DEV_ID, tcontId, pri, queue_cfg_mask, &queue_cfg))
    {
        CA8279_LOG_ERR("%s:%d::set voq map fail",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*Set voq sche table */
	memset(&voq_sche_config, 0, sizeof(aal_puc_voq_sche_cfg_t));
    if (CA_E_OK != aal_puc_voq_sche_cfg_get(SATURN_DEV_ID,&voq_sche_config))
    {
        CA8279_LOG_ERR("%s:%d::set voq map fail",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
	if(voq_sche_config.basesel == 0)
		tbc = weight * 128;
	if(voq_sche_config.basesel == 1)
		tbc = weight * 256;
	if(voq_sche_config.basesel == 2)
		tbc = weight * 512;
	if(voq_sche_config.basesel == 3)
		tbc = weight * 1024;
    if (CA_E_OK != aal_puc_port_voq_sche_info_tbl_set(SATURN_DEV_ID,tcontId,pri,tbc))
    {
        CA8279_LOG_ERR("%s:%d::set voq sche fail",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;

}


static int _ca8279_gpon_aal_pon_queue_get(
    unsigned int tcontId, 
    unsigned int pri, 
    unsigned int *weight
)
{
    aal_puc_queue_cfg_t queue_cfg;
    ca_uint32_t tbc = 0;

    if(IS_OMCC_TCONT(tcontId)){
        CA8279_LOG_ERR("%s:%d::ERROR: TCONT %d only used for OMCC!",__FUNCTION__,__LINE__,tcontId);
        return RT_ERR_FAILED;
    }

    if (CA_E_OK != aal_puc_port_voq_map_get(SATURN_DEV_ID, tcontId, pri, &queue_cfg))
    {
        CA8279_LOG_ERR("%s:%d::get voq map fail",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if (!queue_cfg.queue_ena)
    {
        CA8279_LOG_ERR("%s:%d::the queue is not enabled!",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    *weight = queue_cfg.queue_weight;

    return RT_ERR_OK;
}


static int _ca8279_gpon_aal_pon_queue_del(
    unsigned int tcont_id, 
    unsigned int pri)

{
    aal_puc_queue_cfg_msk_t old_mask;
    aal_puc_queue_cfg_t voq_cfg_old;

    if(IS_OMCC_TCONT(tcont_id)){
        CA8279_LOG_ERR("%s:%d::ERROR: TCONT %d only used for OMCC!",__FUNCTION__,__LINE__,tcont_id);
        return RT_ERR_FAILED;
    }
    
    old_mask.u32 = 0;
    memset(&voq_cfg_old, 0, sizeof(voq_cfg_old));

    if (CA_E_OK != aal_puc_port_voq_map_get(SATURN_DEV_ID, tcont_id, pri,&voq_cfg_old))
    {
        CA8279_LOG_ERR("%s:%d::get voq map fail",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    
    if (CA_E_OK != aal_puc_voq_flush_by_idx(SATURN_DEV_ID, tcont_id,pri,pri))
    {
        CA8279_LOG_ERR("%s:%d::flush voq  fail",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*Set Tcont PRI queue to orignal mapping*/
    old_mask.s.queue_ena = 1;
    old_mask.s.queue_id = 1;
    old_mask.s.queue_weight = 1;

    voq_cfg_old.queue_ena = 1;
    voq_cfg_old.queue_id = 0;
    if(voq_cfg_old.queue_weight != 0){
    	voq_cfg_old.queue_weight = 1;
    }

    if (CA_E_OK != aal_puc_port_voq_map_set(SATURN_DEV_ID, tcont_id, pri,old_mask,&voq_cfg_old))
    {
        CA8279_LOG_ERR("%s:%d::set voq map fail",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }



    /*Set sche table to original*/
    if (CA_E_OK != aal_puc_port_voq_sche_info_tbl_set(SATURN_DEV_ID,tcont_id,pri,0))
    {
        CA8279_LOG_ERR("%s:%d::set voq sch fail",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

int32
_ca8279_gpon_omci_mirror(uint32 msgLen, uint8 *pMsg)
{

    ca_pkt_t halPkt;
    ca_pkt_block_t pkt_data;
    int32 ret;
    u8 *pData;
	int encap_length, total_len;

    /* parameter check */
    RT_PARAM_CHK((NULL == pMsg), RT_ERR_NULL_POINTER);

    encap_length = sizeof(omciHeader);
    total_len = msgLen + encap_length;

    pData = kmalloc(total_len, GFP_KERNEL);
    if (pData == NULL) {

        CA8279_LOG_ERR("%s %d :Not enough memory to allocate skb of size %d.\n",
            __FUNCTION__,__LINE__, total_len);
        return RT_ERR_FAILED;
    }
    memcpy(pData, omciHeader, encap_length);
    memcpy(pData + encap_length, pMsg, msgLen);    

    
    memset(&halPkt,0,sizeof(halPkt));
    halPkt.block_count    = 1;
    halPkt.cos            = 0;
    halPkt.device_id      = 0;
    halPkt.src_port_id    = CA_PORT_ID(CA_PORT_TYPE_CPU, 0x10);
    halPkt.dst_port_id    = CA_PORT_ID(CA_PORT_TYPE_ETHERNET, gMirroringPort);
    halPkt.pkt_type       = CA_PKT_TYPE_OTHERS;
    halPkt.pkt_len        = total_len;
    halPkt.pkt_data       = (ca_pkt_block_t *)&pkt_data;
    halPkt.pkt_data->len  = total_len;
    halPkt.pkt_data->data = pData;
    halPkt.flags          = CA_TX_BYPASS_FWD_ENGINE;

    if ((ret = __ca_tx(0, &halPkt)) != CA_E_OK)
    {
        CA8279_LOG_ERR("%s %d : Mirror OMCI to %d Error,ret=%d !!",
            __FUNCTION__,__LINE__, gMirroringPort, ret);
        kfree(pData);
        return RT_ERR_FAILED;
    }

    kfree(pData);

    return RT_ERR_OK;

}

static int _ca8279_gpon_omci_build_mic(uint8_t * omci_ptr)
{
    rt_gpon_omci_pkt_hdr_t *pHdr = (rt_gpon_omci_pkt_hdr_t *)omci_ptr;
    uint32_t genMic = 0;
	
    if (0x0a == pHdr->devId) // base omci format
    {

        if (CA_E_OK != ca_generate_omci_mic(SATURN_DEV_ID, RT_GPON_OMCI_MIC_DIR_US, omci_ptr, sizeof(rt_gpon_omci_base_pkt_t)-4, 4, &genMic))        
        {
            CA8279_LOG_ERR("%s %d generate OMCI MIC fail\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        memcpy(omci_ptr+sizeof(rt_gpon_omci_base_pkt_t)-4,&genMic,4);
    }
    else if (0x0b == pHdr->devId) //extend omci format
    {
        rt_gpon_omci_ext_pkt_t *pExtHdr = (rt_gpon_omci_ext_pkt_t *)omci_ptr;
        uint16_t contLen = 0;

        contLen = ntohs(pExtHdr->len);

        if(CA_E_OK != ca_generate_omci_mic(SATURN_DEV_ID, RT_GPON_OMCI_MIC_DIR_US, omci_ptr, sizeof(rt_gpon_omci_ext_pkt_t)+contLen, 4, &genMic))
        {
            CA8279_LOG_ERR("%s %d : Generate MIC Error!!",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }


        memcpy(omci_ptr+sizeof(rt_gpon_omci_ext_pkt_t)+contLen,&genMic,4);
    }
    else 
    {
        CA8279_LOG_ERR("%s %d : OMCI device id(%d))is not suppert",__FUNCTION__,__LINE__,pHdr->devId);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}


static int _ca8279_gpon_omci_check_mic(uint8_t * omci_ptr)
{
    rt_gpon_omci_pkt_hdr_t *pHdr = (rt_gpon_omci_pkt_hdr_t *)omci_ptr;
    
    if (0x0a == pHdr->devId) // base omci format
    {
        rt_gpon_omci_base_pkt_t *basePkt = (rt_gpon_omci_base_pkt_t *)omci_ptr;
        uint32_t genMic = 0;
        uint32_t oriMic = 0;

        oriMic = basePkt->crc;

        if (CA_E_OK != ca_generate_omci_mic(SATURN_DEV_ID, RT_GPON_OMCI_MIC_DIR_DS, omci_ptr, sizeof(rt_gpon_omci_base_pkt_t)-4, 4, &genMic))        
        {
            CA8279_LOG_ERR("%s %d generate OMCI MIC fail\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }


        if (oriMic != genMic)
        {
            CA8279_LOG_ERR("%s %d : GenMic [0x%x] oriMic [0x%x]",__FUNCTION__,__LINE__,genMic, oriMic);
            return RT_ERR_FAILED;
        }
    }
    else if (0x0b == pHdr->devId) //extend omci format
    {
        rt_gpon_omci_ext_pkt_t *pExtHdr = (rt_gpon_omci_ext_pkt_t *)omci_ptr;
        uint32_t genMic  = 0;
        uint32_t oriMic  = 0;
        uint16_t contLen = 0;

        contLen = ntohs(pExtHdr->len);
        memcpy(&oriMic, (omci_ptr+sizeof(rt_gpon_omci_ext_pkt_t)+contLen), 4);

        if(CA_E_OK != ca_generate_omci_mic(SATURN_DEV_ID, RT_GPON_OMCI_MIC_DIR_DS, omci_ptr, sizeof(rt_gpon_omci_ext_pkt_t)+contLen, 4, &genMic))
        {
            CA8279_LOG_ERR("%s %d : Generate MIC Error!!",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if (oriMic != genMic)
        {
            CA8279_LOG_ERR("%s %d : GenMic [0x%x] oriMic [0x%x]",__FUNCTION__,__LINE__,genMic, oriMic);
            return RT_ERR_FAILED;
        }
    }
    else 
    {
        CA8279_LOG_ERR("%s %d : OMCI device id(%d))is not suppert",__FUNCTION__,__LINE__,pHdr->devId);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}


int _ca8279_receive_from_nic(struct napi_struct *napi,struct net_device *dev, struct sk_buff *skb, nic_hook_private_t *nh_priv)
{

    uint8_t * omci_ptr;
    uint32_t omci_len;
    uint8 isOmciFlag = FALSE;
    
    
    if(0 == memcmp(&skb->data[12], omci_eth, sizeof(omci_eth)))
    {
        omci_ptr = skb->data+ETH_HEAD_LEN;
        omci_len = skb->len-ETH_HEAD_LEN;
        isOmciFlag = TRUE;
    }
    else if(0 == memcmp(&skb->data[12], vlan_eth, sizeof(vlan_eth))) //single VID
    {
        if(0 == memcmp(&skb->data[16], omci_eth, sizeof(omci_eth)))
        {
            omci_ptr = skb->data+ETH_HEAD_LEN+4;
            omci_len = skb->len-ETH_HEAD_LEN-4;
            isOmciFlag = TRUE;
       }
    }

    //check omci ether type
    if (isOmciFlag == TRUE)
    {
        if(gOmciRxCallback)
        {
            if(_ca8279_gpon_omci_check_mic(omci_ptr)!= RT_ERR_OK)
            {
                CA8279_LOG_ERR("%s %d : OMCI check mic Error!!",__FUNCTION__,__LINE__);
                return RE8670_RX_STOP;
            }
            
            gOmciRxCallback(omci_len, omci_ptr);
            if(ENABLED == gOmciMirrorEnable)
            {
                _ca8279_gpon_omci_mirror(omci_len, omci_ptr);     
            }
        }


        return RE8670_RX_STOP;
    }

    return RE8670_RX_CONTINUE;

}

static int _ca8279_gpon_pm_phyLos_get(rt_gpon_pm_counter_t *pPmCnt)
{
    ca_phy_los_pm_t            stats  = {0};

    memset(&stats, 0x00, sizeof(ca_phy_los_pm_t));  

    if (CA_E_OK != ca_ngp2_port_pm_counters_get(SATURN_DEV_ID,
                                               SATURN_ANI_PORT_ID,
                                               CA_COUNTERS_PHY_LOS_PM_T,
                                               sizeof(ca_phy_los_pm_t),
                                               (ca_uint8_t *)&stats))
    {
        CA8279_LOG_ERR("%s:%d::get ONU PHY/LOS count fail",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    pPmCnt->phyLos.total_words_protected_by_bip = stats.total_words_protected_by_bip;
    pPmCnt->phyLos.bip_error_count = stats.bip_error_count;
    pPmCnt->phyLos.psbd_hec_errors_corrected = stats.psbd_hec_errors_corrected;
    pPmCnt->phyLos.psbd_hec_errors_uncorrectable = stats.psbd_hec_errors_uncorrectable;
    pPmCnt->phyLos.fs_header_errors_corrected = stats.fs_header_errors_corrected;
    pPmCnt->phyLos.fs_header_errors_uncorrectable = stats.fs_header_errors_uncorrectable;
    pPmCnt->phyLos.total_lods_event_count = stats.total_lods_event_count;
    pPmCnt->phyLos.lods_restored_in_operating_twdm_ch = stats.lods_restored_in_operating_twdm_ch;
    pPmCnt->phyLos.lods_restored_in_protection_twdm_ch = stats.lods_restored_in_protection_twdm_ch;
    pPmCnt->phyLos.lods_restored_in_discretionary_twdm_ch = stats.lods_restored_in_discretionary_twdm_ch;
    pPmCnt->phyLos.lods_reactivations = stats.lods_reactivations;
    pPmCnt->phyLos.lods_handshake_failure_in_protection_ch = stats.lods_handshake_failure_in_protection_ch;
    pPmCnt->phyLos.lods_handshake_failure_in_discretionary_ch = stats.lods_handshake_failure_in_discretionary_ch;

    return RT_ERR_OK;
}

static int _ca8279_gpon_pm_fec_get(rt_gpon_pm_counter_t *pPmCnt)
{
    ca_fec_pm_t            stats  = {0};

    memset(&stats, 0x00, sizeof(ca_fec_pm_t));  

    if (CA_E_OK != ca_ngp2_port_pm_counters_get(SATURN_DEV_ID,
                                               SATURN_ANI_PORT_ID,
                                               CA_COUNTERS_FEC_PM_T,
                                               sizeof(ca_fec_pm_t),
                                               (ca_uint8_t *)&stats))
    {
        CA8279_LOG_ERR("%s:%d::get ONU fec count fail",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    pPmCnt->fec.corrected_fec_bytes = stats.corrected_fec_bytes;
    pPmCnt->fec.corrected_fec_codewords = stats.corrected_fec_codewords;
    pPmCnt->fec.uncorrected_fec_codewords= stats.uncorrected_fec_codewords; 
    pPmCnt->fec.total_fec_codewords= stats.total_fec_codewords; 

    return RT_ERR_OK;
}

static int _ca8279_gpon_pm_xgem_get(rt_gpon_pm_counter_t *pPmCnt)
{
    ca_xgem_pm_t            stats  = {0};

    memset(&stats, 0x00, sizeof(ca_xgem_pm_t));  

    if (CA_E_OK != ca_ngp2_port_pm_counters_get(SATURN_DEV_ID,
                                               SATURN_ANI_PORT_ID,
                                               CA_COUNTERS_XGEM_PM_T,
                                               sizeof(ca_xgem_pm_t),
                                               (ca_uint8_t *)&stats))
    {
        CA8279_LOG_ERR("%s:%d::get ONU GEM count fail",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    pPmCnt->xgem.total_transmitted_xgem_frames = stats.total_transmitted_xgem_frames;
    pPmCnt->xgem.transmitted_xgem_frames_with_lf = stats.transmitted_xgem_frames_with_lf;
    pPmCnt->xgem.total_received_xgem_frames = stats.total_received_xgem_frames;
    pPmCnt->xgem.received_xgem_header_hec_errors = stats.received_xgem_header_hec_errors;
    pPmCnt->xgem.fs_words_lost_to_xgem_header_hec_errors = stats.fs_words_lost_to_xgem_header_hec_errors;
    pPmCnt->xgem.xgem_encryption_key_errors = stats.xgem_encryption_key_errors;
    pPmCnt->xgem.total_transmitted_bytes_in_non_idle_xgem_frames = stats.total_transmitted_bytes_in_non_idle_xgem_frames;
    pPmCnt->xgem.total_received_bytes_in_non_idle_xgem_frames = stats.total_received_bytes_in_non_idle_xgem_frames;

    return RT_ERR_OK;
}


static int _ca8279_gpon_pm_ploam1_get(rt_gpon_pm_counter_t *pPmCnt)
{
    ca_ploam_pm1_t            stats  = {0};

    memset(&stats, 0x00, sizeof(ca_ploam_pm1_t));  

    if (CA_E_OK != ca_ngp2_port_pm_counters_get(SATURN_DEV_ID,
                                               SATURN_ANI_PORT_ID,
                                               CA_COUNTERS_PLOAM_PM1_T,
                                               sizeof(ca_ploam_pm1_t),
                                               (ca_uint8_t *)&stats))
    {
        CA8279_LOG_ERR("%s:%d::get ONU PLOAM1 count fail",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    pPmCnt->ploam1.ploam_mic_errors = stats.ploam_mic_errors;
    pPmCnt->ploam1.downstream_ploam_message_count = stats.downstream_ploam_message_count;
    pPmCnt->ploam1.ranging_time_message_count = stats.ranging_time_message_count;
    pPmCnt->ploam1.protection_control_message_count = stats.protection_control_message_count;
    pPmCnt->ploam1.adjust_tx_wavelength_message_count = stats.adjust_tx_wavelength_message_count;
    pPmCnt->ploam1.adjust_tx_wavelength_amplitude = stats.adjust_tx_wavelength_amplitude;

    return RT_ERR_OK;
}

static int _ca8279_gpon_pm_ploam2_get(rt_gpon_pm_counter_t *pPmCnt)
{
    ca_ploam_pm2_t            stats  = {0};

    memset(&stats, 0x00, sizeof(ca_ploam_pm2_t));  

    if (CA_E_OK != ca_ngp2_port_pm_counters_get(SATURN_DEV_ID,
                                               SATURN_ANI_PORT_ID,
                                               CA_COUNTERS_PLOAM_PM2_T,
                                               sizeof(ca_ploam_pm2_t),
                                               (ca_uint8_t *)&stats))
    {
        CA8279_LOG_ERR("%s:%d::get ONU PLOAM2 count fail",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    pPmCnt->ploam2.system_profile_message_count = stats.system_profile_message_count;
    pPmCnt->ploam2.channel_profile_message_count = stats.channel_profile_message_count;
    pPmCnt->ploam2.burst_profile_message_count = stats.burst_profile_message_count;
    pPmCnt->ploam2.assign_onu_id_message_count = stats.assign_onu_id_message_count;
    pPmCnt->ploam2.unsatisfied_adjust_tx_wavelength_requests = stats.unsatisfied_adjust_tx_wavelength_requests;
    pPmCnt->ploam2.deactivate_onu_id_message_count = stats.deactivate_onu_id_message_count;
    pPmCnt->ploam2.disable_serial_number_message_count = stats.disable_serial_number_message_count;
    pPmCnt->ploam2.request_registration_message_count = stats.request_registration_message_count;
    pPmCnt->ploam2.assign_alloc_id_message_count = stats.assign_alloc_id_message_count;
    pPmCnt->ploam2.key_control_message_count = stats.key_control_message_count;
    pPmCnt->ploam2.sleep_allow_message_count = stats.sleep_allow_message_count;

    return RT_ERR_OK;
}

static int _ca8279_gpon_pm_ploam3_get(rt_gpon_pm_counter_t *pPmCnt)
{
    ca_ploam_pm3_t            stats  = {0};

    memset(&stats, 0x00, sizeof(ca_ploam_pm3_t));  

    if (CA_E_OK != ca_ngp2_port_pm_counters_get(SATURN_DEV_ID,
                                               SATURN_ANI_PORT_ID,
                                               CA_COUNTERS_PLOAM_PM3_T,
                                               sizeof(ca_ploam_pm3_t),
                                               (ca_uint8_t *)&stats))
    {
        CA8279_LOG_ERR("%s:%d::get ONU PLOAM3 count fail",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    pPmCnt->ploam3.upstream_ploam_message_count = stats.upstream_ploam_message_count;
    pPmCnt->ploam3.serial_number_onu_in_band_message_count = stats.serial_number_onu_in_band_message_count;
    pPmCnt->ploam3.serial_number_onu_amcc_message_count = stats.serial_number_onu_amcc_message_count;
    pPmCnt->ploam3.registration_message_count = stats.registration_message_count;
    pPmCnt->ploam3.acknowledgement_message_count = stats.acknowledgement_message_count;
    pPmCnt->ploam3.sleep_request_message_count = stats.sleep_request_message_count;
    pPmCnt->ploam3.tuning_response_ack_nack_msg_count = stats.tuning_response_ack_nack_msg_count;
    pPmCnt->ploam3.tuning_response_complete_u_rollback_msg_count = stats.tuning_response_complete_u_rollback_msg_count;
    pPmCnt->ploam3.power_consumption_report_message_count = stats.power_consumption_report_message_count;
    pPmCnt->ploam3.change_power_level_parameter_error_count = stats.change_power_level_parameter_error_count;

    return RT_ERR_OK;
}

static int _ca8279_gpon_pm_chan_tuning1_get(rt_gpon_pm_counter_t *pPmCnt)
{
    ca_chan_tuning_pm1_t            stats  = {0};

    memset(&stats, 0x00, sizeof(ca_chan_tuning_pm1_t));  

    if (CA_E_OK != ca_ngp2_port_pm_counters_get(SATURN_DEV_ID,
                                               SATURN_ANI_PORT_ID,
                                               CA_COUNTERS_CHAN_TUNING_PM1_T,
                                               sizeof(ca_chan_tuning_pm1_t),
                                               (ca_uint8_t *)&stats))
    {
        CA8279_LOG_ERR("%s:%d::get ONU chan_tuning1 count fail",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    pPmCnt->chanTuning1.tuning_control_requests_for_rx_only_or_rx_tx = stats.tuning_control_requests_for_rx_only_or_rx_tx;
    pPmCnt->chanTuning1.tuning_control_requests_for_tx_only = stats.tuning_control_requests_for_tx_only;
    pPmCnt->chanTuning1.tuning_control_requests_rejected_int_spc = stats.tuning_control_requests_rejected_int_spc;
    pPmCnt->chanTuning1.tuning_control_requests_rejected_ds_all = stats.tuning_control_requests_rejected_ds_all;
    pPmCnt->chanTuning1.tuning_control_requests_rejected_us_all = stats.tuning_control_requests_rejected_us_all;
    pPmCnt->chanTuning1.tuning_control_requests_ok_target_channel = stats.tuning_control_requests_ok_target_channel;
    pPmCnt->chanTuning1.tuning_control_requests_failed_to4_exp = stats.tuning_control_requests_failed_to4_exp;
    pPmCnt->chanTuning1.tuning_control_requests_failed_to5_exp = stats.tuning_control_requests_failed_to5_exp;
    pPmCnt->chanTuning1.tuning_control_requests_resolved_discret_ch = stats.tuning_control_requests_resolved_discret_ch;
    pPmCnt->chanTuning1.tuning_control_requests_rollback_com_ds = stats.tuning_control_requests_rollback_com_ds;
    pPmCnt->chanTuning1.tuning_control_requests_rollback_ds_all = stats.tuning_control_requests_rollback_ds_all;
    pPmCnt->chanTuning1.tuning_control_requests_rollback_us_all = stats.tuning_control_requests_rollback_us_all;
    pPmCnt->chanTuning1.tuning_control_requests_failed_reactivation = stats.tuning_control_requests_failed_reactivation;

    return RT_ERR_OK;
}

static int _ca8279_gpon_pm_chan_tuning2_get(rt_gpon_pm_counter_t *pPmCnt)
{
    ca_chan_tuning_pm2_t            stats  = {0};

    memset(&stats, 0x00, sizeof(ca_chan_tuning_pm2_t));  

    if (CA_E_OK != ca_ngp2_port_pm_counters_get(SATURN_DEV_ID,
                                               SATURN_ANI_PORT_ID,
                                               CA_COUNTERS_CHAN_TUNING_PM2_T,
                                               sizeof(ca_chan_tuning_pm2_t),
                                               (ca_uint8_t *)&stats))
    {
        CA8279_LOG_ERR("%s:%d::get ONU chan_tuning2 count fail",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    pPmCnt->chanTuning2.tuning_control_requests_rejected_ds_albl = stats.tuning_control_requests_rejected_ds_albl;
    pPmCnt->chanTuning2.tuning_control_requests_rejected_ds_void = stats.tuning_control_requests_rejected_ds_void;
    pPmCnt->chanTuning2.tuning_control_requests_rejected_ds_part = stats.tuning_control_requests_rejected_ds_part;
    pPmCnt->chanTuning2.tuning_control_requests_rejected_ds_tunr = stats.tuning_control_requests_rejected_ds_tunr;
    pPmCnt->chanTuning2.tuning_control_requests_rejected_ds_lnrt = stats.tuning_control_requests_rejected_ds_lnrt;
    pPmCnt->chanTuning2.tuning_control_requests_rejected_ds_lncd = stats.tuning_control_requests_rejected_ds_lncd;
    pPmCnt->chanTuning2.tuning_control_requests_rejected_us_albl = stats.tuning_control_requests_rejected_us_albl;
    pPmCnt->chanTuning2.tuning_control_requests_rejected_us_void = stats.tuning_control_requests_rejected_us_void;
    pPmCnt->chanTuning2.tuning_control_requests_rejected_us_tunr = stats.tuning_control_requests_rejected_us_tunr;
    pPmCnt->chanTuning2.tuning_control_requests_rejected_us_clbr = stats.tuning_control_requests_rejected_us_clbr;
    pPmCnt->chanTuning2.tuning_control_requests_rejected_us_lktp = stats.tuning_control_requests_rejected_us_lktp;
    pPmCnt->chanTuning2.tuning_control_requests_rejected_us_lnrt = stats.tuning_control_requests_rejected_us_lnrt;
    pPmCnt->chanTuning2.tuning_control_requests_rejected_us_lncd = stats.tuning_control_requests_rejected_us_lncd;

    return RT_ERR_OK;
}

static int _ca8279_gpon_pm_chan_tuning3_get(rt_gpon_pm_counter_t *pPmCnt)
{
    ca_chan_tuning_pm3_t            stats  = {0};

    memset(&stats, 0x00, sizeof(ca_chan_tuning_pm3_t));  

    if (CA_E_OK != ca_ngp2_port_pm_counters_get(SATURN_DEV_ID,
                                               SATURN_ANI_PORT_ID,
                                               CA_COUNTERS_CHAN_TUNING_PM3_T,
                                               sizeof(ca_chan_tuning_pm3_t),
                                               (ca_uint8_t *)&stats))
    {
        CA8279_LOG_ERR("%s:%d::get ONU chan_tuning3 count fail",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    pPmCnt->chanTuning3.tuning_control_requests_rollback_ds_albl = stats.tuning_control_requests_rollback_ds_albl;
    pPmCnt->chanTuning3.tuning_control_requests_rollback_ds_lktp = stats.tuning_control_requests_rollback_ds_lktp;
    pPmCnt->chanTuning3.tuning_control_requests_rollback_us_albl = stats.tuning_control_requests_rollback_us_albl;
    pPmCnt->chanTuning3.tuning_control_requests_rollback_us_void = stats.tuning_control_requests_rollback_us_void;
    pPmCnt->chanTuning3.tuning_control_requests_rollback_us_tunr = stats.tuning_control_requests_rollback_us_tunr;
    pPmCnt->chanTuning3.tuning_control_requests_rollback_us_lktp = stats.tuning_control_requests_rollback_us_lktp;
    pPmCnt->chanTuning3.tuning_control_requests_rollback_us_lnrt = stats.tuning_control_requests_rollback_us_lnrt;
    pPmCnt->chanTuning3.tuning_control_requests_rollback_us_lncd = stats.tuning_control_requests_rollback_us_lncd;

    return RT_ERR_OK;
}

static int _ca8279_gpon_pm_omci_get(rt_gpon_pm_counter_t *pPmCnt)
{
    ca_omci_pm_t            stats  = {0};
    /* OMCI counters for debug */
    aal_xgpon_usbg_xgem_mib_config_t gem_mib;
    aal_xgpon_dsfp_xgem_mib_config_t gem_mib_dn;
    ca_uint32_t  count_up[2] = {0};
    ca_uint32_t  count_dn[2] = {0};
    ca_uint32_t  gem = 0;

    memset(&stats, 0x00, sizeof(ca_omci_pm_t));  

#if 0 //OMCI counters should retrieved from OMCI application
    if (CA_E_OK != ca_ngp2_port_pm_counters_get(SATURN_DEV_ID,
                                               SATURN_ANI_PORT_ID,
                                               CA_COUNTERS_OMCI_PM_T,
                                               sizeof(ca_omci_pm_t),
                                               (ca_uint8_t *)&stats))
    {
        CA8279_LOG_ERR("%s:%d::get ONU OMCI count fail",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    pPmCnt->omci.omci_baseline_msg_count = stats.omci_baseline_msg_count;
    pPmCnt->omci.omci_extended_message_count = stats.omci_extended_message_count;
    pPmCnt->omci.omci_mic_error_count = stats.omci_mic_error_count;
#endif

    /* get OMCI counters from DSFP/USBG OMCI GEMs */
    count_dn[0] = 0;
    AAL_FOR_ALL_OMCI_XGEM_DO(gem) {
        memset(&gem_mib_dn, 0, sizeof(gem_mib_dn));
        aal_xgpon_dsfp_xgem_mib_get(0, gem, &gem_mib_dn);
        count_dn[0] += gem_mib_dn.frame_cnt;
    }

    count_up[0] = 0;
    AAL_FOR_ALL_OMCI_XGEM_DO(gem) {
        memset(&gem_mib, 0, sizeof(gem_mib));
        aal_xgpon_usbg_xgem_mib_get(0, gem, &gem_mib);
        count_up[0] += gem_mib.frame_cnt;
    }

    pPmCnt->omci.omci_tx_msg_count = count_up[0];
    pPmCnt->omci.omci_rx_msg_count = count_dn[0];

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rt_ca8279_gpon_init
 * Description:
 *      gpon register level initial function
 * Input:
 *
 * Output:
 *      
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 dal_rt_ca8279_gpon_init(void)
{
    aal_ilpb_cfg_msk_t              ilpb_cfg_mask;
    aal_ilpb_cfg_t                  ilpb_cfg;
    aal_elpb_cfg_msk_t              elpb_cfg_mask;
    aal_elpb_cfg_t                  elpb_cfg;
    aal_puc_ctrl_cfg_msk_t          puc_ctrl_config_mask; 
    aal_puc_ctrl_cfg_t              puc_ctrl_config; 

    ca_uint32_t pon_mode;

    pon_mode = aal_pon_mac_mode_get(0);

    if(pon_mode != ONU_PON_MAC_XGPON1 && pon_mode != ONU_PON_MAC_XGSPON)
    {
        RT_ERR(ret, (MOD_DAL|MOD_GPON), "");
        gpon_init = INIT_NOT_COMPLETED;
        return RT_ERR_OK;
    }


    _dal_ca8279_gpon_db_init();

    memset(&ilpb_cfg_mask, 0, sizeof(aal_ilpb_cfg_msk_t));
    memset(&elpb_cfg_mask, 0, sizeof(aal_elpb_cfg_msk_t));
    memset(&ilpb_cfg, 0, sizeof(aal_ilpb_cfg_t));
    memset(&elpb_cfg, 0, sizeof(aal_elpb_cfg_t));

    ilpb_cfg_mask.s.cos_mode_sel = 1;  
    ilpb_cfg.cos_mode_sel = AAL_COS_MODE_COS_SEL_BMP;

    ilpb_cfg_mask.s.ing_fltr_ena = 1;
    ilpb_cfg.ing_fltr_ena = 0;
    elpb_cfg_mask.s.egr_ve_srch_en = 1;
    elpb_cfg.egr_ve_srch_en = 0;

    /*Disable ingress VLAN member check*/
    if (CA_E_OK != aal_port_ilpb_cfg_set(SATURN_VLAN_DEV_ID, PORT_ID(SATURN_VEIP_SRC_PORT), ilpb_cfg_mask, &ilpb_cfg)) {
        CA8279_LOG_ERR("%s %d : init fail",__FUNCTION__,__LINE__);
    }

    /*Disable egress VLAN member check*/
    if (CA_E_OK != aal_port_elpb_cfg_set(SATURN_VLAN_DEV_ID, PORT_ID(SATURN_VEIP_SRC_PORT), elpb_cfg_mask, &elpb_cfg)) {
        CA8279_LOG_ERR("%s %d : init fail",__FUNCTION__,__LINE__);
    }


    /*Enable shaper function of PUC*/	
    memset(&puc_ctrl_config_mask, 0, sizeof(aal_puc_ctrl_cfg_msk_t));
    memset(&puc_ctrl_config, 0, sizeof(aal_puc_ctrl_cfg_t));
    puc_ctrl_config_mask.s.shp_en = 1;
    puc_ctrl_config_mask.s.addtok_en = 1;
    puc_ctrl_config_mask.s.subtok_en = 1;
    puc_ctrl_config.shp_en = 1;
    puc_ctrl_config.addtok_en = 1;
    puc_ctrl_config.subtok_en = 1;


    if (CA_E_OK != aal_puc_ctrl_cfg_set(SATURN_DEV_ID, puc_ctrl_config_mask, &puc_ctrl_config)) {
    	CA8279_LOG_ERR("%s %d : init fail",__FUNCTION__,__LINE__);
    }

    gpon_init = INIT_COMPLETED;
    return RT_ERR_OK;    
}   /* end of dal_ca8279_gpon_init */


/* Function Name:
 *      dal_rt_ca8279_gpon_serialNumber_set
 * Description:
 *      Set GPON serial number
 * Input:
 *      pSN   - pointer to the GPON serial number
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32
dal_rt_ca8279_gpon_serialNumber_set(rt_gpon_serialNumber_t *pSN)
{
    ca_status_t ret = CA_E_OK;
    ca_uint8_t serial_number[GPON_ONU_SN_LEN];    
    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pSN), RT_ERR_NULL_POINTER);    

    /* function body */
    memcpy(&serial_number[0], &pSN->vendor[0], GPON_ONU_SN_LEN);

    ret = ca_gpon_serial_number_set(SATURN_DEV_ID, &serial_number[0]);
    if(CA_E_OK != ret)
    {
        CA8279_LOG_ERR("%s %d set gpon sn  fail\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;


}   /* end of dal_ca8279_gpon_serialNumber_set */

/* Function Name:
 *      dal_rt_ca8279_gpon_registrationId_set
 * Description:
 *      Set GPON registration ID (password in 984.3).
 * Input:
 *      pRegId   - pointer to the GPON registration id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32
dal_rt_ca8279_gpon_registrationId_set(rt_gpon_registrationId_t *pRegId)
{
    ca_status_t   ret;

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pRegId), RT_ERR_NULL_POINTER);  

    ret = ca_gpon_registration_id_set(SATURN_DEV_ID, (ca_uint8_t *)pRegId);
    if(CA_E_OK != ret)
    {
        CA8279_LOG_ERR("%s %d set gpon rgsd id fail\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
}   /* end of dal_rt_ca8279_registrationId_set */


/* Function Name:
 *      dal_rt_ca8279_gpon_activate
 * Description:
 *      GPON MAC Activating.
 * Input:
 *      initState   - the initial state when ONU active
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      The GPON MAC is starting to work now.
 */
int32
dal_rt_ca8279_gpon_activate(rt_gpon_initialState_t initState)
{
    ca_uint32_t ret = CA_E_OK;

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((RT_GPON_ONU_INIT_STATE_O7 <initState), RT_ERR_INPUT);

    ca_mdelay(2000);
    if((ret = ca_port_enable_set( 0, CA_PORT_GPON_PORT, 1)) != CA_E_OK)
    {
        CA8279_LOG_ERR("%s %d activate GPON fail\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;

}   /* end of dal_rt_ca8279_activate */

/* Function Name:
 *      dal_rt_ca8279_gpon_deactivate
 * Description:
 *      GPON MAC Deactivate.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      The GPON MAC is out of work now.
*/
int32
dal_rt_ca8279_gpon_deactivate(void)
{
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* function body */
    if((ret = ca_port_enable_set( 0, CA_PORT_GPON_PORT, 0)) != CA_E_OK)
    {
        CA8279_LOG_ERR("%s %d deactivate GPON fail\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;

}   /* end of dal_rt_ca8279_gpon_deactivate */




/* Function Name:
 *      dal_rt_ca8279_gpon_onuState_get
 * Description:
 *      Get ONU State
 * Input:
 *
 * Output:
 *      pOnuState: point for get onu state
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 dal_rt_ca8279_gpon_onuState_get(rt_gpon_onuState_t  *pOnuState)
{
    int32 ret;
    rtk_port_t pon_port;
    ca_port_id_t port_id;
    ca_gpon_port_onu_states_t caGponState;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "");

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pOnuState), RT_ERR_NULL_POINTER);

    /* function body */
    pon_port = HAL_GET_PON_PORT();
    port_id = CA_PORT_ID(CA_PORT_TYPE_GPON,pon_port);
    if((ret = ca_gpon_port_onu_state_get(0, port_id, &caGponState))!=CA_E_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
	switch(caGponState)
    {
        case CA_NGP2_ONU_ACT_STATE_O1_1:	
            *pOnuState = RT_NGP2_ONU_STATE_O1_1;
        case CA_NGP2_ONU_ACT_STATE_O1_2:
            *pOnuState = RT_NGP2_ONU_STATE_O1_2;
            break;
        case CA_NGP2_ONU_ACT_STATE_O2_3:
            *pOnuState = RT_NGP2_ONU_STATE_O2_3;
            break;
        case CA_NGP2_ONU_ACT_STATE_O4:
            *pOnuState = RT_NGP2_ONU_STATE_O4;
            break;
        case CA_NGP2_ONU_ACT_STATE_O5_1:	
            *pOnuState = RT_NGP2_ONU_STATE_O5_1;
            break;
        case CA_NGP2_ONU_ACT_STATE_O5_2:
            *pOnuState = RT_NGP2_ONU_STATE_O5_2;
            break;
        case CA_NGP2_ONU_ACT_STATE_O6:
            *pOnuState = RT_NGP2_ONU_STATE_O6;
            break;
        case CA_NGP2_ONU_ACT_STATE_O7:
            *pOnuState = RT_NGP2_ONU_STATE_O7;
            break;
        case CA_NGP2_ONU_ACT_STATE_O8_1:
            *pOnuState = RT_NGP2_ONU_STATE_O8_1;
            break;
        case CA_NGP2_ONU_ACT_STATE_O8_2:
            *pOnuState = RT_NGP2_ONU_STATE_O8_2;
            break;
        case CA_NGP2_ONU_ACT_STATE_O9:
            *pOnuState = RT_NGP2_ONU_STATE_O9;
            break;
        default:
            ret = RT_ERR_OUT_OF_RANGE;
            break;
    }

    if(ret!=RT_ERR_OK){
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
	
    return RT_ERR_OK;
}   /* end of   dal_ca8279_gpon_onuState_get */


/* Function Name:
 *      dal_rt_ca8279_gpon_omci_tx
 * Description:
 *      Transmit OMCI message.
 * Input:
 *      msgLen   - length of the OMCI message to be transmitted
 *      pMsg     - pointer to the OMCI message data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32
dal_rt_ca8279_gpon_omci_tx(uint32 msgLen, uint8 *pMsg)
{

    ca_pkt_t halPkt;
    ca_pkt_block_t pkt_data;
    int32 ret;

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pMsg), RT_ERR_NULL_POINTER);

    if(_ca8279_gpon_omci_build_mic(pMsg)!= RT_ERR_OK)
    {
        CA8279_LOG_ERR("%s %d : OMCI build mic Error!!",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    
    memset(&halPkt,0,sizeof(halPkt));
    halPkt.block_count    = 1;
    halPkt.cos            = 7;
    halPkt.device_id      = 0;
    halPkt.src_port_id    = CA_PORT_ID(CA_PORT_TYPE_CPU, 0x10);
    halPkt.dst_port_id    = CA_PORT_ID(CA_PORT_TYPE_GPON, 7);
    halPkt.pkt_type       = CA_PKT_TYPE_OMCI;
    halPkt.pkt_len        = msgLen;
    halPkt.pkt_data       = (ca_pkt_block_t *)&pkt_data;
    halPkt.pkt_data->len  = msgLen;
    halPkt.pkt_data->data = pMsg;
    halPkt.flags          = CA_TX_BYPASS_FWD_ENGINE;

    if ((ret = __ca_tx(0, &halPkt)) != CA_E_OK)
    {
        CA8279_LOG_ERR("%s %d : TX OMCI Error!!",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if(ENABLED == gOmciMirrorEnable)
    {
        _ca8279_gpon_omci_mirror(msgLen, pMsg);     
    }

    return RT_ERR_OK;

}


/* Function Name:
 *      dal_rt_ca8279_gpon_omci_rx_callback_register
 * Description:
 *      Register OMCI RX callback function.
 * Input:
 *      omciRx   - pointer to the callback function for OMCI RX
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32
dal_rt_ca8279_gpon_omci_rx_callback_register(rt_gpon_omci_rx_callback omciRx)
{
    int32   ret;
    rtk_port_t pon;
    int ponPortMask;
    aal_ilpb_cfg_t ilpb_cfg;
    aal_l2_specpkt_ctrl_t ctrl;
    aal_l2_specpkt_ctrl_mask_t ctrl_mask;
    aal_l2_specpkt_detect_t detect;
    aal_l2_specpkt_detect_mask_t detect_mask;
    
    pon = HAL_GET_PON_PORT();
    ponPortMask = 1 << pon;

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* function body */

    if(DISABLED == gOmciTrapEnable)
    {
        if((ret = drv_nic_register_rxhook(ponPortMask,7,&_ca8279_receive_from_nic))!=RT_ERR_OK)
        {
            CA8279_LOG_ERR("drv_nic_register_rxhook Error!!\n");
            return ret;
        }

        gOmciTrapEnable = ENABLED;
    }

    gOmciRxCallback = omciRx;
    
    return RT_ERR_OK;
}   /* end of dal_rt_ca8279_gpon_omci_rx_callback_register */


/* Function Name:
 *      dal_rt_ca8279_gpon_tcont_set
 * Description:
 *      Set GPON tcont
 * Input:
 *      allocId   - allocation id
 * Output:
 *      pTcontId  - pointer to the logic TCONT id   
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 */
int32 
dal_rt_ca8279_gpon_tcont_set(uint32 *pTcontId, uint32 allocId)
{

    uint32 tcontId;
    
    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pTcontId), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((allocId >= SATURN_MAX_ALLOC_VAL), RT_ERR_INPUT);

   
    /*Check tcontID is exist*/
    if(RT_ERR_OK == _GetTcontIdByAllocId(allocId,&tcontId))
    {
        *pTcontId = tcontId;
    }
    else
    {
        if(_GetNonUsedTcontId(&tcontId) != RT_ERR_OK)
        {
            CA8279_LOG_ERR("%s %d Get non used TcontId fail\n",
                __FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        
        if (RT_ERR_OK == _ca8279_gpon_aal_tcont_set(LOGIC_TCONID_TO_CA8279_TCONID(tcontId), allocId))
        {

            _AssignNonUsedTcontId(allocId, &tcontId);
            *pTcontId = tcontId;
        }
        else
        {
            CA8279_LOG_ERR("%s %d set ca gpon tcont fail\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }
     CA8279_LOG_DBG("%s %d allocId:%u tcontId:%d\n",__FUNCTION__,__LINE__,allocId,*pTcontId);  


    return RT_ERR_OK;

}


/* Function Name:
 *      dal_rt_ca8279_gpon_tcont_get
 * Description:
 *      Get GPON tcont
 * Input:
 *      tcontId   - logic TCONT id
 * Output:
 *      pAllocId  - pointer to the allocation id 
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32 
dal_rt_ca8279_gpon_tcont_get(uint32 tcontId, uint32 *pAllocId)
{

    uint32 allocId;
    
    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pAllocId), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((tcontId >= SATURN_MAX_TCONT), RT_ERR_INPUT);
   
    /*Check tcontID is exist*/
    if(RT_ERR_OK == _GetAllocIdByTcontId(tcontId, &allocId))
    {
        *pAllocId = allocId;
    }
    else
    {
        //CA8279_LOG_DBG("%s %d : no matched TCONT id [%d]\n", __FUNCTION__, __LINE__, tcontId);  
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_rt_ca8279_gpon_tcont_del
 * Description:
 *      Delete GPON tcont
 * Input:
 *      tcontId   - tcont index
 *
 * Output:
 *      
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 */
int32 
dal_rt_ca8279_gpon_tcont_del(uint32 tcontId)
{


    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((tcontId >= SATURN_MAX_TCONT), RT_ERR_INPUT);

    if(RT_ERR_OK != _ca8279_gpon_aal_tcont_del(LOGIC_TCONID_TO_CA8279_TCONID(tcontId)))
    {
        CA8279_LOG_ERR("%s %d set del tcont fail\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if(RT_ERR_OK != _DelTcontId(tcontId))
    {
        CA8279_LOG_ERR("%s %d set del tcont fail\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;

}


/* Function Name:
 *      dal_rt_ca8279_gpon_usFlow_set
 * Description:
 *      Set GPON upstream flow
 * Input:
 *      usFlowId   - upstream flow id
 *      pUsFlow    - the pointer of upstream flow config
 *
 * Output:
 *      
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_NULL_POINTER
 * Note:
 */
int32 dal_rt_ca8279_gpon_usFlow_set(uint32 usFlowId, const rt_gpon_usFlow_t *pUsFlow)
{
    unsigned short gemIdx = 0;
    rt_gpon_usFlow_t usFlowCfg;
    
    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pUsFlow), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((usFlowId >= SATURN_MAX_GEM_FLOW), RT_ERR_INPUT);
    RT_PARAM_CHK((pUsFlow->tcontId >= SATURN_MAX_TCONT), RT_ERR_INPUT);
    RT_PARAM_CHK((pUsFlow->tcQueueId >= SATURN_MAX_TCONT_QUEUE), RT_ERR_INPUT);
    
    /* function body */

    if((TRUE == gGemUsFlowMap[usFlowId].isValid) && 
        (GEM_IDX_TO_TQID(gGemUsFlowMap[usFlowId].gemIdx) == pUsFlow->tcQueueId) &&
        (GEM_IDX_TO_TCID(gGemUsFlowMap[usFlowId].gemIdx) == LOGIC_TCONID_TO_CA8279_TCONID(pUsFlow->tcontId)) &&
        gGemUsFlowMap[usFlowId].aesEnable == pUsFlow->aesEn)
    {        
        CA8279_LOG_ERR("%s %d Update gemFlowId:%d \n",
            __FUNCTION__,__LINE__,usFlowId);

        return RT_ERR_OK;
    }

    memcpy(&usFlowCfg, pUsFlow, sizeof(rt_gpon_usFlow_t));
    usFlowCfg.tcontId = LOGIC_TCONID_TO_CA8279_TCONID(pUsFlow->tcontId);
    if (RT_ERR_OK != _ca8279_gpon_aal_usGem_set(
                                usFlowId, 
                                &usFlowCfg, 
                                &gemIdx))
    {
        CA8279_LOG_ERR("%s %d set us flow fail\n",
            __FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }


    //save the gemport index
    if(RT_ERR_OK != _SetGemFlowMap(GEM_FLOW_DIR_US, usFlowId, gemIdx, pUsFlow->aesEn))
    {
        CA8279_LOG_ERR("%s %d set gemflow map fail\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;            
    }

    return RT_ERR_OK;
}


 /* Function Name:
 *      dal_rt_ca8279_gpon_usFlow_get
 * Description:
 *      Get the upstream flow
 * Input:
 *      usFlowId  - upstream flow id
 * Output:
 *      pUsFlow   - the pointer of upstream flow config
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32 dal_rt_ca8279_gpon_usFlow_get(uint32 usFlowId, rt_gpon_usFlow_t *pUsFlow)
{
    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pUsFlow), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((usFlowId >= SATURN_MAX_GEM_FLOW), RT_ERR_INPUT);
    
    /* function body */
    if(TRUE != gGemUsFlowMap[usFlowId].isValid) 
    {        
        //CA8279_LOG_ERR("%s %d invalid us flow [%d]\n", __FUNCTION__, __LINE__, usFlowId);
        return RT_ERR_INPUT;
    }

    if (RT_ERR_OK != _ca8279_gpon_aal_usGem_get(
                                usFlowId, 
                                pUsFlow))
    {
        CA8279_LOG_ERR("%s %d get us flow fail\n",
            __FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
	
    pUsFlow->tcontId = CA8279_TCONID_TO_LOGIC_TCONID(GEM_IDX_TO_TCID(gGemUsFlowMap[usFlowId].gemIdx));
    pUsFlow->tcQueueId = GEM_IDX_TO_TQID(gGemUsFlowMap[usFlowId].gemIdx);


    return RT_ERR_OK;
}


/* Function Name:
 *      dal_rt_ca8279_gpon_usFlow_del
 * Description:
 *      Delete GPON upstream flow
 * Input:
 *      usFlowId   - upstream flow id
 * Output:
 *      
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 */
int32 dal_rt_ca8279_gpon_usFlow_del(uint32 usFlowId)
{
    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */    
    RT_PARAM_CHK((usFlowId >= SATURN_MAX_GEM_FLOW), RT_ERR_INPUT);

    CA8279_LOG_DBG("%s %d flowId:%d gemIdx:0x%x\n",
        __FUNCTION__,
        __LINE__,
        usFlowId,
        gGemUsFlowMap[usFlowId].gemIdx);

    /* function body */

    /*Check the US flow is exist*/
    if(FALSE == gGemUsFlowMap[usFlowId].isValid)
        return RT_ERR_OK;
    

    if (RT_ERR_OK != _ca8279_gpon_aal_usGem_del(usFlowId))
    {
        CA8279_LOG_ERR("%s %d del us flow fail\n",
            __FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }


    if(RT_ERR_OK != _DeleteGemFlowMap(usFlowId, GEM_FLOW_DIR_US))
    {
        CA8279_LOG_ERR("%s %d set gemflow map fail\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;            
    }
   
    return RT_ERR_OK;

}


/* Function Name:
 *      dal_rt_ca8279_gpon_usFlow_delAll
 * Description:
 *      Delete all GPON upstream flow
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_NOT_INIT     - The module is not initial
 * Note:
 */
int32 dal_rt_ca8279_gpon_usFlow_delAll(void)
{
    int32 ret = RT_ERR_OK;
    uint32 i;

    /* check Init status */
    RT_INIT_CHK(gpon_init);
    
    for(i=0;i<SATURN_MAX_GEM_FLOW;i++)
    {
        /*Delete DS gem flow*/
        if(TRUE == gGemUsFlowMap[i].isValid)
        {
            if(RT_ERR_OK != dal_rt_ca8279_gpon_usFlow_del(i))
            {
                ret = RT_ERR_FAILED;
            }
                
        }
    }

    return RT_ERR_OK;

}


/* Function Name:
 *      dal_rt_ca8279_gpon_dsFlow_set
 * Description:
 *      Set GPON downstream flow
 * Input:
 *      dsFlowId   - donwstream flow id
 *      pDsFlow    - the pointer of downstream flow config
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_NULL_POINTER
 * Note:
 */
int32 dal_rt_ca8279_gpon_dsFlow_set(uint32 dsFlowId, const rt_gpon_dsFlow_t *pDsFlow)
{
    unsigned short gemIdx = 0;
    
    CA8279_LOG_DBG(
        "%s %d flowId:%d gemportId:%d\n",
        __FUNCTION__,__LINE__,
        dsFlowId,
        pDsFlow->gemPortId);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pDsFlow), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((dsFlowId >= SATURN_MAX_GEM_FLOW), RT_ERR_INPUT);

    /* function body */
    if (RT_ERR_OK != _ca8279_gpon_aal_dsGem_set(
                                dsFlowId, 
                                pDsFlow, 
                                &gemIdx))
    {
        CA8279_LOG_ERR("%s %d set ds flow fail\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

        //save the gemport index
    if(RT_ERR_OK != _SetGemFlowMap(GEM_FLOW_DIR_DS, dsFlowId, gemIdx, FALSE))
    {
        CA8279_LOG_ERR("%s %d set gemflow map fail\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;            
    }
    
    return RT_ERR_OK;
}


/* Function Name:
 *      dal_rt_ca8279_gpon_dsFlow_get
 * Description:
 *      Get GPON downstream flow
 * Input:
 *      dsFlowId   - donwstream flow id
 * Output:
 *      pDsFlow    - the pointer of downstream flow config
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32 dal_rt_ca8279_gpon_dsFlow_get(uint32 dsFlowId, rt_gpon_dsFlow_t *pDsFlow)
{
    
    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pDsFlow), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((dsFlowId >= SATURN_MAX_GEM_FLOW), RT_ERR_INPUT);

    /* function body */
    if(TRUE != gGemDsFlowMap[dsFlowId].isValid)
    {
        //CA8279_LOG_ERR("%s %d invalid ds flow [%d]\n", __FUNCTION__, __LINE__, dsFlowId);
        return RT_ERR_INPUT;
    }
    if (RT_ERR_OK != _ca8279_gpon_aal_dsGem_get(
                                dsFlowId, 
                                pDsFlow))
    {
        CA8279_LOG_ERR("%s %d get ds flow fail\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    
    return RT_ERR_OK;
}


/* Function Name:
 *      dal_rt_ca8279_gpon_dsFlow_del
 * Description:
 *      Delete GPON downstream flow
 * Input:
 *      dsFlowId   - donwstream flow id
 * Output:
 *      
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 */
int32 dal_rt_ca8279_gpon_dsFlow_del(uint32 dsFlowId)
{

    CA8279_LOG_DBG(
        "%s %d flowId:%d gemIdx:0x%x\n",
        __FUNCTION__,__LINE__,
        dsFlowId,
        gGemDsFlowMap[dsFlowId].gemIdx);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */    
    RT_PARAM_CHK((dsFlowId >= SATURN_MAX_GEM_FLOW), RT_ERR_INPUT);

    
    /*Check the DS flow is exist*/
    if(FALSE == gGemDsFlowMap[dsFlowId].isValid)
        return RT_ERR_OK;


    if (RT_ERR_OK != _ca8279_gpon_aal_dsGem_del(dsFlowId))
    {
        CA8279_LOG_ERR("%s %d del ds flow fail\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if(RT_ERR_OK != _DeleteGemFlowMap(dsFlowId, GEM_FLOW_DIR_DS))
    {
        CA8279_LOG_ERR("%s %d set us gemflow map fail\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;            
    }
    
    return RT_ERR_OK;
}


/* Function Name:
 *      dal_rt_ca8279_gpon_dsFlow_delAll
 * Description:
 *      Delete all GPON downstream flow
 * Input:
 *
 * Output:
 *      
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_NOT_INIT     - The module is not initial
 * Note:
 */
int32 dal_rt_ca8279_gpon_dsFlow_delAll(void)
{
    int ret = RT_ERR_OK;
    unsigned int i;


    /* check Init status */
    RT_INIT_CHK(gpon_init);
    
    for(i=0;i<SATURN_MAX_GEM_FLOW;i++)
    {
        /*Delete DS gem flow*/
        if(TRUE == gGemDsFlowMap[i].isValid)
        {
            if(RT_ERR_OK != dal_rt_ca8279_gpon_dsFlow_del(i))
            {
                ret = RT_ERR_FAILED;
            }
        }
    }

    return ret;

}

/* Function Name:
 *      dal_rt_ca8279_gpon_ponQueue_set
 * Description:
 *      Set the pon queue
 * Input:
 *      tcontId    - tcont index
 *      tcQueueId  - tcont queue index
 *      pQueuecfg  - pointer to the queue configuration
 * Output:
 *      
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameters.        
 *      RT_ERR_NULL_POINTER
 * Note:
 */
int32 dal_rt_ca8279_gpon_ponQueue_set(
    uint32 tcontId, 
    uint32 tcQueueId,  
    const rt_gpon_queueCfg_t *pQueuecfg)
{
    int ret;
    rtk_ponmac_queue_t queue;
    rtk_ponmac_queueCfg_t queueCfg;


    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pQueuecfg), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((tcontId >= SATURN_MAX_TCONT), RT_ERR_INPUT);
    RT_PARAM_CHK((tcQueueId >= SATURN_MAX_TCONT_QUEUE), RT_ERR_INPUT);
	
	queue.queueId		= tcQueueId;
	queue.schedulerId	= tcontId;

	queueCfg.pir		= RT_GPON_PON_QUEUE_PIR_UNLIMITED == pQueuecfg->pir?0:RATE_8KBPS_TO_KBPS(pQueuecfg->pir);
	queueCfg.cir		= RT_GPON_PON_QUEUE_PIR_UNLIMITED == pQueuecfg->cir?0:RATE_8KBPS_TO_KBPS(pQueuecfg->cir);
	queueCfg.type		= RT_GPON_TCONT_QUEUE_SCHEDULER_SP == pQueuecfg->scheduleType?STRICT_PRIORITY:WFQ_WRR_PRIORITY;
	queueCfg.weight		= (RT_GPON_TCONT_QUEUE_SCHEDULER_SP == pQueuecfg->scheduleType)?0:pQueuecfg->weight;;

	if(RT_ERR_OK != dal_ca8279_ponmac_queue_add(&queue, &queueCfg))
	{
		CA8279_LOG_ERR("%s %d add pon queue fail, queueId=%d, schedulerId=%d\n"
			, __FUNCTION__, __LINE__, queue.queueId, queue.schedulerId);
		return RT_ERR_FAILED;
	}

     
    return RT_ERR_OK;
}


/* Function Name:
 *      dal_rt_ca8279_gpon_ponQueue_get
 * Description:
 *      Get the pon queue
 * Input:
 *      tcontId    - tcont index
 *      tcQueueId  - tcont queue index
 * Output:
 *      pQueuecfg  - pointer to the queue configuration
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameters.        
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32 dal_rt_ca8279_gpon_ponQueue_get(
    uint32 tcontId, 
    uint32 tcQueueId,  
    rt_gpon_queueCfg_t *pQueuecfg)
{
    int ret;
    rtk_ponmac_queue_t queue;
    rtk_ponmac_queueCfg_t queueCfg;

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pQueuecfg), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((tcontId >= SATURN_MAX_TCONT), RT_ERR_INPUT);
    RT_PARAM_CHK((tcQueueId >= SATURN_MAX_TCONT_QUEUE), RT_ERR_INPUT);

    queue.schedulerId = tcontId;
    queue.queueId = tcQueueId;

    memset(&queueCfg, 0x0, sizeof(rtk_ponmac_queueCfg_t));

    ret = dal_ca8279_ponmac_queue_get(&queue, &queueCfg);

    if(RT_ERR_OK != ret)
    {
        //CA8277B_LOG_ERR("%s %d dal_ca8277b_ponmac_queue_get fail \n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    
    pQueuecfg->cir = (0 == queueCfg.cir)?RT_GPON_PON_QUEUE_PIR_UNLIMITED:RATE_KBPS_TO_8KBPS(queueCfg.cir);
    pQueuecfg->pir = (0 == queueCfg.pir)?RT_GPON_PON_QUEUE_PIR_UNLIMITED:RATE_KBPS_TO_8KBPS(queueCfg.pir);
    pQueuecfg->scheduleType = (STRICT_PRIORITY == queueCfg.type)?RT_GPON_TCONT_QUEUE_SCHEDULER_SP:RT_GPON_TCONT_QUEUE_SCHEDULER_WRR;    
    pQueuecfg->weight = queueCfg.weight;  

    
    return RT_ERR_OK;
}


/* Function Name:
 *      dal_rt_ca8279_gpon_ponQueue_del
 * Description:
 *      Delete the pon queue
 * Input:
 *      tcontId     - tcont index
 *      tcQueueId   - tcont queue index
 *
 * Output:
 *      
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 */
int32 dal_rt_ca8279_gpon_ponQueue_del(    
    uint32 tcontId, 
    uint32 tcQueueId)
{
	rtk_ponmac_queue_t queue;

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((tcontId >= SATURN_MAX_TCONT), RT_ERR_INPUT);
    RT_PARAM_CHK((tcQueueId >= SATURN_MAX_TCONT_QUEUE), RT_ERR_INPUT);

    queue.schedulerId = tcontId;
    queue.queueId = tcQueueId;

    return dal_ca8279_ponmac_queue_del(&queue);

}



/* Function Name:
 *      dal_rt_ca8279_gpon_scheInfo_get
 * Description:
 *      Get scheduling information
 * Input:
 *      pScheInfo  - pointer to the scheduling information
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32 
dal_rt_ca8279_gpon_scheInfo_get(rt_gpon_schedule_info_t *pScheInfo)
{
    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pScheInfo), RT_ERR_NULL_POINTER);

    pScheInfo->max_flow         = SATURN_MAX_GEM_FLOW;
    pScheInfo->max_pon_queue    = SATURN_MAX_PON_QUEUE;
    pScheInfo->max_tcont        = SATURN_MAX_TCONT;
    pScheInfo->max_tcon_queue   = SATURN_MAX_TCONT_QUEUE;


    return RT_ERR_OK;
}   



/* Function Name:
 *      dal_rt_ca8279_gpon_flowCounter_get
 * Description:
 *      Get the flow counters.
 * Input:
 *      flowId   - logical flow ID
 *      flowCntType     - counter type
 * Output:
 *      pFlowCnt - pointer to the flow counters
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32 
dal_rt_ca8279_gpon_flowCounter_get(uint32 flowId, rt_gpon_flow_counter_type_t flowCntType, rt_gpon_flow_counter_t *pFlowCnt)
{
    uint32 internal_gem_idx;
    ca_status_t ret = CA_E_OK;
    aal_xgpon_dsfp_xgem_mib_config_t dsfp_xgem_mib;
    aal_xgpon_usbg_xgem_mib_config_t usbg_xgem_mib;
    
    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((flowId >= SATURN_MAX_GEM_FLOW), RT_ERR_INPUT);
    RT_PARAM_CHK((flowCntType >= RT_GPON_CNT_TYPE_FLOW_MAX), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pFlowCnt), RT_ERR_NULL_POINTER);

    internal_gem_idx = LOGIC_FLOW_TO_CA8279_FLOW(flowId);

    switch(flowCntType) 
    {
        case RT_GPON_CNT_TYPE_FLOW_DS_GEM:
        {
        	ret = aal_xgpon_dsfp_xgem_mib_get(SATURN_DEV_ID,internal_gem_idx&0xFF,&dsfp_xgem_mib);
            if(CA_E_OK == ret)
            {
                pFlowCnt->dsgem.gem_block = dsfp_xgem_mib.frame_cnt;
                pFlowCnt->dsgem.gem_byte = dsfp_xgem_mib.byte_cnt;
            }
            break;
        }
        case RT_GPON_CNT_TYPE_FLOW_US_GEM:
        {
            ret = aal_xgpon_usbg_xgem_mib_get(SATURN_DEV_ID,internal_gem_idx&0xFF,&usbg_xgem_mib);
            if(CA_E_OK == ret)
            {
                pFlowCnt->usgem.gem_block = usbg_xgem_mib.frame_cnt;
                pFlowCnt->usgem.gem_byte = usbg_xgem_mib.byte_cnt;
            }
            break;
        }
        case RT_GPON_CNT_TYPE_FLOW_DS_ETH:
        case RT_GPON_CNT_TYPE_FLOW_US_ETH:
        default:
            ret = CA_E_ERROR;
            CA8279_LOG_ERR("%s %d un-support flowCntType:%d\n",__FUNCTION__,__LINE__,flowCntType);
            break;
    }

    if(CA_E_OK == ret)
        return RT_ERR_OK;
    else
        return RT_ERR_FAILED;
    
}   /* end of dal_rt_ca8279_gpon_flowCounter_get */


/* Function Name:
 *      dal_rt_ca8279_gpon_pmCounter_get
 * Description:
 *      Get the pm counters.
 * Input:
 *      pmCntType     - counter type
 * Output:
 *      pPmCnt -  pointer to the global counters
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32 
dal_rt_ca8279_gpon_pmCounter_get(rt_gpon_pm_type_t pmCntType, rt_gpon_pm_counter_t *pPmCnt)
{
    ca_status_t ret = CA_E_OK;
    
    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((pmCntType >= RT_GPON_PM_TYPE_MAX), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pPmCnt), RT_ERR_NULL_POINTER);

    switch (pmCntType)
    {
        case RT_GPON_PM_TYPE_PHY_LOS:
			ret = _ca8279_gpon_pm_phyLos_get(pPmCnt);
        break;
        case RT_GPON_PM_TYPE_FEC:
			ret = _ca8279_gpon_pm_fec_get(pPmCnt);
        break;
        case RT_GPON_PM_TYPE_XGEM:
			ret = _ca8279_gpon_pm_xgem_get(pPmCnt);
        break;
        case RT_GPON_PM_TYPE_PLOAM_PM1:
			ret = _ca8279_gpon_pm_ploam1_get(pPmCnt);
        break;
        case RT_GPON_PM_TYPE_PLOAM_PM2:
			ret = _ca8279_gpon_pm_ploam2_get(pPmCnt);
        break;
        case RT_GPON_PM_TYPE_PLOAM_PM3:
			ret = _ca8279_gpon_pm_ploam3_get(pPmCnt);
        break;
        case RT_GPON_PM_TYPE_CHAN_TUNING_PM1:
			ret = _ca8279_gpon_pm_chan_tuning1_get(pPmCnt);
        break;
        case RT_GPON_PM_TYPE_CHAN_TUNING_PM2:
			ret = _ca8279_gpon_pm_chan_tuning2_get(pPmCnt);
        break;
        case RT_GPON_PM_TYPE_CHAN_TUNING_PM3:
			ret = _ca8279_gpon_pm_chan_tuning3_get(pPmCnt);
        break;
        case RT_GPON_PM_TYPE_OMCI:
			ret = _ca8279_gpon_pm_omci_get(pPmCnt);
        break;
        default:
        break;
    }

    if(CA_E_OK == ret)
        return RT_ERR_OK;
    else
        return RT_ERR_FAILED;
    
}   /* end of dal_rt_ca8279_gpon_flowCounter_get */


/* Function Name:
 *      dal_rt_ca8279_gpon_ponTag_get
 * Description:
 *      Get the PON Tag.
 * Input:
 *      None
 * Output:
 *      pPonTag       - pointer to the PON TAG
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32 
dal_rt_ca8279_gpon_ponTag_get(rt_gpon_ponTag_t *pPonTag)
{

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPonTag), RT_ERR_NULL_POINTER);



    if (CA_E_OK != ca_gpon_port_pon_tag_get(SATURN_DEV_ID, 8, &pPonTag->tagData[0]))
    {
        CA8279_LOG_ERR("%s %d get pontag fail\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;

}

/* Function Name:
 *      dal_rt_ca8279_gpon_msk_set
 * Description:
 *      Set the Master Session Key.
 * Input:
 *      pMsk - pointer to the Master Session Key
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32 
dal_rt_ca8279_gpon_msk_set(rt_gpon_msk_t *pMsk)
{
    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pMsk), RT_ERR_NULL_POINTER);

    if (CA_E_OK != ca_gpon_port_msk_update(SATURN_DEV_ID, &pMsk->mskData[0]))
    {
        CA8279_LOG_ERR("%s %d set msk fail\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
    
}

/* Function Name:
 *      dal_rt_ca8279_gpon_omci_mic_generate
 * Description:
 *      Generate the OMCI MIC value according to the inputs.
 * Input:
 *      dir     - the direction of the OMCI msg, 0x01 for Downstream and 0x02 for Upstream
 *      pMsg    - pointer to the OMCI message data
 *      msgLen  - length of the OMCI message, not include MIC
 * Output:
 *      mic     - pointer to the generated MIC
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
extern int32
dal_rt_ca8279_gpon_omci_mic_generate(uint32 dir, uint8 *pMsg, uint32 msgLen, uint32 *mic)
{
    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK(((dir < RT_GPON_OMCI_MIC_DIR_DS) | (dir > RT_GPON_OMCI_MIC_DIR_US)), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pMsg), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == mic), RT_ERR_NULL_POINTER);

    if (CA_E_OK != ca_generate_omci_mic(SATURN_DEV_ID, dir, pMsg, msgLen, 4, mic))

    {
        CA8279_LOG_ERR("%s %d generate OMCI MIC fail\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rt_ca8279_gpon_mcKey_set
 * Description:
 *      Set the Multicast Key.
 * Input:
 *      pMcKey - pointer to the Multicast Key
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32 
dal_rt_ca8279_gpon_mcKey_set(rt_gpon_mc_key_t *pMcKey)
{
    uint32 keyIdx = 0;

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK(((pMcKey->idx < RT_GPON_MC_KEY_IDX0) || (pMcKey->idx >= RT_GPON_MC_KEY_IDX_MAX)), RT_ERR_INPUT);
    RT_PARAM_CHK((pMcKey->action >= RT_GPON_MC_KEY_ACTION_MAX), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pMcKey), RT_ERR_NULL_POINTER);

    keyIdx = (pMcKey->idx == RT_GPON_MC_KEY_IDX0) ? CA_NGP2_KEY_IDX_0 : CA_NGP2_KEY_IDX_1;
    if (CA_E_OK != ca_ngp2_port_mc_encryption_key_set(SATURN_DEV_ID,
                                               SATURN_ANI_PORT_ID,
                                               keyIdx,
                                               pMcKey->action,
                                               16,
                                               pMcKey->key))
    {
        CA8279_LOG_ERR("%s %d set Multicast Key fail\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
    
}


/* Function Name:
 *      dal_rt_ca8279_gpon_attribute_set
 * Description:
 *      Set the gpon attribute.
 * Input:
 *      attributeType -  attribute type
 *      pAttributeValue - pointer to the attribute value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32 
dal_rt_ca8279_gpon_attribute_set(rt_gpon_attribute_type_t attributeType, rt_gpon_attribute_t *pAttributeValue)
{
    int32 ret = RT_ERR_OK;
    aal_xgpon_usbg_xgtc_ctrl_config_t xgtc_ctrl;
    aal_xgpon_usbg_xgtc_ctrl_config_msk_t mask;

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((attributeType >=  RT_GPON_ATTRIBUTE_TYPE_MAX), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pAttributeValue), RT_ERR_NULL_POINTER);

    switch(attributeType)
    {
        case RT_GPON_ATTRIBUTE_TYPE_DBRU_STATUS:
            memset(&mask, 0, sizeof(aal_xgpon_usbg_xgtc_ctrl_config_msk_t));
            mask.s.dbru_en = 1;
            if(CA_E_OK != aal_xgpon_usbg_xgtc_ctrl_set(SATURN_DEV_ID, mask, &xgtc_ctrl))
            {
                CA8279_LOG_ERR("%s %d : aal_xgpon_usbg_xgtc_ctrl_set Error!!",__FUNCTION__,__LINE__);
                ret = RT_ERR_FAILED;
            }	 			
            break;
        case RT_GPON_ATTRIBUTE_TYPE_GEM_BLOCK_LENGTH:/*Not support*/
            ret = RT_ERR_FAILED;
            break;
        case RT_GPON_ATTRIBUTE_TYPE_SD_THRESHOLD:
            if (CA_E_OK != ca_gpon_port_sd_threshold_set(SATURN_DEV_ID,
            											 SATURN_ANI_PORT_ID,
            											 pAttributeValue->sdThreshold))
            {
                CA8279_LOG_ERR("%s %d : ca_gpon_port_sd_threshold_set Error!!",__FUNCTION__,__LINE__);
                ret = RT_ERR_FAILED;
            }
            break;
        case RT_GPON_ATTRIBUTE_TYPE_SF_THRESHOLD:
            if (CA_E_OK != ca_gpon_port_sf_threshold_set(SATURN_DEV_ID,
            											 SATURN_ANI_PORT_ID,
            											 pAttributeValue->sfThreshold))
            {
                CA8279_LOG_ERR("%s %d : ca_gpon_port_sf_threshold_set Error!!",__FUNCTION__,__LINE__);
                ret = RT_ERR_FAILED;
            }
            break;
        default:
            ret = RT_ERR_INPUT;
            break;
    }

    return ret;
    
}

/* Function Name:
 *      dal_rt_ca8279_gpon_attribute_get
 * Description:
 *      Get the gpon attribute.
 * Input:
 *      attributeType -  attribute type
 * Output:
 *      pAttributeValue - pointer to the attribute value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32 
dal_rt_ca8279_gpon_attribute_get(rt_gpon_attribute_type_t attributeType, rt_gpon_attribute_t *pAttributeValue)
{
    int32 ret = RT_ERR_OK;
    aal_xgpon_usbg_xgtc_ctrl_config_t xgtc_ctrl;

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((attributeType >=  RT_GPON_ATTRIBUTE_TYPE_MAX), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pAttributeValue), RT_ERR_NULL_POINTER);

    switch(attributeType)
    {
        case RT_GPON_ATTRIBUTE_TYPE_DBRU_STATUS:
            if(CA_E_OK != aal_xgpon_usbg_xgtc_ctrl_get(SATURN_DEV_ID, &xgtc_ctrl))
            {
                CA8279_LOG_ERR("%s %d : aal_xgpon_usbg_xgtc_ctrl_get Error!!",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
            pAttributeValue->dbruStatus = xgtc_ctrl.dbru_en;
            break;
        case RT_GPON_ATTRIBUTE_TYPE_GEM_BLOCK_LENGTH:/*Not support*/
            ret = RT_ERR_FAILED;
            break;
        case RT_GPON_ATTRIBUTE_TYPE_SD_THRESHOLD:
            if(CA_E_OK != ca_gpon_port_sd_threshold_get(SATURN_DEV_ID,
            											 SATURN_ANI_PORT_ID,
            											 &pAttributeValue->sdThreshold))
            {
                CA8279_LOG_ERR("%s %d : ca_gpon_port_sd_threshold_get Error!!",__FUNCTION__,__LINE__);
                ret = RT_ERR_FAILED;
            }
            break;
        case RT_GPON_ATTRIBUTE_TYPE_SF_THRESHOLD:
            if (CA_E_OK != ca_gpon_port_sf_threshold_get(SATURN_DEV_ID,
            											 SATURN_ANI_PORT_ID,
            											 &pAttributeValue->sfThreshold))
            {
                CA8279_LOG_ERR("%s %d : ca_gpon_port_sd_threshold_get Error!!",__FUNCTION__,__LINE__);
                ret = RT_ERR_FAILED;
            }
            break;
        default:
            ret = RT_ERR_INPUT;
            break;
    }

    return ret;
    
}

/* Function Name:
 *      dal_rt_ca8279_gpon_omcc_get
 * Description:
 *      get omcc information.
 * Input:
 *      
 * Output:
 *      pOmcc                 -   omcc information
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found
 * Note:
 */
extern int32 
dal_rt_ca8279_gpon_omcc_get(rt_gpon_omcc_t *pOmcc)
{
    int32 ret = RT_ERR_OK;
    aal_xgpon_bwmp_alloc_id_cfg_t aal_bwmp_cfg;
    aal_xgpon_usbg_gem_config_t us_xgem_cfg;

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pOmcc), RT_ERR_NULL_POINTER);

    memset(&aal_bwmp_cfg, 0, sizeof(aal_bwmp_cfg));
    memset(&us_xgem_cfg, 0, sizeof(aal_xgpon_usbg_gem_config_t));

    if (CA_E_OK != aal_xgpon_bwmp_tcont_get(SATURN_DEV_ID, 0, 0, &aal_bwmp_cfg))
    {
        CA8279_LOG_ERR("%s:%d::Error bwmp get fail",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if(aal_bwmp_cfg.omci_en != 1 && aal_bwmp_cfg.ploam_en != 1)
        return RT_ERR_ENTRY_NOTFOUND;

    pOmcc->allocId = aal_bwmp_cfg.alloc_id;

    if (CA_E_OK != aal_xgpon_usbg_xgem_cfg_get(SATURN_DEV_ID, 0, &us_xgem_cfg))
    {
        CA8279_LOG_ERR("%s:%d::ERROR get usbg xgem fail",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if(us_xgem_cfg.port_en !=1)
        return RT_ERR_ENTRY_NOTFOUND;

    pOmcc->gemId = us_xgem_cfg.xgem_id;

    return ret;
}

/* Function Name:
 *      dal_rt_ca8277b_gpon_omci_mirror_set
 * Description:
 *      set gpon omci mirror function.
 * Input:
 *      enable          - enable mirrior function
 *      mirroringPort   - specified port for mirroring      
 * Return:
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 * Note:
 */

extern int32 
dal_rt_ca8279_gpon_omci_mirror_set(rt_enable_t enable, rt_port_t mirroringPort)
{
    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */

    if(!HAL_IS_GE_PORT(mirroringPort) && !HAL_IS_FE_PORT(mirroringPort))
        return RT_ERR_PORT_ID;
    
    if(enable)
    {
        gOmciMirrorEnable = ENABLED;
        gMirroringPort = mirroringPort;
    }
    else
    {
        gOmciMirrorEnable = DISABLED;
    }

    return RT_ERR_OK;   
}

