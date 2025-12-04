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
#include <rtk/ponmac.h>
#include <rtk/rt/rt_port.h>

#include <rtk/rt/rt_gpon.h>
#include <rtk/rt/rt_ponmisc.h>
#include <dal/ca8277b/dal_ca8277b.h>
#include <dal/ca8277b/dal_rt_ca8277b_gpon.h>
#include <dal/ca8277b/dal_ca8277b_l2.h>
#include <dal/ca8277b/dal_ca8277b_stat.h>
#include <dal/ca8277b/dal_ca8277b_ponmac.h>
#include <dal/ca8277b/dal_rt_ca8277b_ponmisc.h>


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
#include <aal_gpon.h>
#include <aal_puc.h>
#include <aal_pdc.h>
#include <aal_ploam.h>
#include <aal_l2_qos.h>
#include <aal_port.h>
#include <aal_l2_specpkt.h>
#include "ni-drv/ca_ni.h"
#include "ca-ne-rtk/ca_ext.h"

extern ca_uint32_t aal_init_pon;
extern ca_status_t ca_pon_init(ca_device_id_t device_id);

static ca_uint32_t gPonMode;
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
#define __XGPON_MAX_FIFOS  8
#define __XGPON_MAX_GRANTS 32


#define SATURN_MAX_ALLOC_VAL    (0xFFFF)


#define TCONT_ALLOC_ID_984_987_RESERVED (0xFFFF)

#define ETYPE_OFFSET    12
#define ETH_HEAD_LEN    16

static uint8 omci_eth[] = {0xff, 0xf1};
static uint8 vlan_eth[] = {0x81, 0x00};

static uint8 omciHeader[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x02,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
    0x88, 0xb5 };

static uint32_t crctab[256] = {
    0x00000000, 0x04c11db7, 0x09823b6e, 0x0d4326d9, 0x130476dc, 0x17c56b6b, 0x1a864db2, 0x1e475005,
    0x2608edb8, 0x22c9f00f, 0x2f8ad6d6, 0x2b4bcb61, 0x350c9b64, 0x31cd86d3, 0x3c8ea00a, 0x384fbdbd,
    0x4c11db70, 0x48d0c6c7, 0x4593e01e, 0x4152fda9, 0x5f15adac, 0x5bd4b01b, 0x569796c2, 0x52568b75,
    0x6a1936c8, 0x6ed82b7f, 0x639b0da6, 0x675a1011, 0x791d4014, 0x7ddc5da3, 0x709f7b7a, 0x745e66cd,
    0x9823b6e0, 0x9ce2ab57, 0x91a18d8e, 0x95609039, 0x8b27c03c, 0x8fe6dd8b, 0x82a5fb52, 0x8664e6e5,
    0xbe2b5b58, 0xbaea46ef, 0xb7a96036, 0xb3687d81, 0xad2f2d84, 0xa9ee3033, 0xa4ad16ea, 0xa06c0b5d,
    0xd4326d90, 0xd0f37027, 0xddb056fe, 0xd9714b49, 0xc7361b4c, 0xc3f706fb, 0xceb42022, 0xca753d95,
    0xf23a8028, 0xf6fb9d9f, 0xfbb8bb46, 0xff79a6f1, 0xe13ef6f4, 0xe5ffeb43, 0xe8bccd9a, 0xec7dd02d,
    0x34867077, 0x30476dc0, 0x3d044b19, 0x39c556ae, 0x278206ab, 0x23431b1c, 0x2e003dc5, 0x2ac12072,
    0x128e9dcf, 0x164f8078, 0x1b0ca6a1, 0x1fcdbb16, 0x018aeb13, 0x054bf6a4, 0x0808d07d, 0x0cc9cdca,
    0x7897ab07, 0x7c56b6b0, 0x71159069, 0x75d48dde, 0x6b93dddb, 0x6f52c06c, 0x6211e6b5, 0x66d0fb02,
    0x5e9f46bf, 0x5a5e5b08, 0x571d7dd1, 0x53dc6066, 0x4d9b3063, 0x495a2dd4, 0x44190b0d, 0x40d816ba,
    0xaca5c697, 0xa864db20, 0xa527fdf9, 0xa1e6e04e, 0xbfa1b04b, 0xbb60adfc, 0xb6238b25, 0xb2e29692,
    0x8aad2b2f, 0x8e6c3698, 0x832f1041, 0x87ee0df6, 0x99a95df3, 0x9d684044, 0x902b669d, 0x94ea7b2a,
    0xe0b41de7, 0xe4750050, 0xe9362689, 0xedf73b3e, 0xf3b06b3b, 0xf771768c, 0xfa325055, 0xfef34de2,
    0xc6bcf05f, 0xc27dede8, 0xcf3ecb31, 0xcbffd686, 0xd5b88683, 0xd1799b34, 0xdc3abded, 0xd8fba05a,
    0x690ce0ee, 0x6dcdfd59, 0x608edb80, 0x644fc637, 0x7a089632, 0x7ec98b85, 0x738aad5c, 0x774bb0eb,
    0x4f040d56, 0x4bc510e1, 0x46863638, 0x42472b8f, 0x5c007b8a, 0x58c1663d, 0x558240e4, 0x51435d53,
    0x251d3b9e, 0x21dc2629, 0x2c9f00f0, 0x285e1d47, 0x36194d42, 0x32d850f5, 0x3f9b762c, 0x3b5a6b9b,
    0x0315d626, 0x07d4cb91, 0x0a97ed48, 0x0e56f0ff, 0x1011a0fa, 0x14d0bd4d, 0x19939b94, 0x1d528623,
    0xf12f560e, 0xf5ee4bb9, 0xf8ad6d60, 0xfc6c70d7, 0xe22b20d2, 0xe6ea3d65, 0xeba91bbc, 0xef68060b,
    0xd727bbb6, 0xd3e6a601, 0xdea580d8, 0xda649d6f, 0xc423cd6a, 0xc0e2d0dd, 0xcda1f604, 0xc960ebb3,
    0xbd3e8d7e, 0xb9ff90c9, 0xb4bcb610, 0xb07daba7, 0xae3afba2, 0xaafbe615, 0xa7b8c0cc, 0xa379dd7b,
    0x9b3660c6, 0x9ff77d71, 0x92b45ba8, 0x9675461f, 0x8832161a, 0x8cf30bad, 0x81b02d74, 0x857130c3,
    0x5d8a9099, 0x594b8d2e, 0x5408abf7, 0x50c9b640, 0x4e8ee645, 0x4a4ffbf2, 0x470cdd2b, 0x43cdc09c,
    0x7b827d21, 0x7f436096, 0x7200464f, 0x76c15bf8, 0x68860bfd, 0x6c47164a, 0x61043093, 0x65c52d24,
    0x119b4be9, 0x155a565e, 0x18197087, 0x1cd86d30, 0x029f3d35, 0x065e2082, 0x0b1d065b, 0x0fdc1bec,
    0x3793a651, 0x3352bbe6, 0x3e119d3f, 0x3ad08088, 0x2497d08d, 0x2056cd3a, 0x2d15ebe3, 0x29d4f654,
    0xc5a92679, 0xc1683bce, 0xcc2b1d17, 0xc8ea00a0, 0xd6ad50a5, 0xd26c4d12, 0xdf2f6bcb, 0xdbee767c,
    0xe3a1cbc1, 0xe760d676, 0xea23f0af, 0xeee2ed18, 0xf0a5bd1d, 0xf464a0aa, 0xf9278673, 0xfde69bc4,
    0x89b8fd09, 0x8d79e0be, 0x803ac667, 0x84fbdbd0, 0x9abc8bd5, 0x9e7d9662, 0x933eb0bb, 0x97ffad0c,
    0xafb010b1, 0xab710d06, 0xa6322bdf, 0xa2f33668, 0xbcb4666d, 0xb8757bda, 0xb5365d03, 0xb1f740b4
};


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

// to maintain streamId to GPON config

typedef struct {
    unsigned char   isValid;
    unsigned char   tcontId;
    unsigned char   tcQueueId;
    unsigned short  gemIdx;
    unsigned short  gemPortId;
    unsigned char   aesEn;
} dal_ca8277b_usFlowMap_t;

typedef struct {
    unsigned char   isValid;
    unsigned short  gemIdx;
    unsigned short  gemPortId;
} dal_ca8277b_dsFlowMap_t;


typedef enum
{
    GEM_FLOW_DIR_US = 0x1,
    GEM_FLOW_DIR_DS = 0x2,
} GEM_FLOW_DIR;

typedef struct {
    unsigned char       tcontId;     //GEM flow ID
    unsigned char       tcQueueId;  //T-CONT queue ID
    unsigned short      VoqIdx; //VoQ index
#if defined(CONFIG_SDK_KERNEL_LINUX)
    struct list_head    list;
#endif
} voq_map_entry_t;


typedef struct {
    uint8   valid;
} dal_8277b_voq_tbl_t;

typedef struct {
    ca_uint32_t frame_cnt;
    ca_uint64_t byte_cnt;
} dal_mib_t;

typedef struct {
    uint16              phyTcontId; // Physical T-CONT ID
    uint32              allocId;   //T-CONT allodID
} dal_8277b_tcont_info_t;



/****************************************************************/
/* Macro Declaration                                            */
/****************************************************************/



#define RATE_8KBPS_TO_KBPS(_8kbps)  ((_8kbps)*8)
#define RATE_KBPS_TO_8KBPS(_kbps)  ((_kbps)/8)
#define RATE_KBPS_TO_VOQ_RATE_M(_kbps)  ((_kbps)/1000)
#define RATE_KBPS_TO_VOQ_RATE_K(_kbps)  ((_kbps)%1000)


/*CA 8277B GEM flow 0~7 for OMCC use */
#define LOGIC_FLOW_TO_CA8277B_FLOW(_gemFlow) ((_gemFlow)+8)


#define IS_OMCC_TCONT(_tcontId) (_tcontId==0)


#define CA8277B_LOG_ERR(fmt, arg...)  printk(KERN_ERR "[ca8277b]:\t" fmt "\n",## arg)
#define CA8277B_LOG_DBG(fmt, arg...)  printk(KERN_INFO "[ca8277b]:\t" fmt "\n",## arg) 


#define CA8277B_GPON_MAX_TCONT            (15)
#define CA8277B_GPON_MAX_TCONT_QUEUE      (8)
#define CA8277B_GPON_MAX_PON_QUEUE        (120)
#define CA8277B_GPON_MAX_GEM_FLOW         (56)


#define CA8277B_XGPON_MAX_TCONT          (31)
#define CA8277B_XGPON_MAX_TCONT_QUEUE    (8)
#define CA8277B_XGPON_MAX_PON_QUEUE      (248)
#define CA8277B_XGPON_MAX_GEM_FLOW       (248)


#define CA8277B_MAX_TCONT ((gPonMode == ONU_PON_MAC_GPON) ? CA8277B_GPON_MAX_TCONT:CA8277B_XGPON_MAX_TCONT)
#define CA8277B_MAX_TCONT_QUEUE ((gPonMode == ONU_PON_MAC_GPON) ? CA8277B_GPON_MAX_TCONT_QUEUE:CA8277B_XGPON_MAX_TCONT_QUEUE)
#define CA8277B_MAX_PON_QUEUE ((gPonMode == ONU_PON_MAC_GPON) ? CA8277B_GPON_MAX_PON_QUEUE:CA8277B_XGPON_MAX_PON_QUEUE)
#define CA8277B_MAX_GEM_FLOW ((gPonMode == ONU_PON_MAC_GPON) ? CA8277B_GPON_MAX_GEM_FLOW:CA8277B_XGPON_MAX_GEM_FLOW)


//Tcont ID 0 for OMCC channel use
#define LOGIC_TCONID_TO_CA8277B_TCONID(_logicTcontId) (g8277bTcontTbl[_logicTcontId].phyTcontId)



#define TCONID_TO_LDPID(_tcontId, _isUseDq) (_isUseDq?(_tcontId):((_tcontId)+0x20))

/*For GPON mode, the GEM port table for data use from 8~63*/
#define CA8277B_GPON_GEM_IDX_MIN    (8)
#define CA8277B_GPON_GEM_IDX_MAX    (63)

/****************************************************************/
/* Data Declaration                                             */
/****************************************************************/





static dal_ca8277b_usFlowMap_t *g8277bGemUsFlowMap;
dal_ca8277b_dsFlowMap_t *g8277bGemDsFlowMap;
static dal_8277b_tcont_info_t *g8277bTcontTbl;

extern rt_gpon_flow_counter_t g8277bUsGemCnts[AAL_XGPON_GEM_PORT_NUM];
extern rt_gpon_flow_counter_t g8277bDsGemCnts[AAL_XGPON_GEM_PORT_NUM];
extern dal_mib_t g8277bPonUsCnt;
extern dal_mib_t g8277bPonDsCnt;




/*Defien local function*/
int
_dal_ca8277b_gpon_is_inited(void)
{
   return gpon_init;
}

static int32 
_dal_ca8277b_gpon_db_init(void)
{
    unsigned short gemFlow,tcontId;


    g8277bGemUsFlowMap = kmalloc(sizeof(dal_ca8277b_usFlowMap_t) * CA8277B_MAX_GEM_FLOW, GFP_KERNEL);
    if (!g8277bGemUsFlowMap)
    {        
        CA8277B_LOG_ERR("%s %d malloc usflowMap db error",
            __FUNCTION__, __LINE__);
        RT_ERR(ret, (MOD_DAL|MOD_GPON), "");
        return RT_ERR_FAILED;


    }

    for(gemFlow = 0;gemFlow<CA8277B_MAX_GEM_FLOW;gemFlow++)
    {
        g8277bGemUsFlowMap[gemFlow].isValid     = FALSE;
        g8277bGemUsFlowMap[gemFlow].tcontId     = 0x0;
        g8277bGemUsFlowMap[gemFlow].tcQueueId   = 0x0;
        g8277bGemUsFlowMap[gemFlow].gemIdx      = 0x0;
        g8277bGemUsFlowMap[gemFlow].aesEn       = 0x0;
    }


    g8277bGemDsFlowMap = kmalloc(sizeof(dal_ca8277b_dsFlowMap_t) * CA8277B_MAX_GEM_FLOW, GFP_KERNEL);
    if (!g8277bGemDsFlowMap)
    {        
        CA8277B_LOG_ERR("%s %d malloc dsflowMap db error",
            __FUNCTION__, __LINE__);
        RT_ERR(ret, (MOD_DAL|MOD_GPON), "");
        return RT_ERR_FAILED;


    }

    for(gemFlow = 0;gemFlow<CA8277B_MAX_GEM_FLOW;gemFlow++)
    {
        g8277bGemDsFlowMap[gemFlow].isValid     = FALSE;
        g8277bGemDsFlowMap[gemFlow].gemIdx      = 0x0;
    }


    g8277bTcontTbl = kmalloc(sizeof(dal_8277b_tcont_info_t) * CA8277B_MAX_TCONT, GFP_KERNEL);
    if (!g8277bTcontTbl)
    {        
        CA8277B_LOG_ERR("%s %d malloc tcont db error",
            __FUNCTION__, __LINE__);
        RT_ERR(ret, (MOD_DAL|MOD_GPON), "");
        return RT_ERR_FAILED;


    }

    for(tcontId=0;tcontId<CA8277B_MAX_TCONT;tcontId++)
    {
        g8277bTcontTbl[tcontId].allocId = TCONT_ALLOC_ID_984_987_RESERVED;
        dal_rt_ca8277b_ponmisc_tcont_llid_logic2phy_get(tcontId, &g8277bTcontTbl[tcontId].phyTcontId);
        CA8277B_LOG_DBG("%s %d tcontId[%d].phyTcontId=%d",
            __FUNCTION__, __LINE__,tcontId, g8277bTcontTbl[tcontId].phyTcontId);

    }

    return RT_ERR_OK;
}


static int 
_AssignNonUsedTcontId(unsigned int allocId, unsigned int *pTcontId)
{
    unsigned int i;
    for(i=0;i<CA8277B_MAX_TCONT;i++)
    {
        if(g8277bTcontTbl[i].allocId == allocId)
        {
            *pTcontId = i;
            return RT_ERR_OK;
        }
    }

    for(i=0;i<CA8277B_MAX_TCONT;i++)
    {
        if(g8277bTcontTbl[i].allocId == TCONT_ALLOC_ID_984_987_RESERVED)
        {
            g8277bTcontTbl[i].allocId = allocId;
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
       
    for(i=0;i<CA8277B_MAX_TCONT;i++)
    {
        if(g8277bTcontTbl[i].allocId == allocId)
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
    if(TCONT_ALLOC_ID_984_987_RESERVED != g8277bTcontTbl[tcontId].allocId)
    {
        *pAllocId = g8277bTcontTbl[tcontId].allocId;
        return RT_ERR_OK;
    }    
    
    return RT_ERR_FAILED;
}

static int 
_GetNonUsedTcontId(unsigned int *pTcontId)
{
    unsigned int i;
   
    for(i=0;i<CA8277B_MAX_TCONT;i++)
    {
        if(TCONT_ALLOC_ID_984_987_RESERVED == g8277bTcontTbl[i].allocId)
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


    RT_PARAM_CHK((tcontId >= CA8277B_MAX_TCONT), RT_ERR_INPUT);
    
    g8277bTcontTbl[tcontId].allocId = TCONT_ALLOC_ID_984_987_RESERVED;
        
    return RT_ERR_OK;
}

static unsigned char 
__tcont_tbl_is_empty (unsigned short ignoreGemFlowId)
{
    unsigned short gemFlowId;
    unsigned int tcontId = g8277bGemUsFlowMap[ignoreGemFlowId].tcontId;
    

    for (gemFlowId = 0; gemFlowId < CA8277B_MAX_GEM_FLOW; gemFlowId++) 
    {
        if(ignoreGemFlowId == gemFlowId)
            continue;
        
        if (g8277bGemUsFlowMap[gemFlowId].isValid) 
        {
            if(g8277bGemUsFlowMap[gemFlowId].tcontId == tcontId)
                return FALSE;
        }
    }

    return (TRUE);
}


static unsigned char 
__gemport_tbl_is_empty (unsigned short ignoreGemFlowId)
{
    unsigned short gemFlowId;
    unsigned int gemPortId = g8277bGemUsFlowMap[ignoreGemFlowId].gemPortId;
    

    for (gemFlowId = 0; gemFlowId < CA8277B_MAX_GEM_FLOW; gemFlowId++) 
    {
        if(ignoreGemFlowId == gemFlowId)
            continue;
        
        if (g8277bGemUsFlowMap[gemFlowId].isValid) 
        {
            if(g8277bGemUsFlowMap[gemFlowId].gemPortId == gemPortId)
                return FALSE;
        }
    }

    return (TRUE);
}


static int _ca8277b_GetPhy2LogicTcontMap(unsigned int phyTcontId,unsigned int *pLogicTcontId)
{
    unsigned char  tcontId;

    if(phyTcontId>=CA8277B_MAX_TCONT)
        return RT_ERR_FAILED;

    for(tcontId = 0;tcontId<CA8277B_MAX_TCONT;tcontId++)
    {
        if(g8277bTcontTbl[tcontId].phyTcontId == phyTcontId)
        {
            *pLogicTcontId = tcontId;
            return RT_ERR_OK;
            
        }
    }
    return RT_ERR_ENTRY_NOTFOUND;
}

static unsigned int _ca8277b_SetGemFlowMap(
    GEM_FLOW_DIR dir,
    unsigned short gemFlowId, 
    rt_gpon_usFlow_t *pUsFlow,
    unsigned short gemIdx,
    ca_uint16_t    gemPortId)


{    
    /* parameter check */
    RT_PARAM_CHK((gemFlowId >= CA8277B_MAX_GEM_FLOW), RT_ERR_INPUT);

    if(GEM_FLOW_DIR_DS == dir)
    {
        g8277bGemDsFlowMap[gemFlowId].isValid       = TRUE; 
        g8277bGemDsFlowMap[gemFlowId].gemIdx        = gemIdx;
        g8277bGemDsFlowMap[gemFlowId].gemPortId     = gemPortId;
    }

    else
    {      
        g8277bGemUsFlowMap[gemFlowId].isValid       = TRUE;
        g8277bGemUsFlowMap[gemFlowId].tcontId       = pUsFlow->tcontId;
        g8277bGemUsFlowMap[gemFlowId].tcQueueId     = pUsFlow->tcQueueId;
        g8277bGemUsFlowMap[gemFlowId].aesEn         = pUsFlow->aesEn;
        g8277bGemUsFlowMap[gemFlowId].gemIdx        = gemIdx;
        g8277bGemUsFlowMap[gemFlowId].gemPortId     = gemPortId;

    }
    return RT_ERR_OK;

}


static unsigned int 
_ca8277b_DeleteGemFlowMap(unsigned short gemFlowId,GEM_FLOW_DIR dir)
{   
    /* parameter check */
    RT_PARAM_CHK((gemFlowId >= CA8277B_MAX_GEM_FLOW), RT_ERR_INPUT);

    if(GEM_FLOW_DIR_DS == dir)
    {
        g8277bGemDsFlowMap[gemFlowId].isValid   = FALSE;
        g8277bGemDsFlowMap[gemFlowId].gemIdx    = 0x0;
        g8277bGemDsFlowMap[gemFlowId].gemPortId = 0x0;
    } 
    else
    {
        g8277bGemUsFlowMap[gemFlowId].isValid       = FALSE;
        g8277bGemUsFlowMap[gemFlowId].tcontId       = 0x0;
        g8277bGemUsFlowMap[gemFlowId].tcQueueId     = 0x0;
        g8277bGemUsFlowMap[gemFlowId].aesEn         = 0x0;
        g8277bGemUsFlowMap[gemFlowId].gemIdx        = 0x0;
        g8277bGemUsFlowMap[gemFlowId].gemPortId     = 0x0;

    }  
    return RT_ERR_OK;
;

}

/*Direct call AAL API*/
static int 
_ca8277b_gpon_aal_tcont_set(unsigned int tcontId, unsigned int allocId)
{
    aal_xgpon_bwmp_alloc_id_cfg_msk_t mask;
    aal_xgpon_bwmp_alloc_id_cfg_t alloc_id_cfg;
    aal_gpon_tcont_cfg_msk_t gpon_mask;
    aal_gpon_tcont_cfg_t gpon_alloc_id_cfg;

    if(IS_OMCC_TCONT(tcontId)){
        CA8277B_LOG_ERR("%s:%d::ERROR: TCONT %d only used for OMCC!",__FUNCTION__,__LINE__,tcontId);
        return RT_ERR_FAILED;
    }

    if (allocId > 0xFFFF ){
        CA8277B_LOG_ERR("%s:%d:: ERROR alloc_id is :%d",__FUNCTION__,__LINE__,allocId);
        return RT_ERR_FAILED;
    }

    if(CA_PON_MODE == ONU_PON_MAC_GPON)
    {
        gpon_mask.u32 = 0;
        gpon_mask.s.tcont_index =1;
        gpon_mask.s.omci_en =1;
        gpon_mask.s.ploam_en =1;
        gpon_alloc_id_cfg.tcont_index = tcontId;
        gpon_alloc_id_cfg.omci_en = 1;
        gpon_alloc_id_cfg.ploam_en = 1;

        if (CA_E_OK !=aal_gpon_tcont_set(SATURN_DEV_ID, TRUE, allocId, gpon_mask, &gpon_alloc_id_cfg))
        {
            CA8277B_LOG_ERR("%s:%d::create tcont_id:%d fail",__FUNCTION__,__LINE__,tcontId);
            return RT_ERR_FAILED;
        }
    }
    else
    {
        mask.u32 = 0;
        mask.s.alloc_id =1;
        mask.s.omci_en =1;
        mask.s.ploam_en =1;
        alloc_id_cfg.alloc_id = allocId;
        alloc_id_cfg.omci_en = 1;
        alloc_id_cfg.ploam_en = 1;
    
        if (CA_E_OK !=aal_xgpon_bwmp_tcont_set(SATURN_DEV_ID, AAL_XGPON_WAVE_CHANNEL_DEFAULT, tcontId, mask, &alloc_id_cfg))
        {
            CA8277B_LOG_ERR("%s:%d::create tcont_id:%d fail",__FUNCTION__,__LINE__,tcontId);
            return RT_ERR_FAILED;
        }
    }
    CA8277B_LOG_DBG("Add TCONT %u Alloc-ID %u",tcontId,  allocId);

    return RT_ERR_OK;

}


static int 
_ca8277b_gpon_aal_tcont_del(unsigned int tcontId)
{
    aal_xgpon_bwmp_alloc_id_cfg_msk_t mask;
    aal_xgpon_bwmp_alloc_id_cfg_t alloc_id_cfg;
    aal_gpon_tcont_cfg_msk_t gpon_mask;
    aal_gpon_tcont_cfg_t gpon_alloc_id_cfg;
    unsigned int allocId;
    unsigned int logic_tcontId;

    if(IS_OMCC_TCONT(tcontId)){
        CA8277B_LOG_ERR("%s:%d::ERROR: TCONT %d only used for OMCC!",__FUNCTION__,__LINE__,tcontId);
        return RT_ERR_FAILED;
    }


    ca_mdelay(1); /* wait for USBG burst finish */

    if(CA_PON_MODE == ONU_PON_MAC_GPON)
    {
        gpon_mask.u32 = 0;
        gpon_mask.s.tcont_index =1;
        gpon_mask.s.omci_en =1;
        gpon_alloc_id_cfg.tcont_index = 0;
        gpon_alloc_id_cfg.omci_en = 0;

        if(RT_ERR_OK != _ca8277b_GetPhy2LogicTcontMap(tcontId, &logic_tcontId))
        {
            CA8277B_LOG_ERR("%s %d get phy:%u to logic T-cont fail\n",  
                __FUNCTION__, __LINE__, tcontId);
        
            return RT_ERR_FAILED;
        
        }

        _GetAllocIdByTcontId(logic_tcontId, &allocId);
            
        if (CA_E_OK != aal_gpon_tcont_set(SATURN_DEV_ID, FALSE, allocId, gpon_mask, &gpon_alloc_id_cfg))
        {
            CA8277B_LOG_ERR("%s:%d::delete tcont_id:%d fail",__FUNCTION__,__LINE__,tcontId);
            return RT_ERR_FAILED;
        }
    }
    else
    {
#if 0
        if (CA_E_OK != aal_xgpon_tcont_flush(SATURN_DEV_ID, AAL_XGPON_WAVE_CHANNEL_DEFAULT, tcontId))
        {
            CA8277B_LOG_ERR("%s:%d::ERROR delete tcont_id:%d fail",__FUNCTION__,__LINE__,tcontId);
            return RT_ERR_FAILED;
        }
#endif

        mask.u32 = 0;
        mask.s.alloc_id =1;
        mask.s.omci_en =1;
        alloc_id_cfg.alloc_id = 0x3aad;
        alloc_id_cfg.omci_en = 0;

        if (CA_E_OK != aal_xgpon_bwmp_tcont_set(SATURN_DEV_ID, AAL_XGPON_WAVE_CHANNEL_DEFAULT, tcontId, mask, &alloc_id_cfg))
        {
            CA8277B_LOG_ERR("%s:%d::delete tcont_id:%d fail",__FUNCTION__,__LINE__,tcontId);
            return RT_ERR_FAILED;
        }
    }

    return RT_ERR_OK;

}


static int 
_ca8277b_gpon_aal_usGem_set(unsigned int usFlowId, const rt_gpon_usFlow_t *pUsFlow, unsigned short *pGemIdx)
{
    aal_xgpon_usbg_gem_config_msk_t us_mask;
    aal_xgpon_usbg_gem_config_t us_xgem_cfg;
    unsigned int internal_gem_idx;
    int ret;

    /* parameter check */
    RT_PARAM_CHK((NULL == pUsFlow), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((usFlowId >= CA8277B_MAX_GEM_FLOW), RT_ERR_INPUT);

    if(IS_OMCC_TCONT(pUsFlow->tcontId)){
        CA8277B_LOG_ERR("%s:%d::ERROR: TCONT %d only used for OMCC!",__FUNCTION__,__LINE__,pUsFlow->tcontId);
        return RT_ERR_FAILED;
    }

    internal_gem_idx = LOGIC_FLOW_TO_CA8277B_FLOW(usFlowId);
     

    if(CA_PON_MODE == ONU_PON_MAC_GPON)
    {
         unsigned short  gem_id;
         int i;
         unsigned char isFound = FALSE;

        /*Check the GEM port id already exist*/
        for(i = 0; i<CA8277B_MAX_GEM_FLOW; i++)
        {
            if(TRUE == g8277bGemUsFlowMap[i].isValid &&
                g8277bGemUsFlowMap[i].gemPortId == pUsFlow->gemPortId)
            {
                internal_gem_idx = g8277bGemUsFlowMap[i].gemIdx;
                isFound = TRUE;
                CA8277B_LOG_DBG("%s:%d internal_gem_idx=%d, gemPortId=%d exist", 
                    __FUNCTION__, __LINE__, internal_gem_idx, pUsFlow->gemPortId);
                break;
            }
        }

          
        /*search empty internal_gem_idx*/

        if(!isFound)
        {
            for(internal_gem_idx = CA8277B_GPON_GEM_IDX_MIN; internal_gem_idx<=CA8277B_GPON_GEM_IDX_MAX; internal_gem_idx++)
            {
                if (CA_E_OK != aal_gpon_us_gem_port_get(SATURN_DEV_ID, internal_gem_idx, &gem_id))
                {
                    CA8277B_LOG_ERR("%s:%d::ERROR get us gem fail",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }
                
                if(gem_id == 0x0)
                {
                    if (CA_E_OK != aal_gpon_us_gem_port_set(SATURN_DEV_ID, internal_gem_idx, pUsFlow->gemPortId))
                    {
                        CA8277B_LOG_ERR("%s:%d::ERROR set us gem fail",__FUNCTION__,__LINE__);
                        return RT_ERR_FAILED;
                    }
                    CA8277B_LOG_DBG("%s:%d internal_gem_idx=%d, gemPortId=%d new", 
                        __FUNCTION__, __LINE__, internal_gem_idx, pUsFlow->gemPortId);
                    break;
                }  
            }
        }
    }
    else
    {
        /*add us gem table*/
        us_mask.u32 = ~0U;
        
        us_xgem_cfg.port_en     = 1;
        us_xgem_cfg.xgem_id     = pUsFlow->gemPortId;
        us_xgem_cfg.aes_mode    = pUsFlow->aesEn;
        us_xgem_cfg.mc_flag     = 0;
    
        if (CA_E_OK != aal_xgpon_usbg_xgem_cfg_set(SATURN_DEV_ID, internal_gem_idx, us_mask, &us_xgem_cfg))
        {
            CA8277B_LOG_ERR("%s:%d::ERROR set usbg xgem fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }

    /*Enable Tcont*/
    if (CA_E_OK != aal_puc_pvtbl_enable_set(SATURN_DEV_ID, pUsFlow->tcontId, TRUE))
    {
        CA8277B_LOG_ERR("%s:%d::ERROR set pvtbl fail",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    
    *pGemIdx = internal_gem_idx;

    return RT_ERR_OK;
}


static int 
_ca8277b_gpon_aal_usGem_get(unsigned int usFlowId, rt_gpon_usFlow_t *pUsFlow)
{
    aal_xgpon_usbg_gem_config_t us_xgem_cfg;
    unsigned int internal_gem_idx;

    /* parameter check */
    RT_PARAM_CHK((NULL == pUsFlow), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((usFlowId >= CA8277B_MAX_GEM_FLOW), RT_ERR_INPUT);

    internal_gem_idx = g8277bGemUsFlowMap[usFlowId].gemIdx;

    if(CA_PON_MODE == ONU_PON_MAC_GPON)
    {
        unsigned short  gem_id;

        if (CA_E_OK != aal_gpon_us_gem_port_get(SATURN_DEV_ID, internal_gem_idx, &gem_id))
        {
            CA8277B_LOG_ERR("%s:%d::ERROR get us gem fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        pUsFlow->gemPortId = gem_id;
        pUsFlow->aesEn = 0; //GPON not support upstream AES 
    }
    else
    {
        if (CA_E_OK != aal_xgpon_usbg_xgem_cfg_get(SATURN_DEV_ID, internal_gem_idx, &us_xgem_cfg))
        {
            CA8277B_LOG_ERR("%s:%d::ERROR get usbg xgem fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        pUsFlow->gemPortId = us_xgem_cfg.xgem_id;
        pUsFlow->aesEn = us_xgem_cfg.aes_mode;
    }

    return RT_ERR_OK;
}


static int 
_ca8277b_gpon_aal_dsGem_set(unsigned int dsFlowId, const rt_gpon_dsFlow_t *pDsFlow, unsigned short *pGemIdx)
{
    aal_xgpon_dsfp_xgem_cfg_msk_t ds_mask;
    aal_xgpon_dsfp_xgem_cfg_t   ds_xgem_cfg;
    unsigned int internal_gem_idx;

    /* parameter check */
    RT_PARAM_CHK((NULL == pDsFlow), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((dsFlowId >= CA8277B_MAX_GEM_FLOW), RT_ERR_INPUT);


    internal_gem_idx = LOGIC_FLOW_TO_CA8277B_FLOW(dsFlowId);

    if(CA_PON_MODE == ONU_PON_MAC_GPON)
    {
        aal_gpon_gem_port_cfg_msk_t  ds_gem_cfg_mask;
        aal_gpon_gem_port_cfg_t ds_gem_cfg;
        unsigned char aes_en; //FIXME: how to know DS GEM is AES enabled?

        ds_gem_cfg_mask.u32 = 0xffffffff;
        memset(&ds_gem_cfg, 0, sizeof(ds_gem_cfg));
        ds_gem_cfg.gem_index = internal_gem_idx;
        ds_gem_cfg.vld = 1;
        if (CA_E_OK != aal_gpon_ds_gem_port_set(SATURN_DEV_ID, aes_en, pDsFlow->gemPortId, ds_gem_cfg_mask, &ds_gem_cfg))
        {
            CA8277B_LOG_ERR("%s:%d::set ds gem fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }
    else
    {
        ds_mask.u32 = 0xffffffff;
    
        ds_xgem_cfg.gem_vld = 1;
        ds_xgem_cfg.gem_id = pDsFlow->gemPortId;
        ds_xgem_cfg.gem_mc = pDsFlow->isMcGem;
        if (CA_E_OK != aal_xgpon_dsfp_xgem_set(SATURN_DEV_ID, internal_gem_idx, ds_mask, &ds_xgem_cfg))
        {
            CA8277B_LOG_ERR("%s:%d::set ds gem fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }

#if 0 //Not setting PDC
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
            CA8277B_LOG_ERR("%s:%d::set pdc map fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

    }

#endif

    *pGemIdx = internal_gem_idx;

    return RT_ERR_OK;

}


static int 
_ca8277b_gpon_aal_dsGem_get(unsigned int dsFlowId, rt_gpon_dsFlow_t *pDsFlow)
{
    aal_xgpon_dsfp_xgem_cfg_t   ds_xgem_cfg;
    unsigned int internal_gem_idx;

    /* parameter check */
    RT_PARAM_CHK((NULL == pDsFlow), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((dsFlowId >= CA8277B_MAX_GEM_FLOW), RT_ERR_INPUT);

    internal_gem_idx = LOGIC_FLOW_TO_CA8277B_FLOW(dsFlowId);
    if(CA_PON_MODE == ONU_PON_MAC_GPON)
    {
        aal_gpon_gem_port_cfg_t ds_gem_cfg;
        unsigned short gem_id;
        unsigned char aes_en; //FIXME: how to know DS GEM is AES enabled?

        gem_id = g8277bGemDsFlowMap[dsFlowId].gemPortId;
        if (CA_E_OK != aal_gpon_ds_gem_port_get(SATURN_DEV_ID, aes_en, gem_id, &ds_gem_cfg))
        {
            CA8277B_LOG_ERR("%s:%d::ERROR set usbg xgem fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        pDsFlow->gemPortId = gem_id;
        pDsFlow->aesEn = ds_gem_cfg.aes;
    }
    else
    {
        if (CA_E_OK != aal_xgpon_dsfp_xgem_get(SATURN_DEV_ID, internal_gem_idx, &ds_xgem_cfg))
        {
            CA8277B_LOG_ERR("%s:%d::get ds gem fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        pDsFlow->gemPortId = ds_xgem_cfg.gem_id;
        pDsFlow->aesEn = 0; //downstream encryption is decided by OLT in runtime, so no such information
    }

    return RT_ERR_OK;
}


static int 
_ca8277b_gpon_aal_usGem_del(unsigned int usFlowId)
{
    aal_xgpon_usbg_gem_config_msk_t us_mask;
    aal_xgpon_usbg_gem_config_t us_xgem_cfg;
    unsigned int internal_gem_idx;

    RT_PARAM_CHK((usFlowId >= CA8277B_MAX_GEM_FLOW), RT_ERR_INPUT);

    internal_gem_idx = g8277bGemUsFlowMap[usFlowId].gemIdx;

    
    if(CA_PON_MODE == ONU_PON_MAC_GPON)
    {
        unsigned short gem_id = g8277bGemUsFlowMap[usFlowId].gemPortId;

        if (__gemport_tbl_is_empty(usFlowId)) 
        {
            //delete GEM by setting GEM Port ID = 0
            if (CA_E_OK != aal_gpon_us_gem_port_set(SATURN_DEV_ID, internal_gem_idx, 0))
            {
                CA8277B_LOG_ERR("%s:%d::Error Disable gem fail",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
             return RT_ERR_OK;
        }

        if (__tcont_tbl_is_empty(usFlowId)) 
        {
            aal_gpon_mac_ctrl_cfg_msk_t mac_ctrl_mask;
            aal_gpon_mac_ctrl_cfg_t     mac_ctrl_cfg;
            unsigned int tcontId = g8277bGemUsFlowMap[usFlowId].tcontId;

            mac_ctrl_mask.u32 = 0;
            memset(&mac_ctrl_cfg, 0, sizeof(mac_ctrl_cfg));

            mac_ctrl_mask.s.flush_en = 1;
            mac_ctrl_mask.s.flush_id = 1;
            mac_ctrl_cfg.flush_en = 1;
            mac_ctrl_cfg.flush_id = tcontId;

            //flush tcont
            if (CA_E_OK != aal_gpon_mac_ctrl_set(SATURN_DEV_ID, mac_ctrl_mask, &mac_ctrl_cfg))
            {
                CA8277B_LOG_ERR("%s:%d::Error tcont:%d flush fail",__FUNCTION__,__LINE__, tcontId);
                return RT_ERR_FAILED;
            }

            ca_mdelay(10);
            mac_ctrl_cfg.flush_en = 0;
            if (CA_E_OK != aal_gpon_mac_ctrl_set(SATURN_DEV_ID, mac_ctrl_mask, &mac_ctrl_cfg))
            {
                CA8277B_LOG_ERR("%s:%d::Error tcont:%d flush fail",__FUNCTION__,__LINE__, tcontId);
                return RT_ERR_FAILED;
            }

            if (tcontId != gem_id) 
            {
                if (CA_E_OK != aal_puc_pvtbl_enable_set(SATURN_DEV_ID, tcontId, FALSE))
                {
                    CA8277B_LOG_ERR("%s:%d::Error pus pvtbl set fail",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }
            }
        }
    }
    else
    {
        memset(&us_xgem_cfg, 0, sizeof(aal_xgpon_usbg_gem_config_t));
        us_mask.u32 = 0xffffffff;
    
        //us_xgem_cfg.xgem_id = pUsFlow->gemPortId;
        us_xgem_cfg.port_en = FALSE;
    
        
        if (CA_E_OK != aal_xgpon_usbg_xgem_cfg_set(SATURN_DEV_ID, internal_gem_idx, us_mask, &us_xgem_cfg))
        {
            CA8277B_LOG_ERR("%s:%d::Error Disable usbg gem fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    
    
        if (__tcont_tbl_is_empty(usFlowId)) 
        {
            aal_xgpon_bwmp_alloc_id_cfg_t  aal_bwmp_cfg;
            aal_xgpon_dsfp_xgem_cfg_t      aal_dsfp_cfg;
            unsigned int tcontId = g8277bGemUsFlowMap[usFlowId].tcontId;
    
            memset(&aal_bwmp_cfg, 0, sizeof(aal_bwmp_cfg));
    
            if (CA_E_OK != aal_xgpon_bwmp_tcont_get(SATURN_DEV_ID, 0, tcontId, &aal_bwmp_cfg))
            {
                CA8277B_LOG_ERR("%s:%d::Error bwmp get fail",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
    
    
            if (CA_E_OK != aal_xgpon_dsfp_xgem_get(SATURN_DEV_ID, 0, &aal_dsfp_cfg))
            {
                CA8277B_LOG_ERR("%s:%d::Error dsfp get fail",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
    
#if 0
            if (CA_E_OK != aal_xgpon_tcont_flush(SATURN_DEV_ID, 0, tcontId))
            {
                CA8277B_LOG_ERR("%s:%d::Error tcont:%d flush fail",__FUNCTION__,__LINE__,tcontId);
                return RT_ERR_FAILED;
            }
#endif
    
            if (aal_bwmp_cfg.alloc_id != aal_dsfp_cfg.gem_id) 
            {
                if (CA_E_OK != aal_puc_pvtbl_enable_set(SATURN_DEV_ID, tcontId, FALSE))
                {
                    CA8277B_LOG_ERR("%s:%d::Error pus pvtbl set fail",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }
            }
        }
    }
    
    return RT_ERR_OK;
}

static int 
_ca8277b_gpon_aal_dsGem_del(unsigned int dsFlowId)
{
    aal_xgpon_dsfp_xgem_cfg_msk_t ds_mask;
    aal_xgpon_dsfp_xgem_cfg_t   ds_xgem_cfg;
    unsigned short internal_gem_idx = 0;


    internal_gem_idx = LOGIC_FLOW_TO_CA8277B_FLOW(dsFlowId);

    if(CA_PON_MODE == ONU_PON_MAC_GPON)
    {
        aal_gpon_gem_port_cfg_msk_t  ds_gem_cfg_mask;
        aal_gpon_gem_port_cfg_t ds_gem_cfg;
        unsigned ds_gem_id;

        ds_gem_id = g8277bGemDsFlowMap[dsFlowId].gemPortId;
        ds_gem_cfg_mask.u32 = 0xffffffff;
        memset(&ds_gem_cfg, 0, sizeof(ds_gem_cfg));
        ds_gem_cfg.gem_index = internal_gem_idx;
        ds_gem_cfg.vld = 0;
        if (CA_E_OK != aal_gpon_ds_gem_port_set(SATURN_DEV_ID, 0, ds_gem_id, ds_gem_cfg_mask, &ds_gem_cfg))
        {
            CA8277B_LOG_ERR("%s:%d::set ds gem fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }
    else
    {
        ds_mask.u32 = 0xffffffff;
        memset(&ds_xgem_cfg, 0, sizeof(aal_xgpon_dsfp_xgem_cfg_t));
        
        ds_xgem_cfg.gem_vld = 0;
    
        if (CA_E_OK != aal_xgpon_dsfp_xgem_set(SATURN_DEV_ID, internal_gem_idx, ds_mask, &ds_xgem_cfg))
        {
            CA8277B_LOG_ERR("%s:%d::Disable ds gem fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }

    return RT_ERR_OK;

}


int32
_ca8277b_gpon_omci_mirror(uint32 msgLen, uint8 *pMsg)
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

        CA8277B_LOG_ERR("%s %d :Not enough memory to allocate skb of size %d.\n",
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
        CA8277B_LOG_ERR("%s %d : Mirror OMCI to %d Error,ret=%d !!",
            __FUNCTION__,__LINE__, gMirroringPort, ret);
        kfree(pData);
        return RT_ERR_FAILED;
    }

    kfree(pData);

    return RT_ERR_OK;

}

static uint32_t _ca8277b_gpon_omci_gen_crc(uint8_t *p, uint32_t len)
{
    uint32_t crc = 0xffffffff;
    uint32_t ret_crc;

    while (len--) {
        crc = (crc << 8) ^ crctab[(((crc >> 24) ^ *p++) & 0xFF)];
    }

    crc = (crc ^ 0xffffffff);

    ret_crc = (crc&0xff) << 24 | (crc&0xff00) << 8 | (crc&0xff0000) >> 8 | (crc&0xff000000) >> 24;

    return ret_crc;
}


static int _ca8277b_gpon_omci_build_crc(uint8_t * omci_ptr)
{
    rt_gpon_omci_pkt_hdr_t *pHdr = (rt_gpon_omci_pkt_hdr_t *)omci_ptr;
    
    uint32_t genCrc = 0;

    if (0x0a == pHdr->devId) // base omci format
    {
        genCrc = _ca8277b_gpon_omci_gen_crc(omci_ptr,sizeof(rt_gpon_omci_base_pkt_t)-sizeof(genCrc));
        if(!genCrc)
        {
            CA8277B_LOG_ERR("%s %d : Generate Crc Error!!",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        memcpy(omci_ptr+sizeof(rt_gpon_omci_base_pkt_t)-sizeof(genCrc),&genCrc,sizeof(genCrc));
    }
    else if (0x0b == pHdr->devId) //extend omci format
    {
        rt_gpon_omci_ext_pkt_t *pExtHdr = (rt_gpon_omci_ext_pkt_t *)omci_ptr;
        uint16_t contLen = 0;

        contLen = ntohs(pExtHdr->len);

        genCrc = _ca8277b_gpon_omci_gen_crc(omci_ptr,sizeof(rt_gpon_omci_ext_pkt_t)+contLen);
        if(!genCrc)
        {
            CA8277B_LOG_ERR("%s %d : Generate Crc Error!!",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        memcpy(omci_ptr+sizeof(rt_gpon_omci_ext_pkt_t)+contLen,&genCrc,sizeof(genCrc));
    }
    else 
    {
        CA8277B_LOG_ERR("%s %d : OMCI device id(%d))is not suppert",__FUNCTION__,__LINE__,pHdr->devId);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}


static int _ca8277b_gpon_omci_check_crc(uint8_t * omci_ptr)
{
    rt_gpon_omci_pkt_hdr_t *pHdr = (rt_gpon_omci_pkt_hdr_t *)omci_ptr;
    
    if (0x0a == pHdr->devId) // base omci format
    {
        rt_gpon_omci_base_pkt_t *basePkt = (rt_gpon_omci_base_pkt_t *)omci_ptr;
        uint32_t genCrc = 0;
        uint32_t oriCrc = 0;

        oriCrc = basePkt->crc;

        genCrc = _ca8277b_gpon_omci_gen_crc(omci_ptr,sizeof(rt_gpon_omci_base_pkt_t)-sizeof(genCrc));
        if (oriCrc != genCrc)
        {
            CA8277B_LOG_ERR("%s %d : GenCrc [0x%x] OriCrc [0x%x]",__FUNCTION__,__LINE__,genCrc, oriCrc);
            return RT_ERR_FAILED;
        }
    }
    else if (0x0b == pHdr->devId) //extend omci format
    {
        rt_gpon_omci_ext_pkt_t *pExtHdr = (rt_gpon_omci_ext_pkt_t *)omci_ptr;
        uint32_t genCrc  = 0;
        uint32_t oriCrc  = 0;
        uint16_t contLen = 0;

        contLen = ntohs(pExtHdr->len);
        memcpy(&oriCrc, (omci_ptr+sizeof(rt_gpon_omci_ext_pkt_t)+contLen), sizeof(genCrc));

        genCrc = _ca8277b_gpon_omci_gen_crc(omci_ptr,sizeof(rt_gpon_omci_ext_pkt_t)+contLen);
        if (oriCrc != genCrc)
        {
            CA8277B_LOG_ERR("%s %d : GenCrc [0x%x] OriCrc [0x%x]",__FUNCTION__,__LINE__,genCrc, oriCrc);
            return RT_ERR_FAILED;
        }
        
    }
    else 
    {
        CA8277B_LOG_ERR("%s %d : OMCI device id(%d))is not suppert",__FUNCTION__,__LINE__,pHdr->devId);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

static int _ca8277b_gpon_omci_build_mic(uint8_t * omci_ptr)
{
    rt_gpon_omci_pkt_hdr_t *pHdr = (rt_gpon_omci_pkt_hdr_t *)omci_ptr;
    uint32_t genMic = 0;
    
    if (0x0a == pHdr->devId) // base omci format
    {
        if (CA_E_OK != ca_generate_omci_mic(SATURN_DEV_ID, RT_GPON_OMCI_MIC_DIR_US, omci_ptr, sizeof(rt_gpon_omci_base_pkt_t)-4, 4, &genMic))        
        {
            CA8277B_LOG_ERR("%s %d generate OMCI MIC fail\n",__FUNCTION__,__LINE__);
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
            CA8277B_LOG_ERR("%s %d : Generate MIC Error!!",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        memcpy(omci_ptr+sizeof(rt_gpon_omci_ext_pkt_t)+contLen,&genMic,4);
    }
    else 
    {
        CA8277B_LOG_ERR("%s %d : OMCI device id(%d))is not suppert",__FUNCTION__,__LINE__,pHdr->devId);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}


static int _ca8277b_gpon_omci_check_mic(uint8_t * omci_ptr)
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
            CA8277B_LOG_ERR("%s %d generate OMCI MIC fail\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }


        if (oriMic != genMic)
        {
            CA8277B_LOG_ERR("%s %d : GenMic [0x%x] oriMic [0x%x]",__FUNCTION__,__LINE__,genMic, oriMic);
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
            CA8277B_LOG_ERR("%s %d : Generate MIC Error!!",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if (oriMic != genMic)
        {
            CA8277B_LOG_ERR("%s %d : GenMic [0x%x] oriMic [0x%x]",__FUNCTION__,__LINE__,genMic, oriMic);
            return RT_ERR_FAILED;
        }
    }
    else 
    {
        CA8277B_LOG_ERR("%s %d : OMCI device id(%d))is not suppert",__FUNCTION__,__LINE__,pHdr->devId);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}



int _ca8277b_receive_from_nic(struct napi_struct *napi,struct net_device *dev, struct sk_buff *skb, nic_hook_private_t *nh_priv)
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
            if(CA_PON_MODE == ONU_PON_MAC_XGPON1 || CA_PON_MODE == ONU_PON_MAC_XGSPON)
            {
                if(_ca8277b_gpon_omci_check_mic(omci_ptr)!= RT_ERR_OK)
                {
                    CA8277B_LOG_ERR("%s %d : OMCI check mic Error!!",__FUNCTION__,__LINE__);
                    return RE8670_RX_STOP;
                }
            }
            else if(CA_PON_MODE == ONU_PON_MAC_GPON)
            {
                if(_ca8277b_gpon_omci_check_crc(omci_ptr)!= RT_ERR_OK)
                {
                    CA8277B_LOG_ERR("%s %d : OMCI check crc Error!!",__FUNCTION__,__LINE__);
                    return RE8670_RX_STOP;
                }
            }
            else
            {
                CA8277B_LOG_ERR("%s %d : Not support PON MODE!!",__FUNCTION__,__LINE__);
                return RE8670_RX_STOP;
            }

        
            gOmciRxCallback(omci_len, omci_ptr);
            if(ENABLED == gOmciMirrorEnable)
            {
                _ca8277b_gpon_omci_mirror(omci_len, omci_ptr);     
            }
        }


        return RE8670_RX_STOP;
    }

    return RE8670_RX_CONTINUE;

}

static int _ca8277b_gpon_pm_phyLos_get(rt_gpon_pm_counter_t *pPmCnt)
{
    ca_phy_los_pm_t            stats  = {0};
    uint32_t                   bipCnt, bipFrame;

    memset(&pPmCnt->phyLos, 0x0, sizeof(rt_gpon_pm_phy_los_t));

    if(CA_PON_MODE == ONU_PON_MAC_GPON)
    {
        
        if (CA_E_OK != aal_gpon_current_bip_error_get(SATURN_DEV_ID, 0, &bipCnt, &bipFrame))
        {
            CA8277B_LOG_ERR("%s:%d::get ONU PHY/LOS count fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        pPmCnt->phyLos.bip_error_count = bipCnt;
    }
    else
    {
        memset(&stats, 0x00, sizeof(ca_phy_los_pm_t));  
        if (CA_E_OK != ca_ngp2_port_pm_counters_get(SATURN_DEV_ID,
                                                   SATURN_ANI_PORT_ID,
                                                   CA_COUNTERS_PHY_LOS_PM_T,
                                                   sizeof(ca_phy_los_pm_t),
                                                   (ca_uint8_t *)&stats))
        {
            CA8277B_LOG_ERR("%s:%d::get ONU PHY/LOS count fail",__FUNCTION__,__LINE__);
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
    }

    return RT_ERR_OK;
}

static int _ca8277b_gpon_pm_fec_get(rt_gpon_pm_counter_t *pPmCnt)
{
    ca_fec_pm_t            stats  = {0};
    aal_gpon_mac_fec_mibs_t gponStats;

    memset(&pPmCnt->fec, 0x0, sizeof(rt_gpon_pm_counter_fec_t));

    if(CA_PON_MODE == ONU_PON_MAC_GPON)
    {
        if (CA_E_OK != aal_gpon_fec_mibs_get(SATURN_DEV_ID, &gponStats))
        {
            CA8277B_LOG_ERR("%s:%d::get ONU fec count fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        
        pPmCnt->fec.corrected_fec_bytes = gponStats.fec_corr_byte_cnt;
        pPmCnt->fec.corrected_fec_codewords = gponStats.fec_rcnt;
        pPmCnt->fec.uncorrected_fec_codewords= gponStats.fec_ucnt;
        pPmCnt->fec.total_fec_codewords= gponStats.fec_blk_cnt;
    }
    else
    {
        memset(&stats, 0x00, sizeof(ca_fec_pm_t));  
        if (CA_E_OK != ca_ngp2_port_pm_counters_get(SATURN_DEV_ID,
                                                   SATURN_ANI_PORT_ID,
                                                   CA_COUNTERS_FEC_PM_T,
                                                   sizeof(ca_fec_pm_t),
                                                   (ca_uint8_t *)&stats))
        {
            CA8277B_LOG_ERR("%s:%d::get ONU fec count fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        pPmCnt->fec.corrected_fec_bytes = stats.corrected_fec_bytes;
        pPmCnt->fec.corrected_fec_codewords = stats.corrected_fec_codewords;
        pPmCnt->fec.uncorrected_fec_codewords= stats.uncorrected_fec_codewords; 
        pPmCnt->fec.total_fec_codewords= stats.total_fec_codewords; 
    }

    return RT_ERR_OK;
}

static int _ca8277b_gpon_pm_xgem_get(rt_gpon_pm_counter_t *pPmCnt)
{
    memset(&pPmCnt->xgem, 0x0, sizeof(rt_gpon_pm_counter_xgem_t));

    if(CA_PON_MODE == ONU_PON_MAC_GPON)
    {
        aal_gpon_ds_gem_port_mib_t  gpon_gem_dn;
        aal_gpon_us_gem_port_mib_t  gpon_gem_up;
        aal_gpon_stats_t stats;
        uint32_t gem;
        for (gem = 0; gem < AAL_GPON_SYSTEM_MAX_GEM_NUM; gem++)
        {
            if (CA_E_OK != aal_gpon_ds_gem_port_mib_get(SATURN_DEV_ID, gem, 0, &gpon_gem_dn))
            {
                CA8277B_LOG_ERR("%s:%d::get ONU DS GEM count %d fail",__FUNCTION__,__LINE__,gem);
                return RT_ERR_FAILED;
            }
            pPmCnt->xgem.total_received_xgem_frames += gpon_gem_dn.fcnt;
            pPmCnt->xgem.total_received_bytes_in_non_idle_xgem_frames += gpon_gem_dn.bcnt;

            if (CA_E_OK != aal_gpon_us_gem_port_mib_get(SATURN_DEV_ID, AAL_GPON_PLOAM_MIB_READ_CLR, gem, &gpon_gem_up))
            {
                CA8277B_LOG_ERR("%s:%d::get ONU US GEM count %d fail",__FUNCTION__,__LINE__,gem);
                return RT_ERR_FAILED;
            }
            pPmCnt->xgem.total_transmitted_xgem_frames += gpon_gem_up.fcnt;
            pPmCnt->xgem.total_transmitted_bytes_in_non_idle_xgem_frames += gpon_gem_up.bcnt;
        }
        if(CA_E_OK != aal_gpon_port_stats_get(SATURN_DEV_ID, SATURN_ANI_PORT_ID, &stats))
        {
            CA8277B_LOG_ERR("%s:%d::get gpon stats fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        pPmCnt->xgem.received_xgem_header_hec_errors = stats.gem_uncorrect_bit_err;
    }
    else
    {
        ca_xgem_pm_t            stats  = {0};

        memset(&stats, 0x00, sizeof(ca_xgem_pm_t));  
        if (CA_E_OK != ca_ngp2_port_pm_counters_get(SATURN_DEV_ID,
                                                   SATURN_ANI_PORT_ID,
                                                   CA_COUNTERS_XGEM_PM_T,
                                                   sizeof(ca_xgem_pm_t),
                                                   (ca_uint8_t *)&stats))
        {
            CA8277B_LOG_ERR("%s:%d::get ONU GEM count fail",__FUNCTION__,__LINE__);
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
    }

    return RT_ERR_OK;
}


static int _ca8277b_gpon_pm_ploam1_get(rt_gpon_pm_counter_t *pPmCnt)
{
    ca_ploam_pm1_t            stats  = {0};
    uint8_t                   tmp;
    uint32_t                  i;

    memset(&pPmCnt->ploam1, 0x0, sizeof(rt_gpon_pm_counter_ploam1_t));

    if(CA_PON_MODE == ONU_PON_MAC_GPON)
    {
        for(i = AAL_GPON_PLOAM_RX_MIB_UPSTREAM_OVERHEAD; i <= AAL_GPON_PLOAM_RX_MIB_EXTENDED_BURST_LENGTH; i++)
        {
            if (CA_E_OK != aal_gpon_ploam_mib_get(SATURN_DEV_ID, AAL_GPON_PLOAM_MIB_READ, i, &tmp))
            {
                CA8277B_LOG_ERR("%s:%d::get ONU PLOAM1 count fail",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
            pPmCnt->ploam1.downstream_ploam_message_count += tmp;
        }
        if (CA_E_OK != aal_gpon_ploam_mib_get(SATURN_DEV_ID, AAL_GPON_PLOAM_MIB_READ_CLR, AAL_GPON_PLOAM_RX_MIB_CRC_ERROR, &tmp))
        {
            CA8277B_LOG_ERR("%s:%d::get ONU PLOAM1 count fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        pPmCnt->ploam1.ploam_mic_errors = tmp;
        pPmCnt->ploam1.downstream_ploam_message_count += tmp;
        if (CA_E_OK != aal_gpon_ploam_mib_get(SATURN_DEV_ID, AAL_GPON_PLOAM_MIB_READ_CLR, AAL_GPON_PLOAM_RX_MIB_RANGING_TIME, &tmp))
        {
            CA8277B_LOG_ERR("%s:%d::get ONU PLOAM1 count fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        pPmCnt->ploam1.ranging_time_message_count = tmp;
    }
    else
    {
        memset(&stats, 0x00, sizeof(ca_ploam_pm1_t));  
        if (CA_E_OK != ca_ngp2_port_pm_counters_get(SATURN_DEV_ID,
                                                   SATURN_ANI_PORT_ID,
                                                   CA_COUNTERS_PLOAM_PM1_T,
                                                   sizeof(ca_ploam_pm1_t),
                                                   (ca_uint8_t *)&stats))
        {
            CA8277B_LOG_ERR("%s:%d::get ONU PLOAM1 count fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        pPmCnt->ploam1.ploam_mic_errors = stats.ploam_mic_errors;
        pPmCnt->ploam1.downstream_ploam_message_count = stats.downstream_ploam_message_count;
        pPmCnt->ploam1.ranging_time_message_count = stats.ranging_time_message_count;
        pPmCnt->ploam1.protection_control_message_count = stats.protection_control_message_count;
        pPmCnt->ploam1.adjust_tx_wavelength_message_count = stats.adjust_tx_wavelength_message_count;
        pPmCnt->ploam1.adjust_tx_wavelength_amplitude = stats.adjust_tx_wavelength_amplitude;
    }

    return RT_ERR_OK;
}

static int _ca8277b_gpon_pm_ploam2_get(rt_gpon_pm_counter_t *pPmCnt)
{
    ca_ploam_pm2_t            stats  = {0};
    uint8_t                   tmp;

    memset(&pPmCnt->ploam2, 0x0, sizeof(rt_gpon_pm_counter_ploam2_t));

    if(CA_PON_MODE == ONU_PON_MAC_GPON)
    {
        if (CA_E_OK != aal_gpon_ploam_mib_get(SATURN_DEV_ID, AAL_GPON_PLOAM_MIB_READ_CLR, AAL_GPON_PLOAM_RX_MIB_UPSTREAM_OVERHEAD, &tmp))
        {
            CA8277B_LOG_ERR("%s:%d::get ONU PLOAM2 count fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        pPmCnt->ploam2.burst_profile_message_count = tmp;

        if (CA_E_OK != aal_gpon_ploam_mib_get(SATURN_DEV_ID, AAL_GPON_PLOAM_MIB_READ_CLR, AAL_GPON_PLOAM_RX_MIB_ASSIGN_ONU_ID, &tmp))
        {
            CA8277B_LOG_ERR("%s:%d::get ONU PLOAM2 count fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        pPmCnt->ploam2.assign_onu_id_message_count = tmp;

        if (CA_E_OK != aal_gpon_ploam_mib_get(SATURN_DEV_ID, AAL_GPON_PLOAM_MIB_READ_CLR, AAL_GPON_PLOAM_RX_MIB_DEACTIVATE_ONU_ID, &tmp))
        {
            CA8277B_LOG_ERR("%s:%d::get ONU PLOAM2 count fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        pPmCnt->ploam2.deactivate_onu_id_message_count = tmp;

        if (CA_E_OK != aal_gpon_ploam_mib_get(SATURN_DEV_ID, AAL_GPON_PLOAM_MIB_READ_CLR, AAL_GPON_PLOAM_RX_MIB_DISABLE_SERIAL_NUMBER, &tmp))
        {
            CA8277B_LOG_ERR("%s:%d::get ONU PLOAM2 count fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        pPmCnt->ploam2.disable_serial_number_message_count = tmp;

        if (CA_E_OK != aal_gpon_ploam_mib_get(SATURN_DEV_ID, AAL_GPON_PLOAM_MIB_READ_CLR, AAL_GPON_PLOAM_RX_MIB_ENCRYPTED_PORT_ID, &tmp))
        {
            CA8277B_LOG_ERR("%s:%d::get ONU PLOAM2 count fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        pPmCnt->ploam2.enctyped_port_id_message_count = tmp;

        if (CA_E_OK != aal_gpon_ploam_mib_get(SATURN_DEV_ID, AAL_GPON_PLOAM_MIB_READ_CLR, AAL_GPON_PLOAM_RX_MIB_REQUEST_PASSWORD, &tmp))
        {
            CA8277B_LOG_ERR("%s:%d::get ONU PLOAM2 count fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        pPmCnt->ploam2.request_registration_message_count = tmp;

        if (CA_E_OK != aal_gpon_ploam_mib_get(SATURN_DEV_ID, AAL_GPON_PLOAM_MIB_READ_CLR, AAL_GPON_PLOAM_RX_MIB_ASSIGN_ALLOC_ID, &tmp))
        {
            CA8277B_LOG_ERR("%s:%d::get ONU PLOAM2 count fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        pPmCnt->ploam2.assign_alloc_id_message_count = tmp;

        if (CA_E_OK != aal_gpon_ploam_mib_get(SATURN_DEV_ID, AAL_GPON_PLOAM_MIB_READ_CLR, AAL_GPON_PLOAM_RX_MIB_NO_MESSAGE, &tmp))
        {
            CA8277B_LOG_ERR("%s:%d::get ONU PLOAM2 count fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        pPmCnt->ploam2.no_message_count = tmp;

        if (CA_E_OK != aal_gpon_ploam_mib_get(SATURN_DEV_ID, AAL_GPON_PLOAM_MIB_READ_CLR, AAL_GPON_PLOAM_RX_MIB_POPUP, &tmp))
        {
            CA8277B_LOG_ERR("%s:%d::get ONU PLOAM2 count fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        pPmCnt->ploam2.popup_message_count = tmp;

        if (CA_E_OK != aal_gpon_ploam_mib_get(SATURN_DEV_ID, AAL_GPON_PLOAM_MIB_READ_CLR, AAL_GPON_PLOAM_RX_MIB_REQUEST_KEY, &tmp))
        {
            CA8277B_LOG_ERR("%s:%d::get ONU PLOAM2 count fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        pPmCnt->ploam2.request_key_message_count = tmp;

        if (CA_E_OK != aal_gpon_ploam_mib_get(SATURN_DEV_ID, AAL_GPON_PLOAM_MIB_READ_CLR, AAL_GPON_PLOAM_RX_MIB_CONFIGURE_PORT_ID, &tmp))
        {
            CA8277B_LOG_ERR("%s:%d::get ONU PLOAM2 count fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        pPmCnt->ploam2.config_port_id_message_count = tmp;

        if (CA_E_OK != aal_gpon_ploam_mib_get(SATURN_DEV_ID, AAL_GPON_PLOAM_MIB_READ_CLR, AAL_GPON_PLOAM_RX_MIB_PEE, &tmp))
        {
            CA8277B_LOG_ERR("%s:%d::get ONU PLOAM2 count fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        pPmCnt->ploam2.pee_message_count = tmp;

        if (CA_E_OK != aal_gpon_ploam_mib_get(SATURN_DEV_ID, AAL_GPON_PLOAM_MIB_READ_CLR, AAL_GPON_PLOAM_RX_MIB_CHANGE_POWER_LEVEL, &tmp))
        {
            CA8277B_LOG_ERR("%s:%d::get ONU PLOAM2 count fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        pPmCnt->ploam2.change_power_level_message_count = tmp;

        if (CA_E_OK != aal_gpon_ploam_mib_get(SATURN_DEV_ID, AAL_GPON_PLOAM_MIB_READ_CLR, AAL_GPON_PLOAM_RX_MIB_PST_MESSAGE, &tmp))
        {
            CA8277B_LOG_ERR("%s:%d::get ONU PLOAM2 count fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        pPmCnt->ploam2.pst_message_count = tmp;

        if (CA_E_OK != aal_gpon_ploam_mib_get(SATURN_DEV_ID, AAL_GPON_PLOAM_MIB_READ_CLR, AAL_GPON_PLOAM_RX_MIB_BER_INTERVAL, &tmp))
        {
            CA8277B_LOG_ERR("%s:%d::get ONU PLOAM2 count fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        pPmCnt->ploam2.ber_interval_message_count = tmp;

        if (CA_E_OK != aal_gpon_ploam_mib_get(SATURN_DEV_ID, AAL_GPON_PLOAM_MIB_READ_CLR, AAL_GPON_PLOAM_RX_MIB_KEY_SWITCHING_TIME, &tmp))
        {
            CA8277B_LOG_ERR("%s:%d::get ONU PLOAM2 count fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        pPmCnt->ploam2.key_switch_time_message_count = tmp;

        if (CA_E_OK != aal_gpon_ploam_mib_get(SATURN_DEV_ID, AAL_GPON_PLOAM_MIB_READ_CLR, AAL_GPON_PLOAM_RX_MIB_EXTENDED_BURST_LENGTH, &tmp))
        {
            CA8277B_LOG_ERR("%s:%d::get ONU PLOAM2 count fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        pPmCnt->ploam2.extend_burst_len_message_count = tmp;
    }
    else
    {
        memset(&stats, 0x00, sizeof(ca_ploam_pm2_t));  
        if (CA_E_OK != ca_ngp2_port_pm_counters_get(SATURN_DEV_ID,
                                                   SATURN_ANI_PORT_ID,
                                                   CA_COUNTERS_PLOAM_PM2_T,
                                                   sizeof(ca_ploam_pm2_t),
                                                   (ca_uint8_t *)&stats))
        {
            CA8277B_LOG_ERR("%s:%d::get ONU PLOAM2 count fail",__FUNCTION__,__LINE__);
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
    }

    return RT_ERR_OK;
}

static int _ca8277b_gpon_pm_ploam3_get(rt_gpon_pm_counter_t *pPmCnt)
{
    ca_ploam_pm3_t            stats  = {0};
    uint8_t                   tmp;

    memset(&pPmCnt->ploam3, 0x0, sizeof(rt_gpon_pm_counter_ploam3_t));

    if(CA_PON_MODE == ONU_PON_MAC_GPON)
    {
        if (CA_E_OK != aal_gpon_ploam_mib_get(SATURN_DEV_ID, AAL_GPON_PLOAM_MIB_READ_CLR, AAL_GPON_PLOAM_TX_MIB_SERIAL_NUMBER_ONU, &tmp))
        {
            CA8277B_LOG_ERR("%s:%d::get ONU PLOAM3 count fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        pPmCnt->ploam3.serial_number_onu_in_band_message_count = tmp;
        pPmCnt->ploam3.upstream_ploam_message_count += tmp;

        if (CA_E_OK != aal_gpon_ploam_mib_get(SATURN_DEV_ID, AAL_GPON_PLOAM_MIB_READ_CLR, AAL_GPON_PLOAM_TX_MIB_PASSWORD, &tmp))
        {
            CA8277B_LOG_ERR("%s:%d::get ONU PLOAM3 count fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        pPmCnt->ploam3.registration_message_count = tmp;
        pPmCnt->ploam3.upstream_ploam_message_count += tmp;

        if (CA_E_OK != aal_gpon_ploam_mib_get(SATURN_DEV_ID, AAL_GPON_PLOAM_MIB_READ_CLR, AAL_GPON_PLOAM_TX_MIB_DYING_GASP, &tmp))
        {
            CA8277B_LOG_ERR("%s:%d::get ONU PLOAM3 count fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        pPmCnt->ploam3.dying_gasp_message_count = tmp;
        pPmCnt->ploam3.upstream_ploam_message_count += tmp;

        if (CA_E_OK != aal_gpon_ploam_mib_get(SATURN_DEV_ID, AAL_GPON_PLOAM_MIB_READ_CLR, AAL_GPON_PLOAM_TX_MIB_NO_MESSAGE, &tmp))
        {
            CA8277B_LOG_ERR("%s:%d::get ONU PLOAM3 count fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        pPmCnt->ploam3.no_message_count = tmp;
        pPmCnt->ploam3.upstream_ploam_message_count += tmp;
        
        if (CA_E_OK != aal_gpon_ploam_mib_get(SATURN_DEV_ID, AAL_GPON_PLOAM_MIB_READ_CLR, AAL_GPON_PLOAM_TX_MIB_ENCRYPTION_KEY, &tmp))
        {
            CA8277B_LOG_ERR("%s:%d::get ONU PLOAM3 count fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        pPmCnt->ploam3.encrypt_key_message_count = tmp;
        pPmCnt->ploam3.upstream_ploam_message_count += tmp;

        if (CA_E_OK != aal_gpon_ploam_mib_get(SATURN_DEV_ID, AAL_GPON_PLOAM_MIB_READ_CLR, AAL_GPON_PLOAM_TX_MIB_PEE, &tmp))
        {
            CA8277B_LOG_ERR("%s:%d::get ONU PLOAM3 count fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        pPmCnt->ploam3.pee_message_count = tmp;
        pPmCnt->ploam3.upstream_ploam_message_count += tmp;

        if (CA_E_OK != aal_gpon_ploam_mib_get(SATURN_DEV_ID, AAL_GPON_PLOAM_MIB_READ_CLR, AAL_GPON_PLOAM_TX_MIB_PST_MESSAGE, &tmp))
        {
            CA8277B_LOG_ERR("%s:%d::get ONU PLOAM3 count fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        pPmCnt->ploam3.pst_message_count = tmp;
        pPmCnt->ploam3.upstream_ploam_message_count += tmp;

        if (CA_E_OK != aal_gpon_ploam_mib_get(SATURN_DEV_ID, AAL_GPON_PLOAM_MIB_READ_CLR, AAL_GPON_PLOAM_TX_MIB_REI, &tmp))
        {
            CA8277B_LOG_ERR("%s:%d::get ONU PLOAM3 count fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        pPmCnt->ploam3.rei_message_count = tmp;
        pPmCnt->ploam3.upstream_ploam_message_count += tmp;

        if (CA_E_OK != aal_gpon_ploam_mib_get(SATURN_DEV_ID, AAL_GPON_PLOAM_MIB_READ_CLR, AAL_GPON_PLOAM_TX_MIB_ACKNOWLEDGE, &tmp))
        {
            CA8277B_LOG_ERR("%s:%d::get ONU PLOAM3 count fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        pPmCnt->ploam3.acknowledgement_message_count = tmp;
        pPmCnt->ploam3.upstream_ploam_message_count += tmp;
    }
    else
    {
        memset(&stats, 0x00, sizeof(ca_ploam_pm3_t));  
        if (CA_E_OK != ca_ngp2_port_pm_counters_get(SATURN_DEV_ID,
                                                   SATURN_ANI_PORT_ID,
                                                   CA_COUNTERS_PLOAM_PM3_T,
                                                   sizeof(ca_ploam_pm3_t),
                                                   (ca_uint8_t *)&stats))
        {
            CA8277B_LOG_ERR("%s:%d::get ONU PLOAM3 count fail",__FUNCTION__,__LINE__);
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
    }

    return RT_ERR_OK;
}

static int _ca8277b_gpon_pm_chan_tuning1_get(rt_gpon_pm_counter_t *pPmCnt)
{
    ca_chan_tuning_pm1_t            stats  = {0};

    memset(&stats, 0x00, sizeof(ca_chan_tuning_pm1_t));  

    if(CA_PON_MODE == ONU_PON_MAC_GPON)
    {
        //FIXME
    }
    else
    {
        if (CA_E_OK != ca_ngp2_port_pm_counters_get(SATURN_DEV_ID,
                                                   SATURN_ANI_PORT_ID,
                                                   CA_COUNTERS_CHAN_TUNING_PM1_T,
                                                   sizeof(ca_chan_tuning_pm1_t),
                                                   (ca_uint8_t *)&stats))
        {
            CA8277B_LOG_ERR("%s:%d::get ONU chan_tuning1 count fail",__FUNCTION__,__LINE__);
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
    }

    return RT_ERR_OK;
}

static int _ca8277b_gpon_pm_chan_tuning2_get(rt_gpon_pm_counter_t *pPmCnt)
{
    ca_chan_tuning_pm2_t            stats  = {0};

    memset(&stats, 0x00, sizeof(ca_chan_tuning_pm2_t));  

    if(CA_PON_MODE == ONU_PON_MAC_GPON)
    {
        //FIXME
    }
    else
    {
        if (CA_E_OK != ca_ngp2_port_pm_counters_get(SATURN_DEV_ID,
                                                   SATURN_ANI_PORT_ID,
                                                   CA_COUNTERS_CHAN_TUNING_PM2_T,
                                                   sizeof(ca_chan_tuning_pm2_t),
                                                   (ca_uint8_t *)&stats))
        {
            CA8277B_LOG_ERR("%s:%d::get ONU chan_tuning2 count fail",__FUNCTION__,__LINE__);
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
    }

    return RT_ERR_OK;
}

static int _ca8277b_gpon_pm_chan_tuning3_get(rt_gpon_pm_counter_t *pPmCnt)
{
    ca_chan_tuning_pm3_t            stats  = {0};

    memset(&stats, 0x00, sizeof(ca_chan_tuning_pm3_t));  

    if(CA_PON_MODE == ONU_PON_MAC_GPON)
    {
        //FIXME
    }
    else
    {
        if (CA_E_OK != ca_ngp2_port_pm_counters_get(SATURN_DEV_ID,
                                                   SATURN_ANI_PORT_ID,
                                                   CA_COUNTERS_CHAN_TUNING_PM3_T,
                                                   sizeof(ca_chan_tuning_pm3_t),
                                                   (ca_uint8_t *)&stats))
        {
            CA8277B_LOG_ERR("%s:%d::get ONU chan_tuning3 count fail",__FUNCTION__,__LINE__);
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
    }

    return RT_ERR_OK;
}

static int _ca8277b_gpon_pm_omci_get(rt_gpon_pm_counter_t *pPmCnt)
{
#if 0
    ca_omci_pm_t            stats  = {0};
#endif
    /* OMCI counters for debug */
    aal_xgpon_usbg_xgem_mib_config_t gem_mib;
    aal_xgpon_dsfp_xgem_mib_config_t gem_mib_dn;
    ca_uint32_t  count_up[2] = {0};
    ca_uint32_t  count_dn[2] = {0};
    ca_uint32_t  gem = 0;

#if 0 //OMCI counters should retrieved from OMCI application
    memset(&stats, 0x00, sizeof(ca_omci_pm_t));  
    if (CA_E_OK != ca_ngp2_port_pm_counters_get(SATURN_DEV_ID,
                                               SATURN_ANI_PORT_ID,
                                               CA_COUNTERS_OMCI_PM_T,
                                               sizeof(ca_omci_pm_t),
                                               (ca_uint8_t *)&stats))
    {
        CA8277B_LOG_ERR("%s:%d::get ONU OMCI count fail",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    pPmCnt->omci.omci_baseline_msg_count = stats.omci_baseline_msg_count;
    pPmCnt->omci.omci_extended_message_count = stats.omci_extended_message_count;
    pPmCnt->omci.omci_mic_error_count = stats.omci_mic_error_count;
#endif

    if(CA_PON_MODE == ONU_PON_MAC_GPON)
    {
        aal_gpon_stats_t stats;
        aal_gpon_us_gem_port_mib_t gem_up;

        /* get OMCI counters from ds/us OMCI GEMs */
        if(CA_E_OK != aal_gpon_port_stats_get(SATURN_DEV_ID, SATURN_ANI_PORT_ID, &stats))
        {
            CA8277B_LOG_ERR("%s:%d::get gpon stats fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    
        count_up[0] = 0;
        AAL_FOR_ALL_OMCI_XGEM_DO(gem) {
            memset(&gem_up, 0, sizeof(gem_up));
            aal_gpon_us_gem_port_mib_get(SATURN_DEV_ID, 0, gem, &gem_up);
            count_up[0] += gem_up.fcnt;
        }
    
        pPmCnt->omci.omci_tx_msg_count = count_up[0];
        pPmCnt->omci.omci_rx_msg_count = stats.ds_omci_pkt;
    }
    else
    {
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
    }

    return RT_ERR_OK;
}

static int _ca8277b_gpon_omcc_get(rt_gpon_omcc_t *pOmcc)
{
    int32 ret = RT_ERR_OK;

    if(CA_PON_MODE == ONU_PON_MAC_GPON)
    {
        aal_gpon_onu_params_t  gpon_onu_param;

        memset(&gpon_onu_param, 0, sizeof(aal_gpon_onu_params_t));
        if (CA_E_OK != aal_gpon_onu_params_get(SATURN_DEV_ID, 0, &gpon_onu_param))
        {
            CA8277B_LOG_ERR("%s:%d::Error gpon onu get fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if(0xFF == gpon_onu_param.onu_id)
            return RT_ERR_ENTRY_NOTFOUND;

        pOmcc->allocId = gpon_onu_param.onu_id;
        pOmcc->gemId = gpon_onu_param.omci_port;        
    }
    else
    {
        aal_xgpon_bwmp_alloc_id_cfg_t aal_bwmp_cfg;
        aal_xgpon_usbg_gem_config_t us_xgem_cfg;

        memset(&aal_bwmp_cfg, 0, sizeof(aal_bwmp_cfg));
        memset(&us_xgem_cfg, 0, sizeof(aal_xgpon_usbg_gem_config_t));
    
        if (CA_E_OK != aal_xgpon_bwmp_tcont_get(SATURN_DEV_ID, 0, 0, &aal_bwmp_cfg))
        {
            CA8277B_LOG_ERR("%s:%d::Error bwmp get fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    
        if(aal_bwmp_cfg.omci_en != 1 && aal_bwmp_cfg.ploam_en != 1)
            return RT_ERR_ENTRY_NOTFOUND;
    
        pOmcc->allocId = aal_bwmp_cfg.alloc_id;
    
        if (CA_E_OK != aal_xgpon_usbg_xgem_cfg_get(SATURN_DEV_ID, 0, &us_xgem_cfg))
        {
            CA8277B_LOG_ERR("%s:%d::ERROR get usbg xgem fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    
        if(us_xgem_cfg.port_en !=1)
            return RT_ERR_ENTRY_NOTFOUND;
    
        pOmcc->gemId = us_xgem_cfg.xgem_id;
    }

    return ret;
}



/* Function Name:
 *      dal_rt_ca8277b_gpon_init
 * Description:
 *      gpon register level initial function
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 dal_rt_ca8277b_gpon_init(void)
{
    int ret;
    
    gPonMode = aal_pon_mac_mode_get(0);
    if(!CA_PON_MODE_IS_GPON_FAMILY(gPonMode))
    {
        RT_ERR(ret, (MOD_DAL|MOD_GPON), "");
        printk("PON MODE 0x%d is note correct\n", gPonMode);
        gpon_init = INIT_NOT_COMPLETED;
        return RT_ERR_FAILED;
    }

    if(aal_init_pon == 0)
    {
        /* load other pon related scfg settings */
        ret =aal_pon_scfg_load(0);
        if(CA_E_OK != ret) {
            printk("%s:%d %s returns error, ret = %d\n",__FUNCTION__, __LINE__, "aal_pon_scfg_load()", ret);
            gpon_init = INIT_NOT_COMPLETED;
            return RT_ERR_FAILED;
        }

        /* PONMAC init */
        ret = ca_pon_init(0);
        if(CA_E_OK != ret) {
            printk("%s:%d %s returns error, ret = %d\n",__FUNCTION__, __LINE__, "ca_pon_init()", ret);
            gpon_init = INIT_NOT_COMPLETED;
            return RT_ERR_FAILED;
        }
    }

    _dal_ca8277b_gpon_db_init();
        

    /*reset GEM counters*/
    memset(g8277bDsGemCnts, 0x0, sizeof(rt_gpon_flow_counter_t)*AAL_XGPON_GEM_PORT_NUM);
    memset(g8277bUsGemCnts, 0x0, sizeof(rt_gpon_flow_counter_t)*AAL_XGPON_GEM_PORT_NUM);

    /*set GPON DS Gem counter as read-clear */
    if(ONU_PON_MAC_GPON == gPonMode)
    {
        aal_gpon_mac_cfg_msk_t gponmac_mask;
        aal_gpon_mac_cfg_t     gponmac_cfg;

        memset(&gponmac_mask, 0, sizeof(aal_gpon_mac_cfg_msk_t));
        memset(&gponmac_cfg, 0, sizeof(aal_gpon_mac_cfg_t));
        gponmac_mask.s.pm_mode = 1;
        gponmac_cfg.pm_mode = 2;

        aal_gpon_mac_cfg_set(SATURN_DEV_ID, gponmac_mask, &gponmac_cfg);
    }
    dal_ca8277b_stat_port_reset(HAL_GET_PON_PORT());

    gpon_init = INIT_COMPLETED;
    return RT_ERR_OK;    
}   /* end of dal_ca8277b_gpon_init */


/* Function Name:
 *      dal_rt_ca8277b_gpon_serialNumber_set
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
dal_rt_ca8277b_gpon_serialNumber_set(rt_gpon_serialNumber_t *pSN)
{
    ca_status_t ret = CA_E_OK;
    ca_uint8_t serial_number[GPON_ONU_SN_LEN];    
    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pSN), RT_ERR_NULL_POINTER);    

    /* function body */
    memcpy(&serial_number[0], &pSN->vendor[0], GPON_ONU_SN_LEN);

    //FIXME: remove CA API and seperate GPON/XGPON mode
    ret = ca_gpon_serial_number_set(SATURN_DEV_ID, &serial_number[0]);
    if(CA_E_OK != ret)
    {
        CA8277B_LOG_ERR("%s %d set gpon sn  fail\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;


}   /* end of dal_ca8277b_gpon_serialNumber_set */

/* Function Name:
 *      dal_rt_ca8277b_gpon_registrationId_set
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
dal_rt_ca8277b_gpon_registrationId_set(rt_gpon_registrationId_t *pRegId)
{
    ca_status_t   ret;

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pRegId), RT_ERR_NULL_POINTER);  

    ca_mdelay(1500);
    //FIXME: remove CA API and seperate GPON/XGPON mode
    ret = ca_gpon_registration_id_set(SATURN_DEV_ID, (ca_uint8_t *)pRegId);
    if(CA_E_OK != ret)
    {
        CA8277B_LOG_ERR("%s %d set gpon rgsd id fail\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
}   /* end of dal_rt_ca8277b_registrationId_set */


/* Function Name:
 *      dal_rt_ca8277b_gpon_activate
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
dal_rt_ca8277b_gpon_activate(rt_gpon_initialState_t initState)
{
    ca_uint32_t ret = CA_E_OK;
    static int first_init = 1;

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((RT_GPON_ONU_INIT_STATE_O7 <initState), RT_ERR_INPUT);

    if(first_init)
    {   //first init no need to wait PONMAC ready
        first_init = 0;
    }
    else
    {
        ca_mdelay(2500);
    }

    //FIXME: remove CA API and seperate GPON/XGPON mode
    if((ret = ca_port_enable_set( 0, CA_PORT_GPON_PORT, 1)) != CA_E_OK)
    {
        CA8277B_LOG_ERR("%s %d activate GPON fail\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;

}   /* end of dal_rt_ca8277b_activate */

/* Function Name:
 *      dal_rt_ca8277b_gpon_deactivate
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
dal_rt_ca8277b_gpon_deactivate(void)
{
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* function body */
    //FIXME: remove CA API and seperate GPON/XGPON mode
    if((ret = ca_port_enable_set( 0, CA_PORT_GPON_PORT, 0)) != CA_E_OK)
    {
        CA8277B_LOG_ERR("%s %d deactivate GPON fail\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;

}   /* end of dal_rt_ca8277b_gpon_deactivate */




/* Function Name:
 *      dal_rt_ca8277b_gpon_onuState_get
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
int32 dal_rt_ca8277b_gpon_onuState_get(rt_gpon_onuState_t  *pOnuState)
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
    //FIXME: remove CA API and seperate GPON/XGPON mode
    if((ret = ca_gpon_port_onu_state_get(0, port_id, &caGponState))!=CA_E_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    switch(caGponState)
    {
        case CA_NGP2_ONU_ACT_STATE_O1_1:    
            *pOnuState = RT_NGP2_ONU_STATE_O1_1;
            break;		
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
}   /* end of   dal_ca8277b_gpon_onuState_get */


/* Function Name:
 *      dal_rt_ca8277b_gpon_omci_tx
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
dal_rt_ca8277b_gpon_omci_tx(uint32 msgLen, uint8 *pMsg)
{

    ca_pkt_t halPkt;
    ca_pkt_block_t pkt_data;
    int32 ret;

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pMsg), RT_ERR_NULL_POINTER);

    if(CA_PON_MODE == ONU_PON_MAC_XGPON1 || CA_PON_MODE == ONU_PON_MAC_XGSPON)
    {
        if(_ca8277b_gpon_omci_build_mic(pMsg)!= RT_ERR_OK)
        {
            CA8277B_LOG_ERR("%s %d : OMCI build mic Error!!",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }
    else if(CA_PON_MODE == ONU_PON_MAC_GPON)
    {
        if(_ca8277b_gpon_omci_build_crc(pMsg)!= RT_ERR_OK)
        {
            CA8277B_LOG_ERR("%s %d : OMCI build crc Error!!",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }
    else
    {
        CA8277B_LOG_ERR("%s %d : Not support PON MODE!!",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    memset(&halPkt,0,sizeof(halPkt));
    halPkt.block_count    = 1;
    if(pMsg[0] & 0x80)
        halPkt.cos            = 8;
    else
        halPkt.cos            = 6;
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
        CA8277B_LOG_ERR("%s %d : TX OMCI Error!!",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }


    if(ENABLED == gOmciMirrorEnable)
    {
        _ca8277b_gpon_omci_mirror(msgLen, pMsg);     
    }


    return RT_ERR_OK;

}


/* Function Name:
 *      dal_rt_ca8277b_gpon_omci_rx_callback_register
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
dal_rt_ca8277b_gpon_omci_rx_callback_register(rt_gpon_omci_rx_callback omciRx)
{
    int32   ret;
    rtk_port_t pon;
    int ponPortMask;

    pon = HAL_GET_PON_PORT();
    ponPortMask = 1 << pon;

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* function body */

    if(DISABLED == gOmciTrapEnable)
    {
        if((ret = drv_nic_register_rxhook(ponPortMask,7,&_ca8277b_receive_from_nic))!=RT_ERR_OK)
        {
            CA8277B_LOG_ERR("drv_nic_register_rxhook Error!!\n");
            return ret;
        }
        gOmciTrapEnable = ENABLED;
    }

    gOmciRxCallback = omciRx;
    
    return RT_ERR_OK;
}   /* end of dal_rt_ca8277b_gpon_omci_rx_callback_register */


/* Function Name:
 *      dal_rt_ca8277b_gpon_tcont_set
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
dal_rt_ca8277b_gpon_tcont_set(uint32 *pTcontId, uint32 allocId)
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
            CA8277B_LOG_ERR("%s %d Get non used TcontId fail\n",
                __FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        
        if (RT_ERR_OK == _ca8277b_gpon_aal_tcont_set(LOGIC_TCONID_TO_CA8277B_TCONID(tcontId), allocId))
        {

            _AssignNonUsedTcontId(allocId, &tcontId);
            *pTcontId = tcontId;
        }
        else
        {
            CA8277B_LOG_ERR("%s %d set ca gpon tcont fail\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }
     CA8277B_LOG_DBG("%s %d allocId:%u tcontId:%d\n",__FUNCTION__,__LINE__,allocId,*pTcontId);  


    return RT_ERR_OK;

}


/* Function Name:
 *      dal_rt_ca8277b_gpon_tcont_get
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
dal_rt_ca8277b_gpon_tcont_get(uint32 tcontId, uint32 *pAllocId)
{

    uint32 allocId;
    
    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pAllocId), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((tcontId >= CA8277B_MAX_TCONT), RT_ERR_INPUT);
   
    /*Check tcontID is exist*/
    if(RT_ERR_OK == _GetAllocIdByTcontId(tcontId, &allocId))
    {
        *pAllocId = allocId;
    }
    else
    {
        //CA8277B_LOG_DBG("%s %d : no matched TCONT id [%d]\n", __FUNCTION__, __LINE__, tcontId);  
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_rt_ca8277b_gpon_tcont_del
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
dal_rt_ca8277b_gpon_tcont_del(uint32 tcontId)
{

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((tcontId >= CA8277B_MAX_TCONT), RT_ERR_INPUT);

    if(RT_ERR_OK != _ca8277b_gpon_aal_tcont_del(LOGIC_TCONID_TO_CA8277B_TCONID(tcontId)))
    {
        CA8277B_LOG_ERR("%s %d set del tcont fail\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if(RT_ERR_OK != _DelTcontId(tcontId))
    {
        CA8277B_LOG_ERR("%s %d set del tcont fail\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;

}


/* Function Name:
 *      dal_rt_ca8277b_gpon_usFlow_set
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
int32 dal_rt_ca8277b_gpon_usFlow_set(uint32 usFlowId, const rt_gpon_usFlow_t *pUsFlow)
{
    unsigned short gemIdx = 0;
    rt_gpon_usFlow_t usFlowCfg;
    
    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pUsFlow), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((usFlowId >= CA8277B_MAX_GEM_FLOW), RT_ERR_INPUT);
    RT_PARAM_CHK((pUsFlow->tcontId >= CA8277B_MAX_TCONT), RT_ERR_INPUT);
    RT_PARAM_CHK((pUsFlow->tcQueueId >= CA8277B_MAX_TCONT_QUEUE), RT_ERR_INPUT);
    
    /* function body */

    if((TRUE == g8277bGemUsFlowMap[usFlowId].isValid) && 
        (g8277bGemUsFlowMap[usFlowId].tcQueueId == pUsFlow->tcQueueId) &&
        (g8277bGemUsFlowMap[usFlowId].tcontId) == LOGIC_TCONID_TO_CA8277B_TCONID(pUsFlow->tcontId) &&
        g8277bGemUsFlowMap[usFlowId].aesEn == pUsFlow->aesEn)

    {        
        CA8277B_LOG_ERR("%s %d Update gemFlowId:%d \n",
            __FUNCTION__,__LINE__,usFlowId);

        return RT_ERR_OK;
    }

    memcpy(&usFlowCfg, pUsFlow, sizeof(rt_gpon_usFlow_t));
    usFlowCfg.tcontId = LOGIC_TCONID_TO_CA8277B_TCONID(pUsFlow->tcontId);
    if (RT_ERR_OK != _ca8277b_gpon_aal_usGem_set(
                                usFlowId, 
                                &usFlowCfg, 
                                &gemIdx))
    {
        CA8277B_LOG_ERR("%s %d set us flow fail\n",
            __FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }


    //save the US flow database
    if(RT_ERR_OK != _ca8277b_SetGemFlowMap(GEM_FLOW_DIR_US, usFlowId, &usFlowCfg, gemIdx, pUsFlow->gemPortId))
    {
        CA8277B_LOG_ERR("%s %d set gemflow map fail\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;            
    }

    return RT_ERR_OK;
}


 /* Function Name:
 *      dal_rt_ca8277b_gpon_usFlow_get
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
int32 dal_rt_ca8277b_gpon_usFlow_get(uint32 usFlowId, rt_gpon_usFlow_t *pUsFlow)
{
    uint32 logicTcontId = 0;
    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pUsFlow), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((usFlowId >= CA8277B_MAX_GEM_FLOW), RT_ERR_INPUT);
    
    /* function body */
    if(TRUE != g8277bGemUsFlowMap[usFlowId].isValid) 
    {        
        //CA8277B_LOG_ERR("%s %d invalid us flow [%d]\n", __FUNCTION__, __LINE__, usFlowId);
        return RT_ERR_INPUT;
    }

    if (RT_ERR_OK != _ca8277b_gpon_aal_usGem_get(
                                usFlowId, 
                                pUsFlow))
    {
        CA8277B_LOG_ERR("%s %d get us gem fail\n",
            __FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    
    if(RT_ERR_OK != _ca8277b_GetPhy2LogicTcontMap(g8277bGemUsFlowMap[usFlowId].tcontId, &logicTcontId))
    {
        CA8277B_LOG_ERR("%s %d get phy:%u to logic T-cont fail\n",  
            __FUNCTION__, __LINE__, g8277bGemUsFlowMap[usFlowId].tcontId);

        return RT_ERR_FAILED;

    }        
    
    pUsFlow->tcontId        = logicTcontId;
    pUsFlow->tcQueueId      = g8277bGemUsFlowMap[usFlowId].tcQueueId;

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_rt_ca8277b_gpon_usFlow_del
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
int32 dal_rt_ca8277b_gpon_usFlow_del(uint32 usFlowId)
{
    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */    
    RT_PARAM_CHK((usFlowId >= CA8277B_MAX_GEM_FLOW), RT_ERR_INPUT);

    CA8277B_LOG_DBG("%s %d flowId:%d gemIdx:0x%x\n",
        __FUNCTION__,
        __LINE__,
        usFlowId,
        g8277bGemUsFlowMap[usFlowId].gemIdx);

    /* function body */

    /*Check the US flow is exist*/
    if(FALSE == g8277bGemUsFlowMap[usFlowId].isValid)
        return RT_ERR_OK;
    

    if (RT_ERR_OK != _ca8277b_gpon_aal_usGem_del(usFlowId))
    {
        CA8277B_LOG_ERR("%s %d del us flow fail\n",
            __FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }


    if(RT_ERR_OK != _ca8277b_DeleteGemFlowMap(usFlowId, GEM_FLOW_DIR_US))
    {
        CA8277B_LOG_ERR("%s %d del gemflow map fail\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;            
    }
   
    return RT_ERR_OK;

}


/* Function Name:
 *      dal_rt_ca8277b_gpon_usFlow_delAll
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
int32 dal_rt_ca8277b_gpon_usFlow_delAll(void)
{
    int32 ret = RT_ERR_OK;
    uint32 i;

    /* check Init status */
    RT_INIT_CHK(gpon_init);
    
    for(i=0;i<CA8277B_MAX_GEM_FLOW;i++)
    {
        /*Delete DS gem flow*/
        if(TRUE == g8277bGemUsFlowMap[i].isValid)
        {
            if(RT_ERR_OK != dal_rt_ca8277b_gpon_usFlow_del(i))
            {
                ret = RT_ERR_FAILED;
            }
                
        }
    }

    return RT_ERR_OK;

}


/* Function Name:
 *      dal_rt_ca8277b_gpon_dsFlow_set
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
int32 dal_rt_ca8277b_gpon_dsFlow_set(uint32 dsFlowId, const rt_gpon_dsFlow_t *pDsFlow)
{
    unsigned short gemIdx = 0;
    
    CA8277B_LOG_DBG(
        "%s %d flowId:%d gemportId:%d\n",
        __FUNCTION__,__LINE__,
        dsFlowId,
        pDsFlow->gemPortId);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pDsFlow), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((dsFlowId >= CA8277B_MAX_GEM_FLOW), RT_ERR_INPUT);

    /* function body */
    if (RT_ERR_OK != _ca8277b_gpon_aal_dsGem_set(
                                dsFlowId, 
                                pDsFlow, 
                                &gemIdx))
    {
        CA8277B_LOG_ERR("%s %d set ds flow fail\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

        //save the gemport index
    if(RT_ERR_OK != _ca8277b_SetGemFlowMap(GEM_FLOW_DIR_DS, dsFlowId, NULL, gemIdx, pDsFlow->gemPortId))
    {
        CA8277B_LOG_ERR("%s %d set gemflow map fail\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;            
    }
    
    return RT_ERR_OK;
}


/* Function Name:
 *      dal_rt_ca8277b_gpon_dsFlow_get
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
int32 dal_rt_ca8277b_gpon_dsFlow_get(uint32 dsFlowId, rt_gpon_dsFlow_t *pDsFlow)
{
    
    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pDsFlow), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((dsFlowId >= CA8277B_MAX_GEM_FLOW), RT_ERR_INPUT);

    /* function body */
    if(TRUE != g8277bGemDsFlowMap[dsFlowId].isValid)
    {
        //CA8277B_LOG_ERR("%s %d invalid ds flow [%d]\n", __FUNCTION__, __LINE__, dsFlowId);
        return RT_ERR_INPUT;
    }
    if (RT_ERR_OK != _ca8277b_gpon_aal_dsGem_get(
                                dsFlowId, 
                                pDsFlow))
    {
        CA8277B_LOG_ERR("%s %d get ds flow fail\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    
    return RT_ERR_OK;
}


/* Function Name:
 *      dal_rt_ca8277b_gpon_dsFlow_del
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
int32 dal_rt_ca8277b_gpon_dsFlow_del(uint32 dsFlowId)
{

    CA8277B_LOG_DBG(
        "%s %d flowId:%d gemIdx:0x%x\n",
        __FUNCTION__,__LINE__,
        dsFlowId,
        g8277bGemDsFlowMap[dsFlowId].gemIdx);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */    
    RT_PARAM_CHK((dsFlowId >= CA8277B_MAX_GEM_FLOW), RT_ERR_INPUT);

    
    /*Check the DS flow is exist*/
    if(FALSE == g8277bGemDsFlowMap[dsFlowId].isValid)
        return RT_ERR_INPUT;


    if (RT_ERR_OK != _ca8277b_gpon_aal_dsGem_del(dsFlowId))
    {
        CA8277B_LOG_ERR("%s %d del ds flow fail\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if(RT_ERR_OK != _ca8277b_DeleteGemFlowMap(dsFlowId, GEM_FLOW_DIR_DS))
    {
        CA8277B_LOG_ERR("%s %d set us gemflow map fail\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;            
    }
    
    return RT_ERR_OK;
}


/* Function Name:
 *      dal_rt_ca8277b_gpon_dsFlow_delAll
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
int32 dal_rt_ca8277b_gpon_dsFlow_delAll(void)
{
    int ret = RT_ERR_OK;
    unsigned int i;


    /* check Init status */
    RT_INIT_CHK(gpon_init);
    
    for(i=0;i<CA8277B_MAX_GEM_FLOW;i++)
    {
        /*Delete DS gem flow*/
        if(TRUE == g8277bGemDsFlowMap[i].isValid)
        {
            if(RT_ERR_OK != dal_rt_ca8277b_gpon_dsFlow_del(i))
            {
                ret = RT_ERR_FAILED;
            }
        }
    }

    return ret;

}

/* Function Name:
 *      dal_rt_ca8277b_gpon_ponQueue_set
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
int32 dal_rt_ca8277b_gpon_ponQueue_set(
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
    RT_PARAM_CHK((tcontId >= CA8277B_MAX_TCONT), RT_ERR_INPUT);
    RT_PARAM_CHK((tcQueueId >= CA8277B_MAX_TCONT_QUEUE), RT_ERR_INPUT);

    queue.schedulerId = tcontId;
    queue.queueId = tcQueueId;

    queueCfg.cir = (RT_GPON_PON_QUEUE_PIR_UNLIMITED == pQueuecfg->cir)?0:RATE_8KBPS_TO_KBPS(pQueuecfg->cir);
    queueCfg.pir = (RT_GPON_PON_QUEUE_PIR_UNLIMITED == pQueuecfg->pir)?0:RATE_8KBPS_TO_KBPS(pQueuecfg->pir);
    queueCfg.egrssDrop = DISABLED;
    queueCfg.type = (RT_GPON_TCONT_QUEUE_SCHEDULER_SP == pQueuecfg->scheduleType)?STRICT_PRIORITY:WFQ_WRR_PRIORITY;
    queueCfg.weight = (RT_GPON_TCONT_QUEUE_SCHEDULER_SP == pQueuecfg->scheduleType)?0:pQueuecfg->weight;

    ret = dal_ca8277b_ponmac_queue_add(&queue, &queueCfg);

    if(RT_ERR_OK != ret)
    {
        CA8277B_LOG_ERR("%s %d dal_ca8277b_ponmac_queue_add fail \n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;

}


/* Function Name:
 *      dal_rt_ca8277b_gpon_ponQueue_get
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
int32 dal_rt_ca8277b_gpon_ponQueue_get(
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
    RT_PARAM_CHK((tcontId >= CA8277B_MAX_TCONT), RT_ERR_INPUT);
    RT_PARAM_CHK((tcQueueId >= CA8277B_MAX_TCONT_QUEUE), RT_ERR_INPUT);

    queue.schedulerId = tcontId;
    queue.queueId = tcQueueId;

    memset(&queueCfg, 0x0, sizeof(rtk_ponmac_queueCfg_t));

    ret = dal_ca8277b_ponmac_queue_get(&queue, &queueCfg);

    if(RT_ERR_OK != ret)
    {
        //CA8277B_LOG_ERR("%s %d dal_ca8277b_ponmac_queue_get fail \n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    
    pQueuecfg->cir = (CA8277B_METER_RATE_MAX == queueCfg.cir)?RT_GPON_PON_QUEUE_PIR_UNLIMITED:RATE_KBPS_TO_8KBPS(queueCfg.cir);
    pQueuecfg->pir = (CA8277B_METER_RATE_MAX == queueCfg.pir)?RT_GPON_PON_QUEUE_PIR_UNLIMITED:RATE_KBPS_TO_8KBPS(queueCfg.pir);
    pQueuecfg->scheduleType = (STRICT_PRIORITY == queueCfg.type)?RT_GPON_TCONT_QUEUE_SCHEDULER_SP:RT_GPON_TCONT_QUEUE_SCHEDULER_WRR;    
    pQueuecfg->weight = queueCfg.weight;    

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_rt_ca8277b_gpon_ponQueue_del
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
int32 dal_rt_ca8277b_gpon_ponQueue_del(    
    uint32 tcontId, 
    uint32 tcQueueId)
{
    rtk_ponmac_queue_t queue;

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((tcontId >= CA8277B_MAX_TCONT), RT_ERR_INPUT);
    RT_PARAM_CHK((tcQueueId >= CA8277B_MAX_TCONT_QUEUE), RT_ERR_INPUT);

    queue.schedulerId = tcontId;
    queue.queueId = tcQueueId;

    return dal_ca8277b_ponmac_queue_del(&queue);

}



/* Function Name:
 *      dal_rt_ca8277b_gpon_scheInfo_get
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
dal_rt_ca8277b_gpon_scheInfo_get(rt_gpon_schedule_info_t *pScheInfo)
{
    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pScheInfo), RT_ERR_NULL_POINTER);

    pScheInfo->max_flow         = CA8277B_MAX_GEM_FLOW;
    pScheInfo->max_pon_queue    = CA8277B_MAX_PON_QUEUE;
    pScheInfo->max_tcont        = CA8277B_MAX_TCONT;
    pScheInfo->max_tcon_queue   = CA8277B_MAX_TCONT_QUEUE;


    return RT_ERR_OK;
}   



/* Function Name:
 *      dal_rt_ca8277b_gpon_flowCounter_get
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
dal_rt_ca8277b_gpon_flowCounter_get(uint32 flowId, rt_gpon_flow_counter_type_t flowCntType, rt_gpon_flow_counter_t *pFlowCnt)
{
    uint32 internal_gem_idx;
    ca_status_t ret = CA_E_OK;
    aal_xgpon_dsfp_xgem_mib_config_t dsfp_xgem_mib;
    aal_xgpon_usbg_xgem_mib_config_t usbg_xgem_mib;
    
    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((flowId >= CA8277B_MAX_GEM_FLOW), RT_ERR_INPUT);
    RT_PARAM_CHK((flowCntType >= RT_GPON_CNT_TYPE_FLOW_MAX), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pFlowCnt), RT_ERR_NULL_POINTER);

    memset(pFlowCnt, 0x0, sizeof(rt_gpon_flow_counter_t));

    if(CA_PON_MODE == ONU_PON_MAC_GPON)
    {
        aal_gpon_ds_gem_port_mib_t ds_gem;
        aal_gpon_us_gem_port_mib_t us_gem;

        switch(flowCntType) 
        {
            case RT_GPON_CNT_TYPE_FLOW_DS_GEM:
            {
                uint16_t gem, flow;
                GPON_MAC_GPON_MIB_INIT_t mibCfg = {0};

                internal_gem_idx = g8277bGemDsFlowMap[flowId].gemIdx;

                for (flow = 0; flow < CA8277B_MAX_GEM_FLOW; flow++)
                {
                    if(g8277bGemDsFlowMap[flowId].isValid == FALSE)
                        continue;

                    gem = g8277bGemDsFlowMap[flow].gemIdx;

                    if (CA_E_OK != aal_gpon_ds_gem_port_mib_get(SATURN_DEV_ID, gem, 0, &ds_gem))
                    {
                        CA8277B_LOG_ERR("%s:%d::get ONU DS GEM count %d fail",__FUNCTION__,__LINE__,gem);
                        return RT_ERR_FAILED;
                    }
                    g8277bDsGemCnts[gem].dsgem.gem_block += ds_gem.fcnt;
                    g8277bDsGemCnts[gem].dsgem.gem_byte += ds_gem.bcnt;

                    if(gem == internal_gem_idx)
                    {
                        pFlowCnt->dsgem.gem_block = g8277bDsGemCnts[gem].dsgem.gem_block;
                        pFlowCnt->dsgem.gem_byte = g8277bDsGemCnts[gem].dsgem.gem_byte;
                        g8277bDsGemCnts[gem].dsgem.gem_block = 0;
                        g8277bDsGemCnts[gem].dsgem.gem_byte = 0;
                    }
                    g8277bPonDsCnt.frame_cnt += ds_gem.fcnt;
                    g8277bPonDsCnt.byte_cnt += ds_gem.bcnt;
                }
                break;
            }
            case RT_GPON_CNT_TYPE_FLOW_US_GEM:
            {
                if(g8277bGemUsFlowMap[flowId].isValid == TRUE)
                {
                    internal_gem_idx = g8277bGemUsFlowMap[flowId].gemIdx;
    
                    memset(&us_gem, 0x0, sizeof(aal_gpon_us_gem_port_mib_t));
                    ret = aal_gpon_us_gem_port_mib_get(SATURN_DEV_ID, AAL_GPON_PLOAM_MIB_READ_CLR, internal_gem_idx, &us_gem);
                    if(CA_E_OK == ret)
                    {
                        pFlowCnt->usgem.gem_block = us_gem.fcnt + g8277bUsGemCnts[internal_gem_idx].usgem.gem_block;
                        pFlowCnt->usgem.gem_byte = us_gem.bcnt + g8277bUsGemCnts[internal_gem_idx].usgem.gem_byte;
        
                        g8277bPonUsCnt.frame_cnt += us_gem.fcnt;
                        g8277bPonUsCnt.byte_cnt += us_gem.bcnt;
                        g8277bUsGemCnts[internal_gem_idx].usgem.gem_block = 0;
                        g8277bUsGemCnts[internal_gem_idx].usgem.gem_byte = 0;
                    }
                }

                break;
            }
            case RT_GPON_CNT_TYPE_FLOW_DS_ETH:
            case RT_GPON_CNT_TYPE_FLOW_US_ETH:
            default:
                ret = CA_E_ERROR;
                CA8277B_LOG_ERR("%s %d un-support flowCntType:%d\n",__FUNCTION__,__LINE__,flowCntType);
                break;
        }
    }
    else
    {
        switch(flowCntType) 
        {
            case RT_GPON_CNT_TYPE_FLOW_DS_GEM:
            {
                    if(g8277bGemDsFlowMap[flowId].isValid == TRUE)
                    {
                        internal_gem_idx = g8277bGemDsFlowMap[flowId].gemIdx;
        
            memset(&dsfp_xgem_mib, 0x0, sizeof(aal_xgpon_dsfp_xgem_mib_config_t));
                ret = aal_xgpon_dsfp_xgem_mib_get(SATURN_DEV_ID,internal_gem_idx&0xFF,&dsfp_xgem_mib);
                    if(CA_E_OK == ret)
                    {
                        pFlowCnt->dsgem.gem_block = dsfp_xgem_mib.frame_cnt + g8277bDsGemCnts[internal_gem_idx].dsgem.gem_block;
                        pFlowCnt->dsgem.gem_byte = dsfp_xgem_mib.byte_cnt + g8277bDsGemCnts[internal_gem_idx].dsgem.gem_byte;
    
                        g8277bPonDsCnt.frame_cnt += dsfp_xgem_mib.frame_cnt;
                        g8277bPonDsCnt.byte_cnt += dsfp_xgem_mib.byte_cnt;
                        g8277bDsGemCnts[internal_gem_idx].dsgem.gem_block = 0;
                        g8277bDsGemCnts[internal_gem_idx].dsgem.gem_byte = 0;
                    }
                }
                break;
            }
            case RT_GPON_CNT_TYPE_FLOW_US_GEM:
            {
                    if(g8277bGemUsFlowMap[flowId].isValid == TRUE)
                    {
                        internal_gem_idx = g8277bGemUsFlowMap[flowId].gemIdx;
        
            memset(&usbg_xgem_mib, 0x0, sizeof(aal_xgpon_usbg_xgem_mib_config_t));
                    ret = aal_xgpon_usbg_xgem_mib_get(SATURN_DEV_ID,internal_gem_idx&0xFF,&usbg_xgem_mib);
                    if(CA_E_OK == ret)
                    {
                        pFlowCnt->usgem.gem_block = usbg_xgem_mib.frame_cnt + g8277bUsGemCnts[internal_gem_idx].usgem.gem_block;
                        pFlowCnt->usgem.gem_byte = usbg_xgem_mib.byte_cnt + g8277bUsGemCnts[internal_gem_idx].usgem.gem_byte;
    
                        g8277bPonUsCnt.frame_cnt += usbg_xgem_mib.frame_cnt;
                        g8277bPonUsCnt.byte_cnt += usbg_xgem_mib.byte_cnt;
                        g8277bUsGemCnts[internal_gem_idx].usgem.gem_block = 0;
                        g8277bUsGemCnts[internal_gem_idx].usgem.gem_byte = 0;
                    }
                }
                break;
            }
            case RT_GPON_CNT_TYPE_FLOW_DS_ETH:
            case RT_GPON_CNT_TYPE_FLOW_US_ETH:
            default:
                ret = CA_E_ERROR;
                CA8277B_LOG_ERR("%s %d un-support flowCntType:%d\n",__FUNCTION__,__LINE__,flowCntType);
                break;
        }
    }

    if(CA_E_OK == ret)
        return RT_ERR_OK;
    else
        return RT_ERR_FAILED;
    
}   /* end of dal_rt_ca8277b_gpon_flowCounter_get */


/* Function Name:
 *      dal_rt_ca8277b_gpon_pmCounter_get
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
dal_rt_ca8277b_gpon_pmCounter_get(rt_gpon_pm_type_t pmCntType, rt_gpon_pm_counter_t *pPmCnt)
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
            ret = _ca8277b_gpon_pm_phyLos_get(pPmCnt);
        break;
        case RT_GPON_PM_TYPE_FEC:
            ret = _ca8277b_gpon_pm_fec_get(pPmCnt);
        break;
        case RT_GPON_PM_TYPE_XGEM:
            ret = _ca8277b_gpon_pm_xgem_get(pPmCnt);
        break;
        case RT_GPON_PM_TYPE_PLOAM_PM1:
            ret = _ca8277b_gpon_pm_ploam1_get(pPmCnt);
        break;
        case RT_GPON_PM_TYPE_PLOAM_PM2:
            ret = _ca8277b_gpon_pm_ploam2_get(pPmCnt);
        break;
        case RT_GPON_PM_TYPE_PLOAM_PM3:
            ret = _ca8277b_gpon_pm_ploam3_get(pPmCnt);
        break;
        case RT_GPON_PM_TYPE_CHAN_TUNING_PM1:
            ret = _ca8277b_gpon_pm_chan_tuning1_get(pPmCnt);
        break;
        case RT_GPON_PM_TYPE_CHAN_TUNING_PM2:
            ret = _ca8277b_gpon_pm_chan_tuning2_get(pPmCnt);
        break;
        case RT_GPON_PM_TYPE_CHAN_TUNING_PM3:
            ret = _ca8277b_gpon_pm_chan_tuning3_get(pPmCnt);
        break;
        case RT_GPON_PM_TYPE_OMCI:
            ret = _ca8277b_gpon_pm_omci_get(pPmCnt);
        break;
        default:
        break;
    }

    if(CA_E_OK == ret)
        return RT_ERR_OK;
    else
        return RT_ERR_FAILED;
    
}   /* end of dal_rt_ca8277b_gpon_flowCounter_get */


/* Function Name:
 *      dal_rt_ca8277b_gpon_ponTag_get
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
dal_rt_ca8277b_gpon_ponTag_get(rt_gpon_ponTag_t *pPonTag)
{

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPonTag), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((ONU_PON_MAC_GPON == gPonMode), RT_ERR_FAILED);


    if (CA_E_OK != ca_gpon_port_pon_tag_get(SATURN_DEV_ID, 8, &pPonTag->tagData[0]))
    {
        CA8277B_LOG_ERR("%s %d get pontag fail\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;

}

/* Function Name:
 *      dal_rt_ca8277b_gpon_msk_set
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
dal_rt_ca8277b_gpon_msk_set(rt_gpon_msk_t *pMsk)
{
    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pMsk), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((ONU_PON_MAC_GPON == gPonMode), RT_ERR_FAILED);

    if (CA_E_OK != ca_gpon_port_msk_update(SATURN_DEV_ID, &pMsk->mskData[0]))
    {
        CA8277B_LOG_ERR("%s %d set msk fail\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
    
}

/* Function Name:
 *      dal_rt_ca8277b_gpon_omci_mic_generate
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
dal_rt_ca8277b_gpon_omci_mic_generate(uint32 dir, uint8 *pMsg, uint32 msgLen, uint32 *mic)
{
    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK(((dir < RT_GPON_OMCI_MIC_DIR_DS) | (dir > RT_GPON_OMCI_MIC_DIR_US)), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pMsg), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == mic), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((ONU_PON_MAC_GPON == gPonMode), RT_ERR_FAILED);

    if (CA_E_OK != ca_generate_omci_mic(SATURN_DEV_ID, dir, pMsg, msgLen, 4, mic))

    {
        CA8277B_LOG_ERR("%s %d generate OMCI MIC fail\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rt_ca8277b_gpon_mcKey_set
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
dal_rt_ca8277b_gpon_mcKey_set(rt_gpon_mc_key_t *pMcKey)
{
    uint32 keyIdx = 0;

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK(((pMcKey->idx < RT_GPON_MC_KEY_IDX0) || (pMcKey->idx >= RT_GPON_MC_KEY_IDX_MAX)), RT_ERR_INPUT);
    RT_PARAM_CHK((pMcKey->action >= RT_GPON_MC_KEY_ACTION_MAX), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pMcKey), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((ONU_PON_MAC_GPON == gPonMode), RT_ERR_FAILED);

    keyIdx = (pMcKey->idx == RT_GPON_MC_KEY_IDX0) ? CA_NGP2_KEY_IDX_0 : CA_NGP2_KEY_IDX_1;
    if (CA_E_OK != ca_ngp2_port_mc_encryption_key_set(SATURN_DEV_ID,
                                               SATURN_ANI_PORT_ID,
                                               keyIdx,
                                               pMcKey->action,
                                               16,
                                               pMcKey->key))
    {
        CA8277B_LOG_ERR("%s %d set Multicast Key fail\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
    
}


/* Function Name:
 *      dal_rt_ca8277b_gpon_attribute_set
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
dal_rt_ca8277b_gpon_attribute_set(rt_gpon_attribute_type_t attributeType, rt_gpon_attribute_t *pAttributeValue)
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
            if(CA_PON_MODE == ONU_PON_MAC_GPON)
            {/* GPON always enable dbru*/
                /*do nothing*/
            }
            else
            {
                memset(&mask, 0, sizeof(aal_xgpon_usbg_xgtc_ctrl_config_msk_t));
                mask.s.dbru_en = 1;
                xgtc_ctrl.dbru_en = pAttributeValue->dbruStatus;
                if(CA_E_OK != (ret = aal_xgpon_usbg_xgtc_ctrl_set(SATURN_DEV_ID, mask, &xgtc_ctrl)))
                {
                    CA8277B_LOG_ERR("%s %d : aal_xgpon_usbg_xgtc_ctrl_set Error!! ret=%x",__FUNCTION__,__LINE__,ret);
                    ret = RT_ERR_FAILED;
                }               
            }
            break;
        case RT_GPON_ATTRIBUTE_TYPE_GEM_BLOCK_LENGTH:
            if(CA_PON_MODE == ONU_PON_MAC_GPON)
            {/*support 48, 64, 128, 256, 512, 1024, 1536, 2048 bytes*/
                aal_gpon_mac_cfg_t gpon_cfg;
                aal_gpon_mac_cfg_msk_t gpon_mask;
                memset(&gpon_mask, 0x0, sizeof(aal_gpon_mac_cfg_msk_t));
                switch(pAttributeValue->gemBlockLength)
                {
                    case 48:
                        gpon_cfg.dbru_blk_size = 0;
                    break;
                    case 64:
                        gpon_cfg.dbru_blk_size = 2;
                    break;
                    case 128:
                        gpon_cfg.dbru_blk_size = 3;
                    break;
                    case 256:
                        gpon_cfg.dbru_blk_size = 4;
                    break;
                    case 512:
                        gpon_cfg.dbru_blk_size = 5;
                    break;
                    case 1024:
                        gpon_cfg.dbru_blk_size = 6;
                    break;
                    case 1536:
                        gpon_cfg.dbru_blk_size = 1;
                    break;
                    case 2048:
                        gpon_cfg.dbru_blk_size = 7;
                    break;
                    default:
                        ret = RT_ERR_INPUT;
                    break;
                }
                gpon_mask.s.dbru_blk_size = 1;
                if(CA_E_OK != (ret = aal_gpon_mac_cfg_set(SATURN_DEV_ID, gpon_mask, &gpon_cfg)))
                {
                    CA8277B_LOG_ERR("%s %d : aal_xgpon_usbg_xgtc_ctrl_set Error!! ret=%x",__FUNCTION__,__LINE__,ret);
                    ret = RT_ERR_FAILED;
                }
            }
            else
            {/*XG-PON/XGS-PON/NG-PON is fix to 1 bytes*/
                /*do nothing*/
            }
            break;
        case RT_GPON_ATTRIBUTE_TYPE_SD_THRESHOLD:
            if(CA_PON_MODE == ONU_PON_MAC_GPON)
            {
                aal_gpon_sf_degrd_alrm_thrsd_cfg_t th_cfg;
                aal_gpon_sf_degrd_alrm_thrsd_cfg_msk_t th_msk;
                memset(&th_msk, 0x0, sizeof(aal_gpon_sf_degrd_alrm_thrsd_cfg_msk_t));
                if((pAttributeValue->sdThreshold < 4) || (pAttributeValue->sdThreshold > 10)) /*by 984.4 spec*/
                {
                    return RT_ERR_INPUT;
                }
                th_cfg.degrade_threshold = pAttributeValue->sdThreshold - 4;
                th_cfg.degrade_remove_threshold = pAttributeValue->sdThreshold - 5;
                th_msk.s.degrade_threshold = 1;
                th_msk.s.degrade_remove_threshold = 1;
                if (CA_E_OK != (ret = aal_gpon_sf_degrd_alrm_thrsd_set(SATURN_DEV_ID, th_msk, &th_cfg)))
                {
                    CA8277B_LOG_ERR("%s %d : aal_gpon_sf_degrd_alrm_thrsd_set Error!! ret=%x",__FUNCTION__,__LINE__,ret);;
                    ret = RT_ERR_FAILED;
                }
                pAttributeValue->sdThreshold = th_cfg.degrade_threshold;
            }
            else
            {/*XGPON do not support*/
                ret = RT_ERR_CHIP_NOT_SUPPORTED;
            }
            break;
        case RT_GPON_ATTRIBUTE_TYPE_SF_THRESHOLD:
            if(CA_PON_MODE == ONU_PON_MAC_GPON)
            {
                aal_gpon_sf_degrd_alrm_thrsd_cfg_t th_cfg;
                aal_gpon_sf_degrd_alrm_thrsd_cfg_msk_t th_msk;
                memset(&th_msk, 0x0, sizeof(aal_gpon_sf_degrd_alrm_thrsd_cfg_msk_t));
                if((pAttributeValue->sdThreshold < 3) || (pAttributeValue->sdThreshold > 9)) /*by 984.4 spec*/
                {
                    return RT_ERR_INPUT;
                }
                th_cfg.fail_threshold = pAttributeValue->sdThreshold - 3;
                th_cfg.fail_remove_threshold = pAttributeValue->sdThreshold - 4;
                th_msk.s.fail_threshold = 1;
                th_msk.s.fail_remove_threshold = 1;
                if (CA_E_OK != (ret = aal_gpon_sf_degrd_alrm_thrsd_set(SATURN_DEV_ID,th_msk,&th_cfg)))
                {
                    CA8277B_LOG_ERR("%s %d : aal_gpon_sf_degrd_alrm_thrsd_set Error!! ret=%x",__FUNCTION__,__LINE__,ret);
                    ret = RT_ERR_FAILED;
                }
                pAttributeValue->sfThreshold = th_cfg.fail_threshold;
            }
            else
            {/*XGPON do not support*/
                ret = RT_ERR_CHIP_NOT_SUPPORTED;
            }
            break;
        default:
            ret = RT_ERR_INPUT;
            break;
    }

    return ret;
    
}

/* Function Name:
 *      dal_rt_ca8277b_gpon_attribute_get
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
dal_rt_ca8277b_gpon_attribute_get(rt_gpon_attribute_type_t attributeType, rt_gpon_attribute_t *pAttributeValue)
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
            if(CA_PON_MODE == ONU_PON_MAC_GPON)
            {/* GPON always enable dbru*/
                pAttributeValue->dbruStatus = 1;
            }
            else
            {
                if(CA_E_OK != (ret = aal_xgpon_usbg_xgtc_ctrl_get(SATURN_DEV_ID, &xgtc_ctrl)))
                {
                    CA8277B_LOG_ERR("%s %d : aal_xgpon_usbg_xgtc_ctrl_get Error!! ret=%x",__FUNCTION__,__LINE__,ret);
                    return RT_ERR_FAILED;
                }
                pAttributeValue->dbruStatus = xgtc_ctrl.dbru_en;
            }
            break;
        case RT_GPON_ATTRIBUTE_TYPE_GEM_BLOCK_LENGTH:
            if(CA_PON_MODE == ONU_PON_MAC_GPON)
            {/*support 48, 64, 128, 256, 512, 1024, 1536, 2048 bytes*/
                aal_gpon_mac_cfg_t gpon_cfg;
                if(CA_E_OK != (ret = aal_gpon_mac_cfg_get(SATURN_DEV_ID, &gpon_cfg)))
                {
                    CA8277B_LOG_ERR("%s %d : aal_gpon_mac_cfg_get Error!! ret=%x",__FUNCTION__,__LINE__,ret);
                    return RT_ERR_FAILED;
                }
                switch(gpon_cfg.dbru_blk_size)
                {
                    case 0:
                        pAttributeValue->gemBlockLength = 48;
                    break;
                    case 1:
                        pAttributeValue->gemBlockLength = 1536;
                    break;
                    case 2:
                        pAttributeValue->gemBlockLength = 64;
                    break;
                    case 3:
                        pAttributeValue->gemBlockLength = 128;
                    break;
                    case 4:
                        pAttributeValue->gemBlockLength = 256;
                    break;
                    case 5:
                        pAttributeValue->gemBlockLength = 512;
                    break;
                    case 6:
                        pAttributeValue->gemBlockLength = 1024;
                    break;
                    case 7:
                        pAttributeValue->gemBlockLength = 2048;
                    break;
                    default:
                        ret = RT_ERR_FAILED;
                    break;
                }
            }
            else
            {/*XG-PON/XGS-PON/NG-PON is fix to 1 bytes*/
                pAttributeValue->gemBlockLength = 1;
            }
            break;
        case RT_GPON_ATTRIBUTE_TYPE_SD_THRESHOLD:
            if(CA_PON_MODE == ONU_PON_MAC_GPON)
            {
                aal_gpon_sf_degrd_alrm_thrsd_cfg_t th_cfg;
                if (CA_E_OK != (ret = aal_gpon_sf_degrd_alrm_thrsd_get(SATURN_DEV_ID, &th_cfg)))
                {
                    CA8277B_LOG_ERR("%s %d : aal_gpon_sf_degrd_alrm_thrsd_get Error!!, ret=%x",__FUNCTION__,__LINE__,ret);
                    ret = RT_ERR_FAILED;
                }
                pAttributeValue->sdThreshold = th_cfg.degrade_threshold;
            }
            else
            {/*XGPON do not support*/
                ret = RT_ERR_CHIP_NOT_SUPPORTED;
            }
            break;
        case RT_GPON_ATTRIBUTE_TYPE_SF_THRESHOLD:
            if(CA_PON_MODE == ONU_PON_MAC_GPON)
            {
                aal_gpon_sf_degrd_alrm_thrsd_cfg_t th_cfg;
                if (CA_E_OK != (ret = aal_gpon_sf_degrd_alrm_thrsd_get(SATURN_DEV_ID, &th_cfg)))
                {
                    CA8277B_LOG_ERR("%s %d : aal_gpon_sf_degrd_alrm_thrsd_get Error!! ret=%x",__FUNCTION__,__LINE__,ret);
                    ret = RT_ERR_FAILED;
                }
                pAttributeValue->sfThreshold = th_cfg.fail_threshold;
            }
            else
            {/*XGPON do not support*/
                ret = RT_ERR_CHIP_NOT_SUPPORTED;
            }
            break;
        default:
            ret = RT_ERR_INPUT;
            break;
    }

    return ret;
}

 /* Function Name:
 *      dal_rt_ca8277b_gpon_usFlow_phyData_get
 * Description:
 *      Get the physical data of upstream flow
 * Input:
 *      usFlowId    - upstream flow id
 *      pTcontId    - pointer to the physical T-CONT ID value
 *      pTcQueueId  - pointer to the physical T-CONT Queue ID value
 *      pGemIdx     - pointer to the physical GEM index value
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
int32 dal_rt_ca8277b_gpon_usFlow_phyData_get
(uint32 usFlowId, uint32 *pTcontId, uint32 *pTcQueueId, uint32 *pGemIdx)
{
    ca_boolean_t isLanUseDq = 0;
    ca_boolean_t isTconUseDq = 0;


    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pTcontId), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pTcQueueId), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pGemIdx), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((usFlowId >= CA8277B_MAX_GEM_FLOW), RT_ERR_INPUT);
    
    /* function body */
    if(TRUE != g8277bGemUsFlowMap[usFlowId].isValid) 
    {        
        CA8277B_LOG_ERR("%s %d invalid us flow [%d]\n", __FUNCTION__, __LINE__, usFlowId);
        return RT_ERR_INPUT;
    }
    
    if(g8277bGemUsFlowMap[usFlowId].tcontId < CFG_ID_RTK_PHY_PORT_TO_DEEP_Q_LEN)
    {
        if(CA_E_OK == aal_port_physical_port_to_dq_get(0, g8277bGemUsFlowMap[usFlowId].tcontId, &isLanUseDq))
        {
            /*Only LAN(0~7) or T-CONT(0~7) can use deep queue*/
            if(0 == isLanUseDq)
            {
                isTconUseDq = 1;
            }

        }

    }
    else
    {
        isTconUseDq = 0;

    }
    
    *pTcontId        = TCONID_TO_LDPID(g8277bGemUsFlowMap[usFlowId].tcontId, isTconUseDq);
    *pTcQueueId      = g8277bGemUsFlowMap[usFlowId].tcQueueId;
    *pGemIdx         = g8277bGemUsFlowMap[usFlowId].gemIdx;   


    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rt_ca8277b_gpon_omcc_get
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
dal_rt_ca8277b_gpon_omcc_get(rt_gpon_omcc_t *pOmcc)
{
    int32 ret = RT_ERR_OK;

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pOmcc), RT_ERR_NULL_POINTER);

    ret = _ca8277b_gpon_omcc_get(pOmcc);
    if (RT_ERR_OK != ret)
    {
        CA8277B_LOG_ERR("%s:%d::Error OMCC get fail",__FUNCTION__,__LINE__);
    }

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
dal_rt_ca8277b_gpon_omci_mirror_set(rt_enable_t enable, rt_port_t mirroringPort)
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

/* Function Name:
 *      dal_rt_ca8277b_gpon_fec_get
 * Description:
 *      Get gpon fec status function.
 * Input:
 *
 * Output:
 *      pFecStatus               -   fec status information
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32 
dal_rt_ca8277b_gpon_fec_get(rt_gpon_fec_status_t *pFecStatus)
{
    int32 ret = RT_ERR_OK;
    uint32 fifo_id = 0;
    uint32 grant_id = 0;
    uint32 addr = 0;
    uint32 tcontId = 0;
    uint32 chkCnt = 0;
    ca_boolean_t gponUsFec;
    ca_boolean_t gponDsFec;
    aal_xgpon_dsfp_oc_body_config_t oc_body;
    aal_xgpon_bwmp_grt_ff_cfg_t grf_ff_cfg;
	
    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pFecStatus), RT_ERR_NULL_POINTER);

    pFecStatus->us_fec_status = 0;
    pFecStatus->ds_fec_status = 0;

    if(CA_PON_MODE == ONU_PON_MAC_GPON)
    {
        //DS FEC
        if (CA_E_OK != (ret = aal_gpon_fec_get(SATURN_DEV_ID, &gponUsFec, &gponDsFec)))
        {
            CA8277B_LOG_ERR("%s %d : aal_gpon_fec_get Error!!, ret=%x",__FUNCTION__,__LINE__,ret);
            return RT_ERR_FAILED;
        }
        pFecStatus->us_fec_status = gponUsFec;
        pFecStatus->ds_fec_status = gponDsFec;
    }
    else
    {
        //DS FEC
        if(CA_PON_MODE == ONU_PON_MAC_NGPON2)
        {
            if (CA_E_OK != (ret = aal_xgpon_dsfp_oc_body_get(SATURN_DEV_ID, &oc_body)))
            {
                CA8277B_LOG_ERR("%s %d : aal_xgpon_dsfp_oc_body_get Error!!, ret=%x",__FUNCTION__,__LINE__,ret);
                return RT_ERR_FAILED;
            }
            pFecStatus->ds_fec_status = oc_body.DS_FEC_flag1;
        }
        else
        {
            //XG-PON && XGS-PON can't disable DS FEC, may not able get info from OC body
            pFecStatus->ds_fec_status = 1;
        }

        //US FEC
        for(fifo_id = 0; fifo_id < __XGPON_MAX_FIFOS, chkCnt<3 ; fifo_id++) {
            for(grant_id = 0; grant_id < __XGPON_MAX_GRANTS, chkCnt<3; grant_id++) {
                addr = (fifo_id << 5) | grant_id;
                if (CA_E_OK != (ret = aal_xgpon_bwmp_grt_ff_ctrl_cfg_get(SATURN_DEV_ID,addr, 0,&grf_ff_cfg)))
                {
                    CA8277B_LOG_ERR("%s %d : aal_xgpon_bwmp_grt_ff_ctrl_cfg_get Error!!, ret=%x",__FUNCTION__,__LINE__,ret);
                    return RT_ERR_FAILED;
                }
                if(grf_ff_cfg.tcont_id == tcontId)
                {
                    chkCnt++;
                    if(grf_ff_cfg.fecu){
                        pFecStatus->us_fec_status = 1;
                        chkCnt = 3;
                    }
                }
            }
        }
    }

    return RT_ERR_OK;   
}
