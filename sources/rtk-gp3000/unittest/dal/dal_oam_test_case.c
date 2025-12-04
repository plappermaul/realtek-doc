#include <osal/lib.h>
#include <osal/print.h>
#include <ioal/mem32.h>
#include <hal/common/halctrl.h>
#include <common/error.h>
#include <dal/dal_oam_test_case.h>
#include <rtk/oam.h>
#include <rtk/trap.h>
#include <common/unittest_util.h>

int32 dal_oam_test(uint32 caseNo)
{  
    rtk_oam_parser_act_t parActR;
    rtk_oam_multiplexer_act_t mulActR;
    rtk_oam_parser_act_t parActW;
    rtk_oam_multiplexer_act_t mulActW;
    rtk_port_t port;
    rtk_action_t actionR;
    rtk_action_t actionW;
    uint32 dataR;
    uint32 dataW;


    if( rtk_trap_init() != RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( rtk_oam_init() != RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    
    /*error input check*/
    if( rtk_oam_parserAction_set(HAL_GET_MAX_PORT()+1, OAM_PARSER_ACTION_FORWARD) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( rtk_oam_parserAction_set(HAL_GET_MIN_PORT()-1, OAM_PARSER_ACTION_FORWARD) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( rtk_oam_parserAction_set(0, OAM_PARSER_ACTION_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }


    if( rtk_oam_parserAction_get(HAL_GET_MAX_PORT()+1, &parActR) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( rtk_oam_parserAction_get(HAL_GET_MIN_PORT()-1, &parActR) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( rtk_oam_multiplexerAction_set(HAL_GET_MAX_PORT()+1, OAM_MULTIPLEXER_ACTION_FORWARD) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( rtk_oam_multiplexerAction_set(HAL_GET_MIN_PORT()-1, OAM_MULTIPLEXER_ACTION_FORWARD) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( rtk_oam_multiplexerAction_set(0, OAM_MULTIPLEXER_ACTION_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }


    if( rtk_oam_multiplexerAction_get(HAL_GET_MAX_PORT()+1, &mulActR) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( rtk_oam_multiplexerAction_get(HAL_GET_MIN_PORT()-1, &mulActR) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( rtk_trap_oamPduAction_set(ACTION_DROP) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( rtk_trap_oamPduPri_set(HAL_INTERNAL_PRIORITY_MAX()+1) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    
    /*get set testing*/
    HAL_SCAN_ALL_PORT(port)
    {
        for(parActW = OAM_PARSER_ACTION_FORWARD; parActW < OAM_PARSER_ACTION_END; parActW++)
        {
            if( rtk_oam_parserAction_set(port, parActW) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }
        
        
            if( rtk_oam_parserAction_get(port, &parActR) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }


            if(parActW != parActR)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }
        }

        for(mulActW = OAM_MULTIPLEXER_ACTION_FORWARD; mulActW < OAM_MULTIPLEXER_ACTION_END; mulActW++)
        {
            if(mulActW == OAM_MULTIPLEXER_ACTION_CPUONLY)
            {
                switch(UNITTEST_UTIL_CHIP_TYPE)
                {
                    case RTL9603CVD_CHIP_ID:
                    case RTL9607C_CHIP_ID:
                        continue;                    
                        break;
                    default:
                        break;
                }
            }        
            if( rtk_oam_multiplexerAction_set(port, mulActW) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }
        
        
            if( rtk_oam_multiplexerAction_get(port, &mulActR) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }


            if(mulActW != mulActR)
            {
                osal_printf("\n %s %d  port:%d  mulActW:%d   mulActR:%d\n",__FUNCTION__,__LINE__,port,mulActW,mulActR);
                return RT_ERR_FAILED;       
            }
        }
    }   
#if 0
	actionW = ACTION_FORWARD;
    if( rtk_trap_oamPduAction_set(actionW) != RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    
    if( rtk_trap_oamPduAction_get(&actionR) != RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    
    if(actionW != actionR)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

	actionW = ACTION_TRAP2CPU;
    if( rtk_trap_oamPduAction_set(actionW) != RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    
    if( rtk_trap_oamPduAction_get(&actionR) != RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    
    if(actionW != actionR)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }


    for(dataW = 0; dataW <= HAL_INTERNAL_PRIORITY_MAX(); dataW++)
    {
        if( rtk_trap_oamPduPri_set(dataW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }

        if( rtk_trap_oamPduPri_get(&dataR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
        
        if(dataW != dataR)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
    }

    if( rtk_oam_multiplexerAction_get(0, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( rtk_oam_parserAction_get(0, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( rtk_trap_oamPduAction_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( rtk_trap_oamPduPri_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
#endif        
    return RT_ERR_OK;
}    



