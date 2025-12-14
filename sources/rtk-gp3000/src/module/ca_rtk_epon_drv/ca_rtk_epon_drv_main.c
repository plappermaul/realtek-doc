/*
 * Copyright (C) 2018 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 *
 *
 * $Revision: $
 * $Date: $
 *
 * Purpose : EPON kernel drivers
 *
 * Feature : This module install EPON kernel callbacks and other 
 * kernel functions
 *
 */

#ifdef CONFIG_LUNA_G3_SERIES
/*
 * Include Files
 */
#include <linux/module.h> 
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/skbuff.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/version.h>
#include <linux/timekeeping.h>

#include <common/error.h>
#include <common/rt_type.h>
#include <common/rt_error.h>

#include "pkt_redirect.h"

#include "hal/common/halctrl.h"


#include "ca-ne-rtk/ca_ext.h"
#include "cortina-api/include/pkt_tx.h"
#include "cortina-api/include/port.h"
#include "cortina-api/include/special_packet.h"
#include "cortina-api/include/epon.h"
#include "cortina-api/include/classifier.h"
#include "cortina-api/include/vlan.h"
#include "aal_epon.h"
#include "aal_mpcp.h"
#include "aal_puc.h"
#include "aal_psds.h"
#include "aal_l2_vlan.h"
#include "aal_port.h"
#include "aal_l2_qos.h"
#include "scfg/include/scfg.h"
#include "ca_rtk_epon_drv_main.h"

#include <ca_event.h>

#include "rtk/epon.h"
#include "rtk/l2.h"

#ifdef CONFIG_CA8279_SERIES
#include "ni-drv/ca_ni.h"
#endif

#if defined(CONFIG_MULTICHIP_ONEIMAGE)

#ifdef ONE_IMAGE_VENUS
#include "ni-drv-gen2/ca_ni.h"
#endif

#ifdef ONE_IMAGE_77C
#include "ni-drv-77c/ca_ni.h"
#endif

#ifdef ONE_IMAGE_07F
#include "ni-drv-07f/ca_ni.h"
#endif

#else

#ifdef CONFIG_CA8277B_SERIES
#include "ni-drv-gen2/ca_ni.h"
#endif

#ifdef CONFIG_RTL8277C_SERIES
#include "ni-drv-77c/ca_ni.h"
#endif

#ifdef CONFIG_RTL9607F_SERIES
#include "ni-drv-07f/ca_ni.h"
#endif

#endif

#ifdef CONFIG_COMMON_RT_API
#include "rtk/rt/rt_ponmisc.h"
#include "rtk/rt/rt_epon.h"
#endif

#include "module/intr_bcaster/intr_bcaster.h"

extern netdev_tx_t nic_egress_start_xmit(struct sk_buff *skb, struct net_device *dev, ca_ni_tx_config_t *tx_config);

#if defined(ONE_IMAGE_77C) || (!defined(CONFIG_CA8279_SERIES) && !defined(CONFIG_CA8277B_SERIES) && !defined(CONFIG_MULTICHIP_ONEIMAGE))
int nic_register_rxhook(int portmask,int priority,p2rfunc_t rx);
int nic_unregister_rxhook(int portmask,int priority,p2rfunc_t rx);
#else
extern int drv_nic_register_rxhook(int portmask,int priority,p2rfunc_t rx);
extern int drv_nic_unregister_rxhook(int portmask,int priority,p2rfunc_t rx);
#endif

static ca_uint32_t gbl_cls_index=-1;

#define MAX_QUEUE 128
#define MAX_LLID 1
#define MAX_COS 8
static ca_uint32_t gbl_us_pon_cls_index[MAX_QUEUE];

#define MAX_SID 8

#define ETYPE_OFFSET    12
#define ETH_HEAD_LEN    16



/*mpcp gate analysys*/ 
#define CA_EPON_MPCP_ANA_FLAG_START       0x00000001UL
#define CA_EPON_MPCP_ANA_FLAG_STOP        0x00000002UL  
#define CA_EPON_MPCP_ANA_FLAG_ENABLE      0x00008000UL

#define CA_EPON_MPCP_LOG_DUMP             0x80000000UL
#define CA_EPON_MPCP_LOG_DUMP_CLR         0x40000000UL  
#define CA_EPON_MPCP_LOG_DUMP_OVERWRITE   0x20000000UL




#define CA_EPON_MPCP_OFFSET_ADJ      0 /*CA insert on vlan tag*/
static unsigned int ca_gate_ana_flag = 0;

static unsigned int ca_rtk_epon_oam_pkt_rx_cnt=0;


typedef struct epon_mpcp_gate_info_s
{
    uint32  timeStamp;
    uint32  grantStart;
    uint32  grantLen;
    uint8   grantFlag;
} epon_mpcp_gate_info_t;




#define EPON_MPCP_MAX_GRANT_ANA_DB  128
static epon_mpcp_gate_info_t grantDb[EPON_MPCP_MAX_GRANT_ANA_DB];
static uint16 currentGrantIdx=0;
struct proc_dir_entry* ca_gate_ana_entry = NULL;

//reference to ca_ni_tx.c
static ca_mac_addr_t puc_da = {0x00, 0x13, 0x25, 0x00, 0x00, 0x00};
static ca_mac_addr_t puc_sa = {0x00, 0x13, 0x25, 0x00, 0x00, 0x01};

static ca_int8_t etype_oam[]  = {0xff, 0xf1};
static ca_int8_t etype_vlan[] = {0x81, 0x00};

static ca_mac_addr_t oam_da = {0x01, 0x80, 0xc2, 0x00, 0x00, 0x02};

static uint8 one_shot_setting = 0;
static uint8 oam_static_l2_addr_setting = 0;

#ifdef CONFIG_CA8279_SERIES
static int saturn_us_pon_rule_adding(void);
static int saturn_us_pon_rule_deleting(void);
#endif

#if defined(CONFIG_CA8277B_SERIES) || defined(CONFIG_RTL8277C_SERIES) || defined(CONFIG_RTL9607F_SERIES)
static int us_pon_sid_mapping_adding(void);
static int us_pon_sid_mapping_deleting(void);
#endif

int oam_ca_drv_log = 0;

#define OAM_CA_DRV_LOG(level,fmt, arg...) \
    do { if ( oam_ca_drv_log > level) { printk(fmt, ##arg); printk("\n"); } } while (0);

struct proc_dir_entry* ca_rtk_epon_proc_dir = NULL;
struct proc_dir_entry* oam_ca_drv_log_entry = NULL;
struct proc_dir_entry* mpcp_stats_entry = NULL;
struct proc_dir_entry* port_stats_entry = NULL;
struct proc_dir_entry* us_pon_rule_entry = NULL;
struct proc_dir_entry* oam_static_l2_addr_entry = NULL;
struct proc_dir_entry* mpcp_reg_start_entry = NULL;
struct proc_dir_entry* pon_mac_reset_entry = NULL;
struct proc_dir_entry* laser_on_time_entry = NULL;
struct proc_dir_entry* laser_off_time_entry = NULL;

#define MAX_OAM_EVENT_SIZE (sizeof(rtk_epon_oam_event_t) + 2) /* magic key size 2 */


static int ca_rtk_epon_mpcp_pkt_rx(struct sk_buff *skb)
{
    int pucHdrAdj=0;
#if 0
    int i;
    
    for(i=0;i<60;i++)
    {
        if(i%16 ==0)
            printk("\n");

        printk("%2.2x ",skb->data[i]);
    }
#endif
    if(skb->data[12+CA_EPON_MPCP_OFFSET_ADJ]==0xff&&
       skb->data[13+CA_EPON_MPCP_OFFSET_ADJ]==0xf0&&
       skb->data[14+CA_EPON_MPCP_OFFSET_ADJ]==0x00&&
       skb->data[15+CA_EPON_MPCP_OFFSET_ADJ]==0x00)
    {
        pucHdrAdj=16;        
    }
    else
    {
        pucHdrAdj=0;        
    }

    if((skb->data[12+CA_EPON_MPCP_OFFSET_ADJ+pucHdrAdj] == 0x88) && (skb->data[13+CA_EPON_MPCP_OFFSET_ADJ+pucHdrAdj] == 0x08))
    {
        /*ony mac control packet(MPCP) need handle by this API*/ 
        
    }
    else
    {
         return RE8670_RX_CONTINUE;    
    }
    
    /*filter mpcp  register packet*/
    if((skb->data[14+CA_EPON_MPCP_OFFSET_ADJ+pucHdrAdj] == 0x00) &&  (skb->data[15+CA_EPON_MPCP_OFFSET_ADJ+pucHdrAdj] == 0x05))
    {
        printk("\n receive register packet mac [%2.2x %2.2x %2.2x %2.2x %2.2x %2.2x] flag:%d\n",
                        skb->data[0+pucHdrAdj],
                        skb->data[1+pucHdrAdj],
                        skb->data[2+pucHdrAdj],
                        skb->data[3+pucHdrAdj],
                        skb->data[4+pucHdrAdj],
                        skb->data[5+pucHdrAdj],
                        skb->data[22+CA_EPON_MPCP_OFFSET_ADJ+pucHdrAdj]);

        if(skb->len > (27  + CA_EPON_MPCP_OFFSET_ADJ +pucHdrAdj))
        {
            printk("laser on time:%d, laser off time:%d\n",skb->data[26+CA_EPON_MPCP_OFFSET_ADJ+pucHdrAdj],skb->data[27+CA_EPON_MPCP_OFFSET_ADJ+pucHdrAdj]);
        }

        return RE8670_RX_STOP;    
    }
    
    /*filter mpcp gate message*/
    if((skb->data[14+CA_EPON_MPCP_OFFSET_ADJ+pucHdrAdj] == 0x00) &&  (skb->data[15+CA_EPON_MPCP_OFFSET_ADJ+pucHdrAdj] == 0x02))
    {            
        uint32 mpcpGateEnd=0;
        uint16 gateLen=0;
        uint32 mpcpGateStar=0,mpcpTimeStamp=0,grantStart=0;
        int i;

        for(i=0;i<4;i++)
        {
            mpcpTimeStamp = mpcpTimeStamp <<8;
            mpcpTimeStamp += (skb->data[16+CA_EPON_MPCP_OFFSET_ADJ + pucHdrAdj + i]);
        }

        for(i=0;i<4;i++)
        {
            mpcpGateStar = mpcpGateStar <<8;
            mpcpGateStar += (skb->data[21+CA_EPON_MPCP_OFFSET_ADJ + pucHdrAdj + i]);
        }

        for(i=0;i<2;i++)
        {
            gateLen = gateLen <<8;
            gateLen += (skb->data[25+CA_EPON_MPCP_OFFSET_ADJ + pucHdrAdj + i]);
        }

        if(!(ca_gate_ana_flag&CA_EPON_MPCP_ANA_FLAG_STOP))
        {
            {
                if(currentGrantIdx >= EPON_MPCP_MAX_GRANT_ANA_DB)
                    currentGrantIdx=0;
                grantDb[currentGrantIdx].timeStamp = mpcpTimeStamp;
                grantDb[currentGrantIdx].grantStart = mpcpGateStar;
                grantDb[currentGrantIdx].grantLen = gateLen;
                grantDb[currentGrantIdx].grantFlag = skb->data[20+CA_EPON_MPCP_OFFSET_ADJ + pucHdrAdj];
                currentGrantIdx++;
            }
        }

        return RE8670_RX_STOP;    
    }
    return RE8670_RX_CONTINUE;  
}



static int ca_mpcp_gate_ana_read(struct seq_file *seq, void *v)
{
    int i,j;
    uint32 totalTq=0;
    uint32 totalTime;
    uint32 firstGrantStart=0,lastGrantEnd;
	
	ca_gate_ana_flag = ca_gate_ana_flag|CA_EPON_MPCP_ANA_FLAG_STOP;
	
	seq_printf(seq, "\n\n index,TimeStmp,GntStart,GrantEnd,GrantLen,  Flag\n");

    for(j=0;j<EPON_MPCP_MAX_GRANT_ANA_DB;j++)
    {
    	i = currentGrantIdx+j;
    	
    	i= i%EPON_MPCP_MAX_GRANT_ANA_DB;
    	
    	seq_printf(seq, " %-5d,%-8.8x,%-8.8x,%-8.8x,%-8d,0x%4x\n"
    	      ,i
    	      ,grantDb[i].timeStamp
    	      ,grantDb[i].grantStart
    	      ,(grantDb[i].grantStart+grantDb[i].grantLen)
    	      ,grantDb[i].grantLen
    	      ,grantDb[i].grantFlag);
    	lastGrantEnd = (grantDb[i].grantStart+grantDb[i].grantLen);
    	      
    	if(grantDb[i].grantFlag != 0x09)/*skip mpcp discovery gate*/
    	    totalTq=grantDb[i].grantLen+totalTq;      
        
        if(j==0)
            firstGrantStart = grantDb[i].grantStart;
    }


  	seq_printf(seq, "\n\n total bandWidth:%d TQ   start:%u end:%u\n", totalTq,firstGrantStart,lastGrantEnd);
    if(lastGrantEnd > firstGrantStart)
      	seq_printf(seq, "\nRecodr period: %d TQ\n", lastGrantEnd-firstGrantStart);
    else
      	seq_printf(seq, "\nRecodr period: %u TQ\n", ((uint32)0xFFFFFFFF-firstGrantStart) + lastGrantEnd);
            
    
	ca_gate_ana_flag = ca_gate_ana_flag&(uint32)(~CA_EPON_MPCP_ANA_FLAG_STOP);


	return 0;
}

static void oam_static_l2_addr_add(void)
{
    int32 ret;
    rtk_l2_ucastAddr_t l2Addr;

    //Adding L2 mac(00:13:25:00:00:00) at port 7 for guaranting OAM packet can be traped at cos 7 when sw learning is enabling 
    memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));  
    l2Addr.vid = 0;
    memcpy(&l2Addr.mac.octet, puc_da, ETHER_ADDR_LEN);
    ret = rtk_l2_addr_get(&l2Addr);
    if(ret == RT_ERR_L2_ENTRY_NOTFOUND)
    {
        memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
        l2Addr.flags |= RTK_L2_UCAST_FLAG_IVL|RTK_L2_UCAST_FLAG_STATIC;
        l2Addr.vid = 0;
        memcpy(&l2Addr.mac.octet, puc_da, ETHER_ADDR_LEN);
        l2Addr.port = HAL_GET_PON_PORT();

        oam_static_l2_addr_setting = 1;
        OAM_CA_DRV_LOG(0,"Adding deault PUC address (%02x:%02x:%02x:%02x:%02x:%02x)!!\n",puc_da[0],puc_da[1],puc_da[2],puc_da[3],puc_da[4],puc_da[5]);
        if((ret = rtk_l2_addr_add(&l2Addr))!=CA_E_OK)
        {
            oam_static_l2_addr_setting = 0;
            OAM_CA_DRV_LOG(0,"Adding deault PUC address (%02x:%02x:%02x:%02x:%02x:%02x) Error ret=%d!!\n",puc_da[0],puc_da[1],puc_da[2],puc_da[3],puc_da[4],puc_da[5],ret);
        }
    }
}

static void oam_static_l2_addr_del(void)
{
    int32 ret;
    rtk_l2_ucastAddr_t l2Addr;

    //Adding L2 mac(00:13:25:00:00:00) at port 7 for guaranting OAM packet can be traped at cos 7 when sw learning is enabling 
    memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));  
    l2Addr.vid = 0;
    memcpy(&l2Addr.mac.octet, puc_da, ETHER_ADDR_LEN);
    ret = rtk_l2_addr_get(&l2Addr);
    if(ret == RT_ERR_OK)
    {
        oam_static_l2_addr_setting = 0;
        OAM_CA_DRV_LOG(0,"Deleting deault PUC address (%02x:%02x:%02x:%02x:%02x:%02x)!!\n",puc_da[0],puc_da[1],puc_da[2],puc_da[3],puc_da[4],puc_da[5]);
        if((ret = rtk_l2_addr_del(&l2Addr))!=CA_E_OK)
        {
            oam_static_l2_addr_setting = 1;
            OAM_CA_DRV_LOG(0,"Deleting deault PUC address (%02x:%02x:%02x:%02x:%02x:%02x) Error ret=%d!!\n",puc_da[0],puc_da[1],puc_da[2],puc_da[3],puc_da[4],puc_da[5],ret);
        }
    }
}

s_oam_t S_OAM={
    .num = 0,
};

void s_oam_register(s_oam_t *reg_oam)
{
    if(reg_oam->num > MAX_S_OAM_NUM)
    {
        printk("%s %d illegal register oam number=%d\n",__FUNCTION__,__LINE__,reg_oam->num);
        return;
    }
    printk("%s %d register oam number=%d\n",__FUNCTION__,__LINE__,reg_oam->num);
    memcpy(&S_OAM,reg_oam,sizeof(s_oam_t));

    oam_ca_drv_log = 3;
}

void s_oam_unregister(void)
{
    memset(&S_OAM,0,sizeof(s_oam_t));

    oam_ca_drv_log = 0;
}

void send_static_oam_pkt_tx(unsigned char llidIdx,unsigned short dataLen, unsigned char *data)
{
#ifdef CONFIG_CORTINA_BOARD_FPGA
    struct sk_buff *p_skb;
    HEADER_A_T ni_header_a;

    p_skb = netdev_alloc_skb(NULL, dataLen+32);
    if (p_skb == NULL) 
    {
        ca_printf("netdev_alloc_skb fail\n");
        return;
    }
    puc_da[5] = (puc_da[5] & 0xc0) | llidIdx;

    memcpy(p_skb->data,&puc_da[0],6);
    memcpy(p_skb->data+6,&puc_sa[0],6);
    memcpy(p_skb->data+12,&etype_oam[0],2);
    p_skb->data[14]=0x00;
    p_skb->data[15]=0x00;
    memcpy(p_skb->data+16,data,dataLen);
    p_skb->len = dataLen+16;

    memset(&ni_header_a, 0, sizeof(HEADER_A_T));

    ni_header_a.bits.cos = 7;
    ni_header_a.bits.ldpid = 0x20 + llidIdx;
    ni_header_a.bits.lspid = 0x10;
    ni_header_a.bits.pkt_size = p_skb->len;
    ni_header_a.bits.fe_bypass = 1;

    ni_header_a.bits.bits_32_63.pkt_info.no_drop = 1;

    __rtk_ni_start_xmit(p_skb, &ni_header_a, NULL , NULL , NULL, 1);

#else
    ca_pkt_t halPkt;
    ca_pkt_block_t pkt_data;
    unsigned char tmp[60];
    int ret;
#if defined(ONE_IMAGE_VENUS) || (defined(CONFIG_CA8277B_SERIES) && !defined(CONFIG_MULTICHIP_ONEIMAGE))
    ca_uint8_t sw_idx;
#endif

#ifdef CONFIG_COMMON_RT_API
    OAM_CA_DRV_LOG(2,"%s %d : send OAM \n",__FUNCTION__,__LINE__);
    if ((ret = rt_epon_oam_tx(dataLen, data, llidIdx)) != RT_ERR_OK)
    {
        OAM_CA_DRV_LOG(2,"%s %d : TX OAM Error!!\n",__FUNCTION__,__LINE__);
    }
#else
    memset(&halPkt,0,sizeof(halPkt));
    halPkt.block_count    = 1;
    halPkt.cos            = 8;
    halPkt.device_id      = 0;
    halPkt.src_port_id    = CA_PORT_ID(CA_PORT_TYPE_CPU, 0x10);
    halPkt.dst_port_id    = CA_PORT_ID(CA_PORT_TYPE_EPON, 7);
    halPkt.pkt_type       = CA_PKT_TYPE_OAM;
    halPkt.pkt_len        = dataLen;
    halPkt.pkt_data       = (ca_pkt_block_t *)&pkt_data;
    halPkt.pkt_data->len  = dataLen;
    halPkt.pkt_data->data = data;
    halPkt.flags          = CA_TX_BYPASS_FWD_ENGINE;
#if defined(ONE_IMAGE_VENUS) || (defined(CONFIG_CA8277B_SERIES) && !defined(CONFIG_MULTICHIP_ONEIMAGE))
    aal_mpcp_reg_sw_index_offset_get(0,&sw_idx);
    halPkt.dst_sub_port_id = sw_idx;
#else
    halPkt.dst_sub_port_id = llidIdx;
#endif

    if(dataLen < 60)
    {
        memset(tmp,0,60);
        memcpy(tmp,data,dataLen);
        halPkt.pkt_len        = 60;
        halPkt.pkt_data->len  = 60;
        halPkt.pkt_data->data = tmp;
    }

    if ((ret = __ca_tx(0, &halPkt)) != CA_E_OK)
    {
        OAM_CA_DRV_LOG(2,"%s %d : TX OAM Error!!\n",__FUNCTION__,__LINE__);
    }
#endif
#endif
    OAM_CA_DRV_LOG(2,"======%s %d : TX OAM len %u ======\n",__FUNCTION__,__LINE__,dataLen);
    OAM_CA_DRV_LOG(2,"%s %d : TX OAM content\n",__FUNCTION__,__LINE__);
    if (oam_ca_drv_log > 2)
        print_hex_dump(KERN_INFO, "", DUMP_PREFIX_ADDRESS, 16, 1, data, dataLen, true);

}

int ca_rtk_epon_oam_handle_event(unsigned short dataLen, unsigned char *data)
{
    rtk_epon_oam_event_t event;
    unsigned char eventKey[] = {
        0x42, 0x15, /* Magic key */
    };

    if(memcmp(data,eventKey,sizeof(eventKey)))
        return -1;

    memcpy(&event, data+sizeof(eventKey), sizeof(rtk_epon_oam_event_t));
    printk("[%s] eventType[%d]\n", __func__, event.eventType);
    switch(event.eventType)
    {
        case RTK_EPON_OAM_STATE:
        {
            rtk_epon_fsm_status_t state = event.eventData[0];
            if((state <= RTK_EPON_STATE_OAM_AUTH_SUCC) && (state > RTK_EPON_STATE_UNKNOWN))
                queue_broadcast(MSG_TYPE_EPON_STATE, state, 0, ENABLED);
        }
        break;
        case RTK_EPON_OAM_EVENT:
        {
            unsigned int subType = event.eventData[0];
            if(subType == 0)
            {   /* tr069 wan vlan config */
                unsigned int config;
                memcpy(&config, event.eventData+1, sizeof(config));
                queue_broadcast(MSG_TYPE_EPON_EVENT, subType, config, ENABLED);
            }
        }
        break;
    }
    return 0;
}

void ca_rtk_epon_oam_pkt_tx(unsigned short dataLen, unsigned char *data)
{
    ca_pkt_t halPkt;
    ca_pkt_block_t pkt_data;
    int ret;
    unsigned char llidIdx;
    unsigned char tmp[60];
#if defined(ONE_IMAGE_VENUS) || (defined(CONFIG_CA8277B_SERIES) && !defined(CONFIG_MULTICHIP_ONEIMAGE))
    ca_uint8_t sw_idx;
#endif

    if(dataLen <= MAX_OAM_EVENT_SIZE)
    {
        ca_rtk_epon_oam_handle_event(dataLen,data);
        return;
    }
    
    llidIdx = *((unsigned char *) (data + (dataLen - sizeof(unsigned char))));
    dataLen = dataLen - sizeof(unsigned char); /*forllid*/

#ifdef CONFIG_COMMON_RT_API
    OAM_CA_DRV_LOG(2,"%s %d : send OAM \n",__FUNCTION__,__LINE__);
    if ((ret = rt_epon_oam_tx(dataLen, data, llidIdx)) != RT_ERR_OK)
    {
        OAM_CA_DRV_LOG(2,"%s %d : TX OAM Error!!\n",__FUNCTION__,__LINE__);
    }
#else
    memset(&halPkt,0,sizeof(halPkt));
    halPkt.block_count    = 1;
    halPkt.cos            = 8;
    halPkt.device_id      = 0;
    halPkt.src_port_id    = CA_PORT_ID(CA_PORT_TYPE_CPU, 0x10);
    halPkt.dst_port_id    = CA_PORT_ID(CA_PORT_TYPE_EPON, 7);
    halPkt.pkt_type       = CA_PKT_TYPE_OAM;
    halPkt.pkt_len        = dataLen;
    halPkt.pkt_data       = (ca_pkt_block_t *)&pkt_data;
    halPkt.pkt_data->len  = dataLen;
    halPkt.pkt_data->data = data;
    halPkt.flags          = CA_TX_BYPASS_FWD_ENGINE;
#if defined(ONE_IMAGE_VENUS) || (defined(CONFIG_CA8277B_SERIES) && !defined(CONFIG_MULTICHIP_ONEIMAGE))
    aal_mpcp_reg_sw_index_offset_get(0,&sw_idx);
    halPkt.dst_sub_port_id = sw_idx;
#else
    halPkt.dst_sub_port_id = llidIdx;
#endif

    if(dataLen < 60)
    {
        memset(tmp,0,60);
        memcpy(tmp,data,dataLen);
        halPkt.pkt_len        = 60;
        halPkt.pkt_data->len  = 60;
        halPkt.pkt_data->data = tmp;
    }

    OAM_CA_DRV_LOG(2,"%s %d : __ca_tx send OAM \n",__FUNCTION__,__LINE__);
    if ((ret = __ca_tx(0, &halPkt)) != CA_E_OK)
    {
        OAM_CA_DRV_LOG(2,"%s %d : TX OAM Error!!\n",__FUNCTION__,__LINE__);
    }
#endif

    OAM_CA_DRV_LOG(2,"======%s %d : TX OAM len %u ======\n",__FUNCTION__,__LINE__,dataLen);
    OAM_CA_DRV_LOG(2,"%s %d : TX OAM content\n",__FUNCTION__,__LINE__);
    if (oam_ca_drv_log > 2)
        print_hex_dump(KERN_INFO, "", DUMP_PREFIX_ADDRESS, 16, 1, data, dataLen, true);

    if(memcmp(data,oam_da,6))
    {
        OAM_CA_DRV_LOG(2,"%s %d : Not OAM DA no send\n",__FUNCTION__,__LINE__);
        return ;
    }
}

void ca_rtk_epon_oam_dyingGasp_tx(unsigned short dataLen, unsigned char *data)
{
    ca_uint32_t ret = CA_E_OK;
    ca_uint8_t send_count;

#ifdef CONFIG_COMMON_RT_API
    OAM_CA_DRV_LOG(2,"%s %d : send dying gasp OAM \n",__FUNCTION__,__LINE__);
    if ((ret = rt_epon_dyinggasp_set(dataLen, data)) != RT_ERR_OK)
    {
        OAM_CA_DRV_LOG(2,"%s %d : set OAM Error!!\n",__FUNCTION__,__LINE__);
    }
#if defined(ONE_IMAGE_77C) || (!defined(CONFIG_CA8279_SERIES) && !defined(CONFIG_CA8277B_SERIES) && !defined(CONFIG_MULTICHIP_ONEIMAGE))
    us_pon_sid_mapping_adding();
#endif
#else
    send_count = 3;

#ifdef !defined(CONFIG_CA8279_SERIES)
    if((ret = aal_epon_dying_gasp_set(0,0,send_count,data,dataLen)) != CA_E_OK)
    {
        OAM_CA_DRV_LOG(0,"%s %d : aal_epon_dying_gasp_set Error!!",__FUNCTION__,__LINE__);
        return ;
    }
#elif defined(ONE_IMAGE_VENUS) || (defined(CONFIG_CA8277B_SERIES) && !defined(CONFIG_MULTICHIP_ONEIMAGE))
    if((ret = aal_epon_dying_gasp_set(0,7,send_count,data,dataLen)) != CA_E_OK)
    {
        OAM_CA_DRV_LOG(0,"%s %d : aal_epon_dying_gasp_set Error!!",__FUNCTION__,__LINE__);
        return ;
    }
#else
    #"Error not support"
#endif
#endif
}

#define SUPPORT_OAM_KEEPLIVE
#ifdef SUPPORT_OAM_KEEPLIVE
/* siyuan 2017-08-07: oam keeplive mode: we parse the received standard oam discovery complete info packet,
   then generate oam info reply packet and send it to olt directly to avoid the case that eponoamd has no chance to send it in time
   when cpu is busy, otherwise olt will send a mpcp flag 2 to onu which causes onu offline. 
   also drop the standard oam info reply packet send by eponoamd.
 */
#define MAX_OAM_INFO_SIZE 					128
#define EPON_OAM_INFO_COMPLETE_FLAG1 		0x00
#define EPON_OAM_INFO_COMPLETE_FLAG2 		0x50
#define EPON_OAM_SUPPORT_LLID_NUM       	1    /* FIXME: only support llid 0 now */
#define EPON_OAM_OUI_LENGTH                 0x03
/* Info OAMPDU related */
#define EPON_OAMPDU_CODE_INFO               0x00
#define EPON_INFO_OAMPDU_INFO_LEN           0x10
#define EPON_INFO_HEADER_LEN           		0x12
#define EPON_INFO_OAMPDU_VENDER_SPEC_LEN    0x04
/* Info OAMPDU TLV types */
#define EPON_INFO_OAMPDU_TYPE_END           0x00
#define EPON_INFO_OAMPDU_TYPE_LOCAL         0x01
#define EPON_INFO_OAMPDU_TYPE_REMOTE        0x02
#define EPON_INFO_OAMPDU_TYPE_ORG_SPEC      0xFE
#define EPON_INFO_OAMPDU_TYPE_RESERVED      0xFF
#define EPON_INFO_OAMPDU_ORGSPEC_MIN_LEN    0x05 /* Type(1) + Length(1) + OUI(3) */

typedef struct oam_oamInfo_s {
	unsigned char valid;			/* If the OAM info valid */
	unsigned char oamVer;			/* OAM version defined in 802.3 section 57 */
	unsigned short revision;		/* Revision defined in 802.3 section 57 */
	unsigned char state;			/* State defined in 802.3 section 57 */
	unsigned char oamConfig;		/* OAM config defined in 802.3 section 57 */
	unsigned short oamPduConfig;	/* OAMPDU config defined in 802.3 section 57 */
	unsigned char oui[3];			/* OUI version defined in 802.3 section 57 */
	unsigned char venderSpecInfo[4];/* Vender Specific Info defined in 802.3 section 57 */
} oam_oamInfo_t;

typedef struct oam_config_s {
   unsigned char macAddr[6];        /* MAC address through out all OAM protocol */
} oam_config_t;

static unsigned char oam_info_reply[MAX_OAM_INFO_SIZE];
static unsigned char oam_info_keeplive_enable;
static oam_oamInfo_t localOamInfo[EPON_OAM_SUPPORT_LLID_NUM];
static oam_oamInfo_t remoteOamInfo[EPON_OAM_SUPPORT_LLID_NUM];
static oam_config_t  oamConfig[EPON_OAM_SUPPORT_LLID_NUM];
static long long oamrxtime[EPON_OAM_SUPPORT_LLID_NUM];

static unsigned char infoOrgSpecDb[][3] = {
	{ 0x11, 0x11, 0x11 }  /* CTC_OAM_OUI */
};

static void epon_oam_info_stdHdr_gen(unsigned short llidIdx, unsigned char *pFrameBuf)
{
    const unsigned char oamDstMac[6] = {0x01, 0x80, 0xc2, 0x00, 0x00, 0x02};
    unsigned short oamPduFlag;

    /* Already check the size at caller side */
    memcpy(pFrameBuf, oamDstMac, sizeof(oamDstMac));
    pFrameBuf += sizeof(oamDstMac);
    memcpy(pFrameBuf, oamConfig[llidIdx].macAddr, sizeof(oamConfig[llidIdx].macAddr));
    pFrameBuf += sizeof(oamConfig[llidIdx].macAddr);
    /* EtherType */
    *pFrameBuf = 0x88;
    pFrameBuf ++;
    *pFrameBuf = 0x09;
    pFrameBuf ++;
    /* SubType */
    *pFrameBuf = 0x03;
    pFrameBuf ++;
    /* Flag */
    *pFrameBuf = EPON_OAM_INFO_COMPLETE_FLAG1;
    pFrameBuf ++;
    *pFrameBuf = EPON_OAM_INFO_COMPLETE_FLAG2;
    pFrameBuf ++;
    /* Code */
    *pFrameBuf = EPON_OAMPDU_CODE_INFO;
}

static int epon_oam_localInfo_get(unsigned char llidIdx, oam_oamInfo_t *pOamInfo)
{
    /* Parameter check */
    if(llidIdx >= EPON_OAM_SUPPORT_LLID_NUM)
    {
        return -1;
    }	
	if(NULL == pOamInfo)
	{
		return -1;
	}
    *pOamInfo = localOamInfo[llidIdx];
	return 0;
}

static int epon_oam_remoteInfo_get(unsigned char llidIdx, oam_oamInfo_t *pOamInfo)
{
    /* Parameter check */
    if(llidIdx >= EPON_OAM_SUPPORT_LLID_NUM)
    {
        return -1;
    }
    if(NULL == pOamInfo)
    {
        return -1;
    }
    *pOamInfo = remoteOamInfo[llidIdx];
    return 0;
}

static int epon_oam_stdInfo_gen(
    unsigned char llidIdx,
    unsigned char *pBuf,
    unsigned short bufLen,
    unsigned short *pPushedLen)
{
    oam_oamInfo_t localInfo, remoteInfo;

    memset(&localInfo, 0x0, sizeof(oam_oamInfo_t));
    memset(&remoteInfo, 0x0, sizeof(oam_oamInfo_t));

    epon_oam_localInfo_get(llidIdx, &localInfo);
    if(bufLen > EPON_INFO_OAMPDU_INFO_LEN)
    {
        /* Add local info oam */
        pBuf[0] = EPON_INFO_OAMPDU_TYPE_LOCAL;
        pBuf[1] = EPON_INFO_OAMPDU_INFO_LEN;
        pBuf[2] = localInfo.oamVer;
        pBuf[3] = ((unsigned char *)(&localInfo.revision))[0];
        pBuf[4] = ((unsigned char *)(&localInfo.revision))[1];
        pBuf[5] = localInfo.state;
        pBuf[6] = localInfo.oamConfig;
        pBuf[7] = ((unsigned char *)(&localInfo.oamPduConfig))[0];
        pBuf[8] = ((unsigned char *)(&localInfo.oamPduConfig))[1];
        pBuf[9] = localInfo.oui[0];
        pBuf[10] = localInfo.oui[1];
        pBuf[11] = localInfo.oui[2];
        pBuf[12] = localInfo.venderSpecInfo[0];
        pBuf[13] = localInfo.venderSpecInfo[1];
        pBuf[14] = localInfo.venderSpecInfo[2];
        pBuf[15] = localInfo.venderSpecInfo[3];

        pBuf += EPON_INFO_OAMPDU_INFO_LEN;
        *pPushedLen += EPON_INFO_OAMPDU_INFO_LEN;

        epon_oam_remoteInfo_get(llidIdx, &remoteInfo);
        if(bufLen > (EPON_INFO_OAMPDU_INFO_LEN * 2))
        {
            /* Add remote info oam */
            pBuf[0] = EPON_INFO_OAMPDU_TYPE_REMOTE;
            pBuf[1] = EPON_INFO_OAMPDU_INFO_LEN;
            pBuf[2] = remoteInfo.oamVer;
            pBuf[3] = ((unsigned char *)(&remoteInfo.revision))[0];
            pBuf[4] = ((unsigned char *)(&remoteInfo.revision))[1];
            pBuf[5] = remoteInfo.state;
            pBuf[6] = remoteInfo.oamConfig;
            pBuf[7] = ((unsigned char *)(&remoteInfo.oamPduConfig))[0];
            pBuf[8] = ((unsigned char *)(&remoteInfo.oamPduConfig))[1];
            pBuf[9] = remoteInfo.oui[0];
            pBuf[10] = remoteInfo.oui[1];
            pBuf[11] = remoteInfo.oui[2];
            pBuf[12] = remoteInfo.venderSpecInfo[0];
            pBuf[13] = remoteInfo.venderSpecInfo[1];
            pBuf[14] = remoteInfo.venderSpecInfo[2];
            pBuf[15] = remoteInfo.venderSpecInfo[3];
            pBuf += EPON_INFO_OAMPDU_INFO_LEN;
            *pPushedLen += EPON_INFO_OAMPDU_INFO_LEN;

			/* Add end type for reply OAMPDU */
   	 		pBuf[0] = EPON_INFO_OAMPDU_TYPE_END;
    		*pPushedLen += 1;
        }
    }
    return 0;
}

static int epon_oam_stdInfo_parser(
    oam_oamInfo_t *pOamInfo,
    unsigned char *pFrame,
    unsigned short length,
    unsigned short *pExtractLen)
{
    unsigned char *pParsePtr;

    /* Minimum length requirement check */
    if(EPON_INFO_OAMPDU_INFO_LEN > length)
    {
        /* Remain length is not enough for parsing
         * Skip the TLV
         */
        *pExtractLen = length;
        return -1;
    }

    pParsePtr = pFrame + 1; /* Skip information type */
    if(EPON_INFO_OAMPDU_INFO_LEN != *pParsePtr)
    {
        /* Wrong length for local/remote info OAMPDU
         * Skip the TLV
         */
        *pExtractLen = length;
        return -1;
    }

    pParsePtr += 1;
    pOamInfo->oamVer = *pParsePtr;
    pParsePtr += 1;
    ((unsigned char *)(&pOamInfo->revision))[0] = *pParsePtr;
    pParsePtr += 1;
    ((unsigned char *)(&pOamInfo->revision))[1] = *pParsePtr;
    pParsePtr += 1;
    pOamInfo->state = *pParsePtr;
    pParsePtr += 1;
    pOamInfo->oamConfig = *pParsePtr;
    pParsePtr += 1;
    ((unsigned char *)(&pOamInfo->oamPduConfig))[0] = *pParsePtr;
    pParsePtr += 1;
    ((unsigned char *)(&pOamInfo->oamPduConfig))[1] = *pParsePtr;
    pParsePtr += 1;
    memcpy(pOamInfo->oui, pParsePtr, EPON_OAM_OUI_LENGTH);
    pParsePtr += EPON_OAM_OUI_LENGTH;
    memcpy(pOamInfo->venderSpecInfo, pParsePtr, EPON_INFO_OAMPDU_VENDER_SPEC_LEN);
    pParsePtr += EPON_INFO_OAMPDU_VENDER_SPEC_LEN;
    *pExtractLen = (pParsePtr - pFrame);

    return 0;
}

static int epon_oam_orgSpecInfo_find(unsigned char *pOui)
{
	int i;
	for(i = 0; i < sizeof(infoOrgSpecDb)/sizeof(infoOrgSpecDb[0]);i++)
	{
        if((infoOrgSpecDb[i][0] == pOui[0]) &&
           (infoOrgSpecDb[i][1] == pOui[1]) &&
           (infoOrgSpecDb[i][2] == pOui[2]))
        {
            return 0;
        }
    }
    return -1;
}

/* return value: 0:support, -1: not support */
static int epon_oam_orgSpecInfo_parser(
    unsigned char *pFrame,
    unsigned short length,
    unsigned short *pExtractLen)
{
    int ret;
    unsigned char *pParsePtr;
    unsigned short cbExtractLen, orgSpecLen;
	unsigned char oui[EPON_OAM_OUI_LENGTH];
	
    /* 1. Parsing the length and OUI first
     * 2. Check if it is supported by our oam via OUI
     */

    /* Minimum length requirement check */
    if( EPON_INFO_OAMPDU_ORGSPEC_MIN_LEN > length)
    {
        /* Remain length is not enough for parsing
         * Skip the TLV
         */
        *pExtractLen = length;
        return -1;
    }

    pParsePtr = pFrame + 1; /* Skip information type */
    orgSpecLen = *pParsePtr;
    if(length < orgSpecLen)
    {
        /* Remain length is not enough for parsing
         * Skip the TLV
         */
        *pExtractLen = length;
        return -1;
    }
	*pExtractLen = orgSpecLen;

    pParsePtr += 1; /* length */
    memcpy(oui, pParsePtr, EPON_OAM_OUI_LENGTH);
	
	ret = epon_oam_orgSpecInfo_find(oui);
	//printk("%s oui[%x%x%x] ret[%d]\n",__func__, oui[0],oui[1],oui[2], ret);
	return ret;
}

static int epon_oam_stdInfo_process(
	unsigned char llidIdx,
	unsigned char * data,
	unsigned short dataLen,
	int isRx)
{
    int ret = 0; /* ret vlaue 0: a standard oam info; -1:parse error; -2: contains more info content */
    unsigned char isEnd;
    unsigned char *pProcPtr;
    unsigned short remainLen, extractLen;
	oam_oamInfo_t oamInfo;
	
    isEnd = 0;
    pProcPtr = data;
    remainLen = dataLen;

    memset((unsigned char *) &oamInfo, 0x0, sizeof(oam_oamInfo_t));
    
    if(llidIdx>=EPON_OAM_SUPPORT_LLID_NUM)
        return -1;

    while((remainLen >= 1) && (1 != isEnd) && (0 == ret))
    {
        switch(*pProcPtr)
        {
        case EPON_INFO_OAMPDU_TYPE_END:
            /* OAM PDU ended */
            isEnd = 1;
            remainLen --;
            pProcPtr ++;
            break;
        case EPON_INFO_OAMPDU_TYPE_LOCAL:
            /* Receive Remote device's "LOCAL" TLV */
			if(isRx)
			{
	            ret = epon_oam_stdInfo_parser(&remoteOamInfo[llidIdx], pProcPtr, remainLen, &extractLen);
	            if(0 != ret)
	                break;
			}
			else
			{
				extractLen = EPON_INFO_OAMPDU_INFO_LEN;
			}
            remainLen -= extractLen;
            pProcPtr += extractLen;
            break;
        case EPON_INFO_OAMPDU_TYPE_REMOTE:
            /* Receive Remote device's "REMOTE" TLV */
			if(isRx)
			{
	            ret = epon_oam_stdInfo_parser(&oamInfo, pProcPtr, remainLen, &extractLen);
	            if(0 != ret)
	                break;

				memcpy(&localOamInfo[llidIdx], &oamInfo, sizeof(oam_oamInfo_t));
			}
			else
			{
				extractLen = EPON_INFO_OAMPDU_INFO_LEN;
			}
            remainLen -= extractLen;
            pProcPtr += extractLen;
            break;
        case EPON_INFO_OAMPDU_TYPE_ORG_SPEC:
			ret = epon_oam_orgSpecInfo_parser(pProcPtr, remainLen, &extractLen);
			if(ret == 0)
			{	/* supported org info, not reply in kernel */				
				ret = -2;
			}
			else
			{	/* unsupported org info, cotinue to parse */
				remainLen -= extractLen;
            	pProcPtr += extractLen;
				ret = 0;
			}
			break;
        case EPON_INFO_OAMPDU_TYPE_RESERVED:
        default:
			ret = -2; /* contains more info content */
            break;
        }
    }
	return ret;
}

void epon_oam_pkt_tx_for_keeplive(unsigned short dataLen, unsigned char *data)
{
	if(oam_info_keeplive_enable && (dataLen > MAX_OAM_EVENT_SIZE))
	{
		unsigned char llidIdx;
		llidIdx = *((unsigned char *) (data + (dataLen - sizeof(unsigned char))));
		if(llidIdx < EPON_OAM_SUPPORT_LLID_NUM )
		{
			if((data[15] == EPON_OAM_INFO_COMPLETE_FLAG1) && (data[16] == EPON_OAM_INFO_COMPLETE_FLAG2) && (data[17] == EPON_OAMPDU_CODE_INFO))  
			{
				//send a oam discover complete info packet	
				if(remoteOamInfo[llidIdx].valid == 0)
				{
					memcpy(oamConfig[llidIdx].macAddr, &data[6] , 6);
					remoteOamInfo[llidIdx].valid = 1;
					printk("%s llid[%d] valid mac[0x%x%x:%x%x:%x%x]\n",__func__,llidIdx,
						oamConfig[llidIdx].macAddr[0],oamConfig[llidIdx].macAddr[1],
						oamConfig[llidIdx].macAddr[2],oamConfig[llidIdx].macAddr[3],
						oamConfig[llidIdx].macAddr[4],oamConfig[llidIdx].macAddr[5]);
				}
				else
				{
					if(0 == epon_oam_stdInfo_process(llidIdx, data+EPON_INFO_HEADER_LEN, (dataLen-sizeof(unsigned char)-EPON_INFO_HEADER_LEN), 0))
					{
						//it is a standard oam info packet, not send it
						//printk("[%s:%d] llid[%d] len[%d] drop\n", __func__, __LINE__,llidIdx,dataLen);
						return;
					}
				}
			}
			else if(((data[16]&0xff) != EPON_OAM_INFO_COMPLETE_FLAG2) && (data[17] == EPON_OAMPDU_CODE_INFO))
			{	
				//send a oam discover not complete info packet
				remoteOamInfo[llidIdx].valid = 0;
				printk("%s llid[%d] unvalid\n",__func__,llidIdx);
			}
		}
	}
	ca_rtk_epon_oam_pkt_tx(dataLen,data);
}

static void epon_oam_reset_info(void)
{
	/* Init local/remote info database */
    memset((unsigned char *) &localOamInfo, 0x0, sizeof(oam_oamInfo_t) * EPON_OAM_SUPPORT_LLID_NUM);
    memset((unsigned char *) &remoteOamInfo, 0x0, sizeof(oam_oamInfo_t) * EPON_OAM_SUPPORT_LLID_NUM);
}

int epon_oam_keeplive_write_proc(struct file *file, const char __user *buffer, size_t len, loff_t *ppos)
{	
	unsigned char tmpBuf[16] = {0};
	int count = (len > 15) ? 15 : len;

	if (buffer && !copy_from_user(tmpBuf, buffer, count))
	{
		oam_info_keeplive_enable = simple_strtoul(tmpBuf, NULL, 16);
		switch(oam_info_keeplive_enable) 
		{
			case 0:
				epon_oam_reset_info();
				printk("Turn off kernel oam keeplive mode\n");
				break;
			case 1:
				printk("Turn on kernel oam keeplive mode\n");
				break;
		}
		return count;
	}	
	return -EFAULT;
}

static int epon_oam_keeplive_read_proc(struct seq_file *seq, void *v)
{
	int i;
	seq_printf(seq, "Kernel Oam Keeplive Mode:%s\n", (oam_info_keeplive_enable != 0)?"ON":"OFF");
	if(oam_info_keeplive_enable)
	{
		for(i=0; i<EPON_OAM_SUPPORT_LLID_NUM; i++)
		{
			seq_printf(seq, "%d:%lld\n", i, oamrxtime[i]);
		}
	}
	return 0;
}

static int proc_oam_keeplive_open(struct inode *inode, struct file *file)
{
	return single_open(file, epon_oam_keeplive_read_proc, inode->i_private);
}
#if LINUX_VERSION_CODE <= KERNEL_VERSION(5,0,0)
static const struct file_operations oamKeeplive_ops = {
        .owner          = THIS_MODULE,
        .open           = proc_oam_keeplive_open,
        .read           = seq_read,
        .write          = epon_oam_keeplive_write_proc,
        .llseek         = seq_lseek,
        .release        = single_release,
};
#else
static const struct proc_ops oamKeeplive_ops = {
	.proc_open = proc_oam_keeplive_open,
	.proc_write = epon_oam_keeplive_write_proc,
	.proc_release = single_release,
	.proc_read = seq_read,
	.proc_lseek = seq_lseek,
};
#endif
static void epon_oam_keeplive_init(void)
{
	struct proc_dir_entry *entry=NULL;
	entry = proc_create_data("oamKeeplive", 0644, NULL, &oamKeeplive_ops,NULL);
	if (entry == NULL)
	{
		printk("%s create proc oamKeeplive fail!\n",__func__);	
		remove_proc_entry("oamKeeplive", NULL);
		return;
	}

	oam_info_keeplive_enable = 1; /* enable kepplive by default */

	epon_oam_reset_info();
}
#endif

#ifdef CONFIG_COMMON_RT_API
uint32 rt_oam_rx(uint32 msgLen,uint8 *pMsg, uint8 llidIdx)
{
    int ret;
    uint8 *oam_ptr = NULL;
    uint32_t oam_len = msgLen;

    OAM_CA_DRV_LOG(2,"======%s %d : RX oam len %u llidIdx %u======\n",__FUNCTION__,__LINE__,msgLen,llidIdx);
    if (oam_ca_drv_log > 2)
        print_hex_dump(KERN_INFO, "", DUMP_PREFIX_ADDRESS, 16, 1, pMsg, msgLen, true);

    if(S_OAM.num > 0)
    {
        int i,j,match;
        ca_mac_addr_t olt_mac;
        ca_mac_addr_t onu_mac;

        match = 0;

        memset((void *)&olt_mac[0], 0, sizeof(olt_mac));
        aal_mpcp_olt_mac_addr_get(0, 0, olt_mac);

        for(j=0;j<S_OAM.num;j++)
        {
            match = 1;
            for(i=12;i<S_OAM.rx_len[j]-12;i++)
            {
                if(pMsg[i]!=S_OAM.rx[j][i])
                {
                    match = 0;
                    break;
                }
            }
            if(match == 1)
                break;
        }
        
        if(match == 1)
        {
            memset((void *)&onu_mac[0], 0, sizeof(onu_mac));
            aal_mpcp_mac_addr_get(0, 0, llidIdx, onu_mac);
            memcpy(&S_OAM.tx[j][6],&onu_mac[0],6);
            OAM_CA_DRV_LOG(2,"OAM entry num = %d\n",j+1);
            rt_epon_oam_tx(S_OAM.tx_len[j],S_OAM.tx[j],llidIdx);
        }
        else
        {
            OAM_CA_DRV_LOG(2,"====Not Match RX OAM content oam_len=%d====\n",oam_len);
            if (oam_ca_drv_log > 2)
                print_hex_dump(KERN_INFO, "", DUMP_PREFIX_ADDRESS, 16, 1, pMsg, oam_len, true);
        }
        return 0;
    }

    oam_ptr=(uint8 *)kmalloc((msgLen+8)*sizeof(uint8),GFP_ATOMIC);
    if (!oam_ptr)
    {
        OAM_CA_DRV_LOG(2,"kmalloc (%d) fail\n", PR_USER_UID_EPONOAM);
        return 0;
    }

    memcpy(oam_ptr,pMsg,msgLen);
    *((uint8 *)(oam_ptr + oam_len)) = llidIdx;
    
    ret = pkt_redirect_kernelApp_sendPkt(PR_USER_UID_EPONOAM, 1, oam_len +  sizeof(llidIdx), oam_ptr);
    if(ret)
    {
        OAM_CA_DRV_LOG(2,"send to user app (%d) fail (%d)\n", PR_USER_UID_EPONOAM, ret);
    }
#ifdef SUPPORT_OAM_KEEPLIVE
	if(oam_info_keeplive_enable && (llidIdx < EPON_OAM_SUPPORT_LLID_NUM))
	{
		if((oam_ptr[15] == EPON_OAM_INFO_COMPLETE_FLAG1) && (oam_ptr[16] == EPON_OAM_INFO_COMPLETE_FLAG2) && (oam_ptr[17] == EPON_OAMPDU_CODE_INFO)
			&& remoteOamInfo[llidIdx].valid )
		{	//receive a oam discover complete info packet
			unsigned char * data;
			unsigned short length;

			data = &oam_ptr[EPON_INFO_HEADER_LEN];
			length = msgLen - EPON_INFO_HEADER_LEN;
			
			if(0 == epon_oam_stdInfo_process(llidIdx, data, length, 1))
			{
				//it is a standard oam info packet, send it directly				
				memset(oam_info_reply, 0, MAX_OAM_INFO_SIZE);
				epon_oam_info_stdHdr_gen(llidIdx, oam_info_reply);
				data = &oam_info_reply[EPON_INFO_HEADER_LEN];
				length = EPON_INFO_HEADER_LEN;
				
				epon_oam_stdInfo_gen(llidIdx, data, MAX_OAM_INFO_SIZE - EPON_INFO_HEADER_LEN, &length);
				
				if(length>=(MAX_OAM_INFO_SIZE-1))
				{
					printk("data length exceed!!\n");
					goto oam_rx_exit;	   
				}
				
				oam_info_reply[length++] = ((unsigned char) llidIdx);

				OAM_CA_DRV_LOG(2,"%s %d : send oam info reply len %u llidIdx %u\n",__FUNCTION__,__LINE__,length,llidIdx);
				ca_rtk_epon_oam_pkt_tx(length,oam_info_reply);
				oamrxtime[llidIdx] = ktime_to_ms(ktime_get_boottime());
			}
		}
	}
#endif
oam_rx_exit:

    if(oam_ptr != NULL)
        kfree(oam_ptr);

    return 0;
}
#endif

int ca_rtk_epon_oam_pkt_rx(struct napi_struct *napi,struct net_device *dev, struct sk_buff *skb, nic_hook_private_t *nh_priv)
{
    int ret;
    uint8_t * oam_ptr;
    uint32_t oam_len;
    unsigned char llidIdx; 
    uint8 isOamFlag = FALSE;
#if defined(ONE_IMAGE_VENUS) || (defined(CONFIG_CA8277B_SERIES) && !defined(CONFIG_MULTICHIP_ONEIMAGE))
    ca_uint8_t sw_idx;
#endif
    
    ca_rtk_epon_oam_pkt_rx_cnt++;
    
    if(0 != memcmp(&skb->data[0],puc_sa,6))
    {
        return RE8670_RX_CONTINUE;
    }

    if (0 == memcmp(&skb->data[12], etype_oam, sizeof(etype_oam)))
    {
        llidIdx = skb->data[11] & 0x3f;  /* restore llid: puc_da[5:0] */
        oam_ptr = skb->data+ETH_HEAD_LEN;
        oam_len = skb->len-ETH_HEAD_LEN;
        isOamFlag = TRUE;
    }
    else if(0 == memcmp(&skb->data[12], etype_vlan, sizeof(etype_vlan))) //single VID
    {
        if (0 == memcmp(&skb->data[16], etype_oam, sizeof(etype_oam)))
        {
            llidIdx = skb->data[11] & 0x3f;  /* restore llid: puc_da[5:0] */
            oam_ptr = skb->data+ETH_HEAD_LEN+4;
            oam_len = skb->len-ETH_HEAD_LEN-4;
            isOamFlag = TRUE;
        }
    }

    //check oam ether type
    if (isOamFlag == TRUE)
    {
        OAM_CA_DRV_LOG(2,"======%s %d : RX skb len %u llidIdx %u======\n",__FUNCTION__,__LINE__,skb->len,llidIdx);
        OAM_CA_DRV_LOG(2,"%s %d : RX OAM pre header\n",__FUNCTION__,__LINE__);
        if (oam_ca_drv_log > 2)
            print_hex_dump(KERN_INFO, "", DUMP_PREFIX_ADDRESS, 16, 1, skb->data, ETH_HEAD_LEN, true);

        OAM_CA_DRV_LOG(2,"%s %d : RX OAM content\n",__FUNCTION__,__LINE__);
        if (oam_ca_drv_log > 2)
            print_hex_dump(KERN_INFO, "", DUMP_PREFIX_ADDRESS, 16, 1, oam_ptr, oam_len, true);

        if((oam_ptr[12] == 0x88) && (oam_ptr[13] == 0x09) && (oam_ptr[14] == 0x03))
        {
            /* llid index of boardcast oam packet is 0x08, 0x0A(10) or 0x0B(11), it must be changed to llid 0 */
            if(llidIdx == 0x08 || llidIdx == 0x0A || llidIdx == 0x0B || llidIdx == 0x3f)
                llidIdx = 0;

#if defined(ONE_IMAGE_VENUS) || (defined(CONFIG_CA8277B_SERIES) && !defined(CONFIG_MULTICHIP_ONEIMAGE))
            aal_mpcp_reg_sw_index_offset_get(0,&sw_idx);
            llidIdx= llidIdx-sw_idx;
#endif

            if(S_OAM.num > 0)
            {
                int i,j,match;
                ca_mac_addr_t olt_mac;
                ca_mac_addr_t onu_mac;

                match = 0;

                memset((void *)&olt_mac[0], 0, sizeof(olt_mac));
                aal_mpcp_olt_mac_addr_get(0, 0, olt_mac);

                for(j=0;j<S_OAM.num;j++)
                {
                    match = 1;
                    for(i=12;i<S_OAM.rx_len[j]-12;i++)
                    {
                        if(oam_ptr[i]!=S_OAM.rx[j][i])
                        {
                            match = 0;
                            break;
                        }
                    }
                    if(match == 1)
                        break;
                }
                
                if(match == 1)
                {
                    memset((void *)&onu_mac[0], 0, sizeof(onu_mac));
                    aal_mpcp_mac_addr_get(0, 0, llidIdx, onu_mac);
                    memcpy(&S_OAM.tx[j][6],&onu_mac[0],6);
                    OAM_CA_DRV_LOG(2,"OAM entry num = %d\n",j+1);
                    send_static_oam_pkt_tx(llidIdx,S_OAM.tx_len[j],S_OAM.tx[j]);
                }
                else
                {
                    OAM_CA_DRV_LOG(2,"====Not Match RX OAM content oam_len=%d====\n",oam_len);
                    if (oam_ca_drv_log > 2)
                        print_hex_dump(KERN_INFO, "", DUMP_PREFIX_ADDRESS, 16, 1, oam_ptr, oam_len, true);
                }
                return RE8670_RX_STOP;
            }

            skb_put(skb, sizeof(llidIdx));
            *((unsigned char *)(skb->data + (skb->len - sizeof(llidIdx)))) = llidIdx;
            
            ret = pkt_redirect_kernelApp_sendPkt(PR_USER_UID_EPONOAM, 1, oam_len +  sizeof(llidIdx), oam_ptr);
            if(ret)
            {
                OAM_CA_DRV_LOG(2,"send to user app (%d) fail (%d)\n", PR_USER_UID_EPONOAM, ret);
            }
#ifdef SUPPORT_OAM_KEEPLIVE
			if(oam_info_keeplive_enable && (llidIdx < EPON_OAM_SUPPORT_LLID_NUM))
			{
				if((oam_ptr[15] == EPON_OAM_INFO_COMPLETE_FLAG1) && (oam_ptr[16] == EPON_OAM_INFO_COMPLETE_FLAG2) && (oam_ptr[17] == EPON_OAMPDU_CODE_INFO)
					&& remoteOamInfo[llidIdx].valid )
				{	//receive a oam discover complete info packet
					unsigned char * data;
					unsigned short length;
		
					data = &oam_ptr[EPON_INFO_HEADER_LEN];
					length = oam_len - EPON_INFO_HEADER_LEN;
					
					if(0 == epon_oam_stdInfo_process(llidIdx, data, length, 1))
					{
						//it is a standard oam info packet, send it directly				
						memset(oam_info_reply, 0, MAX_OAM_INFO_SIZE);
						epon_oam_info_stdHdr_gen(llidIdx, oam_info_reply);
						data = &oam_info_reply[EPON_INFO_HEADER_LEN];
						length = EPON_INFO_HEADER_LEN;
						
						epon_oam_stdInfo_gen(llidIdx, data, MAX_OAM_INFO_SIZE - EPON_INFO_HEADER_LEN, &length);
						
						if(length>=(MAX_OAM_INFO_SIZE-1))
						{
							printk("data length exceed!!\n");
							goto oam_rx_exit;	   
						}
						
						oam_info_reply[length++] = ((unsigned char) llidIdx);
		
						OAM_CA_DRV_LOG(2,"%s %d : send oam info reply len %u llidIdx %u\n",__FUNCTION__,__LINE__,length,llidIdx);
						ca_rtk_epon_oam_pkt_tx(length,oam_info_reply);
						oamrxtime[llidIdx] = ktime_to_ms(ktime_get_boottime());
					}
				}
			}
oam_rx_exit:
#endif

            return RE8670_RX_STOP;
        }
    }
    return RE8670_RX_CONTINUE;
}

ca_uint32_t ca_rtk_epon_reg_intr(ca_device_id_t device_id,ca_uint32_t event_id,ca_void_t *event,ca_uint32_t len,ca_void_t *cookie)
{
    ca_uint32_t ret;
    ca_event_reg_stat_t* evt_ptr = (ca_event_reg_stat_t *)event;

    if(evt_ptr->status == 1)
    {
        if(one_shot_setting == 0)
        {
#ifdef CONFIG_CA8279_SERIES
            saturn_us_pon_rule_adding();
#elif defined(CONFIG_CA8277B_SERIES) || defined(CONFIG_RTL8277C_SERIES) || defined(CONFIG_RTL9607F_SERIES)
            us_pon_sid_mapping_adding();
#endif
            one_shot_setting = 1;
        }
#if defined(ONE_IMAGE_77C) || defined(ONE_IMAGE_07F) || (!defined(CONFIG_CA8279_SERIES) && !defined(CONFIG_CA8277B_SERIES) && !defined(CONFIG_MULTICHIP_ONEIMAGE))
        us_pon_sid_mapping_adding();
#endif
        printk("llid-idx %d llid %d register\n",evt_ptr->llid_idx,evt_ptr->llid_value);
    }
    else
   {
        /*stop epon driver register*/
#ifdef CONFIG_COMMON_RT_API
        rt_epon_llid_entry_t llidEntry;
        llidEntry.llidIdx = evt_ptr->llid_idx;

        ret = rt_epon_llid_entry_get(&llidEntry);
        if(RT_ERR_OK != ret)
        {
            return ret;
        }

        llidEntry.valid = DISABLED;
        llidEntry.llid = 0x7fff;
        ret = rt_epon_llid_entry_set(&llidEntry);
        if(RT_ERR_OK != ret)
        {
            return ret;
        }
#else
        rtk_epon_llid_entry_t llidEntry;
        llidEntry.llidIdx = evt_ptr->llid_idx;

        ret = rtk_epon_llid_entry_get(&llidEntry);
        if(RT_ERR_OK != ret)
        {
            return ret;
        }

        llidEntry.valid = DISABLED;
        llidEntry.llid = 0x7fff;
        ret = rtk_epon_llid_entry_set(&llidEntry);
        if(RT_ERR_OK != ret)
        {
            return ret;
        }
#endif/*CONFIG_COMMON_RT_API*/
        printk("llid-idx %d de-register\n",evt_ptr->llid_idx);
    }

    return 0;
}

ca_uint32_t ca_rtk_epon_port_intr(ca_device_id_t device_id,ca_uint32_t event_id,ca_void_t *event,ca_uint32_t len,ca_void_t *cookie)
{
    unsigned char losMsg[5] = {
        0x15, 0x68, /* Magic key */
        0xde, 0xad  /* Message body */
    };
    unsigned char losRecoverMsg[5] = {
        0x15, 0x68, /* Magic key */
        0x55, 0x55  /* Message body */
    };
    ca_event_epon_port_link_t* evt_ptr = (ca_event_epon_port_link_t *)event;

    if(evt_ptr->port_id == CA_PORT_ID(CA_PORT_TYPE_EPON, CA_PORT_ID_NI7))
    {
        if(evt_ptr->status == 1)
        {
            OAM_CA_DRV_LOG(0,"epon port link up\n");
            pkt_redirect_kernelApp_sendPkt(PR_USER_UID_EPONOAM, 1, sizeof(losRecoverMsg), losRecoverMsg);
        }
        else
        {
            OAM_CA_DRV_LOG(0,"epon port link down\n");
            pkt_redirect_kernelApp_sendPkt(PR_USER_UID_EPONOAM, 1, sizeof(losMsg), losMsg);
        }
    }
    return 0;
}

ca_uint32_t ca_rtk_epon_silence_intr(ca_device_id_t device_id,ca_uint32_t event_id,ca_void_t *event,ca_uint32_t len,ca_void_t *cookie)
{
    ca_event_silence_stat_t* evt_ptr = (ca_event_silence_stat_t *)event;

    unsigned char silentMsg[] = {
        0x15, 0x68, /* Magic key */
        0x66, 0x66  /* Message body */
    };

    printk("llid-idx %d silence status=%d\n",evt_ptr->llid_idx,evt_ptr->status);

    OAM_CA_DRV_LOG(0,"llid-idx %d silence status=%d\n",evt_ptr->llid_idx,evt_ptr->status);

    if(evt_ptr->status == 1)
    {
        pkt_redirect_kernelApp_sendPkt(PR_USER_UID_EPONOAM, 1, sizeof(silentMsg), silentMsg);
    }

    return 0;
}

#ifdef CONFIG_CA8279_SERIES
/*Setting Saturn port 6 VLAN*/
static int saturn_us_pon_vlan_port_add(unsigned int vid,ca_port_id_t port)
{
    unsigned char count = 1;
    ca_uint8_t oriCount = 0;
    ca_uint8_t oriUntagCount = 0;
    ca_uint8_t idx = 0;
    ca_status_t ret = CA_E_OK;
    ca_uint8_t wanFlag = FALSE;
    ca_port_id_t memPort[9];
    ca_port_id_t oriMemPort[9];
    ca_port_id_t oriUntagMemPort[9];

    aal_ilpb_cfg_msk_t              ilpb_cfg_mask;
    aal_ilpb_cfg_t                  ilpb_cfg;
    aal_elpb_cfg_msk_t              elpb_cfg_mask;
    aal_elpb_cfg_t                  elpb_cfg;

    ret = ca_l2_vlan_create(1, vid);
    if ((CA_E_OK != ret) && (CA_E_EXISTS != ret))
    {
        printk("%s %d vlan create fail vid:%d ret:%d",
            __FUNCTION__, __LINE__, vid, ret);
        return RT_ERR_FAILED;
    }

    memset(&ilpb_cfg_mask, 0, sizeof(aal_ilpb_cfg_msk_t));
    memset(&elpb_cfg_mask, 0, sizeof(aal_elpb_cfg_msk_t));
    memset(&ilpb_cfg, 0, sizeof(aal_ilpb_cfg_t));
    memset(&elpb_cfg, 0, sizeof(aal_elpb_cfg_t));


    /*Set COS mode sel to Marking mode */
    ilpb_cfg_mask.s.cos_mode_sel = 1;  
    ilpb_cfg.cos_mode_sel = AAL_COS_MODE_COS_SEL_BMP;
    ilpb_cfg_mask.s.ing_fltr_ena = 1;
    ilpb_cfg.ing_fltr_ena = 0;
    elpb_cfg_mask.s.egr_ve_srch_en = 1;
    elpb_cfg.egr_ve_srch_en = 0;

    /*Disable ingress VLAN member check*/
    ret = aal_port_ilpb_cfg_set(1, PORT_ID(port), ilpb_cfg_mask, &ilpb_cfg);
    if (CA_E_OK != ret)
    {
        printk("%s %d : aal_port_ilpb_cfg_set fail",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*Disable egress VLAN member check*/
    aal_port_elpb_cfg_set(1, PORT_ID(port), elpb_cfg_mask, &elpb_cfg);
    if (CA_E_OK != ret) 
    {
        printk("%s %d : aal_port_elpb_cfg_set fail",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

static int saturn_us_pon_rule_setting(ca_uint32_t llid,ca_uint32_t cos,ca_uint32_t *cls_index,ca_uint8_t flag)
{
    int32_t ret = RT_ERR_OK;
    ca_uint32_t priority;
    ca_classifier_key_t key;
    ca_classifier_key_mask_t key_mask;
    ca_classifier_action_t action;
    ca_uint32_t get_priority;
    ca_classifier_key_t get_key;
    ca_classifier_key_mask_t get_key_mask;
    ca_classifier_action_t get_action;
    ca_uint32_t rtk_llid_cos,ca_llid_cos;
    rtk_llid_cos = (llid<<4) | cos;
    ca_llid_cos = ((llid<<11) | cos<<8 ) + (llid*8+cos+1); /*llid | tx_cos | flow_id*/

    memset(&key,0,sizeof(ca_classifier_key_t));
    memset(&key_mask,0,sizeof(ca_classifier_key_mask_t));
    memset(&action,0,sizeof(ca_classifier_action_t));

    priority=7;

    key.src_port = CA_PORT_ID(CA_PORT_TYPE_ETHERNET, 6);
    key_mask.src_port = 1;

    key.l2.vlan_count = 1;
    key_mask.l2 = 1;

    key.l2.vlan_otag.vlan_max.vid = rtk_llid_cos;
    key.l2.vlan_otag.vlan_min.vid = rtk_llid_cos;
    key_mask.l2_mask.vlan_otag = 1;
    key_mask.l2_mask.vlan_otag_mask.vid =1;

    action.forward = CA_CLASSIFIER_FORWARD_PORT;
    action.dest.port = CA_PORT_ID(CA_PORT_TYPE_EPON, 7);

    action.options.action_handle.llid_cos_index = ca_llid_cos;
    action.options.masks.action_handle = 1;

    action.options.outer_vlan_act = CA_CLASSIFIER_VLAN_ACTION_POP;
    action.options.masks.outer_vlan_act = 1;

    if(flag == 1) //Adding
    {
        OAM_CA_DRV_LOG(1,"%s %d : saturn_vlan_port_add!!\n",__FUNCTION__,__LINE__);
        if((ret = saturn_us_pon_vlan_port_add(rtk_llid_cos,CA_PORT_ID(CA_PORT_TYPE_ETHERNET,CA_PORT_ID_NI6))) != CA_E_OK)
        {
            printk("%s %d : saturn_vlan_port_add Error ret=%d!!\n",__FUNCTION__,__LINE__,ret);
            return RT_ERR_FAILED;
        }
        OAM_CA_DRV_LOG(1,"%s %d : saturn_vlan_port_add Success!!\n",__FUNCTION__,__LINE__);

        OAM_CA_DRV_LOG(1,"%s %d : ca_classifier_rule_add cls_index=%d!!\n",__FUNCTION__,__LINE__,*cls_index);
        /*CA Driver support check existing rule if existing will return correct rule index before adding rule*/
        if((ret = ca_classifier_rule_add(0,priority,&key,&key_mask,&action,cls_index)) != CA_E_OK)
        {
            printk("%s %d : ca_classifier_rule_add Error ret=%d!!\n",__FUNCTION__,__LINE__,ret);
            return RT_ERR_FAILED;
        }
        OAM_CA_DRV_LOG(1,"%s %d : ca_classifier_rule_add Success cls_index=%d!!\n",__FUNCTION__,__LINE__,*cls_index);
    }
    else //Deleting
    {
        while(1)
        {
            memset(&get_key,0,sizeof(ca_classifier_key_t));
            memset(&get_key_mask,0,sizeof(ca_classifier_key_mask_t));
            memset(&get_action,0,sizeof(ca_classifier_action_t));

            OAM_CA_DRV_LOG(1,"%s %d : ca_classifier_rule_get cls_index=%d!!\n",__FUNCTION__,__LINE__,*cls_index);
            ret = ca_classifier_rule_get(0,*cls_index,&get_priority,&get_key,&get_key_mask,&get_action);
            if(ret == CA_E_NOT_FOUND)
            {
                printk("%s %d : ca_classifier_rule_get index=%d Entry not found!!\n",__FUNCTION__,__LINE__,*cls_index);
                *cls_index = -1;
                return RT_ERR_OK;
            }
            OAM_CA_DRV_LOG(1,"%s %d : ca_classifier_rule_get cls_index=%d!!\n",__FUNCTION__,__LINE__,*cls_index);

    //        printk("priority = %d\n",priority);
    //        printk("key.src_port = 0x%x\n",key.src_port);
    //        printk("key_mask.src_port = %d\n",key_mask.src_port);
    //        printk("key.l2.vlan_count = %d\n",key.l2.vlan_count);
    //        printk("key_mask.l2 = %d\n",key_mask.l2);
    //        printk("key.l2.vlan_otag.vlan_max.vid = %d\n",key.l2.vlan_otag.vlan_max.vid);
    //        printk("key.l2.vlan_otag.vlan_min.vid = %d\n",key.l2.vlan_otag.vlan_min.vid);
    //        printk("key_mask.l2_mask.vlan_otag = %d\n",key_mask.l2_mask.vlan_otag);
    //        printk("key_mask.l2_mask.vlan_otag_mask.vid = %d\n",key_mask.l2_mask.vlan_otag_mask.vid);
    //        printk("action.forward = %d\n",action.forward);
    //        printk("action.dest.port = 0x%x\n",action.dest.port);
    //        printk("action.options.action_handle.llid_cos_index = 0x%x\n",action.options.action_handle.llid_cos_index);
    //        printk("action.options.masks.action_handle = %d\n",action.options.masks.action_handle);
    //        printk("action.options.outer_vlan_act = %d\n",action.options.outer_vlan_act);
    //        printk("action.options.masks.outer_vlan_act = %d\n",action.options.masks.outer_vlan_act);
    //
    //        printk("get_priority = %d\n",get_priority);
    //        printk("get_key.src_port = 0x%x\n",get_key.src_port);
    //        printk("get_key_mask.src_port = %d\n",get_key_mask.src_port);
    //        printk("get_key.l2.vlan_count = %d\n",get_key.l2.vlan_count);
    //        printk("get_key_mask.l2 = %d\n",get_key_mask.l2);
    //        printk("get_key.l2.vlan_otag.vlan_max.vid = %d\n",get_key.l2.vlan_otag.vlan_max.vid);
    //        printk("get_key.l2.vlan_otag.vlan_min.vid = %d\n",get_key.l2.vlan_otag.vlan_min.vid);
    //        printk("get_key_mask.l2_mask.vlan_otag = %d\n",get_key_mask.l2_mask.vlan_otag);
    //        printk("get_key_mask.l2_mask.vlan_otag_mask.vid = %d\n",get_key_mask.l2_mask.vlan_otag_mask.vid);
    //        printk("get_action.forward = %d\n",get_action.forward);
    //        printk("get_action.dest.port = 0x%x\n",get_action.dest.port);
    //        printk("get_action.options.action_handle.llid_cos_index = 0x%x\n",get_action.options.action_handle.llid_cos_index);
    //        printk("get_action.options.masks.action_handle = %d\n",get_action.options.masks.action_handle);
    //        printk("get_action.options.outer_vlan_act = %d\n",get_action.options.outer_vlan_act);
    //        printk("get_action.options.masks.outer_vlan_act = %d\n",get_action.options.masks.outer_vlan_act);

            if(memcmp(&priority,&get_priority,sizeof(ca_uint32_t)))
            {
                printk("priority not match!!\n");
                *cls_index = -1;
                return RT_ERR_OK;
            }

            if(memcmp(&key,&get_key,sizeof(ca_classifier_key_t)))
            {
                printk("key not match!!\n");
                *cls_index = -1;
                return RT_ERR_OK;
            }

            if(memcmp(&key_mask,&get_key_mask,sizeof(ca_classifier_key_mask_t)))
            {
                printk("key_mask not match!!\n");
                *cls_index = -1;
                return RT_ERR_OK;
            }

            if(memcmp(&action,&get_action,sizeof(ca_classifier_action_t)))
            {
                printk("action not match!!\n");
                *cls_index = -1;
                return RT_ERR_OK;
            }

            OAM_CA_DRV_LOG(1,"%s %d : ca_classifier_rule_delete index=%d!!\n",__FUNCTION__,__LINE__,*cls_index);
            if((ret = ca_classifier_rule_delete(0,*cls_index)) != CA_E_OK)
            {
                printk("%s %d : ca_classifier_rule_delete index=%d Error ret=%d!!\n",__FUNCTION__,__LINE__,*cls_index,ret);
                return RT_ERR_FAILED;
            }
        }
    }

    return RT_ERR_OK;
}

static int saturn_us_pon_rule_adding(void)
{
    int32_t ret = RT_ERR_OK;
    ca_uint32_t llid,cos,llid_cos_idx;

    for(llid=0;llid<MAX_LLID;llid++)
    {
        for(cos=0;cos<MAX_COS;cos++)
        {
            llid_cos_idx = llid*8+cos;
            if(llid_cos_idx < MAX_QUEUE)
            {
                if((ret = saturn_us_pon_rule_setting(llid,cos,&gbl_us_pon_cls_index[llid_cos_idx],1)) != RT_ERR_OK)
                {
                    printk("%s %d : saturn_us_pon_rule_setting llid=%d cos=%d Error ret=%d!!\n",__FUNCTION__,__LINE__,llid,cos,ret);
                    return RT_ERR_FAILED;
                }
            }
            else
            {
                printk("%s %d : PON Queue is only %d llid=%d cos=%d not support!!\n",__FUNCTION__,__LINE__,MAX_QUEUE,llid,cos);
                return RT_ERR_FAILED;
            }
        }
    }

    return RT_ERR_OK;
}

static int saturn_us_pon_rule_deleting(void)
{
    int32_t ret = RT_ERR_OK;
    ca_uint32_t llid,cos,llid_cos_idx;

    for(llid=0;llid<MAX_LLID;llid++)
    {
        for(cos=0;cos<MAX_COS;cos++)
        {
            llid_cos_idx = llid*8+cos;
            if(llid_cos_idx < MAX_QUEUE)
            {
                if((ret = saturn_us_pon_rule_setting(llid,cos,&gbl_us_pon_cls_index[llid_cos_idx],0)) != RT_ERR_OK)
                {
                    printk("%s %d : saturn_us_pon_rule_setting llid=%d cos=%d Error ret=%d!!\n",__FUNCTION__,__LINE__,llid,cos,ret);
                    return RT_ERR_FAILED;
                }
            }
            else
            {
                printk("%s %d : PON Queue is only %d llid=%d cos=%d not support!!\n",__FUNCTION__,__LINE__,MAX_QUEUE,llid,cos);
                return RT_ERR_FAILED;
            }
        }
    }
    return RT_ERR_OK;
}
#endif

#if !defined(CONFIG_CA82779_SERIES)
static int us_pon_sid_mapping_adding(void)
{
#ifdef CONFIG_COMMON_RT_API
    uint16 sid;
    rt_ponmisc_sidInfo_t sidInfo;
    ca_uint8_t sw_idx;

    aal_mpcp_reg_sw_index_offset_get(0,&sw_idx);

    memset(&sidInfo,0,sizeof(rt_ponmisc_sidInfo_t));

    for(sid=0;sid<MAX_SID;sid++)
    {
        sidInfo.enFlag = ENABLED;
        sidInfo.dqFlag = ENABLED;
#if defined(ONE_IMAGE_VENUS) || (defined(CONFIG_CA8277B_SERIES) && !defined(CONFIG_MULTICHIP_ONEIMAGE))
        sidInfo.dstPort = sw_idx;
#elif defined(ONE_IMAGE_77C) || defined(ONE_IMAGE_07F) || (!defined(CONFIG_CA8279_SERIES) && !defined(CONFIG_CA8277B_SERIES) && !defined(CONFIG_MULTICHIP_ONEIMAGE))
        sidInfo.dstPort = 0x20+sw_idx;
#endif
        sidInfo.pri = sid;
        sidInfo.flowId = 0;
        rt_ponmisc_sid_set(sid,sidInfo);
    }
#endif
    return RT_ERR_OK;
}

static int us_pon_sid_mapping_deleting(void)
{
#ifdef CONFIG_COMMON_RT_API
    uint32 sid;
    rt_ponmisc_sidInfo_t sidInfo;

    memset(&sidInfo,0,sizeof(rt_ponmisc_sidInfo_t));

    for(sid=0;sid<MAX_SID;sid++)
    {
        sidInfo.enFlag = DISABLED;
        sidInfo.dqFlag = ENABLED;
        sidInfo.dstPort = 0x24;
        sidInfo.pri = sid;
        sidInfo.flowId = 0;
        rt_ponmisc_sid_set(sid,sidInfo);
    }
#endif
    return RT_ERR_OK;
}
#endif

static int oam_ca_drv_log_read(struct seq_file *seq, void *v)
{
    seq_printf(seq, "oam debug status: %u\n", oam_ca_drv_log);
    return 0;
}

static int oam_ca_drv_log_write(struct file *file, const char __user *buffer, size_t count, loff_t *ppos)
{
    unsigned char tmpBuf[16] = {0};
    int len = (count > 15) ? 15 : count;
    
    if (buffer && !copy_from_user(tmpBuf, buffer, len))
    {
        oam_ca_drv_log = simple_strtoul(tmpBuf, NULL, 16);
        printk("\nwrite oam debug to %u\n", oam_ca_drv_log);
        return count;
    }
    return -EFAULT;
}

static int oam_ca_drv_log_open_proc(struct inode *inode, struct file *file)
{
    return single_open(file, oam_ca_drv_log_read, inode->i_private);
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
static const struct proc_ops oam_ca_drv_log_fop = {
    .proc_open           = oam_ca_drv_log_open_proc,
    .proc_write          = oam_ca_drv_log_write,
    .proc_read           = seq_read,
    .proc_lseek          = seq_lseek,
    .proc_release        = single_release,
};

#else
static const struct file_operations oam_ca_drv_log_fop = {
    .owner          = THIS_MODULE,
    .open           = oam_ca_drv_log_open_proc,
    .write          = oam_ca_drv_log_write,
    .read           = seq_read,
    .llseek         = seq_lseek,
    .release        = single_release,
};
#endif


#if defined(ONE_IMAGE_VENUS) || ((defined(CONFIG_CA8279_SERIES) || defined(CONFIG_CA8277B_SERIES)) && !defined(CONFIG_MULTICHIP_ONEIMAGE))
static int mpcp_stats_read(struct seq_file *seq, void *v)
{
    int ret,i;
    ca_port_id_t port_id;
    ca_epon_mpcp_stats_t mpcp_stats;
    ca_epon_llid_stats_t llid_stats;
    ca_llid_t llid;

    port_id = CA_PORT_ID(CA_PORT_TYPE_EPON,7);

    llid = 0;
    ret = ca_epon_mpcp_stats_get(0,port_id,llid,0,&mpcp_stats);
    if(ret == CA_E_OK)
    {
        seq_printf(seq,"rx_mpcp_frames: %u\n",mpcp_stats.rx_mpcp_frames);         /* Indicates received MPCP packet number from 10G RxMAC*/
        seq_printf(seq,"rx_ext_mpcp_frames: %u\n",mpcp_stats.rx_ext_mpcp_frames);     /* Indicates received extended MPCP packet number from
                                                        10G RxMAC. Refer to table "Deregistration Cause" */
        seq_printf(seq,"rx_discovery_frames_drop: %u\n",mpcp_stats.rx_discovery_frames_drop);/* Indicates dropped discovery packet because of
                                                        discovery information mismatch between OLT and ONU */
        seq_printf(seq,"rx_ext_mpcp_frames_drop: %u\n",mpcp_stats.rx_ext_mpcp_frames_drop);/* Indicates dropped extended MPCP packet because of the overflow of buffer */
        for(i=0;i<CA_MPCGRANT_FRAME_DROP_MAX;i++)
            seq_printf(seq,"mpcp_stats.rx_grant_frames_drops[%d]: %u\n",i,mpcp_stats.rx_grant_frames_drops[i]);   /* Indicates the dropped MPCP grant number. System defined eight kinds of drop reasons */
        seq_printf(seq,"tx_mpcp_frames: %u\n",mpcp_stats.tx_mpcp_frames);         /* Indicates transmitted MPCP packet number to 10G TxMAC or 1G TxMAC */
        seq_printf(seq,"tx_ext_mpcp_frames: %u\n",mpcp_stats.tx_ext_mpcp_frames);     /* Indicates transmitted extended MPCP packet number to 10G TxMAC or 1G TxMAC */
    }

    for(i=0;i<AAL_EPON_LLID_NUM_MAX;i++)
    {
        ret = ca_epon_llid_stats_get(0,port_id,i,0,&llid_stats);
        if(ret == CA_E_OK)
        {
            seq_printf(seq,"\t----llididx %d----: %u\n",i);
            seq_printf(seq,"\trx_good_octets: %u\n",llid_stats.rx_good_octets);     /* Received total good octets       */
            seq_printf(seq,"\trx_uc_frames: %u\n",llid_stats.rx_uc_frames);       /* Received unicast frames          */
            seq_printf(seq,"\trx_mc_frames: %u\n",llid_stats.rx_mc_frames);       /* Received multicast frames        */
            seq_printf(seq,"\trx_bc_frames: %u\n",llid_stats.rx_bc_frames);       /* Received broadcast frames        */
            seq_printf(seq,"\trx_oam_frames: %u\n",llid_stats.rx_oam_frames);      /* Received OAM frames          */
            seq_printf(seq,"\trx_crc8_valid_crc32_bad_frames: %u\n",llid_stats.rx_crc8_valid_crc32_bad_frames);/* The frames with bad CRC32 but correct CRC8 */
            seq_printf(seq,"\ttx_good_octets: %u\n",llid_stats.tx_good_octets);     /* Transmitted total good octets    */
            seq_printf(seq,"\ttx_uc_frames: %u\n",llid_stats.tx_uc_frames);       /* Transmitted unicast frames       */
            seq_printf(seq,"\ttx_mc_frames: %u\n",llid_stats.tx_mc_frames);       /* Transmitted multicast frames     */
            seq_printf(seq,"\ttx_bc_frames: %u\n",llid_stats.tx_bc_frames);       /* Transmitted broadcast frames     */
            seq_printf(seq,"\ttx_oam_frames: %u\n",llid_stats.tx_oam_frames);      /* Transmitted OAM frames           */
        }
    }

    return 0;
}

static int mpcp_stats_open_write(struct file *file, const char __user *buffer, size_t count, loff_t *ppos)
{
    unsigned char tmpBuf[16] = {0};
    int len = (count > 15) ? 15 : count;
    int32_t ret = RT_ERR_OK;
    int i;
    ca_port_id_t port_id;
    ca_epon_mpcp_stats_t mpcp_stats;
    ca_epon_llid_stats_t llid_stats;
    ca_llid_t llid;
    
    if (buffer && !copy_from_user(tmpBuf, buffer, len))
    {
        if(simple_strtoul(tmpBuf, NULL, 16)==1)
        {
            port_id = CA_PORT_ID(CA_PORT_TYPE_EPON,7);

            llid = 0;
            ca_epon_mpcp_stats_get(0,port_id,llid,1,&mpcp_stats);
            for(i=0;i<AAL_EPON_LLID_NUM_MAX;i++)
            {
                ca_epon_llid_stats_get(0,port_id,i,1,&llid_stats);
            }
        }

        return count;
    }
    return -EFAULT;
}

static int mpcp_stats_open_proc(struct inode *inode, struct file *file)
{
    return single_open(file, mpcp_stats_read, inode->i_private);
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
static const struct proc_ops mpcp_stats_fop = {
    .proc_open           = mpcp_stats_open_proc,
    .proc_write          = mpcp_stats_open_write,
    .proc_read           = seq_read,
    .proc_lseek          = seq_lseek,
    .proc_release        = single_release,
};
#else
static const struct file_operations mpcp_stats_fop = {
    .owner          = THIS_MODULE,
    .open           = mpcp_stats_open_proc,
    .write          = mpcp_stats_open_write,
    .read           = seq_read,
    .llseek         = seq_lseek,
    .release        = single_release,
};
#endif

static int port_stats_read(struct seq_file *seq, void *v)
{
    int ret,i;
    ca_port_id_t port_id;
    ca_epon_port_stats_t port_stats;
    ca_epon_port_fec_stats_t fec_stats;

    port_id = CA_PORT_ID(CA_PORT_TYPE_EPON,7);

    ret = ca_epon_port_stats_get(0,port_id,0,&port_stats);
    if(ret == CA_E_OK)
    {
        seq_printf(seq,"rx_crc8_sld_err: %u\n",port_stats.rx_crc8_sld_err);        /* Number of received frames of which SLD or CRC8 is bad*/
        seq_printf(seq,"rx_bc_crc8_valid_fcs_err_frames: %u\n",port_stats.rx_bc_crc8_valid_fcs_err_frames);    /* Number of received broadcast frames of which CRC8 is good but FCS is bad */
        seq_printf(seq,"rx_ext_frames: %u\n",port_stats.rx_ext_frames);          /* Number of received MPCP extension frames.*/
        seq_printf(seq,"rx_good_octets: %u\n",port_stats.rx_good_octets);         /* Number of received octets without error.*/
        seq_printf(seq,"rx_uc_frames: %u\n",port_stats.rx_uc_frames);           /* Number of received unicast frames.*/
        seq_printf(seq,"rx_mc_frames: %u\n",port_stats.rx_mc_frames);           /* Number of received multicast frames. */
        seq_printf(seq,"rx_bc_frames: %u\n",port_stats.rx_bc_frames);           /* Number of received broadcast frames. */
        seq_printf(seq,"rx_fcs_error_frames: %u\n",port_stats.rx_fcs_error_frames);
        seq_printf(seq,"rx_oam_frames: %u\n",port_stats.rx_oam_frames);
        seq_printf(seq,"rx_pause_frames: %u\n",port_stats.rx_pause_frames);
        seq_printf(seq,"rx_unknown_oframes: %u\n",port_stats.rx_unknown_oframes);
        seq_printf(seq,"rx_runt_frames: %u\n",port_stats.rx_runt_frames);
        seq_printf(seq,"rx_oversize_frames: %u\n",port_stats.rx_oversize_frames);
        seq_printf(seq,"rx_64_frames: %u\n",port_stats.rx_64_frames);
        seq_printf(seq,"rx_65_127_frames: %u\n",port_stats.rx_65_127_frames);
        seq_printf(seq,"rx_128_255_frames: %u\n",port_stats.rx_128_255_frames);
        seq_printf(seq,"rx_256_511_frames: %u\n",port_stats.rx_256_511_frames);
        seq_printf(seq,"rx_512_1023_frames: %u\n",port_stats.rx_512_1023_frames);
        seq_printf(seq,"rx_1024_1518_frames: %u\n",port_stats.rx_1024_1518_frames);
        seq_printf(seq,"rx_1519_max_frames: %u\n",port_stats.rx_1519_max_frames);      /* frame size is equal to greater than 1519 bytes */
        seq_printf(seq,"tx_ext_frames: %u\n",port_stats.tx_ext_frames);
        seq_printf(seq,"tx_good_octets: %u\n",port_stats.tx_good_octets);
        seq_printf(seq,"tx_uc_frames: %u\n",port_stats.tx_uc_frames);
        seq_printf(seq,"tx_mc_frames: %u\n",port_stats.tx_mc_frames);
        seq_printf(seq,"tx_bc_frames: %u\n",port_stats.tx_bc_frames);
        seq_printf(seq,"tx_oam_frames: %u\n",port_stats.tx_oam_frames);
        seq_printf(seq,"tx_pause_frames: %u\n",port_stats.tx_pause_frames);
        seq_printf(seq,"tx_64_frames: %u\n",port_stats.tx_64_frames);
        seq_printf(seq,"tx_65_127_frames: %u\n",port_stats.tx_65_127_frames);
        seq_printf(seq,"tx_128_255_frames: %u\n",port_stats.tx_128_255_frames);
        seq_printf(seq,"tx_256_511_frames: %u\n",port_stats.tx_256_511_frames);
        seq_printf(seq,"tx_512_1023_frames: %u\n",port_stats.tx_512_1023_frames);
        seq_printf(seq,"tx_1024_1518_frames: %u\n",port_stats.tx_1024_1518_frames);
        seq_printf(seq,"tx_1519_max_frames: %u\n",port_stats.tx_1519_max_frames);      /* frame size is equal to or greater than 1519 bytes */
        seq_printf(seq,"mpcp_rx_bc_reg: %u\n",port_stats.mpcp_rx_bc_reg);         /* Number of received broadcast MPCP registration frames */
        seq_printf(seq,"mpcp_rx_bc_gate: %u\n",port_stats.mpcp_rx_bc_gate);        /* Number of received broadcast MPCP discovery gate frames. */
        seq_printf(seq,"mpcp_tx_mac_ctrl_frames: %u\n",port_stats.mpcp_tx_mac_ctrl_frames);    
        seq_printf(seq,"mpcp_tx_bc_reg_req: %u\n",port_stats.mpcp_tx_bc_reg_req);
        seq_printf(seq,"mpcp_tx_reg_ack: %u\n",port_stats.mpcp_tx_reg_ack);
        seq_printf(seq,"mpcp_rx_mac_ctrl_frames: %u\n",port_stats.mpcp_rx_mac_ctrl_frames);
    }

    ret = ca_epon_port_fec_stats_get(0,port_id,0,&fec_stats);
    if(ret == CA_E_OK)
    {
        seq_printf(seq,"corrected_bytes: %u\n",fec_stats.corrected_bytes);
        seq_printf(seq,"corrected_codewords: %u\n",fec_stats.corrected_codewords);
        seq_printf(seq,"uncorrectable_codewords: %u\n",fec_stats.uncorrectable_codewords);
        seq_printf(seq,"total_codewords: %u\n",fec_stats.total_codewords);
    }

    return 0;
}

static int port_stats_open_write(struct file *file, const char __user *buffer, size_t count, loff_t *ppos)
{
    unsigned char tmpBuf[16] = {0};
    int len = (count > 15) ? 15 : count;
    int32_t ret = RT_ERR_OK;
    ca_port_id_t port_id;
    ca_epon_port_stats_t port_stats;
    ca_epon_port_fec_stats_t fec_stats;
    
    if (buffer && !copy_from_user(tmpBuf, buffer, len))
    {
        if(simple_strtoul(tmpBuf, NULL, 16)==1)
        {
            port_id = CA_PORT_ID(CA_PORT_TYPE_EPON,7);

            ca_epon_port_stats_get(0,port_id,1,&port_stats);
            ca_epon_port_fec_stats_get(0,port_id,1,&fec_stats);
        }

        return count;
    }
    return -EFAULT;
}

static int port_stats_open_proc(struct inode *inode, struct file *file)
{
    return single_open(file, port_stats_read, inode->i_private);
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
static const struct proc_ops port_stats_fop = {
    .proc_open           = port_stats_open_proc,
    .proc_write          = port_stats_open_write,
    .proc_read           = seq_read,
    .proc_lseek          = seq_lseek,
    .proc_release        = single_release,
};
#else
static const struct file_operations port_stats_fop = {
    .owner          = THIS_MODULE,
    .open           = port_stats_open_proc,
    .write          = port_stats_open_write,
    .read           = seq_read,
    .llseek         = seq_lseek,
    .release        = single_release,
};
#endif

#endif

#ifdef CONFIG_CA8279_SERIES
static int us_pon_rule_read(struct seq_file *seq, void *v)
{
    int i;

    for(i=0;i<MAX_QUEUE;i++)
    {
        if(gbl_us_pon_cls_index[i]!=-1)
        {
            seq_printf(seq,"Saturn gbl_us_pon_cls_index[%d] = %d\n",i,gbl_us_pon_cls_index[i]);
        }
    }
    return 0;
}

static int us_pon_rule_write(struct file *file, const char __user *buffer, size_t count, loff_t *ppos)
{
    unsigned char tmpBuf[16] = {0};
    int len = (count > 15) ? 15 : count;
    int32_t ret = RT_ERR_OK;
    
    if (buffer && !copy_from_user(tmpBuf, buffer, len))
    {
        if(simple_strtoul(tmpBuf, NULL, 16)==1)
        {
            saturn_us_pon_rule_adding();
        }
        else
        {
            saturn_us_pon_rule_deleting();
        }

        return count;
    }
    return -EFAULT;
}

static int us_pon_rule_open_proc(struct inode *inode, struct file *file)
{
    return single_open(file, us_pon_rule_read, inode->i_private);
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
static const struct proc_ops us_pon_rule_fop = {
    .proc_open           = us_pon_rule_open_proc,
    .proc_write          = us_pon_rule_write,
    .proc_read           = seq_read,
    .proc_lseek          = seq_lseek,
    .proc_release        = single_release,
};
#else
static const struct file_operations us_pon_rule_fop = {
    .owner          = THIS_MODULE,
    .open           = us_pon_rule_open_proc,
    .write          = us_pon_rule_write,
    .read           = seq_read,
    .llseek         = seq_lseek,
    .release        = single_release,
};
#endif
#endif

static int oam_static_l2_addr_read(struct seq_file *seq, void *v)
{
    seq_printf(seq,"Saturn oam_static_l2_addr_setting = %d\n",oam_static_l2_addr_setting);
    return 0;
}

static int oam_static_l2_addr_write(struct file *file, const char __user *buffer, size_t count, loff_t *ppos)
{
    unsigned char tmpBuf[16] = {0};
    int len = (count > 15) ? 15 : count;
    int32_t ret = RT_ERR_OK;
    
    if (buffer && !copy_from_user(tmpBuf, buffer, len))
    {
        if(simple_strtoul(tmpBuf, NULL, 16)==1)
        {
            oam_static_l2_addr_add();
        }
        else
        {
            oam_static_l2_addr_del();
        }

        return count;
    }
    return -EFAULT;
}

static int oam_static_l2_addr_open_proc(struct inode *inode, struct file *file)
{
    return single_open(file, oam_static_l2_addr_read, inode->i_private);
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
static const struct proc_ops oam_static_l2_addr_fop = {
    .proc_open           = oam_static_l2_addr_open_proc,
    .proc_write          = oam_static_l2_addr_write,
    .proc_read           = seq_read,
    .proc_lseek          = seq_lseek,
    .proc_release        = single_release,
};
#else
static const struct file_operations oam_static_l2_addr_fop = {
    .owner          = THIS_MODULE,
    .open           = oam_static_l2_addr_open_proc,
    .write          = oam_static_l2_addr_write,
    .read           = seq_read,
    .llseek         = seq_lseek,
    .release        = single_release,
};
#endif

#if defined(ONE_IMAGE_VENUS) || ((defined(CONFIG_CA8279_SERIES) || defined(CONFIG_CA8277B_SERIES)) && !defined(CONFIG_MULTICHIP_ONEIMAGE))
static int mpcp_reg_start_read(struct seq_file *seq, void *v)
{
    int32_t ret = RT_ERR_OK;
    ca_boolean_t status = FALSE;

    ret = ca_epon_mpcp_registration_get(0, 0x20007, 0, &status);

    seq_printf(seq,"status=%d ret=%d\n",status,ret);
    return 0;
}

static int mpcp_reg_start_write(struct file *file, const char __user *buffer, size_t count, loff_t *ppos)
{
    unsigned char tmpBuf[16] = {0};
    int len = (count > 15) ? 15 : count;
    int32_t ret = RT_ERR_OK;
    
    if (buffer && !copy_from_user(tmpBuf, buffer, len))
    {
        if(simple_strtoul(tmpBuf, NULL, 16)==1)
        {
            printk("Trigger MPCP Registration !!\n");
            if((ret = ca_epon_mpcp_registration_set(0, 0x20007, 0, 1))!=CA_E_OK)
            {
                printk("Trigger MPCP Registration ret=%d!!\n",ret);
            }
        }
        else
        {
            printk("Trigger MPCP De-registration !!\n");
            if((ret = ca_epon_mpcp_registration_set(0, 0x20007, 0, 0))!=CA_E_OK)
            {
                printk("Trigger MPCP De-registration ret=%d!!\n",ret);
            }
        }

        return count;
    }
    return -EFAULT;
}

static int mpcp_reg_start_open_proc(struct inode *inode, struct file *file)
{
    return single_open(file, mpcp_reg_start_read, inode->i_private);
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
static const struct proc_ops mpcp_reg_start_fop = {
    .proc_open           = mpcp_reg_start_open_proc,
    .proc_write          = mpcp_reg_start_write,
    .proc_read           = seq_read,
    .proc_lseek          = seq_lseek,
    .proc_release        = single_release,
};
#else
static const struct file_operations mpcp_reg_start_fop = {
    .owner          = THIS_MODULE,
    .open           = mpcp_reg_start_open_proc,
    .write          = mpcp_reg_start_write,
    .read           = seq_read,
    .llseek         = seq_lseek,
    .release        = single_release,
};
#endif

#endif

static int pon_mac_reset_read(struct seq_file *seq, void *v)
{
    ca_boolean_t status = FALSE;


    seq_printf(seq,"using echo 1 to reset\n",status);
    return 0;
}

static int pon_mac_reset_write(struct file *file, const char __user *buffer, size_t count, loff_t *ppos)
{
    unsigned char tmpBuf[16] = {0};
    int len = (count > 15) ? 15 : count;
    int32_t ret = RT_ERR_OK;
    ca_uint32_t pon_mode;
    
    if (buffer && !copy_from_user(tmpBuf, buffer, len))
    {
        if(simple_strtoul(tmpBuf, NULL, 16)==1)
        {
            pon_mode = aal_pon_mac_mode_get(0);

            if((ret = aal_psds_init(0, pon_mode))!=CA_E_OK)
            {
                printk("aal_psds_init ret=%d!!\n",ret);
            }
            
            if((ret = aal_epon_init(0, 0,pon_mode))!=CA_E_OK)
            {
                printk("aal_epon_init ret=%d!!\n",ret);
            }
        }
        return count;
    }
    return -EFAULT;
}

static int pon_mac_reset_open_proc(struct inode *inode, struct file *file)
{
    return single_open(file, pon_mac_reset_read, inode->i_private);
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
static const struct proc_ops pon_mac_reset_fop = {
    .proc_open           = pon_mac_reset_open_proc,
    .proc_write          = pon_mac_reset_write,
    .proc_read           = seq_read,
    .proc_lseek          = seq_lseek,
    .proc_release        = single_release,
};
#else
static const struct file_operations pon_mac_reset_fop = {
    .owner          = THIS_MODULE,
    .open           = pon_mac_reset_open_proc,
    .write          = pon_mac_reset_write,
    .read           = seq_read,
    .llseek         = seq_lseek,
    .release        = single_release,
};
#endif

static int laser_on_time_read(struct seq_file *seq, void *v)
{
    int32_t ret = RT_ERR_OK;
    aal_mpcp_cfg_t cfg;

    if((ret = aal_mpcp_cfg_get(0, 0, &cfg))!=CA_E_OK)
    {
        seq_printf(seq,"Get Laser on off time Error ret=%d!!\n",ret);
        return 1;
    }

    seq_printf(seq,"laser_on_time=%d\n",cfg.laser_on_time);
    return 0;
}

static int laser_on_time_write(struct file *file, const char __user *buffer, size_t count, loff_t *ppos)
{
    unsigned char tmpBuf[16] = {0};
    int len = (count > 15) ? 15 : count;
    int32_t ret = RT_ERR_OK;
    aal_mpcp_msk_t msk;
    aal_mpcp_cfg_t cfg;
    
    if (buffer && !copy_from_user(tmpBuf, buffer, len))
    {
        if((ret = aal_mpcp_cfg_get(0, 0, &cfg))!=CA_E_OK)
        {
            printk("Get Laser on off time Error!!\n");
            return -EFAULT;
        }

        msk.s.laser_on_time = 1;
        cfg.laser_on_time = simple_strtoul(tmpBuf, NULL, 10);

        if((ret = aal_mpcp_cfg_set(0, 0, msk, &cfg))!=CA_E_OK)
        {
            printk("Set Laser on off time Error!!\n");
            return -EFAULT;
        }

        return count;
    }
    return -EFAULT;
}

static int laser_on_time_open_proc(struct inode *inode, struct file *file)
{
    return single_open(file, laser_on_time_read, inode->i_private);
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
static const struct proc_ops laser_on_time_fop = {
    .proc_open           = laser_on_time_open_proc,
    .proc_write          = laser_on_time_write,
    .proc_read           = seq_read,
    .proc_lseek          = seq_lseek,
    .proc_release        = single_release,
};
#else
static const struct file_operations laser_on_time_fop = {
    .owner          = THIS_MODULE,
    .open           = laser_on_time_open_proc,
    .write          = laser_on_time_write,
    .read           = seq_read,
    .llseek         = seq_lseek,
    .release        = single_release,
};
#endif

static int laser_off_time_read(struct seq_file *seq, void *v)
{
    int32_t ret = RT_ERR_OK;
    aal_mpcp_cfg_t cfg;

    if((ret = aal_mpcp_cfg_get(0, 0, &cfg))!=CA_E_OK)
    {
        seq_printf(seq,"Get Laser on off time Error ret=%d!!\n",ret);
        return 1;
    }

    seq_printf(seq,"laser_off_time=%d\n",cfg.laser_off_time);
    return 0;
}

static int laser_off_time_write(struct file *file, const char __user *buffer, size_t count, loff_t *ppos)
{
    unsigned char tmpBuf[16] = {0};
    int len = (count > 15) ? 15 : count;
    int32_t ret = RT_ERR_OK;
    aal_mpcp_msk_t msk;
    aal_mpcp_cfg_t cfg;
    
    if (buffer && !copy_from_user(tmpBuf, buffer, len))
    {
        if((ret = aal_mpcp_cfg_get(0, 0, &cfg))!=CA_E_OK)
        {
            printk("Get Laser on off time Error!!\n");
            return -EFAULT;
        }

        msk.s.laser_off_time = 1;
        cfg.laser_off_time = simple_strtoul(tmpBuf, NULL, 10);

        if((ret = aal_mpcp_cfg_set(0, 0, msk, &cfg))!=CA_E_OK)
        {
            printk("Set Laser on off time Error!!\n");
            return -EFAULT;
        }

        return count;
    }
    return -EFAULT;
}

static int laser_off_time_open_proc(struct inode *inode, struct file *file)
{
    return single_open(file, laser_off_time_read, inode->i_private);
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
static const struct proc_ops laser_off_time_fop = {
    .proc_open           = laser_off_time_open_proc,
    .proc_write          = laser_off_time_write,
    .proc_read           = seq_read,
    .proc_lseek          = seq_lseek,
    .proc_release        = single_release,
};
#else
static const struct file_operations laser_off_time_fop = {
    .owner          = THIS_MODULE,
    .open           = laser_off_time_open_proc,
    .write          = laser_off_time_write,
    .read           = seq_read,
    .llseek         = seq_lseek,
    .release        = single_release,
};
#endif

static int ca_mpcp_gate_ana_open(struct inode *inode, struct file *file)
{
        return single_open(file, ca_mpcp_gate_ana_read, inode->i_private);
}

int mpcp_pkt_rx(struct napi_struct *napi,struct net_device *dev, struct sk_buff *skb, nic_hook_private_t *nh_priv)
{
    if(ca_rtk_epon_mpcp_pkt_rx(skb) == RE8670_RX_STOP)
        return RE8670_RX_STOP;

    return RE8670_RX_CONTINUE;
}

static int ca_mpcp_gate_ana_write(struct file *file, const char __user *buffer, size_t count, loff_t *ppos)
{
	unsigned char tmpBuf[16] = {0};
	int len = (count > 15) ? 15 : count;


	if (buffer && !copy_from_user(tmpBuf, buffer, len))
	{
		ca_gate_ana_flag=simple_strtoul(tmpBuf, NULL, 16);

    	printk("[ca_gate_ana_flag = 0x%08x]\n", ca_gate_ana_flag);
    	printk("CA_EPON_MPCP_ANA_FLAG_START  \t0x%08x\n", (unsigned int)CA_EPON_MPCP_ANA_FLAG_START);
    	printk("CA_EPON_MPCP_ANA_FLAG_STOP \t0x%08x\n", (unsigned int)CA_EPON_MPCP_ANA_FLAG_STOP);
    	printk("CA_EPON_MPCP_ANA_FLAG_ENABLE \t0x%08x\n", (unsigned int)CA_EPON_MPCP_ANA_FLAG_ENABLE);
    	printk("CA_EPON_MPCP_LOG_DUMP \t0x%08x\n", (unsigned int)CA_EPON_MPCP_LOG_DUMP);
    	printk("CA_EPON_MPCP_LOG_DUMP_CLR \t0x%08x\n", (unsigned int)CA_EPON_MPCP_LOG_DUMP_CLR);
    	printk("CA_EPON_MPCP_LOG_DUMP_OVERWRITE \t0x%08x\n", (unsigned int)CA_EPON_MPCP_LOG_DUMP_OVERWRITE);

        
    	printk("ca_rtk_epon_oam_pkt_rx_cnt %d\n", ca_rtk_epon_oam_pkt_rx_cnt);

        if( ca_gate_ana_flag & CA_EPON_MPCP_ANA_FLAG_ENABLE)
        {/*start trap mpcp to CPU*/
            rtk_port_t pon;
            int ponPortMask;

            pon = HAL_GET_PON_PORT();
            ponPortMask = 1 << pon;
#if defined(ONE_IMAGE_77C) || (!defined(CONFIG_CA8279_SERIES) && !defined(CONFIG_CA8277B_SERIES) && !defined(CONFIG_MULTICHIP_ONEIMAGE))
            if(nic_register_rxhook(0xffffffff,7,&mpcp_pkt_rx)!=RT_ERR_OK)
            {
                printk("MPCP nic_register_rxhook Error!!\n");
            }
#else
            if(drv_nic_register_rxhook(ponPortMask,7,&mpcp_pkt_rx)!=RT_ERR_OK)
            {
                printk("MPCP drv_nic_register_rxhook Error!!\n");
            }
#endif
        }
        else
        {/*disable mpcp trap*/
            rtk_port_t pon;
            int ponPortMask;

            pon = HAL_GET_PON_PORT();
            ponPortMask = 1 << pon;
#if defined(ONE_IMAGE_77C) || (!defined(CONFIG_CA8279_SERIES) && !defined(CONFIG_CA8277B_SERIES) && !defined(CONFIG_MULTICHIP_ONEIMAGE))
            if(nic_unregister_rxhook(0xffffffff,7,&mpcp_pkt_rx)!=RT_ERR_OK)
            {
                printk("MPCP nic_unregister_rxhook Error!!\n");
            }
#else
            if(drv_nic_unregister_rxhook(ponPortMask,1,&mpcp_pkt_rx)!=RT_ERR_OK)
            {
                printk("MPCP drv_nic_unregister_rxhook Error!!\n");
            }
#endif
        }        
		
        if( ca_gate_ana_flag & CA_EPON_MPCP_LOG_DUMP)
        {/*dump mpcp event log*/		
			rt_enable_t clr,overwrite;
			if( ca_gate_ana_flag & CA_EPON_MPCP_LOG_DUMP_CLR)
				clr=ENABLED;
			else
				clr=DISABLED;	
			if( ca_gate_ana_flag & CA_EPON_MPCP_LOG_DUMP_OVERWRITE)
				overwrite=ENABLED;
			else
				overwrite=DISABLED;

            if(rt_epon_mpcp_dbg_log_dump(overwrite,clr)!=RT_ERR_OK)
            {
                printk("rt_epon_mcpc_dbg_log_dump Error!!\n");
            }
			
		}
		
		return count;
	}
	return -EFAULT;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
static const struct proc_ops ca_mpcp_gate_ana_fops = {
    .proc_open           = ca_mpcp_gate_ana_open,
    .proc_write          = ca_mpcp_gate_ana_write,
    .proc_read           = seq_read,
    .proc_lseek          = seq_lseek,
    .proc_release        = single_release,
};
#else
static const struct file_operations ca_mpcp_gate_ana_fops = {
        .owner          = THIS_MODULE,
        .open           = ca_mpcp_gate_ana_open,
        .read           = seq_read,
        .write          = ca_mpcp_gate_ana_write,
        .llseek         = seq_lseek,
        .release        = single_release,
};
#endif

int __init ca_rtk_epon_drv_init(void)
{
    int32_t ret = RT_ERR_OK;
    rtk_port_t pon;
    int ponPortMask,i;
    unsigned char voqMapMode;
	aal_mpcp_msk_t msk;
    aal_mpcp_cfg_t cfg;

    ret = scfg_read(0, CFG_ID_PON_VOQ_MODE, sizeof(voqMapMode), &voqMapMode);
    if (CA_E_OK != ret) {
        printk("<%s, %d> read CFG_ID_PON_VOQ_MODE fail\r\n", __func__, __LINE__);
    } else if (AAL_PUC_VOQ_MODE_8Q != voqMapMode){
        printk("<%s, %d> Fail! CFG_ID_PON_VOQ_MODE at /config/scfg.txt should be 0 \r\n", __func__, __LINE__);
    }

    ret = scfg_read(0, CFG_ID_PON_VOQ_MODE, sizeof(voqMapMode), &voqMapMode);

    for(i=0;i<MAX_QUEUE;i++)
    {
        gbl_us_pon_cls_index[i] = -1;
    }

    if (NULL == ca_rtk_epon_proc_dir) {
        ca_rtk_epon_proc_dir = proc_mkdir ("ca_rtk_epon", NULL);
    }

    if(ca_rtk_epon_proc_dir)
    {
        oam_ca_drv_log_entry = proc_create("oam_ca_drv_log", 0644, ca_rtk_epon_proc_dir, &oam_ca_drv_log_fop);
        if (!oam_ca_drv_log_entry) {
            printk("oam_ca_drv_log_entry, create proc failed!");
        }

#if defined(ONE_IMAGE_VENUS) || ((defined(CONFIG_CA8279_SERIES) || defined(CONFIG_CA8277B_SERIES)) && !defined(CONFIG_MULTICHIP_ONEIMAGE))
        mpcp_stats_entry = proc_create("mpcp_stats", 0644, ca_rtk_epon_proc_dir, &mpcp_stats_fop);
        if (!mpcp_stats_entry) {
            printk("mpcp_stats_entry, create proc failed!");
        }

        port_stats_entry = proc_create("port_stats", 0644, ca_rtk_epon_proc_dir, &port_stats_fop);
        if (!port_stats_entry) {
            printk("port_stats_entry, create proc failed!");
        }
#endif

#ifdef CONFIG_CA8279_SERIES
        us_pon_rule_entry = proc_create("us_pon_rule", 0644, ca_rtk_epon_proc_dir, &us_pon_rule_fop);
        if (!us_pon_rule_entry) {
            printk("us_pon_rule_entry, create proc failed!");
        }
#endif
        oam_static_l2_addr_entry = proc_create("oam_static_l2_addr", 0644, ca_rtk_epon_proc_dir, &oam_static_l2_addr_fop);
        if (!oam_static_l2_addr_entry) {
            printk("oam_static_l2_addr_entry, create proc failed!");
        }

#if defined(ONE_IMAGE_VENUS) || ((defined(CONFIG_CA8279_SERIES) || defined(CONFIG_CA8277B_SERIES)) && !defined(CONFIG_MULTICHIP_ONEIMAGE))
        mpcp_reg_start_entry = proc_create("mpcp_reg_start", 0644, ca_rtk_epon_proc_dir, &mpcp_reg_start_fop);
        if (!mpcp_reg_start_entry) {
            printk("mpcp_reg_start_entry, create proc failed!");
        }
#endif
        pon_mac_reset_entry = proc_create("pon_mac_reset", 0644, ca_rtk_epon_proc_dir, &pon_mac_reset_fop);
        if (!pon_mac_reset_entry) {
            printk("pon_mac_reset_entry, create proc failed!");
        }
		
		laser_on_time_entry = proc_create("laser_on_time", 0644, ca_rtk_epon_proc_dir, &laser_on_time_fop);
        if (!laser_on_time_entry) {
            printk("laser_on_time_entry, create proc failed!");
        }

        laser_off_time_entry = proc_create("laser_off_time", 0644, ca_rtk_epon_proc_dir, &laser_off_time_fop);
        if (!laser_off_time_entry) {
            printk("laser_off_time_entry, create proc failed!");
        }

        ca_gate_ana_entry = proc_create("gate_ana", 0644, ca_rtk_epon_proc_dir, &ca_mpcp_gate_ana_fops);
        if (!ca_gate_ana_entry) {
            printk("ca_gate_ana_entry, create proc failed!");
            
        }

    }

    pon = HAL_GET_PON_PORT();
    ponPortMask = 1 << pon;
#ifdef SUPPORT_OAM_KEEPLIVE
	epon_oam_keeplive_init();
	if((ret = pkt_redirect_kernelApp_reg(PR_KERNEL_UID_GMAC, epon_oam_pkt_tx_for_keeplive))!=RT_ERR_OK)
#else
    if((ret = pkt_redirect_kernelApp_reg(PR_KERNEL_UID_GMAC, ca_rtk_epon_oam_pkt_tx))!=RT_ERR_OK)
#endif
    {
        printk("OAM PR_KERNEL_UID_GMAC pkt_redirect_kernelApp_reg Error!!\n");
    }

    if((ret = pkt_redirect_kernelApp_reg(PR_KERNEL_UID_EPONDYINGGASP, ca_rtk_epon_oam_dyingGasp_tx))!=RT_ERR_OK)
    {
        printk("OAM PR_KERNEL_UID_EPONDYINGGASP pkt_redirect_kernelApp_reg Error!!\n");
    }

#ifdef CONFIG_COMMON_RT_API
    if((ret = rt_epon_init())!=RT_ERR_OK)
    {
        printk("rt_epon_init Error!!\n");
    }

    if((ret = rt_epon_oam_rx_callback_register(rt_oam_rx))!=RT_ERR_OK)
    {
        printk("rt_epon_oam_rx_callback_register Error!!\n");
    }
#else
    if((ret = drv_nic_register_rxhook(ponPortMask,7,&ca_rtk_epon_oam_pkt_rx))!=RT_ERR_OK)
    {
        printk("OAM drv_nic_register_rxhook Error!!\n");
    }
#endif

    if((ret = ca_event_register(0,CA_EVENT_EPON_REG_STAT_CHNG,ca_rtk_epon_reg_intr,NULL))!=CA_E_OK)
    {
        printk("OAM event epon stat changing event register Error!!\n");
    }

    if((ret = ca_event_register(0,CA_EVENT_EPON_PORT_LINK,ca_rtk_epon_port_intr,NULL))!=CA_E_OK)
    {
        printk("OAM event epon port link register Error!!\n");
    }

    if((ret = ca_event_register(0,CA_EVENT_EPON_SILENCE_STATUS,ca_rtk_epon_silence_intr,NULL))!=CA_E_OK)
    {
        printk("OAM event epon silence status register Error!!\n");
    }

    /* increase lsr-on and lsr-off time for ZTE OLT*/
    if((ret = aal_mpcp_cfg_get(0, 0, &cfg))!=CA_E_OK)
    {
        printk("Get Laser on off time Error!!\n");
    }

    msk.s.laser_on_time = 1;
    msk.s.laser_off_time = 1;
    cfg.laser_on_time = 0x20;
    cfg.laser_off_time = 0x20;

    if((ret = aal_mpcp_cfg_set(0, 0, msk, &cfg))!=CA_E_OK)
    {
        printk("Set Laser on off time Error!!\n");
    }
	
    return 0;
}

void __exit ca_rtk_epon_drv_exit(void)
{
    int32_t ret = RT_ERR_OK;
    rtk_port_t pon;
    int ponPortMask;

    pon = HAL_GET_PON_PORT();
    ponPortMask = 1 << pon;

    if((ret = ca_event_deregister(0,CA_EVENT_EPON_REG_STAT_CHNG,ca_rtk_epon_reg_intr))!=CA_E_OK)
    {
        printk("OAM event epon stat changing event deregister Error!!\n");
        return ;
    }

    if((ret = ca_event_deregister(0,CA_EVENT_EPON_PORT_LINK,ca_rtk_epon_port_intr))!=CA_E_OK)
    {
        printk("OAM event epon port link deregister Error!!\n");
        return ;
    }

    if((ret = ca_event_deregister(0,CA_EVENT_EPON_SILENCE_STATUS,ca_rtk_epon_silence_intr))!=CA_E_OK)
    {
        printk("OAM event epon silence status deregister Error!!\n");
        return ;
    }

#ifndef CONFIG_COMMON_RT_API
    if((ret = drv_nic_unregister_rxhook(ponPortMask,1,&ca_rtk_epon_oam_pkt_rx))!=RT_ERR_OK)
    {
        printk("OAM drv_nic_unregister_rxhook Error!!\n");
    }
#endif

    if((ret = pkt_redirect_kernelApp_dereg(PR_KERNEL_UID_GMAC))!=RT_ERR_OK)
    {
        printk("OAM PR_KERNEL_UID_GMAC pkt_redirect_kernelApp_dereg Error!!\n");
    }

    if((ret = pkt_redirect_kernelApp_dereg(PR_KERNEL_UID_EPONDYINGGASP))!=RT_ERR_OK)
    {
        printk("OAM PR_KERNEL_UID_EPONDYINGGASP pkt_redirect_kernelApp_dereg Error!!\n");
    }

    if(oam_ca_drv_log_entry)
    {
        remove_proc_entry("oam_ca_drv_log", ca_rtk_epon_proc_dir);
        oam_ca_drv_log_entry = NULL;
    }

#if defined(ONE_IMAGE_VENUS) || ((defined(CONFIG_CA8279_SERIES) || defined(CONFIG_CA8277B_SERIES)) && !defined(CONFIG_MULTICHIP_ONEIMAGE))
    if(mpcp_stats_entry)
    {
        remove_proc_entry("mpcp_stats", ca_rtk_epon_proc_dir);
        mpcp_stats_entry = NULL;
    }

    if(port_stats_entry)
    {
        remove_proc_entry("port_stats", ca_rtk_epon_proc_dir);
        port_stats_entry = NULL;
    }
#endif
    if(us_pon_rule_entry)
    {
        remove_proc_entry("us_pon_rule_rule", ca_rtk_epon_proc_dir);
        us_pon_rule_entry = NULL;
    }

    if(oam_static_l2_addr_entry)
    {
        remove_proc_entry("oam_static_l2_addr", ca_rtk_epon_proc_dir);
        oam_static_l2_addr_entry = NULL;
    }

#if defined(ONE_IMAGE_VENUS) || ((defined(CONFIG_CA8279_SERIES) || defined(CONFIG_CA8277B_SERIES)) && !defined(CONFIG_MULTICHIP_ONEIMAGE))
    if(mpcp_reg_start_entry)
    {
        remove_proc_entry("mpcp_reg_start", ca_rtk_epon_proc_dir);
        mpcp_reg_start_entry = NULL;
    }
#endif

    if(pon_mac_reset_entry)
    {
        remove_proc_entry("pon_mac_reset", ca_rtk_epon_proc_dir);
        pon_mac_reset_entry = NULL;
    }

    if(laser_on_time_entry)
    {
        remove_proc_entry("laser_on_time", ca_rtk_epon_proc_dir);
        laser_on_time_entry = NULL;
    }

    if(laser_off_time_entry)
    {
        remove_proc_entry("laser_off_time", ca_rtk_epon_proc_dir);
        laser_off_time_entry = NULL;
    }
	
    if(ca_gate_ana_entry)
    {
        remove_proc_entry("gate_ana", ca_rtk_epon_proc_dir);
        ca_gate_ana_entry = NULL;
    }
	
    if(ca_rtk_epon_proc_dir)
    {
        proc_remove(ca_rtk_epon_proc_dir);
        ca_rtk_epon_proc_dir = NULL;
    }
}


MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("CA RealTek EPON driver module");
MODULE_AUTHOR("Realtek");
module_init(ca_rtk_epon_drv_init);
module_exit(ca_rtk_epon_drv_exit);
EXPORT_SYMBOL(oam_ca_drv_log);
module_param(oam_ca_drv_log,int,0644);
EXPORT_SYMBOL(s_oam_register);
EXPORT_SYMBOL(s_oam_unregister);
#endif
