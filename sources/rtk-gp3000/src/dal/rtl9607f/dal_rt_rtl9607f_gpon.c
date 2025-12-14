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
#include <dal/rtl9607f/dal_rtl9607f.h>
#include <dal/rtl9607f/dal_rt_rtl9607f_gpon.h>
#include <dal/rtl9607f/dal_rtl9607f_l2.h>
#include <dal/rtl9607f/dal_rtl9607f_stat.h>
#include <dal/rtl9607f/dal_rtl9607f_ponmac.h>
#include <dal/rtl9607f/dal_rt_rtl9607f_ponmisc.h>

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
#include "ni-drv-07f/ca_ni.h"
#include "ca-ne-rtk/ca_ext.h"
#include <ca_event.h>
#include <osal_spinlock.h>

//#include "aal/include/aal_gpon.h"
//#include "scfg.h"

extern ca_uint32_t aal_init_pon;
extern ca_status_t ca_pon_init(ca_device_id_t device_id);
extern ca_status_t ca_pon_exit(ca_device_id_t device_id);


static ca_uint32_t gPonMode;
uint32 gpon_init = {INIT_NOT_COMPLETED};
static rt_enable_t  gOmciTrapEnable = DISABLED; //Default disable trap OMCI packet
static rt_enable_t  gOmciMirrorEnable = DISABLED; //Default disable mirror OMCI packet
static rt_port_t gMirroringPort;
extern aal_gpon_port_cfg_t g_gpon_basic_info;
extern xgpon_basic_info_t g_xgpn_basic_info;
extern ca_uint8_t g_admin_state;
extern uint32 orig_gpon_init;

extern ca_uint g_aal_gpon_flow_lock;

static rt_gpon_omci_rx_callback gOmciRxCallback = NULL;

int nic_register_rxhook(int portmask,int priority,p2rfunc_t rx);

static rt_gpon_ploam_rx_callback g_gpon_ploam_rx = NULL;

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
} dal_rtl9607f_usFlowMap_t;

typedef struct {
    unsigned char   isValid;
    unsigned short  gemIdx;
    unsigned short  gemPortId;
    unsigned char   loopEn;
    unsigned char   loopLspid;
} dal_rtl9607f_dsFlowMap_t;


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
} dal_9607f_voq_tbl_t;

typedef struct {
    ca_uint32_t frame_cnt;
    ca_uint64_t byte_cnt;
    ca_uint32_t packet_cnt;
} dal_mib_t;

typedef struct {
    uint16              phyTcontId; // Physical T-CONT ID
    uint32              allocId;   //T-CONT allodID
    uint32              valid;     //T-CONT valid bit
} dal_rtl9607f_tcont_info_t;



/****************************************************************/
/* Macro Declaration                                            */
/****************************************************************/



#define RATE_8KBPS_TO_KBPS(_8kbps)  ((_8kbps)*8)
#define RATE_KBPS_TO_8KBPS(_kbps)  ((_kbps)/8)
#define RATE_KBPS_TO_VOQ_RATE_M(_kbps)  ((_kbps)/1000)
#define RATE_KBPS_TO_VOQ_RATE_K(_kbps)  ((_kbps)%1000)


/*CA 9607F GEM flow 0~7 for OMCC use */
#define LOGIC_FLOW_TO_RTL9607F_FLOW(_gemFlow) ((_gemFlow)+8)

#define OMCC_TCONT_IDX  (0)
#define IS_OMCC_TCONT(_tcontId) (_tcontId==OMCC_TCONT_IDX)

#ifdef CONFIG_CORTINA_BOARD_FPGA
#define RTL9607F_LOG_ERR(fmt, arg...)  printk("[rtl9607f]:\t" fmt "\n",## arg)
#define RTL9607F_LOG_DBG(fmt, arg...)  printk("[rtl9607f]:\t" fmt "\n",## arg) 
#else
#define RTL9607F_LOG_ERR(fmt, arg...)  printk(KERN_ERR "[rtl9607f]:\t" fmt "\n",## arg)
#define RTL9607F_LOG_DBG(fmt, arg...)  if(ca_rtk_debug&0x00000400) printk(KERN_DEBUG "[rtl9607f]:\t" fmt "\n",## arg)
#endif








#define RTL9607F_GPON_MAX_TCONT            (30)
#define RTL9607F_GPON_MAX_TCONT_QUEUE      (8)
#define RTL9607F_GPON_MAX_PON_QUEUE        (248)
#define RTL9607F_GPON_MAX_GEM_FLOW         (248)


#define RTL9607F_XGPON_MAX_TCONT          (31)
#define RTL9607F_XGPON_MAX_TCONT_QUEUE    (8)
#define RTL9607F_XGPON_MAX_PON_QUEUE      (248)
#define RTL9607F_XGPON_MAX_GEM_FLOW       (248)


#define RTL9607F_MAX_TCONT (((gPonMode == ONU_PON_MAC_GPON) || (gPonMode == ONU_PON_MAC_GPON_BI2G5)) ? RTL9607F_GPON_MAX_TCONT:RTL9607F_XGPON_MAX_TCONT)
#define RTL9607F_MAX_TCONT_QUEUE (((gPonMode == ONU_PON_MAC_GPON) || (gPonMode == ONU_PON_MAC_GPON_BI2G5)) ? RTL9607F_GPON_MAX_TCONT_QUEUE:RTL9607F_XGPON_MAX_TCONT_QUEUE)
#define RTL9607F_MAX_PON_QUEUE (((gPonMode == ONU_PON_MAC_GPON) || (gPonMode == ONU_PON_MAC_GPON_BI2G5)) ? RTL9607F_GPON_MAX_PON_QUEUE:RTL9607F_XGPON_MAX_PON_QUEUE)
#define RTL9607F_MAX_GEM_FLOW (((gPonMode == ONU_PON_MAC_GPON) || (gPonMode == ONU_PON_MAC_GPON_BI2G5)) ? RTL9607F_GPON_MAX_GEM_FLOW:RTL9607F_XGPON_MAX_GEM_FLOW)


//Tcont ID 0 for OMCC channel use
#define LOGIC_TCONID_TO_RTL9607F_TCONID(_logicTcontId) ((_logicTcontId == RT_GPON_OMCC_TCONT_ID)?OMCC_TCONT_IDX:g9607fTcontTbl[_logicTcontId].phyTcontId)



#define RTL9607F_TCONID_TO_LDPID(_tcontId) ((_tcontId)+0x20)




/****************************************************************/
/* Data Declaration                                             */
/****************************************************************/
static dal_rtl9607f_usFlowMap_t *g9607fGemUsFlowMap;
static dal_rtl9607f_dsFlowMap_t *g9607fGemDsFlowMap;
static dal_rtl9607f_tcont_info_t *g9607fTcontTbl;


/*
 * Flow Performance Counter
 */
typedef struct dal_gpon_us_flow_counter_s{
    rt_gpon_usFlow_gem_counter_t usgem;
    rt_gpon_usFlow_eth_counter_t useth;
}dal_gpon_us_flow_counter_t;

typedef struct dal_gpon_ds_flow_counter_s{
    rt_gpon_dsFlow_gem_counter_t dsgem;
    rt_gpon_dsFlow_eth_counter_t dseth;
}dal_gpon_ds_flow_counter_t;

static dal_gpon_us_flow_counter_t g9607fUsGemCount[AAL_XGPON_GEM_PORT_NUM];
static dal_gpon_ds_flow_counter_t g9607fDsGemCount[AAL_XGPON_GEM_PORT_NUM];
static rt_gpon_pm_counter_omci_t g9607fOmciCount;


/*Defien local function*/
static int32 
_dal_rtl9607f_gpon_db_init(void)
{
    unsigned short gemFlow,tcontId;
    int ret;

    g9607fGemUsFlowMap = kmalloc(sizeof(dal_rtl9607f_usFlowMap_t) * RTL9607F_MAX_GEM_FLOW, GFP_KERNEL);
    if (!g9607fGemUsFlowMap)
    {        
        RTL9607F_LOG_ERR("%s %d malloc usflowMap db error",
            __FUNCTION__, __LINE__);
        RT_ERR(ret, (MOD_DAL|MOD_GPON), "");
        return RT_ERR_FAILED;


    }

    for(gemFlow = 0;gemFlow<RTL9607F_MAX_GEM_FLOW;gemFlow++)
    {
        g9607fGemUsFlowMap[gemFlow].isValid     = FALSE;
        g9607fGemUsFlowMap[gemFlow].tcontId     = 0x0;
        g9607fGemUsFlowMap[gemFlow].tcQueueId   = 0x0;
        g9607fGemUsFlowMap[gemFlow].gemIdx      = 0x0;
        g9607fGemUsFlowMap[gemFlow].aesEn       = 0x0;
    }


    g9607fGemDsFlowMap = kmalloc(sizeof(dal_rtl9607f_dsFlowMap_t) * RTL9607F_MAX_GEM_FLOW, GFP_KERNEL);
    if (!g9607fGemDsFlowMap)
    {        
        RTL9607F_LOG_ERR("%s %d malloc dsflowMap db error",
            __FUNCTION__, __LINE__);
        RT_ERR(ret, (MOD_DAL|MOD_GPON), "");
        return RT_ERR_FAILED;


    }

    for(gemFlow = 0;gemFlow<RTL9607F_MAX_GEM_FLOW;gemFlow++)
    {
        g9607fGemDsFlowMap[gemFlow].isValid     = FALSE;
        g9607fGemDsFlowMap[gemFlow].gemIdx      = 0x0;
        g9607fGemDsFlowMap[gemFlow].gemPortId   = 0x0;
        g9607fGemDsFlowMap[gemFlow].loopEn      = 0x0;
        g9607fGemDsFlowMap[gemFlow].loopLspid   = 0x0;
    }


    g9607fTcontTbl = kmalloc(sizeof(dal_rtl9607f_tcont_info_t) * RTL9607F_MAX_TCONT, GFP_KERNEL);
    if (!g9607fTcontTbl)
    {        
        RTL9607F_LOG_ERR("%s %d malloc tcont db error",
            __FUNCTION__, __LINE__);
        RT_ERR(ret, (MOD_DAL|MOD_GPON), "");
        return RT_ERR_FAILED;


    }

    for(tcontId=0;tcontId<RTL9607F_MAX_TCONT;tcontId++)
    {
        g9607fTcontTbl[tcontId].allocId = TCONT_ALLOC_ID_984_987_RESERVED;
        g9607fTcontTbl[tcontId].valid   = 0;
        if((ret = dal_rt_rtl9607f_ponmisc_tcont_llid_logic2phy_get(tcontId, &g9607fTcontTbl[tcontId].phyTcontId)) != RT_ERR_OK)
        {
            printk("%s:%d, ret = %d\n",__FUNCTION__, __LINE__, ret);
        }

        RTL9607F_LOG_DBG("%s %d tcontId[%d].phyTcontId=%d",
            __FUNCTION__, __LINE__,tcontId, g9607fTcontTbl[tcontId].phyTcontId);

    }

    return RT_ERR_OK;
}

#if 0
static int 
_AssignNonUsedTcontId(unsigned int allocId, unsigned int *pTcontId)
{
    unsigned int i;
    for(i=0;i<RTL9607F_MAX_TCONT;i++)
    {
        if(g9607fTcontTbl[i].allocId == allocId)
        {
            *pTcontId = i;
            return RT_ERR_OK;
        }
    }

    for(i=0;i<RTL9607F_MAX_TCONT;i++)
    {
        if(g9607fTcontTbl[i].allocId == TCONT_ALLOC_ID_984_987_RESERVED)
        {
            g9607fTcontTbl[i].allocId = allocId;
            *pTcontId = i;
            return RT_ERR_OK;
        }
    }
    return RT_ERR_FAILED;
}
#endif
static int 
_GetTcontIdByAllocId(unsigned int allocId, unsigned int *pTcontId)
{
    unsigned int i;
       
    for(i=0;i<RTL9607F_MAX_TCONT;i++)
    {
        if((g9607fTcontTbl[i].valid == 1) && (g9607fTcontTbl[i].allocId == allocId))
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
    if((g9607fTcontTbl[tcontId].valid == 1) && (TCONT_ALLOC_ID_984_987_RESERVED != g9607fTcontTbl[tcontId].allocId))
    {
        *pAllocId = g9607fTcontTbl[tcontId].allocId;
        return RT_ERR_OK;
    }    
    
    return RT_ERR_FAILED;
}

#if 0
static int 
_GetNonUsedTcontId(unsigned int *pTcontId)
{
    unsigned int i;
   
    for(i=0;i<RTL9607F_MAX_TCONT;i++)
    {
        if(TCONT_ALLOC_ID_984_987_RESERVED == g9607fTcontTbl[i].allocId)
        {
            *pTcontId = i;
            return RT_ERR_OK;
        }

    }
    return RT_ERR_FAILED;
}
#else
static int 
_GetNonUsedTcontId(unsigned int allocId, unsigned int *pTcontId)
{
    unsigned int i;
    unsigned int first_entry = TCONT_ALLOC_ID_984_987_RESERVED;
    unsigned int first_empty_entry = TCONT_ALLOC_ID_984_987_RESERVED;
   
    for(i=0;i<RTL9607F_MAX_TCONT;i++)
    {
        if((g9607fTcontTbl[i].valid == 0) && 
           (first_entry == TCONT_ALLOC_ID_984_987_RESERVED))
        {
            first_entry = i;
        }

        if((g9607fTcontTbl[i].valid == 0) && 
           (g9607fTcontTbl[i].allocId == TCONT_ALLOC_ID_984_987_RESERVED) && 
           (first_empty_entry == TCONT_ALLOC_ID_984_987_RESERVED))
        {
            first_empty_entry = i;
        }

        if((g9607fTcontTbl[i].valid == 0) && (allocId == g9607fTcontTbl[i].allocId))
        {
            *pTcontId = i;
            return RT_ERR_OK;
        }

    }

    if(first_empty_entry != TCONT_ALLOC_ID_984_987_RESERVED)
    {
        *pTcontId = first_empty_entry;
        return RT_ERR_OK;
    }

    if(first_entry != TCONT_ALLOC_ID_984_987_RESERVED)
    {
        *pTcontId = first_entry;
        return RT_ERR_OK;
    }

    return RT_ERR_FAILED;
}
#endif

static int 
_DelTcontId(unsigned int tcontId)
{
    RT_PARAM_CHK((tcontId >= RTL9607F_MAX_TCONT), RT_ERR_INPUT);
    
    //g9607fTcontTbl[tcontId].allocId = TCONT_ALLOC_ID_984_987_RESERVED;
    g9607fTcontTbl[tcontId].valid = 0;
        
    return RT_ERR_OK;
}

static unsigned char 
__tcont_tbl_is_empty (unsigned short ignoreGemFlowId)
{
    unsigned short gemFlowId;
    unsigned int tcontId = g9607fGemUsFlowMap[ignoreGemFlowId].tcontId;
    
    if(OMCC_TCONT_IDX == tcontId)    
    {
        return (FALSE);                
    }
    
    for (gemFlowId = 0; gemFlowId < RTL9607F_MAX_GEM_FLOW; gemFlowId++) 
    {
        if(ignoreGemFlowId == gemFlowId)
            continue;
        
        if (g9607fGemUsFlowMap[gemFlowId].isValid) 
        {
            if(g9607fGemUsFlowMap[gemFlowId].tcontId == tcontId)
                return FALSE;
        }
    }

    return (TRUE);
}

static unsigned char 
__gemport_tbl_is_empty (unsigned short ignoreGemFlowId)
{
    unsigned short gemFlowId;
    unsigned int gemPortId = g9607fGemUsFlowMap[ignoreGemFlowId].gemPortId;
    

    for (gemFlowId = 0; gemFlowId < RTL9607F_MAX_GEM_FLOW; gemFlowId++) 
    {
        if(ignoreGemFlowId == gemFlowId)
            continue;
        
        if (g9607fGemUsFlowMap[gemFlowId].isValid) 
        {
            if(g9607fGemUsFlowMap[gemFlowId].gemPortId == gemPortId)
                return FALSE;
        }
    }

    return (TRUE);
}

static int _rtl9607f_GetPhy2LogicTcontMap(unsigned int phyTcontId,unsigned int *pLogicTcontId)
{
    unsigned char  tcontId;

    if(phyTcontId>=(RTL9607F_MAX_TCONT+1))
        return RT_ERR_FAILED;

    for(tcontId = 0;tcontId<RTL9607F_MAX_TCONT;tcontId++)
    {
        if(g9607fTcontTbl[tcontId].phyTcontId == phyTcontId)
        {
            *pLogicTcontId = tcontId;
            return RT_ERR_OK;
            
        }
    }
    return RT_ERR_ENTRY_NOTFOUND;
}

static unsigned int _rtl9607f_SetGemFlowMap(
    GEM_FLOW_DIR dir,
    unsigned short gemFlowId, 
    rt_gpon_usFlow_t *pUsFlow,
    unsigned short gemIdx,
    ca_uint16_t    gemPortId)


{    
    /* parameter check */
    RT_PARAM_CHK((gemFlowId >= RTL9607F_MAX_GEM_FLOW), RT_ERR_INPUT);

    if(GEM_FLOW_DIR_DS == dir)
    {
        g9607fGemDsFlowMap[gemFlowId].isValid       = TRUE; 
        g9607fGemDsFlowMap[gemFlowId].gemIdx        = gemIdx;
        g9607fGemDsFlowMap[gemFlowId].gemPortId     = gemPortId;
    }

    else
    {      
        g9607fGemUsFlowMap[gemFlowId].isValid       = TRUE;
        g9607fGemUsFlowMap[gemFlowId].tcontId       = pUsFlow->tcontId;
        g9607fGemUsFlowMap[gemFlowId].tcQueueId     = pUsFlow->tcQueueId;
        g9607fGemUsFlowMap[gemFlowId].aesEn         = pUsFlow->aesEn;
        g9607fGemUsFlowMap[gemFlowId].gemIdx        = gemIdx;
        g9607fGemUsFlowMap[gemFlowId].gemPortId     = gemPortId;

    }
    return RT_ERR_OK;

}


static unsigned int 
_rtl9607f_DeleteGemFlowMap(unsigned short gemFlowId,GEM_FLOW_DIR dir)
{   
    /* parameter check */
    RT_PARAM_CHK((gemFlowId >= RTL9607F_MAX_GEM_FLOW), RT_ERR_INPUT);

    if(GEM_FLOW_DIR_DS == dir)
    {
        g9607fGemDsFlowMap[gemFlowId].isValid   = FALSE;
        g9607fGemDsFlowMap[gemFlowId].gemIdx    = 0x0;
        g9607fGemDsFlowMap[gemFlowId].gemPortId = 0x0;
        g9607fGemDsFlowMap[gemFlowId].loopEn    = 0x0;
        g9607fGemDsFlowMap[gemFlowId].loopLspid = 0x0;

    } 
    else
    {
        g9607fGemUsFlowMap[gemFlowId].isValid       = FALSE;
        g9607fGemUsFlowMap[gemFlowId].tcontId       = 0x0;
        g9607fGemUsFlowMap[gemFlowId].tcQueueId     = 0x0;
        g9607fGemUsFlowMap[gemFlowId].aesEn         = 0x0;
        g9607fGemUsFlowMap[gemFlowId].gemIdx        = 0x0;
        g9607fGemUsFlowMap[gemFlowId].gemPortId     = 0x0;

    }  
    return RT_ERR_OK;
;

}

/*Direct call AAL API*/
static int 
_rtl9607f_gpon_aal_tcont_set(unsigned int tcontId, unsigned int allocId)
{
    aal_xgpon_bwmp_alloc_id_cfg_msk_t mask;
    aal_xgpon_bwmp_alloc_id_cfg_t alloc_id_cfg;
    aal_gpon_tcont_cfg_msk_t gpon_mask;
    aal_gpon_tcont_cfg_t gpon_alloc_id_cfg;

    if(IS_OMCC_TCONT(tcontId)){
        RTL9607F_LOG_ERR("%s:%d::ERROR: TCONT %d only used for OMCC!",__FUNCTION__,__LINE__,tcontId);
        return RT_ERR_FAILED;
    }

    if (allocId > 0xFFFF ){
        RTL9607F_LOG_ERR("%s:%d:: ERROR alloc_id is :%d",__FUNCTION__,__LINE__,allocId);
        return RT_ERR_FAILED;
    }

    if(CA_PON_MODE_IS_2D5GPON(gPonMode))
    {
        gpon_mask.u32 = 0;
        gpon_mask.s.tcont_index =1;
        gpon_mask.s.omci_en =1;
        gpon_mask.s.ploam_en =0;
        gpon_alloc_id_cfg.tcont_index = tcontId;
        gpon_alloc_id_cfg.omci_en = 1;
        gpon_alloc_id_cfg.ploam_en = 0;

        ca_spin_lock(g_aal_gpon_flow_lock);
        if (CA_E_OK !=aal_gpon_tcont_set(SATURN_DEV_ID, FALSE, allocId, gpon_mask, &gpon_alloc_id_cfg))
        {
            ca_spin_unlock(g_aal_gpon_flow_lock);
            RTL9607F_LOG_ERR("%s:%d::create tcont_id:%d fail",__FUNCTION__,__LINE__,tcontId);
            return RT_ERR_FAILED;
        }
        ca_spin_unlock(g_aal_gpon_flow_lock);

        (void)aal_puc_pvtbl_enable_set(0,tcontId,1);
    }
    else
    {
#if defined(CONFIG_10G_GPON_FEATURE)
        mask.u32 = 0;
        mask.s.alloc_id =1;
        mask.s.omci_en =1;
        mask.s.ploam_en =1;
        alloc_id_cfg.alloc_id = allocId;
        alloc_id_cfg.omci_en = 1;
        alloc_id_cfg.ploam_en = 1;
    
        if (CA_E_OK !=aal_xgpon_bwmp_tcont_set(SATURN_DEV_ID, AAL_XGPON_WAVE_CHANNEL_DEFAULT, tcontId, mask, &alloc_id_cfg))
        {
            RTL9607F_LOG_ERR("%s:%d::create tcont_id:%d fail",__FUNCTION__,__LINE__,tcontId);
            return RT_ERR_FAILED;
        }
#else
        RTL9607F_LOG_ERR("%s:%d::unknown PON mode",__FUNCTION__, __LINE__);
        return RT_ERR_CHIP_NOT_SUPPORTED;
#endif
    }
    RTL9607F_LOG_DBG("Add TCONT %u Alloc-ID %u",tcontId,  allocId);

    return RT_ERR_OK;

}


static int 
_rtl9607f_gpon_aal_tcont_del(unsigned int tcontId)
{
    aal_xgpon_bwmp_alloc_id_cfg_msk_t mask;
    aal_xgpon_bwmp_alloc_id_cfg_t alloc_id_cfg;
    aal_gpon_tcont_cfg_msk_t gpon_mask;
    aal_gpon_tcont_cfg_t gpon_alloc_id_cfg;
    unsigned int allocId;
    unsigned int logic_tcontId;

    if(IS_OMCC_TCONT(tcontId)){
        RTL9607F_LOG_ERR("%s:%d::ERROR: TCONT %d only used for OMCC!",__FUNCTION__,__LINE__,tcontId);
        return RT_ERR_FAILED;
    }

    ca_mdelay(1); /* wait for USBG burst finish */

    if(CA_PON_MODE_IS_2D5GPON(gPonMode))
    {
        memset(&gpon_alloc_id_cfg, 0x0, sizeof(aal_gpon_tcont_cfg_t));
        gpon_mask.u32 = 0;
        gpon_mask.s.tcont_index =1;
        gpon_mask.s.omci_en =1;
        gpon_alloc_id_cfg.tcont_index = tcontId;
        gpon_alloc_id_cfg.omci_en = 0;

        if(RT_ERR_OK != _rtl9607f_GetPhy2LogicTcontMap(tcontId, &logic_tcontId))
        {
            RTL9607F_LOG_ERR("%s %d get phy:%u to logic T-cont fail\n",  
                __FUNCTION__, __LINE__, tcontId);
        
            return RT_ERR_FAILED;
        
        }

        _GetAllocIdByTcontId(logic_tcontId, &allocId);

        (void)aal_puc_pvtbl_enable_set(0,tcontId,0);

        (void)aal_puc_debug_set(0,10,0);

        ca_spin_lock(g_aal_gpon_flow_lock);
        if (CA_E_OK != aal_gpon_tcont_set(SATURN_DEV_ID, FALSE, allocId, gpon_mask, &gpon_alloc_id_cfg))
        {
            ca_spin_unlock(g_aal_gpon_flow_lock);
            RTL9607F_LOG_ERR("%s:%d::delete tcont_id:%d fail",__FUNCTION__,__LINE__,tcontId);
            return RT_ERR_FAILED;
        }
        ca_spin_unlock(g_aal_gpon_flow_lock);

        (void)aal_puc_debug_set(0,10,1);

        RTL9607F_LOG_DBG("Del TCONT %u Alloc-ID %u",tcontId,  allocId);
    }
    else
    {
#if defined(CONFIG_10G_GPON_FEATURE)
#if 0
        if (CA_E_OK != aal_xgpon_tcont_flush(SATURN_DEV_ID, AAL_XGPON_WAVE_CHANNEL_DEFAULT, tcontId))
        {
            RTL9607F_LOG_ERR("%s:%d::ERROR delete tcont_id:%d fail",__FUNCTION__,__LINE__,tcontId);
            return RT_ERR_FAILED;
        }
#endif
        memset(&alloc_id_cfg, 0x0, sizeof(aal_xgpon_bwmp_alloc_id_cfg_t));
        mask.u32 = 0;
        mask.s.alloc_id =1;
        mask.s.omci_en =1;
        alloc_id_cfg.alloc_id = 0x3aad;
        alloc_id_cfg.omci_en = 0;

        if (CA_E_OK != aal_xgpon_bwmp_tcont_set(SATURN_DEV_ID, AAL_XGPON_WAVE_CHANNEL_DEFAULT, tcontId, mask, &alloc_id_cfg))
        {
            RTL9607F_LOG_ERR("%s:%d::delete tcont_id:%d fail",__FUNCTION__,__LINE__,tcontId);
            return RT_ERR_FAILED;
        }
#else
        RTL9607F_LOG_ERR("%s:%d::unknown PON mode",__FUNCTION__,__LINE__);
        return RT_ERR_CHIP_NOT_SUPPORTED;
#endif
    }

    return RT_ERR_OK;

}


static int 
_rtl9607f_gpon_aal_usGem_set(unsigned int usFlowId, const rt_gpon_usFlow_t *pUsFlow, unsigned short *pGemIdx)
{
    aal_xgpon_usbg_gem_config_msk_t us_mask;
    aal_xgpon_usbg_gem_config_t us_xgem_cfg;
    unsigned int internal_gem_idx;
    int ret;

    /* parameter check */
    RT_PARAM_CHK((NULL == pUsFlow), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((usFlowId >= RTL9607F_MAX_GEM_FLOW), RT_ERR_INPUT);
#if 0
    if(IS_OMCC_TCONT(pUsFlow->tcontId)){
        RTL9607F_LOG_ERR("%s:%d::ERROR: TCONT %d only used for OMCC!",__FUNCTION__,__LINE__,pUsFlow->tcontId);
        return RT_ERR_FAILED;
    }
#endif
    internal_gem_idx = LOGIC_FLOW_TO_RTL9607F_FLOW(usFlowId);
     

    if(CA_PON_MODE_IS_2D5GPON(gPonMode))
    {
        if (CA_E_OK != aal_gpon_us_gem_port_set(SATURN_DEV_ID, internal_gem_idx, pUsFlow->gemPortId))
        {
            RTL9607F_LOG_ERR("%s:%d::ERROR set us gem fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        } 
    }
    else
    {
#if defined(CONFIG_10G_GPON_FEATURE)
        /*add us gem table*/
        us_mask.u32 = ~0U;
        
        us_xgem_cfg.port_en     = 1;
        us_xgem_cfg.xgem_id     = pUsFlow->gemPortId;
        us_xgem_cfg.aes_mode    = pUsFlow->aesEn;
        us_xgem_cfg.mc_flag     = 0;
    
        if (CA_E_OK != aal_xgpon_usbg_xgem_cfg_set(SATURN_DEV_ID, internal_gem_idx, us_mask, &us_xgem_cfg))
        {
            RTL9607F_LOG_ERR("%s:%d::ERROR set usbg xgem fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
#else
        RTL9607F_LOG_ERR("%s:%d::unknown PON mode",__FUNCTION__,__LINE__);
        return RT_ERR_CHIP_NOT_SUPPORTED;
#endif
    }

//    /*Enable Tcont*/
//    if (CA_E_OK != aal_puc_pvtbl_enable_set(SATURN_DEV_ID, pUsFlow->tcontId, TRUE))
//    {
//        RTL9607F_LOG_ERR("%s:%d::ERROR set pvtbl fail",__FUNCTION__,__LINE__);
//        return RT_ERR_FAILED;
//    }

    
    *pGemIdx = internal_gem_idx;

    return RT_ERR_OK;
}


static int 
_rtl9607f_gpon_aal_usGem_get(unsigned int usFlowId, rt_gpon_usFlow_t *pUsFlow)
{
    aal_xgpon_usbg_gem_config_t us_xgem_cfg;
    unsigned int internal_gem_idx;

    /* parameter check */
    RT_PARAM_CHK((NULL == pUsFlow), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((usFlowId >= RTL9607F_MAX_GEM_FLOW), RT_ERR_INPUT);

    internal_gem_idx = g9607fGemUsFlowMap[usFlowId].gemIdx;

    if(CA_PON_MODE_IS_2D5GPON(gPonMode))
    {
        unsigned short  gem_id;

        if (CA_E_OK != aal_gpon_us_gem_port_get(SATURN_DEV_ID, internal_gem_idx, &gem_id))
        {
            RTL9607F_LOG_ERR("%s:%d::ERROR set usbg gem fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        pUsFlow->gemPortId = gem_id;
        pUsFlow->aesEn = 0; //GPON not support upstream AES 
    }
    else
    {
#if defined(CONFIG_10G_GPON_FEATURE)
        if (CA_E_OK != aal_xgpon_usbg_xgem_cfg_get(SATURN_DEV_ID, internal_gem_idx, &us_xgem_cfg))
        {
            RTL9607F_LOG_ERR("%s:%d::ERROR get usbg xgem fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        pUsFlow->gemPortId = us_xgem_cfg.xgem_id;
        pUsFlow->aesEn = us_xgem_cfg.aes_mode;
#else
        RTL9607F_LOG_ERR("%s:%d::unknown PON mode",__FUNCTION__,__LINE__);
        return RT_ERR_CHIP_NOT_SUPPORTED;
#endif
    }

    return RT_ERR_OK;
}


static int 
_rtl9607f_gpon_aal_dsGem_set(unsigned int dsFlowId, const rt_gpon_dsFlow_t *pDsFlow, unsigned short *pGemIdx)
{
    aal_xgpon_dsfp_xgem_cfg_msk_t ds_mask;
    aal_xgpon_dsfp_xgem_cfg_t   ds_xgem_cfg;
    unsigned int internal_gem_idx;

    /* parameter check */
    RT_PARAM_CHK((NULL == pDsFlow), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((dsFlowId >= RTL9607F_MAX_GEM_FLOW), RT_ERR_INPUT);


    internal_gem_idx = LOGIC_FLOW_TO_RTL9607F_FLOW(dsFlowId);

    if(CA_PON_MODE_IS_2D5GPON(gPonMode))
    {
        aal_gpon_gem_port_cfg_msk_t  ds_gem_cfg_mask;
        aal_gpon_gem_port_cfg_t ds_gem_cfg;
        unsigned char aes_en=0; /* sw_aes_en flag */

        ds_gem_cfg_mask.u32 = 0xffffffff;
        memset(&ds_gem_cfg, 0, sizeof(ds_gem_cfg));
        ds_gem_cfg.gem_index = internal_gem_idx;
        ds_gem_cfg.vld = 1;
        if (CA_E_OK != aal_gpon_ds_gem_port_set(SATURN_DEV_ID, aes_en, pDsFlow->gemPortId, ds_gem_cfg_mask, &ds_gem_cfg))
        {
            RTL9607F_LOG_ERR("%s:%d::set ds gem fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }
    else
    {
#if defined(CONFIG_10G_GPON_FEATURE)
        ds_mask.u32 = 0xffffffff;
    
        ds_xgem_cfg.gem_vld = 1;
        ds_xgem_cfg.gem_id = pDsFlow->gemPortId;
        ds_xgem_cfg.gem_mc = pDsFlow->isMcGem;
        if (CA_E_OK != aal_xgpon_dsfp_xgem_set(SATURN_DEV_ID, internal_gem_idx, ds_mask, &ds_xgem_cfg))
        {
            RTL9607F_LOG_ERR("%s:%d::set ds gem fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
#else
        RTL9607F_LOG_ERR("%s:%d::unknown PON mode",__FUNCTION__,__LINE__);
        return RT_ERR_CHIP_NOT_SUPPORTED;
#endif
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
            RTL9607F_LOG_ERR("%s:%d::set pdc map fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

    }

#endif

    *pGemIdx = internal_gem_idx;

    return RT_ERR_OK;

}


static int 
_rtl9607f_gpon_aal_dsGem_get(unsigned int dsFlowId, rt_gpon_dsFlow_t *pDsFlow)
{
    aal_xgpon_dsfp_xgem_cfg_t   ds_xgem_cfg;
    unsigned int internal_gem_idx;

    /* parameter check */
    RT_PARAM_CHK((NULL == pDsFlow), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((dsFlowId >= RTL9607F_MAX_GEM_FLOW), RT_ERR_INPUT);

    internal_gem_idx = LOGIC_FLOW_TO_RTL9607F_FLOW(dsFlowId);
    if(CA_PON_MODE_IS_2D5GPON(gPonMode))
    {
        aal_gpon_gem_port_cfg_t ds_gem_cfg;
        unsigned short gem_id;
        unsigned char aes_en=0; /* sw_aes_en flag */

        gem_id = g9607fGemDsFlowMap[dsFlowId].gemPortId;
        if (CA_E_OK != aal_gpon_ds_gem_port_get(SATURN_DEV_ID, aes_en, gem_id, &ds_gem_cfg))
        {
            RTL9607F_LOG_ERR("%s:%d::ERROR set usbg xgem fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        pDsFlow->gemPortId = gem_id;
        pDsFlow->aesEn = ds_gem_cfg.aes;
    }
    else
    {
#if defined(CONFIG_10G_GPON_FEATURE)
        if (CA_E_OK != aal_xgpon_dsfp_xgem_get(SATURN_DEV_ID, internal_gem_idx, &ds_xgem_cfg))
        {
            RTL9607F_LOG_ERR("%s:%d::get ds gem fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        pDsFlow->gemPortId = ds_xgem_cfg.gem_id;
        pDsFlow->aesEn = 0; //downstream encryption is decided by OLT in runtime, so no such information
#else
        RTL9607F_LOG_ERR("%s:%d::unknown PON mode",__FUNCTION__,__LINE__);
        return RT_ERR_CHIP_NOT_SUPPORTED;
#endif
    }

    return RT_ERR_OK;
}


static int 
_rtl9607f_gpon_aal_usGem_del(unsigned int usFlowId)
{
    aal_xgpon_usbg_gem_config_msk_t us_mask;
    aal_xgpon_usbg_gem_config_t us_xgem_cfg;
    unsigned int internal_gem_idx;

    RT_PARAM_CHK((usFlowId >= RTL9607F_MAX_GEM_FLOW), RT_ERR_INPUT);

    internal_gem_idx = LOGIC_FLOW_TO_RTL9607F_FLOW(usFlowId);

    
    if(CA_PON_MODE_IS_2D5GPON(gPonMode))
    {
        unsigned short gem_id;

        if (CA_E_OK != aal_gpon_us_gem_port_get(SATURN_DEV_ID, internal_gem_idx, &gem_id))
        {
            RTL9607F_LOG_ERR("%s:%d::ERROR set usbg xgem fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        //delete GEM by setting GEM Port ID = 0
        if (CA_E_OK != aal_gpon_us_gem_port_set(SATURN_DEV_ID, internal_gem_idx, 0))
        {
            RTL9607F_LOG_ERR("%s:%d::Error Disable usbg gem fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if (__tcont_tbl_is_empty(usFlowId)) 
        {
            aal_gpon_mac_ctrl_cfg_msk_t mac_ctrl_mask;
            aal_gpon_mac_ctrl_cfg_t     mac_ctrl_cfg;
            unsigned int tcontId = g9607fGemUsFlowMap[usFlowId].tcontId;

            mac_ctrl_mask.u32 = 0;
            memset(&mac_ctrl_cfg, 0, sizeof(mac_ctrl_cfg));

            mac_ctrl_mask.s.flush_en = 1;
            mac_ctrl_mask.s.flush_id = 1;
            mac_ctrl_cfg.flush_en = 1;
            mac_ctrl_cfg.flush_id = tcontId;
#if 0 /* remove tcont flush because the issue sending traffic when MIB reset will lead to GPON MAC hang. */
            //flush tcont
            if (CA_E_OK != aal_gpon_mac_ctrl_set(SATURN_DEV_ID, mac_ctrl_mask, &mac_ctrl_cfg))
            {
                RTL9607F_LOG_ERR("%s:%d::Error tcont:%d flush fail",__FUNCTION__,__LINE__, tcontId);
                return RT_ERR_FAILED;
            }

            ca_mdelay(10);
            mac_ctrl_cfg.flush_en = 0;
            if (CA_E_OK != aal_gpon_mac_ctrl_set(SATURN_DEV_ID, mac_ctrl_mask, &mac_ctrl_cfg))
            {
                RTL9607F_LOG_ERR("%s:%d::Error tcont:%d flush fail",__FUNCTION__,__LINE__, tcontId);
                return RT_ERR_FAILED;
            }
#endif
//            if (tcontId != gem_id) 
//            {
//                if (CA_E_OK != aal_puc_pvtbl_enable_set(SATURN_DEV_ID, tcontId, FALSE))
//                {
//                    RTL9607F_LOG_ERR("%s:%d::Error pus pvtbl set fail",__FUNCTION__,__LINE__);
//                    return RT_ERR_FAILED;
//                }
//            }
        }
    }
    else
    {
#if defined(CONFIG_10G_GPON_FEATURE)
        memset(&us_xgem_cfg, 0, sizeof(aal_xgpon_usbg_gem_config_t));
        us_mask.u32 = 0xffffffff;
    
        //us_xgem_cfg.xgem_id = pUsFlow->gemPortId;
        us_xgem_cfg.port_en = FALSE;
    
        
        if (CA_E_OK != aal_xgpon_usbg_xgem_cfg_set(SATURN_DEV_ID, internal_gem_idx, us_mask, &us_xgem_cfg))
        {
            RTL9607F_LOG_ERR("%s:%d::Error Disable usbg gem fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    
    
        if (__tcont_tbl_is_empty(usFlowId)) 
        {
            aal_xgpon_bwmp_alloc_id_cfg_t  aal_bwmp_cfg;
            aal_xgpon_dsfp_xgem_cfg_t      aal_dsfp_cfg;
            unsigned int tcontId = g9607fGemUsFlowMap[usFlowId].tcontId;
    
            memset(&aal_bwmp_cfg, 0, sizeof(aal_bwmp_cfg));
    
            if (CA_E_OK != aal_xgpon_bwmp_tcont_get(SATURN_DEV_ID, 0, tcontId, &aal_bwmp_cfg))
            {
                RTL9607F_LOG_ERR("%s:%d::Error bwmp get fail",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
    
    
            if (CA_E_OK != aal_xgpon_dsfp_xgem_get(SATURN_DEV_ID, 0, &aal_dsfp_cfg))
            {
                RTL9607F_LOG_ERR("%s:%d::Error dsfp get fail",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
#if 0
            if (CA_E_OK != aal_xgpon_tcont_flush(SATURN_DEV_ID, 0, tcontId))
            {
                RTL9607F_LOG_ERR("%s:%d::Error tcont:%d flush fail",__FUNCTION__,__LINE__,tcontId);
                return RT_ERR_FAILED;
            }

#endif
//            if (aal_bwmp_cfg.alloc_id != aal_dsfp_cfg.gem_id)
//            {
//                if (CA_E_OK != aal_puc_pvtbl_enable_set(SATURN_DEV_ID, tcontId, FALSE))
//                {
//                    RTL9607F_LOG_ERR("%s:%d::Error pus pvtbl set fail",__FUNCTION__,__LINE__);
//                    return RT_ERR_FAILED;
//                }
//            }
        }
#else
        RTL9607F_LOG_ERR("%s:%d::unknown PON mode",__FUNCTION__,__LINE__);
        return RT_ERR_CHIP_NOT_SUPPORTED;
#endif
    }
    
    return RT_ERR_OK;
}

static int 
_rtl9607f_gpon_aal_dsGem_del(unsigned int dsFlowId)
{
    aal_xgpon_dsfp_xgem_cfg_msk_t ds_mask;
    aal_xgpon_dsfp_xgem_cfg_t   ds_xgem_cfg;
    unsigned short internal_gem_idx = 0;


    internal_gem_idx = LOGIC_FLOW_TO_RTL9607F_FLOW(dsFlowId);

    if(CA_PON_MODE_IS_2D5GPON(gPonMode))
    {
        aal_gpon_gem_port_cfg_msk_t  ds_gem_cfg_mask;
        aal_gpon_gem_port_cfg_t ds_gem_cfg;
        unsigned ds_gem_id;

        ds_gem_id = g9607fGemDsFlowMap[dsFlowId].gemPortId;
        ds_gem_cfg_mask.u32 = 0xffffffff;
        memset(&ds_gem_cfg, 0, sizeof(ds_gem_cfg));
        ds_gem_cfg.gem_index = internal_gem_idx;
        ds_gem_cfg.vld = 0;
        if (CA_E_OK != aal_gpon_ds_gem_port_set(SATURN_DEV_ID, 0, ds_gem_id, ds_gem_cfg_mask, &ds_gem_cfg))
        {
            RTL9607F_LOG_ERR("%s:%d::set ds gem fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }
    else
    {
#if defined(CONFIG_10G_GPON_FEATURE)
        ds_mask.u32 = 0xffffffff;
        memset(&ds_xgem_cfg, 0, sizeof(aal_xgpon_dsfp_xgem_cfg_t));
        
        ds_xgem_cfg.gem_vld = 0;
    
        if (CA_E_OK != aal_xgpon_dsfp_xgem_set(SATURN_DEV_ID, internal_gem_idx, ds_mask, &ds_xgem_cfg))
        {
            RTL9607F_LOG_ERR("%s:%d::Disable ds gem fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
#else
        RTL9607F_LOG_ERR("%s:%d::unknown PON mode",__FUNCTION__,__LINE__);
        return RT_ERR_CHIP_NOT_SUPPORTED;
#endif
    }

    return RT_ERR_OK;

}

static int 
_rtl9607f_gpon_aal_loopGem_set(uint8_t enable, uint32 pdcGemIdx, uint32 usGemIdx, uint32 lspid)
{
    aal_pdc_pdc_map_mem_data_mask_t pdc_mask;
    aal_pdc_pdc_map_mem_data_t      pdc_cfg;
    aal_ilpb_cfg_msk_t ilpb_mask;
    aal_ilpb_cfg_t ilpb_cfg;


    /*Config PDC*/    
    memset(&pdc_mask, 0, sizeof(aal_pdc_pdc_map_mem_data_mask_t));
    memset(&pdc_cfg, 0, sizeof(aal_pdc_pdc_map_mem_data_t));

    pdc_mask.bf.pol_id = 1;
    pdc_mask.bf.lspid = 1;

    pdc_cfg.pol_id = enable ? usGemIdx: pdcGemIdx-8;
    pdc_cfg.lspid = enable ? lspid: AAL_PON_PORT_ID;

    if (CA_E_OK != aal_pdc_map_mem_set(0, pdcGemIdx, pdc_mask, &pdc_cfg))
    {
        RTL9607F_LOG_ERR("%s:%d::Set PDC map pdcGemIdx:%d fail", __FUNCTION__, __LINE__, pdcGemIdx);
        return RT_ERR_FAILED;
    }


    /*Config ILPB*/
    memset(&ilpb_mask, 0, sizeof(aal_ilpb_cfg_msk_t));
    memset(&ilpb_cfg, 0, sizeof(aal_ilpb_cfg_t));
    ilpb_mask.s.loop_enable = 1;
    ilpb_mask.s.da_sa_swap_en = 1;

    ilpb_cfg.loop_enable = enable ? 1: 0;
    ilpb_cfg.da_sa_swap_en = enable ? 1: 0;


    if (CA_E_OK != aal_port_ilpb_cfg_set(0, lspid, ilpb_mask, &ilpb_cfg))
    {
        RTL9607F_LOG_ERR("%s:%d::Set ILPB lspid:%d fail", __FUNCTION__, __LINE__, lspid);
        return RT_ERR_FAILED;
    }


    return RT_ERR_OK;
}

int32
_rtl9607f_gpon_omci_mirror(uint32 msgLen, uint8 *pMsg)
{
    int encap_length;
    struct sk_buff *p_skb;
    HEADER_A_T ni_header_a;

    /* parameter check */
    RT_PARAM_CHK((NULL == pMsg), RT_ERR_NULL_POINTER);

    encap_length = sizeof(omciHeader);

    /*NIC driver may add extra 16 bytes header*/
    p_skb = netdev_alloc_skb(NULL, msgLen+encap_length+16);
    if (p_skb == NULL) 
    {
        RTL9607F_LOG_ERR("%s %d : netdev_alloc_skb fail!!",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    memcpy(p_skb->data,omciHeader,encap_length);
    memcpy(p_skb->data+encap_length,pMsg,msgLen);
    p_skb->len = msgLen+encap_length;

    memset(&ni_header_a, 0, sizeof(HEADER_A_T));

    ni_header_a.bits.cos = 7;
    ni_header_a.bits.ldpid = CA_PORT_ID(CA_PORT_TYPE_ETHERNET, gMirroringPort);
    ni_header_a.bits.lspid = 0x10;
    ni_header_a.bits.pkt_size = p_skb->len;
    ni_header_a.bits.fe_bypass = 1;

    ni_header_a.bits.bits_32_63.pkt_info.no_drop = 1;

    __rtk_ni_start_xmit(p_skb, &ni_header_a, NULL , NULL , NULL, 1);

    return RT_ERR_OK;

}


static uint32_t _rtl9607f_gpon_omci_gen_crc(uint8_t *p, uint32_t len)
{
    uint32_t crc = 0xffffffff;
    uint32_t ret_crc;

    while (len--) {
        crc = ((crc & 0x00ffffff) << 8) ^ crctab[(((crc >> 24) ^ *p++) & 0xFF)];
    }

    crc = (crc ^ 0xffffffff);

    ret_crc = (uint32_t)((crc&0xff) << 24 | (crc&0xff00) << 8 | (crc&0xff0000) >> 8 | (crc&0xff000000) >> 24);

    return ret_crc;
}


static int _rtl9607f_gpon_omci_build_crc(uint8_t * omci_ptr)
{
    rt_gpon_omci_pkt_hdr_t *pHdr = (rt_gpon_omci_pkt_hdr_t *)omci_ptr;
    
    uint32_t genCrc = 0;

    if (0x0a == pHdr->devId) // base omci format
    {
        genCrc = _rtl9607f_gpon_omci_gen_crc(omci_ptr,sizeof(rt_gpon_omci_base_pkt_t)-sizeof(genCrc));
        if(!genCrc)
        {
            RTL9607F_LOG_ERR("%s %d : Generate Crc Error!!",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        memcpy(omci_ptr+sizeof(rt_gpon_omci_base_pkt_t)-sizeof(genCrc),&genCrc,sizeof(genCrc));
    }
    else if (0x0b == pHdr->devId) //extend omci format
    {
        rt_gpon_omci_ext_pkt_t *pExtHdr = (rt_gpon_omci_ext_pkt_t *)omci_ptr;
        uint16_t contLen = 0;

        contLen = ntohs(pExtHdr->len);

        genCrc = _rtl9607f_gpon_omci_gen_crc(omci_ptr,sizeof(rt_gpon_omci_ext_pkt_t)+contLen);
        if(!genCrc)
        {
            RTL9607F_LOG_ERR("%s %d : Generate Crc Error!!",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        memcpy(omci_ptr+sizeof(rt_gpon_omci_ext_pkt_t)+contLen,&genCrc,sizeof(genCrc));
    }
    else 
    {
        RTL9607F_LOG_ERR("%s %d : OMCI device id(%d))is not suppert",__FUNCTION__,__LINE__,pHdr->devId);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}


static int _rtl9607f_gpon_omci_check_crc(uint8_t * omci_ptr)
{
    rt_gpon_omci_pkt_hdr_t *pHdr = (rt_gpon_omci_pkt_hdr_t *)omci_ptr;
    
    if (0x0a == pHdr->devId) // base omci format
    {
        rt_gpon_omci_base_pkt_t *basePkt = (rt_gpon_omci_base_pkt_t *)omci_ptr;
        uint32_t genCrc = 0;
        uint32_t oriCrc = 0;

        oriCrc = basePkt->crc;

        genCrc = _rtl9607f_gpon_omci_gen_crc(omci_ptr,sizeof(rt_gpon_omci_base_pkt_t)-sizeof(genCrc));
        if (oriCrc != genCrc)
        {
            RTL9607F_LOG_ERR("%s %d : GenCrc [0x%x] OriCrc [0x%x]",__FUNCTION__,__LINE__,genCrc, oriCrc);
            g9607fOmciCount.omci_rx_mic_error_count++;
            return RT_ERR_FAILED;
        }
        g9607fOmciCount.omci_rx_baseline_count++;
    }
    else if (0x0b == pHdr->devId) //extend omci format
    {
        rt_gpon_omci_ext_pkt_t *pExtHdr = (rt_gpon_omci_ext_pkt_t *)omci_ptr;
        uint32_t genCrc  = 0;
        uint32_t oriCrc  = 0;
        uint16_t contLen = 0;

        contLen = ntohs(pExtHdr->len);
        memcpy(&oriCrc, (omci_ptr+sizeof(rt_gpon_omci_ext_pkt_t)+contLen), sizeof(genCrc));

        genCrc = _rtl9607f_gpon_omci_gen_crc(omci_ptr,sizeof(rt_gpon_omci_ext_pkt_t)+contLen);
        if (oriCrc != genCrc)
        {
            RTL9607F_LOG_ERR("%s %d : GenCrc [0x%x] OriCrc [0x%x]",__FUNCTION__,__LINE__,genCrc, oriCrc);
            g9607fOmciCount.omci_rx_mic_error_count++;
            return RT_ERR_FAILED;
        }
        g9607fOmciCount.omci_rx_extended_count++;
        
    }
    else 
    {
        RTL9607F_LOG_ERR("%s %d : OMCI device id(%d))is not suppert",__FUNCTION__,__LINE__,pHdr->devId);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

static int _rtl9607f_gpon_omci_build_mic(uint8_t * omci_ptr)
{
#if defined(CONFIG_10G_GPON_FEATURE)
    rt_gpon_omci_pkt_hdr_t *pHdr = (rt_gpon_omci_pkt_hdr_t *)omci_ptr;
    uint32_t genMic = 0;
    
    if (0x0a == pHdr->devId) // base omci format
    {
        if (CA_E_OK != ca_generate_omci_mic(SATURN_DEV_ID, RT_GPON_OMCI_MIC_DIR_US, omci_ptr, sizeof(rt_gpon_omci_base_pkt_t)-4, 4, (ca_uint8_t *)&genMic))        
        {
            RTL9607F_LOG_ERR("%s %d generate OMCI MIC fail\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        memcpy(omci_ptr+sizeof(rt_gpon_omci_base_pkt_t)-4,&genMic,4);
    }
    else if (0x0b == pHdr->devId) //extend omci format
    {
        rt_gpon_omci_ext_pkt_t *pExtHdr = (rt_gpon_omci_ext_pkt_t *)omci_ptr;
        uint16_t contLen = 0;

        contLen = ntohs(pExtHdr->len);

        if(CA_E_OK != ca_generate_omci_mic(SATURN_DEV_ID, RT_GPON_OMCI_MIC_DIR_US, omci_ptr, sizeof(rt_gpon_omci_ext_pkt_t)+contLen, 4, (ca_uint8_t *)&genMic))
        {
            RTL9607F_LOG_ERR("%s %d : Generate MIC Error!!",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        memcpy(omci_ptr+sizeof(rt_gpon_omci_ext_pkt_t)+contLen,&genMic,4);
    }
    else 
    {
        RTL9607F_LOG_ERR("%s %d : OMCI device id(%d))is not suppert",__FUNCTION__,__LINE__,pHdr->devId);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
#else
        RTL9607F_LOG_ERR("%s:%d::unknown PON mode",__FUNCTION__,__LINE__);
        return RT_ERR_CHIP_NOT_SUPPORTED;
#endif
}


static int _rtl9607f_gpon_omci_check_mic(uint8_t * omci_ptr)
{
#if defined(CONFIG_10G_GPON_FEATURE)
    rt_gpon_omci_pkt_hdr_t *pHdr = (rt_gpon_omci_pkt_hdr_t *)omci_ptr;
    
    if (0x0a == pHdr->devId) // base omci format
    {
        rt_gpon_omci_base_pkt_t *basePkt = (rt_gpon_omci_base_pkt_t *)omci_ptr;
        uint32_t genMic = 0;
        uint32_t oriMic = 0;

        oriMic = basePkt->crc;

        if (CA_E_OK != ca_generate_omci_mic(SATURN_DEV_ID, RT_GPON_OMCI_MIC_DIR_DS, omci_ptr, sizeof(rt_gpon_omci_base_pkt_t)-4, 4, (ca_uint8_t *)&genMic))        
        {
            RTL9607F_LOG_ERR("%s %d generate OMCI MIC fail\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }


        if (oriMic != genMic)
        {
            RTL9607F_LOG_ERR("%s %d : GenMic [0x%x] oriMic [0x%x]",__FUNCTION__,__LINE__,genMic, oriMic);
            g9607fOmciCount.omci_rx_mic_error_count++;
            return RT_ERR_FAILED;
        }
        g9607fOmciCount.omci_rx_baseline_count++;
    }
    else if (0x0b == pHdr->devId) //extend omci format
    {
        rt_gpon_omci_ext_pkt_t *pExtHdr = (rt_gpon_omci_ext_pkt_t *)omci_ptr;
        uint32_t genMic  = 0;
        uint32_t oriMic  = 0;
        uint16_t contLen = 0;

        contLen = ntohs(pExtHdr->len);
        memcpy(&oriMic, (omci_ptr+sizeof(rt_gpon_omci_ext_pkt_t)+contLen), 4);

        if(CA_E_OK != ca_generate_omci_mic(SATURN_DEV_ID, RT_GPON_OMCI_MIC_DIR_DS, omci_ptr, sizeof(rt_gpon_omci_ext_pkt_t)+contLen, 4,(ca_uint8_t *) &genMic))
        {
            RTL9607F_LOG_ERR("%s %d : Generate MIC Error!!",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if (oriMic != genMic)
        {
            RTL9607F_LOG_ERR("%s %d : GenMic [0x%x] oriMic [0x%x]",__FUNCTION__,__LINE__,genMic, oriMic);
            g9607fOmciCount.omci_rx_mic_error_count++;
            return RT_ERR_FAILED;
        }
        g9607fOmciCount.omci_rx_extended_count++;
    }
    else 
    {
        RTL9607F_LOG_ERR("%s %d : OMCI device id(%d))is not suppert",__FUNCTION__,__LINE__,pHdr->devId);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
#else
        RTL9607F_LOG_ERR("%s:%d::unknown PON mode",__FUNCTION__,__LINE__);
        return RT_ERR_CHIP_NOT_SUPPORTED;
#endif
}



int _rtl9607f_receive_from_nic(struct napi_struct *napi,struct net_device *dev, struct sk_buff *skb, nic_hook_private_t *nh_priv)
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
            if(CA_PON_MODE == ONU_PON_MAC_XGPON1 || CA_PON_MODE == ONU_PON_MAC_XGSPON || CA_PON_MODE == ONU_PON_MAC_NGPON2 || CA_PON_MODE == ONU_PON_MAC_NGPON2_D10G)
            {
                if(_rtl9607f_gpon_omci_check_mic(omci_ptr)!= RT_ERR_OK)
                {
                    RTL9607F_LOG_ERR("%s %d : OMCI check mic Error!!",__FUNCTION__,__LINE__);
                    return RE8670_RX_STOP;
                }
            }
            else if(CA_PON_MODE_IS_2D5GPON(gPonMode))
            {
                if(_rtl9607f_gpon_omci_check_crc(omci_ptr)!= RT_ERR_OK)
                {
                    RTL9607F_LOG_ERR("%s %d : OMCI check crc Error!!",__FUNCTION__,__LINE__);
                    return RE8670_RX_STOP;
                }
            }
            else
            {
                RTL9607F_LOG_ERR("%s %d : Not support PON MODE!!",__FUNCTION__,__LINE__);
                return RE8670_RX_STOP;
            }

            gOmciRxCallback(omci_len, omci_ptr);
            if(ENABLED == gOmciMirrorEnable)
            {
                _rtl9607f_gpon_omci_mirror(omci_len, omci_ptr);     
            }
        }


        return RE8670_RX_STOP;
    }

    return RE8670_RX_CONTINUE;

}

static int _rtl9607f_gpon_pm_phyLos_get(rt_gpon_pm_counter_t *pPmCnt)
{
    memset(&pPmCnt->phyLos, 0x0, sizeof(rt_gpon_pm_phy_los_t));

    if(CA_PON_MODE_IS_2D5GPON(gPonMode))
    {
		uint32_t                   bipCnt, bipFrame;
        
        if (CA_E_OK != aal_gpon_current_bip_error_get(SATURN_DEV_ID, 0, &bipCnt, &bipFrame))
        {
            RTL9607F_LOG_ERR("%s:%d::get ONU PHY/LOS count fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        pPmCnt->phyLos.bip_error_count = bipCnt;
        pPmCnt->phyLos.total_words_protected_by_bip = bipFrame;
    }
    else
    {
#if defined(CONFIG_10G_GPON_FEATURE)
		uint32_t bip32_data0 = 0;
		uint32_t bip32_data1 = 0;
		uint32_t bip32_err = 0;
		uint32_t psbd_hec_err = 0;
		uint32_t psbd_hec_uncorrect_cnt = 0;
		uint32_t xgtc_hec_correct_cnt = 0;
		uint32_t xgtc_hec_uncorrect_cnt = 0;

		if (CA_E_OK != aal_xgpon_dsfp_bip_dw0_get(SATURN_DEV_ID,&bip32_data0))
		{
			RTL9607F_LOG_ERR("%s:%d::get dspf bip dw0 fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
		}
		if (CA_E_OK != aal_xgpon_dsfp_bip_dw1_get(SATURN_DEV_ID,&bip32_data1))
		{
			RTL9607F_LOG_ERR("%s:%d::get dspf bip dw1 fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
		}
		if (CA_E_OK != aal_xgpon_dsfp_bip_err_get(SATURN_DEV_ID,&bip32_err))
		{
			RTL9607F_LOG_ERR("%s:%d::get dspf bip err fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
		}
		if (CA_E_OK != aal_xgpon_dsfp_psbd_hec_err_get(SATURN_DEV_ID,&psbd_hec_err))
		{
			RTL9607F_LOG_ERR("%s:%d::get dspf psbd hec err fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
		}
		if (CA_E_OK != aal_xgpon_dsfp_psbd_hec_uncorrect_cnt_get(SATURN_DEV_ID,&psbd_hec_uncorrect_cnt))
		{
			RTL9607F_LOG_ERR("%s:%d::get psbd hec uncor fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
		}
		if (CA_E_OK != aal_xgpon_dsfp_xgtc_hec_correct_cnt_get(SATURN_DEV_ID, &xgtc_hec_correct_cnt))
		{
			RTL9607F_LOG_ERR("%s:%d::get dspf xgtc hec cor fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
		}
		if (CA_E_OK != aal_xgpon_dsfp_xgtc_hec_uncorrect_cnt_get(SATURN_DEV_ID,&xgtc_hec_uncorrect_cnt))
        {
			RTL9607F_LOG_ERR("%s:%d::get dspf xgtc hec uncor fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

		pPmCnt->phyLos.total_words_protected_by_bip =  ((uint64_t)bip32_data1<<32)|bip32_data0;
		pPmCnt->phyLos.bip_error_count = bip32_err;
		pPmCnt->phyLos.psbd_hec_errors_corrected = psbd_hec_err;
		pPmCnt->phyLos.psbd_hec_errors_uncorrectable = psbd_hec_uncorrect_cnt;
		pPmCnt->phyLos.fs_header_errors_corrected = xgtc_hec_correct_cnt;
		pPmCnt->phyLos.fs_header_errors_uncorrectable = xgtc_hec_uncorrect_cnt;
#else
        RTL9607F_LOG_ERR("%s:%d::unknown PON mode",__FUNCTION__,__LINE__);
        return RT_ERR_CHIP_NOT_SUPPORTED;
#endif
    }

    return RT_ERR_OK;
}

static int _rtl9607f_gpon_pm_fec_get(rt_gpon_pm_counter_t *pPmCnt)
{
    memset(&pPmCnt->fec, 0x0, sizeof(rt_gpon_pm_counter_fec_t));

    if(CA_PON_MODE_IS_2D5GPON(gPonMode))
    {
		aal_gpon_mac_fec_mibs_t gponStats;

        if (CA_E_OK != aal_gpon_fec_mibs_get(SATURN_DEV_ID, &gponStats))
        {
            RTL9607F_LOG_ERR("%s:%d::get ONU fec count fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        
        pPmCnt->fec.corrected_fec_bytes = gponStats.fec_corr_byte_cnt;
        pPmCnt->fec.corrected_fec_codewords = gponStats.fec_rcnt;
        pPmCnt->fec.uncorrected_fec_codewords= gponStats.fec_ucnt;
        pPmCnt->fec.total_fec_codewords= gponStats.fec_blk_cnt;
    }
    else
    {
#if defined(CONFIG_10G_GPON_FEATURE)
		aal_xgpon_fec_stats_t fec_stats;

		if (CA_E_OK != aal_xgpon_fec_stats_get(SATURN_DEV_ID, &fec_stats))
        {
            RTL9607F_LOG_ERR("%s:%d::get ONU fec count fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
		pPmCnt->fec.corrected_fec_bytes = fec_stats.corrected_bytes_rx;
		pPmCnt->fec.corrected_fec_codewords = fec_stats.corrected_codewords_rx;
		pPmCnt->fec.uncorrected_fec_codewords = fec_stats. uncorrectable_codewords_rx;
		pPmCnt->fec.total_fec_codewords = fec_stats.total_codewords_rx;
#else
        RTL9607F_LOG_ERR("%s:%d::unknown PON mode",__FUNCTION__,__LINE__);
        return RT_ERR_CHIP_NOT_SUPPORTED;
#endif
    }

    return RT_ERR_OK;
}

static int _rtl9607f_gpon_pm_xgem_get(rt_gpon_pm_counter_t *pPmCnt)
{
    memset(&pPmCnt->xgem, 0x0, sizeof(rt_gpon_pm_counter_xgem_t));

    if(CA_PON_MODE_IS_2D5GPON(gPonMode))
    {
		aal_gpon_ds_gem_port_mib_t  gpon_gem_dn;
		aal_gpon_us_gem_port_mib_t  gpon_gem_up;
		aal_gpon_stats_t stats;

        uint32_t gem;
        for (gem = 0; gem < AAL_GPON_SYSTEM_MAX_GEM_NUM; gem++)
        {
            if (CA_E_OK != aal_gpon_ds_gem_port_mib_get(SATURN_DEV_ID, gem, 2, &gpon_gem_dn))
            {
                RTL9607F_LOG_ERR("%s:%d::get ONU DS GEM count %d fail",__FUNCTION__,__LINE__,gem);
                return RT_ERR_FAILED;
            }
            pPmCnt->xgem.total_received_xgem_frames += gpon_gem_dn.fcnt;
            pPmCnt->xgem.total_received_bytes_in_non_idle_xgem_frames += gpon_gem_dn.bcnt;

            if (CA_E_OK != aal_gpon_us_gem_port_mib_get(SATURN_DEV_ID, AAL_GPON_PLOAM_MIB_READ_CLR, gem, &gpon_gem_up))
            {
                RTL9607F_LOG_ERR("%s:%d::get ONU US GEM count %d fail",__FUNCTION__,__LINE__,gem);
                return RT_ERR_FAILED;
            }
            pPmCnt->xgem.total_transmitted_xgem_frames += gpon_gem_up.fcnt;
            pPmCnt->xgem.total_transmitted_bytes_in_non_idle_xgem_frames += gpon_gem_up.bcnt;
        }

        if(CA_E_OK != aal_gpon_port_stats_get(SATURN_DEV_ID, SATURN_ANI_PORT_ID, &stats))
        {
            RTL9607F_LOG_ERR("%s:%d::get gpon stats fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        pPmCnt->xgem.received_xgem_header_hec_errors = stats.gem_uncorrect_bit_err;
    }
    else
    {
#if defined(CONFIG_10G_GPON_FEATURE)
		uint32_t xgem_idx;
		aal_xgpon_usbg_xgem_mib_config_t usbg_xgem_mib;
		aal_xgpon_dsfp_xgem_mib_config_t dsfp_xgem_mib;
		aal_xgpon_usbg_stats_t usbg_stats;
		uint32_t dsfp_xgem_hec_err1 = 0;
		uint32_t dsfp_xgem_hec_err0 = 0;
		uint32_t dsfp_xgem_los_word_cnt = 0;
		uint32_t dsfp_xgem_err_key_cnt = 0;

		for(xgem_idx = 0; xgem_idx <AAL_GPON_SYSTEM_MAX_GEM_NUM; xgem_idx++){
			if (CA_E_OK != aal_xgpon_usbg_xgem_mib_get( SATURN_DEV_ID, xgem_idx,&usbg_xgem_mib))
			{
				RTL9607F_LOG_ERR("%s:%d::get usbg xgem mib fail",__FUNCTION__,__LINE__);
				return RT_ERR_FAILED;
			}
			if (CA_E_OK != aal_xgpon_dsfp_xgem_mib_get( SATURN_DEV_ID, xgem_idx,&dsfp_xgem_mib))
			{
				RTL9607F_LOG_ERR("%s:%d::get dsfp xgem mib fail",__FUNCTION__,__LINE__);
				return RT_ERR_FAILED;
			}

			pPmCnt->xgem.total_transmitted_xgem_frames += usbg_xgem_mib.frame_cnt;
			pPmCnt->xgem.total_transmitted_bytes_in_non_idle_xgem_frames += usbg_xgem_mib.byte_cnt;
			pPmCnt->xgem.total_received_xgem_frames += dsfp_xgem_mib.frame_cnt;;
			pPmCnt->xgem.total_received_bytes_in_non_idle_xgem_frames += dsfp_xgem_mib.byte_cnt;
		}

		if (CA_E_OK != aal_xgpon_usbg_stats_get(SATURN_DEV_ID,&usbg_stats))
		{
            RTL9607F_LOG_ERR("%s:%d::get usbg stat fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
		pPmCnt->xgem.transmitted_xgem_frames_with_lf = usbg_stats.xgem_lf0_cnt;
		if (CA_E_OK != aal_xgpon_dsfp_xgem_err1_get(SATURN_DEV_ID,&dsfp_xgem_hec_err1))
		{
            RTL9607F_LOG_ERR("%s:%d::get dsfp xgem err1 fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
		if (CA_E_OK != aal_xgpon_dsfp_xgem_err0_get(SATURN_DEV_ID,&dsfp_xgem_hec_err0))
		{
            RTL9607F_LOG_ERR("%s:%d::get dsfp xgem err0 fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
		pPmCnt->xgem.received_xgem_header_hec_errors =  ((ca_uint64_t)dsfp_xgem_hec_err1<<32)|dsfp_xgem_hec_err0;
		if (CA_E_OK != aal_xgpon_dsfp_xgem_los_word_cnt_get(SATURN_DEV_ID,&dsfp_xgem_los_word_cnt))
		{
            RTL9607F_LOG_ERR("%s:%d::get dsfp xgem los word fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
		pPmCnt->xgem.fs_words_lost_to_xgem_header_hec_errors = dsfp_xgem_los_word_cnt;
		if (CA_E_OK != aal_xgpon_dsfp_xgem_err_key_cnt_get(SATURN_DEV_ID,&dsfp_xgem_err_key_cnt))
        {
            RTL9607F_LOG_ERR("%s:%d::get dsfp xgem err key count fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
		pPmCnt->xgem.xgem_encryption_key_errors = dsfp_xgem_err_key_cnt;

#else
        RTL9607F_LOG_ERR("%s:%d::unknown PON mode",__FUNCTION__,__LINE__);
        return RT_ERR_CHIP_NOT_SUPPORTED;
#endif
    }

    return RT_ERR_OK;
}


static int _rtl9607f_gpon_pm_ploam1_get(rt_gpon_pm_counter_t *pPmCnt)
{
    memset(&pPmCnt->ploam1, 0x0, sizeof(rt_gpon_pm_counter_ploam1_t));

    if(CA_PON_MODE_IS_2D5GPON(gPonMode))
    {
		uint8_t                   tmp;
		uint32_t                  i;

        for(i = AAL_GPON_PLOAM_RX_MIB_UPSTREAM_OVERHEAD; i <= AAL_GPON_PLOAM_RX_MIB_EXTENDED_BURST_LENGTH; i++)
        {
            if (CA_E_OK != aal_gpon_ploam_mib_get(SATURN_DEV_ID, AAL_GPON_PLOAM_MIB_READ, i, &tmp))
            {
                RTL9607F_LOG_ERR("%s:%d::get ONU PLOAM1 count fail",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
            pPmCnt->ploam1.downstream_ploam_message_count += tmp;
        }
        if (CA_E_OK != aal_gpon_ploam_mib_get(SATURN_DEV_ID, AAL_GPON_PLOAM_MIB_READ_CLR, AAL_GPON_PLOAM_RX_MIB_CRC_ERROR, &tmp))
        {
            RTL9607F_LOG_ERR("%s:%d::get ONU PLOAM1 count fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        pPmCnt->ploam1.ploam_mic_errors = tmp;
        pPmCnt->ploam1.downstream_ploam_message_count += tmp;
        if (CA_E_OK != aal_gpon_ploam_mib_get(SATURN_DEV_ID, AAL_GPON_PLOAM_MIB_READ_CLR, AAL_GPON_PLOAM_RX_MIB_RANGING_TIME, &tmp))
        {
            RTL9607F_LOG_ERR("%s:%d::get ONU PLOAM1 count fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        pPmCnt->ploam1.ranging_time_message_count = tmp;
    }
    else
    {
#if defined(CONFIG_10G_GPON_FEATURE)
		ca_uint32_t tmp_cnt=0;

		if (CA_E_OK != aal_ploam_rx_mib_get_type(SATURN_DEV_ID, AAL_PLOAM_RX_MIC_ERR, &tmp_cnt))
        {
            RTL9607F_LOG_ERR("%s:%d::get ONU PLOAM1 RX_MIC_ERR fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        pPmCnt->ploam1.ploam_mic_errors = tmp_cnt;

        if (CA_E_OK != aal_ploam_rx_mib_get_type(SATURN_DEV_ID, AAL_PLOAM_RX_DS, &tmp_cnt))
        {
            RTL9607F_LOG_ERR("%s:%d::get ONU PLOAM1 RX_DS fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        pPmCnt->ploam1.downstream_ploam_message_count = tmp_cnt;

        if (CA_E_OK != aal_ploam_rx_mib_get_type(SATURN_DEV_ID, AAL_PLOAM_RX_RANGE_TIME, &tmp_cnt))
        {
            RTL9607F_LOG_ERR("%s:%d::get ONU PLOAM1 RX_RANGE_TIME fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        pPmCnt->ploam1.ranging_time_message_count = tmp_cnt;

        if (CA_E_OK != aal_ploam_rx_mib_get_type(SATURN_DEV_ID, AAL_PLOAM_RX_PROTECT_CTRL, &tmp_cnt))
        {
            RTL9607F_LOG_ERR("%s:%d::get ONU PLOAM1 RX_PROTECT_CTRL fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        pPmCnt->ploam1.protection_control_message_count = tmp_cnt;

        if (CA_E_OK != aal_ploam_rx_mib_get_type(SATURN_DEV_ID, AAL_PLOAM_RX_ADJ_TX_WAVELEN, &tmp_cnt))
        {
            RTL9607F_LOG_ERR("%s:%d::get ONU PLOAM1 RX_ADJ_TX_WAVELEN fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        pPmCnt->ploam1.adjust_tx_wavelength_message_count = tmp_cnt;
#else
        RTL9607F_LOG_ERR("%s:%d::unknown PON mode",__FUNCTION__,__LINE__);
        return RT_ERR_CHIP_NOT_SUPPORTED;
#endif
    }

    return RT_ERR_OK;
}

static int _rtl9607f_gpon_pm_ploam2_get(rt_gpon_pm_counter_t *pPmCnt)
{
    memset(&pPmCnt->ploam2, 0x0, sizeof(rt_gpon_pm_counter_ploam2_t));

    if(CA_PON_MODE_IS_2D5GPON(gPonMode))
    {
		uint8_t                   tmp;

        if (CA_E_OK != aal_gpon_ploam_mib_get(SATURN_DEV_ID, AAL_GPON_PLOAM_MIB_READ_CLR, AAL_GPON_PLOAM_RX_MIB_UPSTREAM_OVERHEAD, &tmp))
        {
            RTL9607F_LOG_ERR("%s:%d::get ONU PLOAM2 count fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        pPmCnt->ploam2.burst_profile_message_count = tmp;

        if (CA_E_OK != aal_gpon_ploam_mib_get(SATURN_DEV_ID, AAL_GPON_PLOAM_MIB_READ_CLR, AAL_GPON_PLOAM_RX_MIB_ASSIGN_ONU_ID, &tmp))
        {
            RTL9607F_LOG_ERR("%s:%d::get ONU PLOAM2 count fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        pPmCnt->ploam2.assign_onu_id_message_count = tmp;

        if (CA_E_OK != aal_gpon_ploam_mib_get(SATURN_DEV_ID, AAL_GPON_PLOAM_MIB_READ_CLR, AAL_GPON_PLOAM_RX_MIB_DEACTIVATE_ONU_ID, &tmp))
        {
            RTL9607F_LOG_ERR("%s:%d::get ONU PLOAM2 count fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        pPmCnt->ploam2.deactivate_onu_id_message_count = tmp;

        if (CA_E_OK != aal_gpon_ploam_mib_get(SATURN_DEV_ID, AAL_GPON_PLOAM_MIB_READ_CLR, AAL_GPON_PLOAM_RX_MIB_DISABLE_SERIAL_NUMBER, &tmp))
        {
            RTL9607F_LOG_ERR("%s:%d::get ONU PLOAM2 count fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        pPmCnt->ploam2.disable_serial_number_message_count = tmp;

        if (CA_E_OK != aal_gpon_ploam_mib_get(SATURN_DEV_ID, AAL_GPON_PLOAM_MIB_READ_CLR, AAL_GPON_PLOAM_RX_MIB_ENCRYPTED_PORT_ID, &tmp))
        {
            RTL9607F_LOG_ERR("%s:%d::get ONU PLOAM2 count fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        pPmCnt->ploam2.enctyped_port_id_message_count = tmp;

        if (CA_E_OK != aal_gpon_ploam_mib_get(SATURN_DEV_ID, AAL_GPON_PLOAM_MIB_READ_CLR, AAL_GPON_PLOAM_RX_MIB_REQUEST_PASSWORD, &tmp))
        {
            RTL9607F_LOG_ERR("%s:%d::get ONU PLOAM2 count fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        pPmCnt->ploam2.request_registration_message_count = tmp;

        if (CA_E_OK != aal_gpon_ploam_mib_get(SATURN_DEV_ID, AAL_GPON_PLOAM_MIB_READ_CLR, AAL_GPON_PLOAM_RX_MIB_ASSIGN_ALLOC_ID, &tmp))
        {
            RTL9607F_LOG_ERR("%s:%d::get ONU PLOAM2 count fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        pPmCnt->ploam2.assign_alloc_id_message_count = tmp;

        if (CA_E_OK != aal_gpon_ploam_mib_get(SATURN_DEV_ID, AAL_GPON_PLOAM_MIB_READ_CLR, AAL_GPON_PLOAM_RX_MIB_NO_MESSAGE, &tmp))
        {
            RTL9607F_LOG_ERR("%s:%d::get ONU PLOAM2 count fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        pPmCnt->ploam2.no_message_count = tmp;

        if (CA_E_OK != aal_gpon_ploam_mib_get(SATURN_DEV_ID, AAL_GPON_PLOAM_MIB_READ_CLR, AAL_GPON_PLOAM_RX_MIB_POPUP, &tmp))
        {
            RTL9607F_LOG_ERR("%s:%d::get ONU PLOAM2 count fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        pPmCnt->ploam2.popup_message_count = tmp;

        if (CA_E_OK != aal_gpon_ploam_mib_get(SATURN_DEV_ID, AAL_GPON_PLOAM_MIB_READ_CLR, AAL_GPON_PLOAM_RX_MIB_REQUEST_KEY, &tmp))
        {
            RTL9607F_LOG_ERR("%s:%d::get ONU PLOAM2 count fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        pPmCnt->ploam2.request_key_message_count = tmp;

        if (CA_E_OK != aal_gpon_ploam_mib_get(SATURN_DEV_ID, AAL_GPON_PLOAM_MIB_READ_CLR, AAL_GPON_PLOAM_RX_MIB_CONFIGURE_PORT_ID, &tmp))
        {
            RTL9607F_LOG_ERR("%s:%d::get ONU PLOAM2 count fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        pPmCnt->ploam2.config_port_id_message_count = tmp;

        if (CA_E_OK != aal_gpon_ploam_mib_get(SATURN_DEV_ID, AAL_GPON_PLOAM_MIB_READ_CLR, AAL_GPON_PLOAM_RX_MIB_PEE, &tmp))
        {
            RTL9607F_LOG_ERR("%s:%d::get ONU PLOAM2 count fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        pPmCnt->ploam2.pee_message_count = tmp;

        if (CA_E_OK != aal_gpon_ploam_mib_get(SATURN_DEV_ID, AAL_GPON_PLOAM_MIB_READ_CLR, AAL_GPON_PLOAM_RX_MIB_CHANGE_POWER_LEVEL, &tmp))
        {
            RTL9607F_LOG_ERR("%s:%d::get ONU PLOAM2 count fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        pPmCnt->ploam2.change_power_level_message_count = tmp;

        if (CA_E_OK != aal_gpon_ploam_mib_get(SATURN_DEV_ID, AAL_GPON_PLOAM_MIB_READ_CLR, AAL_GPON_PLOAM_RX_MIB_PST_MESSAGE, &tmp))
        {
            RTL9607F_LOG_ERR("%s:%d::get ONU PLOAM2 count fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        pPmCnt->ploam2.pst_message_count = tmp;

        if (CA_E_OK != aal_gpon_ploam_mib_get(SATURN_DEV_ID, AAL_GPON_PLOAM_MIB_READ_CLR, AAL_GPON_PLOAM_RX_MIB_BER_INTERVAL, &tmp))
        {
            RTL9607F_LOG_ERR("%s:%d::get ONU PLOAM2 count fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        pPmCnt->ploam2.ber_interval_message_count = tmp;

        if (CA_E_OK != aal_gpon_ploam_mib_get(SATURN_DEV_ID, AAL_GPON_PLOAM_MIB_READ_CLR, AAL_GPON_PLOAM_RX_MIB_KEY_SWITCHING_TIME, &tmp))
        {
            RTL9607F_LOG_ERR("%s:%d::get ONU PLOAM2 count fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        pPmCnt->ploam2.key_switch_time_message_count = tmp;

        if (CA_E_OK != aal_gpon_ploam_mib_get(SATURN_DEV_ID, AAL_GPON_PLOAM_MIB_READ_CLR, AAL_GPON_PLOAM_RX_MIB_EXTENDED_BURST_LENGTH, &tmp))
        {
            RTL9607F_LOG_ERR("%s:%d::get ONU PLOAM2 count fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        pPmCnt->ploam2.extend_burst_len_message_count = tmp;
    }
    else
    {
#if defined(CONFIG_10G_GPON_FEATURE)
        ca_uint32_t tmp_cnt=0;

        if (CA_E_OK != aal_ploam_rx_mib_get_type(SATURN_DEV_ID, AAL_PLOAM_RX_SYS_PROFILE, &tmp_cnt))
        {
            RTL9607F_LOG_ERR("%s:%d::get ONU PLOAM2 RX_SYS_PROFILE fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        pPmCnt->ploam2.system_profile_message_count = tmp_cnt;

        if (CA_E_OK != aal_ploam_rx_mib_get_type(SATURN_DEV_ID, AAL_PLOAM_RX_CHANNEL_PROFILE, &tmp_cnt))
        {
            RTL9607F_LOG_ERR("%s:%d::get ONU PLOAM2 RX_CHANNEL_PROFILE fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        pPmCnt->ploam2.channel_profile_message_count = tmp_cnt;

        if (CA_E_OK != aal_ploam_rx_mib_get_type(SATURN_DEV_ID, AAL_PLOAM_RX_BURST_PROFILE, &tmp_cnt))
        {
            RTL9607F_LOG_ERR("%s:%d::get ONU PLOAM2 RX_BURST_PROFILE fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        pPmCnt->ploam2.burst_profile_message_count = tmp_cnt;

        if (CA_E_OK != aal_ploam_rx_mib_get_type(SATURN_DEV_ID, AAL_PLOAM_RX_ASSIGN_ONU_ID, &tmp_cnt))
        {
            RTL9607F_LOG_ERR("%s:%d::get ONU PLOAM2 RX_ASSIGN_ONU_ID fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        pPmCnt->ploam2.assign_onu_id_message_count = tmp_cnt;

        pPmCnt->ploam2.unsatisfied_adjust_tx_wavelength_requests = 0;

        if (CA_E_OK != aal_ploam_rx_mib_get_type(SATURN_DEV_ID, AAL_PLOAM_RX_DEACTIVATE, &tmp_cnt))
        {
            RTL9607F_LOG_ERR("%s:%d::get ONU PLOAM2 RX_DEACTIVATE fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        pPmCnt->ploam2.deactivate_onu_id_message_count = tmp_cnt;

        if (CA_E_OK != aal_ploam_rx_mib_get_type(SATURN_DEV_ID, AAL_PLOAM_RX_DISABLE, &tmp_cnt))
        {
            RTL9607F_LOG_ERR("%s:%d::get ONU PLOAM2 RX_DISABLE fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        pPmCnt->ploam2.disable_serial_number_message_count = tmp_cnt;

        if (CA_E_OK != aal_ploam_rx_mib_get_type(SATURN_DEV_ID, AAL_PLOAM_RX_REQ_REGISTRATION, &tmp_cnt))
        {
            RTL9607F_LOG_ERR("%s:%d::get ONU PLOAM2 RX_REQ_REGISTRATION fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        pPmCnt->ploam2.request_registration_message_count = tmp_cnt;

        if (CA_E_OK != aal_ploam_rx_mib_get_type(SATURN_DEV_ID, AAL_PLOAM_RX_ASSIGN_ALLOC_ID, &tmp_cnt))
        {
            RTL9607F_LOG_ERR("%s:%d::get ONU PLOAM2 RX_ASSIGN_ALLOC_ID fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        pPmCnt->ploam2.assign_alloc_id_message_count = tmp_cnt;

        if (CA_E_OK != aal_ploam_rx_mib_get_type(SATURN_DEV_ID, AAL_PLOAM_RX_KEY_CTRL, &tmp_cnt))
        {
            RTL9607F_LOG_ERR("%s:%d::get ONU PLOAM2 RX_KEY_CTRL fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        pPmCnt->ploam2.key_control_message_count = tmp_cnt;

        if (CA_E_OK != aal_ploam_rx_mib_get_type(SATURN_DEV_ID, AAL_PLOAM_RX_SLEEP_ALLOW, &tmp_cnt))
        {
            RTL9607F_LOG_ERR("%s:%d::get ONU PLOAM2 RX_SLEEP_ALLOW fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        pPmCnt->ploam2.sleep_allow_message_count = tmp_cnt;
#else
        RTL9607F_LOG_ERR("%s:%d::unknown PON mode",__FUNCTION__,__LINE__);
        return RT_ERR_CHIP_NOT_SUPPORTED;
#endif
    }

    return RT_ERR_OK;
}

static int _rtl9607f_gpon_pm_ploam3_get(rt_gpon_pm_counter_t *pPmCnt)
{
    memset(&pPmCnt->ploam3, 0x0, sizeof(rt_gpon_pm_counter_ploam3_t));

    if(CA_PON_MODE_IS_2D5GPON(gPonMode))
    {
		uint8_t                   tmp;

        if (CA_E_OK != aal_gpon_ploam_mib_get(SATURN_DEV_ID, AAL_GPON_PLOAM_MIB_READ_CLR, AAL_GPON_PLOAM_TX_MIB_SERIAL_NUMBER_ONU, &tmp))
        {
            RTL9607F_LOG_ERR("%s:%d::get ONU PLOAM3 count fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        pPmCnt->ploam3.serial_number_onu_in_band_message_count = tmp;
        pPmCnt->ploam3.upstream_ploam_message_count += tmp;

        if (CA_E_OK != aal_gpon_ploam_mib_get(SATURN_DEV_ID, AAL_GPON_PLOAM_MIB_READ_CLR, AAL_GPON_PLOAM_TX_MIB_PASSWORD, &tmp))
        {
            RTL9607F_LOG_ERR("%s:%d::get ONU PLOAM3 count fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        pPmCnt->ploam3.registration_message_count = tmp;
        pPmCnt->ploam3.upstream_ploam_message_count += tmp;

        if (CA_E_OK != aal_gpon_ploam_mib_get(SATURN_DEV_ID, AAL_GPON_PLOAM_MIB_READ_CLR, AAL_GPON_PLOAM_TX_MIB_DYING_GASP, &tmp))
        {
            RTL9607F_LOG_ERR("%s:%d::get ONU PLOAM3 count fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        pPmCnt->ploam3.dying_gasp_message_count = tmp;
        pPmCnt->ploam3.upstream_ploam_message_count += tmp;

        if (CA_E_OK != aal_gpon_ploam_mib_get(SATURN_DEV_ID, AAL_GPON_PLOAM_MIB_READ_CLR, AAL_GPON_PLOAM_TX_MIB_NO_MESSAGE, &tmp))
        {
            RTL9607F_LOG_ERR("%s:%d::get ONU PLOAM3 count fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        pPmCnt->ploam3.no_message_count = tmp;
        pPmCnt->ploam3.upstream_ploam_message_count += tmp;
        
        if (CA_E_OK != aal_gpon_ploam_mib_get(SATURN_DEV_ID, AAL_GPON_PLOAM_MIB_READ_CLR, AAL_GPON_PLOAM_TX_MIB_ENCRYPTION_KEY, &tmp))
        {
            RTL9607F_LOG_ERR("%s:%d::get ONU PLOAM3 count fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        pPmCnt->ploam3.encrypt_key_message_count = tmp;
        pPmCnt->ploam3.upstream_ploam_message_count += tmp;

        if (CA_E_OK != aal_gpon_ploam_mib_get(SATURN_DEV_ID, AAL_GPON_PLOAM_MIB_READ_CLR, AAL_GPON_PLOAM_TX_MIB_PEE, &tmp))
        {
            RTL9607F_LOG_ERR("%s:%d::get ONU PLOAM3 count fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        pPmCnt->ploam3.pee_message_count = tmp;
        pPmCnt->ploam3.upstream_ploam_message_count += tmp;

        if (CA_E_OK != aal_gpon_ploam_mib_get(SATURN_DEV_ID, AAL_GPON_PLOAM_MIB_READ_CLR, AAL_GPON_PLOAM_TX_MIB_PST_MESSAGE, &tmp))
        {
            RTL9607F_LOG_ERR("%s:%d::get ONU PLOAM3 count fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        pPmCnt->ploam3.pst_message_count = tmp;
        pPmCnt->ploam3.upstream_ploam_message_count += tmp;

        if (CA_E_OK != aal_gpon_ploam_mib_get(SATURN_DEV_ID, AAL_GPON_PLOAM_MIB_READ_CLR, AAL_GPON_PLOAM_TX_MIB_REI, &tmp))
        {
            RTL9607F_LOG_ERR("%s:%d::get ONU PLOAM3 count fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        pPmCnt->ploam3.rei_message_count = tmp;
        pPmCnt->ploam3.upstream_ploam_message_count += tmp;

        if (CA_E_OK != aal_gpon_ploam_mib_get(SATURN_DEV_ID, AAL_GPON_PLOAM_MIB_READ_CLR, AAL_GPON_PLOAM_TX_MIB_ACKNOWLEDGE, &tmp))
        {
            RTL9607F_LOG_ERR("%s:%d::get ONU PLOAM3 count fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        pPmCnt->ploam3.acknowledgement_message_count = tmp;
        pPmCnt->ploam3.upstream_ploam_message_count += tmp;
    }
    else
    {
#if defined(CONFIG_10G_GPON_FEATURE)
		aal_ploam_tx_mib_t ploam_tx_mib;

		memset(&ploam_tx_mib, 0, sizeof(ploam_tx_mib));
		if (CA_E_OK != aal_ploam_tx_mib_get(SATURN_DEV_ID, &ploam_tx_mib))
        {
            RTL9607F_LOG_ERR("%s:%d::get ONU PLOAM3 count fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

		pPmCnt->ploam3.upstream_ploam_message_count = ploam_tx_mib.us_tx_cnt;
		pPmCnt->ploam3.serial_number_onu_in_band_message_count = ploam_tx_mib.sn_onu_cnt;
		pPmCnt->ploam3.serial_number_onu_amcc_message_count = 0;
		pPmCnt->ploam3.registration_message_count = ploam_tx_mib.registration_cnt;
		pPmCnt->ploam3.acknowledgement_message_count = ploam_tx_mib.ack_cnt;
		pPmCnt->ploam3.sleep_request_message_count = ploam_tx_mib.sleep_request_cnt;
		pPmCnt->ploam3.tuning_response_ack_nack_msg_count = ploam_tx_mib.tuning_resp_ack_nack_cnt;
		pPmCnt->ploam3.tuning_response_complete_u_rollback_msg_count = ploam_tx_mib.tuning_resp_complete_u_rollback_cnt;
		pPmCnt->ploam3.power_consumption_report_message_count = ploam_tx_mib.power_consumption_report_cnt;
		pPmCnt->ploam3.change_power_level_parameter_error_count = 0;
#else
        RTL9607F_LOG_ERR("%s:%d::unknown PON mode",__FUNCTION__,__LINE__);
        return RT_ERR_CHIP_NOT_SUPPORTED;
#endif
    }

    return RT_ERR_OK;
}

static int _rtl9607f_gpon_pm_chan_tuning1_get(rt_gpon_pm_counter_t *pPmCnt)
{
    if(CA_PON_MODE_IS_2D5GPON(gPonMode))
    {
        //FIXME
    }
    else
    {
#if defined(CONFIG_10G_GPON_FEATURE)
		// TODO: get conts from s/w stats
        pPmCnt->chanTuning1.tuning_control_requests_for_rx_only_or_rx_tx = 0;
        pPmCnt->chanTuning1.tuning_control_requests_for_tx_only = 0;
        pPmCnt->chanTuning1.tuning_control_requests_rejected_int_spc = 0;
        pPmCnt->chanTuning1.tuning_control_requests_rejected_ds_all = 0;
        pPmCnt->chanTuning1.tuning_control_requests_rejected_us_all = 0;
        pPmCnt->chanTuning1.tuning_control_requests_ok_target_channel = 0;
        pPmCnt->chanTuning1.tuning_control_requests_failed_to4_exp = 0;
        pPmCnt->chanTuning1.tuning_control_requests_failed_to5_exp = 0;
        pPmCnt->chanTuning1.tuning_control_requests_resolved_discret_ch = 0;
        pPmCnt->chanTuning1.tuning_control_requests_rollback_com_ds = 0;
        pPmCnt->chanTuning1.tuning_control_requests_rollback_ds_all = 0;
        pPmCnt->chanTuning1.tuning_control_requests_rollback_us_all = 0;
        pPmCnt->chanTuning1.tuning_control_requests_failed_reactivation = 0;
#else
        RTL9607F_LOG_ERR("%s:%d::unknown PON mode",__FUNCTION__,__LINE__);
        return RT_ERR_CHIP_NOT_SUPPORTED;
#endif
    }

    return RT_ERR_OK;
}

static int _rtl9607f_gpon_pm_chan_tuning2_get(rt_gpon_pm_counter_t *pPmCnt)
{
    ca_chan_tuning_pm2_t            stats  = {0};

    memset(&stats, 0x00, sizeof(ca_chan_tuning_pm2_t));  

    if(CA_PON_MODE_IS_2D5GPON(gPonMode))
    {
        //FIXME
    }
    else
    {
#if defined(CONFIG_10G_GPON_FEATURE)
		// TODO: get conts from s/w stats
        pPmCnt->chanTuning2.tuning_control_requests_rejected_ds_albl = 0;
        pPmCnt->chanTuning2.tuning_control_requests_rejected_ds_void = 0;
        pPmCnt->chanTuning2.tuning_control_requests_rejected_ds_part = 0;
        pPmCnt->chanTuning2.tuning_control_requests_rejected_ds_tunr = 0;
        pPmCnt->chanTuning2.tuning_control_requests_rejected_ds_lnrt = 0;
        pPmCnt->chanTuning2.tuning_control_requests_rejected_ds_lncd = 0;
        pPmCnt->chanTuning2.tuning_control_requests_rejected_us_albl = 0;
        pPmCnt->chanTuning2.tuning_control_requests_rejected_us_void = 0;
        pPmCnt->chanTuning2.tuning_control_requests_rejected_us_tunr = 0;
        pPmCnt->chanTuning2.tuning_control_requests_rejected_us_clbr = 0;
        pPmCnt->chanTuning2.tuning_control_requests_rejected_us_lktp = 0;
        pPmCnt->chanTuning2.tuning_control_requests_rejected_us_lnrt = 0;
        pPmCnt->chanTuning2.tuning_control_requests_rejected_us_lncd = 0;
#else
        RTL9607F_LOG_ERR("%s:%d::unknown PON mode",__FUNCTION__,__LINE__);
        return RT_ERR_CHIP_NOT_SUPPORTED;
#endif
    }

    return RT_ERR_OK;
}

static int _rtl9607f_gpon_pm_chan_tuning3_get(rt_gpon_pm_counter_t *pPmCnt)
{
    if(CA_PON_MODE_IS_2D5GPON(gPonMode))
    {
        //FIXME
    }
    else
    {
#if defined(CONFIG_10G_GPON_FEATURE)
		// TODO: get conts from s/w stats
        pPmCnt->chanTuning3.tuning_control_requests_rollback_ds_albl = 0;
        pPmCnt->chanTuning3.tuning_control_requests_rollback_ds_lktp = 0;
        pPmCnt->chanTuning3.tuning_control_requests_rollback_us_albl = 0;
        pPmCnt->chanTuning3.tuning_control_requests_rollback_us_void = 0;
        pPmCnt->chanTuning3.tuning_control_requests_rollback_us_tunr = 0;
        pPmCnt->chanTuning3.tuning_control_requests_rollback_us_lktp = 0;
        pPmCnt->chanTuning3.tuning_control_requests_rollback_us_lnrt = 0;
        pPmCnt->chanTuning3.tuning_control_requests_rollback_us_lncd = 0;
#else
        RTL9607F_LOG_ERR("%s:%d::unknown PON mode",__FUNCTION__,__LINE__);
        return RT_ERR_CHIP_NOT_SUPPORTED;
#endif
    }

    return RT_ERR_OK;
}

static int _rtl9607f_gpon_pm_omci_get(rt_gpon_pm_counter_t *pPmCnt)
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
        RTL9607F_LOG_ERR("%s:%d::get ONU OMCI count fail",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    pPmCnt->omci.omci_baseline_msg_count = stats.omci_baseline_msg_count;
    pPmCnt->omci.omci_extended_message_count = stats.omci_extended_message_count;
    pPmCnt->omci.omci_mic_error_count = stats.omci_mic_error_count;
#endif
    pPmCnt->omci.omci_rx_baseline_count     = g9607fOmciCount.omci_rx_baseline_count;
    pPmCnt->omci.omci_rx_extended_count     = g9607fOmciCount.omci_rx_extended_count;
    pPmCnt->omci.omci_rx_mic_error_count    = g9607fOmciCount.omci_rx_mic_error_count;
    /*Read clear*/
    g9607fOmciCount.omci_rx_baseline_count = 0;
    g9607fOmciCount.omci_rx_extended_count = 0;
    g9607fOmciCount.omci_rx_mic_error_count = 0;


    if(CA_PON_MODE_IS_2D5GPON(gPonMode))
    {
        aal_gpon_stats_t stats;
        aal_gpon_us_gem_port_mib_t gem_up;

        /* get OMCI counters from ds/us OMCI GEMs */
        if(CA_E_OK != aal_gpon_port_stats_get(SATURN_DEV_ID, SATURN_ANI_PORT_ID, &stats))
        {
            RTL9607F_LOG_ERR("%s:%d::get gpon stats fail",__FUNCTION__,__LINE__);
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
#if defined(CONFIG_10G_GPON_FEATURE)
        /* get OMCI counters from DSFP/USBG OMCI GEMs */
        count_dn[0] = 0;
        AAL_FOR_ALL_OMCI_XGEM_DO(gem) {
            memset(&gem_mib_dn, 0, sizeof(gem_mib_dn));
            aal_xgpon_dsfp_xgem_mib_get(0, gem, &gem_mib_dn);
            count_dn[0] += gem_mib_dn.pkt_cnt;
        }
    
        count_up[0] = 0;
        AAL_FOR_ALL_OMCI_XGEM_DO(gem) {
            memset(&gem_mib, 0, sizeof(gem_mib));
            aal_xgpon_usbg_xgem_mib_get(0, gem, &gem_mib);
            count_up[0] += gem_mib.pkt_cnt;
        }
    
        pPmCnt->omci.omci_tx_msg_count = count_up[0];
        pPmCnt->omci.omci_rx_msg_count = count_dn[0];
#else
        RTL9607F_LOG_ERR("%s:%d::unknown PON mode",__FUNCTION__,__LINE__);
        return RT_ERR_CHIP_NOT_SUPPORTED;
#endif
    }

    return RT_ERR_OK;
}

static int _rtl9607f_gpon_omcc_get(rt_gpon_omcc_t *pOmcc)
{
    int32 ret = RT_ERR_OK;

    if(CA_PON_MODE_IS_2D5GPON(gPonMode))
    {
        aal_gpon_onu_params_t  gpon_onu_param;

        memset(&gpon_onu_param, 0, sizeof(aal_gpon_onu_params_t));
        if (CA_E_OK != aal_gpon_onu_params_get(SATURN_DEV_ID, 0, &gpon_onu_param))
        {
            RTL9607F_LOG_ERR("%s:%d::Error gpon onu get fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if(0xFF == gpon_onu_param.onu_id)
            return RT_ERR_ENTRY_NOTFOUND;

        pOmcc->allocId = gpon_onu_param.onu_id;
        pOmcc->gemId = gpon_onu_param.omci_port;        
    }
    else
    {
#if defined(CONFIG_10G_GPON_FEATURE)
        aal_xgpon_bwmp_alloc_id_cfg_t aal_bwmp_cfg;
        aal_xgpon_usbg_gem_config_t us_xgem_cfg;

        memset(&aal_bwmp_cfg, 0, sizeof(aal_bwmp_cfg));
        memset(&us_xgem_cfg, 0, sizeof(aal_xgpon_usbg_gem_config_t));
    
        if (CA_E_OK != aal_xgpon_bwmp_tcont_get(SATURN_DEV_ID, 0, 0, &aal_bwmp_cfg))
        {
            RTL9607F_LOG_ERR("%s:%d::Error bwmp get fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    
        if(aal_bwmp_cfg.omci_en != 1 && aal_bwmp_cfg.ploam_en != 1)
            return RT_ERR_ENTRY_NOTFOUND;
    
        pOmcc->allocId = aal_bwmp_cfg.alloc_id;
    
        if (CA_E_OK != aal_xgpon_usbg_xgem_cfg_get(SATURN_DEV_ID, 0, &us_xgem_cfg))
        {
            RTL9607F_LOG_ERR("%s:%d::ERROR get usbg xgem fail",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    
        if(us_xgem_cfg.port_en !=1)
            return RT_ERR_ENTRY_NOTFOUND;
    
        pOmcc->gemId = us_xgem_cfg.xgem_id;
#else
        RTL9607F_LOG_ERR("%s:%d::unknown PON mode",__FUNCTION__,__LINE__);
        return RT_ERR_CHIP_NOT_SUPPORTED;
#endif
    }

    return ret;
}


/* Function Name:
 *      dal_rt_rtl9607f_gpon_deinit
 * Description:
 *      gpon register level de-initial function
 * Input:
 *      pon_mode    - current pon mode to be de-inited
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 dal_rt_rtl9607f_gpon_deinit(uint8 pon_mode)
{
    int32 ret = RT_ERR_OK;
    ca_uint32_t ppid = 0;

    if(((gpon_init!=INIT_COMPLETED))||(!CA_PON_MODE_IS_GPON_FAMILY(pon_mode)))
    {
        RT_ERR(ret, (MOD_DAL|MOD_GPON), "");
        printk("PON MODE 0x%d is note correct, or not inited\n", pon_mode);
        gpon_init = INIT_NOT_COMPLETED;
        return RT_ERR_FAILED;
    }

    gpon_init = INIT_NOT_COMPLETED;
    orig_gpon_init = INIT_NOT_COMPLETED;

    /* Disable PONMAC */
    ppid = PORT_ID(CA_PORT_GPON_PORT);
    __PON_LOCK();
    ret = aal_pon_mac_enable_set(SATURN_DEV_ID, ppid, 0, 0);
    __PON_UNLOCK();
    ca_mdelay(1500);
    g_admin_state = 0;

    if(aal_init_pon != 0)
    {
        /* PONMAC de-init */
        ret = ca_pon_exit(0);
        if(CA_E_OK != ret) {
            printk("%s:%d %s returns error, ret = %d\n",__FUNCTION__, __LINE__, "ca_pon_init()", ret);
            return RT_ERR_FAILED;
        }
    }

   /* Reset PONMAC */
   ret = aal_pon_deinit(SATURN_DEV_ID, pon_mode);

   return ret;
}

/* Function Name:
 *      dal_rt_rtl9607f_gpon_init
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
int32 dal_rt_rtl9607f_gpon_init(void)
{
    int ret;

#if defined(CONFIG_CORTINA_BOARD_FPGA) || defined(CONFIG_REALTEK_BOARD_FPGA)
    aal_init_debug_set(0,2,0);
    mdelay(1000);
//    aal_init_debug_set(0,3,5);
//    mdelay(1000);
#endif

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

#if defined(CONFIG_CORTINA_BOARD_FPGA) || defined(CONFIG_REALTEK_BOARD_FPGA)
    aal_init_debug_set(0,5,0);
    printk("%s %d FPGA init at GPON\n",__FUNCTION__,__LINE__);
#endif

    _dal_rtl9607f_gpon_db_init();
        

    /*reset GEM counters*/
    memset(g9607fDsGemCount, 0x0, sizeof(dal_gpon_ds_flow_counter_t)*AAL_XGPON_GEM_PORT_NUM);
    memset(g9607fUsGemCount, 0x0, sizeof(dal_gpon_us_flow_counter_t)*AAL_XGPON_GEM_PORT_NUM);


    /*set GPON DS Gem counter as read-clear */
    if(CA_PON_MODE_IS_2D5GPON(gPonMode))
    {
        aal_gpon_mac_cfg_msk_t gponmac_mask;
        aal_gpon_mac_cfg_t     gponmac_cfg;

        memset(&gponmac_mask, 0, sizeof(aal_gpon_mac_cfg_msk_t));
        memset(&gponmac_cfg, 0, sizeof(aal_gpon_mac_cfg_t));
        gponmac_mask.s.pm_mode = 1;
        gponmac_cfg.pm_mode = 2;

        if((ret = aal_gpon_mac_cfg_set(SATURN_DEV_ID, gponmac_mask, &gponmac_cfg)) != CA_E_OK)
            printk("%s:%d, ret = %d\n",__FUNCTION__, __LINE__, ret);
    }

    gpon_init = INIT_COMPLETED;
    return RT_ERR_OK;    
}   /* end of dal_rtl9607f_gpon_init */


/* Function Name:
 *      dal_rt_rtl9607f_gpon_serialNumber_set
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
dal_rt_rtl9607f_gpon_serialNumber_set(rt_gpon_serialNumber_t *pSN)
{
    ca_status_t ret = CA_E_OK; 
    aal_xgpn_plmp_rand_cfg_mask_t  rand_mask;
    aal_xgpn_plmp_rand_cfg_t       rand;
    ca_uint32_t random_num;

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pSN), RT_ERR_NULL_POINTER);    

    /* function body */

    /*the admin_state MUST be false before setting SN*/
    if(g_admin_state)
    {
        return RT_ERR_FAILED;
    }

    if(CA_PON_MODE_IS_2D5GPON(gPonMode))
    {
        g_xgpn_basic_info.vendor_id = 
            ((ca_uint32_t)pSN->vendor[0] << 24) | 
            ((ca_uint32_t)pSN->vendor[1] << 16) | 
            ((ca_uint32_t)pSN->vendor[2] << 8) | 
            ((ca_uint32_t)pSN->vendor[3]);

        g_xgpn_basic_info.vssn = 
            ((ca_uint32_t)pSN->specific[0] << 24) | 
            ((ca_uint32_t)pSN->specific[1] << 16) | 
            ((ca_uint32_t)pSN->specific[2] << 8) | 
            ((ca_uint32_t)pSN->specific[3]);

        ret = aal_gpon_vendor_id_set(SATURN_DEV_ID, g_xgpn_basic_info.vendor_id);
        if(CA_E_OK != ret)
        {
            RTL9607F_LOG_ERR("%s %d set gpon sn  fail\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        ret = aal_gpon_vssn_set(SATURN_DEV_ID, g_xgpn_basic_info.vssn);
        if(CA_E_OK != ret)
        {
            RTL9607F_LOG_ERR("%s %d set gpon sn  fail\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }
    else
    {
#if defined(CONFIG_10G_GPON_FEATURE)
        g_xgpn_basic_info.vendor_id = 
            ((ca_uint32_t)pSN->vendor[0] << 24) | 
            ((ca_uint32_t)pSN->vendor[1] << 16) | 
            ((ca_uint32_t)pSN->vendor[2] << 8) | 
            ((ca_uint32_t)pSN->vendor[3]);

        g_xgpn_basic_info.vssn = 
            ((ca_uint32_t)pSN->specific[0] << 24) | 
            ((ca_uint32_t)pSN->specific[1] << 16) | 
            ((ca_uint32_t)pSN->specific[2] << 8) | 
            ((ca_uint32_t)pSN->specific[3]);

        ret = aal_ploam_vendor_id_cfg_set(SATURN_DEV_ID, g_xgpn_basic_info.vendor_id);
        if(CA_E_OK != ret)
        {
            RTL9607F_LOG_ERR("%s %d set gpon sn  fail\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }


        ret = aal_ploam_vssn_cfg_set(SATURN_DEV_ID, g_xgpn_basic_info.vssn);
        if(CA_E_OK != ret)
        {
            RTL9607F_LOG_ERR("%s %d set gpon sn  fail\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        /* set rand_cfg for random delay seed */
        rand_mask.wrd = 0x0;
        memset(&rand, 0, sizeof(rand));
        rand_mask.bf.seed = 1;
        rand_mask.bf.enable = 1;
        get_random_bytes(&random_num, sizeof(random_num));
        rand.seed = random_num;
        rand.enable = 0;
        ret = aal_ploam_rand_cfg_set(SATURN_DEV_ID, rand_mask, &rand);
        if(CA_E_OK != ret)
        {
            RTL9607F_LOG_ERR("%s %d set rand_cfg.seed  fail\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        
        /* toggle and clear rand_cfg.en */
        rand_mask.wrd = 0x0;
        memset(&rand, 0, sizeof(rand));
        rand_mask.bf.enable = 1;
        rand.enable = 1;
        ret = aal_ploam_rand_cfg_set(SATURN_DEV_ID, rand_mask, &rand);
        if(CA_E_OK != ret)
        {
            RTL9607F_LOG_ERR("%s %d set rand_cfg.seed  fail\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        
        rand.enable = 0;
        ret = aal_ploam_rand_cfg_set(SATURN_DEV_ID, rand_mask, &rand);
        if(CA_E_OK != ret)
        {
            RTL9607F_LOG_ERR("%s %d set rand_cfg.seed  fail\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
#else
        RTL9607F_LOG_ERR("%s:%d::unknown PON mode",__FUNCTION__,__LINE__);
        return RT_ERR_CHIP_NOT_SUPPORTED;
#endif
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9607f_gpon_serialNumber_set */

/* Function Name:
 *      dal_rt_rtl9607f_gpon_serialNumber_get
 * Description:
 *      Get GPON serial number
 * Input:
 *      None
 * Output:
 *      pSN   - pointer to the GPON serial number
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32
dal_rt_rtl9607f_gpon_serialNumber_get(rt_gpon_serialNumber_t *pSN)
{
    ca_status_t ret = CA_E_OK;
    aal_xgpn_plmp_rand_cfg_mask_t  rand_mask;
    aal_xgpn_plmp_rand_cfg_t       rand;
    ca_uint32_t random_num;

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pSN), RT_ERR_NULL_POINTER);

    /* function body */
    if(CA_PON_MODE_IS_2D5GPON(gPonMode))
    {
        ret = aal_gpon_vendor_id_get(SATURN_DEV_ID, &(g_xgpn_basic_info.vendor_id));
        if(CA_E_OK != ret)
        {
            RTL9607F_LOG_ERR("%s %d set gpon sn  fail\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        ret = aal_gpon_vssn_get(SATURN_DEV_ID, &(g_xgpn_basic_info.vssn));
        if(CA_E_OK != ret)
        {
            RTL9607F_LOG_ERR("%s %d set gpon sn  fail\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        pSN->vendor[0] = (g_xgpn_basic_info.vendor_id >> 24) & 0xFF;
        pSN->vendor[1] = (g_xgpn_basic_info.vendor_id >> 16) & 0xFF;
        pSN->vendor[2] = (g_xgpn_basic_info.vendor_id >> 8) & 0xFF;
        pSN->vendor[3] = g_xgpn_basic_info.vendor_id & 0xFF;

        pSN->specific[0] = (g_xgpn_basic_info.vssn >> 24) & 0xFF;
        pSN->specific[1] = (g_xgpn_basic_info.vssn >> 16) & 0xFF;
        pSN->specific[2] = (g_xgpn_basic_info.vssn >> 8) & 0xFF;
        pSN->specific[3] = g_xgpn_basic_info.vssn & 0xFF;
    }
    else
    {
#if defined(CONFIG_10G_GPON_FEATURE)
        ret = aal_ploam_vendor_id_cfg_get(SATURN_DEV_ID, &(g_xgpn_basic_info.vendor_id));
        if(CA_E_OK != ret)
        {
            RTL9607F_LOG_ERR("%s %d get gpon sn  fail\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }


        ret = aal_ploam_vssn_cfg_get(SATURN_DEV_ID, &(g_xgpn_basic_info.vssn));
        if(CA_E_OK != ret)
        {
            RTL9607F_LOG_ERR("%s %d get gpon sn  fail\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        pSN->vendor[0] = (g_xgpn_basic_info.vendor_id >> 24) & 0xFF;
        pSN->vendor[1] = (g_xgpn_basic_info.vendor_id >> 16) & 0xFF;
        pSN->vendor[2] = (g_xgpn_basic_info.vendor_id >> 8) & 0xFF;
        pSN->vendor[3] = g_xgpn_basic_info.vendor_id & 0xFF;

        pSN->specific[0] = (g_xgpn_basic_info.vssn >> 24) & 0xFF;
        pSN->specific[1] = (g_xgpn_basic_info.vssn >> 16) & 0xFF;
        pSN->specific[2] = (g_xgpn_basic_info.vssn >> 8) & 0xFF;
        pSN->specific[3] = g_xgpn_basic_info.vssn & 0xFF;
#else
        RTL9607F_LOG_ERR("%s:%d::unknown PON mode",__FUNCTION__,__LINE__);
        return RT_ERR_CHIP_NOT_SUPPORTED;
#endif
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9607f_gpon_serialNumber_get */

/* Function Name:
 *      dal_rt_rtl9607f_gpon_registrationId_set
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
dal_rt_rtl9607f_gpon_registrationId_set(rt_gpon_registrationId_t *pRegId)
{
    ca_status_t   ret;
    aal_gpon_passwd_t passwd = {0};
    static int first_init = 1;

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pRegId), RT_ERR_NULL_POINTER);  

    if(first_init)
    {   //first init no need to wait PONMAC ready
        first_init = 0;
    }
    else
    {
        ca_mdelay(1500);
    }

    /*the admin_state MUST be false before setting registration ID*/
    if(g_admin_state)
    {
        RTL9607F_LOG_ERR("%s %d set gpon rgsd id fail\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if(CA_PON_MODE_IS_2D5GPON(gPonMode))
    {
        memcpy(g_gpon_basic_info.passwd, pRegId->regId, AAL_GPON_PORT_PASSWORD_LEN);
        memcpy(passwd.passwd, g_gpon_basic_info.passwd, AAL_GPON_PORT_PASSWORD_LEN);
    
        ret = aal_gpon_passwd_set(SATURN_DEV_ID, &passwd);
        if(CA_E_OK != ret)
        {
            RTL9607F_LOG_ERR("%s %d set gpon rgsd id fail\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }
    else
    {
#if defined(CONFIG_10G_GPON_FEATURE)
        memcpy(g_xgpn_basic_info.registration_id, pRegId->regId, RT_GPON_REGISTRATION_ID_LEN);
    
        ret = aal_ploam_rgst_id_cfg_set(SATURN_DEV_ID, (aal_xgpn_plmp_rgst_id_cfg_t *)g_xgpn_basic_info.registration_id);
        if(CA_E_OK != ret)
        {
            RTL9607F_LOG_ERR("%s %d set gpon rgsd id fail\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
#else
        RTL9607F_LOG_ERR("%s:%d::unknown PON mode",__FUNCTION__,__LINE__);
        return RT_ERR_CHIP_NOT_SUPPORTED;
#endif
    }
    return RT_ERR_OK;
}   /* end of dal_rt_rtl9607f_registrationId_set */

/* Function Name:
 *      dal_rt_rtl9607f_gpon_registrationId_get
 * Description:
 *      Get GPON registration ID (password in 984.3).
 * Input:
 *      None
 * Output:
 *      pRegId   - pointer to the GPON registration id
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32
dal_rt_rtl9607f_gpon_registrationId_get(rt_gpon_registrationId_t *pRegId)
{
    ca_status_t   ret;
    aal_gpon_passwd_t passwd = {0};

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pRegId), RT_ERR_NULL_POINTER);

    memset(pRegId->regId, 0, RT_GPON_REGISTRATION_ID_LEN);
    if(CA_PON_MODE_IS_2D5GPON(gPonMode))
    {
        ret = aal_gpon_passwd_get(SATURN_DEV_ID, &passwd);
        if(CA_E_OK != ret)
        {
            RTL9607F_LOG_ERR("%s %d set gpon rgsd id fail\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        memcpy(g_gpon_basic_info.passwd, passwd.passwd, AAL_GPON_PORT_PASSWORD_LEN);
        memcpy(pRegId->regId, g_gpon_basic_info.passwd, AAL_GPON_PORT_PASSWORD_LEN);
    }
    else
    {
#if defined(CONFIG_10G_GPON_FEATURE)
        ret = aal_ploam_rgst_id_cfg_get(SATURN_DEV_ID, (aal_xgpn_plmp_rgst_id_cfg_t *)g_xgpn_basic_info.registration_id);

        if(CA_E_OK != ret)
        {
            RTL9607F_LOG_ERR("%s %d get gpon rgsd id fail\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        memcpy(pRegId->regId, g_xgpn_basic_info.registration_id, RT_GPON_REGISTRATION_ID_LEN);
#else
        RTL9607F_LOG_ERR("%s:%d::unknown PON mode",__FUNCTION__,__LINE__);
        return RT_ERR_CHIP_NOT_SUPPORTED;
#endif
    }
    return RT_ERR_OK;
}   /* end of dal_rt_rtl9607f_registrationId_get */


/* Function Name:
 *      dal_rt_rtl9607f_gpon_activate
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
dal_rt_rtl9607f_gpon_activate(rt_gpon_initialState_t initState)
{
    ca_uint32_t ret = CA_E_OK;
    static int first_init = 1;
    ca_uint32_t ppid = 0;

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

    ppid = PORT_ID(CA_PORT_GPON_PORT);

    __PON_LOCK();
    if(initState == RT_GPON_ONU_INIT_STATE_O7)
        ret = aal_pon_mac_enable_set(SATURN_DEV_ID, ppid, 0, 1);
    else
        ret = aal_pon_mac_enable_set(SATURN_DEV_ID, ppid, 1, 1);
    __PON_UNLOCK();
    if (ret != CA_E_OK)
    {
        RTL9607F_LOG_ERR("set pon port mac enable failed. ret = %d\r\n", ret);
        return ret;
    }
    g_admin_state = 1;

    return RT_ERR_OK;

}   /* end of dal_rt_rtl9607f_activate */

/* Function Name:
 *      dal_rt_rtl9607f_gpon_deactivate
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
dal_rt_rtl9607f_gpon_deactivate(void)
{
    int32   ret;
    ca_uint32_t ppid = 0;

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* function body */
    //FIXME: remove CA API and seperate GPON/XGPON mode
    ppid = PORT_ID(CA_PORT_GPON_PORT);

    __PON_LOCK();
    ret = aal_pon_mac_enable_set(SATURN_DEV_ID, ppid, 0, 0);
    __PON_UNLOCK();
    if (ret != CA_E_OK)
    {
        RTL9607F_LOG_ERR("set pon port mac disable failed. ret = %d\r\n",  ret);
        return ret;
    }
    g_admin_state = 0;

    return RT_ERR_OK;

}   /* end of dal_rt_rtl9607f_gpon_deactivate */




/* Function Name:
 *      dal_rt_rtl9607f_gpon_onuState_get
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
int32 dal_rt_rtl9607f_gpon_onuState_get(rt_gpon_onuState_t  *pOnuState)
{
    int32 ret;
    rtk_port_t pon_port;
    ca_port_id_t port_id;
    ca_uint32_t port;
    ca_gpon_port_onu_states_t caGponState;
    aal_xgpon_actp_status_t sts;
    aal_gpon_onu_states_t gpon_sts;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "");

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pOnuState), RT_ERR_NULL_POINTER);

    /* function body */
    pon_port = HAL_GET_PON_PORT();
    port_id = CA_PORT_ID(CA_PORT_TYPE_GPON,pon_port);
    port = PORT_ID(port_id);

    memset(&sts, 0, sizeof(aal_xgpon_actp_status_t));

#if defined(CONFIG_10G_GPON_FEATURE)
    if(!CA_PON_MODE_IS_2D5GPON(gPonMode))
        ret =  aal_xgpon_actp_status_get(0, &sts);
    else
#endif
        ret = aal_gpon_onu_state_get(0, 0, &gpon_sts);
	
    if(ret != CA_E_OK)
    {
        RTL9607F_LOG_ERR( "Get ONU state fail, return failed ret value = %x\n",ret);
        return RT_ERR_FAILED;
    }
    ret = RT_ERR_OK;
 #if defined(CONFIG_10G_GPON_FEATURE)
   if(CA_PON_MODE == ONU_PON_MAC_XGPON1 || CA_PON_MODE == ONU_PON_MAC_XGSPON)
    {
        switch(sts.onu_state)
        {
            case 0:
                *pOnuState = RT_NGP2_ONU_STATE_O1_1;
                break;
            case 1:
                *pOnuState = RT_NGP2_ONU_STATE_O2_3;
                break;
            case 2:
                *pOnuState = RT_NGP2_ONU_STATE_O4;
                break;
            case 3:
                *pOnuState = RT_NGP2_ONU_STATE_O5_1;
                break;
            case 4:
                *pOnuState = RT_NGP2_ONU_STATE_O6;
                break;
            case 5:
                *pOnuState = RT_NGP2_ONU_STATE_O7;
                break;
            default:
				ret = RT_ERR_OUT_OF_RANGE;
                ca_printf("ERROR:Unknown state!!/r/n");
                break;
        }
    }
    if ((CA_PON_MODE == ONU_PON_MAC_NGPON2) || (CA_PON_MODE == ONU_PON_MAC_NGPON2_D10G))
    {
        switch(sts.onu_state)
        {
            case 0:
               *pOnuState = ((0 == sts.onu_sub_state) ? RT_NGP2_ONU_STATE_O1_1 : RT_NGP2_ONU_STATE_O1_2);
               break;
            case 1:
                *pOnuState = RT_NGP2_ONU_STATE_O2_3;
                break;
            case 2:
                *pOnuState = RT_NGP2_ONU_STATE_O4;
                break;
            case 3:
                *pOnuState = ((0 == sts.onu_sub_state) ? RT_NGP2_ONU_STATE_O5_1 : RT_NGP2_ONU_STATE_O5_2);
                break;
            case 4:
                *pOnuState = RT_NGP2_ONU_STATE_O6;
                break;
            case 5:
                *pOnuState = RT_NGP2_ONU_STATE_O7;
                break;
            case 6:
                *pOnuState = ((0 == sts.onu_sub_state) ? RT_NGP2_ONU_STATE_O8_1 : RT_NGP2_ONU_STATE_O8_2);
                break;
            case 7:
                *pOnuState = RT_NGP2_ONU_STATE_O9;
                break;

            default:
				ret = RT_ERR_OUT_OF_RANGE;
                ca_printf("ERROR:Unknown state!!/r/n");
                break;
        }
    }
#endif

    if(CA_PON_MODE_IS_2D5GPON(gPonMode))
    {
        switch(gpon_sts)
        {
            case 0:
                *pOnuState = RT_NGP2_ONU_STATE_O1_1;
                break;
            case 1:
                *pOnuState = RT_NGP2_ONU_STATE_O1_2;
                break;
            case 2:
                *pOnuState = RT_NGP2_ONU_STATE_O2_3;
                break;
            case 3:
                *pOnuState = RT_NGP2_ONU_STATE_O4;
                break;
            case 4:
                *pOnuState = RT_NGP2_ONU_STATE_O5_1;
                break;
            case 5:
                *pOnuState = RT_NGP2_ONU_STATE_O6;
                break;
            case 6:
                *pOnuState = RT_NGP2_ONU_STATE_O7;
                break;
            default:
				ret = RT_ERR_OUT_OF_RANGE;
                ca_printf("ERROR:Unknown state!!/r/n");
                break;
        }
    }
	
    if(ret!=RT_ERR_OK){
        RTL9607F_LOG_ERR( "The onu state is out of range\n");
        return RT_ERR_FAILED;
    }
    
    return RT_ERR_OK;
}   /* end of   dal_rtl9607f_gpon_onuState_get */


/* Function Name:
 *      dal_rt_rtl9607f_gpon_omci_tx
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
dal_rt_rtl9607f_gpon_omci_tx(uint32 msgLen, uint8 *pMsg)
{

    ca_pkt_t halPkt;
    ca_pkt_block_t pkt_data;
    int32 ret;
    struct sk_buff *p_skb;
    HEADER_A_T ni_header_a;
    ca_mac_addr_t puc_da = {0x00, 0x13, 0x25, 0x00, 0x00, 0x00};
    ca_mac_addr_t puc_sa = {0x00, 0x13, 0x25, 0x00, 0x00, 0x01};
    ca_int8_t etype_oam[]  = {0xff, 0xf1};

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pMsg), RT_ERR_NULL_POINTER);

#if defined(CONFIG_10G_GPON_FEATURE)
    if(CA_PON_MODE == ONU_PON_MAC_XGPON1 || CA_PON_MODE == ONU_PON_MAC_XGSPON || CA_PON_MODE == ONU_PON_MAC_NGPON2 || CA_PON_MODE == ONU_PON_MAC_NGPON2_D10G)
    {
        if(_rtl9607f_gpon_omci_build_mic(pMsg)!= RT_ERR_OK)
        {
            RTL9607F_LOG_ERR("%s %d : OMCI build mic Error!!",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }
    else if(CA_PON_MODE_IS_2D5GPON(gPonMode))
#else
    if(CA_PON_MODE_IS_2D5GPON(gPonMode))
#endif
    {
        if(_rtl9607f_gpon_omci_build_crc(pMsg)!= RT_ERR_OK)
        {
            RTL9607F_LOG_ERR("%s %d : OMCI build crc Error!!",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }
    else
    {
        RTL9607F_LOG_ERR("%s %d : Not support PON MODE!!",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*NIC driver may add extra 16 bytes header*/
    if(msgLen >= 48)
        p_skb = netdev_alloc_skb(NULL, msgLen+32);
    else
        p_skb = netdev_alloc_skb(NULL, 80);
    if (p_skb == NULL) 
    {
        RTL9607F_LOG_ERR("%s %d : netdev_alloc_skb fail!!",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    memcpy(p_skb->data,&puc_da[0],6);
    memcpy(p_skb->data+6,&puc_sa[0],6);
    memcpy(p_skb->data+12,&etype_oam[0],2);
    if(msgLen >= 48)
        p_skb->data[14]=0x00;
    else
        p_skb->data[14]=msgLen;

    if(pMsg[0] & 0x80)
        p_skb->data[15]=0x01;
    else
        p_skb->data[15]=0x00;
    
    memcpy(p_skb->data+16,pMsg,msgLen);

    if(msgLen >= 48)
        p_skb->len = msgLen+16;
    else
        p_skb->len = 64;

    memset(&ni_header_a, 0, sizeof(HEADER_A_T));

    ni_header_a.bits.cos = 7;
//#ifdef CONFIG_CORTINA_BOARD_FPGA
//    ni_header_a.bits.ldpid = 0x20;
//#else
    ni_header_a.bits.ldpid = 0xf;
//#endif
    ni_header_a.bits.lspid = 0x11; //For mirror
    ni_header_a.bits.pkt_size = msgLen+16;
    ni_header_a.bits.fe_bypass = 1;

    ni_header_a.bits.bits_32_63.pkt_info.no_drop = 1;

    ca_spin_lock(g_aal_gpon_flow_lock);
    __rtk_ni_start_xmit(p_skb, &ni_header_a, NULL , NULL , NULL, 1);
    ca_spin_unlock(g_aal_gpon_flow_lock);

    if(ENABLED == gOmciMirrorEnable)
    {
        _rtl9607f_gpon_omci_mirror(msgLen, pMsg);     
    }


    return RT_ERR_OK;

}


/* Function Name:
 *      dal_rt_rtl9607f_gpon_omci_rx_callback_register
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
dal_rt_rtl9607f_gpon_omci_rx_callback_register(rt_gpon_omci_rx_callback omciRx)
{
    int32   ret;
    rtk_port_t pon;
    int uniPortMask = 0, portmask = 0xffffffff;

    /* check Init status */
    RT_INIT_CHK(gpon_init);
    
    pon = HAL_GET_PON_PORT();
    uniPortMask = (1 << pon)-1;
    portmask &= (~uniPortMask);/*Remove UNI port mask*/

    /* function body */

    if(DISABLED == gOmciTrapEnable)
    {
        if((ret = nic_register_rxhook(portmask,RENIC_RXPRI_PATCH,_rtl9607f_receive_from_nic))!=RT_ERR_OK)
        {
            RTL9607F_LOG_ERR("nic_register_rxhook Error!!\n");
            return ret;
        }
        gOmciTrapEnable = ENABLED;
    }

    gOmciRxCallback = omciRx;
    
    return RT_ERR_OK;
}   /* end of dal_rt_rtl9607f_gpon_omci_rx_callback_register */


/* Function Name:
 *      dal_rt_rtl9607f_gpon_tcont_set
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
dal_rt_rtl9607f_gpon_tcont_set(uint32 *pTcontId, uint32 allocId)
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
        if(_GetNonUsedTcontId(allocId, &tcontId) != RT_ERR_OK)
        {
            RTL9607F_LOG_ERR("%s %d Get non used TcontId fail\n",
                __FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if (RT_ERR_OK == _rtl9607f_gpon_aal_tcont_set(LOGIC_TCONID_TO_RTL9607F_TCONID(tcontId), allocId))
        {

            //_AssignNonUsedTcontId(allocId, &tcontId);
            g9607fTcontTbl[tcontId].allocId = allocId;
            g9607fTcontTbl[tcontId].valid = 1;
            *pTcontId = tcontId;
        }
        else
        {
            RTL9607F_LOG_ERR("%s %d set ca gpon tcont fail\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }
    RTL9607F_LOG_DBG("%s %d allocId:%u tcontId:%d\n",__FUNCTION__,__LINE__,allocId,*pTcontId);  


    return RT_ERR_OK;

}


/* Function Name:
 *      dal_rt_rtl9607f_gpon_tcont_get
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
dal_rt_rtl9607f_gpon_tcont_get(uint32 tcontId, uint32 *pAllocId)
{

    uint32 allocId;
    
    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pAllocId), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((tcontId >= RTL9607F_MAX_TCONT), RT_ERR_INPUT);
   
    /*Check tcontID is exist*/
    if(RT_ERR_OK == _GetAllocIdByTcontId(tcontId, &allocId))
    {
        *pAllocId = allocId;
    }
    else
    {
        //RTL9607F_LOG_DBG("%s %d : no matched TCONT id [%d]\n", __FUNCTION__, __LINE__, tcontId);  
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_rt_rtl9607f_gpon_tcont_del
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
dal_rt_rtl9607f_gpon_tcont_del(uint32 tcontId)
{

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((tcontId >= RTL9607F_MAX_TCONT), RT_ERR_INPUT);

    if(RT_ERR_OK != _rtl9607f_gpon_aal_tcont_del(LOGIC_TCONID_TO_RTL9607F_TCONID(tcontId)))
    {
        RTL9607F_LOG_ERR("%s %d set del tcont fail\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if(RT_ERR_OK != _DelTcontId(tcontId))
    {
        RTL9607F_LOG_ERR("%s %d set del tcont fail\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;

}


/* Function Name:
 *      dal_rt_rtl9607f_gpon_usFlow_set
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
int32 dal_rt_rtl9607f_gpon_usFlow_set(uint32 usFlowId, const rt_gpon_usFlow_t *pUsFlow)
{
    unsigned short gemIdx = 0;
    rt_gpon_usFlow_t usFlowCfg;
    
    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pUsFlow), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((usFlowId >= RTL9607F_MAX_GEM_FLOW), RT_ERR_INPUT);
    RT_PARAM_CHK((pUsFlow->tcontId >= RTL9607F_MAX_TCONT && pUsFlow->tcontId!= RT_GPON_OMCC_TCONT_ID), RT_ERR_INPUT);
    RT_PARAM_CHK((pUsFlow->tcQueueId >= RTL9607F_MAX_TCONT_QUEUE), RT_ERR_INPUT);
    
    /* function body */

    if((TRUE == g9607fGemUsFlowMap[usFlowId].isValid) && 
        (g9607fGemUsFlowMap[usFlowId].tcQueueId == pUsFlow->tcQueueId) &&
        (g9607fGemUsFlowMap[usFlowId].tcontId) == LOGIC_TCONID_TO_RTL9607F_TCONID(pUsFlow->tcontId) &&
        g9607fGemUsFlowMap[usFlowId].aesEn == pUsFlow->aesEn)

    {        
        RTL9607F_LOG_ERR("%s %d Update gemFlowId:%d \n",
            __FUNCTION__,__LINE__,usFlowId);

        return RT_ERR_OK;
    }

    memcpy(&usFlowCfg, pUsFlow, sizeof(rt_gpon_usFlow_t));
    usFlowCfg.tcontId = LOGIC_TCONID_TO_RTL9607F_TCONID(pUsFlow->tcontId);
    if (RT_ERR_OK != _rtl9607f_gpon_aal_usGem_set(
                                usFlowId, 
                                &usFlowCfg, 
                                &gemIdx))
    {
        RTL9607F_LOG_ERR("%s %d set us flow fail\n",
            __FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }


    //save the US flow database
    if(RT_ERR_OK != _rtl9607f_SetGemFlowMap(GEM_FLOW_DIR_US, usFlowId, &usFlowCfg, gemIdx, pUsFlow->gemPortId))
    {
        RTL9607F_LOG_ERR("%s %d set gemflow map fail\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;            
    }

    // if set OMCC tcont queue 0-5, also set voq valid
    if((pUsFlow->tcontId == RT_GPON_OMCC_TCONT_ID) &&
       (pUsFlow->tcQueueId <= 5))
    {
        if(RT_ERR_OK != aal_puc_voq_valid_set(SATURN_DEV_ID, pUsFlow->tcQueueId, 1))
        {
            RTL9607F_LOG_ERR("%s %d set voq valid fail for OMCC voq_%d\n",__FUNCTION__,__LINE__, pUsFlow->tcQueueId);
            return RT_ERR_FAILED;
        }
    }

    return RT_ERR_OK;
}


 /* Function Name:
 *      dal_rt_rtl9607f_gpon_usFlow_get
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
int32 dal_rt_rtl9607f_gpon_usFlow_get(uint32 usFlowId, rt_gpon_usFlow_t *pUsFlow)
{
    uint32 logicTcontId = 0;

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pUsFlow), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((usFlowId >= RTL9607F_MAX_GEM_FLOW), RT_ERR_INPUT);
    
    /* function body */
    if(TRUE != g9607fGemUsFlowMap[usFlowId].isValid) 
    {        
        //RTL9607F_LOG_ERR("%s %d invalid us flow [%d]\n", __FUNCTION__, __LINE__, usFlowId);
        return RT_ERR_ENTRY_NOTFOUND;
    }

    if (RT_ERR_OK != _rtl9607f_gpon_aal_usGem_get(
                                usFlowId, 
                                pUsFlow))
    {
        RTL9607F_LOG_ERR("%s %d get us gem fail\n",
            __FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    

    if(IS_OMCC_TCONT(g9607fGemUsFlowMap[usFlowId].tcontId))
    {
        logicTcontId = RT_GPON_OMCC_TCONT_ID;
    }
    else
    {
       
        if(RT_ERR_OK != _rtl9607f_GetPhy2LogicTcontMap(g9607fGemUsFlowMap[usFlowId].tcontId, &logicTcontId))
        {
            RTL9607F_LOG_ERR("%s %d get phy:%u to logic T-cont fail\n",  
                __FUNCTION__, __LINE__, g9607fGemUsFlowMap[usFlowId].tcontId);
        
            return RT_ERR_FAILED;
        
        }
    }


    pUsFlow->tcontId        = logicTcontId;
    pUsFlow->tcQueueId      = g9607fGemUsFlowMap[usFlowId].tcQueueId;

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_rt_rtl9607f_gpon_usFlow_del
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
int32 dal_rt_rtl9607f_gpon_usFlow_del(uint32 usFlowId)
{
    uint32 tcontId=0, queueId=0;

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */    
    RT_PARAM_CHK((usFlowId >= RTL9607F_MAX_GEM_FLOW), RT_ERR_INPUT);

    RTL9607F_LOG_DBG("%s %d flowId:%d gemIdx:0x%x\n",
        __FUNCTION__,
        __LINE__,
        usFlowId,
        g9607fGemUsFlowMap[usFlowId].gemIdx);

    /* function body */

    /*Check the US flow is exist*/
    if(FALSE == g9607fGemUsFlowMap[usFlowId].isValid)
        return RT_ERR_OK;
    

    if (RT_ERR_OK != _rtl9607f_gpon_aal_usGem_del(usFlowId))
    {
        RTL9607F_LOG_ERR("%s %d del us flow fail\n",
            __FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    tcontId = g9607fGemUsFlowMap[usFlowId].tcontId;
    queueId = g9607fGemUsFlowMap[usFlowId].tcQueueId;

    if(RT_ERR_OK != _rtl9607f_DeleteGemFlowMap(usFlowId, GEM_FLOW_DIR_US))
    {
        RTL9607F_LOG_ERR("%s %d del gemflow map fail\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;            
    }
   
   // if del OMCC tcont queue 0-5, also set voq invalid
    if((tcontId == RT_GPON_OMCC_TCONT_ID) &&
       (queueId <= 5))
    {
        if(RT_ERR_OK != aal_puc_voq_valid_set(SATURN_DEV_ID, queueId, 0))
        {
            RTL9607F_LOG_ERR("%s %d set voq valid fail for OMCC voq_%d\n",__FUNCTION__,__LINE__, queueId);
            return RT_ERR_FAILED;
        }
    }

    return RT_ERR_OK;

}


/* Function Name:
 *      dal_rt_rtl9607f_gpon_usFlow_delAll
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
int32 dal_rt_rtl9607f_gpon_usFlow_delAll(void)
{
    int32 ret = RT_ERR_OK;
    uint32 i;

    /* check Init status */
    RT_INIT_CHK(gpon_init);
    
    for(i=0;i<RTL9607F_MAX_GEM_FLOW;i++)
    {
        /*Delete DS gem flow*/
        if(TRUE == g9607fGemUsFlowMap[i].isValid)
        {
            if(RT_ERR_OK != dal_rt_rtl9607f_gpon_usFlow_del(i))
            {
                ret = RT_ERR_FAILED;
            }
                
        }
    }

    return RT_ERR_OK;

}


/* Function Name:
 *      dal_rt_rtl9607f_gpon_dsFlow_set
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
int32 dal_rt_rtl9607f_gpon_dsFlow_set(uint32 dsFlowId, const rt_gpon_dsFlow_t *pDsFlow)
{
    unsigned short gemIdx = 0;
    
    RTL9607F_LOG_DBG(
        "%s %d flowId:%d gemportId:%d\n",
        __FUNCTION__,__LINE__,
        dsFlowId,
        pDsFlow->gemPortId);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pDsFlow), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((dsFlowId >= RTL9607F_MAX_GEM_FLOW), RT_ERR_INPUT);

    /* function body */
    if (RT_ERR_OK != _rtl9607f_gpon_aal_dsGem_set(
                                dsFlowId, 
                                pDsFlow, 
                                &gemIdx))
    {
        RTL9607F_LOG_ERR("%s %d set ds flow fail\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

        //save the gemport index
    if(RT_ERR_OK != _rtl9607f_SetGemFlowMap(GEM_FLOW_DIR_DS, dsFlowId, NULL, gemIdx, pDsFlow->gemPortId))
    {
        RTL9607F_LOG_ERR("%s %d set gemflow map fail\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;            
    }
    
    return RT_ERR_OK;
}


/* Function Name:
 *      dal_rt_rtl9607f_gpon_dsFlow_get
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
int32 dal_rt_rtl9607f_gpon_dsFlow_get(uint32 dsFlowId, rt_gpon_dsFlow_t *pDsFlow)
{
    
    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pDsFlow), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((dsFlowId >= RTL9607F_MAX_GEM_FLOW), RT_ERR_INPUT);

    /* function body */
    if(TRUE != g9607fGemDsFlowMap[dsFlowId].isValid)
    {
        //RTL9607F_LOG_ERR("%s %d invalid ds flow [%d]\n", __FUNCTION__, __LINE__, dsFlowId);
        return RT_ERR_ENTRY_NOTFOUND;
    }
    if (RT_ERR_OK != _rtl9607f_gpon_aal_dsGem_get(
                                dsFlowId, 
                                pDsFlow))
    {
        RTL9607F_LOG_ERR("%s %d get ds flow fail\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    
    return RT_ERR_OK;
}


/* Function Name:
 *      dal_rt_rtl9607f_gpon_dsFlow_del
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
int32 dal_rt_rtl9607f_gpon_dsFlow_del(uint32 dsFlowId)
{

    RTL9607F_LOG_DBG(
        "%s %d flowId:%d\n",
        __FUNCTION__,__LINE__,
        dsFlowId);

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */    
    RT_PARAM_CHK((dsFlowId >= RTL9607F_MAX_GEM_FLOW), RT_ERR_INPUT);

    RTL9607F_LOG_DBG(
        "%s %d gemIdx:0x%x\n",
        __FUNCTION__,__LINE__,
        g9607fGemDsFlowMap[dsFlowId].gemIdx);
    
    /*Check the DS flow is exist*/
    if(FALSE == g9607fGemDsFlowMap[dsFlowId].isValid)
        return RT_ERR_ENTRY_NOTFOUND;


    /*Recovery loop GEM setting before delete ds gem*/
    if(TRUE == g9607fGemDsFlowMap[dsFlowId].loopEn)
    {

        if (RT_ERR_OK != _rtl9607f_gpon_aal_loopGem_set(FALSE, 
                LOGIC_FLOW_TO_RTL9607F_FLOW(dsFlowId),
                0,
                g9607fGemDsFlowMap[dsFlowId].loopLspid))
        {
            RTL9607F_LOG_ERR("%s %d disable loop gem fail,dsFlowId=%d \n",__FUNCTION__,__LINE__, dsFlowId);
            return RT_ERR_FAILED;
        }
    }


    if (RT_ERR_OK != _rtl9607f_gpon_aal_dsGem_del(dsFlowId))
    {
        RTL9607F_LOG_ERR("%s %d del ds flow fail\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if(RT_ERR_OK != _rtl9607f_DeleteGemFlowMap(dsFlowId, GEM_FLOW_DIR_DS))
    {
        RTL9607F_LOG_ERR("%s %d set us gemflow map fail\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;            
    }
    
    return RT_ERR_OK;
}


/* Function Name:
 *      dal_rt_rtl9607f_gpon_dsFlow_delAll
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
int32 dal_rt_rtl9607f_gpon_dsFlow_delAll(void)
{
    int ret = RT_ERR_OK;
    unsigned int i;


    /* check Init status */
    RT_INIT_CHK(gpon_init);
    
    for(i=0;i<RTL9607F_MAX_GEM_FLOW;i++)
    {
        /*Delete DS gem flow*/
        if(TRUE == g9607fGemDsFlowMap[i].isValid)
        {
            if(RT_ERR_OK != dal_rt_rtl9607f_gpon_dsFlow_del(i))
            {
                ret = RT_ERR_FAILED;
            }
        }
    }

    return ret;

}

/* Function Name:
 *      dal_rt_rtl9607f_loop_gemport_set
 * Description:
 *      Set GPON downstream flow
 * Input:
 *      gemPortId   - GEM port ID
 *      enable    - Enable/disble loop GEM port function
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_INPUT                - Invalid input parameters.
 *      RT_ERR_ENTRY_NOTFOUND       - specified entry not found
 * Note:
 */
int32 dal_rt_rtl9607f_loop_gemPort_set(uint32 gemPortId, rt_enable_t enable)
{
    unsigned short pdcGemIdx = 0;
    unsigned short usGemIdx = 0;
    unsigned short lspid = 0;
    unsigned int i,dsFlowId;
    unsigned char isFound = FALSE;   

    /* check Init status */
    RT_INIT_CHK(gpon_init);


    /*Check if downstream GEM exist*/
    isFound = FALSE;
    for(i=0;i<RTL9607F_MAX_GEM_FLOW;i++)
    {
        if(TRUE == g9607fGemDsFlowMap[i].isValid && 
            gemPortId == g9607fGemDsFlowMap[i].gemPortId)
        {
            isFound = TRUE;
            dsFlowId = i;
            pdcGemIdx = g9607fGemDsFlowMap[i].gemIdx; 
            
            RTL9607F_LOG_DBG("%s %d dsFlowId=%d, pdcGemIdx=%d",
                __FUNCTION__, __LINE__, dsFlowId , pdcGemIdx);
            break;

        }
    }

    if(!isFound)
        return RT_ERR_ENTRY_NOTFOUND;
    
    /*Check if upstream GEM exist*/
    isFound = FALSE;
    for(i=0;i<RTL9607F_MAX_GEM_FLOW;i++)
    {
        /*Delete DS gem flow*/
        if(TRUE == g9607fGemUsFlowMap[i].isValid &&
            gemPortId == g9607fGemUsFlowMap[i].gemPortId)
        {
            isFound = TRUE;
            usGemIdx = g9607fGemUsFlowMap[i].gemIdx;
            lspid = RTL9607F_TCONID_TO_LDPID(g9607fGemUsFlowMap[i].tcontId);

            RTL9607F_LOG_DBG("%s %d usGemIdx=%d, lspid=0x%X",
                __FUNCTION__, __LINE__, usGemIdx , lspid);

            break;
        }
    }

    if(!isFound)
        return RT_ERR_ENTRY_NOTFOUND; 


    if (RT_ERR_OK != _rtl9607f_gpon_aal_loopGem_set(enable, 
            pdcGemIdx,
            usGemIdx,
            lspid))
    {
        RTL9607F_LOG_ERR("%s %d set loop gem fail,dsFlowId=%d \n",__FUNCTION__,__LINE__, gemPortId);
        return RT_ERR_FAILED;
    }

    g9607fGemDsFlowMap[dsFlowId].loopEn = enable;
    g9607fGemDsFlowMap[dsFlowId].loopLspid = enable? lspid: 7;

   
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rt_rtl9607f_loop_gemPort_get
 * Description:
 *      Set GPON downstream flow
 * Input:
 *      gemPortId   - GEM port ID
 *
 * Output:
 *      pEnable    - Enable/disble loop GEM port function
 *
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_INPUT                - Invalid input parameters.
 *      RT_ERR_ENTRY_NOTFOUND       - specified entry not found
 * Note:
 */
int32 dal_rt_rtl9607f_loop_gemPort_get(uint32 gemPortId, rt_enable_t *pEnable)
{
    unsigned int i;

    /* check Init status */
    RT_INIT_CHK(gpon_init);


    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);
    
    /*Check if downstream GEM exist*/
    *pEnable = 0;
    for(i=0;i<RTL9607F_MAX_GEM_FLOW;i++)
    {
        if(TRUE == g9607fGemDsFlowMap[i].isValid && 
            gemPortId == g9607fGemDsFlowMap[i].gemPortId)
        {
            *pEnable = g9607fGemDsFlowMap[i].loopEn; 
            return RT_ERR_OK;

        }
    }

    return RT_ERR_ENTRY_NOTFOUND;
}


/* Function Name:
 *      dal_rt_rtl9607f_gpon_ponQueue_set
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
int32 dal_rt_rtl9607f_gpon_ponQueue_set(
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
    RT_PARAM_CHK((tcontId >= RTL9607F_MAX_TCONT), RT_ERR_INPUT);
    RT_PARAM_CHK((tcQueueId >= RTL9607F_MAX_TCONT_QUEUE), RT_ERR_INPUT);

    queue.schedulerId = tcontId;
    queue.queueId = tcQueueId;

    queueCfg.cir = (RT_GPON_PON_QUEUE_PIR_UNLIMITED == pQueuecfg->cir)?0:RATE_8KBPS_TO_KBPS(pQueuecfg->cir);
    queueCfg.pir = (RT_GPON_PON_QUEUE_PIR_UNLIMITED == pQueuecfg->pir)?0:RATE_8KBPS_TO_KBPS(pQueuecfg->pir);
    queueCfg.egrssDrop = DISABLED;
    queueCfg.type = (RT_GPON_TCONT_QUEUE_SCHEDULER_SP == pQueuecfg->scheduleType)?STRICT_PRIORITY:WFQ_WRR_PRIORITY;
    queueCfg.weight = (RT_GPON_TCONT_QUEUE_SCHEDULER_SP == pQueuecfg->scheduleType)?0:pQueuecfg->weight;

    ret = dal_rtl9607f_ponmac_queue_add(&queue, &queueCfg);

    if(RT_ERR_OK != ret)
    {
        RTL9607F_LOG_ERR("%s %d dal_rtl9607f_ponmac_queue_add fail \n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;

}


/* Function Name:
 *      dal_rt_rtl9607f_gpon_ponQueue_get
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
int32 dal_rt_rtl9607f_gpon_ponQueue_get(
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
    RT_PARAM_CHK((tcontId >= RTL9607F_MAX_TCONT), RT_ERR_INPUT);
    RT_PARAM_CHK((tcQueueId >= RTL9607F_MAX_TCONT_QUEUE), RT_ERR_INPUT);

    queue.schedulerId = tcontId;
    queue.queueId = tcQueueId;

    memset(&queueCfg, 0x0, sizeof(rtk_ponmac_queueCfg_t));

    ret = dal_rtl9607f_ponmac_queue_get(&queue, &queueCfg);

    if(RT_ERR_OK != ret)
    {
        //RTL9607F_LOG_ERR("%s %d dal_rtl9607f_ponmac_queue_get fail \n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    
    pQueuecfg->cir = (RTL9607F_METER_RATE_MAX == queueCfg.cir)?RT_GPON_PON_QUEUE_PIR_UNLIMITED:RATE_KBPS_TO_8KBPS(queueCfg.cir);
    pQueuecfg->pir = (RTL9607F_METER_RATE_MAX == queueCfg.pir)?RT_GPON_PON_QUEUE_PIR_UNLIMITED:RATE_KBPS_TO_8KBPS(queueCfg.pir);
    pQueuecfg->scheduleType = (STRICT_PRIORITY == queueCfg.type)?RT_GPON_TCONT_QUEUE_SCHEDULER_SP:RT_GPON_TCONT_QUEUE_SCHEDULER_WRR;    
    pQueuecfg->weight = queueCfg.weight;    

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_rt_rtl9607f_gpon_ponQueue_del
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
int32 dal_rt_rtl9607f_gpon_ponQueue_del(    
    uint32 tcontId, 
    uint32 tcQueueId)
{
    rtk_ponmac_queue_t queue;

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((tcontId >= RTL9607F_MAX_TCONT), RT_ERR_INPUT);
    RT_PARAM_CHK((tcQueueId >= RTL9607F_MAX_TCONT_QUEUE), RT_ERR_INPUT);

    queue.schedulerId = tcontId;
    queue.queueId = tcQueueId;

    return dal_rtl9607f_ponmac_queue_del(&queue);

}



/* Function Name:
 *      dal_rt_rtl9607f_gpon_scheInfo_get
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
dal_rt_rtl9607f_gpon_scheInfo_get(rt_gpon_schedule_info_t *pScheInfo)
{
    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pScheInfo), RT_ERR_NULL_POINTER);

    pScheInfo->max_flow         = RTL9607F_MAX_GEM_FLOW;
    pScheInfo->max_pon_queue    = RTL9607F_MAX_PON_QUEUE;
    pScheInfo->max_tcont        = RTL9607F_MAX_TCONT;
    pScheInfo->max_tcon_queue   = RTL9607F_MAX_TCONT_QUEUE;


    return RT_ERR_OK;
}   



/* Function Name:
 *      dal_rt_rtl9607f_gpon_flowCounter_get
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
dal_rt_rtl9607f_gpon_flowCounter_get(uint32 flowId, rt_gpon_flow_counter_type_t flowCntType, rt_gpon_flow_counter_t *pFlowCnt)
{
    uint32 internal_gem_idx;
    ca_status_t ret = CA_E_OK;
    
    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((flowId >= RTL9607F_MAX_GEM_FLOW), RT_ERR_INPUT);
    RT_PARAM_CHK((flowCntType >= RT_GPON_CNT_TYPE_FLOW_MAX), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pFlowCnt), RT_ERR_NULL_POINTER);

    memset(pFlowCnt, 0x0, sizeof(rt_gpon_flow_counter_t));

    if(CA_PON_MODE_IS_2D5GPON(gPonMode))
    {
        aal_gpon_ds_gem_port_mib_t ds_gem;
        aal_gpon_us_gem_port_mib_t us_gem;

        switch(flowCntType) 
        {
            case RT_GPON_CNT_TYPE_FLOW_DS_GEM:
            {
                if(g9607fGemDsFlowMap[flowId].isValid == TRUE)
                {
                internal_gem_idx = g9607fGemDsFlowMap[flowId].gemIdx;

                    memset(&ds_gem, 0x0, sizeof(aal_gpon_ds_gem_port_mib_t));

                    if (CA_E_OK != aal_gpon_ds_gem_port_mib_get(SATURN_DEV_ID, internal_gem_idx, 2, &ds_gem))
                    {
                        RTL9607F_LOG_ERR("%s:%d::get ONU DS GEM count %d fail",__FUNCTION__,__LINE__,internal_gem_idx);
                        return RT_ERR_FAILED;
                    }
                    pFlowCnt->dsgem.gem_block = ds_gem.fcnt + g9607fDsGemCount[internal_gem_idx].dsgem.gem_block;
                    pFlowCnt->dsgem.gem_byte = ds_gem.bcnt + g9607fDsGemCount[internal_gem_idx].dsgem.gem_byte;

                    g9607fDsGemCount[internal_gem_idx].dseth.eth_pkt_fwd = ds_gem.pcnt;
                    g9607fDsGemCount[internal_gem_idx].dsgem.gem_block = 0;
                    g9607fDsGemCount[internal_gem_idx].dsgem.gem_byte = 0;

                }
                break;
            }
            case RT_GPON_CNT_TYPE_FLOW_US_GEM:
            {
                if(g9607fGemUsFlowMap[flowId].isValid == TRUE)
                {
                    internal_gem_idx = g9607fGemUsFlowMap[flowId].gemIdx;
    
                    memset(&us_gem, 0x0, sizeof(aal_gpon_us_gem_port_mib_t));
                    ret = aal_gpon_us_gem_port_mib_get(SATURN_DEV_ID, AAL_GPON_PLOAM_MIB_READ_CLR, internal_gem_idx, &us_gem);
                    if(CA_E_OK == ret)
                    {
                        pFlowCnt->usgem.gem_block = us_gem.fcnt + g9607fUsGemCount[internal_gem_idx].usgem.gem_block;
                        pFlowCnt->usgem.gem_byte = us_gem.bcnt + g9607fUsGemCount[internal_gem_idx].usgem.gem_byte;
        
                        g9607fUsGemCount[internal_gem_idx].useth.eth_cnt = us_gem.pcnt;
                        g9607fUsGemCount[internal_gem_idx].usgem.gem_block = 0;
                        g9607fUsGemCount[internal_gem_idx].usgem.gem_byte = 0;
                    }
                }

                break;
            }
            case RT_GPON_CNT_TYPE_FLOW_DS_ETH:
            {
                if(g9607fGemDsFlowMap[flowId].isValid == TRUE)
                {
                    internal_gem_idx = g9607fGemDsFlowMap[flowId].gemIdx;

                    memset(&ds_gem, 0x0, sizeof(aal_gpon_ds_gem_port_mib_t));

                    if (CA_E_OK != aal_gpon_ds_gem_port_mib_get(SATURN_DEV_ID, internal_gem_idx, 2, &ds_gem))
                    {
                        RTL9607F_LOG_ERR("%s:%d::get ONU DS GEM count %d fail",__FUNCTION__,__LINE__,internal_gem_idx);
                        return RT_ERR_FAILED;
                    }
                    pFlowCnt->dseth.eth_pkt_fwd = ds_gem.pcnt + g9607fDsGemCount[internal_gem_idx].dseth.eth_pkt_fwd;

                    g9607fDsGemCount[internal_gem_idx].dsgem.gem_block = ds_gem.fcnt;
                    g9607fDsGemCount[internal_gem_idx].dsgem.gem_byte = ds_gem.bcnt;
                    g9607fDsGemCount[internal_gem_idx].dseth.eth_pkt_fwd = 0;
                }
                break;
            }
            case RT_GPON_CNT_TYPE_FLOW_US_ETH:
            {
                if(g9607fGemUsFlowMap[flowId].isValid == TRUE)
                {
                    internal_gem_idx = g9607fGemUsFlowMap[flowId].gemIdx;

                    memset(&us_gem, 0x0, sizeof(aal_gpon_us_gem_port_mib_t));
                    ret = aal_gpon_us_gem_port_mib_get(SATURN_DEV_ID, AAL_GPON_PLOAM_MIB_READ_CLR, internal_gem_idx, &us_gem);
                    if(CA_E_OK == ret)
                    {
                        pFlowCnt->useth.eth_cnt = us_gem.pcnt + g9607fUsGemCount[internal_gem_idx].useth.eth_cnt;

                        g9607fUsGemCount[internal_gem_idx].usgem.gem_block = us_gem.fcnt;
                        g9607fUsGemCount[internal_gem_idx].usgem.gem_byte = us_gem.bcnt;
                        g9607fUsGemCount[internal_gem_idx].useth.eth_cnt = 0;
                    }
                }
                break;
            }
            default:
                ret = CA_E_ERROR;
                RTL9607F_LOG_ERR("%s %d un-support flowCntType:%d\n",__FUNCTION__,__LINE__,flowCntType);
                break;
        }
    }
    else
    {
#if defined(CONFIG_10G_GPON_FEATURE)
        aal_xgpon_dsfp_xgem_mib_config_t dsfp_xgem_mib;
        aal_xgpon_usbg_xgem_mib_config_t usbg_xgem_mib;

        switch(flowCntType) 
        {
            case RT_GPON_CNT_TYPE_FLOW_DS_GEM:
            {
                    if(g9607fGemDsFlowMap[flowId].isValid == TRUE)
                    {
                        internal_gem_idx = g9607fGemDsFlowMap[flowId].gemIdx;
        
            memset(&dsfp_xgem_mib, 0x0, sizeof(aal_xgpon_dsfp_xgem_mib_config_t));
                ret = aal_xgpon_dsfp_xgem_mib_get(SATURN_DEV_ID,internal_gem_idx&0xFF,&dsfp_xgem_mib);
                    if(CA_E_OK == ret)
                    {
                        pFlowCnt->dsgem.gem_block = dsfp_xgem_mib.frame_cnt + g9607fDsGemCount[internal_gem_idx].dsgem.gem_block;
                        pFlowCnt->dsgem.gem_byte = dsfp_xgem_mib.byte_cnt + g9607fDsGemCount[internal_gem_idx].dsgem.gem_byte;
    
                        g9607fDsGemCount[internal_gem_idx].dseth.eth_pkt_fwd = dsfp_xgem_mib.pkt_cnt;
                        g9607fDsGemCount[internal_gem_idx].dsgem.gem_block = 0;
                        g9607fDsGemCount[internal_gem_idx].dsgem.gem_byte = 0;
                    }
                }
                break;
            }
            case RT_GPON_CNT_TYPE_FLOW_US_GEM:
            {
                    if(g9607fGemUsFlowMap[flowId].isValid == TRUE)
                    {
                        internal_gem_idx = g9607fGemUsFlowMap[flowId].gemIdx;
        
            memset(&usbg_xgem_mib, 0x0, sizeof(aal_xgpon_usbg_xgem_mib_config_t));
                    ret = aal_xgpon_usbg_xgem_mib_get(SATURN_DEV_ID,internal_gem_idx&0xFF,&usbg_xgem_mib);
                    if(CA_E_OK == ret)
                    {
                        pFlowCnt->usgem.gem_block = usbg_xgem_mib.frame_cnt + g9607fUsGemCount[internal_gem_idx].usgem.gem_block;
                        pFlowCnt->usgem.gem_byte = usbg_xgem_mib.byte_cnt + g9607fUsGemCount[internal_gem_idx].usgem.gem_byte;
    
                        g9607fUsGemCount[internal_gem_idx].useth.eth_cnt = usbg_xgem_mib.pkt_cnt;
                        g9607fUsGemCount[internal_gem_idx].usgem.gem_block = 0;
                        g9607fUsGemCount[internal_gem_idx].usgem.gem_byte = 0;
                    }
                }
                break;
            }
            case RT_GPON_CNT_TYPE_FLOW_DS_ETH:
            {
                if(g9607fGemDsFlowMap[flowId].isValid == TRUE)
                {
                    internal_gem_idx = g9607fGemDsFlowMap[flowId].gemIdx;
    
                    memset(&dsfp_xgem_mib, 0x0, sizeof(aal_xgpon_dsfp_xgem_mib_config_t));
                    ret = aal_xgpon_dsfp_xgem_mib_get(SATURN_DEV_ID,internal_gem_idx&0xFF,&dsfp_xgem_mib);
                    if(CA_E_OK == ret)
                    {
                        pFlowCnt->dseth.eth_pkt_fwd = dsfp_xgem_mib.pkt_cnt + g9607fDsGemCount[internal_gem_idx].dseth.eth_pkt_fwd;
    
                        g9607fDsGemCount[internal_gem_idx].dsgem.gem_block = dsfp_xgem_mib.frame_cnt;
                        g9607fDsGemCount[internal_gem_idx].dsgem.gem_byte = dsfp_xgem_mib.byte_cnt;
                        g9607fDsGemCount[internal_gem_idx].dseth.eth_pkt_fwd = 0;
                    }
                }
                break;
            }
            case RT_GPON_CNT_TYPE_FLOW_US_ETH:
            {
                if(g9607fGemUsFlowMap[flowId].isValid == TRUE)
                {
                    internal_gem_idx = g9607fGemUsFlowMap[flowId].gemIdx;
    
                    memset(&usbg_xgem_mib, 0x0, sizeof(aal_xgpon_usbg_xgem_mib_config_t));
                    ret = aal_xgpon_usbg_xgem_mib_get(SATURN_DEV_ID,internal_gem_idx&0xFF,&usbg_xgem_mib);
                    if(CA_E_OK == ret)
                    {
                        pFlowCnt->useth.eth_cnt = usbg_xgem_mib.pkt_cnt + g9607fUsGemCount[internal_gem_idx].useth.eth_cnt;

                        g9607fUsGemCount[internal_gem_idx].usgem.gem_block = usbg_xgem_mib.frame_cnt;
                        g9607fUsGemCount[internal_gem_idx].usgem.gem_byte = usbg_xgem_mib.byte_cnt;
                        g9607fUsGemCount[internal_gem_idx].useth.eth_cnt = 0;
                    }
                }
                break;
            }
            default:
                ret = CA_E_ERROR;
                RTL9607F_LOG_ERR("%s %d un-support flowCntType:%d\n",__FUNCTION__,__LINE__,flowCntType);
                break;
        }
#else
        RTL9607F_LOG_ERR("%s:%d::unknown PON mode",__FUNCTION__,__LINE__);
        return RT_ERR_CHIP_NOT_SUPPORTED;
#endif
    }

    if(CA_E_OK == ret)
        return RT_ERR_OK;
    else
        return RT_ERR_FAILED;
    
}   /* end of dal_rt_rtl9607f_gpon_flowCounter_get */


/* Function Name:
 *      dal_rt_rtl9607f_gpon_pmCounter_get
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
dal_rt_rtl9607f_gpon_pmCounter_get(rt_gpon_pm_type_t pmCntType, rt_gpon_pm_counter_t *pPmCnt)
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
            ret = _rtl9607f_gpon_pm_phyLos_get(pPmCnt);
        break;
        case RT_GPON_PM_TYPE_FEC:
            ret = _rtl9607f_gpon_pm_fec_get(pPmCnt);
        break;
        case RT_GPON_PM_TYPE_XGEM:
            ret = _rtl9607f_gpon_pm_xgem_get(pPmCnt);
        break;
        case RT_GPON_PM_TYPE_PLOAM_PM1:
            ret = _rtl9607f_gpon_pm_ploam1_get(pPmCnt);
        break;
        case RT_GPON_PM_TYPE_PLOAM_PM2:
            ret = _rtl9607f_gpon_pm_ploam2_get(pPmCnt);
        break;
        case RT_GPON_PM_TYPE_PLOAM_PM3:
            ret = _rtl9607f_gpon_pm_ploam3_get(pPmCnt);
        break;
        case RT_GPON_PM_TYPE_CHAN_TUNING_PM1:
            ret = _rtl9607f_gpon_pm_chan_tuning1_get(pPmCnt);
        break;
        case RT_GPON_PM_TYPE_CHAN_TUNING_PM2:
            ret = _rtl9607f_gpon_pm_chan_tuning2_get(pPmCnt);
        break;
        case RT_GPON_PM_TYPE_CHAN_TUNING_PM3:
            ret = _rtl9607f_gpon_pm_chan_tuning3_get(pPmCnt);
        break;
        case RT_GPON_PM_TYPE_OMCI:
            ret = _rtl9607f_gpon_pm_omci_get(pPmCnt);
        break;
        default:
        break;
    }

    if(CA_E_OK == ret)
        return RT_ERR_OK;
    else
        return RT_ERR_FAILED;
    
}   /* end of dal_rt_rtl9607f_gpon_flowCounter_get */


/* Function Name:
 *      dal_rt_rtl9607f_gpon_ponTag_get
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
dal_rt_rtl9607f_gpon_ponTag_get(rt_gpon_ponTag_t *pPonTag)
{
    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPonTag), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((CA_PON_MODE_IS_2D5GPON(gPonMode)), RT_ERR_FAILED);

#if defined(CONFIG_10G_GPON_FEATURE)
    memcpy(pPonTag->tagData, g_pon_tag, RT_GPON_PONTAG_LEN);
#else
    RTL9607F_LOG_ERR("%s:%d::unknown PON mode",__FUNCTION__,__LINE__);
    return RT_ERR_CHIP_NOT_SUPPORTED;
#endif

    return RT_ERR_OK;

}

/* Function Name:
 *      dal_rt_rtl9607f_gpon_msk_set
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
dal_rt_rtl9607f_gpon_msk_set(rt_gpon_msk_t *pMsk)
{
	aal_xgpon_aes_plmp_gbl_ctrl_t aes_plmp_gbl_ctrl;
	aal_xgpon_aes_plmp_gbl_ctrl_msk_t mask;

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pMsk), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((CA_PON_MODE_IS_2D5GPON(gPonMode)), RT_ERR_FAILED);

#if defined(CONFIG_10G_GPON_FEATURE)
	memset(&aes_plmp_gbl_ctrl, 0, sizeof(aes_plmp_gbl_ctrl));
	mask.u32 = 0;

	if (CA_E_OK != aal_xgpon_aes_plmp_glb_ctrl_get(SATURN_DEV_ID,&aes_plmp_gbl_ctrl))
    {
        RTL9607F_LOG_ERR("%s %d get aes plmp glb ctrl fail\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
	if(aes_plmp_gbl_ctrl.sma_mode == 0)
	{
		mask.s.sma_mode = 1 ;
		aes_plmp_gbl_ctrl.sma_mode = 1;
		if (CA_E_OK != aal_xgpon_aes_plmp_glb_ctrl_set(SATURN_DEV_ID, mask, &aes_plmp_gbl_ctrl))
		{
			RTL9607F_LOG_ERR("%s %d set aes plmp glb ctrl fail\n",__FUNCTION__,__LINE__);
			return RT_ERR_FAILED;
		}
	}

	if (CA_E_OK != aal_xgpon_aes_plmp_msk_set(SATURN_DEV_ID, &pMsk->mskData[0]))
	{
        RTL9607F_LOG_ERR("%s %d set aes plmp msk fail\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

	if (CA_E_OK != xgpn_aes_key_generate_by_msk(SATURN_DEV_ID, &pMsk->mskData[0]))
	{
        RTL9607F_LOG_ERR("%s %d set aes key ge by msk fail\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
#else
    RTL9607F_LOG_ERR("%s:%d::unknown PON mode",__FUNCTION__,__LINE__);
    return RT_ERR_CHIP_NOT_SUPPORTED;
#endif
    
}

/* Function Name:
 *      dal_rt_rtl9607f_gpon_omci_mic_generate
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
dal_rt_rtl9607f_gpon_omci_mic_generate(uint32 dir, uint8 *pMsg, uint32 msgLen, uint32 *mic)
{
    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK(((dir < RT_GPON_OMCI_MIC_DIR_DS) | (dir > RT_GPON_OMCI_MIC_DIR_US)), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pMsg), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == mic), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((CA_PON_MODE_IS_2D5GPON(gPonMode)), RT_ERR_FAILED);

#if defined(CONFIG_10G_GPON_FEATURE)
    if (CA_E_OK != ca_generate_omci_mic(SATURN_DEV_ID, dir, pMsg, msgLen, 4, (ca_uint8_t *)mic))

    {
        RTL9607F_LOG_ERR("%s %d generate OMCI MIC fail\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
#else
    RTL9607F_LOG_ERR("%s:%d::unknown PON mode",__FUNCTION__,__LINE__);
    return RT_ERR_CHIP_NOT_SUPPORTED;
#endif

}

/* Function Name:
 *      dal_rt_rtl9607f_gpon_mcKey_set
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
dal_rt_rtl9607f_gpon_mcKey_set(rt_gpon_mc_key_t *pMcKey)
{
    uint32 keyIdx = 0;
	int32_t ret = RT_ERR_OK;
	aal_mc_enc_key_config_msk_t mc_key_cfg_msk;
	aal_mc_enc_key_config_t mc_key_cfg;
	aal_xgpon_dsfp_xgem_mc_key_ctrl_config_msk_t key_ctrl_cfg_msk;
	aal_xgpon_dsfp_xgem_mc_key_ctrl_config_t     key_ctrl_cfg;
	uint8_t                                   decryptKey[CA_NGP2_BC_KEY_LEN] = {0};
	uint8_t                                   mckey[CA_NGP2_BC_KEY_LEN] = {0};

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK(((pMcKey->idx < RT_GPON_MC_KEY_IDX0) || (pMcKey->idx >= RT_GPON_MC_KEY_IDX_MAX)), RT_ERR_INPUT);
    RT_PARAM_CHK((pMcKey->action >= RT_GPON_MC_KEY_ACTION_MAX), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pMcKey), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((CA_PON_MODE_IS_2D5GPON(gPonMode)), RT_ERR_FAILED);

#if defined(CONFIG_10G_GPON_FEATURE)
    keyIdx = (pMcKey->idx == RT_GPON_MC_KEY_IDX0) ? CA_NGP2_KEY_IDX_0 : CA_NGP2_KEY_IDX_1;

    aes128_ecb_decrypt(kek, pMcKey->key, decryptKey);
    memset(&key_ctrl_cfg, 0, sizeof(aal_xgpon_dsfp_xgem_mc_key_ctrl_config_t));

    mc_key_cfg_msk.u32 = ~0U;
    memset(&mc_key_cfg,0, sizeof(aal_mc_enc_key_config_t));
    memcpy(mc_key_cfg.mc_key, decryptKey, CA_NGP2_BC_KEY_LEN);
    mc_key_cfg.p_effective_key_length = 16;

    ret = aal_xgpon_dsfp_xgem_mc_key_ctrl_get(SATURN_DEV_ID,&key_ctrl_cfg);
    CA_RET_VALUE_CHECK(ret);
    if(!CA_PON_MODE_IS_2D5GPON(gPonMode)){
        switch(pMcKey->action){
            case CA_MC_FLAG_MC_ENCRYPTION_KEY_SET:
                if(keyIdx == CA_NGP2_KEY_INDEX_0){
                    if(!key_ctrl_cfg.key0_idx_vld){
                        mc_key_cfg.p_key_active = 1;
                        if (CA_E_OK != aal_xgpon_aes_mc_enc_key_set(SATURN_DEV_ID, CA_AAL_XGPON_KEY_IDX_0, mc_key_cfg_msk,&mc_key_cfg))
                        {
							RTL9607F_LOG_ERR("%s %d set Multicast Key fail\n",__FUNCTION__,__LINE__);
							ret = RT_ERR_FAILED;
						}
                    }
                }
                if(keyIdx == CA_NGP2_KEY_INDEX_1){
                    if(!key_ctrl_cfg.key1_idx_vld){
                        mc_key_cfg.p_key_active = 1;
                        if (CA_E_OK != aal_xgpon_aes_mc_enc_key_set(SATURN_DEV_ID, CA_AAL_XGPON_KEY_IDX_1, mc_key_cfg_msk,&mc_key_cfg))
                        {
							RTL9607F_LOG_ERR("%s %d set Multicast Key fail\n",__FUNCTION__,__LINE__);
							ret = RT_ERR_FAILED;
						}
                    }
                }
            break;
            case CA_MC_FLAG_MC_ENCRYPTION_KEY_CLEAR:
                if(keyIdx == CA_NGP2_KEY_INDEX_0){
                    if(key_ctrl_cfg.key0_idx_vld){
                        mc_key_cfg.p_key_active = 0;
                        if (CA_E_OK != aal_xgpon_aes_mc_enc_key_set(SATURN_DEV_ID, CA_AAL_XGPON_KEY_IDX_0, mc_key_cfg_msk,&mc_key_cfg))
                        {
							RTL9607F_LOG_ERR("%s %d set Multicast Key fail\n",__FUNCTION__,__LINE__);
							ret = RT_ERR_FAILED;
						}
                    }
                }
                if(keyIdx == CA_NGP2_KEY_INDEX_1){
                    if(key_ctrl_cfg.key1_idx_vld){
                        mc_key_cfg.p_key_active = 0;
                        if (CA_E_OK != aal_xgpon_aes_mc_enc_key_set(SATURN_DEV_ID, CA_AAL_XGPON_KEY_IDX_1, mc_key_cfg_msk,&mc_key_cfg))
                        {
							RTL9607F_LOG_ERR("%s %d set Multicast Key fail\n",__FUNCTION__,__LINE__);
							ret = RT_ERR_FAILED;
						}
                    }
                }
            break;
            case CA_MC_FLAG_MC_ENCRYPTION_KEY_TBL_FLUSH:
                key_ctrl_cfg_msk.u32 = ~0U;
                key_ctrl_cfg.key0_idx_vld = 0;
                key_ctrl_cfg.key1_idx_vld = 0;
                if (CA_E_OK != aal_xgpon_dsfp_xgem_mc_key_ctrl_set(SATURN_DEV_ID, key_ctrl_cfg_msk, &key_ctrl_cfg))
                {
                    RTL9607F_LOG_ERR("%s %d set Multicast Key fail\n",__FUNCTION__,__LINE__);
					ret = RT_ERR_FAILED;
				}
                
            break;
            default:
            break;
        }

        #if 0

        if(!key_ctrl_cfg.key0_idx_vld){
            ret = aal_xgpon_aes_mc_enc_key_set(SATURN_DEV_ID, 0,mc_key_cfg_msk,&mc_key_cfg);
            CA_RET_VALUE_CHECK(ret);
        }
        if(!key_ctrl_cfg.key1_idx_vld){
            ret = aal_xgpon_aes_mc_enc_key_set(SATURN_DEV_ID, 1,mc_key_cfg_msk,&mc_key_cfg);
            CA_RET_VALUE_CHECK(ret);
        }
        #endif
    }else{
    //gpon
    }

    return ret;
#else
    RTL9607F_LOG_ERR("%s:%d::unknown PON mode",__FUNCTION__,__LINE__);
    return RT_ERR_CHIP_NOT_SUPPORTED;
#endif
}


/* Function Name:
 *      dal_rt_rtl9607f_gpon_attribute_set
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
dal_rt_rtl9607f_gpon_attribute_set(rt_gpon_attribute_type_t attributeType, rt_gpon_attribute_t *pAttributeValue)
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
            if(CA_PON_MODE_IS_2D5GPON(gPonMode))
            {/* GPON always enable dbru*/
                /*do nothing*/
            }
            else
            {
#if defined(CONFIG_10G_GPON_FEATURE)
                memset(&xgtc_ctrl, 0x0, sizeof(aal_xgpon_usbg_xgtc_ctrl_config_t));
                memset(&mask, 0, sizeof(aal_xgpon_usbg_xgtc_ctrl_config_msk_t));
                mask.s.dbru_en = 1;
                xgtc_ctrl.dbru_en = pAttributeValue->dbruStatus;
                if(CA_E_OK != (ret = aal_xgpon_usbg_xgtc_ctrl_set(SATURN_DEV_ID, mask, &xgtc_ctrl)))
                {
                    RTL9607F_LOG_ERR("%s %d : aal_xgpon_usbg_xgtc_ctrl_set Error!! ret=%x",__FUNCTION__,__LINE__,ret);
                    ret = RT_ERR_FAILED;
                }               
#else
                RTL9607F_LOG_ERR("%s:%d::unknown PON mode",__FUNCTION__,__LINE__);
                return RT_ERR_CHIP_NOT_SUPPORTED;
#endif
           }
            break;
        case RT_GPON_ATTRIBUTE_TYPE_GEM_BLOCK_LENGTH:
            if(CA_PON_MODE_IS_2D5GPON(gPonMode))
            {/*support 48, 64, 128, 256, 512, 1024, 1536, 2048 bytes*/
                aal_gpon_mac_cfg_t gpon_cfg;
                aal_gpon_mac_cfg_msk_t gpon_mask;
                memset(&gpon_cfg, 0x0, sizeof(aal_gpon_mac_cfg_t));
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
                    RTL9607F_LOG_ERR("%s %d : aal_xgpon_usbg_xgtc_ctrl_set Error!! ret=%x",__FUNCTION__,__LINE__,ret);
                    ret = RT_ERR_FAILED;
                }
            }
            else
            {/*XG-PON/XGS-PON/NG-PON is fix to 1 bytes*/
                /*do nothing*/
            }
            break;
        case RT_GPON_ATTRIBUTE_TYPE_SD_THRESHOLD:
            if(CA_PON_MODE_IS_2D5GPON(gPonMode))
            {
                aal_gpon_sf_degrd_alrm_thrsd_cfg_t th_cfg;
                aal_gpon_sf_degrd_alrm_thrsd_cfg_msk_t th_msk;
                memset(&th_cfg, 0x0, sizeof(aal_gpon_sf_degrd_alrm_thrsd_cfg_t));
                memset(&th_msk, 0x0, sizeof(aal_gpon_sf_degrd_alrm_thrsd_cfg_msk_t));
                if((pAttributeValue->sdThreshold < 4) || (pAttributeValue->sdThreshold > 10)) /*by 984.4 spec*/
                {
                    return RT_ERR_INPUT;
                }
                th_cfg.degrade_threshold = pAttributeValue->sdThreshold;
                th_cfg.degrade_remove_threshold = pAttributeValue->sdThreshold + 1;
                th_msk.s.degrade_threshold = 1;
                th_msk.s.degrade_remove_threshold = 1;
                if (CA_E_OK != (ret = aal_gpon_sf_degrd_alrm_thrsd_set(SATURN_DEV_ID, th_msk, &th_cfg)))
                {
                    RTL9607F_LOG_ERR("%s %d : aal_gpon_sf_degrd_alrm_thrsd_set Error!! ret=%x",__FUNCTION__,__LINE__,ret);;
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
            if(CA_PON_MODE_IS_2D5GPON(gPonMode))
            {
                aal_gpon_sf_degrd_alrm_thrsd_cfg_t th_cfg;
                aal_gpon_sf_degrd_alrm_thrsd_cfg_msk_t th_msk;
                memset(&th_cfg, 0x0, sizeof(aal_gpon_sf_degrd_alrm_thrsd_cfg_t));
                memset(&th_msk, 0x0, sizeof(aal_gpon_sf_degrd_alrm_thrsd_cfg_msk_t));
                if((pAttributeValue->sdThreshold < 3) || (pAttributeValue->sdThreshold > 9)) /*by 984.4 spec*/
                {
                    return RT_ERR_INPUT;
                }
                th_cfg.fail_threshold = pAttributeValue->sdThreshold;
                th_cfg.fail_remove_threshold = pAttributeValue->sdThreshold + 1;
                th_msk.s.fail_threshold = 1;
                th_msk.s.fail_remove_threshold = 1;
                if (CA_E_OK != (ret = aal_gpon_sf_degrd_alrm_thrsd_set(SATURN_DEV_ID,th_msk,&th_cfg)))
                {
                    RTL9607F_LOG_ERR("%s %d : aal_gpon_sf_degrd_alrm_thrsd_set Error!! ret=%x",__FUNCTION__,__LINE__,ret);
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
 *      dal_rt_rtl9607f_gpon_attribute_get
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
dal_rt_rtl9607f_gpon_attribute_get(rt_gpon_attribute_type_t attributeType, rt_gpon_attribute_t *pAttributeValue)
{
    int32 ret = RT_ERR_OK;
    aal_xgpon_usbg_xgtc_ctrl_config_t xgtc_ctrl;
    uint32 eqd = 0;

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((attributeType >=  RT_GPON_ATTRIBUTE_TYPE_MAX), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pAttributeValue), RT_ERR_NULL_POINTER);

    switch(attributeType)
    {
        case RT_GPON_ATTRIBUTE_TYPE_DBRU_STATUS:
            if(CA_PON_MODE_IS_2D5GPON(gPonMode))
            {/* GPON always enable dbru*/
                pAttributeValue->dbruStatus = 1;
            }
            else
            {
#if defined(CONFIG_10G_GPON_FEATURE)
                if(CA_E_OK != (ret = aal_xgpon_usbg_xgtc_ctrl_get(SATURN_DEV_ID, &xgtc_ctrl)))
                {
                    RTL9607F_LOG_ERR("%s %d : aal_xgpon_usbg_xgtc_ctrl_get Error!! ret=%x",__FUNCTION__,__LINE__,ret);
                    return RT_ERR_FAILED;
                }
                pAttributeValue->dbruStatus = xgtc_ctrl.dbru_en;
#else
                RTL9607F_LOG_ERR("%s:%d::unknown PON mode",__FUNCTION__,__LINE__);
                return RT_ERR_CHIP_NOT_SUPPORTED;
#endif
            }
            break;
        case RT_GPON_ATTRIBUTE_TYPE_GEM_BLOCK_LENGTH:
            if(CA_PON_MODE_IS_2D5GPON(gPonMode))
            {/*support 48, 64, 128, 256, 512, 1024, 1536, 2048 bytes*/
                aal_gpon_mac_cfg_t gpon_cfg;
                if(CA_E_OK != (ret = aal_gpon_mac_cfg_get(SATURN_DEV_ID, &gpon_cfg)))
                {
                    RTL9607F_LOG_ERR("%s %d : aal_gpon_mac_cfg_get Error!! ret=%x",__FUNCTION__,__LINE__,ret);
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
            if(CA_PON_MODE_IS_2D5GPON(gPonMode))
            {
                aal_gpon_sf_degrd_alrm_thrsd_cfg_t th_cfg;
                if (CA_E_OK != (ret = aal_gpon_sf_degrd_alrm_thrsd_get(SATURN_DEV_ID, &th_cfg)))
                {
                    RTL9607F_LOG_ERR("%s %d : aal_gpon_sf_degrd_alrm_thrsd_get Error!!, ret=%x",__FUNCTION__,__LINE__,ret);
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
            if(CA_PON_MODE_IS_2D5GPON(gPonMode))
            {
                aal_gpon_sf_degrd_alrm_thrsd_cfg_t th_cfg;
                if (CA_E_OK != (ret = aal_gpon_sf_degrd_alrm_thrsd_get(SATURN_DEV_ID, &th_cfg)))
                {
                    RTL9607F_LOG_ERR("%s %d : aal_gpon_sf_degrd_alrm_thrsd_get Error!! ret=%x",__FUNCTION__,__LINE__,ret);
                    ret = RT_ERR_FAILED;
                }
                pAttributeValue->sfThreshold = th_cfg.fail_threshold;
            }
            else
            {/*XGPON do not support*/
                ret = RT_ERR_CHIP_NOT_SUPPORTED;
            }
            break;
        case RT_GPON_ATTRIBUTE_TYPE_GTC_INFO:
            if(CA_PON_MODE_IS_2D5GPON(gPonMode))
            {
                if (CA_E_OK != (ret = aal_gpon_eqd_get(SATURN_DEV_ID, 0, &eqd)))
                {
                    RTL9607F_LOG_ERR("%s %d : aal_gpon_eqd_get Error!! ret=%x",__FUNCTION__,__LINE__,ret);
                    ret = RT_ERR_FAILED;
                }
                pAttributeValue->gtc_info.eqd = eqd;

                /* response_time is base_time add offset*/
                if(gPonMode == ONU_PON_MAC_GPON)
                {
                    pAttributeValue->gtc_info.responTime = 35800;
                }
                if(gPonMode == ONU_PON_MAC_GPON_BI2G5)
                {
                    pAttributeValue->gtc_info.responTime = 35600;
                }
            }
            else
            {
#if defined(CONFIG_10G_GPON_FEATURE)
                if(CA_E_OK != (ret = aal_xgpon_bwmp_eqd_sts_get(SATURN_DEV_ID, &eqd)))
                {
                    RTL9607F_LOG_ERR("%s %d : aal_xgpon_bwmp_eqd_sts_get Error!! ret=%x",__FUNCTION__,__LINE__,ret);
                    ret = RT_ERR_FAILED;
                }
                pAttributeValue->gtc_info.eqd = eqd;
#else
                RTL9607F_LOG_ERR("%s:%d::unknown PON mode",__FUNCTION__,__LINE__);
                return RT_ERR_CHIP_NOT_SUPPORTED;
#endif
            }
            break;
        default:
            ret = RT_ERR_INPUT;
            break;
    }

    return ret;
}

 /* Function Name:
 *      dal_rt_rtl9607f_gpon_usFlow_phyData_get
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
int32 dal_rt_rtl9607f_gpon_usFlow_phyData_get
(uint32 usFlowId, uint32 *pTcontId, uint32 *pTcQueueId, uint32 *pGemIdx)
{
    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pTcontId), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pTcQueueId), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pGemIdx), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((usFlowId >= RTL9607F_MAX_GEM_FLOW), RT_ERR_INPUT);
    
    /* function body */
    if(TRUE != g9607fGemUsFlowMap[usFlowId].isValid) 
    {        
        //RTL9607F_LOG_ERR("%s %d invalid us flow [%d]\n", __FUNCTION__, __LINE__, usFlowId);
        return RT_ERR_INPUT;
    }
    
    *pTcontId        = RTL9607F_TCONID_TO_LDPID(g9607fGemUsFlowMap[usFlowId].tcontId);
    *pTcQueueId      = g9607fGemUsFlowMap[usFlowId].tcQueueId;
    *pGemIdx         = g9607fGemUsFlowMap[usFlowId].gemIdx;   


    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rt_rtl9607f_gpon_omcc_get
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
dal_rt_rtl9607f_gpon_omcc_get(rt_gpon_omcc_t *pOmcc)
{
    int32 ret = RT_ERR_OK;

    /* check Init status */
    RT_INIT_CHK(gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pOmcc), RT_ERR_NULL_POINTER);

    ret = _rtl9607f_gpon_omcc_get(pOmcc);
    if (RT_ERR_OK != ret)
    {
        RTL9607F_LOG_ERR("%s:%d::Error OMCC get fail",__FUNCTION__,__LINE__);
    }

    return ret;
}


/* Function Name:
 *      dal_rt_rtl9607f_gpon_omci_mirror_set
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

int32 
dal_rt_rtl9607f_gpon_omci_mirror_set(rt_enable_t enable, rt_port_t mirroringPort)
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
 *      dal_rt_rtl9607f_gpon_omci_mirror_get
 * Description:
 *      Get the GPON OMCI mirror function.
 * Input:
 *      None
 * Output:
 *      pEnable    - get Enable/disble mirroring OMCI status
 *      pMirroringPort - get mirroring port 
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT   - Invalid input 
 * Note:
 */
int32
dal_rt_rtl9607f_gpon_omci_mirror_get(rt_enable_t *pEnable, rt_port_t *pMirroringPort)
{
    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pMirroringPort), RT_ERR_NULL_POINTER);

    /* function body */
    *pEnable = gOmciMirrorEnable;
    *pMirroringPort = gOmciMirrorEnable?gMirroringPort:0;

    return RT_ERR_OK;
}   /* end of dal_rt_rtl9607f_gpon_omci_mirror_get */


/* Function Name:
 *      dal_rt_rtl9607f_gpon_fec_get
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
dal_rt_rtl9607f_gpon_fec_get(rt_gpon_fec_status_t *pFecStatus)
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

    if(CA_PON_MODE_IS_2D5GPON(gPonMode))
    {
        //DS FEC
        if (CA_E_OK != (ret = aal_gpon_fec_get(SATURN_DEV_ID, &gponUsFec, &gponDsFec)))
        {
            RTL9607F_LOG_ERR("%s %d : aal_gpon_fec_get Error!!, ret=%x",__FUNCTION__,__LINE__,ret);
            return RT_ERR_FAILED;
        }
        pFecStatus->us_fec_status = gponUsFec;
        pFecStatus->ds_fec_status = gponDsFec;
    }
    else
    {
#if defined(CONFIG_10G_GPON_FEATURE)
        //DS FEC
        if(CA_PON_MODE == ONU_PON_MAC_NGPON2)
        {
            if (CA_E_OK != (ret = aal_xgpon_dsfp_oc_body_get(SATURN_DEV_ID, &oc_body)))
            {
                RTL9607F_LOG_ERR("%s %d : aal_xgpon_dsfp_oc_body_get Error!!, ret=%x",__FUNCTION__,__LINE__,ret);
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
                    RTL9607F_LOG_ERR("%s %d : aal_xgpon_bwmp_grt_ff_ctrl_cfg_get Error!!, ret=%x",__FUNCTION__,__LINE__,ret);
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
#else
        RTL9607F_LOG_ERR("%s:%d::unknown PON mode",__FUNCTION__,__LINE__);
        return RT_ERR_CHIP_NOT_SUPPORTED;
#endif
    }

    return RT_ERR_OK;   
}

#define _XGPON_PLOAM_MSG_LEN 48
#define _GPON_PLOAM_MSG_LEN 13

static ca_uint32_t ca_rtk_gpon_ploam_rx_cb(ca_device_id_t device_id,ca_uint32_t event_id,ca_void_t *event,ca_uint32_t len,ca_void_t *cookie)
{
    ca_event_gpon_ploam_t* ploam_ptr = (ca_event_gpon_ploam_t *)event;
    uint32 i;
    
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_INTR),"%s",__FUNCTION__);

    /*printk("ploam message: onu_id=%d, type=%d\n", ploam_ptr->onuid, ploam_ptr->type);
    for(i=0; i<13; i++){
        printk("%02x", ploam_ptr->msg[i]);
    }
    printk("\n");*/

    if(g_gpon_ploam_rx != NULL)
    {   
        if(CA_PON_MODE_IS_2D5GPON(gPonMode))
            g_gpon_ploam_rx(ploam_ptr->msg, _GPON_PLOAM_MSG_LEN);
        else
            g_gpon_ploam_rx(ploam_ptr->msg, _XGPON_PLOAM_MSG_LEN);
    }

    return CA_EVT_OK;

}

/* Function Name:
 *      dal_rt_rtl9607f_gpon_ploam_rx_register
 * Description:
 *      register ploam rx callback
 * Input:
 *      func            - callback func
 * Return:
 *      RT_ERR_OK       - OK
 *      RT_ERR_FAILED   - Failed
 *      RT_ERR_INPUT    - null callback func
 * Note:
 */
int32 
dal_rt_rtl9607f_gpon_ploam_rx_register(rt_gpon_ploam_rx_callback func)
{
    int32 ret;

    if(!func)
        return RT_ERR_INPUT;
    
    if(NULL == g_gpon_ploam_rx)
    {
        if((ret = ca_event_register(0, CA_EVENT_GPON_PLOAM_RX, ca_rtk_gpon_ploam_rx_cb, NULL))!=CA_E_OK)
        {
            return RT_ERR_FAILED;
        }
    }
    g_gpon_ploam_rx = func;

    if(CA_PON_MODE_IS_2D5GPON(gPonMode))
    {
        aal_gpon_mac_cfg_t cfg;
        aal_gpon_mac_cfg_msk_t mask;

        mask.u32 = 0;
        memset(&cfg, 0x0, sizeof(cfg));

        mask.s.plmd_ff_wr_ext = 1;
        cfg.plmd_ff_wr_ext = 1;
        
        if (CA_E_OK != (ret = aal_gpon_mac_cfg_set(SATURN_DEV_ID, mask, &cfg)))
        {
            RTL9607F_LOG_ERR("%s %d : aal_gpon_mac_cfg_set for plmd_ff_wr_ext Error!!, ret=%x",__FUNCTION__,__LINE__,ret);
            return RT_ERR_FAILED;
        }
    }
    else 
    {/*XG/XGS GPON turn on by default*/
    }

    return RT_ERR_OK;
}


