#include <osal/lib.h>
#include <common/debug/rt_log.h>
#include <dal/apollo/raw/apollo_raw_meter.h>
#include <ioal/mem32.h>
#include <dal/raw/apollo/dal_raw_meter_test_case.h>

int32 dal_meter_raw_test(uint32 caseNo)
{
    uint32 meterIdx;
    uint32 meterRateW;
    uint32 meterRateR;
    rtk_enable_t ifgW;
    rtk_enable_t ifgR;
    uint32 lbthresholdW;
    uint32 lbthresholdR;
    uint32 tickPeriodW;
    uint32 tickPeriodR;
    uint32 tknW;
    uint32 tknR;
    uint32 ponTickPeriodR;
    uint32 ponTknR;
    apollo_raw_meter_op_t meterOpW;
    apollo_raw_meter_op_t meterOpR;
    uint32 regData;
    uint32 wData;
    uint32 dataMask;
    uint32 checkData;
    uint32 index;

    /*error input check*/
    /*out of range*/
    if( apollo_raw_meter_rate_set(HAL_MAX_NUM_OF_METERING(), APOLLO_RATE_MAX) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if( apollo_raw_meter_rate_set(HAL_MAX_NUM_OF_METERING() - 1, HAL_MAX_NUM_OF_METERING()) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if( apollo_raw_meter_ifg_set(HAL_MAX_NUM_OF_METERING() , RTK_ENABLE_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if( apollo_raw_meter_ifg_set(HAL_MAX_NUM_OF_METERING() - 1 , RTK_ENABLE_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if( apollo_raw_meter_bucketSize_set(HAL_MAX_NUM_OF_METERING(), APOLLO_METER_BUCKETSIZE_MAX) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if( apollo_raw_meter_bucketSize_set(HAL_MAX_NUM_OF_METERING() - 1, APOLLO_METER_BUCKETSIZE_MAX + 1) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if( apollo_raw_meter_exceed_set(HAL_MAX_NUM_OF_METERING()) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if( apollo_raw_meter_btCtr_set(APOLLO_RAW_METER_TICK_PERIOD_MAX + 1, APOLLO_RAW_METER_TKN_MAX) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if( apollo_raw_meter_btCtr_set(APOLLO_RAW_METER_TICK_PERIOD_MAX, APOLLO_RAW_METER_TKN_MAX +1) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if( apollo_raw_meter_ponBtCtr_set(APOLLO_RAW_METER_TICK_PERIOD_MAX + 1, APOLLO_RAW_METER_TKN_MAX) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if( apollo_raw_meter_ponBtCtr_set(APOLLO_RAW_METER_TICK_PERIOD_MAX, APOLLO_RAW_METER_TKN_MAX + 1) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }


    /* get/set test*/
    for (meterIdx = 0; meterIdx < HAL_MAX_NUM_OF_METERING(); meterIdx++)
    {
        for ( meterRateW = 1; meterRateW < APOLLO_RATE_MAX; meterRateW = (meterRateW<<1))
        {
            if( apollo_raw_meter_rate_set(meterIdx, meterRateW) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
            if( apollo_raw_meter_rate_get(meterIdx, &meterRateR) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
            if(meterRateW != meterRateR)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
        }

        for ( ifgW = 0 ; ifgW < RTK_ENABLE_END; ifgW++)
        {
            if( apollo_raw_meter_ifg_set(meterIdx, ifgW) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
            if( apollo_raw_meter_ifg_get(meterIdx, &ifgR) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
            if(ifgR != ifgW)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

        }


        for ( lbthresholdW = 0 ; lbthresholdW < RTK_ENABLE_END; lbthresholdW++)
        {
            if( apollo_raw_meter_bucketSize_set(meterIdx, lbthresholdW) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
            if( apollo_raw_meter_bucketSize_get(meterIdx, &lbthresholdR) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
            if(lbthresholdW != lbthresholdR)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

        }

        /*if( apollo_raw_meter_exceed_set(meterIdx) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }*/

    }

    tknW = 0xee;
    for (tickPeriodW = 0 ; tickPeriodW <= APOLLO_RAW_METER_TICK_PERIOD_MAX ; tickPeriodW++)
    {
        if( apollo_raw_meter_btCtr_set(tickPeriodW, tknW) != RT_ERR_OK)
        {
           osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
           return RT_ERR_FAILED;
        }
        if( apollo_raw_meter_btCtr_get(&tickPeriodR, &tknR) != RT_ERR_OK)
        {
           osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
           return RT_ERR_FAILED;
        }
        if(tknW != tknR || tickPeriodW != tickPeriodR)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if( apollo_raw_meter_ponBtCtr_set(tickPeriodW, tknW) != RT_ERR_OK)
        {
           osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
           return RT_ERR_FAILED;
        }
        if( apollo_raw_meter_ponBtCtr_get(&ponTickPeriodR, &ponTknR) != RT_ERR_OK)
        {
           osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
           return RT_ERR_FAILED;
        }
        if(tknW != ponTknR || tickPeriodW != ponTickPeriodR)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }


    }

    tickPeriodW = 0xee;
    for (tknW = 0 ; tknW <= APOLLO_RAW_METER_TKN_MAX; tknW++)
    {
        if( apollo_raw_meter_btCtr_set(tickPeriodW, tknW) != RT_ERR_OK)
        {
           osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
           return RT_ERR_FAILED;
        }
        if( apollo_raw_meter_btCtr_get(&tickPeriodR, &tknR) != RT_ERR_OK)
        {
           osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
           return RT_ERR_FAILED;
        }
        if(tknW != tknR || tickPeriodW != tickPeriodR)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

        if( apollo_raw_meter_ponBtCtr_set(tickPeriodW, tknW) != RT_ERR_OK)
        {
           osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
           return RT_ERR_FAILED;
        }
        if( apollo_raw_meter_ponBtCtr_get(&ponTickPeriodR, &ponTknR) != RT_ERR_OK)
        {
           osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
           return RT_ERR_FAILED;
        }
        if(tknW != ponTknR || tickPeriodW != ponTickPeriodR)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }
    meterOpW = APOLLO_RAW_METER_OP_CONSUMETKN;
    if( apollo_raw_meter_btCtrOp_set(meterOpW) != RT_ERR_OK)
    {
       osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
       return RT_ERR_FAILED;
    }
    if( apollo_raw_meter_btCtrOp_get(&meterOpR) != RT_ERR_OK)
    {
       osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
       return RT_ERR_FAILED;
    }
    if(meterOpW != meterOpR)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    meterOpW = APOLLO_RAW_METER_OP_CONSUMETKN;
    if( apollo_raw_meter_ponBtCtrOp_set(meterOpW) != RT_ERR_OK)
    {
       osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
       return RT_ERR_FAILED;
    }
    if( apollo_raw_meter_ponBtCtrOp_get(&meterOpR) != RT_ERR_OK)
    {
       osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
       return RT_ERR_FAILED;
    }
    if(meterOpW != meterOpR)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*null pointer*/
    meterIdx = 1;
    if ( apollo_raw_meter_rate_get(meterIdx, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if ( apollo_raw_meter_ifg_get(meterIdx, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if ( apollo_raw_meter_bucketSize_get(meterIdx, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if ( apollo_raw_meter_exceed_get(meterIdx, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if ( apollo_raw_meter_btCtr_get(NULL, &tknR) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if ( apollo_raw_meter_btCtr_get(&tickPeriodR, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if ( apollo_raw_meter_ponBtCtr_get(NULL, &ponTknR) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if ( apollo_raw_meter_ponBtCtr_get(&ponTickPeriodR, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if ( apollo_raw_meter_btCtrOp_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if ( apollo_raw_meter_ponBtCtrOp_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*register access test*/
    for (index = 0; index < 32; index++)
    {
        ioal_mem32_write(HAL_GET_REG_ADDR(METER_GLB_CTRLr) + 8*index, 0x0);
        apollo_raw_meter_rate_set(index, 0x1ffff);
        ioal_mem32_read(HAL_GET_REG_ADDR(METER_GLB_CTRLr) + 8*index,&regData);
        dataMask = 0x1ffff;
        checkData =0x1ffff;
        /*mask out reserve bits*/
        regData = regData & dataMask;
        if(regData != checkData)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

    }

    for (index = 0; index < 32; index++)
    {
        ioal_mem32_write(HAL_GET_REG_ADDR(METER_GLB_CTRLr) + 8*index + 4, 0x0);
        apollo_raw_meter_ifg_set(index, ENABLED);
        ioal_mem32_read(HAL_GET_REG_ADDR(METER_GLB_CTRLr) + 8*index + 4,&regData);
        dataMask = 0x1;
        checkData =ENABLED;
        /*mask out reserve bits*/
        regData = regData & dataMask;
        if(regData != checkData)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

    }
#if 0 /*This would reset the register value to 0*/
    for (index = 0; index < 32; index++)
    {
        ioal_mem32_write(HAL_GET_REG_ADDR(METER_LB_EXCEED_STSr), 0x0);
        apollo_raw_meter_exceed_set(index);
        ioal_mem32_read(HAL_GET_REG_ADDR(METER_LB_EXCEED_STSr),&regData);
        osal_printf("%x,", regData);
        dataMask = 0xffff;
        checkData = 1<<index;
        /*mask out reserve bits*/
        regData = regData & dataMask;
        if(regData != checkData)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }
#endif
    for (index = 0; index < 32; index++)
    {
        ioal_mem32_write(HAL_GET_REG_ADDR(METER_GLB_CTRLr) + 8*index + 4, 0x0);
        apollo_raw_meter_bucketSize_set(index, APOLLO_METER_BUCKETSIZE_MAX);
        ioal_mem32_read(HAL_GET_REG_ADDR(METER_GLB_CTRLr) + 8*index + 4, &regData);
        dataMask = 0x1fffe;
        checkData =0x1fffe;
        /*mask out reserve bits*/
        regData = regData & dataMask;
        if(regData != checkData)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }

    }


    ioal_mem32_write(HAL_GET_REG_ADDR(METER_TB_CTRLr), 0x0);
    apollo_raw_meter_btCtr_set(APOLLO_RAW_METER_TICK_PERIOD_MAX, 0);
    ioal_mem32_read(HAL_GET_REG_ADDR(METER_TB_CTRLr),&regData);
    dataMask = 0xff00;
    checkData =0xff00;
    /*mask out reserve bits*/
    regData = regData & dataMask;
    if(regData != checkData)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    ioal_mem32_write(HAL_GET_REG_ADDR(METER_TB_CTRLr), 0x0);
    apollo_raw_meter_btCtr_set(0, APOLLO_RAW_METER_TKN_MAX);
    ioal_mem32_read(HAL_GET_REG_ADDR(METER_TB_CTRLr),&regData);
    dataMask = 0xff;
    checkData =0xff;
    /*mask out reserve bits*/
    regData = regData & dataMask;
    if(regData != checkData)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    ioal_mem32_write(HAL_GET_REG_ADDR(METER_TB_CTRLr), 0x0);
    apollo_raw_meter_btCtrOp_set(APOLLO_RAW_METER_OP_CONSUMETKN);
    ioal_mem32_read(HAL_GET_REG_ADDR(METER_TB_CTRLr),&regData);
    dataMask = 0x10000;
    checkData =0x10000;
    /*mask out reserve bits*/
    regData = regData & dataMask;
    if(regData != checkData)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    ioal_mem32_write(HAL_GET_REG_ADDR(PON_TB_CTRLr), 0x0);
    apollo_raw_meter_ponBtCtr_set(APOLLO_RAW_METER_TICK_PERIOD_MAX, 0);
    ioal_mem32_read(HAL_GET_REG_ADDR(PON_TB_CTRLr),&regData);
    dataMask = 0xff00;
    checkData =0xff00;
    /*mask out reserve bits*/
    regData = regData & dataMask;
    if(regData != checkData)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    ioal_mem32_write(HAL_GET_REG_ADDR(PON_TB_CTRLr), 0x0);
    apollo_raw_meter_ponBtCtr_set(0, APOLLO_RAW_METER_TKN_MAX);
    ioal_mem32_read(HAL_GET_REG_ADDR(PON_TB_CTRLr),&regData);
    dataMask = 0xff;
    checkData =0xff;
    /*mask out reserve bits*/
    regData = regData & dataMask;
    if(regData != checkData)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    ioal_mem32_write(HAL_GET_REG_ADDR(PON_TB_CTRLr), 0x0);
    apollo_raw_meter_ponBtCtrOp_set(APOLLO_RAW_METER_OP_CONSUMETKN);
    ioal_mem32_read(HAL_GET_REG_ADDR(PON_TB_CTRLr),&regData);
    dataMask = 0x10000;
    checkData =0x10000;
    /*mask out reserve bits*/
    regData = regData & dataMask;
    if(regData != checkData)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;

}

