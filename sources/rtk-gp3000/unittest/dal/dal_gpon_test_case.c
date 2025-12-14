/*
 * Copyright(c) Realtek Semiconductor Corporation, 2011
 * All rights reserved.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Definition of HAL API test APIs in the SDK
 *
 * Feature : HAL API test APIs
 *
 */

/*
 * Include Files
 */
#include <osal/lib.h>
#include <common/debug/rt_log.h>
#include <module/gpon/gpon_defs.h>
//#include <dal/apollo/gpon/gpon_debug.h>
#include <rtk/gpon.h>
#include <dal/dal_gpon_test_case.h>

/*
 * Function Declaration
 */
int32 dal_gpon_sn_test(uint32 testcase)
{
    rtk_gpon_serialNumber_t sn, tmp_sn;
    rtk_gpon_password_t     pwd, tmp_pwd;
    int32 ret;

    /* Serial number Test */
    osal_memcpy(sn.vendor, "TEST", RTK_GPON_VENDOR_ID_LEN);
    osal_memset(sn.specific, 0xAA, RTK_GPON_VENDOR_SPECIFIC_LEN);
    if((ret = rtk_gpon_serialNumber_set(&sn)) != RT_ERR_OK)
    {
        osal_printf("rtk_gpon_serialNumber_set fail:0x%x\n\r",ret);
        return RT_ERR_FAILED;
    }
    if((ret = rtk_gpon_serialNumber_get(&tmp_sn)) != RT_ERR_OK)
    {
        osal_printf("rtk_gpon_serialNumber_get fail:0x%x\n\r",ret);
        return RT_ERR_FAILED;
    }
    if(osal_memcmp(&sn, &tmp_sn, sizeof(rtk_gpon_serialNumber_t)) != 0)
    {
        osal_printf("osal_memcmp SN fail:sn %s-%x%x%x%x, tmp_sn: %s-%x%x%x%x\n\r",
               sn.vendor,sn.specific[0],sn.specific[1],sn.specific[2],sn.specific[3],
               tmp_sn.vendor,tmp_sn.specific[0],tmp_sn.specific[1],tmp_sn.specific[2],tmp_sn.specific[3]);
        return RT_ERR_FAILED;
    }

    /* Serial number Test */
    osal_memset(pwd.password, 0x55, RTK_GPON_PASSWORD_LEN);
    if(rtk_gpon_password_set(&pwd) != RT_ERR_OK)
        return RT_ERR_FAILED;
    if(rtk_gpon_password_get(&tmp_pwd) != RT_ERR_OK)
        return RT_ERR_FAILED;
    if(osal_memcmp(&pwd, &tmp_pwd, sizeof(rtk_gpon_password_t)) != 0)
        return RT_ERR_FAILED;

    return RT_ERR_OK;
}
typedef int32 (*reg_get_func_t)();

static int32 para_compare(uint32 type, reg_get_func_t reg_get_func,
                          void *para1, void *para2, uint32 len)
{
    int32 ret;
    int32 i;

    if((ret = rtk_gpon_parameter_set(type, para1)) != RT_ERR_OK)
    {
        osal_printf("para compare[%d] set fail:0x%x\n\r",type,ret);
        return RT_ERR_FAILED;
    }
    osal_memset(para2, 0x0, len);
    if((ret = rtk_gpon_parameter_get(type,para2)) != RT_ERR_OK)
    {
        osal_printf("db function[%d] get fail:0x%x\n\r",type,ret);
        return RT_ERR_FAILED;
    }
    if(osal_memcmp(para1, para2, len) != 0)
    {
        osal_printf("db[%d] compare fail len=%d\n\r",type,len);
        osal_printf("para1:[");
        for(i=0;i<len;i++)
        {
            osal_printf("%x,",*(((uint8*)para1)+i));
        }
        osal_printf("]\n\r");
        osal_printf("para2:[");
        for(i=0;i<len;i++)
        {
            osal_printf("%x,",*(((uint8*)para2)+i));
        }
        osal_printf("]\n\r");
        return RT_ERR_FAILED;
    }
    if(reg_get_func != NULL)
    {
        osal_memset(para2, 0x0, len);
        if((ret = reg_get_func(para2))!=RT_ERR_OK)
        {
            osal_printf("register function[%d] get fail:0x%x\n\r",type,ret);
            return RT_ERR_FAILED;
        }
        if(osal_memcmp(para1, para2, len) != 0)
        {
            osal_printf("register[%d] compare fail\n\r",type);
            osal_printf("para1[");
            for(i=0;i<len;i++)
            {
                osal_printf("%x,",*(((uint8*)para1)+i));
            }
            osal_printf("]\n\r");
            osal_printf("para2[");
            for(i=0;i<len;i++)
            {
                osal_printf("%x,",*(((uint8*)para2)+i));
            }
            osal_printf("]\n\r");
                return RT_ERR_FAILED;
        }
    }

    return RT_ERR_OK;
}

int32 dal_gpon_para_test(uint32 testcase)
{
    rtk_gpon_onu_activation_para_t active,tmp_active;
    rtk_gpon_laser_para_t laser, tmp_laser;
    rtk_gpon_ds_physical_para_t ds_phy, tmp_ds_phy;
    rtk_gpon_ds_ploam_para_t    ds_ploam, tmp_ds_ploam;
    rtk_gpon_ds_bwMap_para_t bwmap, tmp_bwmap;
    rtk_gpon_ds_gem_para_t  gem, tmp_gem;
    rtk_gpon_ds_eth_para_t eth, tmp_eth;
    rtk_gpon_ds_omci_para_t omci, tmp_omci;
    rtk_gpon_us_physical_para_t us_phy, tmp_us_phy;
    rtk_gpon_us_ploam_para_t us_ploam, tmp_us_ploam;
    rtk_gpon_us_dbr_para_t dbr, tmp_dbr;
    int32 i,j,k,m,n,p,q;
#ifdef CONFIG_SDK_APOLLO
    /* active timer test */
    osal_memset(&active, 0, sizeof(rtk_gpon_onu_activation_para_t));
    osal_memset(&tmp_active, 0, sizeof(rtk_gpon_onu_activation_para_t));
    active.to1_timer = 20000;
    active.to2_timer = 200;
    if(para_compare(RTK_GPON_PARA_TYPE_ONU_ACTIVATION,NULL,
                    &active,&tmp_active,sizeof(rtk_gpon_onu_activation_para_t))!= RT_ERR_OK)
        return RT_ERR_FAILED;

    /* laser test */
    osal_memset(&laser, 0, sizeof(rtk_gpon_laser_para_t));
    osal_memset(&tmp_laser, 0, sizeof(rtk_gpon_laser_para_t));
    for(i=0; i<=1; i++)
        for(j=0; j<=1; j++)
            for(k=0; k<=1; k++)
                for(m=0; m<=1; m++)
                    for(n=0; n<=1; n++)
                        for(p=0; p<=1; p++)
                            for(q=0; q<=1; q++)
                            {
                                laser.laser_optic_los_en = i;
                                laser.laser_optic_los_polar = j;
                                laser.laser_cdr_los_en = k;
                                laser.laser_cdr_los_polar = m;
                                laser.laser_los_filter_en = n;
                                laser.laser_us_on = p;
                                laser.laser_us_off = q;
                                if(para_compare(RTK_GPON_PARA_TYPE_LASER,gpon_dbg_para_laser_get,
                                                &laser,&tmp_laser,sizeof(rtk_gpon_laser_para_t))!= RT_ERR_OK)
                                    return RT_ERR_FAILED;
                            }


    /* ds_phy test */
    osal_memset(&ds_phy, 0, sizeof(rtk_gpon_ds_physical_para_t));
    osal_memset(&tmp_ds_phy, 0, sizeof(rtk_gpon_ds_physical_para_t));
    for(i=0; i<=1; i++)
        for(j=0; j<=1; j++)
            for(k=0; k<=GPON_DEV_MAX_DS_FEC_DET_THRSH; k++)
            {
                ds_phy.ds_scramble_en  = i;
                ds_phy.ds_fec_bypass   = j;
                ds_phy.ds_fec_thrd     = k;
                if(para_compare(RTK_GPON_PARA_TYPE_DS_PHY,gpon_dbg_para_dsPhy_get,
                                &ds_phy,&tmp_ds_phy,sizeof(rtk_gpon_ds_physical_para_t))!= RT_ERR_OK)
                    return RT_ERR_FAILED;
            }

    /* ploam test */
    osal_memset(&ds_ploam, 0, sizeof(rtk_gpon_ds_ploam_para_t));
    osal_memset(&tmp_ds_ploam, 0, sizeof(rtk_gpon_ds_ploam_para_t));
    for(i=0; k<=1; i++)
        for(j=0; j<=1; j++)
            for(k=0; k<=1; k++)
            {
                    ds_ploam.ds_ploam_onuid_filter     = i;
                    ds_ploam.ds_ploam_broadcast_accpt  = j;
                    ds_ploam.ds_ploam_drop_crc_err     = k;
                    if(para_compare(RTK_GPON_PARA_TYPE_DS_PLOAM,gpon_dbg_para_dsPloam_get,
                                    &ds_ploam,&tmp_ds_ploam,sizeof(rtk_gpon_ds_ploam_para_t))!= RT_ERR_OK)
                        return RT_ERR_FAILED;
            }

    /* bwmap test */
    osal_memset(&bwmap, 0, sizeof(rtk_gpon_ds_bwMap_para_t));
    osal_memset(&tmp_bwmap, 0, sizeof(rtk_gpon_ds_bwMap_para_t));
    for(i=0; i<=1; i++)
        for(j=0; j<=1; j++)
            for(k=0; k<=1; k++)
            {
                bwmap.ds_bwmap_crc_chk  = i;
                bwmap.ds_bwmap_onuid_filter  = j;
                bwmap.ds_bwmap_plend_mode  = k;
                if(para_compare(RTK_GPON_PARA_TYPE_DS_BWMAP,gpon_dbg_para_dsBwmap_get,
                                &bwmap,&tmp_bwmap,sizeof(rtk_gpon_ds_bwMap_para_t))!= RT_ERR_OK)
                    return RT_ERR_FAILED;
            }

    /* gem test */
    osal_memset(&gem, 0, sizeof(rtk_gpon_ds_gem_para_t));
    osal_memset(&tmp_gem, 0, sizeof(rtk_gpon_ds_gem_para_t));
    for(i=0; i<=GPON_DEV_MAX_ASSM_TIMEOUT_FRM; i++)
    {
        gem.assemble_timer = i;
        if(para_compare(RTK_GPON_PARA_TYPE_DS_GEM,gpon_dbg_para_dsGem_get,
                        &gem,&tmp_gem,sizeof(rtk_gpon_ds_gem_para_t))!= RT_ERR_OK)
            return RT_ERR_FAILED;
    }

    /* ETH test */
    osal_memset(&eth, 0, sizeof(rtk_gpon_ds_eth_para_t));
    osal_memset(&tmp_eth, 0, sizeof(rtk_gpon_ds_eth_para_t));
    for(i=0; i<=1; i++)
        for(j=0; j<=GPON_DEV_MAX_PTI; j++)
            for(k=0; k<=GPON_DEV_MAX_PTI; k++)
            {
                eth.ds_eth_crc_chk = i;
                eth.ds_eth_pti_mask = j;
                eth.ds_eth_pti_ptn = k;
                if(para_compare(RTK_GPON_PARA_TYPE_DS_ETH,gpon_dbg_para_dsEth_get,
                                &eth,&tmp_eth,sizeof(rtk_gpon_ds_eth_para_t))!= RT_ERR_OK)
                    return RT_ERR_FAILED;
            }

    /* OMCI test */
    osal_memset(&omci, 0, sizeof(rtk_gpon_ds_omci_para_t));
    osal_memset(&tmp_omci, 0, sizeof(rtk_gpon_ds_omci_para_t));
    for(j=0; j<=GPON_DEV_MAX_PTI; j++)
        for(k=0; k<=GPON_DEV_MAX_PTI; k++)
        {
            omci.ds_omci_pti_mask = j;
            omci.ds_omci_pti_ptn = k;
            if(para_compare(RTK_GPON_PARA_TYPE_DS_OMCI,gpon_dbg_para_dsOmci_get,
                            &omci,&tmp_omci,sizeof(rtk_gpon_ds_omci_para_t))!= RT_ERR_OK)
                return RT_ERR_FAILED;
        }

    /* us phy test */
    osal_memset(&us_phy, 0, sizeof(rtk_gpon_us_physical_para_t));
    osal_memset(&tmp_us_phy, 0, sizeof(rtk_gpon_us_physical_para_t));
    for(i=0; i<=1; i++)
        for(j=0; j<=1; j++)
            for(k=0; k<=1; k++)
                for(m=0; m<=1; m++)
                {
                    us_phy.us_scramble_en = i;
                    us_phy.us_burst_en_polar = j;
                    us_phy.small_sstart_en = k;
                    us_phy.suppress_laser_en = m;
                    if(para_compare(RTK_GPON_PARA_TYPE_US_PHY,gpon_dbg_para_usPhy_get,
                                    &us_phy,&tmp_us_phy,sizeof(rtk_gpon_us_physical_para_t))!= RT_ERR_OK)
                        return RT_ERR_FAILED;
                }
 
    /* us phy test */
    osal_memset(&us_ploam, 0, sizeof(rtk_gpon_us_ploam_para_t));
    osal_memset(&tmp_us_ploam, 0, sizeof(rtk_gpon_us_ploam_para_t));
    for(i=0; i<=1; i++)
    {
        us_ploam.us_ploam_en = i;
        if(para_compare(RTK_GPON_PARA_TYPE_US_PLOAM,gpon_dbg_para_usPloam_get,
                        &us_ploam,&tmp_us_ploam,sizeof(rtk_gpon_us_ploam_para_t))!= RT_ERR_OK)
            return RT_ERR_FAILED;
    }
#endif    

#ifdef CONFIG_SDK_APOLLO 
    /* us dbr test */
    osal_memset(&dbr, 0, sizeof(rtk_gpon_us_dbr_para_t));
    osal_memset(&tmp_dbr, 0, sizeof(rtk_gpon_us_dbr_para_t));
    for(i=0; i<=1; i++)
    {
        dbr.us_dbru_en = i;
        if(para_compare(RTK_GPON_PARA_TYPE_US_DBR,gpon_dbg_para_usDbr_get,
                        &dbr,&tmp_dbr,sizeof(rtk_gpon_us_dbr_para_t))!= RT_ERR_OK)
            return RT_ERR_FAILED;
    }
#endif
    return RT_ERR_OK;
}

int32 dal_gpon_tcont_test(uint32 testcase)
{
    rtk_gpon_tcont_ind_t    ind;
    rtk_gpon_tcont_attr_t   attr, tmp_attr;
    int32 i,j;

    osal_memset(&ind, 0, sizeof(rtk_gpon_tcont_ind_t));
    osal_memset(&attr, 0, sizeof(rtk_gpon_tcont_attr_t));
    osal_memset(&tmp_attr, 0, sizeof(rtk_gpon_tcont_attr_t));

    ind.alloc_id = GPON_DEV_MAX_ALLOC_ID+1;
    ind.type = RTK_GPON_TCONT_TYPE_1;
    if( rtk_gpon_tcont_create(&ind, &attr) == RT_ERR_OK)
    {
        osal_printf("tcont create alloc=%d, type=%d, tcont=%d over max\n\r",ind.alloc_id,ind.type,attr.tcont_id);
        return RT_ERR_FAILED;
    }

    for(i=GPON_DEV_MAX_ALLOC_ID-31; i<=GPON_DEV_MAX_ALLOC_ID; i++)
        for(j=RTK_GPON_TCONT_TYPE_1; j<=RTK_GPON_TCONT_TYPE_5; j++)
        {
            ind.alloc_id = i;
            ind.type = j;
            if( rtk_gpon_tcont_create(&ind, &attr) != RT_ERR_OK)
            {
                osal_printf("tcont create alloc=%d, type=%d, tcont=%d fail\n\r",ind.alloc_id,ind.type,attr.tcont_id);
                return RT_ERR_FAILED;
            }
            if( rtk_gpon_tcont_get(&ind,&tmp_attr) != RT_ERR_OK)
            {
                osal_printf("tcont get alloc=%d, type=%d fail \n\r",ind.alloc_id,ind.type);
                return RT_ERR_FAILED;
            }
            if( osal_memcmp(&attr, &tmp_attr, sizeof(rtk_gpon_tcont_attr_t)) != 0)
            {
                osal_printf("compare tcont tcont=%d, tmp_tcont=%d \n\r",attr.tcont_id,tmp_attr.tcont_id);
                return RT_ERR_FAILED;
            }
            if( rtk_gpon_tcont_destroy(&ind) != RT_ERR_OK)
            {
                osal_printf("tcont destroy alloc=%d, type=%d fail \n\r",ind.alloc_id,ind.type);
                return RT_ERR_FAILED;
            }
            if( rtk_gpon_tcont_get(&ind,&tmp_attr) == RT_ERR_OK)
            {
                osal_printf("tcont get alloc=%d, type=%d deleted OK\n\r",ind.alloc_id,ind.type);
                return RT_ERR_FAILED;
            }
        }

    for(i=0; i<GPON_DEV_MAX_TCONT_NUM; i++)
    {
        ind.alloc_id = i;
        ind.type = RTK_GPON_TCONT_TYPE_1;
        if( rtk_gpon_tcont_create(&ind, &attr) != RT_ERR_OK)
        {
            osal_printf("tcont create alloc=%d, type=%d, tcont=%d fail \n\r",ind.alloc_id,ind.type,attr.tcont_id);
            return RT_ERR_FAILED;
        }

    }
    ind.alloc_id = GPON_DEV_MAX_TCONT_NUM;
    ind.type = RTK_GPON_TCONT_TYPE_1;
    if( rtk_gpon_tcont_create(&ind, &attr) == RT_ERR_OK)
    {
        osal_printf("tcont create alloc=%d, type=%d, tcont=%d full OK\n\r",ind.alloc_id,ind.type,attr.tcont_id);
        return RT_ERR_FAILED;
    }
    for(i=0; i<GPON_DEV_MAX_TCONT_NUM; i++)
    {
        ind.alloc_id = i;

        if( rtk_gpon_tcont_destroy(&ind) != RT_ERR_OK)
        {
            osal_printf("tcont create alloc=%d, type=%d, tcont=%d fail \n\r",ind.alloc_id,ind.type,attr.tcont_id);
            return RT_ERR_FAILED;
        }

    }


    return RT_ERR_OK;
}

int32 dal_gpon_dsFlow_test(uint32 testcase)
{
    uint32 flow_id;
    rtk_gpon_dsFlow_attr_t   attr, tmp_attr;
    int32 i,j,k,m,n;
    int32 ret;

    osal_memset(&attr, 0, sizeof(rtk_gpon_dsFlow_attr_t));
    osal_memset(&tmp_attr, 0, sizeof(rtk_gpon_dsFlow_attr_t));

    /* flow id is invalid */
    flow_id = GPON_DEV_MAX_FLOW_NUM;
    attr.gem_port_id = 1;
    attr.type = RTK_GPON_FLOW_TYPE_ETH;
    if( rtk_gpon_dsFlow_set(flow_id, &attr) == RT_ERR_OK)
        return RT_ERR_FAILED;

    /* gem port id is invalid */
    flow_id = 1;
    attr.gem_port_id = GPON_DEV_MAX_GEMPORT_ID+1;
    attr.type = RTK_GPON_FLOW_TYPE_ETH;
    if( rtk_gpon_dsFlow_set(flow_id, &attr) == RT_ERR_OK)
        return RT_ERR_FAILED;

    /* type is invalid */
    flow_id = 1;
    attr.gem_port_id = 1;
    attr.type = RTK_GPON_FLOW_TYPE_TDM+1;
    if( rtk_gpon_dsFlow_set(flow_id, &attr) == RT_ERR_OK)
        return RT_ERR_FAILED;

    for(i=0; i<GPON_DEV_MAX_FLOW_NUM; i++)
        //for(j=0; j<=GPON_DEV_MAX_GEMPORT_ID; j++)
        for(j=0; j<=2; j++)
            for(k=RTK_GPON_FLOW_TYPE_OMCI; k<=RTK_GPON_FLOW_TYPE_TDM; k++)
                for(m=0; m<=1; m++)
                    for(n=0; n<=1; n++)
                    {
                        flow_id = i;
                        attr.gem_port_id = j;
                        attr.type = k;
                        attr.multicast = (attr.type==RTK_GPON_FLOW_TYPE_ETH)?m:0;
                        attr.aes_en = n;
                        if( rtk_gpon_dsFlow_set(flow_id, &attr) != RT_ERR_OK)
                            return RT_ERR_FAILED;
                        if( rtk_gpon_dsFlow_get(flow_id,&tmp_attr) != RT_ERR_OK)
                            return RT_ERR_FAILED;
                        if( osal_memcmp(&attr, &tmp_attr, sizeof(rtk_gpon_dsFlow_attr_t)) != 0)
                        {
                            osal_printf("ds flow db cmp, attr1: flow=%d, gem=%d, type=%d, mc=%d, aes=%d\n\r",
                                   flow_id,attr.gem_port_id,attr.type,attr.multicast,attr.aes_en);
                            osal_printf("ds flow db cmp, attr2: flow=%d, gem=%d, type=%d, mc=%d, aes=%d\n\r",
                                   flow_id,tmp_attr.gem_port_id,tmp_attr.type,tmp_attr.multicast,tmp_attr.aes_en);
                            return RT_ERR_FAILED;
                        }
#ifdef CONFIG_SDK_APOLLO 
                        if( gpon_dbg_dsFlow_get(flow_id, &tmp_attr) != RT_ERR_OK)
                            return RT_ERR_FAILED;
                        if( osal_memcmp(&attr, &tmp_attr, sizeof(rtk_gpon_dsFlow_attr_t)) != 0)
                        {
                            osal_printf("ds flow reg cmp, attr1: flow=%d, gem=%d, type=%d, mc=%d, aes=%d\n\r",
                                   flow_id,attr.gem_port_id,attr.type,attr.multicast,attr.aes_en);
                            osal_printf("ds flow reg cmp, attr2: flow=%d, gem=%d, type=%d, mc=%d, aes=%d\n\r",
                                   flow_id,tmp_attr.gem_port_id,tmp_attr.type,tmp_attr.multicast,tmp_attr.aes_en);
                            return RT_ERR_FAILED;
                        }
#endif
                        /* delete entry */
                        attr.gem_port_id = RTK_GPON_GEMPORT_ID_NOUSE;
                        if((ret = rtk_gpon_dsFlow_set(flow_id, &attr)) != RT_ERR_OK)
                        {
                            osal_printf("ds flow del fail[0x%x]: flow=%d, gem=%d\n\r", ret, flow_id, attr.gem_port_id);
                            return RT_ERR_FAILED;
                        }
                    }

    for(i=0; i<GPON_DEV_MAX_FLOW_NUM; i++)
    {
        flow_id = i;
        attr.gem_port_id = i;
        attr.type = RTK_GPON_FLOW_TYPE_ETH;
        attr.multicast = 0;
        attr.aes_en = 0;
        if((ret = rtk_gpon_dsFlow_set(flow_id, &attr)) != RT_ERR_OK)
        {
            osal_printf("add ds flow fail[0x%x], attr1: flow=%d, gem=%d\n\r",
                                   ret, flow_id,attr.gem_port_id);
            return RT_ERR_FAILED;
        }
    }
    flow_id = GPON_DEV_MAX_FLOW_NUM;
    attr.gem_port_id = GPON_DEV_MAX_FLOW_NUM;
    if( rtk_gpon_dsFlow_set(flow_id, &attr) == RT_ERR_OK)
    {
        osal_printf("add one more ds flow OK, flow=%d, gem=%d\n\r",
                                   flow_id,attr.gem_port_id);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

int32 dal_gpon_usFlow_test(uint32 testcase)
{
    uint32 flow_id;
    rtk_gpon_usFlow_attr_t attr, tmp_attr;
    int32 i,j,k,m;
    int32 ret;

    osal_memset(&attr, 0, sizeof(rtk_gpon_usFlow_attr_t));
    osal_memset(&tmp_attr, 0, sizeof(rtk_gpon_usFlow_attr_t));

    /* flow id is invalid */
    flow_id = GPON_DEV_MAX_FLOW_NUM;
    attr.gem_port_id = 1;
    attr.type = RTK_GPON_FLOW_TYPE_ETH;
    attr.tcont_id = 1;
    if( rtk_gpon_usFlow_set(flow_id, &attr) == RT_ERR_OK)
        return RT_ERR_FAILED;

    /* gem port id is invalid */
    flow_id = 1;
    attr.gem_port_id = GPON_DEV_MAX_GEMPORT_ID+1;
    attr.type = RTK_GPON_FLOW_TYPE_ETH;
    attr.tcont_id = 1;
    if( rtk_gpon_usFlow_set(flow_id, &attr) == RT_ERR_OK)
        return RT_ERR_FAILED;

    /* type is invalid */
    flow_id = 1;
    attr.gem_port_id = 1;
    attr.type = RTK_GPON_FLOW_TYPE_TDM+1;
    attr.tcont_id = 1;
    if( rtk_gpon_usFlow_set(flow_id, &attr) == RT_ERR_OK)
        return RT_ERR_FAILED;

    /* tcont is invalid */
    flow_id = 1;
    attr.gem_port_id = 1;
    attr.type = RTK_GPON_FLOW_TYPE_ETH;
    attr.tcont_id = GPON_DEV_MAX_TCONT_NUM;
    attr.channel = GPON_DEV_MAX_TDM_CHANNEL_NUM;
    if( rtk_gpon_usFlow_set(flow_id, &attr) == RT_ERR_OK)
        return RT_ERR_FAILED;

    for(i=0; i<GPON_DEV_MAX_FLOW_NUM; i++)
        //for(j=0; j<=GPON_DEV_MAX_GEMPORT_ID; j++)
        for(j=0; j<=2; j++)
            for(k=RTK_GPON_FLOW_TYPE_OMCI; k<RTK_GPON_FLOW_TYPE_TDM; k++)
                //for(m=0; m<GPON_DEV_MAX_TCONT_NUM; m++)
                for(m=0; m<2; m++)
                {
                    flow_id = i;
                    attr.gem_port_id = j;
                    attr.type = k;
                    attr.tcont_id = m;
                    attr.channel = GPON_DEV_MAX_TDM_CHANNEL_NUM;
                    if( (ret=rtk_gpon_usFlow_set(flow_id, &attr)) != RT_ERR_OK)
                    {
                        osal_printf("rtk_gpon_usFlow_set, ret=%x: flow=%d, gem=%d, type=%d, tcont=%d, ch=%d\n\r",
                               ret,flow_id,attr.gem_port_id,attr.type,attr.tcont_id,attr.channel);
                        return RT_ERR_FAILED;
                    }
                    if( rtk_gpon_usFlow_get(flow_id,&tmp_attr) != RT_ERR_OK)
                        return RT_ERR_FAILED;
                    if( osal_memcmp(&attr, &tmp_attr, sizeof(rtk_gpon_usFlow_attr_t)) != 0)
                    {
                        osal_printf("us flow db cmp, attr1: flow=%d, gem=%d, type=%d, tcont=%d, ch=%d\n\r",
                               flow_id,attr.gem_port_id,attr.type,attr.tcont_id,attr.channel);
                        osal_printf("us flow db cmp, attr2: flow=%d, gem=%d, type=%d, tcont=%d, ch=%d\n\r",
                               flow_id,tmp_attr.gem_port_id,tmp_attr.type,tmp_attr.tcont_id,tmp_attr.channel);
                        return RT_ERR_FAILED;
                    }
#ifdef CONFIG_SDK_APOLLO 
                    if( gpon_dbg_usFlow_get(flow_id, &tmp_attr.gem_port_id) != RT_ERR_OK)
                        return RT_ERR_FAILED;
                    if( attr.gem_port_id != tmp_attr.gem_port_id )
                    {
                        osal_printf("us flow reg cmp, attr1: flow=%d, gem=%d\n\r",
                               flow_id,attr.gem_port_id);
                        osal_printf("us flow reg cmp, attr2: flow=%d, gem=%d\n\r",
                               flow_id,tmp_attr.gem_port_id);
                        return RT_ERR_FAILED;
                    }
#endif                    
                    /* delete entry */
                    attr.gem_port_id = RTK_GPON_GEMPORT_ID_NOUSE;
                    if( rtk_gpon_usFlow_set(flow_id, &attr) != RT_ERR_OK)
                        return RT_ERR_FAILED;
                }

    for(i=0; i<GPON_DEV_MAX_FLOW_NUM; i++)
    {
        flow_id = i;
        attr.gem_port_id = i;
        attr.type = RTK_GPON_FLOW_TYPE_ETH;
        attr.tcont_id = 1;
        attr.channel = GPON_DEV_MAX_TDM_CHANNEL_NUM;
        if( (ret = rtk_gpon_usFlow_set(flow_id, &attr)) != RT_ERR_OK)
        {
            osal_printf("rtk_gpon_usFlow_set, ret=%x: flow=%d, gem=%d\n\r",
                               ret,flow_id,attr.gem_port_id);
            return RT_ERR_FAILED;
        }
    }
    flow_id = GPON_DEV_MAX_FLOW_NUM;
    attr.gem_port_id = GPON_DEV_MAX_FLOW_NUM;
    if( rtk_gpon_usFlow_set(flow_id, &attr) == RT_ERR_OK)
    {
        osal_printf("rtk_gpon_usFlow_set, ret=%x: flow=%d, gem=%d\n\r",
                               ret,flow_id,attr.gem_port_id);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

int32 dal_gpon_mcFilter_test(uint32 testcase)
{
    int32 mode, tmp_mode;
    uint32 patern_v4, tmp_patern_v4, patern_v6, tmp_patern_v6;
    rtk_gpon_macTable_exclude_mode_t filter, tmp_filter;
    rtk_gpon_mc_force_mode_t force_v4, tmp_force_v4, force_v6, tmp_force_v6;
    rtk_gpon_macTable_entry_t entry, tmp_entry;
    int32 i,ret;

    /* broadcast passs mode, non-multicast pass mode */
    if( rtk_gpon_broadcastPass_set(2) == RT_ERR_OK )
    {
        osal_printf("fun:%s, line=%d \n\r",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if( rtk_gpon_nonMcastPass_set(2) == RT_ERR_OK )
    {
        osal_printf("fun:%s, line=%d \n\r",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    for(mode=0; mode<=1; mode++)
    {
        if( rtk_gpon_broadcastPass_set(mode) != RT_ERR_OK )
        {
            osal_printf("fun:%s, line=%d \n\r",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        if( rtk_gpon_broadcastPass_get(&tmp_mode) != RT_ERR_OK )
        {
            osal_printf("fun:%s, line=%d \n\r",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        if( mode != tmp_mode )
        {
            osal_printf("bcPass db, mode=%d, tmp_mode=%d\n\r",
                               mode,tmp_mode);
            return RT_ERR_FAILED;
        }
#ifdef CONFIG_SDK_APOLLO
        if( gpon_dbg_broadcastPass_get(&tmp_mode) != RT_ERR_OK )
            return RT_ERR_FAILED;
        if( mode != tmp_mode )
        {
            osal_printf("bcPass reg, mode=%d, tmp_mode=%d\n\r",
                               mode,tmp_mode);
            return RT_ERR_FAILED;
        }
#endif
        if( rtk_gpon_nonMcastPass_set(mode) != RT_ERR_OK )
        {
            osal_printf("fun:%s, line=%d \n\r",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        if( rtk_gpon_nonMcastPass_get(&tmp_mode) != RT_ERR_OK )
        {
            osal_printf("fun:%s, line=%d \n\r",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        if( mode != tmp_mode )
        {
            osal_printf("nonMcPass db, mode=%d, tmp_mode=%d\n\r",
                               mode,tmp_mode);
            return RT_ERR_FAILED;
        }
#ifdef CONFIG_SDK_APOLLO 
        if( gpon_dbg_nonMcastPass_get(&tmp_mode) != RT_ERR_OK )
            return RT_ERR_FAILED;
        if( mode != tmp_mode )
        {
            osal_printf("nonMcastPass reg, mode=%d, tmp_mode=%d\n\r",
                               mode,tmp_mode);
            return RT_ERR_FAILED;
        }
#endif        
    }

    /* force mode */
    if( rtk_gpon_mcForceMode_set(RTK_GPON_MCFORCE_MODE_DROP+1,RTK_GPON_MCFORCE_MODE_DROP+1) == RT_ERR_OK )
    {
        osal_printf("fun:%s, line=%d \n\r",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    for(force_v4=RTK_GPON_MCFORCE_MODE_NORMAL; force_v4<=RTK_GPON_MCFORCE_MODE_DROP; force_v4++)
    {
        for(force_v6=RTK_GPON_MCFORCE_MODE_NORMAL; force_v6<=RTK_GPON_MCFORCE_MODE_DROP; force_v6++)
        {
            if( rtk_gpon_mcForceMode_set(force_v4,force_v6) != RT_ERR_OK )
            {
                osal_printf("fun:%s, line=%d \n\r",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
            if( rtk_gpon_mcForceMode_get(&tmp_force_v4,&tmp_force_v6) != RT_ERR_OK )
            {
                osal_printf("fun:%s, line=%d \n\r",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
            if( force_v4 != tmp_force_v4 )
            {
                osal_printf("mcForceMode db, force_v4=%d, tmp_force_v4=%d\n\r",
                                   force_v4,tmp_force_v4);
                return RT_ERR_FAILED;
            }
            if( force_v6 != tmp_force_v6 )
            {
                osal_printf("mcForceMode db, force_v6=%d, tmp_force_v6=%d\n\r",
                                   force_v6,tmp_force_v6);
                return RT_ERR_FAILED;
            }
#ifdef CONFIG_SDK_APOLLO 
            if( gpon_dbg_mcForceMode_get(&tmp_force_v4,&tmp_force_v6) != RT_ERR_OK )
                return RT_ERR_FAILED;
            if( force_v4 != tmp_force_v4 )
            {
                osal_printf("mcForceMode reg, force_v4=%d, tmp_force_v4=%d\n\r",
                                   force_v4,tmp_force_v4);
                return RT_ERR_FAILED;
            }
            if( force_v6 != tmp_force_v6 )
            {
                osal_printf("mcForceMode reg, force_v6=%d, tmp_force_v6=%d\n\r",
                                   force_v6,tmp_force_v6);
                return RT_ERR_FAILED;
            }
#endif            
        }
    }

    /* MAC address pattern */
    for(patern_v4=0; patern_v4<=0x01005E; patern_v4+=0x01005E)
    {
        for(patern_v6=0; patern_v6<=0x3333; patern_v6+=0x3333)
        {
            if( rtk_gpon_multicastAddrCheck_set(patern_v4, patern_v6) != RT_ERR_OK )
            {
                osal_printf("fun:%s, line=%d \n\r",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
            if( rtk_gpon_multicastAddrCheck_get(&tmp_patern_v4, &tmp_patern_v6) != RT_ERR_OK )
            {
                osal_printf("fun:%s, line=%d \n\r",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
            if( (patern_v4 != tmp_patern_v4) || (patern_v6 != tmp_patern_v6))
            {
                osal_printf("patern db, patern_v4=%x, tmp_patern_v4=%x, patern_v6=%x, tmp_patern_v6=%x\n\r",
                               patern_v4,tmp_patern_v4,patern_v6,tmp_patern_v6);
                return RT_ERR_FAILED;
            }
#ifdef CONFIG_SDK_APOLLO 
            if( gpon_dbg_multicastAddrCheck_get(&tmp_patern_v4, &tmp_patern_v6) != RT_ERR_OK )
            {
                osal_printf("fun:%s, line=%d \n\r",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
            if( (patern_v4 != tmp_patern_v4) || (patern_v6 != tmp_patern_v6))
            {
                osal_printf("patern reg, patern_v4=%x, tmp_patern_v4=%x, patern_v6=%x, tmp_patern_v6=%x\n\r",
                               patern_v4,tmp_patern_v4,patern_v6,tmp_patern_v6);
                return RT_ERR_FAILED;
            }
#endif            
        }
    }

    /* filter mode */
    if( rtk_gpon_macFilterMode_set((RTK_GPON_MACTBL_MODE_EXCLUDE+1)) == RT_ERR_OK )
    {
        osal_printf("fun:%s, line=%d \n\r",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    for(filter=RTK_GPON_MACTBL_MODE_INCLUDE; filter<=RTK_GPON_MACTBL_MODE_EXCLUDE; filter++)
    {
        if( rtk_gpon_macFilterMode_set(filter) != RT_ERR_OK )
        {
            osal_printf("fun:%s, line=%d \n\r",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        if( rtk_gpon_macFilterMode_get(&tmp_filter) != RT_ERR_OK )
        {
            osal_printf("fun:%s, line=%d \n\r",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        if( filter != tmp_filter )
        {
            osal_printf("filter db, mode=%d, tmp_mode=%d\n\r",
                   filter,tmp_filter);
            return RT_ERR_FAILED;
        }
#ifdef CONFIG_SDK_APOLLO 
        if( gpon_dbg_macFilterMode_get(&tmp_filter) != RT_ERR_OK )
        {
            osal_printf("fun:%s, line=%d \n\r",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        if( filter != tmp_filter )
        {
            osal_printf("filter reg, mode=%d, tmp_mode=%d\n\r",
                   filter,tmp_filter);
            return RT_ERR_FAILED;
        }
#endif
    }

    /* mac table entry */
    osal_memset(&entry, 0, sizeof(rtk_gpon_macTable_entry_t));
    osal_memset(&tmp_entry, 0, sizeof(rtk_gpon_macTable_entry_t));
    for(i=0; i<GPON_DEV_MAX_MACTBL_NUM; i++)
    {
        entry.mac_addr[5]=i;
        if( (ret = rtk_gpon_macEntry_add(&entry)) != RT_ERR_OK)
        {
            osal_printf("fun:%s, line=%d, i=%d, ret=%x \n\r",__FUNCTION__,__LINE__,i,ret);
            return RT_ERR_FAILED;
        }
        if( (ret = rtk_gpon_macEntry_get(i, &tmp_entry)) != RT_ERR_OK )
        {
            osal_printf("fun:%s, line=%d, i=%d, ret=%x\n\r",__FUNCTION__,__LINE__,i,ret);
            return RT_ERR_FAILED;
        }
        if( osal_memcmp(&entry, &tmp_entry, sizeof(rtk_gpon_macTable_entry_t)) != 0 )
        {
            osal_printf("mac table db, entry=%x-%x-%x, tmp_entry=%x-%x-%x\n\r",
                   entry.mac_addr[5],entry.mac_addr[4],entry.mac_addr[3],
                   tmp_entry.mac_addr[5],tmp_entry.mac_addr[4],tmp_entry.mac_addr[3]);
            return RT_ERR_FAILED;
        }
#ifdef CONFIG_SDK_APOLLO 
        if( gpon_dbg_macEntry_get(i, &tmp_entry) != RT_ERR_OK )
        {
            osal_printf("fun:%s, line=%d \n\r",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        if( osal_memcmp(&entry, &tmp_entry, sizeof(rtk_gpon_macTable_entry_t)) != 0 )
        {
            osal_printf("mac table reg, entry=%x-%x-%x, tmp_entry=%x-%x-%x\n\r",
                   entry.mac_addr[5],entry.mac_addr[4],entry.mac_addr[3],
                   tmp_entry.mac_addr[5],tmp_entry.mac_addr[4],tmp_entry.mac_addr[3]);
            return RT_ERR_FAILED;
        }
#endif        
    }

    entry.mac_addr[4] = 0x1;
    if( rtk_gpon_macEntry_add(&entry) == RT_ERR_OK)
    {
        osal_printf("fun:%s, line=%d \n\r",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&entry, 0, sizeof(rtk_gpon_macTable_entry_t));
    for(i=0; i<GPON_DEV_MAX_MACTBL_NUM; i++)
    {
        entry.mac_addr[5]=i;
        if( rtk_gpon_macEntry_del(&entry) != RT_ERR_OK)
        {
            osal_printf("fun:%s, line=%d \n\r",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        if( rtk_gpon_macEntry_get(i, &entry) == RT_ERR_OK )
        {
            osal_printf("fun:%s, line=%d \n\r",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }
    return RT_ERR_OK;
}

int32 dal_gpon_misc_test(uint32 testcase)
{
    int32 mode, tmp_mode;
    rtk_gpon_laser_status_t laser, tmp_laser;

    for(mode=0; mode<=1; mode++)
    {
        /* RDI */
        if( rtk_gpon_rdi_set(mode) != RT_ERR_OK)
        {
            osal_printf("fun:%s, line=%d \n\r",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        if( rtk_gpon_rdi_get(&tmp_mode) != RT_ERR_OK )
        {
            osal_printf("fun:%s, line=%d \n\r",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        if(mode != tmp_mode)
        {
            osal_printf("rdi db, mode=%d, tmp_mode=%d\n\r",
                               mode,tmp_mode);
            return RT_ERR_FAILED;
        }
#ifdef CONFIG_SDK_APOLLO 
        if( gpon_dbg_rdi_get(&tmp_mode) != RT_ERR_OK )
        {
            osal_printf("fun:%s, line=%d \n\r",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        if(mode != tmp_mode)
        {
            osal_printf("rdi reg, mode=%d, tmp_mode=%d\n\r",
                               mode,tmp_mode);
            return RT_ERR_FAILED;
        }
#endif        
        /* FS Idle */
        if( rtk_gpon_txForceIdle_set(mode) != RT_ERR_OK)
        {
            osal_printf("fun:%s, line=%d \n\r",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        if( rtk_gpon_txForceIdle_get(&tmp_mode) != RT_ERR_OK )
        {
            osal_printf("fun:%s, line=%d \n\r",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        if(mode != tmp_mode)
        {
            osal_printf("FS Idle reg, mode=%d, tmp_mode=%d\n\r",
                               mode,tmp_mode);
            return RT_ERR_FAILED;
        }
#if 0
        /* FS PRBS */
        if( rtk_gpon_txForcePRBS_set(mode) != RT_ERR_OK)
        {
            osal_printf("fun:%s, line=%d \n\r",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        if( rtk_gpon_txForcePRBS_get(&tmp_mode) != RT_ERR_OK )
        {
            osal_printf("fun:%s, line=%d \n\r",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        if(mode != tmp_mode)
        {
            osal_printf("FS PRBS reg, mode=%d, tmp_mode=%d\n\r",
                               mode,tmp_mode);
            return RT_ERR_FAILED;
        }
#endif
    }

    /* FS Laser */
    for(laser=RTK_GPON_LASER_STATUS_NORMAL; laser<=RTK_GPON_LASER_STATUS_FORCE_OFF; laser++)
    {
        if( rtk_gpon_txForceLaser_set(laser) != RT_ERR_OK)
        {
            osal_printf("fun:%s, line=%d \n\r",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        if( rtk_gpon_txForceLaser_get(&tmp_laser) != RT_ERR_OK )
        {
            osal_printf("fun:%s, line=%d \n\r",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        if(laser != tmp_laser)
        {
            osal_printf("laser reg, mode=%d, tmp_mode=%d\n\r",
                               mode,tmp_mode);
            return RT_ERR_FAILED;
        }

    }
    return RT_ERR_OK;
}


