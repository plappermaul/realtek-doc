#include <osal/lib.h>
#include <osal/print.h>
#include <ioal/mem32.h>
#include <hal/common/halctrl.h>
#include <common/error.h>
#include <dal/dal_l2_test_case.h>
#include <rtk/l2.h>
#include <common/unittest_util.h>

static int32 _dal_apollomp_l2_addr(void)
{
    rtk_vlan_t  vid;
    uint32      mac;
    uint32      fid;
    uint32      efid;
    rtk_port_t  port;
    uint32      ext_port;
    uint32      flags;          /* Refer to RTK_L2_UCAST_FLAG_XXX */
    uint32      priority;
    uint8       auth;

    int32       index;

    rtk_l2_ucastAddr_t l2AddrW;
    rtk_l2_ucastAddr_t l2AddrR;

    int32 retVal;

    /* Get/set */
    for(vid = 1; vid <= RTK_VLAN_ID_MAX; vid+=4094)
    {
        for(mac = 0x01; mac <= 0xFF; mac += 0xFE)
        {
            for(fid = 0; fid <= HAL_VLAN_FID_MAX(); fid+=HAL_VLAN_FID_MAX())
            {
                for(efid = 0; efid <= HAL_ENHANCED_FID_MAX(); efid+=HAL_ENHANCED_FID_MAX())
                {
                    for(port = 0; port <= HAL_GET_MAX_PORT(); port+=HAL_GET_MAX_PORT())
                    {
                        for(ext_port = 0; ext_port <= HAL_GET_MAX_EXT_PORT(); ext_port+=HAL_GET_MAX_EXT_PORT())
                        {
                            if(ext_port != HAL_GET_EXT_CPU_PORT())
                            {
                                for(flags = RTK_L2_UCAST_FLAG_SA_BLOCK; flags <= RTK_L2_UCAST_FLAG_ALL; flags = (flags << 1))
                                {
                                    for(priority = 0; priority <= HAL_INTERNAL_PRIORITY_MAX(); priority+=HAL_INTERNAL_PRIORITY_MAX())
                                    {
                                        for(auth = 0; auth < RTK_ENABLE_END; auth++)
                                        {
                                            osal_memset(&l2AddrW, 0x00, sizeof(rtk_l2_ucastAddr_t));
                                            l2AddrW.vid             = vid;
                                            l2AddrW.mac.octet[5]    = mac;
                                            l2AddrW.port            = port;
                                            l2AddrW.flags           = flags | RTK_L2_UCAST_FLAG_STATIC;
                                            l2AddrW.age             = 1;
                                        #ifdef CONFIG_SDK_RTL9601B 
                                            l2AddrW.ctag_vid        = fid;
                                        #elif defined(CONFIG_SDK_RTL9602C)
                                            l2AddrW.priority        = priority;
                                            l2AddrW.auth            = auth;
                                            l2AddrW.fid             = fid;
                                            l2AddrW.ext_port        = ext_port;
                                        #else
                                            l2AddrW.priority        = priority;
                                            l2AddrW.auth            = auth;
                                            l2AddrW.fid             = fid;
                                            l2AddrW.efid            = efid;
                                            l2AddrW.ext_port        = ext_port;
                                        #endif    
                                            /* Create */
                                            if((retVal = rtk_l2_addr_add(&l2AddrW)) != RT_ERR_OK)
                                            {
                                                osal_printf("\n %s %d 0x%X\n",__FUNCTION__,__LINE__, retVal);
                                                return RT_ERR_FAILED;
                                            }

                                            /* Get */
                                            osal_memset(&l2AddrR, 0x00, sizeof(rtk_l2_ucastAddr_t));
                                            l2AddrR.vid             = vid;
                                            l2AddrR.mac.octet[5]    = mac;
                                        #ifdef CONFIG_SDK_RTL9601B
                                        #elif defined(CONFIG_SDK_RTL9602C)
                                            l2AddrR.fid             = fid;
                                        #else            
                                            l2AddrR.fid             = fid;
                                            l2AddrR.efid            = efid;
                                        #endif
                                            l2AddrR.flags           = flags | RTK_L2_UCAST_FLAG_STATIC;
                                            if((retVal = rtk_l2_addr_get(&l2AddrR)) != RT_ERR_OK)
                                            {
                                                osal_printf("\n %s %d 0x%X\n",__FUNCTION__,__LINE__, retVal);
                                                return RT_ERR_FAILED;
                                            }

                                            if(flags & RTK_L2_UCAST_FLAG_IVL)
                                                l2AddrW.fid = l2AddrR.fid;
                                            else
                                                l2AddrW.vid = l2AddrR.vid;

                                            l2AddrW.index = l2AddrR.index;

                                            if(osal_memcmp(&l2AddrW, &l2AddrR, sizeof(rtk_l2_ucastAddr_t)) != 0)
                                            {
                                                osal_printf("\n l2AddrW.vid: %d\n",l2AddrW.vid);
                                                osal_printf("\n l2AddrW.fid: %d\n",l2AddrW.fid);
                                                osal_printf("\n l2AddrW.port: %d\n",l2AddrW.port);
                                                osal_printf("\n l2AddrW.flags: %d\n",l2AddrW.flags);
                                                osal_printf("\n l2AddrW.index: %d\n",l2AddrW.index);
                                                
                                                osal_printf("\n l2AddrR.vid: %d\n",l2AddrR.vid);
                                                osal_printf("\n l2AddrR.fid: %d\n",l2AddrR.fid);
                                                osal_printf("\n l2AddrR.port: %d\n",l2AddrR.port);
                                                osal_printf("\n l2AddrR.flags: %d\n",l2AddrR.flags);
                                                osal_printf("\n l2AddrR.index: %d\n",l2AddrR.index);
                                                
                                                
                                                osal_printf("\n %s %d 0x%X\n",__FUNCTION__,__LINE__, retVal);
                                                return RT_ERR_FAILED;
                                            }

                                            /* Get Next */
                                            index = 0;
                                            if((retVal = rtk_l2_nextValidAddr_get(&index, &l2AddrR)) != RT_ERR_OK)
                                            {
                                                osal_printf("\n %s %d 0x%X\n",__FUNCTION__,__LINE__, retVal);
                                                return RT_ERR_FAILED;
                                            }

                                            if(osal_memcmp(&l2AddrW, &l2AddrR, sizeof(rtk_l2_ucastAddr_t)) != 0)
                                            {
                                                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                                                return RT_ERR_FAILED;
                                            }

                                            /* Get Next on port*/
                                            index = 0;
                                            if((retVal = rtk_l2_nextValidAddrOnPort_get(port, &index, &l2AddrR)) != RT_ERR_OK)
                                            {
                                                osal_printf("\n %s %d 0x%X\n",__FUNCTION__,__LINE__, retVal);
                                                return RT_ERR_FAILED;
                                            }

                                            if(osal_memcmp(&l2AddrW, &l2AddrR, sizeof(rtk_l2_ucastAddr_t)) != 0)
                                            {
                                                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                                                return RT_ERR_FAILED;
                                            }

                                            /* Delete */
                                            if((retVal = rtk_l2_addr_del(&l2AddrR)) != RT_ERR_OK)
                                            {
                                                osal_printf("\n %s %d 0x%X\n",__FUNCTION__,__LINE__, retVal);
                                                return RT_ERR_FAILED;
                                            }

                                            /* Confirm the entry is deleted */
                                            osal_memset(&l2AddrR, 0x00, sizeof(rtk_l2_ucastAddr_t));
                                            l2AddrR.vid             = vid;
                                            l2AddrR.mac.octet[5]    = mac;
                                            l2AddrR.fid             = fid;
                                        #if defined(CONFIG_SDK_RTL9602C)
                                        #else
                                            l2AddrR.efid            = efid;
                                        #endif
                                            l2AddrR.flags           = flags | RTK_L2_UCAST_FLAG_STATIC;
                                            if(rtk_l2_addr_get(&l2AddrR) == RT_ERR_OK)
                                            {
                                                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                                                return RT_ERR_FAILED;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return RT_ERR_OK;
}

static int32 _dal_rtl9607c_l2_addr(void)
{
    rtk_vlan_t  vid;
    uint32      mac;
    uint32      fid;
    uint32      efid;
    rtk_port_t  port;
    uint32      ext_port;
    uint32      flags;          /* Refer to RTK_L2_UCAST_FLAG_XXX */
    uint32      priority;
    uint8       auth;

    int32       index;

    rtk_l2_ucastAddr_t l2AddrW;
    rtk_l2_ucastAddr_t l2AddrR;

    int32 retVal;

    /* Get/set */
    for(vid = 1; vid <= RTK_VLAN_ID_MAX; vid+=4094)
    {
        for(mac = 0x01; mac <= 0xFF; mac += 0xFE)
        {
            for(fid = 0; fid <= HAL_VLAN_FID_MAX(); fid+=HAL_VLAN_FID_MAX())
            {
                HAL_SCAN_ALL_PORT(port)
                {
                    for(ext_port = 0; ext_port <= HAL_GET_MAX_EXT_PORT(); ext_port+=HAL_GET_MAX_EXT_PORT())
                    {
                        if(ext_port != HAL_GET_EXT_CPU_PORT())
                        {
                            for(flags = RTK_L2_UCAST_FLAG_SA_BLOCK; flags <= RTK_L2_UCAST_FLAG_ALL; flags = (flags << 1))
                            {
                                if((flags == RTK_L2_UCAST_FLAG_FWD_PRI) || 
                                    (flags == RTK_L2_UCAST_FLAG_LOOKUP_PRI))
                                {
                                    continue;
                                }
                                osal_memset(&l2AddrW, 0x00, sizeof(rtk_l2_ucastAddr_t));
                                l2AddrW.vid             = vid;
                                l2AddrW.mac.octet[5]    = mac;
                                l2AddrW.port            = port;
                                l2AddrW.flags           = flags | RTK_L2_UCAST_FLAG_STATIC;
                                l2AddrW.age             = 1;
                                l2AddrW.fid             = fid;
                                switch(UNITTEST_UTIL_CHIP_TYPE)
                                {
                                case RTL9607C_CHIP_ID:
                                    /* RTL9607C has 18 ext port for 3 CPU ports, for each CPU, the ext port maximum is 6 */
                                    if(ext_port >= 6)
                                    {
                                        ext_port = 5;
                                    }
                                    break;
                                default:
                                    break;
                                }

                                l2AddrW.ext_port        = ext_port;
                                 /* Create */
                                if((retVal = rtk_l2_addr_add(&l2AddrW)) != RT_ERR_OK)
                                {
                                    osal_printf("\n %s %d 0x%X\n",__FUNCTION__,__LINE__, retVal);
                                    osal_printf("vid %d port %d fid %d ext_port %d\n", vid, port, fid, ext_port);
                                    return RT_ERR_FAILED;
                                }

                                /* Get */
                                osal_memset(&l2AddrR, 0x00, sizeof(rtk_l2_ucastAddr_t));
                                l2AddrR.vid             = vid;
                                l2AddrR.mac.octet[5]    = mac;
                                l2AddrR.fid             = fid;
                                l2AddrR.flags           = flags | RTK_L2_UCAST_FLAG_STATIC;
                                if((retVal = rtk_l2_addr_get(&l2AddrR)) != RT_ERR_OK)
                                {
                                    osal_printf("\n %s %d 0x%X\n",__FUNCTION__,__LINE__, retVal);
                                    return RT_ERR_FAILED;
                                }

                                if(flags & RTK_L2_UCAST_FLAG_IVL)
                                    l2AddrW.fid = l2AddrR.fid;
                                else
                                    l2AddrW.vid = l2AddrR.vid;

                                l2AddrW.index = l2AddrR.index;

                                if(osal_memcmp(&l2AddrW, &l2AddrR, sizeof(rtk_l2_ucastAddr_t)) != 0)
                                {
                                    osal_printf("\n l2AddrW.vid: %d\n",l2AddrW.vid);
                                    osal_printf("\n l2AddrW.fid: %d\n",l2AddrW.fid);
                                    osal_printf("\n l2AddrW.port: %d\n",l2AddrW.port);
                                    osal_printf("\n l2AddrW.flags: %d\n",l2AddrW.flags);
                                    osal_printf("\n l2AddrW.index: %d\n",l2AddrW.index);
                                    
                                    osal_printf("\n l2AddrR.vid: %d\n",l2AddrR.vid);
                                    osal_printf("\n l2AddrR.fid: %d\n",l2AddrR.fid);
                                    osal_printf("\n l2AddrR.port: %d\n",l2AddrR.port);
                                    osal_printf("\n l2AddrR.flags: %d\n",l2AddrR.flags);
                                    osal_printf("\n l2AddrR.index: %d\n",l2AddrR.index);
                                    
                                    osal_printf("\n %s %d 0x%X\n",__FUNCTION__,__LINE__, retVal);
                                    return RT_ERR_FAILED;
                                }

                                /* Get Next */
                                index = 0;
                                if((retVal = rtk_l2_nextValidAddr_get(&index, &l2AddrR)) != RT_ERR_OK)
                                {
                                    osal_printf("\n %s %d 0x%X\n",__FUNCTION__,__LINE__, retVal);
                                    return RT_ERR_FAILED;
                                }

                                if(osal_memcmp(&l2AddrW, &l2AddrR, sizeof(rtk_l2_ucastAddr_t)) != 0)
                                {
                                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                                    return RT_ERR_FAILED;
                                }

                                /* Get Next on port*/
                                index = 0;
                                if((retVal = rtk_l2_nextValidAddrOnPort_get(port, &index, &l2AddrR)) != RT_ERR_OK)
                                {
                                    osal_printf("\n %s %d 0x%X\n",__FUNCTION__,__LINE__, retVal);
                                    return RT_ERR_FAILED;
                                }

                                if(osal_memcmp(&l2AddrW, &l2AddrR, sizeof(rtk_l2_ucastAddr_t)) != 0)
                                {
                                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                                    return RT_ERR_FAILED;
                                }

                                /* Delete */
                                if((retVal = rtk_l2_addr_del(&l2AddrR)) != RT_ERR_OK)
                                {
                                    osal_printf("\n %s %d 0x%X\n",__FUNCTION__,__LINE__, retVal);
                                    return RT_ERR_FAILED;
                                }

                                /* Confirm the entry is deleted */
                                osal_memset(&l2AddrR, 0x00, sizeof(rtk_l2_ucastAddr_t));
                                l2AddrR.vid             = vid;
                                l2AddrR.mac.octet[5]    = mac;
                                l2AddrR.fid             = fid;
                                l2AddrR.flags           = flags | RTK_L2_UCAST_FLAG_STATIC;
                                if(rtk_l2_addr_get(&l2AddrR) == RT_ERR_OK)
                                {
                                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                                    return RT_ERR_FAILED;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return RT_ERR_OK;
}

static int32 _dal_apollomp_mcastAddr(void)
{
    uint16          vid;
    uint32          mac;
    uint32          fid;
    rtk_portmask_t  portmask;
    rtk_portmask_t  ext_portmask;
    uint32          flags;
    uint32          priority;

    int32           index;

    rtk_l2_mcastAddr_t  l2AddrW;
    rtk_l2_mcastAddr_t  l2AddrR;

    /* Get/set */
    for(vid = 1; vid <= RTK_VLAN_ID_MAX; vid+=4094)
    {
        for(mac = 0x01; mac <= 0xFF; mac+=0xFE)
        {
            for(fid = 0; fid <= HAL_VLAN_FID_MAX(); fid+=HAL_VLAN_FID_MAX())
            {
                for(portmask.bits[0] = 0x1; HAL_IS_PORTMASK_VALID(portmask); portmask.bits[0] = portmask.bits[0] << 1)
                {
                    for(ext_portmask.bits[0] = 0x1; HAL_IS_EXTPORTMASK_VALID(ext_portmask); ext_portmask.bits[0] = ext_portmask.bits[0] << 1)
                    {
                        if(!RTK_PORTMASK_IS_PORT_SET(ext_portmask, HAL_GET_EXT_CPU_PORT()))
                        {
                            for(flags = RTK_L2_MCAST_FLAG_FWD_PRI; flags <= RTK_L2_MCAST_FLAG_ALL; flags = (flags << 1))
                            {
                                for(priority = 0; priority <= HAL_INTERNAL_PRIORITY_MAX(); priority+=HAL_INTERNAL_PRIORITY_MAX())
                                {
                                    /* Create */
                                    osal_memset(&l2AddrW, 0x00, sizeof(rtk_l2_mcastAddr_t));
                                    l2AddrW.vid = vid;
                                    l2AddrW.mac.octet[0] = 0x01;
                                    l2AddrW.mac.octet[5] = mac;
                                    l2AddrW.fid = fid;
                                    RTK_PORTMASK_ASSIGN(l2AddrW.portmask, portmask);
                                    RTK_PORTMASK_ASSIGN(l2AddrW.ext_portmask, ext_portmask);
                                    l2AddrW.flags = flags;
                                    l2AddrW.priority = priority;
                                    if(rtk_l2_mcastAddr_add(&l2AddrW) != RT_ERR_OK)
                                    {
                                        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                                        return RT_ERR_FAILED;
                                    }

                                    /* Get */
                                    osal_memset(&l2AddrR, 0x00, sizeof(rtk_l2_mcastAddr_t));
                                    l2AddrR.vid = vid;
                                    l2AddrR.mac.octet[0] = 0x01;
                                    l2AddrR.mac.octet[5] = mac;
                                    l2AddrR.fid = fid;
                                    l2AddrR.flags = flags;
                                    if(rtk_l2_mcastAddr_get(&l2AddrR) != RT_ERR_OK)
                                    {
                                        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                                        return RT_ERR_FAILED;
                                    }

                                    if(flags & RTK_L2_MCAST_FLAG_IVL)
                                        l2AddrW.fid = l2AddrR.fid;
                                    else
                                        l2AddrW.vid = l2AddrR.vid;

                                    l2AddrW.index = l2AddrR.index;

                                    if(osal_memcmp(&l2AddrW, &l2AddrR, sizeof(rtk_l2_mcastAddr_t)) != 0)
                                    {
                                        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                                        return RT_ERR_FAILED;
                                    }

                                    /* Get Next*/
                                    index = 0;
                                    osal_memset(&l2AddrR, 0x00, sizeof(rtk_l2_mcastAddr_t));
                                    if(rtk_l2_nextValidMcastAddr_get(&index, &l2AddrR) != RT_ERR_OK)
                                    {
                                        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                                        return RT_ERR_FAILED;
                                    }

                                    if(osal_memcmp(&l2AddrW, &l2AddrR, sizeof(rtk_l2_mcastAddr_t)) != 0)
                                    {
                                        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                                        return RT_ERR_FAILED;
                                    }

                                    /* Delete */
                                    if(rtk_l2_mcastAddr_del(&l2AddrR) != RT_ERR_OK)
                                    {
                                        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                                        return RT_ERR_FAILED;
                                    }

                                    /* Confirm the entry is deleted or not */
                                    osal_memset(&l2AddrR, 0x00, sizeof(rtk_l2_mcastAddr_t));
                                    l2AddrR.vid = vid;
                                    l2AddrR.mac.octet[0] = 0x01;
                                    l2AddrR.mac.octet[5] = mac;
                                    l2AddrR.fid = fid;
                                    l2AddrR.flags = flags;
                                    if(rtk_l2_mcastAddr_get(&l2AddrR) == RT_ERR_OK)
                                    {
                                        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                                        return RT_ERR_FAILED;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return RT_ERR_OK;
}

static int32 _dal_rtl9607c_mcastAddr(void)
{
    uint16          vid;
    uint32          mac;
    uint32          fid;
    rtk_portmask_t  portmask;
    uint32          ext_idx, ext_idx_max;
    uint32          flags;

    int32           index;

    rtk_l2_mcastAddr_t  l2AddrW;
    rtk_l2_mcastAddr_t  l2AddrR;

    switch(UNITTEST_UTIL_CHIP_TYPE)
    {
    case RTL9607C_CHIP_ID:
        ext_idx_max = 0x1f;
        break;
    case RTL9603CVD_CHIP_ID:
        ext_idx_max = 0x1f;
        break;
    default:
        break;
    }

    /* Get/set */
    for(vid = 1; vid <= RTK_VLAN_ID_MAX; vid+=4094)
    {
        for(mac = 0x01; mac <= 0xFF; mac+=0xFE)
        {
            for(fid = 0; fid <= HAL_VLAN_FID_MAX(); fid+=HAL_VLAN_FID_MAX())
            {
                for(portmask.bits[0] = 0x1; HAL_IS_PORTMASK_VALID(portmask); portmask.bits[0] = portmask.bits[0] << 1)
                {
                    for(ext_idx = 0; ext_idx < ext_idx_max ; ext_idx += ext_idx_max)
                    {
                        for(flags = RTK_L2_MCAST_FLAG_FWD_PRI; flags <= RTK_L2_MCAST_FLAG_ALL; flags = (flags << 1))
                        {
                            if(flags != RTK_L2_MCAST_FLAG_IVL)
                            {
                                continue;
                            }
                            /* Create */
                            osal_memset(&l2AddrW, 0x00, sizeof(rtk_l2_mcastAddr_t));
                            l2AddrW.vid = vid;
                            l2AddrW.mac.octet[0] = 0x01;
                            l2AddrW.mac.octet[5] = mac;
                            l2AddrW.fid = fid;
                            RTK_PORTMASK_ASSIGN(l2AddrW.portmask, portmask);
                            l2AddrW.ext_portmask_idx = ext_idx;
                            l2AddrW.flags = flags;
                            if(rtk_l2_mcastAddr_add(&l2AddrW) != RT_ERR_OK)
                            {
                                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                                return RT_ERR_FAILED;
                            }

                            /* Get */
                            osal_memset(&l2AddrR, 0x00, sizeof(rtk_l2_mcastAddr_t));
                            l2AddrR.vid = vid;
                            l2AddrR.mac.octet[0] = 0x01;
                            l2AddrR.mac.octet[5] = mac;
                            l2AddrR.fid = fid;
                            l2AddrR.ext_portmask_idx = ext_idx;
                            l2AddrR.flags = flags;
                            if(rtk_l2_mcastAddr_get(&l2AddrR) != RT_ERR_OK)
                            {
                                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                                return RT_ERR_FAILED;
                            }

                            if(flags & RTK_L2_MCAST_FLAG_IVL)
                                l2AddrW.fid = l2AddrR.fid;
                            else
                                l2AddrW.vid = l2AddrR.vid;

                            l2AddrW.index = l2AddrR.index;

                            if(osal_memcmp(&l2AddrW, &l2AddrR, sizeof(rtk_l2_mcastAddr_t)) != 0)
                            {
                                osal_printf("\n l2AddrW.vid: %d\n",l2AddrW.vid);
                                osal_printf("\n l2AddrW.fid: %d\n",l2AddrW.fid);
                                osal_printf("\n l2AddrW.port: %d\n",l2AddrW.ext_portmask_idx);
                                osal_printf("\n l2AddrW.flags: %d\n",l2AddrW.flags);
                                osal_printf("\n l2AddrW.index: %d\n",l2AddrW.index);
                                
                                osal_printf("\n l2AddrR.vid: %d\n",l2AddrR.vid);
                                osal_printf("\n l2AddrR.fid: %d\n",l2AddrR.fid);
                                osal_printf("\n l2AddrR.port: %d\n",l2AddrR.ext_portmask_idx);
                                osal_printf("\n l2AddrR.flags: %d\n",l2AddrR.flags);
                                osal_printf("\n l2AddrR.index: %d\n",l2AddrR.index);

                                return RT_ERR_FAILED;
                            }

                            /* Get Next*/
                            index = 0;
                            osal_memset(&l2AddrR, 0x00, sizeof(rtk_l2_mcastAddr_t));
                            if(rtk_l2_nextValidMcastAddr_get(&index, &l2AddrR) != RT_ERR_OK)
                            {
                                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                                return RT_ERR_FAILED;
                            }

                            if(osal_memcmp(&l2AddrW, &l2AddrR, sizeof(rtk_l2_mcastAddr_t)) != 0)
                            {
                                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                                return RT_ERR_FAILED;
                            }

                            /* Delete */
                            if(rtk_l2_mcastAddr_del(&l2AddrR) != RT_ERR_OK)
                            {
                                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                                return RT_ERR_FAILED;
                            }

                            /* Confirm the entry is deleted or not */
                            osal_memset(&l2AddrR, 0x00, sizeof(rtk_l2_mcastAddr_t));
                            l2AddrR.vid = vid;
                            l2AddrR.mac.octet[0] = 0x01;
                            l2AddrR.mac.octet[5] = mac;
                            l2AddrR.fid = fid;
                            l2AddrR.flags = flags;
                            if(rtk_l2_mcastAddr_get(&l2AddrR) == RT_ERR_OK)
                            {
                                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                                return RT_ERR_FAILED;
                            }
                        }
                    }
                }
            }
        }
    }

    return RT_ERR_OK;
}

static int32 _dal_apollomp_ipMcastAddr(void)
{
    ipaddr_t        dip;
    ipaddr_t        sip;
    uint16          vid;
    rtk_portmask_t  portmask;
    rtk_portmask_t  ext_portmask;
    uint32          l3_trans_index;
    uint32          flags;          /* Refer to RTK_L2_IPMCAST_FLAG_XXX */
    uint32          priority;

    int32           index;

    rtk_l2_ipMcastAddr_t ipAddrW;
    rtk_l2_ipMcastAddr_t ipAddrR;

    /* Get/set in DIP Only */
    for(dip = 0xE0000000; dip <= 0xEFFFFFFF; dip+=0x0FFFFFFF)
    {
        for(l3_trans_index = 0; l3_trans_index <= 0x0FFFFFFF; l3_trans_index += 0x0FFFFFFF)
        {
            for(portmask.bits[0] = 0x1; HAL_IS_PORTMASK_VALID(portmask); portmask.bits[0] = portmask.bits[0] << 1)
            {
                for(ext_portmask.bits[0] = 0x1; HAL_IS_EXTPORTMASK_VALID(ext_portmask); ext_portmask.bits[0] = ext_portmask.bits[0] << 1)
                {
                    if(!RTK_PORTMASK_IS_PORT_SET(ext_portmask, HAL_GET_EXT_CPU_PORT()))
                    {
                        for(flags = RTK_L2_IPMCAST_FLAG_FWD_PRI; flags <= RTK_L2_IPMCAST_FLAG_ALL; flags = (flags << 1))
                        {
                            for(priority = 0; priority <= HAL_INTERNAL_PRIORITY_MAX(); priority+=HAL_INTERNAL_PRIORITY_MAX())
                            {
                                /* Create */
                                osal_memset(&ipAddrW, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
                                ipAddrW.dip = dip;
                                RTK_PORTMASK_ASSIGN(ipAddrW.portmask, portmask);
                                RTK_PORTMASK_ASSIGN(ipAddrW.ext_portmask, ext_portmask);
                                ipAddrW.l3_trans_index = l3_trans_index;
                                ipAddrW.flags = flags | RTK_L2_IPMCAST_FLAG_DIP_ONLY | RTK_L2_IPMCAST_FLAG_STATIC;
                                ipAddrW.priority = priority;
                                if(rtk_l2_ipMcastAddr_add(&ipAddrW) != RT_ERR_OK)
                                {
                                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                                    return RT_ERR_FAILED;
                                }

                                /* Get */
                                osal_memset(&ipAddrR, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
                                ipAddrR.dip = dip;
                                ipAddrR.flags = flags | RTK_L2_IPMCAST_FLAG_DIP_ONLY | RTK_L2_IPMCAST_FLAG_STATIC;
                                if(rtk_l2_ipMcastAddr_get(&ipAddrR) != RT_ERR_OK)
                                {

                                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                                    return RT_ERR_FAILED;
                                }

                                ipAddrW.index = ipAddrR.index;
                                if(osal_memcmp(&ipAddrW, &ipAddrR, sizeof(rtk_l2_ipMcastAddr_t)) != 0)
                                {
                                    osal_printf("\n 0x%08X 0x%08X %d 0x%x 0x%x %d 0x%02X %d %d",
                                    ipAddrW.dip,
                                    ipAddrW.sip,
                                    ipAddrW.vid,
                                    ipAddrW.portmask.bits[0],
                                    ipAddrW.ext_portmask.bits[0],
                                    ipAddrW.l3_trans_index,
                                    ipAddrW.flags,
                                    ipAddrW.priority,
                                    ipAddrW.index);
                                    osal_printf("\n 0x%08X 0x%08X %d 0x%x 0x%x %d 0x%02X %d %d",
                                    ipAddrR.dip,
                                    ipAddrR.sip,
                                    ipAddrR.vid,
                                    ipAddrR.portmask.bits[0],
                                    ipAddrR.ext_portmask.bits[0],
                                    ipAddrR.l3_trans_index,
                                    ipAddrR.flags,
                                    ipAddrR.priority,
                                    ipAddrR.index);
                                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                                    return RT_ERR_FAILED;
                                }

                                /* Get next */
                                osal_memset(&ipAddrR, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
                                index = 0;
                                if(rtk_l2_nextValidIpMcastAddr_get(&index, &ipAddrR) != RT_ERR_OK)
                                {
                                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                                    return RT_ERR_FAILED;
                                }

                                if(osal_memcmp(&ipAddrW, &ipAddrR, sizeof(rtk_l2_ipMcastAddr_t)) != 0)
                                {
                                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                                    return RT_ERR_FAILED;
                                }

                                /* Delete */
                                if(rtk_l2_ipMcastAddr_del(&ipAddrR) != RT_ERR_OK)
                                {
                                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                                    return RT_ERR_FAILED;
                                }

                                /* Confirm */
                                osal_memset(&ipAddrR, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
                                ipAddrR.dip = dip;
                                ipAddrR.flags = flags | RTK_L2_IPMCAST_FLAG_DIP_ONLY | RTK_L2_IPMCAST_FLAG_STATIC;
                                if(rtk_l2_ipMcastAddr_get(&ipAddrR) == RT_ERR_OK)
                                {
                                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                                    return RT_ERR_FAILED;
                                }
                            }
                        }
                    }
                }
            }
        }
    }


    /* Get/set in DIP + VID*/
    rtk_l2_ipmcMode_set(LOOKUP_ON_DIP_AND_VID);
    for(dip = 0xE0000000; dip <= 0xEFFFFFFF; dip+=0x0FFFFFFF)
    {
        for(vid = 1; vid <= RTK_VLAN_ID_MAX; vid += 4093)
        {
            for(portmask.bits[0] = 0x1; HAL_IS_PORTMASK_VALID(portmask); portmask.bits[0] = portmask.bits[0] << 1)
            {
                for(ext_portmask.bits[0] = 0x1; HAL_IS_EXTPORTMASK_VALID(ext_portmask); portmask.bits[0] = ext_portmask.bits[0] << 1)
                {
                    if(!RTK_PORTMASK_IS_PORT_SET(ext_portmask, HAL_GET_EXT_CPU_PORT()))
                    {
                        for(flags = 0; flags <= RTK_L2_IPMCAST_FLAG_FWD_PRI; flags++) /* Only one flag */
                        {
                            for(priority = 0; priority <= HAL_INTERNAL_PRIORITY_MAX(); priority+=HAL_INTERNAL_PRIORITY_MAX())
                            {
                                /* Create */
                                osal_memset(&ipAddrW, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
                                ipAddrW.dip = dip;
                                ipAddrW.vid = vid;
                                RTK_PORTMASK_ASSIGN(ipAddrW.portmask, portmask);
                                RTK_PORTMASK_ASSIGN(ipAddrW.ext_portmask, ext_portmask);
                                ipAddrW.flags = flags | RTK_L2_IPMCAST_FLAG_STATIC;
                                ipAddrW.priority = priority;
                                if(rtk_l2_ipMcastAddr_add(&ipAddrW) != RT_ERR_OK)
                                {
                                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                                    return RT_ERR_FAILED;
                                }

                                /* Get */
                                osal_memset(&ipAddrR, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
                                ipAddrR.dip = dip;
                                ipAddrR.vid = vid;
                                ipAddrR.flags = flags | RTK_L2_IPMCAST_FLAG_STATIC;
                                if(rtk_l2_ipMcastAddr_get(&ipAddrR) != RT_ERR_OK)
                                {
                                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                                    return RT_ERR_FAILED;
                                }

                                ipAddrW.index = ipAddrR.index;
                                if(osal_memcmp(&ipAddrW, &ipAddrR, sizeof(rtk_l2_ipMcastAddr_t)) != 0)
                                {
                                    osal_printf("\n 0x%08X 0x%08X %d 0x%x 0x%x %d 0x%02X %d %d",
                                    ipAddrW.dip,
                                    ipAddrW.sip,
                                    ipAddrW.vid,
                                    ipAddrW.portmask.bits[0],
                                    ipAddrW.ext_portmask.bits[0],
                                    ipAddrW.l3_trans_index,
                                    ipAddrW.flags,
                                    ipAddrW.priority,
                                    ipAddrW.index);
                                    osal_printf("\n 0x%08X 0x%08X %d 0x%x 0x%x %d 0x%02X %d %d",
                                    ipAddrR.dip,
                                    ipAddrR.sip,
                                    ipAddrR.vid,
                                    ipAddrR.portmask.bits[0],
                                    ipAddrR.ext_portmask.bits[0],
                                    ipAddrR.l3_trans_index,
                                    ipAddrR.flags,
                                    ipAddrR.priority,
                                    ipAddrR.index);
                                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                                    return RT_ERR_FAILED;
                                }

                                /* Get next */
                                osal_memset(&ipAddrR, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
                                index = 0;
                                if(rtk_l2_nextValidIpMcastAddr_get(&index, &ipAddrR) != RT_ERR_OK)
                                {
                                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                                    return RT_ERR_FAILED;
                                }

                                if(osal_memcmp(&ipAddrW, &ipAddrR, sizeof(rtk_l2_ipMcastAddr_t)) != 0)
                                {
                                    osal_printf("\n 0x%08X 0x%08X %d 0x%x 0x%x %d 0x%02X %d %d",
                                    ipAddrW.dip,
                                    ipAddrW.sip,
                                    ipAddrW.vid,
                                    ipAddrW.portmask.bits[0],
                                    ipAddrW.ext_portmask.bits[0],
                                    ipAddrW.l3_trans_index,
                                    ipAddrW.flags,
                                    ipAddrW.priority,
                                    ipAddrW.index);
                                    osal_printf("\n 0x%08X 0x%08X %d 0x%x 0x%x %d 0x%02X %d %d",
                                    ipAddrR.dip,
                                    ipAddrR.sip,
                                    ipAddrR.vid,
                                    ipAddrR.portmask.bits[0],
                                    ipAddrR.ext_portmask.bits[0],
                                    ipAddrR.l3_trans_index,
                                    ipAddrR.flags,
                                    ipAddrR.priority,
                                    ipAddrR.index);
                                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                                    return RT_ERR_FAILED;
                                }

                                /* Delete */
                                if(rtk_l2_ipMcastAddr_del(&ipAddrR) != RT_ERR_OK)
                                {
                                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                                    return RT_ERR_FAILED;
                                }

                                /* Confirm */
                                osal_memset(&ipAddrR, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
                                ipAddrR.dip = dip;
                                ipAddrR.vid = vid;
                                ipAddrR.flags = flags | RTK_L2_IPMCAST_FLAG_STATIC;
                                if(rtk_l2_ipMcastAddr_get(&ipAddrR) == RT_ERR_OK)
                                {
                                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                                    return RT_ERR_FAILED;
                                }
                            }
                        }
                    }
                }
            }
        }
    }


    /* Get/set in DIP + SIP*/
    rtk_l2_ipmcMode_set(LOOKUP_ON_DIP_AND_SIP);
     for(dip = 0xE0000000; dip <= 0xEFFFFFFF; dip+=0x0FFFFFFF)
    {
        for(sip = 0x00000001; sip <= 0xA0000001; sip += 0xA0000000)
        {
            for(portmask.bits[0] = 0x1; HAL_IS_PORTMASK_VALID(portmask); portmask.bits[0] = portmask.bits[0] << 1)
            {
                for(ext_portmask.bits[0] = 0x1; HAL_IS_EXTPORTMASK_VALID(ext_portmask); portmask.bits[0] = ext_portmask.bits[0] << 1)
                {
                    if(!RTK_PORTMASK_IS_PORT_SET(ext_portmask, HAL_GET_EXT_CPU_PORT()))
                    {
                        for(flags = 0; flags <= RTK_L2_IPMCAST_FLAG_FWD_PRI; flags++) /* Only one flag */
                        {
                            for(priority = 0; priority <= HAL_INTERNAL_PRIORITY_MAX(); priority+=HAL_INTERNAL_PRIORITY_MAX())
                            {
                                /* Create */
                                osal_memset(&ipAddrW, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
                                ipAddrW.dip = dip;
                                ipAddrW.sip = sip;
                                RTK_PORTMASK_ASSIGN(ipAddrW.portmask, portmask);
                                RTK_PORTMASK_ASSIGN(ipAddrW.ext_portmask, ext_portmask);
                                ipAddrW.flags = flags | RTK_L2_IPMCAST_FLAG_STATIC;
                                ipAddrW.priority = priority;
                                if(rtk_l2_ipMcastAddr_add(&ipAddrW) != RT_ERR_OK)
                                {
                                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                                    return RT_ERR_FAILED;
                                }

                                /* Get */
                                osal_memset(&ipAddrR, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
                                ipAddrR.dip = dip;
                                ipAddrR.sip = sip;
                                ipAddrR.flags = flags | RTK_L2_IPMCAST_FLAG_STATIC;
                                if(rtk_l2_ipMcastAddr_get(&ipAddrR) != RT_ERR_OK)
                                {
                                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                                    return RT_ERR_FAILED;
                                }

                                ipAddrW.index = ipAddrR.index;
                                if(osal_memcmp(&ipAddrW, &ipAddrR, sizeof(rtk_l2_ipMcastAddr_t)) != 0)
                                {
                                    osal_printf("\n 0x%08X 0x%08X %d 0x%x 0x%x %d 0x%02X %d %d",
                                    ipAddrW.dip,
                                    ipAddrW.sip,
                                    ipAddrW.vid,
                                    ipAddrW.portmask.bits[0],
                                    ipAddrW.ext_portmask.bits[0],
                                    ipAddrW.l3_trans_index,
                                    ipAddrW.flags,
                                    ipAddrW.priority,
                                    ipAddrW.index);
                                    osal_printf("\n 0x%08X 0x%08X %d 0x%x 0x%x %d 0x%02X %d %d",
                                    ipAddrR.dip,
                                    ipAddrR.sip,
                                    ipAddrR.vid,
                                    ipAddrR.portmask.bits[0],
                                    ipAddrR.ext_portmask.bits[0],
                                    ipAddrR.l3_trans_index,
                                    ipAddrR.flags,
                                    ipAddrR.priority,
                                    ipAddrR.index);
                                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                                    return RT_ERR_FAILED;
                                }

                                /* Get next */
                                osal_memset(&ipAddrR, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
                                index = 0;
                                if(rtk_l2_nextValidIpMcastAddr_get(&index, &ipAddrR) != RT_ERR_OK)
                                {
                                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                                    return RT_ERR_FAILED;
                                }

                                if(osal_memcmp(&ipAddrW, &ipAddrR, sizeof(rtk_l2_ipMcastAddr_t)) != 0)
                                {
                                    osal_printf("\n 0x%08X 0x%08X %d 0x%x 0x%x %d 0x%02X %d %d",
                                    ipAddrW.dip,
                                    ipAddrW.sip,
                                    ipAddrW.vid,
                                    ipAddrW.portmask.bits[0],
                                    ipAddrW.ext_portmask.bits[0],
                                    ipAddrW.l3_trans_index,
                                    ipAddrW.flags,
                                    ipAddrW.priority,
                                    ipAddrW.index);
                                    osal_printf("\n 0x%08X 0x%08X %d 0x%x 0x%x %d 0x%02X %d %d",
                                    ipAddrR.dip,
                                    ipAddrR.sip,
                                    ipAddrR.vid,
                                    ipAddrR.portmask.bits[0],
                                    ipAddrR.ext_portmask.bits[0],
                                    ipAddrR.l3_trans_index,
                                    ipAddrR.flags,
                                    ipAddrR.priority,
                                    ipAddrR.index);
                                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                                    return RT_ERR_FAILED;
                                }

                                /* Delete */
                                if(rtk_l2_ipMcastAddr_del(&ipAddrR) != RT_ERR_OK)
                                {
                                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                                    return RT_ERR_FAILED;
                                }

                                /* Confirm */
                                osal_memset(&ipAddrR, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
                                ipAddrR.dip = dip;
                                ipAddrR.sip = sip;
                                ipAddrR.flags = flags | RTK_L2_IPMCAST_FLAG_STATIC;
                                if(rtk_l2_ipMcastAddr_get(&ipAddrR) == RT_ERR_OK)
                                {
                                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                                    return RT_ERR_FAILED;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return RT_ERR_OK;
}

static int32 _dal_rtl9607c_ipMcastAddr(void)
{
    ipaddr_t        dip;
    ipaddr_t        sip;
    uint16          vid;
    rtk_portmask_t  portmask;
    uint32          ext_idx, ext_idx_max;
    uint32          flags;          /* Refer to RTK_L2_IPMCAST_FLAG_XXX */

    int32           index;

    rtk_l2_ipMcastAddr_t ipAddrW;
    rtk_l2_ipMcastAddr_t ipAddrR;

    switch(UNITTEST_UTIL_CHIP_TYPE)
    {
    case RTL9607C_CHIP_ID:
        ext_idx_max = 0x1f;
        break;
    case RTL9603CVD_CHIP_ID:
        ext_idx_max = 0x1f;
        break;
    default:
        break;
    }

    /* Get/set in DIP + VID/FID */
    rtk_l2_ipmcMode_set(LOOKUP_ON_DIP_AND_VID_FID);
    for(dip = 0xE0000000; dip <= 0xEFFFFFFF; dip+=0x0FFFFFFF)
    {
        for(vid = 1; vid <= RTK_VLAN_ID_MAX; vid += 4093)
        {
            for(portmask.bits[0] = 0x1; HAL_IS_PORTMASK_VALID(portmask); portmask.bits[0] = portmask.bits[0] << 1)
            {
                for(ext_idx = 0; ext_idx < ext_idx_max ; ext_idx += ext_idx_max)
                {
                    for(flags = 0; flags <= RTK_L2_IPMCAST_FLAG_FWD_PRI; flags++) /* Only one flag */
                    {
                        if((flags != RTK_L2_IPMCAST_FLAG_STATIC))
                        {
                            continue;
                        }
                        /* Create */
                        osal_memset(&ipAddrW, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
                        ipAddrW.dip = dip;
                        ipAddrW.vid = vid;
                        RTK_PORTMASK_ASSIGN(ipAddrW.portmask, portmask);
                        ipAddrW.ext_portmask_idx = ext_idx;
                        ipAddrW.flags = flags | RTK_L2_IPMCAST_FLAG_STATIC;
                        if(rtk_l2_ipMcastAddr_add(&ipAddrW) != RT_ERR_OK)
                        {
                            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                            return RT_ERR_FAILED;
                        }

                        /* Get */
                        osal_memset(&ipAddrR, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
                        ipAddrR.dip = dip;
                        ipAddrR.vid = vid;
                        ipAddrR.flags = flags | RTK_L2_IPMCAST_FLAG_STATIC;
                        if(rtk_l2_ipMcastAddr_get(&ipAddrR) != RT_ERR_OK)
                        {
                            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                            return RT_ERR_FAILED;
                        }

                        ipAddrW.index = ipAddrR.index;
                        if(osal_memcmp(&ipAddrW, &ipAddrR, sizeof(rtk_l2_ipMcastAddr_t)) != 0)
                        {
                            osal_printf("\n 0x%08X 0x%08X %d 0x%x %d 0x%02X %d",
                            ipAddrW.dip,
                            ipAddrW.sip,
                            ipAddrW.vid,
                            ipAddrW.portmask.bits[0],
                            ipAddrW.ext_portmask_idx,
                            ipAddrW.flags,
                            ipAddrW.index);
                            osal_printf("\n 0x%08X 0x%08X %d 0x%x %d 0x%02X %d",
                            ipAddrR.dip,
                            ipAddrR.sip,
                            ipAddrR.vid,
                            ipAddrR.portmask.bits[0],
                            ipAddrR.ext_portmask_idx,
                            ipAddrR.flags,
                            ipAddrR.index);
                            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                            return RT_ERR_FAILED;
                        }

                        /* Get next */
                        osal_memset(&ipAddrR, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
                        index = 0;
                        if(rtk_l2_nextValidIpMcastAddr_get(&index, &ipAddrR) != RT_ERR_OK)
                        {
                            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                            return RT_ERR_FAILED;
                        }

                        if(osal_memcmp(&ipAddrW, &ipAddrR, sizeof(rtk_l2_ipMcastAddr_t)) != 0)
                        {
                            osal_printf("\n 0x%08X 0x%08X %d 0x%x %d 0x%02X %d",
                            ipAddrW.dip,
                            ipAddrW.sip,
                            ipAddrW.vid,
                            ipAddrW.portmask.bits[0],
                            ipAddrW.ext_portmask_idx,
                            ipAddrW.flags,
                            ipAddrW.index);
                            osal_printf("\n 0x%08X 0x%08X %d 0x%x %d 0x%02X %d",
                            ipAddrR.dip,
                            ipAddrR.sip,
                            ipAddrR.vid,
                            ipAddrR.portmask.bits[0],
                            ipAddrR.ext_portmask_idx,
                            ipAddrR.flags,
                            ipAddrR.index);
                            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                            return RT_ERR_FAILED;
                        }

                        /* Delete */
                        if(rtk_l2_ipMcastAddr_del(&ipAddrR) != RT_ERR_OK)
                        {
                            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                            return RT_ERR_FAILED;
                        }

                        /* Confirm */
                        osal_memset(&ipAddrR, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
                        ipAddrR.dip = dip;
                        ipAddrR.vid = vid;
                        ipAddrR.flags = flags | RTK_L2_IPMCAST_FLAG_STATIC;
                        if(rtk_l2_ipMcastAddr_get(&ipAddrR) == RT_ERR_OK)
                        {
                            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                            return RT_ERR_FAILED;
                        }
                    }
                }
            }
        }
    }


    /* Get/set in DIP + SIP*/
    rtk_l2_ipmcMode_set(LOOKUP_ON_DIP_AND_SIP);
     for(dip = 0xE0000000; dip <= 0xEFFFFFFF; dip+=0x0FFFFFFF)
    {
        for(sip = 0x00000001; sip <= 0xA0000001; sip += 0xA0000000)
        {
            for(portmask.bits[0] = 0x1; HAL_IS_PORTMASK_VALID(portmask); portmask.bits[0] = portmask.bits[0] << 1)
            {
                for(ext_idx = 0; ext_idx < ext_idx_max ; ext_idx += ext_idx_max)
                {
                    for(flags = 0; flags <= RTK_L2_IPMCAST_FLAG_FWD_PRI; flags++) /* Only one flag */
                    {
                        if((flags != RTK_L2_IPMCAST_FLAG_STATIC))
                        {
                            continue;
                        }
                        /* Create */
                        osal_memset(&ipAddrW, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
                        ipAddrW.dip = dip;
                        ipAddrW.sip = sip;
                        RTK_PORTMASK_ASSIGN(ipAddrW.portmask, portmask);
                        ipAddrW.ext_portmask_idx = ext_idx;
                        ipAddrW.flags = flags | RTK_L2_IPMCAST_FLAG_STATIC;
                        if(rtk_l2_ipMcastAddr_add(&ipAddrW) != RT_ERR_OK)
                        {
                            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                            return RT_ERR_FAILED;
                        }

                        /* Get */
                        osal_memset(&ipAddrR, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
                        ipAddrR.dip = dip;
                        ipAddrR.sip = sip;
                        ipAddrR.flags = flags | RTK_L2_IPMCAST_FLAG_STATIC;
                        if(rtk_l2_ipMcastAddr_get(&ipAddrR) != RT_ERR_OK)
                        {
                            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                            return RT_ERR_FAILED;
                        }

                        ipAddrW.index = ipAddrR.index;
                        if(osal_memcmp(&ipAddrW, &ipAddrR, sizeof(rtk_l2_ipMcastAddr_t)) != 0)
                        {
                            osal_printf("\n 0x%08X 0x%08X %d 0x%x %d 0x%02X %d",
                            ipAddrW.dip,
                            ipAddrW.sip,
                            ipAddrW.vid,
                            ipAddrW.portmask.bits[0],
                            ipAddrW.ext_portmask_idx,
                            ipAddrW.flags,
                            ipAddrW.index);
                            osal_printf("\n 0x%08X 0x%08X %d 0x%x %d 0x%02X %d",
                            ipAddrR.dip,
                            ipAddrR.sip,
                            ipAddrR.vid,
                            ipAddrR.portmask.bits[0],
                            ipAddrR.ext_portmask_idx,
                            ipAddrR.flags,
                            ipAddrR.index);
                            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                            return RT_ERR_FAILED;
                        }

                        /* Get next */
                        osal_memset(&ipAddrR, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
                        index = 0;
                        if(rtk_l2_nextValidIpMcastAddr_get(&index, &ipAddrR) != RT_ERR_OK)
                        {
                            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                            return RT_ERR_FAILED;
                        }

                        if(osal_memcmp(&ipAddrW, &ipAddrR, sizeof(rtk_l2_ipMcastAddr_t)) != 0)
                        {
                            osal_printf("\n 0x%08X 0x%08X %d 0x%x %d 0x%02X %d",
                            ipAddrW.dip,
                            ipAddrW.sip,
                            ipAddrW.vid,
                            ipAddrW.portmask.bits[0],
                            ipAddrW.ext_portmask_idx,
                            ipAddrW.flags,
                            ipAddrW.index);
                            osal_printf("\n 0x%08X 0x%08X %d 0x%x %d 0x%02X %d",
                            ipAddrR.dip,
                            ipAddrR.sip,
                            ipAddrR.vid,
                            ipAddrR.portmask.bits[0],
                            ipAddrR.ext_portmask_idx,
                            ipAddrR.flags,
                            ipAddrR.index);
                            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                            return RT_ERR_FAILED;
                        }

                        /* Delete */
                        if(rtk_l2_ipMcastAddr_del(&ipAddrR) != RT_ERR_OK)
                        {
                            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                            return RT_ERR_FAILED;
                        }

                        /* Confirm */
                        osal_memset(&ipAddrR, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
                        ipAddrR.dip = dip;
                        ipAddrR.sip = sip;
                        ipAddrR.flags = flags | RTK_L2_IPMCAST_FLAG_STATIC;
                        if(rtk_l2_ipMcastAddr_get(&ipAddrR) == RT_ERR_OK)
                        {
                            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                            return RT_ERR_FAILED;
                        }
                    }
                }
            }
        }
    }

    return RT_ERR_OK;
}
int32 dal_l2_age_test(uint32 caseNo)
{
    uint32 timeW;
    uint32 timeR;

    /*error input check*/
    /*out of range*/
    if(rtk_l2_aging_set(HAL_L2_AGING_TIME_MAX() + 1) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*null pointer*/
    if(rtk_l2_aging_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* Get/set */
    for(timeW = 100; timeW <= HAL_L2_AGING_TIME_MAX(); timeW+=100)
    {
        if(rtk_l2_aging_set(timeW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if(rtk_l2_aging_get(&timeR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if(timeW != timeR)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }

    return RT_ERR_OK;
}

int32 dal_l2_extPortEgrFilterMask_test(uint32 caseNo)
{
    rtk_portmask_t portmaskW;
    rtk_portmask_t portmaskR;


    portmaskW.bits[0] = 0x0;
    if(rtk_l2_extPortEgrFilterMask_set(&portmaskW) == RT_ERR_DRIVER_NOT_FOUND)
    {
        return RT_ERR_OK;
    }

    /*error input check*/
    /*out of range*/
    portmaskW.bits[0] = 0xFFFFFFFF;
    if(rtk_l2_extPortEgrFilterMask_set(&portmaskW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*null pointer*/
    if(rtk_l2_extPortEgrFilterMask_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* Get/set */
    for(portmaskW.bits[0] = 0x00; HAL_IS_EXTPORTMASK_VALID(portmaskW); portmaskW.bits[0]++)
    {
        if(!RTK_PORTMASK_IS_PORT_SET(portmaskW, HAL_GET_EXT_CPU_PORT()))
        {
            if(rtk_l2_extPortEgrFilterMask_set(&portmaskW) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            if(rtk_l2_extPortEgrFilterMask_get(&portmaskR) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            if(RTK_PORTMASK_COMPARE(portmaskW, portmaskR) != 0)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
        }
    }

    return RT_ERR_OK;
}

int32 dal_l2_flushLinkDownPortAddrEnable_test(uint32 caseNo)
{
    rtk_enable_t stateW;
    rtk_enable_t stateR;

    /*error input check*/
    /*out of range*/
    if(rtk_l2_flushLinkDownPortAddrEnable_set(RTK_ENABLE_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*null pointer*/
    if(rtk_l2_flushLinkDownPortAddrEnable_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    for(stateW = DISABLED; stateW < RTK_ENABLE_END; stateW++)
    {
        if(rtk_l2_flushLinkDownPortAddrEnable_set(stateW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if(rtk_l2_flushLinkDownPortAddrEnable_get(&stateR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if(stateW != stateR)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }

    /* Get/set */
    return RT_ERR_OK;
}

int32 dal_l2_illegalPortMoveAction_test(uint32 caseNo)
{
    rtk_port_t port;
    rtk_action_t actW;
    rtk_action_t actR;

    if(rtk_l2_illegalPortMoveAction_set(HAL_GET_MAX_PORT(), ACTION_FORWARD) == RT_ERR_DRIVER_NOT_FOUND)
    {
        return RT_ERR_OK;
    }
    
    /*error input check*/
    /*out of range*/
    if(rtk_l2_illegalPortMoveAction_set(HAL_GET_MAX_PORT() + 1, ACTION_FORWARD) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if(rtk_l2_illegalPortMoveAction_set(0, ACTION_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*null pointer*/
    if(rtk_l2_illegalPortMoveAction_get(0, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* Get/set */
    HAL_SCAN_ALL_PORT(port)
    {
        for(actW = ACTION_FORWARD; actW < ACTION_END; actW++)
        {
            switch(UNITTEST_UTIL_CHIP_TYPE)
            {
            case APOLLOMP_CHIP_ID:
                break;
            case RTL9601B_CHIP_ID:
                switch(actW)
                {
                default:
                    continue;
                }
                break;
            case RTL9602C_CHIP_ID:
            case RTL9607C_CHIP_ID:
            case RTL9603CVD_CHIP_ID:
                switch(actW)
                {
                case ACTION_FORWARD:
                case ACTION_DROP:
                case ACTION_TRAP2CPU:
                    break;
                default:
                    continue;
                }
                break;
            default:
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            if(rtk_l2_illegalPortMoveAction_set(port, actW) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            if(rtk_l2_illegalPortMoveAction_get(port, &actR) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            if(actW != actR)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
        }
    }


    return RT_ERR_OK;
}

int32 dal_l2_portAgingEnable_test(uint32 caseNo)
{
    rtk_port_t port;
    rtk_enable_t stateW;
    rtk_enable_t stateR;

    /*error input check*/
    /*out of range*/
    if(rtk_l2_portAgingEnable_set(HAL_GET_MAX_PORT() + 1, ENABLED) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if(rtk_l2_portAgingEnable_set(0, RTK_ENABLE_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*null pointer*/
    if(rtk_l2_portAgingEnable_get(0, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* Get/set */
    HAL_SCAN_ALL_PORT(port)
    {
        for(stateW = DISABLED; stateW < RTK_ENABLE_END; stateW++)
        {
            if(rtk_l2_portAgingEnable_set(port, stateW) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            if(rtk_l2_portAgingEnable_get(port, &stateR) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            if(stateW != stateR)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
        }
    }

    return RT_ERR_OK;
}

int32 dal_l2_ipmcGroupLookupMissHash_test(uint32 caseNo)
{
    rtk_l2_ipmcHashOp_t hashW;
    rtk_l2_ipmcHashOp_t hashR;

    if(rtk_l2_ipmcGroupLookupMissHash_set(HASH_DIP_AND_SIP) == RT_ERR_DRIVER_NOT_FOUND)
    {
        return RT_ERR_OK;
    }
    
    /*error input check*/
    /*out of range*/
    if(rtk_l2_ipmcGroupLookupMissHash_set(HASH_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*null pointer*/
    if(rtk_l2_ipmcGroupLookupMissHash_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* Get/set */
    for(hashW = HASH_DIP_AND_SIP; hashW < HASH_END; hashW++)
    {
        if(rtk_l2_ipmcGroupLookupMissHash_set(hashW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if(rtk_l2_ipmcGroupLookupMissHash_get(&hashR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if(hashW != hashR)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }

    return RT_ERR_OK;
}

int32 dal_l2_ipmcMode_test(uint32 caseNo)
{
    rtk_l2_ipmcMode_t modeW;
    rtk_l2_ipmcMode_t modeR;
    /*error input check*/
    /*out of range*/
    for(modeW = LOOKUP_ON_MAC_AND_VID_FID; modeW < IPMC_MODE_END; modeW ++)
    {
        switch(UNITTEST_UTIL_CHIP_TYPE)
        {
        case APOLLOMP_CHIP_ID:
            switch(modeW)
            {
            case LOOKUP_ON_MAC_AND_VID_FID:
            case LOOKUP_ON_DIP_AND_SIP:
            case LOOKUP_ON_DIP_AND_VID:
                /* Support modes */
                continue;
            default:
                break;
            }
            break;
        case RTL9601B_CHIP_ID:
            switch(modeW)
            {
            case LOOKUP_ON_MAC_AND_VID_FID:
            case LOOKUP_ON_DIP:
            case LOOKUP_ON_DIP_AND_VID:
            case LOOKUP_ON_DIP_AND_CVID:
                /* Support modes */
                continue;
            default:
                break;
            }
            break;
        case RTL9602C_CHIP_ID:
            switch(modeW)
            {
            case LOOKUP_ON_MAC_AND_VID_FID:
            case LOOKUP_ON_DIP_AND_VID_FID:
                /* Support modes */
                continue;
            default:
                break;
            }
            break;
        case RTL9607C_CHIP_ID:
        case RTL9603CVD_CHIP_ID:
            switch(modeW)
            {
            case LOOKUP_ON_MAC_AND_VID_FID:
            case LOOKUP_ON_DIP_AND_VID_FID:
            case LOOKUP_ON_DIP_AND_SIP:
                /* Support modes */
                continue;
            default:
                break;
            }
            break;
        default:
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        if(rtk_l2_ipmcMode_set(modeW) == RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }

    /*null pointer*/
    if(rtk_l2_ipmcMode_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* Get/set */
    for(modeW = LOOKUP_ON_MAC_AND_VID_FID; modeW < IPMC_MODE_END; modeW++)
    {
        switch(UNITTEST_UTIL_CHIP_TYPE)
        {
        case APOLLOMP_CHIP_ID:
            switch(modeW)
            {
            case LOOKUP_ON_MAC_AND_VID_FID:
            case LOOKUP_ON_DIP_AND_SIP:
            case LOOKUP_ON_DIP_AND_VID:
                /* Support modes */
                break;
            default:
                continue;
            }
            break;
        case RTL9601B_CHIP_ID:
            switch(modeW)
            {
            case LOOKUP_ON_MAC_AND_VID_FID:
            case LOOKUP_ON_DIP:
            case LOOKUP_ON_DIP_AND_VID:
            case LOOKUP_ON_DIP_AND_CVID:
                /* Support modes */
                break;;
            default:
                continue;
            }
            break;
        case RTL9602C_CHIP_ID:
            switch(modeW)
            {
            case LOOKUP_ON_MAC_AND_VID_FID:
            case LOOKUP_ON_DIP_AND_VID_FID:
                /* Support modes */
                break;;
            default:
                continue;
            }
            break;
        case RTL9607C_CHIP_ID:
        case RTL9603CVD_CHIP_ID:
            switch(modeW)
            {
            case LOOKUP_ON_MAC_AND_VID_FID:
            case LOOKUP_ON_DIP_AND_VID_FID:
            case LOOKUP_ON_DIP_AND_SIP:
                /* Support modes */
                break;
            default:
                continue;
            }
            break;
        default:
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        if(rtk_l2_ipmcMode_set(modeW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if(rtk_l2_ipmcMode_get(&modeR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if(modeW != modeR)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }

    return RT_ERR_OK;
}

int32 dal_l2_limitLearningCnt_test(uint32 caseNo)
{
    uint32 cntW;
    uint32 cntR;

    if(rtk_l2_limitLearningCnt_set(1) == RT_ERR_DRIVER_NOT_FOUND)
    {
        return RT_ERR_OK;
    }

    /*error input check*/
    /*out of range*/
    if(rtk_l2_limitLearningCnt_set(HAL_L2_LEARN_LIMIT_CNT_MAX() + 1) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*null pointer*/
    if(rtk_l2_limitLearningCnt_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if(rtk_l2_learningCnt_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* Get/set */
    for(cntW = 0; cntW <= HAL_L2_LEARN_LIMIT_CNT_MAX(); cntW+=32 )
    {
        if(rtk_l2_limitLearningCnt_set(cntW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if(rtk_l2_limitLearningCnt_get(&cntR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if(cntW != cntR)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }

    if(rtk_l2_learningCnt_get(&cntR) != RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

int32 dal_l2_limitLearningCntAction_test(uint32 caseNo)
{
    rtk_l2_limitLearnCntAction_t actW;
    rtk_l2_limitLearnCntAction_t actR;

    if(rtk_l2_limitLearningCnt_set(1) == RT_ERR_DRIVER_NOT_FOUND)
    {
        return RT_ERR_OK;
    }

    /*error input check*/
    /*out of range*/
    if(rtk_l2_limitLearningCntAction_set(LIMIT_LEARN_CNT_ACTION_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*null pointer*/
    if(rtk_l2_limitLearningCntAction_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* Get/set */
    for(actW = LIMIT_LEARN_CNT_ACTION_DROP; actW < LIMIT_LEARN_CNT_ACTION_END; actW++)
    {
        if( (actW == LIMIT_LEARN_CNT_ACTION_DROP) ||
            (actW == LIMIT_LEARN_CNT_ACTION_FORWARD) ||
            (actW == LIMIT_LEARN_CNT_ACTION_TO_CPU) ||
            (actW == LIMIT_LEARN_CNT_ACTION_COPY_CPU) )
        {
            if(rtk_l2_limitLearningCntAction_set(actW) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            if(rtk_l2_limitLearningCntAction_get(&actR) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            if(actW != actW)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
        }
    }

    return RT_ERR_OK;
}


int32 dal_l2_srcPortEgrFilterMask_test(uint32 caseNo)
{
    rtk_portmask_t portmaskW;
    rtk_portmask_t portmaskR;
    uint32 port;

    /*error input check*/
    /*out of range*/
    portmaskW.bits[0] = 0xFFFF;
    if(rtk_l2_srcPortEgrFilterMask_set(&portmaskW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*null pointer*/
    if(rtk_l2_srcPortEgrFilterMask_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* Get/set */
    portmaskW.bits[0] = 0x0;
    HAL_SCAN_ALL_PORT(port)
    {
        portmaskW.bits[0] |= 1 << port;
        if(rtk_l2_srcPortEgrFilterMask_set(&portmaskW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if(rtk_l2_srcPortEgrFilterMask_get(&portmaskR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if(RTK_PORTMASK_COMPARE(portmaskW, portmaskR) != 0)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }

    return RT_ERR_OK;

}

int32 dal_l2_newMacOp_test(uint32 caseNo)
{
    rtk_port_t port;
    rtk_action_t actW;
    rtk_action_t actR;
    rtk_l2_newMacLrnMode_t modeR;

    /*error input check*/
    /*out of range*/
    if(rtk_l2_newMacOp_set(HAL_GET_MAX_PORT() + 1, HARDWARE_LEARNING, ACTION_FORWARD) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if(rtk_l2_newMacOp_set(0, SOFTWARE_LEARNING, ACTION_FORWARD) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if(rtk_l2_newMacOp_set(0, NOT_LEARNING, ACTION_FORWARD) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if(rtk_l2_newMacOp_set(0, HARDWARE_LEARNING, ACTION_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*null pointer*/
    if(rtk_l2_newMacOp_get(0, NULL, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* Get/set */
    HAL_SCAN_ALL_PORT(port)
    {
        for(actW = ACTION_FORWARD; actW <= ACTION_END; actW++)
        {
            if( (actW == ACTION_FORWARD) || (actW == ACTION_DROP) || (actW == ACTION_TRAP2CPU) || (actW == ACTION_COPY2CPU) )
            {
                if(rtk_l2_newMacOp_set(port, HARDWARE_LEARNING, actW) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }

                if(rtk_l2_newMacOp_get(port, &modeR, &actR) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }

                if(actW != actR)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }
            }
        }
    }

    return RT_ERR_OK;
}


int32 dal_l2_lookupMissAction_test(uint32 caseNo)
{
    rtk_l2_lookupMissType_t type;
    rtk_action_t actW;
    rtk_action_t actR;

    /*error input check*/
    /*out of range*/
    if(rtk_l2_lookupMissAction_set(DLF_TYPE_END, ACTION_FORWARD) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if(rtk_l2_lookupMissAction_set(DLF_TYPE_MCAST, ACTION_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*null pointer*/
    if(rtk_l2_lookupMissAction_get(DLF_TYPE_MCAST, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* Get/set */
    for(type = DLF_TYPE_IPMC; type < DLF_TYPE_END; type++)
    {
        if(type != DLF_TYPE_BCAST)
        {
            for(actW = ACTION_FORWARD; actW < ACTION_END; actW++)
            {
                switch(UNITTEST_UTIL_CHIP_TYPE)
                {
                case APOLLOMP_CHIP_ID:
                case RTL9601B_CHIP_ID:
                    switch(actW)
                    {
                    case ACTION_FORWARD:
                    case ACTION_DROP:
                    case ACTION_TRAP2CPU:
                        /* Support modes */
                        break;
                    case ACTION_DROP_EXCLUDE_RMA:
                        if(type == DLF_TYPE_MCAST)
                        {
                            /* Support modes */
                            break;
                        }
                        else
                        {
                            continue;
                        }
                    default:
                        continue;
                    }
                    break;
                case RTL9602C_CHIP_ID:
                    switch(actW)
                    {
                    case ACTION_FORWARD:
                    case ACTION_TRAP2CPU:
                        /* Support modes */
                        break;
                    case ACTION_DROP:
                        if((type == DLF_TYPE_RSVIPMC) || (type == DLF_TYPE_RSVIP6MC) || (type == DLF_TYPE_ICMP6MC) || (type == DLF_TYPE_DHCP6MC))
                        {
                            continue;
                        }
                        else
                        {
                            /* Support modes */
                            break;
                        }
                        break;
                    case ACTION_DROP_EXCLUDE_RMA:
                        if(type == DLF_TYPE_MCAST)
                        {
                            /* Support modes */
                            break;
                        }
                        else
                        {
                            continue;
                        }
                    case ACTION_FLOOD_IN_VLAN:
                        if((type == DLF_TYPE_RSVIPMC) || (type == DLF_TYPE_RSVIP6MC))
                        {
                            /* Support modes */
                            break;
                        }
                        else
                        {
                            continue;
                        }
                        break;
                    default:
                        continue;
                    }
                    break;
                case RTL9607C_CHIP_ID:
                case RTL9603CVD_CHIP_ID:
                    switch(actW)
                    {
                    case ACTION_FORWARD:
                    case ACTION_TRAP2CPU:
                        /* Support modes */
                        break;
                    case ACTION_DROP:
                        if((type == DLF_TYPE_RSVIPMC) || (type == DLF_TYPE_RSVIP6MC) || (type == DLF_TYPE_ICMP6MC) || (type == DLF_TYPE_DHCP6MC))
                        {
                            continue;
                        }
                        else
                        {
                            /* Support modes */
                            break;
                        }
                        break;
                    case ACTION_FLOOD_IN_VLAN:
                        if(type == DLF_TYPE_RSVIPMC || (type == DLF_TYPE_RSVIP6MC))
                        {
                            /* Support modes */
                            break;
                        }
                        else
                        {
                            continue;
                        }
                        break;
                    default:
                        continue;
                    }
                    break;
                default:
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }
                if(rtk_l2_lookupMissAction_set(type, actW) != RT_ERR_OK)
                {
                    osal_printf("\n %d %d", type, actW);
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }

                if(rtk_l2_lookupMissAction_get(type, &actR) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }

                if(actW != actR)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }
            }
        }
    }

    return RT_ERR_OK;
}

int32 dal_l2_lookupMissFloodPortMask_test(uint32 caseNo)
{
    rtk_l2_lookupMissType_t type;
    rtk_portmask_t portmaskW;
    rtk_portmask_t portmaskR;
    uint32 port;

    /*error input check*/
    /*out of range*/
    RTK_PORTMASK_RESET(portmaskW);
    for(type = DLF_TYPE_IPMC; type < DLF_TYPE_END; type++)
    {
        if((type != DLF_TYPE_IPMC) && (type != DLF_TYPE_UCAST) && (type != DLF_TYPE_MCAST) && (type != DLF_TYPE_IP6MC) && (type != DLF_TYPE_BCAST))
        {
            if(rtk_l2_lookupMissFloodPortMask_set(DLF_TYPE_END, &portmaskW) == RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
        }
    }

    portmaskW.bits[0] = 0xFFFF;
    if(rtk_l2_lookupMissFloodPortMask_set(DLF_TYPE_MCAST, &portmaskW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*null pointer*/
    if(rtk_l2_lookupMissFloodPortMask_get(DLF_TYPE_MCAST, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* Get/set */
    for(type = DLF_TYPE_IPMC; type < DLF_TYPE_END; type++)
    {
        if((type == DLF_TYPE_IPMC) || (type == DLF_TYPE_UCAST) || (type == DLF_TYPE_MCAST) || (type == DLF_TYPE_IP6MC) || (type == DLF_TYPE_BCAST))
        {
            portmaskW.bits[0] = 0x0;
            HAL_SCAN_ALL_PORT(port)
            {
                portmaskW.bits[0] |= 1 << port;
                if(rtk_l2_lookupMissFloodPortMask_set(type, &portmaskW) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }

                if(rtk_l2_lookupMissFloodPortMask_get(type, &portmaskR) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }

                if(RTK_PORTMASK_COMPARE(portmaskW, portmaskR) != 0)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }
            }
        }
    }

    return RT_ERR_OK;
}

int32 dal_l2_addr_test(uint32 caseNo)
{
    rtk_vlan_t  vid;
    uint32      mac;
    uint32      fid;
    rtk_port_t  port;
    uint32      ext_port;
    uint32      flags;          /* Refer to RTK_L2_UCAST_FLAG_XXX */

    int32       index;

    rtk_l2_ucastAddr_t l2AddrW;
    rtk_l2_ucastAddr_t l2AddrR;

    int32 retVal;

    /*error input check*/
    /*out of range*/
    osal_memset(&l2AddrW, 0x00, sizeof(rtk_l2_ucastAddr_t));
    l2AddrW.vid = RTK_VLAN_ID_MAX + 1;
    if(rtk_l2_addr_add(&l2AddrW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&l2AddrW, 0x00, sizeof(rtk_l2_ucastAddr_t));
    l2AddrW.mac.octet[0] = 0x01;
    if(rtk_l2_addr_add(&l2AddrW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    switch(UNITTEST_UTIL_CHIP_TYPE)
    {
    case RTL9601B_CHIP_ID:
        osal_memset(&l2AddrW, 0x00, sizeof(rtk_l2_ucastAddr_t));
        l2AddrW.ctag_vid = RTK_VLAN_ID_MAX + 1;
        if(rtk_l2_addr_add(&l2AddrW) == RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        break;
    default:
        break;
    }

    switch(UNITTEST_UTIL_CHIP_TYPE)
    {
    case RTL9601B_CHIP_ID:
        break;
    case RTL9607C_CHIP_ID:
    case RTL9603CVD_CHIP_ID:
        osal_memset(&l2AddrW, 0x00, sizeof(rtk_l2_ucastAddr_t));
        l2AddrW.fid = HAL_VLAN_FID_MAX() + 1;
        if(rtk_l2_addr_add(&l2AddrW) == RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

         osal_memset(&l2AddrW, 0x00, sizeof(rtk_l2_ucastAddr_t));
        l2AddrW.ext_port = HAL_GET_MAX_EXT_PORT() + 1;
        if(rtk_l2_addr_add(&l2AddrW) == RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        break;
    default:
        osal_memset(&l2AddrW, 0x00, sizeof(rtk_l2_ucastAddr_t));
        l2AddrW.fid = HAL_VLAN_FID_MAX() + 1;
        if(rtk_l2_addr_add(&l2AddrW) == RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        osal_memset(&l2AddrW, 0x00, sizeof(rtk_l2_ucastAddr_t));
        l2AddrW.priority = HAL_INTERNAL_PRIORITY_MAX() + 1;
        if(rtk_l2_addr_add(&l2AddrW) == RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        osal_memset(&l2AddrW, 0x00, sizeof(rtk_l2_ucastAddr_t));
        l2AddrW.auth = RTK_ENABLE_END;
        if(rtk_l2_addr_add(&l2AddrW) == RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        
        osal_memset(&l2AddrW, 0x00, sizeof(rtk_l2_ucastAddr_t));
        l2AddrW.ext_port = HAL_GET_MAX_EXT_PORT() + 1;
        if(rtk_l2_addr_add(&l2AddrW) == RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        break;
    }

    switch(UNITTEST_UTIL_CHIP_TYPE)
    {
    case APOLLOMP_CHIP_ID:
        osal_memset(&l2AddrW, 0x00, sizeof(rtk_l2_ucastAddr_t));
        l2AddrW.efid = HAL_ENHANCED_FID_MAX() + 1;
        if(rtk_l2_addr_add(&l2AddrW) == RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        break;
    default:
        break;
    }

    osal_memset(&l2AddrW, 0x00, sizeof(rtk_l2_ucastAddr_t));
    l2AddrW.port = HAL_GET_MAX_PORT() + 1;
    if(rtk_l2_addr_add(&l2AddrW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&l2AddrW, 0x00, sizeof(rtk_l2_ucastAddr_t));
    l2AddrW.flags = RTK_L2_UCAST_FLAG_ALL + 1;
    if(rtk_l2_addr_add(&l2AddrW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&l2AddrW, 0x00, sizeof(rtk_l2_ucastAddr_t));
    l2AddrW.age = 8;
    if(rtk_l2_addr_add(&l2AddrW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*null pointer*/
    if(rtk_l2_addr_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    switch(UNITTEST_UTIL_CHIP_TYPE)
    {
    case APOLLOMP_CHIP_ID:
    case RTL9601B_CHIP_ID:
    case RTL9602C_CHIP_ID:
        return _dal_apollomp_l2_addr();
    case RTL9607C_CHIP_ID:
    case RTL9603CVD_CHIP_ID:
        return _dal_rtl9607c_l2_addr();
    default:
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}


int32 dal_l2_mcastAddr_test(uint32 caseNo)
{
    uint16          vid;
    uint32          mac;
    uint32          fid;
    rtk_portmask_t  portmask;
    rtk_portmask_t  ext_portmask;
    uint32          flags;
    uint32          priority;

    int32           index;

    rtk_l2_mcastAddr_t  l2AddrW;
    rtk_l2_mcastAddr_t  l2AddrR;

    /*error input check*/
    /*out of range*/
    osal_memset(&l2AddrW, 0x00, sizeof(rtk_l2_mcastAddr_t));
    l2AddrW.mac.octet[0] = 0x01;
    l2AddrW.vid = RTK_VLAN_ID_MAX + 1;
    if(rtk_l2_mcastAddr_add(&l2AddrW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&l2AddrW, 0x00, sizeof(rtk_l2_mcastAddr_t));
    l2AddrW.mac.octet[0] = 0x00;
    if(rtk_l2_mcastAddr_add(&l2AddrW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&l2AddrW, 0x00, sizeof(rtk_l2_mcastAddr_t));
    l2AddrW.mac.octet[0] = 0x01;
    l2AddrW.fid = HAL_VLAN_FID_MAX() + 1;
    if(rtk_l2_mcastAddr_add(&l2AddrW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&l2AddrW, 0x00, sizeof(rtk_l2_mcastAddr_t));
    l2AddrW.mac.octet[0] = 0x01;
    l2AddrW.portmask.bits[0] = 0xFFFFFFFF;
    if(rtk_l2_mcastAddr_add(&l2AddrW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    switch(UNITTEST_UTIL_CHIP_TYPE)
    {
    case APOLLOMP_CHIP_ID:
    case RTL9601B_CHIP_ID:
    case RTL9602C_CHIP_ID:
        osal_memset(&l2AddrW, 0x00, sizeof(rtk_l2_mcastAddr_t));
        l2AddrW.mac.octet[0] = 0x01;
        l2AddrW.ext_portmask.bits[0] = 0xFFFFFFFF;
        if(rtk_l2_mcastAddr_add(&l2AddrW) == RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        break;
    case RTL9607C_CHIP_ID:
    case RTL9603CVD_CHIP_ID:
        osal_memset(&l2AddrW, 0x00, sizeof(rtk_l2_mcastAddr_t));
        l2AddrW.mac.octet[0] = 0x01;
        l2AddrW.ext_portmask_idx = 0xFFFFFFFF;
        if(rtk_l2_mcastAddr_add(&l2AddrW) == RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        break;
    default:
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&l2AddrW, 0x00, sizeof(rtk_l2_mcastAddr_t));
    l2AddrW.mac.octet[0] = 0x01;
    l2AddrW.flags = RTK_L2_MCAST_FLAG_ALL + 1;
    if(rtk_l2_mcastAddr_add(&l2AddrW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    switch(UNITTEST_UTIL_CHIP_TYPE)
    {
    case APOLLOMP_CHIP_ID:
        osal_memset(&l2AddrW, 0x00, sizeof(rtk_l2_mcastAddr_t));
        l2AddrW.mac.octet[0] = 0x01;
        l2AddrW.priority = HAL_INTERNAL_PRIORITY_MAX() + 1;
        if(rtk_l2_mcastAddr_add(&l2AddrW) == RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        break;
    case RTL9601B_CHIP_ID:
    case RTL9602C_CHIP_ID:
    case RTL9607C_CHIP_ID:
    case RTL9603CVD_CHIP_ID:
        break;
    default:
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*null pointer*/
    if(rtk_l2_mcastAddr_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    switch(UNITTEST_UTIL_CHIP_TYPE)
    {
    case APOLLOMP_CHIP_ID:
    case RTL9601B_CHIP_ID:
    case RTL9602C_CHIP_ID:
        return _dal_apollomp_mcastAddr();
    case RTL9607C_CHIP_ID:
    case RTL9603CVD_CHIP_ID:
        return _dal_rtl9607c_mcastAddr();
    default:
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

int32 dal_l2_ipMcastAddr_test(uint32 caseNo)
{
    ipaddr_t        dip;
    ipaddr_t        sip;
    uint16          vid;
    rtk_portmask_t  portmask;
    rtk_portmask_t  ext_portmask;
    uint32          l3_trans_index;
    uint32          flags;          /* Refer to RTK_L2_IPMCAST_FLAG_XXX */
    uint32          priority;

    int32           index;

    rtk_l2_ipMcastAddr_t ipAddrW;
    rtk_l2_ipMcastAddr_t ipAddrR;

    /*error input check*/
    /*out of range*/
    osal_memset(&ipAddrW, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
    ipAddrW.dip = 0x00000000;
    if(rtk_l2_ipMcastAddr_add(&ipAddrW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    switch(UNITTEST_UTIL_CHIP_TYPE)
    {
    case APOLLOMP_CHIP_ID:
    case RTL9601B_CHIP_ID:
        rtk_l2_ipmcMode_set(LOOKUP_ON_DIP_AND_VID);
        break;
    case RTL9602C_CHIP_ID:
        break;
    case RTL9607C_CHIP_ID:
    case RTL9603CVD_CHIP_ID:
        rtk_l2_ipmcMode_set(LOOKUP_ON_MAC_AND_VID_FID);
        break;
    default:
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    osal_memset(&ipAddrW, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
    ipAddrW.dip = 0xE0000001;
    ipAddrW.flags |= RTK_L2_IPMCAST_FLAG_IVL;
    ipAddrW.vid = RTK_VLAN_ID_MAX + 1;
    if(rtk_l2_ipMcastAddr_add(&ipAddrW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    switch(UNITTEST_UTIL_CHIP_TYPE)
    {
    case APOLLOMP_CHIP_ID:
        rtk_l2_ipmcMode_set(LOOKUP_ON_DIP_AND_SIP);
        osal_memset(&ipAddrW, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
        ipAddrW.dip = 0xE0000001;
        ipAddrW.priority = HAL_INTERNAL_PRIORITY_MAX() + 1;
        if(rtk_l2_ipMcastAddr_add(&ipAddrW) == RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        break;
    case RTL9601B_CHIP_ID:
    case RTL9602C_CHIP_ID:
    case RTL9607C_CHIP_ID:
    case RTL9603CVD_CHIP_ID:
        break;
    default:
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*null pointer*/
    if(rtk_l2_ipMcastAddr_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    switch(UNITTEST_UTIL_CHIP_TYPE)
    {
    case APOLLOMP_CHIP_ID:
    case RTL9601B_CHIP_ID:
    case RTL9602C_CHIP_ID:
        return _dal_apollomp_ipMcastAddr();
    case RTL9607C_CHIP_ID:
    case RTL9603CVD_CHIP_ID:
        return _dal_rtl9607c_ipMcastAddr();
    default:
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

int32 dal_l2_ipmcGroup_test(uint32 caseNo)
{
    int32 ret;
    ipaddr_t gip;
    rtk_portmask_t portmaskW;
    rtk_portmask_t portmaskR;

    /*error input check*/
    /*out of range*/
    gip = 0x00000000;
    portmaskW.bits[0] = 0;
    if(rtk_l2_ipmcGroup_add(gip, &portmaskW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    gip = 0xE0000001;
    portmaskW.bits[0] = 0xFFFF;
    if(rtk_l2_ipmcGroup_add(gip, &portmaskW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* delete */
    if(rtk_l2_ipmcGroup_del(gip) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*null pointer*/
    if(rtk_l2_ipmcGroup_get(gip, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* Get/set */
    for(gip = 0xE0000001; gip <= 0xEFFFFFFF; gip+=0x0FFFFFFE)
    {
        for(portmaskW.bits[0] = 0x00; HAL_IS_PORTMASK_VALID(portmaskW); portmaskW.bits[0]++)
        {
            if( (ret = rtk_l2_ipmcGroup_add(gip, &portmaskW)) != RT_ERR_OK)
            {
                osal_printf("\n %s %d 0x%X\n",__FUNCTION__,__LINE__, ret);
                return RT_ERR_FAILED;
            }

            if(rtk_l2_ipmcGroup_get(gip, &portmaskR) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            if(RTK_PORTMASK_COMPARE(portmaskW, portmaskR) != 0)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            if(rtk_l2_ipmcGroup_del(gip) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            if(rtk_l2_ipmcGroup_get(gip, &portmaskR) == RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
        }
    }

    return RT_ERR_OK;
}


int32 dal_l2_portLimitLearningCnt_test(uint32 caseNo)
{
    rtk_port_t port;
    int32 ret;
    uint32 cntW;
    uint32 cntR;

    /*error input check*/
    /*out of range*/
    if( (ret = rtk_l2_portLimitLearningCnt_set(HAL_GET_MAX_PORT() + 1, HAL_L2_LEARN_LIMIT_CNT_MAX())) == RT_ERR_OK)
    {
        osal_printf("\n %s %d (0x%X)\n",__FUNCTION__,__LINE__,ret);
        return RT_ERR_FAILED;
    }

    if( (ret = rtk_l2_portLimitLearningCnt_set(0, HAL_L2_LEARN_LIMIT_CNT_MAX() + 1)) == RT_ERR_OK)
    {
        osal_printf("\n %s %d (0x%X)\n",__FUNCTION__,__LINE__,ret);
        return RT_ERR_FAILED;
    }

    /*null pointer*/
    if( (ret = rtk_l2_portLimitLearningCnt_get(0, NULL)) == RT_ERR_OK)
    {
        osal_printf("\n %s %d (0x%X)\n",__FUNCTION__,__LINE__,ret);
        return RT_ERR_FAILED;
    }

    if( (ret = rtk_l2_portLearningCnt_get(0, NULL)) == RT_ERR_OK)
    {
        osal_printf("\n %s %d (0x%X)\n",__FUNCTION__,__LINE__,ret);
        return RT_ERR_FAILED;
    }

    /* Get/set */
    HAL_SCAN_ALL_PORT(port)
    {
        for(cntW = 0; cntW <= HAL_L2_LEARN_LIMIT_CNT_MAX(); cntW+=123)
        {
            if( (ret = rtk_l2_portLimitLearningCnt_set(port, cntW)) != RT_ERR_OK)
            {
                osal_printf("\n %s %d (0x%X)\n",__FUNCTION__,__LINE__,ret);
                return RT_ERR_FAILED;
            }

            if( (ret = rtk_l2_portLimitLearningCnt_get(port, &cntR)) != RT_ERR_OK)
            {
                osal_printf("\n %s %d (0x%X)\n",__FUNCTION__,__LINE__,ret);
                return RT_ERR_FAILED;
            }

            if(cntW != cntR)
            {
                osal_printf("\n %s %d (0x%X)\n",__FUNCTION__,__LINE__,ret);
                return RT_ERR_FAILED;
            }

            if( (ret = rtk_l2_portLearningCnt_get(port, &cntR)) != RT_ERR_OK)
            {
                osal_printf("\n %s %d (0x%X)\n",__FUNCTION__,__LINE__,ret);
                return RT_ERR_FAILED;
            }
        }
    }

    return RT_ERR_OK;
}

int32 dal_l2_portLimitLearningCntAction_test(uint32 caseNo)
{
    rtk_port_t port;
    rtk_l2_limitLearnCntAction_t actW;
    rtk_l2_limitLearnCntAction_t actR;

    /*error input check*/
    /*out of range*/
    if(rtk_l2_portLimitLearningCntAction_set(HAL_GET_MAX_PORT() + 1, LIMIT_LEARN_CNT_ACTION_FORWARD) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if(rtk_l2_portLimitLearningCntAction_set(0, LIMIT_LEARN_CNT_ACTION_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*null pointer*/
    if(rtk_l2_portLimitLearningCntAction_get(0, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* Get/set */
    HAL_SCAN_ALL_PORT(port)
    {
        for(actW = LIMIT_LEARN_CNT_ACTION_DROP; actW < LIMIT_LEARN_CNT_ACTION_END; actW++)
        {
            if( (actW == LIMIT_LEARN_CNT_ACTION_DROP) ||
                (actW == LIMIT_LEARN_CNT_ACTION_FORWARD) ||
                (actW == LIMIT_LEARN_CNT_ACTION_TO_CPU) ||
                (actW == LIMIT_LEARN_CNT_ACTION_COPY_CPU) )
            {
                if(rtk_l2_portLimitLearningCntAction_set(port, actW) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }

                if(rtk_l2_portLimitLearningCntAction_get(port, &actR) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }

                if(actW != actW)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }
            }
        }
    }

    return RT_ERR_OK;
}

int32 dal_l2_portLimitLearningOverStatus_test(uint32 caseNo)
{
    rtk_port_t port;
    uint32 status;

    /*error input check*/
    /*out of range*/
    if(rtk_l2_portLimitLearningOverStatus_get(HAL_GET_MAX_PORT() + 1, &status) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if(rtk_l2_portLimitLearningOverStatus_clear(HAL_GET_MAX_PORT() + 1) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*null pointer*/
    if(rtk_l2_portLimitLearningOverStatus_get(0, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* Get/set */
    HAL_SCAN_ALL_PORT(port)
    {
        if(rtk_l2_portLimitLearningOverStatus_get(port, &status) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if(rtk_l2_portLimitLearningOverStatus_clear(port) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }


    return RT_ERR_OK;
}

int32 dal_l2_limitLearningOverStatus_test(uint32 caseNo)
{
    uint32 status;

    if(rtk_l2_limitLearningOverStatus_get(&status) == RT_ERR_DRIVER_NOT_FOUND)
    {
        return RT_ERR_OK;
    }

    /*error input check*/
    /*out of range*/

    /*null pointer*/
    if(rtk_l2_limitLearningOverStatus_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* Get/set */
    if(rtk_l2_limitLearningOverStatus_get(&status) != RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if(rtk_l2_limitLearningOverStatus_clear() != RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

int32 dal_l2_portLookupMissAction_test(uint32 caseNo)
{
    rtk_port_t port;
    rtk_l2_lookupMissType_t type;
    rtk_action_t actW;
    rtk_action_t actR;

    /*error input check*/
    /*out of range*/
    if(rtk_l2_portLookupMissAction_set(HAL_GET_MAX_PORT() + 1, DLF_TYPE_MCAST, ACTION_FORWARD) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if(rtk_l2_portLookupMissAction_set(0, DLF_TYPE_END, ACTION_FORWARD) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if(rtk_l2_portLookupMissAction_set(0, DLF_TYPE_MCAST, ACTION_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*null pointer*/
    if(rtk_l2_portLookupMissAction_get(0, DLF_TYPE_MCAST, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* Get/set */
    HAL_SCAN_ALL_PORT(port)
    {
        for(type = DLF_TYPE_IPMC; type < DLF_TYPE_END; type++)
        {
            switch(UNITTEST_UTIL_CHIP_TYPE)
            {
            case APOLLOMP_CHIP_ID:
            case RTL9601B_CHIP_ID:
            case RTL9602C_CHIP_ID:
                switch(type)
                {
                case DLF_TYPE_IPMC:
                case DLF_TYPE_UCAST:
                case DLF_TYPE_MCAST:
                case DLF_TYPE_IP6MC:
                    break;
                default:
                    continue;
                }
            case RTL9607C_CHIP_ID:
            case RTL9603CVD_CHIP_ID:
                switch(type)
                {
                case DLF_TYPE_IPMC:
                case DLF_TYPE_UCAST:
                case DLF_TYPE_MCAST:
                case DLF_TYPE_BCAST:
                case DLF_TYPE_IP6MC:
                    break;
                default:
                    continue;
                }
                break;
            default:
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
            for(actW = ACTION_FORWARD; actW < ACTION_END; actW++)
            {
                switch(UNITTEST_UTIL_CHIP_TYPE)
                {
                case APOLLOMP_CHIP_ID:
                case RTL9601B_CHIP_ID:
                case RTL9602C_CHIP_ID:
                    switch(actW)
                    {
                    case ACTION_FORWARD:
                    case ACTION_DROP:
                    case ACTION_TRAP2CPU:
                        /* Support modes */
                        break;
                    case ACTION_DROP_EXCLUDE_RMA:
                        if(type == DLF_TYPE_MCAST)
                        {
                            /* Support modes */
                            break;
                        }
                        else
                        {
                            continue;
                        }
                    default:
                        continue;
                    }
                    break;
                case RTL9607C_CHIP_ID:
                case RTL9603CVD_CHIP_ID:
                    switch(actW)
                    {
                    case ACTION_FORWARD:
                    case ACTION_TRAP2CPU:
                        /* Support modes */
                        break;
                    case ACTION_DROP:
                        if(type != DLF_TYPE_BCAST)
                        {
                            /* Support modes */
                            break;
                        }
                        else
                        {
                            continue;
                        }
                        break;
                    case ACTION_FOLLOW_FB:
                        if(type == DLF_TYPE_BCAST)
                        {
                            /* Support modes */
                            break;
                        }
                        else
                        {
                            continue;
                        }
                        break;
                    default:
                        continue;
                    }
                    break;
                default:
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }                        
                if(rtk_l2_portLookupMissAction_set(port, type, actW) != RT_ERR_OK)
                {
                    osal_printf("\n %d %d", type, actW);
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }

                if(rtk_l2_portLookupMissAction_get(port, type, &actR) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }

                if(actW != actR)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }
            }
        }
    }

    return RT_ERR_OK;
}

int32 dal_l2_portIpmcAction_test(uint32 caseNo)
{
    rtk_port_t port;
    rtk_action_t actW;
    rtk_action_t actR;

    /*error input check*/
    /*out of range*/
    if(rtk_l2_portIpmcAction_set(HAL_GET_MAX_PORT() + 1, ACTION_FORWARD) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if(rtk_l2_portIpmcAction_set(HAL_GET_MAX_PORT(), ACTION_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if(rtk_l2_portIpmcAction_get(HAL_GET_MAX_PORT() + 1, &actR) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if(rtk_l2_portIpmcAction_get(0, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* Get/set */
    HAL_SCAN_ALL_PORT(port)
    {
        for(actW = ACTION_FORWARD; actW < ACTION_END; actW++)
        {
            if( (actW == ACTION_FORWARD) || (actW == ACTION_DROP) )
            {
                if(rtk_l2_portIpmcAction_set(port, actW) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }

                if(rtk_l2_portIpmcAction_get(port, &actR) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }

                if(actW != actR)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }
            }
        }
    }

    return RT_ERR_OK;
}
