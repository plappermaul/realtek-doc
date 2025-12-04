#include <osal/lib.h>
#include <common/debug/rt_log.h>
#include <dal/apollo/raw/apollo_raw_trap.h>
#include <ioal/mem32.h>
#include <dal/raw/apollo/dal_raw_rma_test_case.h>

int32 dal_rma_raw_test(uint32 caseNo)
{
    uint32 priW;
    uint32 priR;
    uint32 rmaTail;
    rtk_action_t actionW;
    rtk_action_t actionR;
    apollo_raw_rmaFeature_t typeW;
    apollo_raw_rmaFeature_t typeR;
    rtk_enable_t enableW;
    rtk_enable_t enableR;
    uint32 tmp;
    uint32 actionArr[4] = {ACTION_FORWARD, ACTION_TRAP2CPU,ACTION_DROP,ACTION_FORWARD_EXCLUDE_CPU};

    uint32 regData;
    uint32 wData;
    uint32 dataMask;
    uint32 checkData;
    uint32 index;
    uint32 regAddr;
    uint32 regField;
    uint32 fieldIdx;
    int32 ret;

    /*error input check*/
    /*out of range*/

    if( apollo_raw_trap_rmaPriority_set(APOLLO_RAW_RMA_MAXPRI+1) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if( apollo_raw_trap_rmaAction_set(APOLLO_RAW_RMA_L2TAILNUM_MAX +1 , ACTION_FORWARD) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if( apollo_raw_trap_rmaAction_set(APOLLO_RAW_RMA_L2TAILNUM_MAX, ACTION_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_trap_rmaFeature_set(APOLLO_RAW_RMA_L2TAILNUM_MAX +1 , RAW_RMA_FUN_VLAN_LEAKY, DISABLED) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if( apollo_raw_trap_rmaFeature_set(APOLLO_RAW_RMA_L2TAILNUM_MAX  , RAW_RMA_FUN_END, DISABLED) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if( apollo_raw_trap_rmaFeature_set(APOLLO_RAW_RMA_L2TAILNUM_MAX , RAW_RMA_FUN_VLAN_LEAKY, RTK_ENABLE_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if( apollo_raw_trap_rmaCiscoAction_set(0xcc, ACTION_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_trap_rmaCiscoFeature_set(0xcc, RAW_RMA_FUN_END, DISABLED) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if( apollo_raw_trap_rmaCiscoFeature_set(0xcc, RAW_RMA_FUN_BYPASS_STORM, RTK_ENABLE_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
     if( apollo_raw_trap_rmaCiscoFeature_set(APOLLO_RAW_RMA_L2TAILNUM_MAX, RAW_RMA_FUN_BYPASS_STORM, DISABLED) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }


    /* get/set test*/

    for (priW =0 ; priW <= APOLLO_RAW_RMA_MAXPRI; priW++)
    {
        if( apollo_raw_trap_rmaPriority_set(priW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        if( apollo_raw_trap_rmaPriority_get(&priR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        if(priW != priR)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

    }

    for(rmaTail = 0 ; rmaTail <= APOLLO_RAW_RMA_L2TAILNUM_MAX; rmaTail++)
    {
        for(tmp = 0 ; tmp < 4 ; tmp++)
        {
            actionW = actionArr[tmp];
            if( apollo_raw_trap_rmaAction_set(rmaTail, actionW) != RT_ERR_OK)
            {
                osal_printf("\n %s %d, tail:%x,action:%x\n",__FUNCTION__,__LINE__,rmaTail,actionW);
                return RT_ERR_FAILED;
            }
            if( apollo_raw_trap_rmaAction_get(rmaTail, &actionR) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
            if(actionR != actionW)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }


        }
        for (typeW = 0; typeW < RAW_RMA_FUN_END; typeW++)
        {
            for (enableW = 0; enableW < RTK_ENABLE_END ; enableW++)
            {
                if( apollo_raw_trap_rmaFeature_set(rmaTail, typeW, enableW) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }
                if( apollo_raw_trap_rmaFeature_get(rmaTail, typeW, &enableR) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }
                if(enableW != enableR)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }


           }



        }


    }

    for(rmaTail = 0xcc ; rmaTail <= 0xcd; rmaTail++)
    {
        for(tmp = 0 ; tmp < 4 ; tmp++)
        {
            actionW = actionArr[tmp];
            if( apollo_raw_trap_rmaCiscoAction_set(rmaTail, actionW) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
            if( apollo_raw_trap_rmaCiscoAction_get(rmaTail, &actionR) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
            if(actionR != actionW)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }


        }
        for (typeW = 0; typeW < RAW_RMA_FUN_END; typeW++)
        {
            for (enableW = 0; enableW < RTK_ENABLE_END ; enableW++)
            {
                if( apollo_raw_trap_rmaCiscoFeature_set(rmaTail, typeW, enableW) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }
                if( apollo_raw_trap_rmaCiscoFeature_get(rmaTail, typeW, &enableR) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }
                if(enableW != enableR)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }

           }

        }
    }

    /*null pointer*/
    rmaTail = 1;
    if( apollo_raw_trap_rmaPriority_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if( apollo_raw_trap_rmaAction_get(rmaTail, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if( apollo_raw_trap_rmaFeature_get(rmaTail, RAW_RMA_FUN_END, &enableR) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if( apollo_raw_trap_rmaFeature_get(rmaTail, RAW_RMA_FUN_PISO_LEAKY, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }



    if( apollo_raw_trap_rmaCiscoAction_get(rmaTail, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
     if( apollo_raw_trap_rmaCiscoFeature_get(rmaTail, RAW_RMA_FUN_END, &enableR) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if( apollo_raw_trap_rmaCiscoFeature_get(rmaTail, RAW_RMA_FUN_KEEP_CTGA_FMT, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }


    /*register access test*/
    ioal_mem32_write(HAL_GET_REG_ADDR(RMA_CFGr), 0x0);
    apollo_raw_trap_rmaPriority_set(APOLLO_RAW_RMA_MAXPRI);
    ioal_mem32_read(HAL_GET_REG_ADDR(RMA_CFGr),&regData);
    dataMask = 0x7;
    checkData =APOLLO_RAW_RMA_MAXPRI;
    /*mask out reserve bits*/
    regData = regData & dataMask;
    if(regData != checkData)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    for (index = 0; index <= 0x2f; index++)
    {
        switch(index)
	    {
		    case 0x00:
			    regAddr = RMA_CTRL00r;
			    break;
		    case 0x01:
			    regAddr = RMA_CTRL01r;
    			break;
	    	case 0x02:
		    	regAddr = RMA_CTRL02r;
			    break;
    		case 0x03:
	    		regAddr = RMA_CTRL03r;
		    	break;
    		case 0x04:
    		case 0x05:
    		case 0x06:
    		case 0x07:
    		case 0x09:
    		case 0x0A:
    		case 0x0B:
    		case 0x0C:
    		case 0x0F:
    			regAddr = RMA_CTRL04r;
    			break;
    		case 0x08:
    			regAddr = RMA_CTRL08r;
    			break;
    		case 0x0D:
    			regAddr = RMA_CTRL0Dr;
    			break;
    		case 0x0E:
    			regAddr = RMA_CTRL0Er;
		    	break;
	    	case 0x10:
    			regAddr = RMA_CTRL10r;
    			break;
    		case 0x11:
    			regAddr = RMA_CTRL11r;
    			break;
    		case 0x12:
    			regAddr = RMA_CTRL12r;
    			break;
    		case 0x13:
    		case 0x14:
    		case 0x15:
    		case 0x16:
    		case 0x17:
    		case 0x19:
	    	case 0x1B:
    		case 0x1C:
    		case 0x1D:
    		case 0x1E:
    		case 0x1F:
	    		regAddr = RMA_CTRL13r;
			    break;
		    case 0x18:
    			regAddr = RMA_CTRL18r;
    			break;
    		case 0x1A:
    			regAddr = RMA_CTRL1Ar;
    			break;
    		case 0x20:
    			regAddr = RMA_CTRL20r;
    			break;
    		case 0x21:
    			regAddr = RMA_CTRL21r;
    			break;
    		case 0x22:
            case 0x23:
            case 0x24:
            case 0x25:
            case 0x26:
            case 0x27:
            case 0x28:
            case 0x29:
            case 0x2a:
            case 0x2b:
            case 0x2c:
            case 0x2d:
            case 0x2e:
            case 0x2f:
                regAddr = RMA_CTRL22r;
		    	break;

      	}
        ioal_mem32_write(HAL_GET_REG_ADDR(regAddr), 0x0);
        if ((ret = apollo_raw_trap_rmaAction_set(index , ACTION_FORWARD_EXCLUDE_CPU)) != RT_ERR_OK)
        {
            osal_printf("\n %s %d,index: %x,regData:%x,checkData:%x,regAddr:%x\n",__FUNCTION__,__LINE__, index, regData, checkData, regAddr);
            return ret;
        }
        ioal_mem32_read(HAL_GET_REG_ADDR(regAddr),&regData);
        dataMask = 0x30;
        checkData = 0x30;
        /*mask out reserve bits*/
        regData = regData & dataMask;
        if(regData != checkData)
        {
            osal_printf("\n %s %d,index: %x,regData:%x,checkData:%x,regAddr:%x\n",__FUNCTION__,__LINE__, index, regData, checkData, regAddr);
            return RT_ERR_FAILED;
        }

        ioal_mem32_write(HAL_GET_REG_ADDR(regAddr), 0x0);
        apollo_raw_trap_rmaFeature_set(index , RAW_RMA_FUN_VLAN_LEAKY, ENABLED);
        ioal_mem32_read(HAL_GET_REG_ADDR(regAddr),&regData);
        dataMask = 0x2;
        checkData = 0x2;
        /*mask out reserve bits*/
        regData = regData & dataMask;
        if(regData != checkData)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        ioal_mem32_write(HAL_GET_REG_ADDR(regAddr), 0x0);
        apollo_raw_trap_rmaFeature_set(index , RAW_RMA_FUN_PISO_LEAKY, ENABLED);
        ioal_mem32_read(HAL_GET_REG_ADDR(regAddr),&regData);
        dataMask = 0x1;
        checkData = 0x1;
        /*mask out reserve bits*/
        regData = regData & dataMask;
        if(regData != checkData)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        ioal_mem32_write(HAL_GET_REG_ADDR(regAddr), 0x0);
        apollo_raw_trap_rmaFeature_set(index , RAW_RMA_FUN_BYPASS_STORM, ENABLED);
        ioal_mem32_read(HAL_GET_REG_ADDR(regAddr),&regData);
        dataMask = 0x8;
        checkData = 0x8;
        /*mask out reserve bits*/
        regData = regData & dataMask;
        if(regData != checkData)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        ioal_mem32_write(HAL_GET_REG_ADDR(regAddr), 0x0);
        apollo_raw_trap_rmaFeature_set(index , RAW_RMA_FUN_KEEP_CTGA_FMT, ENABLED);
        ioal_mem32_read(HAL_GET_REG_ADDR(regAddr),&regData);
        dataMask = 0x4;
        checkData = 0x4;
        /*mask out reserve bits*/
        regData = regData & dataMask;
        if(regData != checkData)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }


    }

    for (index = 0xcc; index <= 0xcd; index++)
    {
        if (index == 0xcc)
            regAddr = RMA_CTRL_CDPr;
        else
            regAddr = RMA_CTRL_SSTPr;

        ioal_mem32_write(HAL_GET_REG_ADDR(regAddr), 0x0);
        apollo_raw_trap_rmaCiscoAction_set(index, ACTION_FORWARD_EXCLUDE_CPU);
        ioal_mem32_read(HAL_GET_REG_ADDR(regAddr),&regData);
        dataMask = 0x30;
        checkData = 0x30;
        /*mask out reserve bits*/
        regData = regData & dataMask;
        if(regData != checkData)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        ioal_mem32_write(HAL_GET_REG_ADDR(regAddr), 0x0);
        apollo_raw_trap_rmaCiscoFeature_set(index , RAW_RMA_FUN_VLAN_LEAKY, ENABLED);
        ioal_mem32_read(HAL_GET_REG_ADDR(regAddr),&regData);
        dataMask = 0x2;
        checkData = 0x2;
        /*mask out reserve bits*/
        regData = regData & dataMask;
        if(regData != checkData)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        ioal_mem32_write(HAL_GET_REG_ADDR(regAddr), 0x0);
        apollo_raw_trap_rmaCiscoFeature_set(index , RAW_RMA_FUN_PISO_LEAKY, ENABLED);
        ioal_mem32_read(HAL_GET_REG_ADDR(regAddr),&regData);
        dataMask = 0x1;
        checkData = 0x1;
        /*mask out reserve bits*/
        regData = regData & dataMask;
        if(regData != checkData)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        ioal_mem32_write(HAL_GET_REG_ADDR(regAddr), 0x0);
        apollo_raw_trap_rmaCiscoFeature_set(index , RAW_RMA_FUN_BYPASS_STORM, ENABLED);
        ioal_mem32_read(HAL_GET_REG_ADDR(regAddr),&regData);
        dataMask = 0x8;
        checkData = 0x8;
        /*mask out reserve bits*/
        regData = regData & dataMask;
        if(regData != checkData)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        ioal_mem32_write(HAL_GET_REG_ADDR(regAddr), 0x0);
        apollo_raw_trap_rmaCiscoFeature_set(index , RAW_RMA_FUN_KEEP_CTGA_FMT, ENABLED);
        ioal_mem32_read(HAL_GET_REG_ADDR(regAddr),&regData);
        dataMask = 0x4;
        checkData = 0x4;
        /*mask out reserve bits*/
        regData = regData & dataMask;
        if(regData != checkData)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

    }

    return RT_ERR_OK;
}

