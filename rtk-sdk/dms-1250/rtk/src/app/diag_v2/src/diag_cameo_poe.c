/*
 * Include Files
 */
#include <common/rt_autoconf.h>
#include <common/rt_error.h>
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <common/debug/rt_log.h>
#include <diag_util.h>
#include <diag_om.h>
#include <parser/cparser_priv.h>
#include <drv/uart/uart.h>


/* poe frame structure define*/
typedef struct POEDRV_TYPE_PktBuf_RTK_S   
{	
    unsigned char      command;                           /* operation code              */
    unsigned char      seq_no;                            /* sequence number             */
    unsigned char      data1;                             /* DATA1 field                 */
    unsigned char      data2;                             /* DATA2 field                 */
    unsigned char      data3;                             /* DATA3 field                 */
    unsigned char      data4;                             /* DATA4 field                 */
    unsigned char      data5;                             /* DATA5 field                 */
    unsigned char      data6;                             /* DATA6 field                 */
    unsigned char      data7;                             /* DATA7 field                 */
    unsigned char      data8;                             /* DATA8 field                 */
    unsigned char      data9;                             /* DATA9 field                 */
    unsigned char      checksum;                          /* checksum                    */

}POEDRV_TYPE_PktBuf_RTK_T;

enum
{
  POE_RTK_DRV_ACK = 0,
  POE_RTK_DRV_NACK
};

#define POE_RTK_NA_FIELD_VALUE      0xFF
#define RTK_MSG_LEN                 sizeof(POEDRV_TYPE_PktBuf_RTK_T) /* bytes */

#define POEDRV_RTK_COM_GLOBAL_PSE_SET                   0x00
#define POEDRV_RTK_COM_POWER_MANAGE_MODE_SET            0x10
#define POEDRV_RTK_COM_SYSTEM_POWER_GUARD_SET           0x04
#define POEDRV_RTK_COM_TOTAL_POWER_ALLOCATED            0x41
#define POEDRV_RTK_COM_PM_CONFIGURATION_GET             0x4B

unsigned char   CmdBuffer[RTK_MSG_LEN];
unsigned char   RespBuffer[RTK_MSG_LEN];

int CAMEO_PoeSetPowerManageMode (unsigned int mode);
int CAMEO_PoeSetGlobalAdmin(unsigned int admin_status);
int CAMEO_PoeGetTotalPowerAllocated (uint16 *totalPower, uint16 *availPower, uint16 *mpsm_state);
int CAMEO_PoeSetSystemPowerBudget (unsigned short psu_watts, unsigned short guard_band, unsigned char mpss);

extern void osal_time_usleep(uint32 usec);

/* -------------------------------------------------------------------------
* Purpose     : To set the buffer to given value
* ************************************************************************ */
void CAMEO_POEDRV_memset(void *dptr, unsigned char val, unsigned short len)
{
    memset(dptr, val, len);
}

/* --------------------------------------------------------------------------
*  Purpose     : RTL8238B POE driver to insert checksum
*  ************************************************************************ */
void POEDRV_RTKInsertCheckSum(POEDRV_TYPE_PktBuf_RTK_T *pCmd)
{
    /* Last byte carries the check sum */
    pCmd->checksum = pCmd->command + pCmd->seq_no + pCmd->data1 +
                      pCmd->data2 + pCmd->data3 + pCmd->data4 +
                      pCmd->data5 + pCmd->data6 + pCmd->data7 +
                      pCmd->data8 + pCmd->data9;
}


/*******************************************************************************
 *	  Function Name 		: CmIssUartWriteData
 *	  Description			: Send the PoE Uart Data 
 *	  Iuput(s) 				: unsigned char *pkt - PoE Command
 *	  Output(s) 			: None
 *	  Returns				: CPARSER_OK/CPARSER_NOT_OK
 *****************************************************************************/
int CAMEO_IssUartWriteData(unsigned char *pkt)
{
    uint32  unit;
    uint8   len;
    
    unit = 0;
    len = 12;
    
    drv_uart_putc(unit, len, &pkt[0]);
	return CPARSER_OK;
}


/*******************************************************************************
 *	  Function Name 		: CmIssUartReadData
 *	  Description			: Read the PoE Uart Data 
 *	  Iuput(s) 				: unsigned char *pkt - PoE Command
 *	  Output(s) 			: None
 *	  Returns				: CPARSER_OK/CPARSER_NOT_OK
 *****************************************************************************/
int CAMEO_IssUartReadData(unsigned char *pkt)
{
    uint32  unit;
	uint8   len;
	
	unit = 0;
    len = 12;
    drv_uart_getc(unit, len, &pkt[0], 1);
	return CPARSER_OK;
}


/*****************************************************************************
 *	  Function Name 		: CmHwNpPoEUartSendcmdDrv
 *	  Description			: PoE Uart Send command dummy driver
 *	  Iuput(s) 				: None
 *	  Output(s) 			: None
 *	  Returns				: CPARSER_OK/CPARSER_NOT_OK
 *****************************************************************************/
int CAMEO_HwNpPoEUartSendcmdDrv(uint8 *pCommand, uint8 *pResp)
{
    int i4reval;
    int i4retryval;
   
    i4reval = CPARSER_OK;
    i4retryval = 0;

    while(i4retryval < 3)
	{
#if 0	    
	    diag_util_printf("pCommand->key = 0x%x\r\n",pCommand[0]);
        diag_util_printf("pCommand->echo = 0x%x\r\n",pCommand[1]);
        diag_util_printf("pCommand->data1 = 0x%x\r\n",pCommand[2]);
        diag_util_printf("pCommand->data2 = 0x%x\r\n",pCommand[3]);
        diag_util_printf("pCommand->data3 = 0x%x\r\n",pCommand[4]);
        diag_util_printf("pCommand->data4 = 0x%x\r\n",pCommand[5]);
        diag_util_printf("pCommand->data5 = 0x%x\r\n",pCommand[6]);
        diag_util_printf("pCommand->data6 = 0x%x\r\n",pCommand[7]);
        diag_util_printf("pCommand->data7 = 0x%x\r\n",pCommand[8]);
        diag_util_printf("pCommand->data8 = 0x%x\r\n",pCommand[9]);
        diag_util_printf("pCommand->data9 = 0x%x\r\n",pCommand[10]);
        diag_util_printf("pCommand->checksum = 0x%x\r\n",pCommand[11]);
#endif        
        /* send pCommand to PoE controller */
        CAMEO_IssUartWriteData(pCommand); 

        /* For Broadcom Uart Rule inter packet gap must over 10ms */
        osal_time_usleep(20 * 1000);

        /* get pResp from PoE controller */
        CAMEO_IssUartReadData(pResp); 

        osal_time_usleep(20 * 1000);

#if 0
        diag_util_printf("pResp->key = 0x%x\r\n",pResp[0]);
        diag_util_printf("pResp->echo = 0x%x\r\n",pResp[1]);
        diag_util_printf("pResp->data1 = 0x%x\r\n",pResp[2]);
        diag_util_printf("pResp->data2 = 0x%x\r\n",pResp[3]);
        diag_util_printf("pResp->data3 = 0x%x\r\n",pResp[4]);
        diag_util_printf("pResp->data4 = 0x%x\r\n",pResp[5]);
        diag_util_printf("pResp->data5 = 0x%x\r\n",pResp[6]);
        diag_util_printf("pResp->data6 = 0x%x\r\n",pResp[7]);
        diag_util_printf("pResp->data7 = 0x%x\r\n",pResp[8]);
        diag_util_printf("pResp->data8 = 0x%x\r\n",pResp[9]);
        diag_util_printf("pResp->data9 = 0x%x\r\n",pResp[10]);
        diag_util_printf("pResp->checksum = 0x%x\r\n",pResp[11]);
#endif
         
        /* Check response of Clear Image and CRC check Command for BCM59111 and BCM59121 */
        if ((pCommand[0] == 0xE0) && (pCommand[1] == 0x40 || pCommand[1] == 0xC0) && (pResp[0] == 0xAF))
        {
            i4reval = CPARSER_OK;
            break;
        }
        /* Check response of Other Command for RTL8238B and BCM59121 */
        else if((pCommand[0]) == (pResp[0]) && (pCommand[1]) == (pResp[1]))
        {
            i4reval = CPARSER_OK;
            break;
        }
        else
        {
            diag_util_printf("CmHwNpPoEUartSendcmdDrv Retry!\r\n");       
            i4retryval++;
        }
    }
    
    if(i4retryval >= 3)
    {
        i4reval = CPARSER_NOT_OK;
    }
            
    return i4reval;
}


unsigned int RTK_UART_POE_cmd(unsigned char *pCommand, unsigned char *pResp)	/* for Commands and Programs */
{

    if(CAMEO_HwNpPoEUartSendcmdDrv(pCommand, pResp) != CPARSER_OK)
    {
        diag_util_printf(" %s : failed! [%d] \r\n",__FUNCTION__, __LINE__);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}


int CAMEO_PoeSetPowerManageMode (unsigned int mode)
{
    unsigned char res = CPARSER_NOT_OK;
    
    POEDRV_TYPE_PktBuf_RTK_T *pCmd = (POEDRV_TYPE_PktBuf_RTK_T *) CmdBuffer;
    POEDRV_TYPE_PktBuf_RTK_T *pResp = (POEDRV_TYPE_PktBuf_RTK_T *) RespBuffer;
    
    /* fill poe message buffer */
    CAMEO_POEDRV_memset(pCmd, POE_RTK_NA_FIELD_VALUE, RTK_MSG_LEN);
    CAMEO_POEDRV_memset(pResp, POE_RTK_NA_FIELD_VALUE, RTK_MSG_LEN);
    
    pCmd->command = POEDRV_RTK_COM_POWER_MANAGE_MODE_SET;
    pCmd->seq_no = 0x1;
    pCmd->data1 = mode; 
    
    POEDRV_RTKInsertCheckSum(pCmd);
    
    if(RTK_UART_POE_cmd((unsigned char *)pCmd, (unsigned char *)pResp) == CPARSER_OK)
    {
        if(pResp->data1 == POE_RTK_DRV_ACK)
        {
            res = CPARSER_OK; /* No error*/
        }
        else
        {
            diag_util_printf("PoeSetPowerManageMode Failed!\n");
        }
    }
    else
    {
        diag_util_printf("RTK_UART_POE_cmd: PoeSetPowerManageMode Failed!\n");
    }
    
    return res;
}


/*****************************************************************************
*     Function Name    : PoeSetGlobalAdmin (command 0x00)
*     Description      : This function sets the PSE functionality on all ports.
*     Input            : i1PoeStatus - 
*                        0 - Disable PSE functionality on all ports.
*                        1 - Enable PSE functionality on all ports.
*     Output           : None          
*****************************************************************************/
int CAMEO_PoeSetGlobalAdmin(unsigned int admin_status)
{
    unsigned char res = CPARSER_NOT_OK;

    POEDRV_TYPE_PktBuf_RTK_T *pCmd = (POEDRV_TYPE_PktBuf_RTK_T *) CmdBuffer;
    POEDRV_TYPE_PktBuf_RTK_T *pResp = (POEDRV_TYPE_PktBuf_RTK_T *) RespBuffer;
    
    /* fill poe message buffer */
    CAMEO_POEDRV_memset(pCmd, POE_RTK_NA_FIELD_VALUE, RTK_MSG_LEN);
    CAMEO_POEDRV_memset(pResp, POE_RTK_NA_FIELD_VALUE, RTK_MSG_LEN);
    
    pCmd->command = POEDRV_RTK_COM_GLOBAL_PSE_SET;
    pCmd->seq_no = 0x00;
    pCmd->data1 = admin_status;
    
    POEDRV_RTKInsertCheckSum(pCmd);
    
    if(RTK_UART_POE_cmd((unsigned char *)pCmd, (unsigned char *)pResp) == CPARSER_OK)
    {
        if(pResp->data1 == POE_RTK_DRV_ACK)
        {
            res = CPARSER_OK; /* No error*/
        }
        else
        {
            diag_util_printf("PoeSetGlobalAdmin Failed!\n");
        }
    }
    else
    {
        diag_util_printf("RTK_UART_POE_cmd: PoeSetGlobalAdmin Failed!\n");
    }
    
    return res;
}


/*****************************************************************************
*     Function Name    : PoeGetTotalPowerAllocated (command 0x23)
*     Description      : This function acquires the total power allocated in the PoE subsystem.
*     Input            : None
*     Output           : totalPower  - total power allocated to the port 0.1 W/LSB.
*                        availPower  - the configuard power availabed from the PoE subsystem.         
*****************************************************************************/
int CAMEO_PoeGetTotalPowerAllocated (uint16 *totalPower, uint16 *availPower, uint16 *mpsm_state)
{
    unsigned char res = CPARSER_NOT_OK;
    uint16  psu_watts, guard_band;

    POEDRV_TYPE_PktBuf_RTK_T *pCmd = (POEDRV_TYPE_PktBuf_RTK_T *) CmdBuffer;
    POEDRV_TYPE_PktBuf_RTK_T *pResp = (POEDRV_TYPE_PktBuf_RTK_T *) RespBuffer;
    
    /* fill poe message buffer */
    CAMEO_POEDRV_memset(pCmd, POE_RTK_NA_FIELD_VALUE, RTK_MSG_LEN);
    CAMEO_POEDRV_memset(pResp, POE_RTK_NA_FIELD_VALUE, RTK_MSG_LEN);
    
    pCmd->command = POEDRV_RTK_COM_TOTAL_POWER_ALLOCATED;
    pCmd->seq_no = 0x01;
    
    POEDRV_RTKInsertCheckSum(pCmd);
    
    
    if(RTK_UART_POE_cmd((unsigned char *)pCmd, (unsigned char *)pResp) == CPARSER_OK)
    {
        *totalPower = 0;
        *totalPower = pResp->data6;
        *totalPower = ((*totalPower << 8) | pResp->data7);
        *mpsm_state = pResp->data5;   
        /* No error */
    }
    else
    {
        diag_util_printf("PoeGetTotalPowerAllocated Failed!\n");
        return res;
    }
    
    /* fill poe message buffer */
    CAMEO_POEDRV_memset(pCmd, POE_RTK_NA_FIELD_VALUE, RTK_MSG_LEN);
    CAMEO_POEDRV_memset(pResp, POE_RTK_NA_FIELD_VALUE, RTK_MSG_LEN);
    
    pCmd->command = POEDRV_RTK_COM_PM_CONFIGURATION_GET;
    pCmd->seq_no = *mpsm_state;
    
    POEDRV_RTKInsertCheckSum(pCmd);
    
    if(RTK_UART_POE_cmd((unsigned char *)pCmd, (unsigned char *)pResp) == CPARSER_OK)
    {
        psu_watts = 0;
        guard_band = 0;
        *availPower = 0;
        psu_watts = pResp->data2;
        psu_watts = ((psu_watts << 8) | pResp->data3);
        guard_band = pResp->data4;
        guard_band = (guard_band << 8) | pResp->data5;
        *availPower = (psu_watts - guard_band);
        /* No error */
    }
    else
    {
        diag_util_printf("PoeGetTotalPowerAllocated Failed!\n");
    }
    
    return res;
}


/*****************************************************************************
*     Function Name    : PoeSetSystemPowerBudget (command 0x18)
*     Description      : This function sets the power source details.
*     Input            : psu_watts  - total power 0.1 W/LSB.
*                        guard_band - guard band 0.1 W/LSB.
*                        mpss       - 0-7, multi power source status pin          
*****************************************************************************/
int CAMEO_PoeSetSystemPowerBudget (unsigned short psu_watts, unsigned short guard_band, unsigned char mpss)
{
    unsigned char res = CPARSER_NOT_OK;

    POEDRV_TYPE_PktBuf_RTK_T *pCmd = (POEDRV_TYPE_PktBuf_RTK_T *) CmdBuffer;
    POEDRV_TYPE_PktBuf_RTK_T *pResp = (POEDRV_TYPE_PktBuf_RTK_T *) RespBuffer;
    
    /* fill poe message buffer */
    CAMEO_POEDRV_memset(pCmd, POE_RTK_NA_FIELD_VALUE, RTK_MSG_LEN);
    CAMEO_POEDRV_memset(pResp, POE_RTK_NA_FIELD_VALUE, RTK_MSG_LEN);
    
    
    pCmd->command = POEDRV_RTK_COM_SYSTEM_POWER_GUARD_SET;
    pCmd->seq_no = 0x1;
    pCmd->data1 = mpss;
    pCmd->data2 = (psu_watts >> 8) & 0xFF;
    pCmd->data3 = psu_watts & 0xFF;
    pCmd->data4 = (guard_band >> 8) & 0xFF;
    pCmd->data5 = guard_band & 0xFF;
    
    POEDRV_RTKInsertCheckSum(pCmd);
    
    if(RTK_UART_POE_cmd((unsigned char *)pCmd, (unsigned char *)pResp) == CPARSER_OK)
    {
        /*
          diag_util_printf("pResp->data1 0x%x\n", pResp->data1);  	  	
          diag_util_printf("pResp->data2 0x%x\n", pResp->data2);  
          diag_util_printf("pResp->data3 0x%x\n", pResp->data3);  	  	
          diag_util_printf("pResp->data4 0x%x\n", pResp->data4);  	  	
          diag_util_printf("pResp->data5 0x%x\n", pResp->data5);  	  	
          diag_util_printf("pResp->data6 0x%x\n", pResp->data6);  
          diag_util_printf("pResp->data7 0x%x\n", pResp->data7);  	  	
          diag_util_printf("pResp->data8 0x%x\n", pResp->data8);              
          diag_util_printf("pResp->data9 0x%x\n", pResp->data9); 
        */
        
        if( pResp->data2 == POE_RTK_DRV_ACK)
        {
            res = CPARSER_OK; /* No error*/
        }
        else
        {
            diag_util_printf("PoeSetSystemPowerBudget Failed!\n");
        }
    }
    else
    {
        diag_util_printf("RTK_UART_POE_cmd: PoeSetSystemPowerBudget Failed!\n");
    }

    return res;
}
