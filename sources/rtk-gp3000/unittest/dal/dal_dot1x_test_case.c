#include <osal/lib.h>
#include <osal/print.h>
#include <ioal/mem32.h>
#include <hal/common/halctrl.h>
#include <common/error.h>
#include <dal/dal_dot1x_test_case.h>
#include <rtk/dot1x.h>
#include <common/unittest_util.h>

int32 dal_dot1x_unauthPacketOper_test(uint32 caseNo)
{
	rtk_action_t actW;
	rtk_action_t actR;
	rtk_port_t port;

    /*error input check*/
    /*out of range*/
    if(rtk_dot1x_unauthPacketOper_set(HAL_GET_MAX_PORT() + 1, ACTION_DROP) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

	if(rtk_dot1x_unauthPacketOper_set(0, ACTION_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
	
    /*null pointer*/
    if(rtk_dot1x_unauthPacketOper_get(0, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* Get/set */
    HAL_SCAN_ALL_PORT(port)
    {
        for(actW = ACTION_FORWARD; actW < ACTION_END; actW++)
        {
            if( (actW == ACTION_DROP) || (actW == ACTION_TRAP2CPU) || (actW == ACTION_TO_GUESTVLAN) )
            {
                if(rtk_dot1x_unauthPacketOper_set(port, actW) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }

                if(rtk_dot1x_unauthPacketOper_get(port, &actR) != RT_ERR_OK)
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

int32 dal_dot1x_portBasedEnable_test(uint32 caseNo)
{
	rtk_enable_t stateW;
	rtk_enable_t stateR;
	rtk_port_t port;

    /*error input check*/
    /*out of range*/
	if(rtk_dot1x_portBasedEnable_set(HAL_GET_MAX_PORT() + 1, DISABLED) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
	
	if(rtk_dot1x_portBasedEnable_set(0, RTK_ENABLE_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

	/*null pointer*/
	if(rtk_dot1x_portBasedEnable_get(0, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
	
	/* Get/set */
	HAL_SCAN_ALL_PORT(port)
    {
        for(stateW = DISABLED; stateW < RTK_ENABLE_END; stateW++)
    	{
    		if(rtk_dot1x_portBasedEnable_set(port, stateW) != RT_ERR_OK)
			{
				osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
				return RT_ERR_FAILED;
			}

			if(rtk_dot1x_portBasedEnable_get(port, &stateR) != RT_ERR_OK)
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



int32 dal_dot1x_portBasedAuthStatus_test(uint32 caseNo)
{
	rtk_dot1x_auth_status_t stateW;
	rtk_dot1x_auth_status_t stateR;
	rtk_port_t port;

	/*error input check*/
    /*out of range*/
    if(rtk_dot1x_portBasedAuthStatus_set(HAL_GET_MAX_PORT() + 1, UNAUTH) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
	
	if(rtk_dot1x_portBasedAuthStatus_set(0, AUTH_STATUS_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

	/*null pointer*/
	if(rtk_dot1x_portBasedAuthStatus_get(0, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
	
	/* Get/set */
	HAL_SCAN_ALL_PORT(port)
    {
        for(stateW = UNAUTH; stateW < AUTH_STATUS_END; stateW++)
    	{
    		if(rtk_dot1x_portBasedAuthStatus_set(port, stateW) != RT_ERR_OK)
			{
				osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
				return RT_ERR_FAILED;
			}

			if(rtk_dot1x_portBasedAuthStatus_get(port, &stateR) != RT_ERR_OK)
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


int32 dal_dot1x_portBasedDirection_test(uint32 caseNo)
{
	rtk_dot1x_direction_t directionW;
	rtk_dot1x_direction_t directionR;
	rtk_port_t port;

	/*error input check*/
    /*out of range*/
    if(rtk_dot1x_portBasedDirection_set(HAL_GET_MAX_PORT() + 1, BOTH) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
	
	if(rtk_dot1x_portBasedDirection_set(0, DIRECTION_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

	/*null pointer*/
	if(rtk_dot1x_portBasedDirection_get(0, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
	
	/* Get/set */
	HAL_SCAN_ALL_PORT(port)
    {
        for(directionW = BOTH; directionW < DIRECTION_END; directionW++)
    	{
    		if(rtk_dot1x_portBasedDirection_set(port, directionW) != RT_ERR_OK)
			{
				osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
				return RT_ERR_FAILED;
			}

			if(rtk_dot1x_portBasedDirection_get(port, &directionR) != RT_ERR_OK)
			{
				osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
				return RT_ERR_FAILED;
			}

			if(directionW != directionR)
			{
				osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
				return RT_ERR_FAILED;
			}
		}
	}
	
	return RT_ERR_OK;
}


int32 dal_dot1x_macBasedEnable_test(uint32 caseNo)
{
	rtk_enable_t stateW;
	rtk_enable_t stateR;
	rtk_port_t port;

	/*error input check*/
    /*out of range*/
    if(rtk_dot1x_macBasedEnable_set(HAL_GET_MAX_PORT() + 1, DISABLED) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
	
	if(rtk_dot1x_macBasedEnable_set(0, RTK_ENABLE_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

	/*null pointer*/
	if(rtk_dot1x_macBasedEnable_get(0, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
	
	/* Get/set */
	HAL_SCAN_ALL_PORT(port)
    {
        for(stateW = DISABLED; stateW < RTK_ENABLE_END; stateW++)
    	{
    		if(rtk_dot1x_macBasedEnable_set(port, stateW) != RT_ERR_OK)
			{
				osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
				return RT_ERR_FAILED;
			}

			if(rtk_dot1x_macBasedEnable_get(port, &stateR) != RT_ERR_OK)
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


int32 dal_dot1x_macBasedDirection_test(uint32 caseNo)
{
	rtk_dot1x_direction_t directionW;
	rtk_dot1x_direction_t directionR;

	/*error input check*/
    /*out of range*/
    if(rtk_dot1x_macBasedDirection_set(DIRECTION_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

	/*null pointer*/
	if(rtk_dot1x_macBasedDirection_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
	
	/* Get/set */
	for(directionW = BOTH; directionW < DIRECTION_END; directionW++)
	{
		if(rtk_dot1x_macBasedDirection_set(directionW) != RT_ERR_OK)
		{
			osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
			return RT_ERR_FAILED;
		}

		if(rtk_dot1x_macBasedDirection_get(&directionR) != RT_ERR_OK)
		{
			osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
			return RT_ERR_FAILED;
		}

		if(directionW != directionR)
		{
			osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
			return RT_ERR_FAILED;
		}
	}
	
	return RT_ERR_OK;
}



int32 dal_dot1x_guestVlan_test(uint32 caseNo)
{
	rtk_vlan_t vlanW;
	rtk_vlan_t vlanR;

	/*error input check*/
    /*out of range*/
    if(rtk_dot1x_guestVlan_set(RTK_VLAN_ID_MAX + 1) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

	/*null pointer*/
	if(rtk_dot1x_guestVlan_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
	
	/* Get/set */
	for(vlanW = 1; vlanW <= RTK_VLAN_ID_MAX; vlanW += (RTK_VLAN_ID_MAX-1))
	{
		if(rtk_dot1x_guestVlan_set(vlanW) != RT_ERR_OK)
		{
			osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
			return RT_ERR_FAILED;
		}

		if(rtk_dot1x_guestVlan_get(&vlanR) != RT_ERR_OK)
		{
			osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
			return RT_ERR_FAILED;
		}

		if(vlanW != vlanR)
		{
			osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
			return RT_ERR_FAILED;
		}
	}
	
	return RT_ERR_OK;
}


int32 dal_dot1x_guestVlanBehavior_test(uint32 caseNo)
{
	rtk_dot1x_guestVlanBehavior_t gvlanBehaviorW;
	rtk_dot1x_guestVlanBehavior_t gvlanBehaviorR;

	/*error input check*/
    /*out of range*/
    if(rtk_dot1x_guestVlanBehavior_set(GUEST_VLAN_BEHAVIOR_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

	/*null pointer*/
	if(rtk_dot1x_guestVlanBehavior_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
	
	/* Get/set */
	for(gvlanBehaviorW = DISALLOW_TO_AUTH_DA; gvlanBehaviorW < GUEST_VLAN_BEHAVIOR_END; gvlanBehaviorW++)
	{
		if(rtk_dot1x_guestVlanBehavior_set(gvlanBehaviorW) != RT_ERR_OK)
		{
			osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
			return RT_ERR_FAILED;
		}

		if(rtk_dot1x_guestVlanBehavior_get(&gvlanBehaviorR) != RT_ERR_OK)
		{
			osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
			return RT_ERR_FAILED;
		}

		if(gvlanBehaviorW != gvlanBehaviorR)
		{
			osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
			return RT_ERR_FAILED;
		}
	}
	
	return RT_ERR_OK;
}


int32 dal_dot1x_trapPri_test(uint32 caseNo)
{
	rtk_pri_t priW;
	rtk_pri_t priR;

	/*error input check*/
    /*out of range*/
    if(rtk_dot1x_trapPri_set(HAL_INTERNAL_PRIORITY_MAX() + 1) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

	/*null pointer*/
	if(rtk_dot1x_trapPri_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
	
	/* Get/set */
	for(priW = 0; priW <= HAL_INTERNAL_PRIORITY_MAX(); priW++)
	{
		if(rtk_dot1x_trapPri_set(priW) != RT_ERR_OK)
		{
			osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
			return RT_ERR_FAILED;
		}

		if(rtk_dot1x_trapPri_get(&priR) != RT_ERR_OK)
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
	
	return RT_ERR_OK;
}

