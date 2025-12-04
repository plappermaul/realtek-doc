
#include <generated/ca_ne_autoconf.h>
#include "osal_common.h"
#include "aal_common.h"
#include "aal_fdb.h"
#include "util_table.h"
#include <soc/cortina/registers.h>
#if 0//yocto
#include <soc/cortina/cortina-soc.h>
#else//sd1 uboot for 98f
#endif//sd1 uboot for 98f
#include "aal_l2_cls.h"
#include "port.h"
#include "l2.h"
#include "aal_port.h"
#include "aal_l2_specpkt.h"
#include "aal_l2_te.h"
#include "aal_arb.h"
#include "aal_l2_vlan.h"
#include "ca_event.h"

/*______________________________________________________________________________
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%  data declaration
%%____________________________________________________________________________*/
ca_table_t *fdb_tbl_p;
ca_table_t *fdb_tbl_svl_p;
ca_table_t *mac_filter_tbl_p;
ca_uint g_fdb_lock;
ca_boolean_t mac_filter_dft[CA_AAL_FDB_ETH_PORT_NUM];
ca_int32_t aal_fdb_debug = 0;
extern ca_uint32_t l2_aging_time ;

aal_port_flooding_t flood_cb;

ca_status_t aal_fdb_delete_oldest_entry_by_port(
    CA_IN  ca_device_id_t              device_id,
    CA_IN  ca_uint32_t              lpid);


/*______________________________________________________________________________
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%  macro
%%____________________________________________________________________________*/


#define __FDB_LOCK()    \
    do{ \
        __fdb_acc_lock();\
    }while(0)

#define __FDB_UNLOCK()    \
    do{ \
        __fdb_acc_unlock();\
    }while(0)

/*______________________________________________________________________________
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%  functions
%%____________________________________________________________________________*/

#if 0//yocto
void __fdb_acc_lock(void)
{
    ca_spin_lock(g_fdb_lock);
}

void __fdb_acc_unlock(void)
{
    ca_spin_unlock(g_fdb_lock);
}
#else//sd1 uboot for 98f
#endif//sd1 uboot for 98f

static ca_status_t __aal_fdb_access(
    ca_device_id_t     device_id,
    ca_uint32       op_cmd,
    ca_uint32       *WORD4,
    ca_uint32       *WORD3,
    ca_uint32       *WORD2,
    ca_uint32       *WORD1,
    ca_uint32       *WORD0,
    ca_uint32       *DATA3,
    ca_uint32       *DATA2,
    ca_uint32       *DATA1,
    ca_uint32       *DATA0,
    ca_uint32       *cmd_return
)
{
    L2FE_L2E_FDB_ACCESS_t l2_fdb_access;
    L2FE_L2E_FDB_CMD_RETURN_t cmd_ret;
    ca_uint32                 timeout = 0;


    /* If busy, just return error */
    if (device_id == 0)
        l2_fdb_access.wrd = CA_NE_REG_READ(L2FE_L2E_FDB_ACCESS);
#ifdef CONFIG_ARCH_CORTINA_G3HGU
    else if (device_id == 1){
        l2_fdb_access.wrd = CA_8279_NE_REG_READ(L2FE_L2E_FDB_ACCESS);
    }
#endif

    if (l2_fdb_access.bf.access) {
        L2_DEBUG("HW BUSY, wrd:0x%08x\r\n", l2_fdb_access.wrd);

        return CA_E_BUSY;
    }

    l2_fdb_access.wrd = 0x80000000 | op_cmd;

#if 0//yocto
    __FDB_LOCK();
#else//sd1 uboot for 98f
#endif//sd1 uboot for 98f

    if (device_id == 0){
        FDB_REG_WRITE(L2FE_L2E_FDB_CMD_RETURN, 0); /* init cmd return */
    }
#ifdef CONFIG_ARCH_CORTINA_G3HGU
    else if (device_id == 1){
        CA_8279_NE_REG_WRITE(0, L2FE_L2E_FDB_CMD_RETURN); /* init cmd return */
    }
#endif

    switch (op_cmd) {
        case __FDB_TBL_OPCODE_INIT:
            /* Set nothing */
            break;

        case __FDB_TBL_OPCODE_SCAN:
            if (WORD0 == NULL ||
                DATA0 == NULL ||
                DATA1 == NULL ||
                DATA2 == NULL ||
                DATA3 == NULL) {
                __FDB_UNLOCK();
                return CA_E_PARAM;
            }

            l2_fdb_access.wrd |= (*WORD0 & 0x1FFF) << 8; /* start addr */
            if (device_id == 0){
                FDB_REG_WRITE(L2FE_L2E_FDB_DATA0, 0); /* init data */
                FDB_REG_WRITE(L2FE_L2E_FDB_DATA1, 0);
                FDB_REG_WRITE(L2FE_L2E_FDB_DATA2, 0);
                FDB_REG_WRITE(L2FE_L2E_FDB_DATA3, 0);
            }
#ifdef CONFIG_ARCH_CORTINA_G3HGU
            else if (device_id == 1){
                CA_8279_NE_REG_WRITE(0, L2FE_L2E_FDB_DATA0); /* init data */
                CA_8279_NE_REG_WRITE(0, L2FE_L2E_FDB_DATA1);
                CA_8279_NE_REG_WRITE(0, L2FE_L2E_FDB_DATA2);
                CA_8279_NE_REG_WRITE(0, L2FE_L2E_FDB_DATA3);
            }
#endif

            break;

            /* go through */
        case __FDB_TBL_OPCODE_READ:
        case __FDB_TBL_OPCODE_APPEND:
        case __FDB_TBL_OPCODE_APPEND_REPLACE:
        case __FDB_TBL_OPCODE_DELETE:
            if (DATA0 == NULL ||
                DATA1 == NULL ||
                DATA2 == NULL ||
                DATA3 == NULL) {
                __FDB_UNLOCK();
                return CA_E_PARAM;
            }
#if 1
            L2_DEBUG("input: data0:0x%08x\r\n", *DATA0);
            L2_DEBUG("       data1:0x%08x\r\n", *DATA1);
            L2_DEBUG("       data2:0x%08x\r\n", *DATA2);
            L2_DEBUG("       data3:0x%08x\r\n", *DATA3);
#endif
            if (device_id == 0){
                FDB_REG_WRITE(L2FE_L2E_FDB_DATA0, *DATA0);
                FDB_REG_WRITE(L2FE_L2E_FDB_DATA1, *DATA1);
                FDB_REG_WRITE(L2FE_L2E_FDB_DATA2, *DATA2);
                FDB_REG_WRITE(L2FE_L2E_FDB_DATA3, *DATA3);
            }
#ifdef CONFIG_ARCH_CORTINA_G3HGU
            else if (device_id == 1){
                CA_8279_NE_REG_WRITE(*DATA0, L2FE_L2E_FDB_DATA0);
                CA_8279_NE_REG_WRITE(*DATA1, L2FE_L2E_FDB_DATA1);
                CA_8279_NE_REG_WRITE(*DATA2, L2FE_L2E_FDB_DATA2);
                CA_8279_NE_REG_WRITE(*DATA3, L2FE_L2E_FDB_DATA3);
            }
#endif
            break;

        case __FDB_TBL_OPCODE_FLUSH:
        case __FDB_TBL_OPCODE_FLUSH_OLDEST:
            /*
            if (WORD0 == NULL ||
                WORD1 == NULL ||
                WORD2 == NULL ||
                WORD3 == NULL ||
                WORD4 == NULL) {
                __FDB_UNLOCK();
                return CA_E_PARAM;
            }

            FDB_REG_WRITE(L2FE_L2E_FDB_CPU_CMD_WORD0, *WORD0);
            FDB_REG_WRITE(L2FE_L2E_FDB_CPU_CMD_WORD1, *WORD1);
            FDB_REG_WRITE(L2FE_L2E_FDB_CPU_CMD_WORD2, *WORD2);
            FDB_REG_WRITE(L2FE_L2E_FDB_CPU_CMD_WORD3, *WORD3);
            FDB_REG_WRITE(L2FE_L2E_FDB_CPU_CMD_WORD4, *WORD4);
            */
            break;

        default:
            L2_DEBUG("not support\r\n");
            __FDB_UNLOCK();
            return CA_E_NOT_SUPPORT;
    }

    /* Start operation command */
#if 0
/*
    if (op_cmd == __FDB_TBL_OPCODE_SCAN) {
        l2_fdb_access.wrd |= (*WORD0 << 3);
    }
*/

    /* write bit */
    if (op_cmd == __FDB_TBL_OPCODE_APPEND || op_cmd == __FDB_TBL_OPCODE_DELETE) {
        l2_fdb_access.wrd |= 0x40000000;
    }
#endif



    L2_DEBUG("access wrd:0x%08x\r\n", l2_fdb_access.wrd);
    if (device_id == 0){
        FDB_REG_WRITE(L2FE_L2E_FDB_ACCESS, l2_fdb_access.wrd);
    }
#ifdef CONFIG_ARCH_CORTINA_G3HGU
    else if (device_id == 1){
        CA_8279_NE_REG_WRITE(l2_fdb_access.wrd, L2FE_L2E_FDB_ACCESS);
    }
#endif

    do {
        if (device_id == 0){
            l2_fdb_access.wrd = CA_NE_REG_READ(L2FE_L2E_FDB_ACCESS);
        }
#ifdef CONFIG_ARCH_CORTINA_G3HGU
        else if (device_id == 1){
            l2_fdb_access.wrd = CA_8279_NE_REG_READ(L2FE_L2E_FDB_ACCESS);
        }
#endif

        if (timeout++ >= CA_AAL_FDB_TBL_OP_TIMEOUT) {
#if 0//yocto
            __FDB_UNLOCK();
#else//sd1 uboot for 98f
#endif//sd1 uboot for 98f
            L2_DEBUG("timeout\r\n");
            return CA_E_TIMEOUT;
        }

    } while (l2_fdb_access.bf.access);

    /* get return code and feedback */
    if (device_id == 0){
        cmd_ret.wrd = CA_NE_REG_READ(L2FE_L2E_FDB_CMD_RETURN);
    }
#ifdef CONFIG_ARCH_CORTINA_G3HGU
    else if (device_id == 1){
        cmd_ret.wrd = CA_8279_NE_REG_READ(L2FE_L2E_FDB_CMD_RETURN);
    }
#endif
    switch (op_cmd) {
        case __FDB_TBL_OPCODE_INIT:
        case __FDB_TBL_OPCODE_APPEND:
        case __FDB_TBL_OPCODE_APPEND_REPLACE:
        case __FDB_TBL_OPCODE_DELETE:
        case __FDB_TBL_OPCODE_FLUSH:
        case __FDB_TBL_OPCODE_FLUSH_OLDEST:
            /* nothing feedback */
            break;

        case __FDB_TBL_OPCODE_READ:
        case __FDB_TBL_OPCODE_SCAN:

            if (cmd_ret.bf.status == CA_AAL_FDB_ACCESS_HIT) {
                if (device_id == 0){
                    *DATA0 = CA_NE_REG_READ(L2FE_L2E_FDB_DATA0);
                    *DATA1 = CA_NE_REG_READ(L2FE_L2E_FDB_DATA1);
                    *DATA2 = CA_NE_REG_READ(L2FE_L2E_FDB_DATA2);
                    *DATA3 = CA_NE_REG_READ(L2FE_L2E_FDB_DATA3);
                }
#ifdef CONFIG_ARCH_CORTINA_G3HGU
                else if (device_id == 1){
                    *DATA0 = CA_8279_NE_REG_READ(L2FE_L2E_FDB_DATA0);
                    *DATA1 = CA_8279_NE_REG_READ(L2FE_L2E_FDB_DATA1);
                    *DATA2 = CA_8279_NE_REG_READ(L2FE_L2E_FDB_DATA2);
                    *DATA3 = CA_8279_NE_REG_READ(L2FE_L2E_FDB_DATA3);
                }
#endif                //*WORD0 = cmd_ret.wrd;
            #if 0
                L2_DEBUG("output: data0:0x%08x\r\n", *DATA0);
                L2_DEBUG("        data1:0x%08x\r\n", *DATA1);
                L2_DEBUG("        data2:0x%08x\r\n", *DATA2);
                L2_DEBUG("        data3:0x%08x\r\n", *DATA3);
            #endif
            }

            break;

        default:
            break;
    }

#if 0//yocto
    __FDB_UNLOCK();
#else//sd1 uboot for 98f
#endif//sd1 uboot for 98f

    L2_DEBUG("cmd_ret.wrd:0x%08x\r\n", cmd_ret.wrd);

    if (cmd_return)
        *cmd_return = cmd_ret.wrd;

    return CA_E_OK;

}

#if 0
static ca_status_t __aal_fdb_access_to_speed(
    ca_device_id_t     device_id,
    ca_uint32       op_cmd,
    ca_uint32       *DATA7,
    ca_uint32       *DATA6,
    ca_uint32       *DATA5,
    ca_uint32       *DATA4,
    ca_uint32       *DATA3,
    ca_uint32       *DATA2,
    ca_uint32       *DATA1,
    ca_uint32       *DATA0,
    ca_uint32       *cmd_return
)
{
    FE_PR_FE_L2_CPU_OPERATION_t reg_l2_cpu_op;

    ca_uint32                   timeout = 0;

    if ((op_cmd > __FDB_TBL_OPCODE_CNT_DEC) ||
        (op_cmd == __FDB_TBL_OPCODE_RESV0) ||
        (NULL == cmd_return)) {
        return CA_E_PARAM;
    }

    /* If busy, just return error */
    CA_NE_REG_READ(FE_PR_FE_L2_CPU_OPERATION, reg_l2_cpu_op.wrd);

    if (reg_l2_cpu_op.wrd & 0x80000000) {
        return CA_E_BUSY;
    }

    switch (op_cmd) {
        case __FDB_TBL_OPCODE_INIT:
            /* Set nothing */
            break;

        case __FDB_TBL_OPCODE_APPEND:

            /* parameters check */
            if (DATA0 == NULL ||
                DATA1 == NULL ||
                DATA2 == NULL ||
                DATA3 == NULL ||
                DATA4 == NULL ||
                DATA5 == NULL ||
                DATA6 == NULL) {
                return CA_E_PARAM;
            }

            /* set the related elements to create new fdb entry */
            FDB_REG_WRITE(FE_PR_FE_L2_CPU_DATA0, *DATA0);
            FDB_REG_WRITE(FE_PR_FE_L2_CPU_DATA1, *DATA1);
            FDB_REG_WRITE(FE_PR_FE_L2_CPU_DATA2, *DATA2);
            FDB_REG_WRITE(FE_PR_FE_L2_CPU_DATA3, *DATA3);

            /* set the related elements to create hash key */
            FDB_REG_WRITE(FE_PR_FE_L2_CPU_DATA4, *DATA4);
            FDB_REG_WRITE(FE_PR_FE_L2_CPU_DATA5, *DATA5);
            FDB_REG_WRITE(FE_PR_FE_L2_CPU_DATA6, *DATA6);

            break;

        case __FDB_TBL_OPCODE_READ:

            /* parameters check */
            if (DATA4 == NULL ||
                DATA5 == NULL ||
                DATA6 == NULL) {
                return CA_E_PARAM;
            }

            /* rzhou add just to clear, can be deleted when debug finished */
            FDB_REG_WRITE(FE_PR_FE_L2_CPU_DATA0, 0);
            FDB_REG_WRITE(FE_PR_FE_L2_CPU_DATA1, 0);
            FDB_REG_WRITE(FE_PR_FE_L2_CPU_DATA2, 0);
            FDB_REG_WRITE(FE_PR_FE_L2_CPU_DATA3, 0);

            /* set the related elements to create hash key */
            FDB_REG_WRITE(FE_PR_FE_L2_CPU_DATA4, *DATA4);
            FDB_REG_WRITE(FE_PR_FE_L2_CPU_DATA5, *DATA5);
            FDB_REG_WRITE(FE_PR_FE_L2_CPU_DATA6, *DATA6);

            break;

        case __FDB_TBL_OPCODE_DELETE:

            /* parameters check */
            if (DATA4 == NULL ||
                DATA5 == NULL ||
                DATA6 == NULL) {

                return CA_E_PARAM;
            }

            /* set the related elements to create hash key */
            FDB_REG_WRITE(FE_PR_FE_L2_CPU_DATA4, *DATA4);
            FDB_REG_WRITE(FE_PR_FE_L2_CPU_DATA5, *DATA5);
            FDB_REG_WRITE(FE_PR_FE_L2_CPU_DATA6, *DATA6);

            break;

        case __FDB_TBL_OPCODE_SCAN:
            if (DATA4 == NULL) {
                return CA_E_PARAM;
            }

            /* rzhou add just to clear, can be deleted when debug finished */
            FDB_REG_WRITE(FE_PR_FE_L2_CPU_DATA0, 0);
            FDB_REG_WRITE(FE_PR_FE_L2_CPU_DATA1, 0);
            FDB_REG_WRITE(FE_PR_FE_L2_CPU_DATA2, 0);
            FDB_REG_WRITE(FE_PR_FE_L2_CPU_DATA3, 0);

            /* set offset address */
            FDB_REG_WRITE(FE_PR_FE_L2_CPU_DATA4, *DATA4);
            FDB_REG_WRITE(FE_PR_FE_L2_CPU_DATA5, *DATA5);

            break;

        case __FDB_TBL_OPCODE_FLUSH:

            /* parameters check */
            if (DATA4 == NULL ||
                DATA5 == NULL ||
                DATA6 == NULL ||
                DATA7 == NULL) {
                return CA_E_PARAM;
            }

            /* set programmed rule */
            FDB_REG_WRITE(FE_PR_FE_L2_CPU_DATA4, *DATA4);
            FDB_REG_WRITE(FE_PR_FE_L2_CPU_DATA5, *DATA5);
            FDB_REG_WRITE(FE_PR_FE_L2_CPU_DATA6, *DATA6);
            FDB_REG_WRITE(FE_PR_FE_L2_CPU_DATA7, *DATA7);

            break;

        case __FDB_TBL_OPCODE_CNT_INC:

            /* parameters check */
            if (DATA0 == NULL ||
                DATA4 == NULL) {
                return CA_E_PARAM;
            }

            /* set inc cnt */
            FDB_REG_WRITE(FE_PR_FE_L2_CPU_DATA0, *DATA0);

            /* set target port/sub id */
            FDB_REG_WRITE(FE_PR_FE_L2_CPU_DATA4, *DATA4);

            break;

        case __FDB_TBL_OPCODE_CNT_INIT:

            /* parameters check */
            if ( DATA4 == NULL) {
                return CA_E_PARAM;
            }

            /* set target port/sub id */
            FDB_REG_WRITE(FE_PR_FE_L2_CPU_DATA4, *DATA4);

            break;

        case __FDB_TBL_OPCODE_CNT_WR:

            /* parameters check */
            if (DATA0 == NULL ||
                DATA4 == NULL) {
                return CA_E_PARAM;
            }

            /* set counter to be writen */
            FDB_REG_WRITE(FE_PR_FE_L2_CPU_DATA0, *DATA0);

            /* set target port/sub id */
            FDB_REG_WRITE(FE_PR_FE_L2_CPU_DATA4, *DATA4);

            break;

        case __FDB_TBL_OPCODE_CNT_RD:

            /* parameters check */
            if (DATA4 == NULL) {
                return CA_E_PARAM;
            }

            /* set target port/sub id */
            FDB_REG_WRITE(FE_PR_FE_L2_CPU_DATA4, *DATA4);

            break;

        case __FDB_TBL_OPCODE_LMT_WR:

            /* parameters check */
            if (DATA0 == NULL || DATA4 == NULL) {
                return CA_E_PARAM;
            }

            /* set limit */
            FDB_REG_WRITE(FE_PR_FE_L2_CPU_DATA0, *DATA0);

            /* set target port/sub id */
            FDB_REG_WRITE(FE_PR_FE_L2_CPU_DATA4, *DATA4);

            break;

        case __FDB_TBL_OPCODE_LMT_RD:

            /* parameters check */
            if (DATA4 == NULL) {
                return CA_E_PARAM;
            }

            /* set target port/sub id */
            FDB_REG_WRITE(FE_PR_FE_L2_CPU_DATA4, *DATA4);

            break;

        case __FDB_TBL_OPCODE_LMT_INIT:

            /* parameters check */
            if (DATA4 == NULL) {
                return CA_E_PARAM;
            }

            /* set target port/sub id */
            FDB_REG_WRITE(FE_PR_FE_L2_CPU_DATA4, *DATA4);

            break;

        case __FDB_TBL_OPCODE_CNT_DEC:

            /* parameters check */
            if (DATA0 == NULL ||
                DATA4 == NULL) {
                return CA_E_PARAM;
            }

            /* set dec cnt */
            FDB_REG_WRITE(FE_PR_FE_L2_CPU_DATA0, *DATA0);

            /* set target port/sub id */
            FDB_REG_WRITE(FE_PR_FE_L2_CPU_DATA4, *DATA4);

            break;

        default:
            break;
    }

    /* Start operation command */
    reg_l2_cpu_op.wrd = 0x80000000 | op_cmd;

    FDB_REG_WRITE(FE_PR_FE_L2_CPU_OPERATION, reg_l2_cpu_op.bf.op_cmd);

    do {
        CA_NE_REG_READ(FE_PR_FE_L2_CPU_OPERATION, reg_l2_cpu_op.bf.op_cmd);

        if (timeout++ >= __FDB_TBL_OPER_TIMEOUT) {
            return CA_E_TIMEOUT;
        }

    }
    while (reg_l2_cpu_op.wrd & 0x80000000);

    /* Get return code and feedback */
    CA_NE_REG_READ(FE_PR_FE_L2_CPU_CMD_RETURN, *cmd_return);

    switch (op_cmd) {
        case __FDB_TBL_OPCODE_INIT:

            /* nothing feedback */
        case __FDB_TBL_OPCODE_APPEND:
            /* nothing feedback */
            break;

        case __FDB_TBL_OPCODE_READ:

            /* parameters check */
            if (DATA0 == NULL ||
                DATA1 == NULL ||
                DATA2 == NULL ||
                DATA3 == NULL) {
                return CA_E_PARAM;
            }

            /* set the related elements to create new fdb entry */
            CA_NE_REG_READ(FE_PR_FE_L2_CPU_DATA0, *DATA0);
            CA_NE_REG_READ(FE_PR_FE_L2_CPU_DATA1, *DATA1);
            CA_NE_REG_READ(FE_PR_FE_L2_CPU_DATA2, *DATA2);
            CA_NE_REG_READ(FE_PR_FE_L2_CPU_DATA3, *DATA3);

            break;

        case __FDB_TBL_OPCODE_DELETE:
            /* nothing feedback */
            break;

        case __FDB_TBL_OPCODE_SCAN:

            /* parameters check */
            if (DATA0 == NULL ||
                DATA1 == NULL ||
                DATA2 == NULL ||
                DATA3 == NULL) {
                return CA_E_PARAM;
            }

            CA_NE_REG_READ(FE_PR_FE_L2_CPU_DATA0, *DATA0);
            CA_NE_REG_READ(FE_PR_FE_L2_CPU_DATA1, *DATA1);
            CA_NE_REG_READ(FE_PR_FE_L2_CPU_DATA2, *DATA2);
            CA_NE_REG_READ(FE_PR_FE_L2_CPU_DATA3, *DATA3);

            break;

        case __FDB_TBL_OPCODE_FLUSH:
        case __FDB_TBL_OPCODE_RESV0:
        case __FDB_TBL_OPCODE_CNT_INC:
        case __FDB_TBL_OPCODE_CNT_INIT:
        case __FDB_TBL_OPCODE_CNT_WR:
            /* nothing feedback */
            break;

        case __FDB_TBL_OPCODE_CNT_RD:

            /* parameters check */
            if (DATA0 == NULL) {
                return CA_E_PARAM;
            }

            /* Get the read counter */
            CA_NE_REG_READ(FE_PR_FE_L2_CPU_DATA0, *DATA0);

            break;

        case __FDB_TBL_OPCODE_LMT_WR:
            /* nothing feedback */
            break;

        case __FDB_TBL_OPCODE_LMT_RD:

            /* parameters check */
            if (DATA0 == NULL) {
                return CA_E_PARAM;
            }

            /* Get the read limit */
            CA_NE_REG_READ(FE_PR_FE_L2_CPU_DATA0, *DATA0);

            break;

        case __FDB_TBL_OPCODE_LMT_INIT:
        case __FDB_TBL_OPCODE_CNT_DEC:
            /* nothing feedback */
            break;

        default:
            break;
    }

    return CA_E_OK;
}
#endif

#if 0//yocto
void
octet_dump( char *tp, unsigned char *dp, int len )
{
  unsigned char *cp = dp;
  int  ii;
  int  jj;

  printk("%s\n", tp);
  for ( ii=len; ii>16; ii-=16 )
  {
    for ( jj=0; jj<16; jj++, cp++ )
      printk("%02x ", *cp);
    printk("\n");
  }
  for ( jj=0; jj<ii; jj++, cp++ )
  printk("%02x ", *cp);
  printk("\n");
}

ca_status_t __aal_fdb_sc_ind_get(
    ca_device_id_t device_id,
    ca_uint32_t lpid,
    ca_boolean_t *sc_ind)
{
    aal_ilpb_cfg_t ilpb_cfg;
    ca_status_t  ret = CA_E_OK;

    CA_ASSERT_RET(sc_ind != NULL, CA_E_PARAM);

    *sc_ind = 0;
    memset(&ilpb_cfg, 0, sizeof(aal_ilpb_cfg_t));

    ret = aal_port_ilpb_cfg_get(device_id, lpid, &ilpb_cfg);
    if (ret != CA_E_OK) {
        return 0;
    }

    if(ilpb_cfg.top_s_tpid_enable == 1 && ilpb_cfg.top_c_tpid_enable == 0){
        *sc_ind = 1;

    }else if(ilpb_cfg.top_s_tpid_enable == 0 && ilpb_cfg.top_c_tpid_enable == 1){
        *sc_ind = 0;

    }else{
        *sc_ind = 1;
    }

    return ret;
}
#else//sd1 uboot for 98f
#endif//sd1 uboot for 98f

ca_status_t __aal_l2e_cpu_op_mode(ca_device_id_t device_id, ca_uint8 mode)
{
    L2FE_L2E_CTRL_t           l2e_ctrl;

    if (device_id == 0){
        l2e_ctrl.wrd = CA_NE_REG_READ(L2FE_L2E_CTRL);
    }
#ifdef CONFIG_ARCH_CORTINA_G3HGU
    else if (device_id == 1){
        l2e_ctrl.wrd = CA_8279_NE_REG_READ(L2FE_L2E_CTRL);
    }
#endif

    if (mode == CA_AAL_L2E_INDIRECT_MODE) {
        l2e_ctrl.bf.cpu_opt_mode = CA_AAL_L2E_INDIRECT_MODE;
        if (device_id == 0){
            FDB_REG_WRITE(L2FE_L2E_CTRL, l2e_ctrl.wrd);
        }
#ifdef CONFIG_ARCH_CORTINA_G3HGU
        else if (device_id == 1){
            CA_8279_NE_REG_WRITE(l2e_ctrl.wrd, L2FE_L2E_CTRL);
        }
#endif
    }
    else {
        l2e_ctrl.bf.cpu_opt_mode = CA_AAL_L2E_CMD_MODE;
        if (device_id == 0){
            FDB_REG_WRITE(L2FE_L2E_CTRL, l2e_ctrl.wrd);
        }
#ifdef CONFIG_ARCH_CORTINA_G3HGU
        else if (device_id == 1){
            CA_8279_NE_REG_WRITE(l2e_ctrl.wrd, L2FE_L2E_CTRL);
        }
#endif
    }

    return CA_E_OK;
}

#if 0//yocto
ca_status_t __aal_port_mac_cnt(
    ca_device_id_t     device_id,
    ca_int32        port_id,
    ca_uint32       op_cmd,
    ca_uint32       *DATA0,
    ca_uint32       *cmd_return)
{
    L2FE_L2E_PORT_MAC_CNT_LMT_ACCESS_t mac_cnt_access;
//    L2FE_L2E_FDB_CMD_RETURN_t cmd_ret;
    ca_uint32                   timeout = 0;

    if (cmd_return)
        *cmd_return = CA_AAL_L3E_CNT_LMT_FAIL;

    /* If busy, just return error */
    if (device_id == 0){
        mac_cnt_access.wrd = CA_NE_REG_READ(L2FE_L2E_PORT_MAC_CNT_LMT_ACCESS);
    }
#ifdef CONFIG_ARCH_CORTINA_G3HGU
    else if (device_id == 1){
        mac_cnt_access.wrd = CA_8279_NE_REG_READ(L2FE_L2E_PORT_MAC_CNT_LMT_ACCESS);
    }
#endif

    if (mac_cnt_access.bf.access) {
        return CA_E_BUSY;
    }

    L2_DEBUG("port_id:%d, op_cmd:0x%x\r\n", port_id, op_cmd);

    mac_cnt_access.wrd = 0;
    mac_cnt_access.bf.access = 1;
    mac_cnt_access.bf.addr = port_id;

    switch (op_cmd) {
        case __FDB_TBL_OPCODE_CNT_INC:

            /* parameters check */
            /*if (DATA0 == NULL) {
                return CA_E_PARAM;
            }*/

            mac_cnt_access.wrd |= CA_AAL_L2E_CNT_INC;
            /* inc cnt */
            //FDB_REG_WRITE(L2FE_L2E_PORT_MAC_CNT_ACCESS, mac_cnt_access.wrd);

            break;

        case __FDB_TBL_OPCODE_CNT_DEC:

            /* parameters check */
            /*if (DATA0 == NULL) {
                return CA_E_PARAM;
            }*/

            mac_cnt_access.wrd |= CA_AAL_L2E_CNT_DEC;

            /* dec cnt */
            //FDB_REG_WRITE(L2FE_L2E_PORT_MAC_CNT_ACCESS, mac_cnt_access.wrd);

            break;

        case __FDB_TBL_OPCODE_CNT_INIT:

            /* parameters check */
            /*if (DATA0 == NULL) {
                return CA_E_PARAM;
            }*/

            /* set csr_l2e_cpu_intf_mode to indirect mode */
            __aal_l2e_cpu_op_mode(device_id, CA_AAL_L2E_INDIRECT_MODE);

            /* set counter to be writen */
            if (device_id == 0){
                FDB_REG_WRITE(L2FE_L2E_PORT_MAC_CNT_LMT_DATA, 0);
            }
#ifdef CONFIG_ARCH_CORTINA_G3HGU
            else if (device_id == 1){
                CA_8279_NE_REG_WRITE(0, L2FE_L2E_PORT_MAC_CNT_LMT_DATA);
            }
#endif
            mac_cnt_access.bf.rbw = 1;

            break;

        case __FDB_TBL_OPCODE_CNT_WR:

            /* parameters check */
            if (DATA0 == NULL) {
                L2_DEBUG("empty data\r\n");
                return CA_E_PARAM;
            }

            /* set csr_l2e_cpu_intf_mode to indirect mode */
            __aal_l2e_cpu_op_mode(device_id, CA_AAL_L2E_INDIRECT_MODE);

            /* set counter to be writen */
            if (device_id == 0){
                FDB_REG_WRITE(L2FE_L2E_PORT_MAC_CNT_LMT_DATA, *DATA0);
            }
#ifdef CONFIG_ARCH_CORTINA_G3HGU
            else if (device_id == 1){
                CA_8279_NE_REG_WRITE(*DATA0, L2FE_L2E_PORT_MAC_CNT_LMT_DATA);
            }
#endif

            mac_cnt_access.bf.rbw = 1;
            break;

        case __FDB_TBL_OPCODE_CNT_RD:

            /* parameters check */
            if (DATA0 == NULL) {
                L2_DEBUG("empty data\r\n");
                return CA_E_PARAM;
            }

            /* set csr_l2e_cpu_intf_mode to indirect mode */
            __aal_l2e_cpu_op_mode(device_id, CA_AAL_L2E_INDIRECT_MODE);

            mac_cnt_access.bf.rbw = 0;

            break;

        default:
            break;
    }

    L2_DEBUG("cnt access wrd:0x%08x\r\n", mac_cnt_access.wrd);

    /* Start operation command */
    if (device_id == 0){
        FDB_REG_WRITE(L2FE_L2E_PORT_MAC_CNT_LMT_ACCESS, mac_cnt_access.wrd);
    }
#ifdef CONFIG_ARCH_CORTINA_G3HGU
    else if (device_id == 1){
        CA_8279_NE_REG_WRITE(mac_cnt_access.wrd, L2FE_L2E_PORT_MAC_CNT_LMT_ACCESS);
    }
#endif

    do {
        if (device_id == 0){
            mac_cnt_access.wrd = CA_NE_REG_READ(L2FE_L2E_PORT_MAC_CNT_LMT_ACCESS);
        }
#ifdef CONFIG_ARCH_CORTINA_G3HGU
        else if (device_id == 1){
            mac_cnt_access.wrd = CA_8279_NE_REG_READ(L2FE_L2E_PORT_MAC_CNT_LMT_ACCESS);
        }
#endif

        if (timeout++ >= CA_AAL_FDB_TBL_OP_TIMEOUT) {
            __aal_l2e_cpu_op_mode(device_id, CA_AAL_L2E_CMD_MODE);
            return CA_E_TIMEOUT;
        }

    }
    while (mac_cnt_access.bf.access);

    /* Get return code and feedback */
    //cmd_ret.wrd = CA_NE_REG_READ(L2FE_L2E_FDB_CMD_RETURN);

    switch (op_cmd) {

        case __FDB_TBL_OPCODE_CNT_INC:
        case __FDB_TBL_OPCODE_CNT_INIT:
        case __FDB_TBL_OPCODE_CNT_WR:
        case __FDB_TBL_OPCODE_CNT_DEC:
            /* nothing feedback */
            break;

        case __FDB_TBL_OPCODE_CNT_RD:

            /* parameters check */
            if (DATA0 == NULL) {
                L2_DEBUG("empty data\r\n");
                return CA_E_PARAM;
            }

            /* Get the read counter */
            if (device_id == 0){
                *DATA0 = CA_NE_REG_READ(L2FE_L2E_PORT_MAC_CNT_LMT_DATA);
            }
#ifdef CONFIG_ARCH_CORTINA_G3HGU
            else if (device_id == 1){
                *DATA0 = CA_8279_NE_REG_READ(L2FE_L2E_PORT_MAC_CNT_LMT_DATA);
            }
#endif
            break;

        default:
            break;
    }

    /* restore csr_l2e_cpu_intf_mode to cmd mode */
    __aal_l2e_cpu_op_mode(device_id, CA_AAL_L2E_CMD_MODE);


    if (cmd_return)
        *cmd_return = CA_AAL_L3E_CNT_LMT_SUCCESS;

    L2_DEBUG("cmd_return:%d\r\n", *cmd_return);

    return CA_E_OK;
}
#else//sd1 uboot for 98f
#endif//sd1 uboot for 98f

ca_status_t __aal_port_mac_lmt(
    ca_device_id_t     device_id,
    ca_int32        port_id,
    ca_uint32       op_cmd,
    ca_uint32       *DATA0,
    ca_uint32       *cmd_return)
{
    L2FE_L2E_PORT_MAC_CNT_LMT_ACCESS_t mac_lmt_access;
//    L2FE_L2E_FDB_CMD_RETURN_t cmd_ret;
    ca_uint32                   timeout = 0;

    if (cmd_return)
        *cmd_return = CA_AAL_L3E_CNT_LMT_FAIL;

    /* If busy, just return error */
    if (device_id == 0){
        mac_lmt_access.wrd = CA_NE_REG_READ(L2FE_L2E_PORT_MAC_CNT_LMT_ACCESS);
    }
#ifdef CONFIG_ARCH_CORTINA_G3HGU
    else if (device_id == 1){
        mac_lmt_access.wrd = CA_8279_NE_REG_READ(L2FE_L2E_PORT_MAC_CNT_LMT_ACCESS);
    }
#endif

    if (mac_lmt_access.bf.access) {
        return CA_E_BUSY;
    }

    L2_DEBUG("port_id:%d, op_cmd:0x%x\r\n", port_id, op_cmd);

    mac_lmt_access.wrd = 0;
    mac_lmt_access.bf.access = 1;
    mac_lmt_access.bf.addr = port_id;

    /* limit access */
    mac_lmt_access.wrd |= CA_AAL_L2E_LMT_ACCESS;

    switch (op_cmd) {
        case __FDB_TBL_OPCODE_LMT_INC:

            mac_lmt_access.wrd |= CA_AAL_L2E_CNT_INC;
            /* inc cnt */
            //FDB_REG_WRITE(L2FE_L2E_PORT_MAC_CNT_ACCESS, mac_cnt_access.wrd);

            break;

        case __FDB_TBL_OPCODE_LMT_DEC:

            mac_lmt_access.wrd |= CA_AAL_L2E_CNT_DEC;

            /* dec cnt */
            //FDB_REG_WRITE(L2FE_L2E_PORT_MAC_CNT_ACCESS, mac_cnt_access.wrd);

            break;

        case __FDB_TBL_OPCODE_LMT_INIT:
            /* set csr_l2e_cpu_intf_mode to indirect mode */
            __aal_l2e_cpu_op_mode(device_id, CA_AAL_L2E_INDIRECT_MODE);

            /* set counter to be writen */
            if (device_id == 0){
                FDB_REG_WRITE(L2FE_L2E_PORT_MAC_CNT_LMT_DATA, 0);
            }
#ifdef CONFIG_ARCH_CORTINA_G3HGU
            else if (device_id == 1){
                CA_8279_NE_REG_WRITE(0, L2FE_L2E_PORT_MAC_CNT_LMT_DATA);
            }
#endif
            mac_lmt_access.bf.rbw = 1;
            break;

        case __FDB_TBL_OPCODE_LMT_WR:

            /* parameters check */
            if (DATA0 == NULL) {
                L2_DEBUG("null data\r\n");
                return CA_E_PARAM;
            }

            /* set csr_l2e_cpu_intf_mode to indirect mode */
            __aal_l2e_cpu_op_mode(device_id, CA_AAL_L2E_INDIRECT_MODE);

            /* set counter to be writen */
            if (device_id == 0){
                FDB_REG_WRITE(L2FE_L2E_PORT_MAC_CNT_LMT_DATA, *DATA0);
            }
#ifdef CONFIG_ARCH_CORTINA_G3HGU
            else if (device_id == 1){
                CA_8279_NE_REG_WRITE(*DATA0, L2FE_L2E_PORT_MAC_CNT_LMT_DATA);
            }
#endif
            mac_lmt_access.bf.rbw = 1;


            break;

        case __FDB_TBL_OPCODE_LMT_RD:

            /* parameters check */
            if (DATA0 == NULL) {
                L2_DEBUG("null data\r\n");
                return CA_E_PARAM;
            }

            /* set csr_l2e_cpu_intf_mode to indirect mode */
            __aal_l2e_cpu_op_mode(device_id, CA_AAL_L2E_INDIRECT_MODE);

            mac_lmt_access.bf.rbw = 0;

            break;

        default:
            break;
    }

    L2_DEBUG("cnt access wrd:0x%08x\r\n", mac_lmt_access.wrd);

    /* Start operation command */
    if (device_id == 0){
        FDB_REG_WRITE(L2FE_L2E_PORT_MAC_CNT_LMT_ACCESS, mac_lmt_access.wrd);
    }
#ifdef CONFIG_ARCH_CORTINA_G3HGU
    else if (device_id == 1){
        CA_8279_NE_REG_WRITE(mac_lmt_access.wrd, L2FE_L2E_PORT_MAC_CNT_LMT_ACCESS);
    }
#endif
    do {
        if (device_id == 0){
            mac_lmt_access.wrd = CA_NE_REG_READ(L2FE_L2E_PORT_MAC_CNT_LMT_ACCESS);
        }
#ifdef CONFIG_ARCH_CORTINA_G3HGU
        else if (device_id == 1){
            mac_lmt_access.wrd = CA_8279_NE_REG_READ(L2FE_L2E_PORT_MAC_CNT_LMT_ACCESS);
        }
#endif
        if (timeout++ >= CA_AAL_FDB_TBL_OP_TIMEOUT) {
            __aal_l2e_cpu_op_mode(device_id, CA_AAL_L2E_CMD_MODE);
            return CA_E_TIMEOUT;
        }

    }
    while (mac_lmt_access.bf.access);

    /* Get return code and feedback */
    //cmd_ret.wrd = CA_NE_REG_READ(L2FE_L2E_FDB_CMD_RETURN);

    switch (op_cmd) {
        case __FDB_TBL_OPCODE_LMT_INC:
        case __FDB_TBL_OPCODE_LMT_INIT:
        case __FDB_TBL_OPCODE_LMT_DEC:
            /* nothing feedback */
            break;

        case __FDB_TBL_OPCODE_LMT_WR:
            break;

        case __FDB_TBL_OPCODE_LMT_RD:

            /* parameters check */
            if (DATA0 == NULL) {
                return CA_E_PARAM;
            }

            /* Get the read counter */
            if (device_id == 0){
                *DATA0 = CA_NE_REG_READ(L2FE_L2E_PORT_MAC_CNT_LMT_DATA);
            }
#ifdef CONFIG_ARCH_CORTINA_G3HGU
            else if (device_id == 1){
                *DATA0 = CA_8279_NE_REG_READ(L2FE_L2E_PORT_MAC_CNT_LMT_DATA);
            }
#endif
            break;

        default:
            break;
    }

    /* restore csr_l2e_cpu_intf_mode to cmd mode */
    __aal_l2e_cpu_op_mode(device_id, CA_AAL_L2E_CMD_MODE);

    if (cmd_return)
        *cmd_return = CA_AAL_L3E_CNT_LMT_SUCCESS;

    return CA_E_OK;
}

#if 0//yocto
void __aal_mac_2_fdb_data(
    L2FE_L2E_FDB_DATA3_t *fdb_data3,
    L2FE_L2E_FDB_DATA2_t *fdb_data2,
    L2FE_L2E_FDB_DATA1_t *fdb_data1,
    ca_mac_addr_t mac
)
{
    if (fdb_data1 == NULL || fdb_data2 == NULL || fdb_data3 == NULL || mac == NULL)
        return;

    fdb_data3->bf.mac_addr = (ca_uint32_t)(mac[0] >> 5) & 0x7;
    fdb_data2->bf.mac_addr = (ca_uint32_t)((mac[0] & 0x1F) << 27) | \
                             (mac[1] << 19) | \
                             (mac[2] << 11) | \
                             (mac[3] << 3)  | \
                             ((mac[4] >> 5) & 0x7);
    fdb_data1->bf.mac_addr = (ca_uint32_t)((mac[4] & 0x1F) << 8) | mac[5];

    return;
}
#else//sd1 uboot for 98f
#endif//sd1 uboot for 98f

void __aal_fdb_data_2_mac(
    L2FE_L2E_FDB_DATA3_t *fdb_data3,
    L2FE_L2E_FDB_DATA2_t *fdb_data2,
    L2FE_L2E_FDB_DATA1_t *fdb_data1,
    ca_mac_addr_t mac
)
{

    if (fdb_data1 == NULL || fdb_data2 == NULL || fdb_data3 == NULL || mac == NULL)
        return;
    /*
    mac[5] = (ca_uint8_t)((fdb_data3->bf.mac_addr << 6) | (fdb_data2->bf.mac_addr >>
                          26));

    mac[4] = (fdb_data2->bf.mac_addr & 0x03fc0000) >> 18;
    mac[3] = (fdb_data2->bf.mac_addr & 0x0003fc00) >> 10;
    mac[2] = (fdb_data2->bf.mac_addr & 0x000003fc) >> 2;
    mac[1] = ((fdb_data2->bf.mac_addr & 0x00000003) << 6) | \
             ((fdb_data1->bf.mac_addr & 0xfc000000) >> 26);
    mac[0] = (fdb_data1->bf.mac_addr & 0x03fc0000) >> 18;
    */

    mac[0] = (fdb_data3->bf.mac_addr << 5) |
        ((fdb_data2->bf.mac_addr >> 27) & 0x1F);
    mac[1] = (fdb_data2->bf.mac_addr >> 19) & 0xFF;
    mac[2] = (fdb_data2->bf.mac_addr >> 11) & 0xFF;
    mac[3] = (fdb_data2->bf.mac_addr >>  3) & 0xFF;
    mac[4] = ((fdb_data2->bf.mac_addr & 0x7) << 5) | \
             ((fdb_data1->bf.mac_addr >> 8) & 0x1F);
    mac[5] = fdb_data1->bf.mac_addr & 0xFF;


    return;
}

void __aal_fdb_data_2_entry(
    L2FE_L2E_FDB_DATA1_t fdb_data1,
    L2FE_L2E_FDB_DATA0_t fdb_data0,
    aal_fdb_entry_cfg_t  *fdb_entry
)
{
    aal_fdb_data0_t *uc_data0_p;

    if (fdb_entry == NULL){
        L2_DEBUG("NULL pointer\r\n");
        return;
    }

    fdb_entry->key_vid      = fdb_data1.bf.key_vid;
    fdb_entry->key_sc_ind   = fdb_data1.bf.key_scind;
    fdb_entry->dot1p        = fdb_data1.bf.key_dot1p;
    fdb_entry->lrnt_vid     = (fdb_data1.bf.lrned_vid << 9) | fdb_data0.bf.lrned_vid;
    fdb_entry->lrnt_sc_ind  = fdb_data0.bf.lrned_scind;
    fdb_entry->sa_permit    = fdb_data0.bf.sa_permit;
    fdb_entry->da_permit    = fdb_data0.bf.da_permit;
    fdb_entry->static_flag  = fdb_data0.bf.is_static;
    //fdb_entry->aging_sts    = FDB_AGING_TIME(fdb_data0.bf.aging_sts);
    fdb_entry->aging_sts    = fdb_data0.bf.aging_sts;
    fdb_entry->mc_flag      = fdb_data0.bf.mc_flag;

    if (fdb_entry->mc_flag) { /* MC Group ID */
        fdb_entry->port_id  = fdb_data0.bf.mcgid;
        fdb_entry->valid    = 1;
        fdb_entry->tmp_flag = 0;
    } else { /* Port ID */
        uc_data0_p = (aal_fdb_data0_t *)&fdb_data0;
        fdb_entry->port_id = uc_data0_p->bf.lpid;
        fdb_entry->valid = uc_data0_p->bf.entry_valid;
        fdb_entry->tmp_flag = uc_data0_p->bf.tmp_flag;
    }

    return;

}

#if 0//yocto
ca_status_t __aal_fdb_entry_add_del(
    ca_device_id_t             device_id,
    ca_uint32               op_cmd,
    aal_fdb_entry_cfg_t  *fdb
)
{
    L2FE_L2E_FDB_DATA0_t fdb_data0;
    L2FE_L2E_FDB_DATA1_t fdb_data1;
    L2FE_L2E_FDB_DATA2_t fdb_data2;
    L2FE_L2E_FDB_DATA3_t fdb_data3;
    aal_fdb_data0_t      *data0_p = (aal_fdb_data0_t *)&fdb_data0;

    L2FE_L2E_FDB_CMD_RETURN_t cmd_return;

    CA_ASSERT_RET(fdb != NULL, CA_E_PARAM);

    L2_DEBUG("op_cmd:0x%x, mac:0x%02x:%02x:%02x:%02x:%02x:%02x, vlan:%d, mc:%d, port:%d, dot1p:%d, sa:%d, da:%d, static:%d, aging_sts:%d\r\n",
              op_cmd,
              fdb->mac[0], fdb->mac[1], fdb->mac[2], fdb->mac[3], fdb->mac[4], fdb->mac[5],
              fdb->key_vid, fdb->mc_flag, fdb->port_id, fdb->dot1p,
              fdb->sa_permit, fdb->da_permit,
              fdb->static_flag, fdb->aging_sts);

    CA_ASSERT_RET(op_cmd == __FDB_TBL_OPCODE_APPEND ||
                  op_cmd == __FDB_TBL_OPCODE_APPEND_REPLACE ||
                  op_cmd == __FDB_TBL_OPCODE_DELETE, CA_E_PARAM);

    if (fdb->key_vid == 0xffff) fdb->key_vid = 0;

    if (fdb->static_flag > 1 || fdb->dot1p > 7 ||
        fdb->key_sc_ind > 1 || fdb->key_vid > 0xfff  ||
        fdb->da_permit > 1 || fdb->sa_permit > 1 || fdb->mc_flag > 1) {
        L2_DEBUG("invalid data\r\n");
        return CA_E_PARAM;
    }

    /* aging status should not be ZERO for dynamic entry */
    if (fdb->aging_sts == 0 && fdb->static_flag == 0 && op_cmd != __FDB_TBL_OPCODE_DELETE){
        L2_DEBUG("invalid aging sts\r\n");
        return CA_E_PARAM;
    }


    memset((void *)&cmd_return, 0, sizeof(L2FE_L2E_FDB_CMD_RETURN_t));
    memset((void *)&fdb_data0, 0, sizeof(L2FE_L2E_FDB_DATA0_t));
    memset((void *)&fdb_data1, 0, sizeof(L2FE_L2E_FDB_DATA1_t));
    memset((void *)&fdb_data2, 0, sizeof(L2FE_L2E_FDB_DATA2_t));
    memset((void *)&fdb_data3, 0, sizeof(L2FE_L2E_FDB_DATA3_t));

    __aal_fdb_sc_ind_get(0, fdb->port_id, &fdb->key_sc_ind);

    L2_DEBUG("port_id:%d, sc_ind:%d\r\n", fdb->port_id, fdb->key_sc_ind);

    __aal_mac_2_fdb_data(&fdb_data3, &fdb_data2, &fdb_data1, fdb->mac);
    fdb_data1.bf.key_vid      = fdb->key_vid;
    fdb_data1.bf.key_scind    = fdb->key_sc_ind;
    fdb_data1.bf.key_dot1p    = fdb->dot1p;
    fdb_data1.bf.lrned_vid    = (fdb->lrnt_vid >> 9) & 0x7;
    fdb_data0.bf.lrned_vid    = fdb->lrnt_vid & 0x1FF;
    fdb_data0.bf.lrned_scind  = fdb->lrnt_sc_ind;
    fdb_data0.bf.sa_permit    = fdb->sa_permit;
    fdb_data0.bf.da_permit    = fdb->da_permit;
    fdb_data0.bf.is_static    = fdb->static_flag;
    fdb_data0.bf.aging_sts    = fdb->aging_sts;
    fdb_data0.bf.mc_flag      = fdb->mc_flag;
    if (fdb->mc_flag)
        fdb_data0.bf.mcgid        = fdb->port_id;
    else{
        data0_p->bf.entry_valid = 1;
        data0_p->bf.lpid = fdb->port_id;

    }

    if (__aal_fdb_access(device_id, op_cmd,
                         NULL,
                         NULL,
                         NULL,
                         NULL,
                         NULL,
                         (ca_uint32 *)&fdb_data3,
                         (ca_uint32 *)&fdb_data2,
                         (ca_uint32 *)&fdb_data1,
                         (ca_uint32 *)&fdb_data0,
                         (ca_uint32 *)&cmd_return) != CA_E_OK) {
        L2_DEBUG("fdb access fail\r\n");
        return CA_E_ERROR;
    }

    /* Check ret */
    if (cmd_return.bf.status != CA_AAL_FDB_ACCESS_HIT) {
        L2_DEBUG("op fail, status:%d\r\n", cmd_return.bf.status);
        return CA_E_ERROR;
    }

    L2_DEBUG("done\r\n");

    return CA_E_OK;
}

ca_int32 __aal_fdb_tbl_cmp(void *key_x, void *key_y)
{
    aal_tbl_fdb_key_t *x = (aal_tbl_fdb_key_t *)key_x;
    aal_tbl_fdb_key_t *y = (aal_tbl_fdb_key_t *)key_y;

    CA_ASSERT_RET(key_x != NULL && key_y != NULL, 0);

    if (x->vid < y->vid)
        return -1;

    if (x->vid > y->vid)
        return 1;

    if (memcmp(x->mac, y->mac, sizeof(ca_mac_addr_t)) < 0)
        return -1;

    if (memcmp(x->mac, y->mac, sizeof(ca_mac_addr_t)) > 0)
        return 1;
    /*
    if (x->dot1p < y->dot1p)
        return -1;

    if (x->dot1p > y->dot1p)
        return 1;
    */
    return 0;
}

ca_int32 __aal_fdb_tbl_cmp_svl(void *key_x, void *key_y)
{
    aal_tbl_fdb_key_t *x = (aal_tbl_fdb_key_t *)key_x;
    aal_tbl_fdb_key_t *y = (aal_tbl_fdb_key_t *)key_y;

    CA_ASSERT_RET(key_x != NULL && key_y != NULL, 0);

    if (memcmp(x->mac, y->mac, sizeof(ca_mac_addr_t)) < 0)
        return -1;

    if (memcmp(x->mac, y->mac, sizeof(ca_mac_addr_t)) > 0)
        return 1;

    return 0;
}


ca_status_t __aal_fdb_tbl_create(void)
{
    fdb_tbl_p = ca_table_create(CA_AAL_TBL_FDB_NAME,
                                CA_RBTREE_TABLE,
                                sizeof(aal_fdb_entry_cfg_t),
                                __aal_fdb_tbl_cmp);
    fdb_tbl_svl_p = ca_table_create(CA_AAL_TBL_FDB_SVL_NAME,
                                CA_RBTREE_TABLE,
                                sizeof(aal_fdb_entry_cfg_t),
                                __aal_fdb_tbl_cmp_svl);

    L2_DEBUG("fdb_tbl_p:%p, svl:%p\r\n", fdb_tbl_p, fdb_tbl_svl_p);

    return CA_E_OK;
}

ca_status_t __aal_fdb_tbl_destory(void)
{
    ca_int32 ret;

    ret = ca_table_destory(fdb_tbl_p);
    ret += ca_table_destory(fdb_tbl_svl_p);

    L2_DEBUG("ret:%d\r\n", ret);

    if (ret == 0) {
        fdb_tbl_p = NULL;
        fdb_tbl_svl_p = NULL;
        return CA_E_OK;
    }
    else
        return CA_E_ERROR;
}

ca_status_t __aal_fdb_tbl_add_del(ca_uint32 op_cmd, aal_fdb_entry_cfg_t *fdb)
{
    aal_tbl_fdb_key_t x;
    ca_int32 ret;

    CA_ASSERT_RET(fdb != NULL, CA_E_PARAM);
    CA_ASSERT_RET(op_cmd == __FDB_TBL_OPCODE_APPEND ||
                  op_cmd == __FDB_TBL_OPCODE_APPEND_REPLACE ||
                  op_cmd == __FDB_TBL_OPCODE_DELETE, CA_E_PARAM);

    L2_DEBUG("static_flag:%d, op_cmd:0x%x, fdb_tbl_p:%p/%p\r\n", fdb->static_flag, op_cmd, fdb_tbl_p, fdb_tbl_svl_p);

    if (!fdb->static_flag){
        L2_DEBUG("is NOT static, don't buffer it\r\n");
        return CA_E_OK;
    }

    memset((void *)&x, 0, sizeof(aal_tbl_fdb_key_t));

    x.dot1p = fdb->dot1p;
    memcpy(x.mac, fdb->mac, CA_ETH_ADDR_LEN);
    x.vid = fdb->key_vid;

    if (op_cmd == __FDB_TBL_OPCODE_APPEND || op_cmd == __FDB_TBL_OPCODE_APPEND_REPLACE){
        ret = ca_table_entry_insert(fdb_tbl_p, (void *)&x, (void *)fdb);
        ret += ca_table_entry_insert(fdb_tbl_svl_p, (void *)&x, (void *)fdb);
    }
    else{
        ret = ca_table_entry_remove(fdb_tbl_p, (void *)&x);
        ret += ca_table_entry_remove(fdb_tbl_svl_p, (void *)&x);
    }

    L2_DEBUG("op: %s, ret:%d\r\n",
             op_cmd == __FDB_TBL_OPCODE_APPEND || op_cmd == __FDB_TBL_OPCODE_APPEND_REPLACE ? "add" : "del", ret);

    if (ret == 0)
        return CA_E_OK;
    else
        return CA_E_ERROR;
}
#else//sd1 uboot for 98f
#endif//sd1 uboot for 98f

ca_status_t __aal_fdb_addr_delete_by_port (
    CA_IN ca_device_id_t           device_id,
    CA_IN ca_port_id_t          aal_port,
    CA_IN ca_uint32_t           flag
)
{
    aal_fdb_cpu_cmd_word4_msk_t  flush_mask;
    aal_fdb_flush_cfg_t          flush_cfg;
    ca_status_t                  ret = CA_E_OK;

    flush_mask.wrd = ~0U;

    switch (flag) {
    case CA_L2_ADDR_OP_STATIC:
        flush_mask.bf.static_flag = 0;
        flush_cfg.static_flag     = 1;
        break;
    case CA_L2_ADDR_OP_DYNAMIC:
        flush_mask.bf.static_flag = 0;
        flush_cfg.static_flag     = 0;
        break;
    case CA_L2_ADDR_OP_BOTH:
        flush_mask.bf.static_flag = 1;
        break;
    default:
        return CA_E_PARAM;
    }

    flush_mask.bf.lpid  = 0;
    flush_cfg.lpid_high = aal_port;
    flush_cfg.lpid_low  = aal_port;

    ret = aal_fdb_scan_flush_field_set(device_id, flush_mask, &flush_cfg);

    if (CA_E_OK == ret)
        ret = aal_fdb_entry_flush(device_id);

    return ret;
}

#if 0//yocto
ca_int32 __aal_mac_filter_tbl_cmp(void *key_x, void *key_y)
{
    aal_tbl_mac_filter_key_t *x = (aal_tbl_mac_filter_key_t *)key_x;
    aal_tbl_mac_filter_key_t *y = (aal_tbl_mac_filter_key_t *)key_y;

    CA_ASSERT_RET(key_x != NULL && key_y != NULL, 0);


    L2_DEBUG("x: mac:0x%02x:%02x:%02x:%02x:%02x:%02x, vlan:%d, port:%d, mac_flag:%d\r\n",
        x->mac[0], x->mac[1], x->mac[2], x->mac[3], x->mac[4], x->mac[5],
        x->vid, x->port_id, x->mac_flag);

    L2_DEBUG("y: mac:0x%02x:%02x:%02x:%02x:%02x:%02x, vlan:%d, port:%d, mac_flag:%d\r\n",
        y->mac[0], y->mac[1], y->mac[2], y->mac[3], y->mac[4], y->mac[5],
        y->vid, y->port_id, y->mac_flag);


    if (x->port_id < y->port_id)
        return -1;

    if (x->port_id > y->port_id)
        return 1;

    if (memcmp(x->mac, y->mac, sizeof(ca_mac_addr_t)) < 0)
        return -1;

    if (memcmp(x->mac, y->mac, sizeof(ca_mac_addr_t)) > 0)
        return 1;

    if (x->vid < y->vid)
        return -1;

    if (x->vid > y->vid)
        return 1;

    if (x->mac_flag < y->mac_flag)
        return -1;

    if (x->mac_flag > y->mac_flag)
        return 1;


    return 0;
}
////#else//sd1 uboot for 98f
////#endif//sd1 uboot for 98f

ca_status_t __aal_mac_filter_tbl_create(void)
{
    mac_filter_tbl_p = ca_table_create(CA_AAL_TBL_MAC_FILTER_NAME,
                                       CA_RBTREE_TABLE,
                                       sizeof(aal_tbl_mac_filter_entry_t),
                                       __aal_mac_filter_tbl_cmp);

    L2_DEBUG("mac_filter_tbl_p:%p\r\n", mac_filter_tbl_p);
    return CA_E_OK;
}

////#if 0//yocto
ca_status_t __aal_mac_filter_tbl_destory(void)
{
    ca_int32 ret;

    ret = ca_table_destory(mac_filter_tbl_p);

    L2_DEBUG("ret:%d\r\n", ret);

    if (ret == 0) {
        mac_filter_tbl_p = NULL;
        return CA_E_OK;
    }
    else
        return CA_E_ERROR;
}

/*
  rule_id is base 1,
  return   0, full

 */
ca_uint32  __aal_mac_filter_alloc_rule_id(ca_int32 port_id)
{
    ca_uint8 rule[CA_AAL_FDB_MAC_FILTER_RULE] = {0};
    aal_tbl_mac_filter_entry_t *tmp_p;
    ca_int32 i;
    ca_uint32 rule_id = 0;

    tmp_p = (aal_tbl_mac_filter_entry_t *)ca_table_entry_getfirst(
                mac_filter_tbl_p);
    L2_DEBUG("tmp_p:%p\r\n", tmp_p);

    while (tmp_p) {

        if (tmp_p->port_id == port_id) {
            if (tmp_p->rule_id > CA_AAL_FDB_MAC_FILTER_RULE) {
                L2_DEBUG("fatal error, rule_id:%d\r\n", tmp_p->rule_id);
                break;
            }

            rule[tmp_p->rule_id - 1] = 1;
        }

        tmp_p = (aal_tbl_mac_filter_entry_t *)ca_table_entry_getnext(
                    mac_filter_tbl_p);

        if (tmp_p == NULL) {
            L2_DEBUG("search end\r\n");
            break;
        }
    }

    for (i = 0; i < CA_AAL_FDB_MAC_FILTER_RULE; i++) {
        if (rule[i] == 0) {
            rule_id = i + 1;
            L2_DEBUG("find available! rule id:%d\r\n", rule_id);
            break;
        }
    }

    L2_DEBUG("allocate rule id:%d\r\n", rule_id);

    return rule_id;
}

/* take rule id back in entry if new rule */
ca_status_t __aal_mac_filter_tbl_add_del(CA_IN ca_uint32 op_cmd,
        CA_IN_OUT aal_tbl_mac_filter_entry_t *entry)
{
    aal_tbl_mac_filter_key_t x;
    //aal_tbl_mac_filter_entry_t *filter_p = NULL;
    ca_int32 ret;
    //ca_uint32 rule_id = 0;

    CA_ASSERT_RET(entry != NULL, CA_E_PARAM);
    CA_ASSERT_RET(op_cmd == __FDB_TBL_OPCODE_APPEND ||
                  op_cmd == __FDB_TBL_OPCODE_DELETE, CA_E_PARAM);
    //CA_ASSERT_RET(entry->mask != CA_L2_MAC_FILTER_MASK_MAC_ONLY && entry->mask != CA_L2_MAC_FILTER_MASK_MAC_VLAN, CA_E_PARAM);

    /*
    rule_cnt = ca_table_entry_count(mac_filter_tbl_p);
    if (rule_cnt >= CA_AAL_FDB_MAC_FILTER_RULE)
        return CA_E_UNAVAIL;
    */
    memset((void *)&x, 0, sizeof(aal_tbl_mac_filter_key_t));

    memcpy(x.mac, entry->mac, CA_ETH_ADDR_LEN);
    x.mac_flag = entry->mac_flag;
    x.port_id = entry->port_id;

    if (CA_L2_MAC_FILTER_MASK_MAC_VLAN == entry->mask)
        x.vid = entry->vid;

    if (op_cmd == __FDB_TBL_OPCODE_APPEND) {
#if 0
    /* actually, rule id is allocate in caller */
        /* 1. search */
        filter_p = (aal_tbl_mac_filter_entry_t *)ca_table_entry_search(
                       mac_filter_tbl_p, &x);

        /* 2. allocate rule id if no found */
        if (filter_p == NULL) {
            L2_DEBUG("new rule\r\n");
            rule_id = __aal_mac_filter_alloc_rule_id(entry->port_id);

            if (rule_id == 0) {
                /* full */
                L2_DEBUG("unavailable  port:%d, rule id:%d\r\n", entry->port_id, rule_id);
                return CA_E_UNAVAIL;
            }

            entry->rule_id = rule_id;
        }
#endif
        ret = ca_table_entry_insert(mac_filter_tbl_p, (void *)&x, (void *)entry);
    }
    else
        ret = ca_table_entry_remove(mac_filter_tbl_p, (void *)&x);

    L2_DEBUG("op: %s, ret:%d\r\n",
             op_cmd == __FDB_TBL_OPCODE_APPEND ? "add" : "del", ret);

    if (ret == 0)
        return CA_E_OK;
    else
        return CA_E_ERROR;
}


ca_status_t __aal_mac_filter_tbl_flush_port(ca_int32 port_id)
{
    aal_tbl_mac_filter_entry_t *tmp_p;
    ca_rbtree_node_t *node_p, *next_p;
    ca_rbtree_t *mac_filter_rb_p;
    ca_status_t ret;

    if (mac_filter_tbl_p == NULL || mac_filter_tbl_p->content == NULL) {
        L2_DEBUG("fatal, empty\r\n");
        return CA_E_EMPTY;
    }

    L2_DEBUG("port_id:%d/0x%x\r\n", port_id, port_id);

    mac_filter_rb_p = (ca_rbtree_t *)mac_filter_tbl_p->content;
    node_p = ca_rbtree_first(mac_filter_rb_p);

    while (node_p != mac_filter_rb_p->nil) {

        next_p = ca_rbtree_successor(mac_filter_rb_p, node_p);

        if (node_p == next_p){
            L2_DEBUG("node_p:%p == next_p, break out, need check\r\n", node_p, next_p);
            break;
        }

        tmp_p = (aal_tbl_mac_filter_entry_t *)node_p->data;

        if(tmp_p){
            L2_DEBUG("mac:0x%02x:%02x:%02x:%02x:%02x:%02x, vlan:%d, port:%d, tree cnt:%d\r\n",
                tmp_p->mac[0], tmp_p->mac[1], tmp_p->mac[2], tmp_p->mac[3], tmp_p->mac[4], tmp_p->mac[5],
                tmp_p->vid, tmp_p->port_id, ca_rbtree_count(mac_filter_rb_p));
        }
        if (tmp_p && tmp_p->port_id == port_id) {
            L2_DEBUG("find, rule_id:%d, mac_filter_rb_p:%p, node_p:%p\r\n",
                    tmp_p->rule_id, mac_filter_rb_p, node_p);
            ret = ca_rbtree_delete(mac_filter_rb_p, node_p);
        }

        node_p = next_p;

    }

    L2_DEBUG("\r\n");
    return CA_E_OK;
}

aal_tbl_mac_filter_entry_t *__aal_mac_filter_rule_get_by_rule(
    CA_IN ca_int32 port_id,
    CA_IN ca_uint32 rule_id
)
{
    aal_tbl_mac_filter_entry_t *tmp_p = NULL;

    CA_ASSERT_RET(rule_id <= CA_AAL_FDB_MAC_FILTER_DFT_ID, NULL);

    tmp_p = (aal_tbl_mac_filter_entry_t *)ca_table_entry_getfirst(
                mac_filter_tbl_p);
    L2_DEBUG("tmp_p:%p, rule id:%d\r\n", tmp_p, rule_id);

    while (tmp_p) {

        if (tmp_p->port_id == port_id && tmp_p->rule_id == rule_id) {
            L2_DEBUG("find, rule_id:%d\r\n", tmp_p->rule_id);
            break;
        }

        tmp_p = (aal_tbl_mac_filter_entry_t *)ca_table_entry_getnext(
                    mac_filter_tbl_p);

        if (tmp_p == NULL) {
            L2_DEBUG("search end\r\n");
            break;
        }
    }

    return tmp_p;
}

aal_tbl_mac_filter_entry_t *__aal_mac_filter_rule_get_by_key(
    CA_IN ca_int32 port_id,
    CA_IN aal_tbl_mac_filter_entry_t *entry
)
{
    aal_tbl_mac_filter_entry_t *tmp_p = NULL;
    aal_tbl_mac_filter_key_t key;

    CA_ASSERT_RET(entry != NULL, NULL);

    memset(&key, 0, sizeof(aal_tbl_mac_filter_key_t));

    memcpy(key.mac, entry->mac, CA_ETH_ADDR_LEN);
    key.mac_flag = entry->mac_flag;
    key.port_id = port_id;
    if (entry->mask == CA_L2_MAC_FILTER_MASK_MAC_VLAN)
        key.vid = entry->vid;
    tmp_p = (aal_tbl_mac_filter_entry_t *)ca_table_entry_search(mac_filter_tbl_p,
            (void *)&key);

    return tmp_p;
}

ca_status_t __aal_mac_filter_load_cls_rule(
    ca_int32_t port_id,
    aal_l2_cls_l2rl2r_key_mask_t *cls_mask,
    ca_uint32_t   rule,
    aal_l2_cls_l2r_key_t *l2r,
    ca_mac_addr_t mac,
    ca_uint32_t   mac_flag,   /* 0 sa, 1 da */
    ca_uint32_t   mask,
    ca_uint32_t   vid)
{

    ca_boolean_t is_svlan = 0;

    CA_ASSERT_RET(cls_mask != NULL && l2r != NULL, CA_E_PARAM);

    __aal_fdb_sc_ind_get(0, port_id, &is_svlan);
    /* mac */
    //l2r->l2_key_mask.s.mac_field_ctrl = 1;
    memcpy(l2r->mac_da, mac, CA_ETH_ADDR_LEN);
    memcpy(l2r->mac_sa, mac, CA_ETH_ADDR_LEN);

    if (mac_flag == 0)
        l2r->mac_field_ctrl = AAL_L2_CLS_MAC_FIELD_CTRL_SA_RANGE;
    else
        l2r->mac_field_ctrl = AAL_L2_CLS_MAC_FIELD_CTRL_DA_RANGE;

    /* vlan */
    if (mask == CA_L2_MAC_FILTER_MASK_MAC_VLAN) {
        //l2r->l2_key_mask.s.vid_field_ctrl = 1;
        l2r->top_vlan_id = vid;
        l2r->inner_vlan_id = vid;
        l2r->top_is_svlan = is_svlan;
        l2r->vlan_field_ctrl = AAL_L2_CLS_VLAN_FIELD_CTRL_TOP_RANGE;
    }else{
        l2r->vlan_field_ctrl = AAL_L2_CLS_VLAN_FIELD_CTRL_MASK_VLAN;
    }
    l2r->dot1p_ctrl         = AAL_L2_CLS_DOT1P_FIELD_CTRL_MASK_DOT1P;
    l2r->l4_port_field_ctrl = AAL_L2_CLS_L4_PORT_FIELD_CTRL_MASK_L4PORT;
    l2r->vlan_number_vld    = FALSE;
    l2r->ethernet_type_vld  = FALSE;

    /* set mask */
    if (rule == 0){
        cls_mask->s.mac_sa_0 = 1;
        cls_mask->s.mac_da_0 = 1;

        cls_mask->s.mac_field_ctrl_0 = 1;

        cls_mask->s.top_vlan_id_0 = 1;
        cls_mask->s.inner_vlan_id_0 = 1;
        cls_mask->s.top_is_svlan_0 = 1;
        cls_mask->s.vlan_field_ctrl_0 = 1;
        cls_mask->s.dot1p_ctrl_0 = 1;
        cls_mask->s.l4_port_field_ctrl_0 = 1;
        cls_mask->s.vlan_number_vld_0 = 1;
        cls_mask->s.ethernet_type_vld_0 = 1;

    }
    else{
        cls_mask->s.mac_sa_1 = 1;
        cls_mask->s.mac_da_1 = 1;

        cls_mask->s.mac_field_ctrl_1 = 1;

        cls_mask->s.top_vlan_id_1 = 1;
        cls_mask->s.inner_vlan_id_1 = 1;
        cls_mask->s.top_is_svlan_1 = 1;
        cls_mask->s.vlan_field_ctrl_1 = 1;

        cls_mask->s.dot1p_ctrl_1 = 1;
        cls_mask->s.l4_port_field_ctrl_1 = 1;
        cls_mask->s.vlan_number_vld_1 = 1;
        cls_mask->s.ethernet_type_vld_1 = 1;
    }

    return CA_E_OK;
}
/*
 * ----------------------------------------------------------------- *
 */

ca_status_t aal_fdb_debug_set(
    CA_IN ca_device_id_t device_id,
    CA_IN ca_int32_t value1,
    CA_IN ca_int32_t value2
)
{
    ca_printf("\r\nvalue1 1: Set value2 to aal_fdb_debug\r\n ");

    switch (value1){
        case 1:
            aal_fdb_debug = value2;
            ca_printf("fdb debug %d\r\n", aal_fdb_debug);
            break;
        case 2:
            {
                ca_iterator_t entry;
                ca_l2_addr_entry_t *l2_addr, *cfg;
                ca_status_t ret = CA_E_OK;
                int cnt = 0, j;
                int sz = 0;

                printk("ca_l2_addr_iterate\r\n");
                memset(&entry, 0, sizeof(ca_iterator_t));
                entry.entry_count = value2;
                sz = sizeof(ca_l2_addr_entry_t) * entry.entry_count;

                l2_addr = ca_malloc(sz);

                memset(l2_addr, 0, sz);
                entry.p_entry_data = l2_addr;
                entry.user_buffer_size = sizeof(ca_l2_addr_entry_t) * entry.entry_count;

                /*while (1)*/{
                    ret = ca_l2_addr_iterate(0, &entry);

                    if (ret == CA_E_UNAVAIL){
                        printk("end search\r\n");
                  //      break;
                    }

                }

                printk("entry_count = %#x \n",entry.entry_count);
                printk("p_entry_data = %p\n",entry.p_entry_data);
                printk("user_buffer_size = %#x \n",entry.user_buffer_size);
                printk("p_prev_handle = %p, %d\n",entry.p_prev_handle, (ca_uint32_t)entry.p_prev_handle);

                for (cnt = 0; cnt < entry.entry_count; cnt++){
                    cfg = l2_addr + cnt;
                    printk(" mac:");
                    for(j = 0 ; j < 6; j++){
                        printk("%x:", cfg->mac_addr[j]);
                    }
                    printk(" key_vid:%d, lrn_vid:%d, dot1p:%d, static:%d, sa:%d, da:%d, port_id:%#x, mc:%d, age:%d\r\n",
                        cfg->vid, cfg->fwd_vid, cfg->dot1p, cfg->static_flag,
                        cfg->sa_permit, cfg->da_permit, cfg->port_id, cfg->mc_group_id, cfg->aging_timer);
                }

                ca_free(l2_addr);
            }
            break;
        case 3:
            {
                ca_iterator_t entry;
                ca_l2_addr_entry_t *l2_addr, *cfg;
                ca_status_t ret = CA_E_OK;
                int cnt = 0, j;
                int sz = 0;

                printk("ca_l2_addr_iterate\r\n");
                memset(&entry, 0, sizeof(ca_iterator_t));
                entry.entry_count = 1;
                sz = sizeof(ca_l2_addr_entry_t) * entry.entry_count;

                l2_addr = ca_malloc(sz);

                memset(l2_addr, 0, sz);
                entry.p_entry_data = l2_addr;
                entry.user_buffer_size = sizeof(ca_l2_addr_entry_t) * entry.entry_count;

                for(cnt = 0; cnt < value2; cnt++){
                    ret = ca_l2_addr_iterate(0, &entry);

                    if (ret == CA_E_UNAVAIL){
                        printk("end search\r\n");
                        break;
                    }


                    printk("entry_count = %#x \n",entry.entry_count);
                    printk("p_entry_data = %p\n",entry.p_entry_data);
                    printk("user_buffer_size = %#x \n",entry.user_buffer_size);
                    printk("p_prev_handle = %p, %d\n",entry.p_prev_handle, (ca_uint32_t)entry.p_prev_handle);

                    cfg = l2_addr;
                    printk(" mac:");
                    for(j = 0 ; j < 6; j++){
                        printk("%x:", cfg->mac_addr[j]);
                    }
                    printk(" key_vid:%d, lrn_vid:%d, dot1p:%d, static:%d, sa:%d, da:%d, port_id:%#x, mc:%d, age:%d\r\n",
                        cfg->vid, cfg->fwd_vid, cfg->dot1p, cfg->static_flag,
                        cfg->sa_permit, cfg->da_permit, cfg->port_id, cfg->mc_group_id, cfg->aging_timer);

                }

                ca_free(l2_addr);
            }
            break;
        case 4:
            aal_fdb_delete_oldest_entry_by_port(0, value2);
            break;
        case 5:
            {
                aal_tbl_mac_filter_entry_t filter_entry;
                ca_status_t ret = CA_E_OK;
                ca_int32 lpid;
                ca_mac_addr_t mac = {0};
                int i;

                memset(&filter_entry, 0, sizeof(aal_tbl_mac_filter_entry_t));

                filter_entry.mask = CA_L2_MAC_FILTER_MASK_MAC_ONLY;
                filter_entry.mac_flag = 1;
                filter_entry.vid = 0;
                filter_entry.drop_flag = 1;

                for (lpid = 1; lpid < 5; lpid++){
                    for (i = 0; i < CA_AAL_FDB_MAC_FILTER_RULE; i++){
                        mac[3] = lpid;
                        mac[5] = i + 1;
                        memcpy(filter_entry.mac, mac, CA_ETH_ADDR_LEN);

                        ret = aal_fdb_mac_filter_add(0, lpid, &filter_entry);
                    }
                }
            }
            break;
        case 6:
            {
                lrn_fwd_ctrl_t       fwd_ctrl;
                lrn_fwd_ctrl_mask_t  fwd_ctrl_mask;

                memset(&fwd_ctrl, 0, sizeof(lrn_fwd_ctrl_t));
                memset(&fwd_ctrl_mask, 0, sizeof(lrn_fwd_ctrl_mask_t));
                fwd_ctrl_mask.bf.sa_hash_cllsn_fwd_ctrl = 1;
                fwd_ctrl_mask.bf.sa_hash_cllsn_ldpid = 1;
                fwd_ctrl_mask.bf.sa_limit_excd_fwd_ctrl = 1;
                fwd_ctrl_mask.bf.sa_limit_excd_ldpid = 1;
                fwd_ctrl_mask.bf.l2_mac_sw_lrn_ldpid = 1;

                fwd_ctrl.hash_cllsn_mode = value2;
                fwd_ctrl.hash_cllsn_ldpid = AAL_LPORT_CPU_0;
                fwd_ctrl.limit_excd_mode = value2;
                fwd_ctrl.limit_excd_ldpid = AAL_LPORT_CPU_0;
                fwd_ctrl.l2_mac_sw_lrn_ldpid = AAL_LPORT_CPU_0;

                aal_fdb_lrn_fwd_ctrl_set(device_id, fwd_ctrl_mask, &fwd_ctrl);
            }
            break;
        case 7:
            {
                int i, j;
                aal_port_flooding_t *f_p = &flood_cb;

                printk("---unkwn pol idx to lpid mapping---\r\n");
                for (i = 0; i < AAL_PORT_NUM_LOGICAL; i++){
                    printk("lspid_policer[%d] = %d\r\n", i, f_p->lspid_policer[i]);
                }

                printk("---port type profile assignment---\r\n");
                for (i = 0; i < AAL_ILPB_UNKWN_POL_NUM; i++){
                    for (j = 0; j <= CA_L2_FLOODING_TYPE_UUC; j++){
                        printk("lspid_type_prof[%d][%d].profile_id = %d\r\n", i, j, f_p->lspid_type_prof[i][j].profile_id);
                        //printk("                    .pkt_fwd_type = %d\r\n", f_p->l2te_pol_profile[i].pkt_fwd_type);
                        //printk("                    .rate = %d\r\n", f_p->l2te_pol_profile[i].rate);
                        //printk("                    .pir_m = %d\r\n", f_p->l2te_pol_profile[i].pir_m);
                        //printk("                    .p_bs = %d\r\n", f_p->l2te_pol_profile[i].p_bs);
                    }
                }

                printk("---profile rate---\r\n");
                for (i = 0; i < AAL_L2TM_L2TE_POL_PKT_TYPE_PROFILE_NUM; i++){
                    printk("profile[%d].rate = %d\r\n", i, f_p->profile[i].rate);
                }
            }
            break;
           case 8:
            {    ca_iterator_t entry;
                 ca_l2_mac_filter_entry_iterator_t *entry_iterator = NULL;
                 ca_l2_mac_filter_entry_t *mac_filter_entry = NULL ;
                 ca_uint32 sz = 0,cnt = 0,j=0;
                 ca_uint32_t entry_size = sizeof(ca_l2_mac_filter_entry_iterator_t);

                printk("........Now testing ca_l2_mac_filter_iterate start......\r\n");

                memset(&entry,0,sizeof(ca_iterator_t));

                entry.entry_count = value2;
                sz = entry_size*entry.entry_count;

                entry_iterator = ca_malloc(sz);
                memset(entry_iterator,0,sz);

                entry.p_entry_data = entry_iterator;
                entry.user_buffer_size = sz;

                ca_l2_mac_filter_iterate(0,&entry);

                printk("entry_count = %#x \n",entry.entry_count);
                printk("p_entry_data = %p\n",entry.p_entry_data);
                printk("user_buffer_size = %#x \n",entry.user_buffer_size);
                printk("p_prev_handle = %p, %d\n",entry.p_prev_handle, (ca_uint32_t)entry.p_prev_handle);

               for (cnt = 0; cnt < value2; cnt++){
                    entry_iterator = entry.p_entry_data+cnt*entry_size;
                    mac_filter_entry = entry.p_entry_data +cnt*entry_size+sizeof(ca_port_id_t);
                    printk(" mac:");
                    for(j = 0 ; j < 6; j++){
                        printk("%x:", mac_filter_entry->mac[j]);
                    }
                    printk("port_id :%d, mask :%d, mac_flag:%d, vid :%d, drop_flag:%d\r\n",entry_iterator->port_id, mac_filter_entry->mask,\
                                    mac_filter_entry->mac_flag,mac_filter_entry->vid,mac_filter_entry->drop_flag);
                }
                ca_free(entry_iterator);
                entry_iterator = NULL;
                mac_filter_entry = NULL ;
                printk("........Now testing ca_l2_mac_filter_iterate end......\r\n");
            }
            break;
        case 11:
        {
            ca_status_t ret = CA_E_ERROR;
            ca_l2_addr_entry_t  cfg;
            int j;

            memset(&cfg, 0, sizeof(ca_l2_addr_entry_t));
            ret = ca_l2_addr_aged_event_get(0, &cfg);
            ca_printf("<%s, %d>ret:%d\r\n", __FUNCTION__, __LINE__, ret);

            if (ret == CA_E_OK){
                ca_printf(" mac:");
                for(j = 0 ; j < 6; j++){
                    ca_printf("%x:", cfg.mac_addr[j]);
                }
                ca_printf(" key_vid:%d, lrn_vid:%d, dot1p:%d, static:%d, sa:%d, da:%d, port_id:%#x, mc:%d, age:%d\r\n",
                    cfg.vid, cfg.fwd_vid, cfg.dot1p, cfg.static_flag,
                    cfg.sa_permit, cfg.da_permit, cfg.port_id, cfg.mc_group_id, cfg.aging_timer);

                ca_l2_addr_delete(0, &cfg);
            }
        }
            break;
        default:
            break;
    }

    return CA_E_OK;
}

ca_status_t aal_fdb_debug_get(
    CA_IN ca_device_id_t device_id,
    CA_IN ca_int32_t  value1,
    CA_OUT ca_int32_t *value2
)
{

    if (value2) *value2 = aal_fdb_debug;

    ca_printf("fdb debug %d\r\n", aal_fdb_debug);

    return CA_E_OK;
}


ca_status_t aal_fdb_tbl_del_all()
{
    ca_int32 ret;

    __FDB_LOCK();

    ret = ca_table_entry_remove_all(fdb_tbl_p);
    ret += ca_table_entry_remove_all(fdb_tbl_svl_p);
    /*
    ret = __aal_fdb_tbl_destory();

    if (ret == 0)
        ret = __aal_fdb_tbl_create();
    */
    __FDB_UNLOCK();

    if (ret == 0)
        return CA_E_OK;
    else
        return CA_E_ERROR;
}

ca_status_t aal_fdb_entry_get(
    CA_IN ca_device_id_t            device_id,
    CA_IN ca_mac_addr_t          mac,
    CA_IN ca_uint16_t            key_vid,
    CA_OUT aal_fdb_entry_cfg_t  *fdb_entry
)
{
    L2FE_L2E_FDB_DATA0_t fdb_data0;
    L2FE_L2E_FDB_DATA1_t fdb_data1;
    L2FE_L2E_FDB_DATA2_t fdb_data2;
    L2FE_L2E_FDB_DATA3_t fdb_data3;

    L2FE_L2E_FDB_CMD_RETURN_t   cmd_return;
    ca_status_t         ret;

    CA_ASSERT_RET(fdb_entry != NULL, CA_E_PARAM);

    if (key_vid > 0xFFF) {
        L2_DEBUG("invalid vid:%d\r\n", key_vid);
        return CA_E_PARAM;
    }

    memset((void *)&cmd_return, 0, sizeof(L2FE_L2E_FDB_CMD_RETURN_t));
    memset((void *)&fdb_data0, 0, sizeof(L2FE_L2E_FDB_DATA0_t));
    memset((void *)&fdb_data1, 0, sizeof(L2FE_L2E_FDB_DATA1_t));
    memset((void *)&fdb_data2, 0, sizeof(L2FE_L2E_FDB_DATA2_t));
    memset((void *)&fdb_data3, 0, sizeof(L2FE_L2E_FDB_DATA3_t));

    __aal_mac_2_fdb_data(&fdb_data3, &fdb_data2, &fdb_data1, mac);

    fdb_data1.bf.key_vid = key_vid;
    fdb_data1.bf.key_scind    = fdb_entry->key_sc_ind;

    /* TODO 2 bit for dot1p ?? */
    //fdb_data1.bf.key_dot1p = sig->dot1p;

    //__FDB_LOCK();

    ret = __aal_fdb_access(device_id, __FDB_TBL_OPCODE_READ,
                           NULL,
                           NULL,
                           NULL,
                           NULL,
                           NULL,
                           (ca_uint32 *)&fdb_data3,
                           (ca_uint32 *)&fdb_data2,
                           (ca_uint32 *)&fdb_data1,
                           (ca_uint32 *)&fdb_data0,
                           (ca_uint32 *)&cmd_return);

    if (ret != CA_E_OK) {
        //__FDB_UNLOCK();
        L2_DEBUG("access fdb fail\r\n");
        return ret;
    }

    if (cmd_return.bf.status != CA_AAL_FDB_ACCESS_HIT) {
        //__FDB_UNLOCK();
        L2_DEBUG("fdb no find entry with sc:%d\r\n", fdb_entry->key_sc_ind);

        memset((void *)&cmd_return, 0, sizeof(L2FE_L2E_FDB_CMD_RETURN_t));
        memset((void *)&fdb_data0, 0, sizeof(L2FE_L2E_FDB_DATA0_t));
        memset((void *)&fdb_data1, 0, sizeof(L2FE_L2E_FDB_DATA1_t));
        memset((void *)&fdb_data2, 0, sizeof(L2FE_L2E_FDB_DATA2_t));
        memset((void *)&fdb_data3, 0, sizeof(L2FE_L2E_FDB_DATA3_t));

        /* it can not use __aal_fdb_sc_ind_get because there is not port id
         *
         * Use !key_sc_ind to get again
         */
        __aal_mac_2_fdb_data(&fdb_data3, &fdb_data2, &fdb_data1, mac);

        fdb_data1.bf.key_vid = key_vid;
        fdb_data1.bf.key_scind    = !fdb_entry->key_sc_ind;

        ret = __aal_fdb_access(device_id, __FDB_TBL_OPCODE_READ,
                               NULL,
                               NULL,
                               NULL,
                               NULL,
                               NULL,
                               (ca_uint32 *)&fdb_data3,
                               (ca_uint32 *)&fdb_data2,
                               (ca_uint32 *)&fdb_data1,
                               (ca_uint32 *)&fdb_data0,
                               (ca_uint32 *)&cmd_return);

        if (ret != CA_E_OK) {
            L2_DEBUG("access fdb fail\r\n");
            return ret;
        }

        if (cmd_return.bf.status != CA_AAL_FDB_ACCESS_HIT){
            L2_DEBUG("fdb STILL no find entry with !key_sc:%d\r\n", !fdb_entry->key_sc_ind);
            return CA_E_NOT_FOUND;
        }
    }

    __aal_fdb_data_2_mac(&fdb_data3, &fdb_data2, &fdb_data1, fdb_entry->mac);
    __aal_fdb_data_2_entry(fdb_data1, fdb_data0, fdb_entry);

    //__FDB_UNLOCK();

    return CA_E_OK;
}


ca_status_t aal_fdb_entry_add(
    CA_IN ca_device_id_t           device_id,
    CA_IN aal_fdb_entry_cfg_t  *fdb
)
{
    ca_status_t ret;

    //__FDB_LOCK();

    ret = __aal_fdb_entry_add_del(device_id, __FDB_TBL_OPCODE_APPEND, fdb);

    if (ret == CA_E_OK)
        ret = __aal_fdb_tbl_add_del(__FDB_TBL_OPCODE_APPEND, fdb);

    //__FDB_UNLOCK();

    L2_DEBUG("ret:%d\r\n", ret);
    return ret;
}

/*
 from l2fe mas, Table 81
 the addr[5] represents update with LRU algorithm overwrite while with hash collsion detected in hash UPD append mode
 */
ca_status_t aal_fdb_entry_add_replace(
    CA_IN ca_device_id_t           device_id,
    CA_IN aal_fdb_entry_cfg_t  *fdb
)
{
    ca_status_t ret;

    AAL_ASSERT_RET(fdb != NULL, CA_E_PARAM);
    //__FDB_LOCK();

    ret = __aal_fdb_entry_add_del(device_id, __FDB_TBL_OPCODE_APPEND_REPLACE, fdb);

    if (ret == CA_E_OK)
        ret = __aal_fdb_tbl_add_del(__FDB_TBL_OPCODE_APPEND_REPLACE, fdb);

    //__FDB_UNLOCK();

    L2_DEBUG("ret:%d\r\n", ret);
    return ret;
}


ca_status_t aal_fdb_entry_del(
    CA_IN ca_device_id_t           device_id,
    CA_IN aal_fdb_entry_cfg_t  *fdb
)
{
    ca_status_t ret;

    //__FDB_LOCK();

    ret = __aal_fdb_entry_add_del(device_id, __FDB_TBL_OPCODE_DELETE, fdb);

    if (ret == CA_E_OK)
        ret = __aal_fdb_tbl_add_del(__FDB_TBL_OPCODE_DELETE, fdb);

    //__FDB_UNLOCK();

    L2_DEBUG("ret:%d\r\n", ret);

    return ret;
}
#else//sd1 uboot for 98f
#endif//sd1 uboot for 98f

#if 0//yocto
ca_status_t aal_fdb_addr_aged_entry_get(
    CA_IN ca_device_id_t       device_id,
    CA_OUT aal_fdb_entry_cfg_t  *fdb_entry
)
{
    ca_status_t ret = CA_E_ERROR;
    L2FE_L2E_SW_AGING_REPORT_FIFO_STTS_t fifo_stts;
    L2FE_L2E_SW_AGING_CPT_ENTRY_3_t cpt_3;
    L2FE_L2E_SW_AGING_CPT_ENTRY_2_t cpt_2;
    L2FE_L2E_SW_AGING_CPT_ENTRY_1_t cpt_1;
    L2FE_L2E_SW_AGING_CPT_ENTRY_0_t cpt_0;
    int cnt = 0;
    L2FE_L2E_FDB_DATA0_t fdb_data0;
    L2FE_L2E_FDB_DATA1_t fdb_data1;
    L2FE_L2E_FDB_DATA2_t fdb_data2;
    L2FE_L2E_FDB_DATA3_t fdb_data3;

    L2FE_L2E_FDB_CMD_RETURN_t   cmd_return;

    CA_ASSERT_RET(fdb_entry != NULL, CA_E_PARAM);

    /*
    memset((void *)&fifo_stts, 0, sizeof(L2FE_L2E_SW_AGING_REPORT_FIFO_STTS_t));
    memset((void *)&cpt_3, 0, sizeof(L2FE_L2E_SW_AGING_CPT_ENTRY_3_t));
    memset((void *)&cpt_2, 0, sizeof(L2FE_L2E_SW_AGING_CPT_ENTRY_2_t));
    memset((void *)&cpt_1, 0, sizeof(L2FE_L2E_SW_AGING_CPT_ENTRY_1_t));
    memset((void *)&cpt_0, 0, sizeof(L2FE_L2E_SW_AGING_CPT_ENTRY_0_t));
    */

    while(cnt++ < 8){
        fifo_stts.wrd =  CA_NE_REG_READ(L2FE_L2E_SW_AGING_REPORT_FIFO_STTS);

        if(!fifo_stts.bf.cpt_entry_flg){
            ret = CA_E_NOT_FOUND;
            break;
        }
        else{
            cpt_3.wrd = CA_NE_REG_READ(L2FE_L2E_SW_AGING_CPT_ENTRY_3);
            cpt_2.wrd = CA_NE_REG_READ(L2FE_L2E_SW_AGING_CPT_ENTRY_2);
            cpt_1.wrd = CA_NE_REG_READ(L2FE_L2E_SW_AGING_CPT_ENTRY_1);
            cpt_0.wrd = CA_NE_REG_READ(L2FE_L2E_SW_AGING_CPT_ENTRY_0);

            memset((void *)&cmd_return, 0, sizeof(L2FE_L2E_FDB_CMD_RETURN_t));

            fdb_data0.wrd = cpt_0.wrd;
            fdb_data1.wrd = cpt_1.wrd;
            fdb_data2.wrd = cpt_2.wrd;
            fdb_data3.wrd = cpt_3.wrd;

            ret = __aal_fdb_access(device_id, __FDB_TBL_OPCODE_READ,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   (ca_uint32 *)&fdb_data3,
                                   (ca_uint32 *)&fdb_data2,
                                   (ca_uint32 *)&fdb_data1,
                                   (ca_uint32 *)&fdb_data0,
                                   (ca_uint32 *)&cmd_return);

            if (ret != CA_E_OK) {
                L2_DEBUG("access fdb fail\r\n");
                return ret;
            }

            if (cmd_return.bf.status != CA_AAL_FDB_ACCESS_HIT) {
                L2_DEBUG("fdb no find entry with, probably the entry is deleted by sw\r\n");
                ret = CA_E_NOT_FOUND;
                continue;
            }

            __aal_fdb_data_2_mac(&fdb_data3, &fdb_data2, &fdb_data1, fdb_entry->mac);
            __aal_fdb_data_2_entry(fdb_data1, fdb_data0, fdb_entry);
            L2_DEBUG("found!\r\n");
            ret = CA_E_OK;
        }
    }
    return ret;
}
#else//sd1 uboot for 98f
#endif//sd1 uboot for 98f

ca_status_t aal_fdb_cmp_cfg_load(aal_fdb_flush_cfg_t *cfg)
{
    CA_ASSERT_RET(cfg != NULL , CA_E_PARAM);

    L2_DEBUG("\r\n");

    cfg->key_sc_ind = 0;
    cfg->mc_flag = 0;
    cfg->static_flag = 0;
    cfg->sa_permit = 1;
    cfg->da_permit = 1;
    cfg->in_refresh_flag = 1;
    cfg->in_st_mv_flag = 1;
    cfg->tmp_flag = 1;

    cfg->key_vid_high = 0xfff;
    cfg->key_vid_low = 0;

    cfg->lrned_vid_high = 0xfff;
    cfg->lrned_vid_low = 0;

    cfg->dot1p_high = 0x7;
    cfg->dot1p_low = 0;

    cfg->aging_status_high = 0xff;
    cfg->aging_status_low = 0;

    cfg->mcgid_high = 0x3ff;
    cfg->mcgid_low = 0;

    cfg->lpid_high = 0x7f;
    cfg->lpid_low = 0;


    return CA_E_OK;
}

ca_status_t aal_fdb_scan_flush_field_set(
    CA_IN ca_device_id_t device_id,
    CA_IN aal_fdb_cpu_cmd_word4_msk_t mask,
    CA_IN aal_fdb_flush_cfg_t *cfg
)
{
    aal_fdb_cpu_cmd_word4_msk_t *word4_p = &mask;
    aal_fdb_cpu_cmd_word3_t word3;
    aal_fdb_cpu_cmd_word2_t word2;
    aal_fdb_cpu_cmd_word1_t word1;
    aal_fdb_cpu_cmd_word0_t word0;

    memset((void *)&word3, 0, sizeof(aal_fdb_cpu_cmd_word3_t));
    memset((void *)&word2, 0, sizeof(aal_fdb_cpu_cmd_word2_t));
    memset((void *)&word1, 0, sizeof(aal_fdb_cpu_cmd_word1_t));
    memset((void *)&word0, 0, sizeof(aal_fdb_cpu_cmd_word0_t));

    /* here, we only care valid entry */
    word4_p->bf.invalid_entry = 1;

    if (!word4_p->bf.key_sc_ind) {
        word3.bf.key_sc_ind = cfg->key_sc_ind;
    }

    if (!word4_p->bf.mc_flag) {
        word3.bf.mc_flag = cfg->mc_flag;
    }


    if (!word4_p->bf.sa_permit) {
        word3.bf.sa_permit = cfg->sa_permit;
    }

    if (!word4_p->bf.da_permit) {
        word3.bf.da_permit = cfg->da_permit;
    }

    if (!word4_p->bf.static_flag) {
        word3.bf.static_flag = cfg->static_flag;
    }

    if (!word4_p->bf.in_refresh_flag) {
        word3.bf.in_refresh_flag = cfg->in_refresh_flag;
    }

    if (!word4_p->bf.in_st_mv_flag) {
        word3.bf.in_st_mv_flag = cfg->in_st_mv_flag;
    }

    if (!word4_p->bf.tmp_flag) {
        word3.bf.tmp_flag = cfg->tmp_flag;
    }

    if (!word4_p->bf.key_vid) {
        word3.bf.key_vid_high = cfg->key_vid_high;
        word2.bf.key_vid_low = cfg->key_vid_low;
    }

    if (!word4_p->bf.lrned_vid) {
        word3.bf.lrned_vid_high = cfg->lrned_vid_high;
        word2.bf.lrned_vid_low = cfg->lrned_vid_low;
    }

    if (!word4_p->bf.key_dot1p) {
        word1.bf.key_dot1p_high = cfg->dot1p_high;
        word0.bf.key_dot1p_low = cfg->dot1p_low;
    }

    if (!word4_p->bf.aging_status) {
        word1.bf.aging_status_high = cfg->aging_status_high;
        word0.bf.aging_status_low = cfg->aging_status_low;
    }

    if (!word4_p->bf.mcgid) {
        word1.bf.mcgid_high = cfg->mcgid_high;
        word0.bf.mcgid_low = cfg->mcgid_low;
    }

    if (!word4_p->bf.lpid) {
        word1.bf.lpid_high = cfg->lpid_high;
        word0.bf.lpid_low = cfg->lpid_low;
    }

    if (device_id == 0){
        FDB_REG_WRITE(L2FE_L2E_FDB_CPU_CMD_WORD0, *(ca_uint32_t*)&word0);
        FDB_REG_WRITE(L2FE_L2E_FDB_CPU_CMD_WORD1, *(ca_uint32_t*)&word1);
        FDB_REG_WRITE(L2FE_L2E_FDB_CPU_CMD_WORD2, *(ca_uint32_t*)&word2);
        FDB_REG_WRITE(L2FE_L2E_FDB_CPU_CMD_WORD3, *(ca_uint32_t*)&word3);
        FDB_REG_WRITE(L2FE_L2E_FDB_CPU_CMD_WORD4, *(ca_uint32_t*)word4_p);
    }
#ifdef CONFIG_ARCH_CORTINA_G3HGU
    else if (device_id == 1){
        CA_8279_NE_REG_WRITE(*(ca_uint32_t*)&word0, L2FE_L2E_FDB_CPU_CMD_WORD0);
        CA_8279_NE_REG_WRITE(*(ca_uint32_t*)&word1, L2FE_L2E_FDB_CPU_CMD_WORD1);
        CA_8279_NE_REG_WRITE(*(ca_uint32_t*)&word2, L2FE_L2E_FDB_CPU_CMD_WORD2);
        CA_8279_NE_REG_WRITE(*(ca_uint32_t*)&word3, L2FE_L2E_FDB_CPU_CMD_WORD3);
        CA_8279_NE_REG_WRITE(*(ca_uint32_t*)word4_p, L2FE_L2E_FDB_CPU_CMD_WORD4);
    }
#endif

    L2_DEBUG("word0/1/2/3/4: 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x\r\n",
            *(ca_uint32_t*)&word0, *(ca_uint32_t*)&word1, *(ca_uint32_t*)&word2,
            *(ca_uint32_t*)&word3, *(ca_uint32_t*)word4_p);

    return CA_E_OK;
}

#if 0//yocto
ca_status_t aal_fdb_scan_flush_field_get(
    CA_IN ca_device_id_t device_id,
    CA_IN aal_fdb_flush_cfg_t *cfg
)
{
    aal_fdb_cpu_cmd_word4_msk_t word4;
    aal_fdb_cpu_cmd_word3_t word3;
    aal_fdb_cpu_cmd_word2_t word2;
    aal_fdb_cpu_cmd_word1_t word1;
    aal_fdb_cpu_cmd_word0_t word0;
    ca_uint32_t data;

    if (device_id == 0){
        data = CA_NE_REG_READ(L2FE_L2E_FDB_CPU_CMD_WORD0);
        word0 = *(aal_fdb_cpu_cmd_word0_t *)&data;

        data = CA_NE_REG_READ(L2FE_L2E_FDB_CPU_CMD_WORD1);
        word1 = *(aal_fdb_cpu_cmd_word1_t *)&data;

        data = CA_NE_REG_READ(L2FE_L2E_FDB_CPU_CMD_WORD2);
        word2 = *(aal_fdb_cpu_cmd_word2_t *)&data;

        data = CA_NE_REG_READ(L2FE_L2E_FDB_CPU_CMD_WORD3);
        word3 = *(aal_fdb_cpu_cmd_word3_t *)&data;

        data = CA_NE_REG_READ(L2FE_L2E_FDB_CPU_CMD_WORD4);
        word4 = *(aal_fdb_cpu_cmd_word4_msk_t *)&data;
    }
#ifdef CONFIG_ARCH_CORTINA_G3HGU
    else if (device_id == 1){
        data = CA_8279_NE_REG_READ(L2FE_L2E_FDB_CPU_CMD_WORD0);
        word0 = *(aal_fdb_cpu_cmd_word0_t *)&data;

        data = CA_8279_NE_REG_READ(L2FE_L2E_FDB_CPU_CMD_WORD1);
        word1 = *(aal_fdb_cpu_cmd_word1_t *)&data;

        data = CA_8279_NE_REG_READ(L2FE_L2E_FDB_CPU_CMD_WORD2);
        word2 = *(aal_fdb_cpu_cmd_word2_t *)&data;

        data = CA_8279_NE_REG_READ(L2FE_L2E_FDB_CPU_CMD_WORD3);
        word3 = *(aal_fdb_cpu_cmd_word3_t *)&data;

        data = CA_8279_NE_REG_READ(L2FE_L2E_FDB_CPU_CMD_WORD4);
        word4 = *(aal_fdb_cpu_cmd_word4_msk_t *)&data;
    }
#endif

    ca_printf("mask key_sc_ind = %d\r\n", word4.bf.key_sc_ind);
    ca_printf("     key_sc_ind : %d\r\n", word3.bf.key_sc_ind);

    ca_printf("mask mc_flag = %d\r\n", word4.bf.mc_flag);
    ca_printf("     mc_flag : %d\r\n", word3.bf.mc_flag);

    ca_printf("mask sa_permit = %d\r\n", word4.bf.sa_permit);
    ca_printf("     sa_permit : %d\r\n", word3.bf.sa_permit);

    ca_printf("mask da_permit = %d\r\n", word4.bf.da_permit);
    ca_printf("     da_permit : %d\r\n", word3.bf.da_permit);

    ca_printf("mask static_flag = %d\r\n", word4.bf.static_flag);
    ca_printf("     static_flag : %d\r\n", word3.bf.static_flag);

    ca_printf("mask in_refresh_flag = %d\r\n", word4.bf.in_refresh_flag);
    ca_printf("     in_refresh_flag : %d\r\n", word3.bf.in_refresh_flag);

    ca_printf("mask in_st_mv_flag = %d\r\n", word4.bf.in_st_mv_flag);
    ca_printf("     in_st_mv_flag : %d\r\n", word3.bf.in_st_mv_flag);

    ca_printf("mask tmp_flag = %d\r\n", word4.bf.tmp_flag);
    ca_printf("     tmp_flag : %d\r\n", word3.bf.tmp_flag);

    ca_printf("mask key_vid = %d\r\n", word4.bf.key_vid);
    ca_printf("     key_vid_high: %d\r\n", word3.bf.key_vid_high);
    ca_printf("     key_vid_low : %d\r\n", word2.bf.key_vid_low);

    ca_printf("mask lrned_vid = %d\r\n", word4.bf.lrned_vid);
    ca_printf("     lrned_vid_high: %d\r\n", word3.bf.lrned_vid_high);
    ca_printf("     lrned_vid_low : %d\r\n", word2.bf.lrned_vid_low);

    ca_printf("mask key_dot1p = %d\r\n", word4.bf.key_dot1p);
    ca_printf("     key_dot1p_high: %d\r\n", word1.bf.key_dot1p_high);
    ca_printf("     key_dot1p_low : %d\r\n", word0.bf.key_dot1p_low);

    ca_printf("mask aging_status = %d\r\n", word4.bf.aging_status);
    ca_printf("     aging_status_high: %d\r\n", word1.bf.aging_status_high);
    ca_printf("     aging_status_low : %d\r\n", word0.bf.aging_status_low);

    ca_printf("mask mcgid = %d\r\n", word4.bf.mcgid);
    ca_printf("     mcgid_high: %d\r\n", word1.bf.mcgid_high);
    ca_printf("     mcgid_low : %d\r\n", word0.bf.mcgid_low);

    ca_printf("mask lpid = %d\r\n", word4.bf.lpid);
    ca_printf("     lpid_high: %d\r\n", word1.bf.lpid_high);
    ca_printf("     lpid_low : %d\r\n", word0.bf.lpid_low);


    return CA_E_OK;
}
#else//sd1 uboot for 98f
#endif//sd1 uboot for 98f

ca_status_t aal_fdb_scan_flush_field_init(CA_IN ca_device_id_t device_id)
{
    aal_fdb_flush_cfg_t cfg;
    aal_fdb_cpu_cmd_word4_msk_t  mask;

    memset((void *)&cfg, 0, sizeof(aal_fdb_flush_cfg_t));

    aal_fdb_cmp_cfg_load(&cfg);

    mask.wrd = 0xffffffff;
    /* only join compare lpid */
    mask.bf.lpid = 0;

    aal_fdb_scan_flush_field_set(0, mask, &cfg);

    return CA_E_OK;
}

#define AAL_FDB_TABLE_SIZE          4096
#define AAL_FDB_OVERFLOW_TABLE_SIZE 32
#define AAL_FDB_MAX_NUM  (AAL_FDB_TABLE_SIZE + AAL_FDB_OVERFLOW_TABLE_SIZE)

ca_status_t aal_fdb_scan(
    CA_IN  ca_device_id_t          device_id,
    CA_IN  ca_uint32_t          start_offset,
    CA_OUT ca_uint32_t         *next_offset,
    CA_OUT aal_fdb_entry_cfg_t *fdb_entry
)
{
    L2FE_L2E_FDB_DATA0_t fdb_data0;
    L2FE_L2E_FDB_DATA1_t fdb_data1;
    L2FE_L2E_FDB_DATA2_t fdb_data2;
    L2FE_L2E_FDB_DATA3_t fdb_data3;
    ca_uint32_t          word0 = start_offset;
    ca_status_t          ret = CA_E_OK;
    __l2fe_l2e_fdb_scan_return_t be_ret;

#if 0//yocto
    AAL_ASSERT_RET(fdb_entry != NULL &&
        next_offset != NULL &&
        start_offset < AAL_FDB_MAX_NUM,
        CA_E_PARAM);
#else//sd1 uboot for 98f - debug
if ( !( fdb_entry != NULL &&
        next_offset != NULL &&
        start_offset < AAL_FDB_MAX_NUM ) ){
        printf("%s(%d) fdb_entry: %#x\n", __func__, __LINE__, fdb_entry);
        printf("%s(%d) next_offset: %#x\n", __func__, __LINE__, next_offset);
        printf("%s(%d) start_offset: %#x\n", __func__, __LINE__, start_offset);
}
#endif//sd1 uboot for 98f - debug

    memset((void *)&be_ret, 0, sizeof(__l2fe_l2e_fdb_scan_return_t));
    //memset((void *)&word0, 0, sizeof(aal_fdb_scan_t));
    //memset((void *)&fdb_data0, 0, sizeof(L2FE_L2E_FDB_DATA0_t));
    //memset((void *)&fdb_data1, 0, sizeof(L2FE_L2E_FDB_DATA1_t));
    //memset((void *)&fdb_data2, 0, sizeof(L2FE_L2E_FDB_DATA2_t));
    //memset((void *)&fdb_data3, 0, sizeof(L2FE_L2E_FDB_DATA3_t));

    fdb_data3.wrd = 0;
    fdb_data2.wrd = 0;
    fdb_data1.wrd = 0;
    fdb_data0.wrd = 0;


    //__FDB_LOCK();

    ret = __aal_fdb_access(device_id, __FDB_TBL_OPCODE_SCAN,
                           NULL,
                           NULL,
                           NULL,
                           NULL,
                           (ca_uint32 *)&word0,
                           (ca_uint32 *)&fdb_data3,
                           (ca_uint32 *)&fdb_data2,
                           (ca_uint32 *)&fdb_data1,
                           (ca_uint32 *)&fdb_data0,
                           (ca_uint32 *)&be_ret);
    //__FDB_UNLOCK();

    if (ret != CA_E_OK) {
        L2_DEBUG("ret:%d\r\n", ret);
        return ret;
    }

    if (be_ret.status != CA_AAL_FDB_ACCESS_HIT) {
        L2_DEBUG("fdb not hit\r\n");
        return CA_AAL_FDB_SCAN_RETURN_END;
    }

    *next_offset            = be_ret.ret_address + 1;

    __aal_fdb_data_2_entry(fdb_data1, fdb_data0, fdb_entry);
    __aal_fdb_data_2_mac(&fdb_data3, &fdb_data2, &fdb_data1, fdb_entry->mac);
#if 0
    if (fdb_entry->mc_flag || (fdb_entry->valid && fdb_entry->aging_sts)) {
        L2_DEBUG("SCAN FDB %02X:%02X:%02X:%02X:%02X:%02X VID (%u) offset (%u)\n",
            fdb_entry->mac[0], fdb_entry->mac[1], fdb_entry->mac[2],
            fdb_entry->mac[3], fdb_entry->mac[4], fdb_entry->mac[5],
            fdb_entry->lrnt_vid,
            be_ret.ret_address
            );
    }
#endif
    return ret;

}

ca_status_t aal_fdb_entry_flush(
    CA_IN ca_device_id_t                  device_id
)
{
    L2FE_L2E_FDB_CMD_RETURN_t cmd_return;
    ca_status_t                ret;

    memset((void *)&cmd_return, 0, sizeof(L2FE_L2E_FDB_CMD_RETURN_t));

    L2_DEBUG("please make sure call function aal_fdb_scan_flush_field_set to set word0--word4\r\n");

    ret = __aal_fdb_access(device_id, __FDB_TBL_OPCODE_FLUSH,
                           NULL,
                           NULL,
                           NULL,
                           NULL,
                           NULL,
                           NULL,
                           NULL,
                           NULL,
                           NULL,
                           (ca_uint32 *)&cmd_return);


    if (cmd_return.bf.status != CA_AAL_FDB_ACCESS_HIT) {
        L2_DEBUG("fdb no hit, no any entry is flushed.\r\n");
        //return CA_E_ERROR;
    }

    return CA_E_OK;
}

/*
 */

#if 0//yocto
ca_status_t aal_fdb_entry_flush_cmd(
    CA_IN ca_device_id_t              device_id,
    CA_IN __fdb_tbl_opcode_t       op_cmd
)
{
    L2FE_L2E_FDB_CMD_RETURN_t cmd_return;
    ca_status_t                ret;

    AAL_ASSERT_RET(op_cmd == __FDB_TBL_OPCODE_FLUSH || op_cmd == __FDB_TBL_OPCODE_FLUSH_OLDEST, CA_E_PARAM);

    memset((void *)&cmd_return, 0, sizeof(L2FE_L2E_FDB_CMD_RETURN_t));

    L2_DEBUG("please make sure call function aal_fdb_scan_flush_field_set to set word0--word4\r\n");
    L2_DEBUG("op_cmd:0x%x\r\n", op_cmd);

    ret = __aal_fdb_access(device_id, op_cmd,
                           NULL,
                           NULL,
                           NULL,
                           NULL,
                           NULL,
                           NULL,
                           NULL,
                           NULL,
                           NULL,
                           (ca_uint32 *)&cmd_return);


    if (cmd_return.bf.status != CA_AAL_FDB_ACCESS_HIT) {
        L2_DEBUG("fdb no hit\r\n");
        return CA_E_ERROR;
    }

    return CA_E_OK;
}


ca_status_t aal_fdb_entry_scan(
    CA_IN ca_device_id_t             device_id,
    CA_IN ca_uint32               start_addr,
    CA_OUT ca_uint32              *hit_addr,
    CA_OUT aal_fdb_entry_cfg_t    *fdb_entry,
    CA_OUT ca_uint32              *cmd_return
)
{
    L2FE_L2E_FDB_DATA0_t fdb_data0;
    L2FE_L2E_FDB_DATA1_t fdb_data1;
    L2FE_L2E_FDB_DATA2_t fdb_data2;
    L2FE_L2E_FDB_DATA3_t fdb_data3;
    //aal_fdb_scan_t  word0;
    ca_uint32_t word0;

    __l2fe_l2e_fdb_scan_return_t be_ret;

    ca_status_t           ret;

    CA_ASSERT_RET(fdb_entry != NULL &&
                  cmd_return != NULL &&
                  start_addr < AAL_FDB_MAX_NUM, CA_E_PARAM);

    memset((void *)&be_ret, 0, sizeof(__l2fe_l2e_fdb_scan_return_t));
    //memset((void *)&word0, 0, sizeof(aal_fdb_scan_t));
    memset((void *)&fdb_data0, 0, sizeof(L2FE_L2E_FDB_DATA0_t));
    memset((void *)&fdb_data1, 0, sizeof(L2FE_L2E_FDB_DATA1_t));
    memset((void *)&fdb_data2, 0, sizeof(L2FE_L2E_FDB_DATA2_t));
    memset((void *)&fdb_data3, 0, sizeof(L2FE_L2E_FDB_DATA3_t));

    //word0.bf.start_addr = start_addr;
    word0 = start_addr;

//    L2_DEBUG("\r\n");

    //__FDB_LOCK();

    ret = __aal_fdb_access(device_id, __FDB_TBL_OPCODE_SCAN,
                           NULL,
                           NULL,
                           NULL,
                           NULL,
                           (ca_uint32 *)&word0,
                           (ca_uint32 *)&fdb_data3,
                           (ca_uint32 *)&fdb_data2,
                           (ca_uint32 *)&fdb_data1,
                           (ca_uint32 *)&fdb_data0,
                           (ca_uint32 *)&be_ret);

    if (ret != CA_E_OK) {
        //__FDB_UNLOCK();
        L2_DEBUG("ret:%d\r\n", ret);
        return ret;
    }

//    L2_DEBUG("\r\n");

    *cmd_return = be_ret.status;

    if (*cmd_return != CA_AAL_FDB_ACCESS_HIT) {
        //__FDB_UNLOCK();
        L2_DEBUG("fdb not hit\r\n");
        return CA_AAL_FDB_SCAN_RETURN_END;
    }
    else {
        *hit_addr = be_ret.ret_address;

        __aal_fdb_data_2_mac(&fdb_data3, &fdb_data2, &fdb_data1, fdb_entry->mac);
        __aal_fdb_data_2_entry(fdb_data1, fdb_data0, fdb_entry);

    }

    //__FDB_UNLOCK();

    return ret;
}

aal_fdb_entry_cfg_t *aal_fdb_static_getfirst(ca_table_t *table_p)
{
    aal_fdb_entry_cfg_t *tmp_p = NULL;

    if (table_p == NULL)
        return NULL;

    tmp_p = (aal_fdb_entry_cfg_t *) ca_table_entry_getfirst(
                table_p);

    return tmp_p;
}

aal_fdb_entry_cfg_t *aal_fdb_static_getnext(ca_table_t *table_p)
{
    aal_fdb_entry_cfg_t *tmp_p = NULL;

    if (table_p == NULL)
        return NULL;

    tmp_p = (aal_fdb_entry_cfg_t *) ca_table_entry_getnext(
                table_p);

    return tmp_p;
}

ca_status_t aal_fdb_port_limit_init(
    CA_IN ca_device_id_t device_id,
    CA_IN ca_int32    port_id
)
{
    ca_uint32               cmd_return = 0;
    ca_status_t             ret;

    /* port_id is lpid */
    CA_AAL_FDB_PORT_CHECK(port_id);

    __FDB_LOCK();
    ret = __aal_port_mac_lmt(device_id,
                             port_id,
                             __FDB_TBL_OPCODE_LMT_INIT,
                             NULL,
                             &cmd_return);
    __FDB_UNLOCK();

    if (cmd_return != CA_AAL_L3E_CNT_LMT_SUCCESS) {
        L2_DEBUG("cmd_return:%d\r\n", cmd_return);
        return CA_E_ERROR;
    }

    return CA_E_OK;
}
#else//sd1 uboot for 98f
#endif//sd1 uboot for 98f

ca_status_t aal_fdb_port_limit_set(
    CA_IN ca_device_id_t device_id,
    CA_IN ca_int32    port_id,  /* lpid */
    CA_IN ca_uint32   addr_limit
)
{
    L2FE_L2E_PORT_MAC_CNT_LMT_DATA_t data;
    ca_uint32_t             current_limit = 0;
    ca_uint32               cmd_return;
    ca_status_t             ret;
    L2FE_L2E_PORT_MAC_CNT_LMT_DATA_t max_data;

    max_data.wrd = ~0U;

    /* keep max value if exceed */
    if (addr_limit > max_data.bf.val){
        L2_DEBUG("exceed port limit:%d\r\n", max_data.bf.val);
        return CA_E_PARAM;
    }

    CA_AAL_FDB_PORT_CHECK(port_id);

    memset((void *)&data, 0, sizeof(L2FE_L2E_PORT_MAC_CNT_LMT_DATA_t));

#if 0//yocto
    __FDB_LOCK();
#else//sd1 uboot for 98f
#endif//sd1 uboot for 98f

    ret = __aal_port_mac_lmt(device_id,
                             port_id,
                             __FDB_TBL_OPCODE_LMT_RD,
                             (ca_uint32 *)&data,
                             &cmd_return);
    current_limit = data.bf.val;

#if 0//yocto
    __FDB_UNLOCK();
#else//sd1 uboot for 98f
#endif//sd1 uboot for 98f

    if (addr_limit < current_limit){
        /* clear current port fdb table */
        ret = __aal_fdb_addr_delete_by_port(device_id, port_id, CA_L2_ADDR_OP_DYNAMIC);
    }

#if 0//yocto
    __FDB_LOCK();
#else//sd1 uboot for 98f
#endif//sd1 uboot for 98f

    /* set limit */
    data.bf.val = addr_limit;

    ret = __aal_port_mac_lmt(device_id,
                             port_id,
                             __FDB_TBL_OPCODE_LMT_WR,
                             (ca_uint32 *)&data,
                             &cmd_return);

#if 0//yocto
    __FDB_UNLOCK();
#else//sd1 uboot for 98f
#endif//sd1 uboot for 98f

    if (cmd_return != CA_AAL_L3E_CNT_LMT_SUCCESS) {
        L2_DEBUG("cmd_return:%d\r\n", cmd_return);
        return CA_E_ERROR;
    }

    return CA_E_OK;
}

#if 0//yocto
ca_status_t aal_fdb_port_limit_get(
    CA_IN ca_device_id_t device_id,
    CA_IN ca_int32    port_id,
    CA_OUT ca_uint32  *addr_limit
)
{
    L2FE_L2E_PORT_MAC_CNT_LMT_DATA_t data;

    ca_uint32               cmd_return;
    ca_status_t             ret;

    CA_ASSERT_RET(addr_limit != NULL, CA_E_PARAM);

    CA_AAL_FDB_PORT_CHECK(port_id);
    memset((void *)&data, 0, sizeof(L2FE_L2E_PORT_MAC_CNT_LMT_DATA_t));

    __FDB_LOCK();

    /* set limit */
    ret = __aal_port_mac_lmt(device_id,
                             port_id,
                             __FDB_TBL_OPCODE_LMT_RD,
                             (ca_uint32 *)&data,
                             &cmd_return);

    *addr_limit = data.bf.val;

    __FDB_UNLOCK();

    if (cmd_return != CA_AAL_L3E_CNT_LMT_SUCCESS) {
        L2_DEBUG("cmd_ret:%d\r\n", cmd_return);
        return CA_E_ERROR;
    }

    return CA_E_OK;
}

ca_status_t aal_fdb_port_limit_inc(
    CA_IN ca_device_id_t device_id,
    CA_IN ca_int32    port_id
)
{
    ca_uint32               cmd_return;
    ca_status_t             ret;

    CA_AAL_FDB_PORT_CHECK(port_id);

    __FDB_LOCK();

    /* set limit */
    ret = __aal_port_mac_lmt(device_id,
                             port_id,
                             __FDB_TBL_OPCODE_LMT_INC,
                             NULL,
                             &cmd_return);
    __FDB_UNLOCK();

    if (cmd_return != CA_AAL_L3E_CNT_LMT_SUCCESS) {
        L2_DEBUG("cmd_ret:%d\r\n", cmd_return);
        return CA_E_ERROR;
    }

    return CA_E_OK;
}


ca_status_t aal_fdb_port_limit_dec(
    CA_IN ca_device_id_t device_id,
    CA_IN ca_int32    port_id
)
{
    ca_uint32               cmd_return;
    ca_status_t             ret;

    CA_AAL_FDB_PORT_CHECK(port_id);

    __FDB_LOCK();

    /* set limit */
    ret = __aal_port_mac_lmt(device_id,
                             port_id,
                             __FDB_TBL_OPCODE_LMT_DEC,
                             NULL,
                             &cmd_return);

    __FDB_UNLOCK();

    if (cmd_return != CA_AAL_L3E_CNT_LMT_SUCCESS) {
        L2_DEBUG("cmd_ret:%d\r\n", cmd_return);
        return CA_E_ERROR;
    }

    return CA_E_OK;
}


ca_status_t aal_fdb_port_cnt_init(
    CA_IN ca_device_id_t device_id,
    CA_IN ca_int32    port_id
)
{
    ca_uint32               cmd_return;
    ca_status_t             ret;

    CA_AAL_FDB_PORT_CHECK(port_id);

    __FDB_LOCK();

    ret = __aal_port_mac_cnt(device_id,
                             port_id,
                             __FDB_TBL_OPCODE_CNT_INIT,
                             NULL,
                             &cmd_return);

    __FDB_UNLOCK();

    if (cmd_return != CA_AAL_L3E_CNT_LMT_SUCCESS) {
        L2_DEBUG("cmd_ret:%d\r\n", cmd_return);
        return CA_E_ERROR;
    }

    return CA_E_OK;
}


ca_status_t aal_fdb_port_cnt_set(
    CA_IN ca_device_id_t     device_id,
    CA_IN ca_int32        port_id,
    CA_IN ca_uint32       addr_cnt
)
{
    L2FE_L2E_PORT_MAC_CNT_LMT_DATA_t data;

    ca_uint32               cmd_return;
    ca_status_t             ret;

    CA_AAL_FDB_PORT_CHECK(port_id);

    memset((void *)&data, 0, sizeof(L2FE_L2E_PORT_MAC_CNT_LMT_DATA_t));

    __FDB_LOCK();

    data.bf.val = addr_cnt;

    ret = __aal_port_mac_cnt(device_id,
                             port_id,
                             __FDB_TBL_OPCODE_CNT_WR,
                             (ca_uint32 *)&data,
                             &cmd_return);

    __FDB_UNLOCK();

    if (cmd_return != CA_AAL_L3E_CNT_LMT_SUCCESS) {
        L2_DEBUG("cmd_ret:%d\r\n", cmd_return);
        return CA_E_ERROR;
    }

    return CA_E_OK;
}


ca_status_t aal_fdb_port_cnt_get(
    CA_IN ca_device_id_t     device_id,
    CA_IN ca_int32        port_id,
    CA_OUT ca_uint32      *addr_cnt
)
{
    L2FE_L2E_PORT_MAC_CNT_LMT_DATA_t data;

    ca_uint32               cmd_return;
    ca_status_t             ret;

    CA_ASSERT_RET(addr_cnt != NULL, CA_E_PARAM);

    CA_AAL_FDB_PORT_CHECK(port_id);

    memset((void *)&data, 0, sizeof(L2FE_L2E_PORT_MAC_CNT_LMT_DATA_t));
    __FDB_LOCK();

    ret = __aal_port_mac_cnt(device_id,
                             port_id,
                             __FDB_TBL_OPCODE_CNT_RD,
                             (ca_uint32 *)&data,
                             &cmd_return);

    *addr_cnt = data.bf.val;

    __FDB_UNLOCK();

    if (cmd_return != CA_AAL_L3E_CNT_LMT_SUCCESS) {
        L2_DEBUG("cmd_return:%d\r\n", cmd_return);
        return CA_E_ERROR;
    }

    return CA_E_OK;
}

ca_status_t aal_fdb_port_cnt_inc(
    CA_IN ca_device_id_t     device_id,
    CA_IN ca_int32        port_id
)
{
    ca_uint32               cmd_return;
    ca_status_t             ret;

    CA_AAL_FDB_PORT_CHECK(port_id);

    __FDB_LOCK();

    ret = __aal_port_mac_cnt(device_id,
                             port_id,
                             __FDB_TBL_OPCODE_CNT_INC,
                             NULL,
                             &cmd_return);

    __FDB_UNLOCK();

    if (cmd_return != CA_AAL_L3E_CNT_LMT_SUCCESS) {
        L2_DEBUG("cmd_ret:%d\r\n", cmd_return);
        return CA_E_ERROR;
    }

    return CA_E_OK;
}

ca_status_t aal_fdb_port_cnt_dec(
    CA_IN ca_device_id_t     device_id,
    CA_IN ca_int32        port_id
)
{
    ca_uint32               cmd_return;
    ca_status_t             ret;

    CA_AAL_FDB_PORT_CHECK(port_id);

    __FDB_LOCK();

    ret = __aal_port_mac_cnt(device_id,
                             port_id,
                             __FDB_TBL_OPCODE_CNT_DEC,
                             NULL,
                             &cmd_return);

    __FDB_UNLOCK();

    if (cmd_return != CA_AAL_L3E_CNT_LMT_SUCCESS) {
        L2_DEBUG("cmd_ret:%d\r\n", cmd_return);
        return CA_E_ERROR;
    }

    return CA_E_OK;
}
#else//sd1 uboot for 98f
#endif//sd1 uboot for 98f

ca_status_t aal_port_flooding_init(CA_IN ca_device_id_t device_id)
{
    aal_port_flooding_t *f_p = &flood_cb;
//    aal_l2_te_policer_cfg_msk_t  msk;
//    aal_l2_te_policer_cfg_t      cfg;
    ca_status_t ret;
    ca_int32_t i, j;

    memset(f_p, 0x0, sizeof(aal_port_flooding_t));

    for (i = 0; i < AAL_L2TM_L2TE_POL_PKT_TYPE_PROFILE_NUM; i++){
        f_p->profile[i].rate = CA_UINT32_INVALID;
    }

    /*
    for (i = 1; i < AAL_ILPB_UNKWN_POL_NUM; i++){
        for (j = 0; j <= CA_L2_FLOODING_TYPE_UUC; j++){
            f_p->lspid_type_prof[i][j].profile_id = CA_UINT8_INVALID;
        }
    }
    */
    /* set L2FE_ARB_NON_KNOWN_POL_MAP_TBL_DATA to default profile */
    for (i = 0; i < AAL_ILPB_UNKWN_POL_NUM; i++){
        for (j = 0; j < AAL_ARB_PKT_FWD_TYPE_MAX; j++){
            f_p->lspid_type_prof[i][j].profile_id = CA_AAL_FDB_PORT_FLOOD_DEF_PROFILE;
            ret = aal_arb_non_known_pol_map_set(device_id, i, j, CA_AAL_FDB_PORT_FLOOD_DEF_PROFILE);

            if (ret != CA_E_OK){
                ca_printf("aal_port_flooding_init, aal_arb_non_known_pol_map_set FAIL!\n");
                return CA_E_ERROR;
            }
        }
    }

    /* set packet mode to pps */
    /*memset(&msk, 0, sizeof(aal_l2_te_policer_cfg_msk_t));
    memset(&cfg, 0, sizeof(aal_l2_te_policer_cfg_t));

    msk.s.pol_update_mode = 1;
    cfg.pol_update_mode = CA_AAL_POLICER_UPDATE_MODE_PKT;

    for (i = 0; i < AAL_L2TM_L2TE_POL_PKT_TYPE_PROFILE_NUM; i++){
        ret = aal_l2_te_policer_pkt_type_cfg_set(device_id, i, msk, &cfg);
        if (ret != CA_E_OK){
            ca_printf("aal_port_flooding_init FAIL!\n");
            return CA_E_ERROR;
        }
    }
    */
    return CA_E_OK;
}

#if 0//yocto
ca_int32_t aal_port_ilpb_unknw_pol_alloc(
    CA_IN ca_device_id_t device_id,
    CA_IN ca_int32_t  lspid,
    CA_IN ca_uint8_t  pkt_type,
    CA_IN ca_uint8_t  profile
)
{
    aal_port_flooding_t *f_p = &flood_cb;
    ca_int32_t policer[AAL_ILPB_UNKWN_POL_NUM] = {0};
    int i;
    ca_int32_t available_idx, used_idx;

    CA_ASSERT_RET(lspid < AAL_PORT_NUM_LOGICAL, -1);

    available_idx = used_idx = -1;

    for (i = 0; i < AAL_PORT_NUM_LOGICAL; i++){
        if (f_p->lspid_policer[i] != 0 && f_p->lspid_policer[i] < AAL_ILPB_UNKWN_POL_NUM)
            if (i == lspid)
                used_idx = f_p->lspid_policer[lspid];

            policer[f_p->lspid_policer[i]] = 1;
    }

    for (i = 1; i < AAL_ILPB_UNKWN_POL_NUM; i++){
        if (policer[i] == 0){
            available_idx = i;
            break;
        }
    }

    if (used_idx != -1)
        return used_idx;

    if (available_idx != -1)
        return available_idx;

    return -1;
}


ca_int32_t aal_port_pkt_type_profile_alloc(
    CA_IN ca_device_id_t device_id,
    CA_IN ca_uint32_t rate
)
{
    ca_int32_t i, available_idx, used_idx;
    aal_port_flooding_t *f_p = &flood_cb;
    //arb_non_known_pol_t *pol_p = NULL;
    l2te_pol_profile_t *profile_p = NULL;

    used_idx = available_idx = -1;

    for (i = 0; i < AAL_L2TM_L2TE_POL_PKT_TYPE_PROFILE_NUM; i++){
        profile_p = f_p->profile + i;

        if (profile_p->rate == rate && used_idx == -1){
           /* same profile */
            used_idx = i;
        }

        if (profile_p->rate == CA_UINT32_INVALID
            && available_idx == -1)
        {
            available_idx = i;
        }
    }

    L2_DEBUG("pkt_type profile used_idx:%d, available_idx:%d\r\n", used_idx, available_idx);
    /*
    if (used_idx != -1)
        return used_idx;
    */
    if (available_idx != -1)
        return available_idx;

    return -1;

}


ca_status_t __aal_port_flooding_update(
    ca_device_id_t device_id,
    ca_int32_t lspid,
    aal_arb_pkt_fwd_type_t arb_pkt_type,
    ca_int32_t unkwn_pol_id,
    ca_int32_t profile_id,
    ca_uint32_t rate)
{
    aal_port_flooding_t *f_p = &flood_cb;
    ca_uint8_t policer_idx;
    int policer_used;
    int i;
    ca_status_t ret = CA_E_OK;
    aal_ilpb_cfg_msk_t igr_msk;
    aal_ilpb_cfg_t igr_cfg;

    L2_DEBUG("lspid:%d, arb_pkt_type:%d\r\n", lspid, arb_pkt_type);

    CA_ASSERT_RET(lspid < AAL_PORT_NUM_LOGICAL
                  && arb_pkt_type <= AAL_ARB_PKT_FWD_TYPE_MAX, CA_E_PARAM);

    /* when rate CA_UINT32_INVALID, unkwn_pol_id and profile_id is invalid */
    if (rate == CA_UINT32_INVALID){
        policer_idx = f_p->lspid_policer[lspid];
        L2_DEBUG("policer_idx:%d, pkt_type:%d\r\n", policer_idx, arb_pkt_type);
        if (policer_idx == CA_UINT8_INVALID){
            L2_DEBUG("delete invalid policer_idx!\r\n");
            return CA_E_ERROR;
        }

        //f_p->lspid_policer[lspid] = 0; /* in arb, [0][0..2]= 15, default profile */
        f_p->lspid_type_prof[policer_idx][arb_pkt_type].profile_id = CA_AAL_FDB_PORT_FLOOD_DEF_PROFILE;
        ret = aal_arb_non_known_pol_map_set(device_id, policer_idx, arb_pkt_type, CA_AAL_FDB_PORT_FLOOD_DEF_PROFILE);
        policer_used = 0;
        for (i = 0; i <= CA_L2_FLOODING_TYPE_UUC; i++){
            if (f_p->lspid_type_prof[policer_idx][i].profile_id != CA_AAL_FDB_PORT_FLOOD_DEF_PROFILE){
                policer_used = 1;
                break;
            }
        }

        if (policer_used == 0){
            /* release pol_idx in ilpb */
            f_p->lspid_policer[lspid] = 0;
            /* set to ilpb HW */
            memset(&igr_msk, 0, sizeof(aal_ilpb_cfg_msk_t));
            memset(&igr_cfg, 0, sizeof(aal_ilpb_cfg_t));

            igr_msk.s.unkwn_pol_idx = 1;
            igr_cfg.unkwn_pol_idx = 0;

            ret = aal_port_ilpb_cfg_set(device_id, lspid, igr_msk, &igr_cfg);
            L2_DEBUG("ret:%d\r\n", ret);
        }

        return CA_E_OK;
    }

    CA_ASSERT_RET(unkwn_pol_id < AAL_ILPB_UNKWN_POL_NUM
                  && profile_id < AAL_L2TM_L2TE_POL_PKT_TYPE_PROFILE_NUM, CA_E_PARAM);

    f_p->lspid_policer[lspid] = unkwn_pol_id;
    f_p->lspid_type_prof[unkwn_pol_id][arb_pkt_type].profile_id = profile_id;
    f_p->profile[profile_id].rate = rate;

    return CA_E_OK;
}

ca_int32_t __aal_port_flooding_profile_id_get(
    ca_l2_flooding_type_t pkt_type,
    ca_int32_t unkwn_pol_id)
{
    aal_port_flooding_t *f_p = &flood_cb;
    int i;

    CA_ASSERT_RET(pkt_type <= CA_L2_FLOODING_TYPE_UUC
                  && unkwn_pol_id < AAL_ILPB_UNKWN_POL_NUM
                  , CA_E_PARAM);

    i = f_p->lspid_type_prof[unkwn_pol_id][pkt_type].profile_id;

    L2_DEBUG("pkt_type:%d, pol_id:%d, profile:%d\r\n", pkt_type, unkwn_pol_id, i);

    return i;
}


ca_uint8_t __aal_port_flooding_pol_idx_get(
    ca_int32_t  lspid
)
{
    aal_port_flooding_t *f_p = &flood_cb;
    ca_uint8_t pol_id;

    CA_ASSERT_RET(lspid < AAL_PORT_NUM_LOGICAL, CA_E_PARAM);

    L2_DEBUG("lspid:%d\r\n", lspid);

    pol_id = f_p->lspid_policer[lspid];

    return pol_id;
}

aal_arb_pkt_fwd_type_t __pkt_type_convert(ca_uint8_t  pkt_type)
{
    aal_arb_pkt_fwd_type_t arb_pkt_type = AAL_ARB_PKT_FWD_TYPE_BC;

    switch (pkt_type){
        case CA_L2_FLOODING_TYPE_MC:
            arb_pkt_type = AAL_ARB_PKT_FWD_TYPE_UMC;
            break;
        case CA_L2_FLOODING_TYPE_UUC:
            arb_pkt_type = AAL_ARB_PKT_FWD_TYPE_UUC;
            break;
        case CA_L2_FLOODING_TYPE_BC:
            arb_pkt_type = AAL_ARB_PKT_FWD_TYPE_BC;
            break;
        default:
            arb_pkt_type = AAL_ARB_PKT_FWD_TYPE_BC;
            break;
    }

    return arb_pkt_type;
}

ca_status_t aal_fdb_ilpb_unknw_pol_idx_map_set(
    CA_IN ca_device_id_t device_id,
    CA_IN ca_int32_t  lspid,
    CA_IN ca_uint8_t  pkt_type,
    CA_IN ca_uint32_t rate
)
{
    aal_ilpb_cfg_msk_t igr_msk;
    aal_ilpb_cfg_t igr_cfg;
    aal_l2_te_policer_profile_msk_t profile_msk;
    aal_l2_te_policer_profile_t     profile_cfg;
    ca_int32_t unkwn_pol_id = -1, profile_id = -1;
    ca_status_t ret = CA_E_OK;
    ca_uint32_t value;
    ca_uint8_t pol_idx;
    aal_port_flooding_t *f_p = &flood_cb;
    aal_arb_pkt_fwd_type_t arb_pkt_type;

    L2_DEBUG("lspid:%d, pkt_type:%d, rate:%d\r\n", lspid, pkt_type, rate);

    CA_ASSERT_RET(lspid < AAL_PORT_NUM_LOGICAL
                  && pkt_type <= CA_L2_FLOODING_TYPE_UUC, CA_E_PARAM);

    arb_pkt_type = __pkt_type_convert(pkt_type);

    /* TODO, rate = -1, set to default profile */
    if (rate == CA_UINT32_INVALID){
        pol_idx = f_p->lspid_policer[lspid];
        L2_DEBUG("pol_idx:%d\r\n", pol_idx);
        /* update table */
        ret = __aal_port_flooding_update(device_id, lspid, arb_pkt_type, unkwn_pol_id, profile_id, rate);
#if 0
        if (ret == CA_E_OK && pol_idx < AAL_ILPB_UNKWN_POL_NUM && pol_idx > 0){
            for (i = 0; i <= CA_L2_FLOODING_TYPE_UUC; i++){
                //if (f_p->lspid_type_prof[pol_idx][i].profile != CA_UINT8_INVALID)
                f_p->lspid_type_prof[pol_idx][i].profile_id = CA_AAL_FDB_PORT_FLOOD_DEF_PROFILE;
                //ret = aal_arb_non_known_pol_map_set(device_id, lspid, i, CA_AAL_FDB_PORT_FLOOD_DEF_PROFILE);
            }
        }
#endif
        return ret;
    }

    profile_id = aal_port_pkt_type_profile_alloc(device_id, rate);

    L2_DEBUG("profile_id:%d\r\n", profile_id);
    if (-1 == profile_id){
        ca_printf("pkt type profile resource unavailable\r\n");
        return CA_E_RESOURCE;
    }

    unkwn_pol_id = aal_port_ilpb_unknw_pol_alloc(device_id, lspid, arb_pkt_type, profile_id);

    L2_DEBUG("unkwn_pol_id:%d\r\n", unkwn_pol_id);
    if (-1 == unkwn_pol_id){
        ca_printf(" ilpd resource unavailable\r\n");
        return CA_E_RESOURCE;
    }

    /* set to l2te HW */
    memset(&profile_msk, 0, sizeof(aal_l2_te_policer_profile_msk_t));
    memset(&profile_cfg, 0, sizeof(aal_l2_te_policer_profile_t));
    profile_msk.s.pir_m = 1;
    profile_msk.s.pir_k = 1;
    value = rate / 1000;
    profile_cfg.pir_m = (value > 0xffff ? 0xffff : value);
    value = rate % 1000;
    profile_cfg.pir_k = value;
    ret = aal_l2_te_policer_pkt_type_profile_set(device_id, profile_id, profile_msk, &profile_cfg);

    L2_DEBUG("ret:%d\r\n", ret);

    /* set arb */
    ret = aal_arb_non_known_pol_map_set(device_id, unkwn_pol_id, arb_pkt_type, profile_id);

    /* set to ilpb HW */
    memset(&igr_msk, 0, sizeof(aal_ilpb_cfg_msk_t));
    memset(&igr_cfg, 0, sizeof(aal_ilpb_cfg_t));

    igr_msk.s.unkwn_pol_idx = 1;
    igr_cfg.unkwn_pol_idx = unkwn_pol_id;

    ret = aal_port_ilpb_cfg_set(device_id, lspid, igr_msk, &igr_cfg);
    L2_DEBUG("set to ilpb HW, ilpb->pol_id, %d--->%d, ret:%d\r\n", lspid, unkwn_pol_id, ret);

    /* write to local table */
    ret = __aal_port_flooding_update(device_id, lspid, arb_pkt_type, unkwn_pol_id, profile_id, rate);

    return CA_E_OK;

}


ca_status_t aal_fdb_ilpb_unknw_pol_idx_map_get(
    CA_IN ca_device_id_t device_id,
    CA_IN ca_int32_t  lspid,
    CA_IN ca_uint8_t  pkt_type,
    CA_IN ca_uint32_t *rate
)
{
    aal_ilpb_cfg_t igr_cfg;
    aal_l2_te_policer_profile_t     profile_cfg;
    ca_int32_t unkwn_pol_id;
    ca_uint16_t profile_id = 0;
    ca_status_t ret = CA_E_OK;
    aal_arb_pkt_fwd_type_t arb_pkt_type;

    ca_printf("CA_L2_FLOODING_TYPE_MC  = %d\r\n", CA_L2_FLOODING_TYPE_MC);
    ca_printf("CA_L2_FLOODING_TYPE_BC  = %d\r\n", CA_L2_FLOODING_TYPE_BC);
    ca_printf("CA_L2_FLOODING_TYPE_UUC = %d\r\n", CA_L2_FLOODING_TYPE_UUC);

    arb_pkt_type = __pkt_type_convert(pkt_type);
    L2_DEBUG("lspid:%d, pkt_type:%d, arb_pkt_type:%d\r\n", lspid, pkt_type, arb_pkt_type);

    CA_ASSERT_RET(lspid < AAL_PORT_NUM_LOGICAL
                  && pkt_type <= CA_L2_FLOODING_TYPE_UUC
                  && rate != NULL, CA_E_PARAM);


    ca_printf("ARB TYPE MC_UC = %d\r\n", AAL_ARB_PKT_FWD_TYPE_MC_UC);
    ca_printf("ARB TYPE UUC = %d\r\n",   AAL_ARB_PKT_FWD_TYPE_UUC);
    ca_printf("ARB TYPE UMC = %d\r\n",   AAL_ARB_PKT_FWD_TYPE_UMC);
    ca_printf("ARB TYPE BC = %d\r\n",    AAL_ARB_PKT_FWD_TYPE_BC);

    /* get from ilpb HW */
    memset(&igr_cfg, 0, sizeof(aal_ilpb_cfg_t));

    ret = aal_port_ilpb_cfg_get(device_id, lspid, &igr_cfg);
    unkwn_pol_id = igr_cfg.unkwn_pol_idx;
    L2_DEBUG("ret:%d, unkwn_pol_idx:%d\r\n", ret, unkwn_pol_id);

    /* get from ARB */
    ret = aal_arb_non_known_pol_map_get(device_id, unkwn_pol_id, arb_pkt_type, &profile_id);
    L2_DEBUG("ret:%d, profile_id:%d\r\n", ret, profile_id);

    /*
    memset(&profile_cfg, 0, sizeof(aal_l2_te_policer_profile_t));
    profile_id = __aal_port_flooding_profile_id_get(pkt_type, unkwn_pol_id);

    if (profile_id == -1){
        L2_DEBUG("no specify profile\r\n");
        return ret;
    }
    */
    /* get from l2te HW */
    ret = aal_l2_te_policer_pkt_type_profile_get(device_id, profile_id, &profile_cfg);

    L2_DEBUG("ret:%d, pir_m:%d, pir_k:%d\r\n", ret, profile_cfg.pir_m, profile_cfg.pir_k);
    ca_printf("%s:%d, pir_m:%d/0x%x, pir_k:%d/0x%x\r\n", __FUNCTION__,__LINE__,
        profile_cfg.pir_m, profile_cfg.pir_m, profile_cfg.pir_k, profile_cfg.pir_k);

    *rate = profile_cfg.pir_m * 1000 + profile_cfg.pir_k;

    return CA_E_OK;

}

ca_status_t aal_fdb_ilpb_unknw_pol_idx_cb_get(
    CA_IN ca_device_id_t device_id
)
{
    int i, j;
    aal_port_flooding_t *f_p = &flood_cb;

    printk("CA_L2_FLOODING_TYPE_MC  = %d\r\n", CA_L2_FLOODING_TYPE_MC);
    printk("CA_L2_FLOODING_TYPE_BC  = %d\r\n", CA_L2_FLOODING_TYPE_BC);
    printk("CA_L2_FLOODING_TYPE_UUC = %d\r\n", CA_L2_FLOODING_TYPE_UUC);


    printk("ARB TYPE MC_UC = %d\r\n", AAL_ARB_PKT_FWD_TYPE_MC_UC);
    printk("ARB TYPE UUC = %d\r\n",   AAL_ARB_PKT_FWD_TYPE_UUC);
    printk("ARB TYPE UMC = %d\r\n",   AAL_ARB_PKT_FWD_TYPE_UMC);
    printk("ARB TYPE BC = %d\r\n",    AAL_ARB_PKT_FWD_TYPE_BC);

    printk("---Local CB---\r\n");
    printk("--- lpid/policer idx mapping---\r\n");
    printk("      0   1   2   3   4   5   6   7   8   9\r\n");
    printk("      ---------------------------------------\r\n");

    for(i = 0; i < AAL_PORT_NUM_LOGICAL; i++){
        if (i % 10 == 0)
            printk(" %d:", i / 10);
        printk("   %d", f_p->lspid_policer[i]);
        if (i % 10 == 9)
            printk("\r\n");
    }

    printk("\r\n");

    printk("\r\n---port type profile assignment---\r\n");
    for (i = 0; i < AAL_ILPB_UNKWN_POL_NUM; i++){
        for (j = 0; j < AAL_ARB_PKT_FWD_TYPE_MAX; j++){
            printk("lspid_type_prof[%d][%d].profile_id = %d\r\n", i, j, f_p->lspid_type_prof[i][j].profile_id);
        }
    }

    printk("\r\n---profile rate---\r\n");
    for (i = 0; i < AAL_L2TM_L2TE_POL_PKT_TYPE_PROFILE_NUM; i++){
        printk("profile[%d].rate = %d\r\n", i, f_p->profile[i].rate);
    }

    return CA_E_OK;

}

ca_status_t aal_fdb_entry_2_l2_addr(
    CA_OUT ca_l2_addr_entry_t *addr,
    CA_IN aal_fdb_entry_cfg_t *fdb
)
{
    CA_ASSERT_RET(fdb != NULL && addr != NULL, CA_E_PARAM);

    memcpy(addr->mac_addr, fdb->mac, CA_ETH_ADDR_LEN);
    addr->fwd_vid       = fdb->lrnt_vid;
    addr->vid           = fdb->key_vid;
    addr->dot1p         = fdb->dot1p;
    addr->static_flag   = fdb->static_flag;
    addr->sa_permit     = fdb->sa_permit;
    addr->da_permit     = fdb->da_permit;

    if (fdb->mc_flag){
        addr->mc_group_id = fdb->port_id;
        addr->port_id     = CA_UINT32_INVALID;
    }else{
        addr->port_id       = ca_port_id_get(fdb->port_id);
        addr->mc_group_id   = INVALID_MC_GROUP_ID;
    }
    addr->aging_timer  = FDB_AGING_TIME(fdb->aging_sts);

    return CA_E_OK;
}

ca_status_t aal_fdb_mac_filter_default_set (
    CA_IN ca_device_id_t  device_id,
    CA_IN ca_int32     port_id,
    CA_IN ca_boolean_t drop_flag
)
{
    aal_l2_cls_l2rl2r_key_t cfg;
    aal_l2_cls_l2rl2r_key_mask_t cls_mask;
    aal_l2_cls_fib_t  fib;
    aal_l2_cls_fib_mask_t  fib_mask;
    ca_uint32 rule_0, rule_1;
    ca_uint32 entry_id = CLS_ENTRY_ID(port_id, CA_AAL_FDB_MAC_FILTER_DFT_ID);
    ca_status_t ret;
    /*aal_tbl_mac_filter_entry_t *partner = NULL;
    aal_tbl_mac_filter_entry_t new_entry;
    aal_l2_cls_l2r_key_t *l2r_p = NULL;
    ca_uint32 partner_id;
    */

    CA_AAL_FDB_PORT_CHECK(port_id);
    L2_DEBUG("port_id:0x%x, entry_id:%d\r\n", port_id, entry_id);

    __FDB_LOCK();

    /* get partner id in one entry */
    /*
    partner_id = CA_AAL_RULE_ID_PARTNER(CA_AAL_FDB_MAC_FILTER_DFT_ID);

    partner = __aal_mac_filter_rule_get_by_rule(port_id, partner_id);
    */
    memset((void *)&cfg, 0, sizeof(aal_l2_cls_l2rl2r_key_t));
    cls_mask.u64 = 0;

    rule_0 = 0;
    rule_1 = 1;  /* default rule always use last one */

    cfg.valid = TRUE;
    //cfg.entry_type = AAL_L2_CLS_KEY_ENTRY_TYPE_2RULE1ENTRY;
    //cfg.two_rule_is_and = FALSE;
    cls_mask.s.valid = 1;
   // cls_mask.s.entry_type = 1;
   // cls_mask.s.two_rule_is_and = 1;

    /* rule 0 */
    /*
    if (partner) {
        cfg.rule_type[rule_0] = AAL_L2_CLS_KEY_2RULE_TYPE_L2RULE;
        l2r_p = &cfg.l2r_key[rule_0];

        __aal_mac_filter_load_cls_rule(l2r_p, partner->filter_cfg.mac,
                                       partner->filter_cfg.mac_flag,
                                       partner->filter_cfg.mask,
                                       partner->filter_cfg.vid);

    }
    */
    /* rule 1 */
    //cfg.rule_type_1 = AAL_L2_CLS_KEY_2RULE_TYPE_L2RULE;
    //cls_mask.s.rule_type_1 = 1;

    ret = aal_l2_cls_l2rl2r_key_get(device_id,entry_id,&cfg);
    if(ret != CA_E_OK){
       goto end;
    }

    cls_mask.s.valid                   = 1 ;
    cls_mask.s.ethernet_type_vld_1     = 1 ;
    cls_mask.s.l4_port_field_ctrl_1    = 1 ;
    cls_mask.s.mac_field_ctrl_1        = 1 ;
    cls_mask.s.vlan_field_ctrl_1       = 1 ;
    cls_mask.s.dot1p_ctrl_1            = 1 ;
    cls_mask.s.vlan_number_vld_1       = 1 ;
    cfg.valid                          = TRUE;
    cfg.ethernet_type_vld_1            = FALSE;
    cfg.l4_port_field_ctrl_1           = AAL_L2_CLS_L4_PORT_FIELD_CTRL_MASK_L4PORT;
    cfg.mac_field_ctrl_1               = AAL_L2_CLS_MAC_FIELD_CTRL_MASK_MAC;
    cfg.vlan_field_ctrl_1              = AAL_L2_CLS_VLAN_FIELD_CTRL_MASK_VLAN;
    cfg.dot1p_ctrl_1                   = AAL_L2_CLS_DOT1P_FIELD_CTRL_MASK_DOT1P;
    cfg.vlan_number_vld_1              = FALSE;

    /*** set rule ***/
    ret = aal_l2_cls_l2rl2r_key_set(device_id, entry_id, cls_mask,&cfg);

    if (ret != CA_E_OK) {
        L2_DEBUG("set rule entry_id:%d fail\r\n", entry_id);
    }

    /* fib 0 */
    /*
    memset(&fib, 0, sizeof(aal_l2_cls_fib_t));
    fib.cls_fib[0].mask.s.permit = 1;
    fib.cls_fib[0].permit_valid = 1;
    fib.cls_fib[0].permit = !partner->filter_cfg.drop_flag;
    */
    /* fib 1 */
    fib_mask.u64 = 0;
    fib_mask.s.permit_valid_1 = 1;
    fib_mask.s.spt_mode_1 = 1;
    fib.permit_valid_1 = 1;
    fib_mask.s.permit_1 = 1;
    fib.permit_1 = !drop_flag;
    fib.spt_mode_1 = AAL_L2_CLS_SPT_MODE_FWD_LEARN;

    /*** set fib ***/
    ret = aal_l2_cls_fib_set(device_id, entry_id,fib_mask,&fib);

    if (ret != CA_E_OK) {
        L2_DEBUG("set fib entry_id:%d fail\r\n", entry_id);
    }

    /* modify rb tree */
    /* -- not save in rb tree
    memset(&new_entry, 0, sizeof(aal_tbl_mac_filter_entry_t));
    new_entry.port_id = port_id;
    new_entry.rule_id = CA_AAL_FDB_MAC_FILTER_DFT_ID;
    new_entry.filter_cfg.drop_flag = drop_flag;
    __aal_mac_filter_tbl_add_del(__FDB_TBL_OPCODE_APPEND, &new_entry);
    */
    /* save default behavior*/
    mac_filter_dft[port_id] = drop_flag;

    __FDB_UNLOCK();

    return CA_E_OK;

end:
    __FDB_UNLOCK();
    return ret;


}


ca_status_t aal_fdb_mac_filter_default_get (
    CA_IN  ca_device_id_t  device_id,
    CA_IN  ca_int32     port_id,
    CA_OUT ca_boolean_t *drop_flag
)
{
    ca_uint32 rule_id = CA_AAL_FDB_MAC_FILTER_DFT_ID;

    CA_ASSERT_RET(drop_flag != NULL, CA_E_PARAM);

    CA_AAL_FDB_PORT_CHECK(port_id);

    __FDB_LOCK();

    *drop_flag = mac_filter_dft[port_id];

    L2_DEBUG("rule_id:%d, drop_flag:%d\r\n", rule_id, *drop_flag);

    __FDB_UNLOCK();

    return CA_E_OK;
}


ca_status_t aal_fdb_mac_filter_add (
    CA_IN ca_device_id_t              device_id,
    CA_IN ca_int32                 port_id,
    CA_IN aal_tbl_mac_filter_entry_t *entry
)
{
    ca_uint32 rule_id, partner_id;
    aal_tbl_mac_filter_entry_t *rule = NULL;
    aal_tbl_mac_filter_entry_t *partner = NULL;
    aal_tbl_mac_filter_entry_t new_entry;
    ca_uint32 rule_0, rule_1;
    aal_l2_cls_l2r_key_t *l2r_p = NULL;
    ca_uint32 entry_id;
    aal_l2_cls_l2rl2r_key_t cfg;
    aal_l2_cls_l2rl2r_key_mask_t cls_mask;
    ca_status_t ret;
    aal_l2_cls_fib_t fib;
    aal_l2_cls_fib_mask_t  fib_mask;

    CA_ASSERT_RET(entry != NULL, CA_E_PARAM);

    CA_AAL_FDB_PORT_CHECK(port_id);

    __FDB_LOCK();


    rule = __aal_mac_filter_rule_get_by_key(port_id, entry);
    if (rule != NULL) {
        __FDB_UNLOCK();
        L2_DEBUG("found duplicate\r\n");
        return CA_E_CONFLICT;
    }

    rule_id = __aal_mac_filter_alloc_rule_id(port_id);

    if (rule_id == 0) {
        L2_DEBUG("mac filter table full\r\n");
        __FDB_UNLOCK();
        return CA_E_UNAVAIL;
    }

    /* get partner id in one entry */
    partner_id = CA_AAL_RULE_ID_PARTNER(rule_id);
    partner = __aal_mac_filter_rule_get_by_rule(port_id, partner_id);

    L2_DEBUG("rule_id:%d, partner_id:%d\r\n", rule_id, partner_id);

    memset((void *)&cfg, 0, sizeof(aal_l2_cls_l2rl2r_key_t));
    cls_mask.u64 = 0;

    rule_0 = CLS_IDX_IN_ENTRY(rule_id);
    rule_1 = CLS_IDX_IN_ENTRY(partner_id);

    L2_DEBUG("rule_id:%d, rule_0/1:%d/%d\r\n", rule_id, rule_0, rule_1);

    cfg.valid = TRUE;
   // cfg.entry_type = AAL_L2_CLS_KEY_ENTRY_TYPE_2RULE1ENTRY;
   // cfg.two_rule_is_and = FALSE;
    cls_mask.s.valid = 1;
    //cls_mask.s.entry_type = 1;
    //cls_mask.s.two_rule_is_and = 1;


    /* rule parnter */
    if (partner) {
        if(rule_1 == 0){
            //cfg.rule_type_0 = AAL_L2_CLS_KEY_2RULE_TYPE_L2RULE;
            //cls_mask.s.rule_type_0 = 1;
            L2_DEBUG("\r\n");
            l2r_p = (aal_l2_cls_l2r_key_t *)&cfg.mac_da_0;
        }else{
           // cfg.rule_type_1 = AAL_L2_CLS_KEY_2RULE_TYPE_L2RULE;
            //cls_mask.s.rule_type_1 = 1;
            L2_DEBUG("\r\n");
            l2r_p = (aal_l2_cls_l2r_key_t *)&cfg.mac_da_1;
        }
        __aal_mac_filter_load_cls_rule(port_id, &cls_mask, rule_1, l2r_p, partner->mac,
                                       partner->mac_flag,
                                       partner->mask,
                                       partner->vid);

        L2_DEBUG("mac_da:0x%02x:%02x:%02x:%02x:%02x:%02x, mac_sa:0x%02x:%02x:%02x:%02x:%02x:%02x, mac_field_ctrl:%d, vlan:%d/%d\r\n",
        l2r_p->mac_da[0], l2r_p->mac_da[1], l2r_p->mac_da[2], l2r_p->mac_da[3], l2r_p->mac_da[4], l2r_p->mac_da[5],
        l2r_p->mac_sa[0], l2r_p->mac_sa[1], l2r_p->mac_sa[2], l2r_p->mac_sa[3], l2r_p->mac_sa[4], l2r_p->mac_sa[5],
        l2r_p->mac_field_ctrl, l2r_p->top_vlan_id, l2r_p->inner_vlan_id);
    }

    /* rule 0 */
    if(rule_0 == 0){
        // cfg.rule_type_0 = AAL_L2_CLS_KEY_2RULE_TYPE_L2RULE;
        // cls_mask.s.rule_type_0 = 1;
        L2_DEBUG("\r\n");
        l2r_p = (aal_l2_cls_l2r_key_t *)&cfg.mac_da_0;
    }else{
        //cfg.rule_type_1 = AAL_L2_CLS_KEY_2RULE_TYPE_L2RULE;
        //cls_mask.s.rule_type_1 = 1;
        L2_DEBUG("\r\n");
        l2r_p = (aal_l2_cls_l2r_key_t *)&cfg.mac_da_1;
    }

    /* mac */
    __aal_mac_filter_load_cls_rule(port_id, &cls_mask, rule_0, l2r_p, entry->mac,
                                   entry->mac_flag,
                                   entry->mask,
                                   entry->vid);

    L2_DEBUG("mac_da:0x%02x:%02x:%02x:%02x:%02x:%02x, mac_sa:0x%02x:%02x:%02x:%02x:%02x:%02x, mac_field_ctrl:%d, vlan:%d/%d\r\n",
    l2r_p->mac_da[0], l2r_p->mac_da[1], l2r_p->mac_da[2], l2r_p->mac_da[3], l2r_p->mac_da[4], l2r_p->mac_da[5],
    l2r_p->mac_sa[0], l2r_p->mac_sa[1], l2r_p->mac_sa[2], l2r_p->mac_sa[3], l2r_p->mac_sa[4], l2r_p->mac_sa[5],
    l2r_p->mac_field_ctrl, l2r_p->top_vlan_id, l2r_p->inner_vlan_id);

    /*** set rule ***/
    entry_id = CLS_ENTRY_ID(port_id, rule_id);

    L2_DEBUG("set entry:%d, rule:%d, port_id:%d\r\n", entry_id, rule_id, port_id);

    /*octet_dump("cfg", (ca_uint8_t *)&cfg, sizeof(cfg));*/

    ret = aal_l2_cls_l2rl2r_key_set(device_id, entry_id, cls_mask,&cfg);


    if (ret != CA_E_OK) {
        L2_DEBUG("set rule entry_id:%d fail\r\n", entry_id);
    }

    /* fib parnter */
    fib_mask.u64 = 0;
    memset((void *)&fib, 0, sizeof(aal_l2_cls_fib_t));
    if (partner){
        fib_mask.s.permit_valid_1 = 1;
        fib.permit_valid_1 = 1;
        fib_mask.s.permit_1 = 1;
        fib.permit_1 = !partner->drop_flag;
    }

    /* fib rule */
    fib_mask.s.permit_valid_0 = 1;
    fib.permit_valid_0 = 1;
    fib_mask.s.permit_0 = 1;
    fib.permit_0 = !entry->drop_flag;

    L2_DEBUG("set fib\r\n");

    /*** set fib ***/
    ret = aal_l2_cls_fib_set(device_id, entry_id,fib_mask,&fib);

    if (ret != CA_E_OK) {
        L2_DEBUG("set fib entry_id:%d fail\r\n", entry_id);
    }

    /* modify rb tree */
    memset((void *)&new_entry, 0, sizeof(aal_tbl_mac_filter_entry_t));
    new_entry.port_id = port_id;
    new_entry.rule_id = rule_id;
    new_entry.entry_id = entry_id;
    new_entry.mask = entry->mask;
    new_entry.mac_flag = entry->mac_flag;

    if (entry->mask == CA_L2_MAC_FILTER_MASK_MAC_VLAN)
        new_entry.vid = entry->vid;
    new_entry.drop_flag = entry->drop_flag;
    memcpy(&new_entry.mac, &entry->mac, CA_ETH_ADDR_LEN);

    L2_DEBUG("insert rb tree, port:%d, rule:%d/%d, mask:%d, mac_flag:%d, vid:%d, drop_flag:%d, mac:0x%02x:%02x:%02x:%02x:%02x:%02x\r\n",
        new_entry.port_id, new_entry.entry_id, new_entry.rule_id, new_entry.mask, new_entry.mac_flag, new_entry.vid, new_entry.drop_flag,
        new_entry.mac[0], new_entry.mac[1], new_entry.mac[2], new_entry.mac[3], new_entry.mac[4], new_entry.mac[5]);
    __aal_mac_filter_tbl_add_del(__FDB_TBL_OPCODE_APPEND, &new_entry);

    __FDB_UNLOCK();

    return CA_E_OK;
}


ca_status_t aal_fdb_mac_filter_delete (
    CA_IN ca_device_id_t              device_id,
    CA_IN ca_int32                 port_id,
    CA_IN aal_tbl_mac_filter_entry_t *entry
)
{
    aal_tbl_mac_filter_entry_t *rule = NULL;
    aal_tbl_mac_filter_entry_t *partner = NULL;
    aal_tbl_mac_filter_entry_t new_entry;
    ca_uint32 rule_id, partner_id;
    ca_uint32 rule_0, rule_1;
    aal_l2_cls_l2r_key_t *l2r_p = NULL;
    ca_uint32 entry_id;
    aal_l2_cls_l2rl2r_key_t cfg;
    aal_l2_cls_l2rl2r_key_mask_t cls_mask;
    aal_l2_cls_fib_t fib;
    aal_l2_cls_fib_mask_t fib_mask;
    ca_status_t ret;

    CA_ASSERT_RET(entry != NULL, CA_E_PARAM);

    CA_AAL_FDB_PORT_CHECK(port_id);

    __FDB_LOCK();

    rule = __aal_mac_filter_rule_get_by_key(port_id, entry);

    if (rule == NULL) {
        __FDB_UNLOCK();
        L2_DEBUG("no found entry\r\n");
        return CA_E_UNAVAIL;
    }

    rule_id = rule->rule_id;

    /* get partner id in one entry */
    partner_id = CA_AAL_RULE_ID_PARTNER(rule_id);
    partner = __aal_mac_filter_rule_get_by_rule(port_id, partner_id);

    L2_DEBUG("rule_id:%d, partner_id:%d\r\n", rule_id, partner_id);

    memset((void *)&cfg, 0, sizeof(aal_l2_cls_l2rl2r_key_t));
    cls_mask.u64 = 0;

    rule_0 = CLS_IDX_IN_ENTRY(rule_id);
    rule_1 = CLS_IDX_IN_ENTRY(partner_id);

    L2_DEBUG("rule_0/1:%d/%d\r\n", rule_0, rule_1);

    cfg.valid = FALSE;
    //cfg.entry_type = AAL_L2_CLS_KEY_ENTRY_TYPE_2RULE1ENTRY;
   // cfg.two_rule_is_and = FALSE;
    cls_mask.s.valid = 1;
    //cls_mask.s.entry_type = 1;
    //cls_mask.s.two_rule_is_and = 1;

    /* rule parnter */
    if (partner) {
        cfg.valid = TRUE;

        if(rule_1 == 0){
           // cfg.rule_type_0 = AAL_L2_CLS_KEY_2RULE_TYPE_L2RULE;
            //cls_mask.s.rule_type_0 = 1;
            l2r_p = (aal_l2_cls_l2r_key_t *)&cfg.mac_da_0;
        }else{
            //cfg.rule_type_1 = AAL_L2_CLS_KEY_2RULE_TYPE_L2RULE;
            //cls_mask.s.rule_type_1 = 1;
            l2r_p = (aal_l2_cls_l2r_key_t *)&cfg.mac_da_1;
        }

        __aal_mac_filter_load_cls_rule(port_id, &cls_mask, rule_1, l2r_p, partner->mac,
                                       partner->mac_flag,
                                       partner->mask,
                                       partner->vid);
    }

    /* rule 0 */
    if(rule_0 == 0){
       // cfg.rule_type_0 = AAL_L2_CLS_KEY_2RULE_TYPE_L2RULE;
        //cls_mask.s.rule_type_0 = 1;
        cfg.mac_field_ctrl_0 = AAL_L2_CLS_MAC_FIELD_CTRL_MASK_MAC;
        cls_mask.s.mac_field_ctrl_0 = 1;
        l2r_p = (aal_l2_cls_l2r_key_t *)&cfg.mac_da_0;

        /* vlan */
        if (entry->mask == CA_L2_MAC_FILTER_MASK_MAC_VLAN){
            cfg.vlan_field_ctrl_0 = AAL_L2_CLS_VLAN_FIELD_CTRL_MASK_VLAN;
            cls_mask.s.vlan_field_ctrl_0 = 1;
        }
    }else{
      // cfg.rule_type_1 = AAL_L2_CLS_KEY_2RULE_TYPE_L2RULE;
       //cls_mask.s.rule_type_1 = 1;
       cfg.mac_field_ctrl_1 = AAL_L2_CLS_MAC_FIELD_CTRL_MASK_MAC;
       cls_mask.s.mac_field_ctrl_1 = 1;
       l2r_p = (aal_l2_cls_l2r_key_t *)&cfg.mac_da_1;

        /* vlan */
        if (entry->mask == CA_L2_MAC_FILTER_MASK_MAC_VLAN){
            cfg.vlan_field_ctrl_1 = AAL_L2_CLS_VLAN_FIELD_CTRL_MASK_VLAN;
            cls_mask.s.vlan_field_ctrl_1 = 1;
        }
    }

    /*** set rule ***/
    entry_id = CLS_ENTRY_ID(port_id, rule_id);

    L2_DEBUG("set entry:%d, rule:%d, port_id:%d\r\n", entry_id, rule_id, port_id);

    ret = aal_l2_cls_l2rl2r_key_set(device_id, entry_id,cls_mask, &cfg);

    if (ret != CA_E_OK) {
        L2_DEBUG("set rule entry_id:%d fail\r\n", entry_id);
    }

    /* fib parnter */
    memset((void *)&fib, 0, sizeof(aal_l2_cls_fib_t));
    fib_mask.u64 = 0;
    if (partner){
        if (rule_1 == 1){
            fib_mask.s.permit_valid_1 = 1;
            fib.permit_valid_1= 1;
            fib_mask.s.permit_1 = 1;
            fib.permit_1 = !partner->drop_flag;

        }
        else{
            fib_mask.s.permit_valid_0 = 1;
            fib.permit_valid_0 = 1;
            fib.permit_0 = !partner->drop_flag;
            fib_mask.s.permit_0 = 1;

        }
    }


    L2_DEBUG("set fib\r\n");
    /*** set fib ***/
    ret = aal_l2_cls_fib_set(device_id, entry_id,fib_mask,&fib);

    if (ret != CA_E_OK) {
        L2_DEBUG("set fib entry_id:%d fail\r\n", entry_id);
    }

    /* modify rb tree */
    memset((void *)&new_entry, 0, sizeof(aal_tbl_mac_filter_entry_t));
    new_entry.port_id = port_id;
    new_entry.rule_id = rule_id;
    new_entry.mask = entry->mask;
    new_entry.mac_flag = entry->mac_flag;
    if (entry->mask == CA_L2_MAC_FILTER_MASK_MAC_VLAN)
        new_entry.vid = entry->vid;
    new_entry.drop_flag = entry->drop_flag;
    memcpy(&new_entry.mac, &entry->mac, CA_ETH_ADDR_LEN);

    L2_DEBUG("delete rb tree, port:%d, rule:%d, mask:%d, mac_flag:%d, vid:%d, drop_flag:%d, mac:0x%02x:%02x:%02x:%02x:%02x:%02x\r\n",
        new_entry.port_id, new_entry.rule_id, new_entry.mask, new_entry.mac_flag, new_entry.vid, new_entry.drop_flag,
        new_entry.mac[0], new_entry.mac[1], new_entry.mac[2], new_entry.mac[3], new_entry.mac[4], new_entry.mac[5]);

    __aal_mac_filter_tbl_add_del(__FDB_TBL_OPCODE_DELETE, &new_entry);

    __FDB_UNLOCK();

    return CA_E_OK;
}


ca_status_t aal_fdb_mac_filter_delete_all (
    CA_IN ca_device_id_t  device_id,
    CA_IN ca_int32     port_id
)
{
    ca_uint32 entry_max = CLS_IDX_IN_ENTRY(CA_AAL_FDB_MAC_FILTER_DFT_ID);
    ca_int32 i;
    aal_l2_cls_l2rl2r_key_t cfg;
    aal_l2_cls_l2rl2r_key_mask_t cls_mask;
    aal_l2_cls_fib_t fib;
    aal_l2_cls_fib_mask_t fib_mask;
    ca_status_t ret;
    ca_uint32 start_id = CLS_ENTRY_ID(port_id,1);
    ca_uint32 entry_id;
    aal_l2_cls_entry_status_mask_t cls_entry_status_mask;
    aal_l2_cls_entry_status_t  cls_entry_status;


    L2_DEBUG("port_id:%d\r\n", port_id);
    CA_AAL_FDB_PORT_CHECK(port_id);


    memset((void *)&cfg, 0, sizeof(aal_l2_cls_l2rl2r_key_t));
    memset((void *)&fib, 0, sizeof(aal_l2_cls_fib_t));
    memset(&cls_entry_status_mask,0,sizeof(aal_l2_cls_entry_status_mask_t));
    memset(&cls_entry_status,0,sizeof(aal_l2_cls_entry_status_t));

    cls_mask.u64 = 0;
    fib_mask.u64 = 0;
    cls_entry_status_mask.u32 = 0;

    fib_mask.s.permit_0 = 1;
    fib_mask.s.permit_valid_0 = 1;
    fib_mask.s.permit_1 = 1;
    fib_mask.s.permit_valid_1 = 1;

    cls_entry_status_mask.s.valid = 1;
    cls_entry_status_mask.s.entry_type = 1;
    cls_entry_status_mask.s.is_ipv6_rule = 1;
    cls_entry_status_mask.s.rule_0 = 1;
    cls_entry_status_mask.s.rule_1 = 1;

    __FDB_LOCK();
    cfg.mac_field_ctrl_1 = AAL_L2_CLS_MAC_FIELD_CTRL_MASK_MAC;
    cls_mask.s.mac_field_ctrl_1 = 1;
    cfg.mac_field_ctrl_0 = AAL_L2_CLS_MAC_FIELD_CTRL_MASK_MAC;
    cls_mask.s.mac_field_ctrl_0 = 1;

    cls_entry_status.valid = 0;
    cls_entry_status.entry_type = 0;
    cls_entry_status.is_ipv6_rule = 0;
    cls_entry_status.rule_0 = 0;
    cls_entry_status.rule_1 = 0;

    for (i = 0; i < 3; i++) {
        entry_id = start_id + i;
        aal_l2_cls_entry_status_set(device_id,entry_id,cls_entry_status_mask,&cls_entry_status);
        aal_l2_cls_fib_set(device_id, entry_id,fib_mask,&fib);
    }

    L2_DEBUG("clean rb tree\r\n");
    ret = __aal_mac_filter_tbl_flush_port(port_id);

    L2_DEBUG("clean rb tree end\r\n");

    __FDB_UNLOCK();

    return CA_E_OK;
}

aal_tbl_mac_filter_entry_t *aal_fdb_mac_filter_getfirst(void)
{
    aal_tbl_mac_filter_entry_t *tmp_p = NULL;

    tmp_p = (aal_tbl_mac_filter_entry_t *) ca_table_entry_getfirst(
                mac_filter_tbl_p);

    return tmp_p;
}

aal_tbl_mac_filter_entry_t *aal_fdb_mac_filter_getnext(void)
{
    aal_tbl_mac_filter_entry_t *tmp_p = NULL;

    tmp_p = (aal_tbl_mac_filter_entry_t *) ca_table_entry_getnext(
                mac_filter_tbl_p);

    return tmp_p;
}

ca_status_t aal_l2e_hash_collision_int_set(ca_device_id_t device_id, ca_boolean_t flag)
{
    L2FE_GLB_INTERRUPTE_t l2fe_glb;


    L2_DEBUG("hash collision enable:%d\r\n", flag);
    if (device_id == 0){
        l2fe_glb.wrd  = CA_NE_REG_READ(L2FE_GLB_INTERRUPTE);

        l2fe_glb.bf.l2e_hash_collision_intIE = flag;

        FDB_REG_WRITE(L2FE_GLB_INTERRUPTE, l2fe_glb.wrd);
    }
#ifdef CONFIG_ARCH_CORTINA_G3HGU
    else if (device_id == 1){
        l2fe_glb.wrd  = CA_8279_NE_REG_READ(L2FE_GLB_INTERRUPTE);

        l2fe_glb.bf.l2e_hash_collision_intIE = flag;

        CA_8279_NE_REG_WRITE(l2fe_glb.wrd, L2FE_GLB_INTERRUPTE);
    }
#endif
    return CA_E_OK;
}
#else//sd1 uboot for 98f
#endif//sd1 uboot for 98f

ca_status_t aal_fdb_init(
    ca_device_id_t     device_id
)
{
    ca_int32 ret = 0;
    ca_int32 i = 0;
    aal_fdb_ctrl_mask_t  fdb_ctrl_mask;
    aal_fdb_ctrl_t       fdb_ctrl;
    lrn_fwd_ctrl_t       fwd_ctrl;
    lrn_fwd_ctrl_mask_t  fwd_ctrl_mask;

#if 0//yocto
    struct ca_soc_data   soc_data;

    if (ca_soc_data_get(&soc_data) != 0)
        ca_printf("Fail to get SoC data.\n");

    __aal_fdb_tbl_create();
    __aal_mac_filter_tbl_create();

    ret = ca_spin_lock_init(&g_fdb_lock, SPINLOCK_BH);

    if (ret != 0) {
        ca_printf("create spin lock %s fail\r\n", CA_AAL_FDB_MUTEX_NAME);
    }
#else//sd1 uboot for 98f
#endif//sd1 uboot for 98f

    /* TODO */
    for (i = 0; i < 64; i++) {
        aal_fdb_port_limit_set(0, i, 0x1fff);
    }

    aal_fdb_scan_flush_field_init(0);
    fdb_ctrl_mask.wrd = ~0U;
#if 0
#ifdef CONFIG_CORTINA_BOARD_FPGA
    fdb_ctrl.aging_en                    = 0;
#else
    fdb_ctrl.aging_en                    = 1;
#endif
#else
    fdb_ctrl.aging_en                    = 1;
#endif
    fdb_ctrl.aging_out_time              = CA_AAL_FDB_DEF_AGING_OUT_TIME;

#if 0//98f luna
    if (soc_data.core_clk != 0) {
        /* micro-second base timer is 256, aging status timer is 255 */
        fdb_ctrl.cnt_to_1s               = soc_data.core_clk / 256 / 255;//luna: 0x1312d00 / 256 / 255 = 0x132
    } else {
        fdb_ctrl.cnt_to_1s               = 3830; /* assume core clock is 250MHz */
    }
#else//sd1 uboot for 98f
    fdb_ctrl.cnt_to_1s = 0x132;

#if CONFIG_98F_UBOOT_NE_DBG//from 98f luna, for sd1 uboot
    ca_printf("\t%s(%d) fdb_ctrl.cnt_to_1s: %#x\n", __func__, __LINE__, fdb_ctrl.cnt_to_1s);//luna: 0x132
#endif//from 98f luna, for sd1 uboot
#endif//sd1 uboot for 98f

    fdb_ctrl.tmp_entry_aging_dis         = 0;
    fdb_ctrl.st_move_en                  = 1;
    fdb_ctrl.lrn_dis                     = 0;
    fdb_ctrl.da_deny_dnt_lrn             = 0;
    fdb_ctrl.hash_collision_overwrite    = 1;
    fdb_ctrl.ovfl_entries_rcyc_en        = 0;
    fdb_ctrl.hashing_mode                = 3;
    fdb_ctrl.lrn_mode                    = CA_AAL_FDB_LRN_MODE_HW;
    fdb_ctrl.aging_mode                  = 0;
    fdb_ctrl.aging_start_val             = CA_AAL_FDB_DEF_AGING_START_VAL;
    fdb_ctrl.st_mv_window                = 0x1FF;
    fdb_ctrl.cpu_opt_mode                = 1;
#if 1//yocto - per Michael's MP request, we make unkvlan to be learnt to FDB table
    fdb_ctrl.unkvlan_learn               = CA_AAL_FDB_UNK_VLAN_BE_LEARN;
#else//sd1 uboot for 98f ne mp test - always do uuc flooding
    fdb_ctrl.unkvlan_learn               = CA_AAL_FDB_UNK_VLAN_NOT_LEARN;
#endif//sd1 uboot for 98f ne mp test - always do uuc flooding
    fdb_ctrl.unk_ethtype_no_learn        = 0;
    fdb_ctrl.igr_dropped_pkt_no_learn    = 0;
#if 0//yocto
    fdb_ctrl.non_std_sa_all_zero_learn   = 0;
    fdb_ctrl.non_std_sa_all_one_learn    = 0;
    fdb_ctrl.non_std_sa_b40_is_one_learn = 0;
#else//sd1 uboot for 98f ne mp test - per Michael's MP request, we make all pkt be learnt
    fdb_ctrl.non_std_sa_all_zero_learn   = 1;
    fdb_ctrl.non_std_sa_all_one_learn    = 1;
    fdb_ctrl.non_std_sa_b40_is_one_learn = 1;
#endif//sd1 uboot for 98f ne mp test - per Michael's MP request, we make all pkt be learnt
    fdb_ctrl.loop_pkt_no_learn           = 0;

    ret = aal_fdb_ctrl_set(device_id, fdb_ctrl_mask, &fdb_ctrl);
    if (CA_E_OK != ret) {
        ca_printf("aal_fdb_ctrl_set FAIL, return %#x\n", ret);
    }

#if 0//yocto
    /* enable l2e hash collision interrupt */
    aal_l2e_hash_collision_int_set(device_id, TRUE);
#else//sd1 uboot for 98f
#endif//sd1 uboot for 98f

    /* default set to forward to cpu when hash collision */
    memset(&fwd_ctrl_mask, 0, sizeof(lrn_fwd_ctrl_mask_t));
    memset(&fwd_ctrl, 0, sizeof(lrn_fwd_ctrl_t));

    fwd_ctrl_mask.bf.sa_hash_cllsn_fwd_ctrl = 1;
    fwd_ctrl.hash_cllsn_mode = SA_LRN_FWD_CTRL_REDIRECT;
    fwd_ctrl_mask.bf.sa_hash_cllsn_ldpid = 1;
    fwd_ctrl.hash_cllsn_ldpid = AAL_LPORT_CPU_0;
    fwd_ctrl_mask.bf.sa_limit_excd_fwd_ctrl = 1;
    fwd_ctrl.limit_excd_mode = SA_LRN_FWD_CTRL_REDIRECT;
    fwd_ctrl_mask.bf.sa_limit_excd_ldpid = 1;
    fwd_ctrl.limit_excd_ldpid = AAL_LPORT_CPU_0;
    fwd_ctrl_mask.bf.l2_mac_sw_lrn_ldpid = 1;
    fwd_ctrl.l2_mac_sw_lrn_ldpid = AAL_LPORT_CPU_0;
    fwd_ctrl_mask.bf.sm_static_entry_fwd = 1;
    fwd_ctrl.sm_static_entry_fwd = 1;
#if 0//yocto
#else//sd1 uboot for 98f ne mp test - per Michael's MP request, we make all pkt be learnt
    fwd_ctrl_mask.bf.nonstd_sa_b40_is_one_fwd = 1;
    fwd_ctrl.nonstd_sa_b40_is_one_fwd = 1;
    fwd_ctrl_mask.bf.nonstd_sa_all_one_fwd = 1;
    fwd_ctrl.nonstd_sa_all_one_fwd = 1;
    fwd_ctrl_mask.bf.nonstd_sa_all_zero_fwd = 1;
    fwd_ctrl.nonstd_sa_all_zero_fwd = 1;
#endif//sd1 uboot for 98f ne mp test - per Michael's MP request, we make all pkt be learnt


    aal_fdb_lrn_fwd_ctrl_set(device_id, fwd_ctrl_mask, &fwd_ctrl);
    aal_port_flooding_init(device_id);

#if CONFIG_98F_UBOOT_NE_DBG
    ca_printf("FDB init OK\n");
#endif /* CONFIG_98F_UBOOT_NE_DBG */

    //disable fdb aging
    aal_fdb_disable_table_aging(device_id, DISABLE);

    return CA_E_OK;
}

#if 0//yocto
ca_status_t aal_fdb_mac_filter_init(
    ca_device_id_t     device_id
)
{
    int i = 0;

    memset((void *)&mac_filter_dft, 0, sizeof(mac_filter_dft));

    for (i = 0; i < CA_AAL_FDB_ETH_PORT_NUM; i++){
        aal_fdb_mac_filter_default_set(device_id, i, FALSE);
    }

    return CA_E_OK;
}
#else//sd1 uboot for 98f
#endif//sd1 uboot for 98f

ca_status_t aal_fdb_ctrl_set(
    CA_IN ca_device_id_t          device_id,
    CA_IN aal_fdb_ctrl_mask_t  ctrl_mask,
    CA_IN aal_fdb_ctrl_t       *ctrl
    )
{
    L2FE_L2E_CTRL_t  l2e_ctrl;
    L2FE_PLC_L2_LEARNING_CTRL_t  l2_lrn;
    L2FE_L2E_AGING_CTRL_t l2e_aging;

    if (device_id == 0){
        l2e_ctrl.wrd  = CA_NE_REG_READ(L2FE_L2E_CTRL);
        l2_lrn.wrd    = CA_NE_REG_READ(L2FE_PLC_L2_LEARNING_CTRL);
        l2e_aging.wrd = CA_NE_REG_READ(L2FE_L2E_AGING_CTRL);
    }
#ifdef CONFIG_ARCH_CORTINA_G3HGU
    else if (device_id == 1){
        l2e_ctrl.wrd  = CA_8279_NE_REG_READ(L2FE_L2E_CTRL);
        l2_lrn.wrd    = CA_8279_NE_REG_READ(L2FE_PLC_L2_LEARNING_CTRL);
        l2e_aging.wrd = CA_8279_NE_REG_READ(L2FE_L2E_AGING_CTRL);
    }
#endif
    if (ctrl_mask.s.aging_en             )
        l2e_ctrl.bf.aging_en              = ctrl->aging_en;
    if (ctrl_mask.s.tmp_entry_aging_dis  )
        l2e_ctrl.bf.tmp_entry_aging_dis   = ctrl->tmp_entry_aging_dis;
    if (ctrl_mask.s.st_move_en           )
        l2e_ctrl.bf.st_move_en            = ctrl->st_move_en;
    if (ctrl_mask.s.lrn_dis              )
        l2e_ctrl.bf.lrn_dis               = ctrl->lrn_dis;
    if (ctrl_mask.s.da_deny_dnt_lrn      )
        l2e_ctrl.bf.da_deny_dnt_lrn       = ctrl->da_deny_dnt_lrn;
    if (ctrl_mask.s.hash_collision_overwrite)
        l2e_ctrl.bf.hash_collision_overwrite = ctrl->hash_collision_overwrite;
    if (ctrl_mask.s.ovfl_entries_rcyc_en )
        l2e_ctrl.bf.ovfl_entries_rcyc_en  = ctrl->ovfl_entries_rcyc_en;
    if (ctrl_mask.s.hashing_mode         )
        l2e_ctrl.bf.hashing_mode          = ctrl->hashing_mode;
    if (ctrl_mask.s.lrn_mode             )
        l2e_ctrl.bf.lrn_mode              = ctrl->lrn_mode;
    if (ctrl_mask.s.aging_mode           )
        l2e_ctrl.bf.aging_mode            = ctrl->aging_mode;
    if (ctrl_mask.s.aging_start_val      )
        l2e_ctrl.bf.aging_start_val       = ctrl->aging_start_val;
    if (ctrl_mask.s.st_mv_window         )
        l2e_ctrl.bf.st_mv_window          = ctrl->st_mv_window;
    if (ctrl_mask.s.cpu_opt_mode         )
        l2e_ctrl.bf.cpu_opt_mode          = ctrl->cpu_opt_mode;
    if (ctrl_mask.s.unkvlan_learn        )
        l2_lrn.bf.unkvlan_learn           = ctrl->unkvlan_learn;
    if (ctrl_mask.s.unk_ethtype_no_learn )
        l2_lrn.bf.unk_ethtype_no_learn   = ctrl->unk_ethtype_no_learn;
    if (ctrl_mask.s.igr_dropped_pkt_no_learn )
        l2_lrn.bf.igr_dropped_pkt_no_learn  = ctrl->igr_dropped_pkt_no_learn;
    if (ctrl_mask.s.non_std_sa_all_zero_learn)
        l2_lrn.bf.non_std_sa_all_zero_learn  = ctrl->non_std_sa_all_zero_learn;
    if (ctrl_mask.s.non_std_sa_all_one_learn )
        l2_lrn.bf.non_std_sa_all_one_learn  = ctrl->non_std_sa_all_one_learn;
    if (ctrl_mask.s.non_std_sa_b40_is_one_learn)
        l2_lrn.bf.non_std_sa_b40_is_one_learn = ctrl->non_std_sa_b40_is_one_learn;
    if (ctrl_mask.s.loop_pkt_no_learn)
        l2_lrn.bf.loop_pkt_no_learn = ctrl->loop_pkt_no_learn;
    if (ctrl_mask.s.aging_out_time){
        l2e_aging.bf.aging_out_time = ctrl->aging_out_time;
        l2_aging_time = ctrl->aging_out_time;
    }
    if (ctrl_mask.s.cnt_to_1s)
        l2e_aging.bf.cnt_to_1s = ctrl->cnt_to_1s;

    if (device_id == 0){
        FDB_REG_WRITE(L2FE_L2E_CTRL, l2e_ctrl.wrd);
        FDB_REG_WRITE(L2FE_PLC_L2_LEARNING_CTRL, l2_lrn.wrd);
        FDB_REG_WRITE(L2FE_L2E_AGING_CTRL, l2e_aging.wrd);
    }
#ifdef CONFIG_ARCH_CORTINA_G3HGU
    else if (device_id == 1){
        CA_8279_NE_REG_WRITE(l2e_ctrl.wrd, L2FE_L2E_CTRL);
        CA_8279_NE_REG_WRITE(l2_lrn.wrd, L2FE_PLC_L2_LEARNING_CTRL);
        CA_8279_NE_REG_WRITE(l2e_aging.wrd, L2FE_L2E_AGING_CTRL);
    }
#endif
    return CA_E_OK;
}

#if 0//yocto
ca_status_t aal_fdb_ctrl_get(
    CA_IN ca_device_id_t       device_id,
    CA_OUT aal_fdb_ctrl_t  *ctrl)
{
    L2FE_L2E_CTRL_t             l2e_ctrl;
    L2FE_PLC_L2_LEARNING_CTRL_t l2_lrn;
    L2FE_L2E_AGING_CTRL_t       l2e_aging;

    AAL_ASSERT_RET(ctrl != NULL, CA_E_PARAM);

    if (device_id == 0){
        l2e_ctrl.wrd  = CA_NE_REG_READ(L2FE_L2E_CTRL);
        l2_lrn.wrd    = CA_NE_REG_READ(L2FE_PLC_L2_LEARNING_CTRL);
        l2e_aging.wrd = CA_NE_REG_READ(L2FE_L2E_AGING_CTRL);
    }
#ifdef CONFIG_ARCH_CORTINA_G3HGU
    else if (device_id == 1){
        l2e_ctrl.wrd  = CA_8279_NE_REG_READ(L2FE_L2E_CTRL);
        l2_lrn.wrd    = CA_8279_NE_REG_READ(L2FE_PLC_L2_LEARNING_CTRL);
        l2e_aging.wrd = CA_8279_NE_REG_READ(L2FE_L2E_AGING_CTRL);
    }
#endif
    ctrl->aging_en                    = l2e_ctrl.bf.aging_en;
    ctrl->tmp_entry_aging_dis         = l2e_ctrl.bf.tmp_entry_aging_dis;
    ctrl->st_move_en                  = l2e_ctrl.bf.st_move_en;
    ctrl->lrn_dis                     = l2e_ctrl.bf.lrn_dis;
    ctrl->da_deny_dnt_lrn             = l2e_ctrl.bf.da_deny_dnt_lrn;
    ctrl->hash_collision_overwrite    = l2e_ctrl.bf.hash_collision_overwrite;
    ctrl->ovfl_entries_rcyc_en        = l2e_ctrl.bf.ovfl_entries_rcyc_en;
    ctrl->hashing_mode                = l2e_ctrl.bf.hashing_mode;
    ctrl->lrn_mode                    = l2e_ctrl.bf.lrn_mode;
    ctrl->aging_mode                  = l2e_ctrl.bf.aging_mode;
    ctrl->aging_start_val             = l2e_ctrl.bf.aging_start_val;
    ctrl->st_mv_window                = l2e_ctrl.bf.st_mv_window;
    ctrl->cpu_opt_mode                = l2e_ctrl.bf.cpu_opt_mode;
    ctrl->unkvlan_learn               = l2_lrn.bf.unkvlan_learn              ;
    ctrl->unk_ethtype_no_learn        = l2_lrn.bf.unk_ethtype_no_learn       ;
    ctrl->igr_dropped_pkt_no_learn    = l2_lrn.bf.igr_dropped_pkt_no_learn   ;
    ctrl->non_std_sa_all_zero_learn   = l2_lrn.bf.non_std_sa_all_zero_learn  ;
    ctrl->non_std_sa_all_one_learn    = l2_lrn.bf.non_std_sa_all_one_learn   ;
    ctrl->non_std_sa_b40_is_one_learn = l2_lrn.bf.non_std_sa_b40_is_one_learn;
    ctrl->loop_pkt_no_learn           = l2_lrn.bf.loop_pkt_no_learn          ;
    ctrl->aging_out_time              = l2e_aging.bf.aging_out_time;
    ctrl->cnt_to_1s                   = l2e_aging.bf.cnt_to_1s;

    return CA_E_OK;
}

ca_status_t aal_fdb_glb_intr_enable_set(
    CA_IN ca_device_id_t          device_id,
    CA_IN aal_fdb_glb_intr_t  *config
)
{
    L2FE_GLB_INTERRUPTE_t l2fe_intr;

    AAL_ASSERT_RET(config, CA_E_PARAM);

    if (device_id == 0){
        l2fe_intr.wrd = CA_NE_REG_READ(L2FE_GLB_INTERRUPTE);
    }
#ifdef CONFIG_ARCH_CORTINA_G3HGU
    else if (device_id == 1){
        l2fe_intr.wrd = CA_8279_NE_REG_READ(L2FE_GLB_INTERRUPTE);
    }
#endif
    l2fe_intr.bf.lf_overflow_intIE        = config->lf_overflow        ;
    l2fe_intr.bf.ple_hd_ff_overflow_intIE = config->ple_hd_ff_overflow ;
    l2fe_intr.bf.port_mac_lmt_excd_intIE  = config->port_mac_lmt_excd  ;
    l2fe_intr.bf.l2e_hash_collision_intIE = config->l2e_hash_collision ;

    if (device_id == 0){
        FDB_REG_WRITE(L2FE_GLB_INTERRUPTE, l2fe_intr.wrd);
    }
#ifdef CONFIG_ARCH_CORTINA_G3HGU
    else if (device_id == 1){
        CA_8279_NE_REG_WRITE(l2fe_intr.wrd, L2FE_GLB_INTERRUPTE);
    }
#endif
    return CA_E_OK;

}

ca_status_t aal_fdb_glb_intr_enable_get(
    CA_IN  ca_device_id_t         device_id,
    CA_OUT aal_fdb_glb_intr_t *config
)
{
    L2FE_GLB_INTERRUPTE_t l2fe_intr;

    AAL_ASSERT_RET(config, CA_E_PARAM);

    if (device_id == 0){
        l2fe_intr.wrd = CA_NE_REG_READ(L2FE_GLB_INTERRUPTE);
    }
#ifdef CONFIG_ARCH_CORTINA_G3HGU
    else if (device_id == 1){
        l2fe_intr.wrd = CA_8279_NE_REG_READ(L2FE_GLB_INTERRUPTE);
    }
#endif
    config->lf_overflow          = l2fe_intr.bf.lf_overflow_intIE       ;
    config->ple_hd_ff_overflow   = l2fe_intr.bf.ple_hd_ff_overflow_intIE;
    config->port_mac_lmt_excd    = l2fe_intr.bf.port_mac_lmt_excd_intIE ;
    config->l2e_hash_collision   = l2fe_intr.bf.l2e_hash_collision_intIE;

    return CA_E_OK;


}


ca_status_t aal_fdb_glb_intr_status_get(
    CA_IN  ca_device_id_t         device_id,
    CA_OUT aal_fdb_glb_intr_t *status
)
{
    L2FE_GLB_INTERRUPT_t l2fe_intr;

    AAL_ASSERT_RET(status, CA_E_PARAM);

    if (device_id == 0){
        l2fe_intr.wrd = CA_NE_REG_READ(L2FE_GLB_INTERRUPT);
    }
#ifdef CONFIG_ARCH_CORTINA_G3HGU
    else if (device_id == 1){
        l2fe_intr.wrd = CA_8279_NE_REG_READ(L2FE_GLB_INTERRUPT);
    }
#endif
    status->lf_overflow          = l2fe_intr.bf.lf_overflow_intI       ;
    status->ple_hd_ff_overflow   = l2fe_intr.bf.ple_hd_ff_overflow_intI;
    status->port_mac_lmt_excd    = l2fe_intr.bf.port_mac_lmt_excd_intI ;
    status->l2e_hash_collision   = l2fe_intr.bf.l2e_hash_collision_intI;

    return CA_E_OK;
}

/** @aal_fdb_learn_status_get
 * Get the Learning update info FIFO status and
 * The number of the learning information entries cached in the
 * information FIFO
 */
ca_status_t aal_fdb_learn_status_get(
    CA_IN  ca_device_id_t              device_id,
    CA_IN  ca_uint32_t              port_id,
    CA_OUT aal_fdb_learn_status_t  *status)
{

    L2FE_L2E_INFO_FF_STS_REG_t  fifo_sts;
    L2FE_L2E_INFO_FF_CNT_0_t    cached_cnt;

    AAL_ASSERT_RET(port_id < 16, CA_E_PARAM);
    AAL_ASSERT_RET(status, CA_E_PARAM);

    if (device_id == 0){
        fifo_sts.wrd   = CA_NE_REG_READ(L2FE_L2E_INFO_FF_STS_REG);
        cached_cnt.wrd = CA_NE_REG_READ(L2FE_L2E_INFO_FF_CNT_0 + (port_id << 2));
    }
#ifdef CONFIG_ARCH_CORTINA_G3HGU
    else if (device_id == 1){
        fifo_sts.wrd   = CA_8279_NE_REG_READ(L2FE_L2E_INFO_FF_STS_REG);
        cached_cnt.wrd = CA_8279_NE_REG_READ(L2FE_L2E_INFO_FF_CNT_0 + (port_id << 2));
    }
#endif
    status->over_flow_flag   = (fifo_sts.bf.overflow_ind >> port_id) & 0x1;
    status->cached_entry_num = cached_cnt.bf.num;

    return CA_E_OK;

}

/* it need use indirect mode */
ca_status_t aal_fdb_get_by_hash_addr(
    CA_IN  ca_device_id_t              device_id,
    CA_IN  ca_uint32_t              hash_addr,
    CA_IN  ca_uint32_t              offset,
    CA_OUT aal_fdb_entry_cfg_t*     fdb_entry
)
{
    L2FE_L2E_FDB_ACCESS_t l2_fdb_access;
    L2FE_L2E_FDB_CMD_RETURN_t cmd_ret;
    L2FE_L2E_FDB_DATA0_t fdb_data0;
    L2FE_L2E_FDB_DATA1_t fdb_data1;
    L2FE_L2E_FDB_DATA2_t fdb_data2;
    L2FE_L2E_FDB_DATA3_t fdb_data3;
    ca_uint32 timeout = 0;

    AAL_ASSERT_RET(offset < 8, CA_E_PARAM);
    AAL_ASSERT_RET(hash_addr < 0x1ff, CA_E_PARAM);
    AAL_ASSERT_RET(fdb_entry, CA_E_PARAM);

    if (device_id == 0){
        l2_fdb_access.wrd = CA_NE_REG_READ(L2FE_L2E_FDB_ACCESS);
    }
#ifdef CONFIG_ARCH_CORTINA_G3HGU
    else if (device_id == 1){
        l2_fdb_access.wrd = CA_8279_NE_REG_READ(L2FE_L2E_FDB_ACCESS);
    }
#endif

    if (l2_fdb_access.bf.access) {
        L2_DEBUG("HW BUSY, wrd:0x%08x\r\n", l2_fdb_access.wrd);

        return CA_E_BUSY;
    }


    l2_fdb_access.wrd = 0x80000000 | (hash_addr << 3) | offset;

    __FDB_LOCK();

    if (device_id == 0){
        FDB_REG_WRITE(L2FE_L2E_FDB_CMD_RETURN, 0); /* init cmd return */
    }
#ifdef CONFIG_ARCH_CORTINA_G3HGU
    else if (device_id == 1){
        CA_8279_NE_REG_WRITE(0, L2FE_L2E_FDB_CMD_RETURN); /* init cmd return */
    }
#endif
    L2_DEBUG("access wrd:0x%08x\r\n", l2_fdb_access.wrd);

    if (device_id == 0){
        FDB_REG_WRITE(L2FE_L2E_FDB_ACCESS, l2_fdb_access.wrd);
    }
#ifdef CONFIG_ARCH_CORTINA_G3HGU
    else if (device_id == 1){
        CA_8279_NE_REG_WRITE(l2_fdb_access.wrd, L2FE_L2E_FDB_ACCESS);
    }
#endif
    do {
        if (device_id == 0){
            l2_fdb_access.wrd = CA_NE_REG_READ(L2FE_L2E_FDB_ACCESS);
        }
#ifdef CONFIG_ARCH_CORTINA_G3HGU
        else if (device_id == 1){
            l2_fdb_access.wrd = CA_8279_NE_REG_READ(L2FE_L2E_FDB_ACCESS);
        }
#endif
        if (timeout++ >= CA_AAL_FDB_TBL_OP_TIMEOUT) {
            __FDB_UNLOCK();
            L2_DEBUG("timeout\r\n");
            return CA_E_TIMEOUT;
        }

    } while (l2_fdb_access.bf.access);

    /* get return code and feedback */
    if (device_id == 0){
        cmd_ret.wrd = CA_NE_REG_READ(L2FE_L2E_FDB_CMD_RETURN);
    }
#ifdef CONFIG_ARCH_CORTINA_G3HGU
    else if (device_id == 1){
        cmd_ret.wrd = CA_8279_NE_REG_READ(L2FE_L2E_FDB_CMD_RETURN);
    }
#endif

    if (cmd_ret.bf.status == CA_AAL_FDB_ACCESS_HIT) {
        if (device_id == 0){
            fdb_data0.wrd = CA_NE_REG_READ(L2FE_L2E_FDB_DATA0);
            fdb_data1.wrd = CA_NE_REG_READ(L2FE_L2E_FDB_DATA1);
            fdb_data2.wrd = CA_NE_REG_READ(L2FE_L2E_FDB_DATA2);
            fdb_data3.wrd = CA_NE_REG_READ(L2FE_L2E_FDB_DATA3);
        }
#ifdef CONFIG_ARCH_CORTINA_G3HGU
        else if (device_id == 1){
            fdb_data0.wrd = CA_8279_NE_REG_READ(L2FE_L2E_FDB_DATA0);
            fdb_data1.wrd = CA_8279_NE_REG_READ(L2FE_L2E_FDB_DATA1);
            fdb_data2.wrd = CA_8279_NE_REG_READ(L2FE_L2E_FDB_DATA2);
            fdb_data3.wrd = CA_8279_NE_REG_READ(L2FE_L2E_FDB_DATA3);
        }
#endif
    //*WORD0 = cmd_ret.wrd;
    #if 0
        L2_DEBUG("output: data0:0x%08x\r\n", *DATA0);
        L2_DEBUG("        data1:0x%08x\r\n", *DATA1);
        L2_DEBUG("        data2:0x%08x\r\n", *DATA2);
        L2_DEBUG("        data3:0x%08x\r\n", *DATA3);
    #endif
    }


    __FDB_UNLOCK();

    L2_DEBUG("cmd_ret.wrd:0x%08x\r\n", cmd_ret.wrd);

    if (cmd_ret.bf.status == CA_AAL_FDB_ACCESS_HIT){
        __aal_fdb_data_2_entry(fdb_data1, fdb_data0, fdb_entry);
        __aal_fdb_data_2_mac(&fdb_data3, &fdb_data2, &fdb_data1, fdb_entry->mac);

        return CA_E_OK;
    }
    else
        return CA_E_NOT_FOUND;
}
#else//sd1 uboot for 98f
#endif//sd1 uboot for 98f

ca_status_t aal_fdb_lrn_fwd_ctrl_set(
    CA_IN  ca_device_id_t       device_id,
    CA_IN  lrn_fwd_ctrl_mask_t fwd_ctrl_mask,
    CA_IN  lrn_fwd_ctrl_t    *fwd_ctrl
)
{
    L2FE_PLC_LRN_FWD_CTRL_1_t fwd_ctrl_1;
    L2FE_PLC_LRN_FWD_CTRL_0_t fwd_ctrl_0;

    AAL_ASSERT_RET(fwd_ctrl, CA_E_PARAM);

#if 0//yocto
    __FDB_LOCK();
#else//sd1 uboot for 98f
#endif//sd1 uboot for 98f

    if (device_id == 0){
        fwd_ctrl_1.wrd = CA_NE_REG_READ(L2FE_PLC_LRN_FWD_CTRL_1);
        fwd_ctrl_0.wrd = CA_NE_REG_READ(L2FE_PLC_LRN_FWD_CTRL_0);
    }
#ifdef CONFIG_ARCH_CORTINA_G3HGU
    else if (device_id == 1){
        fwd_ctrl_1.wrd = CA_8279_NE_REG_READ(L2FE_PLC_LRN_FWD_CTRL_1);
        fwd_ctrl_0.wrd = CA_8279_NE_REG_READ(L2FE_PLC_LRN_FWD_CTRL_0);
    }
#endif
    if (fwd_ctrl_mask.bf.sa_hash_cllsn_fwd_ctrl)
        fwd_ctrl_1.bf.sa_hash_cllsn_fwd_ctrl = fwd_ctrl->hash_cllsn_mode;
    if (fwd_ctrl_mask.bf.sa_hash_cllsn_ldpid)
        fwd_ctrl_1.bf.sa_hash_cllsn_ldpid = fwd_ctrl->hash_cllsn_ldpid;

    if (fwd_ctrl_mask.bf.sa_limit_excd_fwd_ctrl)
        fwd_ctrl_1.bf.sa_limit_excd_fwd_ctrl = fwd_ctrl->limit_excd_mode;
    if (fwd_ctrl_mask.bf.sa_limit_excd_ldpid)
        fwd_ctrl_1.bf.sa_limit_excd_ldpid = fwd_ctrl->limit_excd_ldpid;

    if (fwd_ctrl_mask.bf.l2_mac_sw_lrn_ldpid)
        fwd_ctrl_0.bf.l2_mac_sw_lrn_ldpid = fwd_ctrl->l2_mac_sw_lrn_ldpid;

    if (fwd_ctrl_mask.bf.l2_mac_sw_lrn_cos)
        fwd_ctrl_0.bf.l2_mac_sw_lrn_cos = fwd_ctrl->l2_mac_sw_lrn_cos;
    if (fwd_ctrl_mask.bf.l2_mac_sw_lrn_keep_old_wan)
        fwd_ctrl_0.bf.l2_mac_sw_lrn_keep_old_wan = fwd_ctrl->l2_mac_sw_lrn_keep_old_wan;
    if (fwd_ctrl_mask.bf.l2_mac_sw_lrn_keep_old_lan)
        fwd_ctrl_0.bf.l2_mac_sw_lrn_keep_old_lan = fwd_ctrl->l2_mac_sw_lrn_keep_old_lan;
    if (fwd_ctrl_mask.bf.sa_miss_fwd)
        fwd_ctrl_0.bf.sa_miss_fwd = fwd_ctrl->sa_miss_fwd;
    if (fwd_ctrl_mask.bf.nonstd_sa_all_zero_fwd)
        fwd_ctrl_0.bf.nonstd_sa_all_zero_fwd = fwd_ctrl->nonstd_sa_all_zero_fwd;
    if (fwd_ctrl_mask.bf.nonstd_sa_all_one_fwd)
        fwd_ctrl_0.bf.nonstd_sa_all_one_fwd = fwd_ctrl->nonstd_sa_all_one_fwd;
    if (fwd_ctrl_mask.bf.nonstd_sa_b40_is_one_fwd)
        fwd_ctrl_0.bf.nonstd_sa_b40_is_one_fwd = fwd_ctrl->nonstd_sa_b40_is_one_fwd;
    if (fwd_ctrl_mask.bf.sm_static_entry_fwd)
        fwd_ctrl_0.bf.sm_static_entry_fwd = fwd_ctrl->sm_static_entry_fwd;
    if (fwd_ctrl_mask.bf.sm_tmp_entry_fwd)
        fwd_ctrl_0.bf.sm_tmp_entry_fwd = fwd_ctrl->sm_tmp_entry_fwd;
    if (fwd_ctrl_mask.bf.sm_sm_entry_fwd)
        fwd_ctrl_0.bf.sm_sm_entry_fwd = fwd_ctrl->sm_sm_entry_fwd;

    if (fwd_ctrl_mask.bf.sa_limit_excd_cos)
        fwd_ctrl_1.bf.sa_limit_excd_cos = fwd_ctrl->sa_limit_excd_cos;
    if (fwd_ctrl_mask.bf.sa_limit_excd_keep_old_wan)
        fwd_ctrl_1.bf.sa_limit_excd_keep_old_wan = fwd_ctrl->sa_limit_excd_keep_old_wan;
    if (fwd_ctrl_mask.bf.sa_limit_excd_keep_old_lan)
        fwd_ctrl_1.bf.sa_limit_excd_keep_old_lan = fwd_ctrl->sa_limit_excd_keep_old_lan;
    if (fwd_ctrl_mask.bf.sa_hash_cllsn_cos)
        fwd_ctrl_1.bf.sa_hash_cllsn_cos = fwd_ctrl->sa_hash_cllsn_cos;
    if (fwd_ctrl_mask.bf.sa_hash_cllsn_keep_old_wan)
        fwd_ctrl_1.bf.sa_hash_cllsn_keep_old_wan = fwd_ctrl->sa_hash_cllsn_keep_old_wan;
    if (fwd_ctrl_mask.bf.sa_hash_cllsn_keep_old_lan)
        fwd_ctrl_1.bf.sa_hash_cllsn_keep_old_lan = fwd_ctrl->sa_hash_cllsn_keep_old_lan;
    if (fwd_ctrl_mask.bf.swlrn_othr_pkt_fwd)
        fwd_ctrl_1.bf.swlrn_othr_pkt_fwd = fwd_ctrl->swlrn_othr_pkt_fwd;


    if (device_id == 0){
        CA_NE_REG_WRITE(fwd_ctrl_1.wrd, L2FE_PLC_LRN_FWD_CTRL_1);
        CA_NE_REG_WRITE(fwd_ctrl_0.wrd, L2FE_PLC_LRN_FWD_CTRL_0);
    }
#ifdef CONFIG_ARCH_CORTINA_G3HGU
    else if (device_id == 1){
        CA_8279_NE_REG_WRITE(fwd_ctrl_1.wrd, L2FE_PLC_LRN_FWD_CTRL_1);
        CA_8279_NE_REG_WRITE(fwd_ctrl_0.wrd, L2FE_PLC_LRN_FWD_CTRL_0);
    }
#endif

#if 0//yocto
    __FDB_UNLOCK();
#else//sd1 uboot for 98f
#endif//sd1 uboot for 98f

    L2_DEBUG("hash mode:%d, ldpid:%d;  limit excd: mode:%d, ldpid:%d, sw lrn ldpid:%d\r\n",
            fwd_ctrl->hash_cllsn_mode, fwd_ctrl->hash_cllsn_ldpid,
            fwd_ctrl->limit_excd_mode, fwd_ctrl->limit_excd_ldpid, fwd_ctrl->l2_mac_sw_lrn_ldpid);

    return CA_E_OK;
}

#if 0//yocto
ca_status_t aal_fdb_lrn_fwd_ctrl_get(
    CA_IN  ca_device_id_t       device_id,
    CA_OUT lrn_fwd_ctrl_t    *fwd_ctrl
)
{
    L2FE_PLC_LRN_FWD_CTRL_1_t fwd_ctrl_1;
    L2FE_PLC_LRN_FWD_CTRL_0_t fwd_ctrl_0;

    AAL_ASSERT_RET(fwd_ctrl, CA_E_PARAM);

    __FDB_LOCK();

    if (device_id == 0){
        fwd_ctrl_1.wrd = CA_NE_REG_READ(L2FE_PLC_LRN_FWD_CTRL_1);
        fwd_ctrl_0.wrd = CA_NE_REG_READ(L2FE_PLC_LRN_FWD_CTRL_0);
    }
#ifdef CONFIG_ARCH_CORTINA_G3HGU
    else if (device_id == 1){
        fwd_ctrl_1.wrd = CA_8279_NE_REG_READ(L2FE_PLC_LRN_FWD_CTRL_1);
        fwd_ctrl_0.wrd = CA_8279_NE_REG_READ(L2FE_PLC_LRN_FWD_CTRL_0);
    }
#endif
    __FDB_UNLOCK();

    fwd_ctrl->hash_cllsn_mode = fwd_ctrl_1.bf.sa_hash_cllsn_fwd_ctrl;
    fwd_ctrl->hash_cllsn_ldpid =  fwd_ctrl_1.bf.sa_hash_cllsn_ldpid;
    fwd_ctrl->limit_excd_mode = fwd_ctrl_1.bf.sa_limit_excd_fwd_ctrl;
    fwd_ctrl->limit_excd_ldpid =  fwd_ctrl_1.bf.sa_limit_excd_ldpid;
    fwd_ctrl->l2_mac_sw_lrn_ldpid =  fwd_ctrl_0.bf.l2_mac_sw_lrn_ldpid;

    fwd_ctrl->l2_mac_sw_lrn_cos          = fwd_ctrl_0.bf.l2_mac_sw_lrn_cos;
    fwd_ctrl->l2_mac_sw_lrn_keep_old_wan = fwd_ctrl_0.bf.l2_mac_sw_lrn_keep_old_wan;
    fwd_ctrl->l2_mac_sw_lrn_keep_old_lan = fwd_ctrl_0.bf.l2_mac_sw_lrn_keep_old_lan;
    fwd_ctrl->sa_miss_fwd                = fwd_ctrl_0.bf.sa_miss_fwd;
    fwd_ctrl->nonstd_sa_all_zero_fwd     = fwd_ctrl_0.bf.nonstd_sa_all_zero_fwd;
    fwd_ctrl->nonstd_sa_all_one_fwd      = fwd_ctrl_0.bf.nonstd_sa_all_one_fwd;
    fwd_ctrl->nonstd_sa_b40_is_one_fwd   = fwd_ctrl_0.bf.nonstd_sa_b40_is_one_fwd;
    fwd_ctrl->sm_static_entry_fwd        = fwd_ctrl_0.bf.sm_static_entry_fwd;
    fwd_ctrl->sm_tmp_entry_fwd           = fwd_ctrl_0.bf.sm_tmp_entry_fwd;
    fwd_ctrl->sm_sm_entry_fwd            = fwd_ctrl_0.bf.sm_sm_entry_fwd;
    fwd_ctrl->sa_limit_excd_cos          = fwd_ctrl_1.bf.sa_limit_excd_cos;
    fwd_ctrl->sa_limit_excd_keep_old_wan = fwd_ctrl_1.bf.sa_limit_excd_keep_old_wan;
    fwd_ctrl->sa_limit_excd_keep_old_lan = fwd_ctrl_1.bf.sa_limit_excd_keep_old_lan;
    fwd_ctrl->sa_hash_cllsn_cos          = fwd_ctrl_1.bf.sa_hash_cllsn_cos;
    fwd_ctrl->sa_hash_cllsn_keep_old_wan = fwd_ctrl_1.bf.sa_hash_cllsn_keep_old_wan;
    fwd_ctrl->sa_hash_cllsn_keep_old_lan = fwd_ctrl_1.bf.sa_hash_cllsn_keep_old_lan;
    fwd_ctrl->swlrn_othr_pkt_fwd         = fwd_ctrl_1.bf.swlrn_othr_pkt_fwd;


    L2_DEBUG("mode:%d, ldpid:%d;  limit excd:%d/%d, sw lrn ldpid:%d\r\n", fwd_ctrl->hash_cllsn_mode, fwd_ctrl->hash_cllsn_ldpid,
                    fwd_ctrl->limit_excd_mode, fwd_ctrl->limit_excd_ldpid, fwd_ctrl_0.bf.l2_mac_sw_lrn_ldpid);

    return CA_E_OK;
}

/*
Hash handling collision
1. Replace the oldest FDB entry in bucket with specified FDB entry
    Put FDB parameters in L2FE_L2E_FDB_DATA0-3
    L2FE_L2E_FDB_ACCESS[5] = 1 (delete oldest entry)
    Put FDB parameters in L2FE_L2E_FDB_DATA0-3
    L2FE_L2E_FDB_ACCESS[2:0] = 3'b101 (update FDB)
    L2FE_L2E_FDB_ACCESS[7:6] = 2'b01 (append)

*/
ca_status_t aal_fdb_hash_collision(
    CA_IN  ca_device_id_t              device_id,
    CA_IN  aal_fdb_entry_cfg_t      *fdb_entry
)
{
    ca_uint32_t full_ctrl = 0;
    aal_l2_vlan_default_cfg_t vlan_cfg;
    ca_status_t ret = CA_E_OK;

    L2_DEBUG("Enter\r\n");
    AAL_ASSERT_RET(fdb_entry, CA_E_PARAM);

    aal_fdb_limit_excd_fwd_ctrl_get(0, &full_ctrl);
    if (full_ctrl == SA_LRN_FWD_CTRL_DROP){
        L2_DEBUG("limit_excd_fwd_ctrl is drop\r\n");
        return CA_E_OK;
    }

    memset(&vlan_cfg, 0, sizeof(aal_l2_vlan_default_cfg_t));
    ret = aal_l2_vlan_default_cfg_get(device_id, &vlan_cfg);

    if (vlan_cfg.vlan_mode == AAL_L2_VLAN_MODE_SVL){
        fdb_entry->key_sc_ind = 0;
        fdb_entry->key_vid = 0;
    }

    ret = aal_fdb_entry_add_replace(device_id, fdb_entry);

    /* send event out */
    ret = ca_event_send(device_id, CA_EVENT_L2_FDB_HASH_COLLISION, (void *)fdb_entry, sizeof(aal_fdb_entry_cfg_t));

    return CA_E_OK;
}

ca_status_t aal_fdb_delete_oldest_entry_by_port(
    CA_IN  ca_device_id_t              device_id,
    CA_IN  ca_uint32_t              lpid
)
{
    aal_fdb_cpu_cmd_word4_msk_t  flush_mask;
    aal_fdb_flush_cfg_t          flush_cfg;
    ca_status_t ret = CA_E_OK;

    flush_mask.wrd = ~0U;

    /* static flag join comparion, is 0, dynamic */
    flush_mask.bf.static_flag = 0;
    flush_cfg.static_flag     = 0;


    flush_mask.bf.lpid  = 0;
    flush_cfg.lpid_high = lpid;
    flush_cfg.lpid_low  = lpid;

    ret = aal_fdb_scan_flush_field_set(device_id, flush_mask, &flush_cfg);

    if (CA_E_OK == ret)
        ret = aal_fdb_entry_flush_cmd(device_id, __FDB_TBL_OPCODE_FLUSH_OLDEST);

    L2_DEBUG("ret:%d, lpid:0x%x\r\n", ret, lpid);

    if (ret != CA_E_OK){
        L2_DEBUG("flush oldest base port fail\r\n");
        return ret;
    }

    return CA_E_OK;
}
/*
AC limit handling
1. Delete the oldest FDB entry for specified LPID
    L2FE_L2E_FDB_CPU_CMD_WORD0.[6:0]= lpid range comparison low end
    L2FE_L2E_FDB_CPU_CMD_WORD1.[5:0]= lpid range comparison high end
    L2FE_L2E_FDB_CPU_CMD_WORD4.[0] = lpid range compare mask.
    L2FE_L2E_FDB_ACCESS[2:0] = 3'b010 (update FDB)
    L2FE_L2E_FDB_ACCESS[7:6] = 2'b01 (flush)
    L2FE_L2E_FDB_ACCESS[5] = 1 (delete oldest entry)

2. Add new FDB entry
    Put FDB parameters in L2FE_L2E_FDB_DATA0-3
    L2FE_L2E_FDB_ACCESS[2:0] = 3'b101 (update FDB)
    L2FE_L2E_FDB_ACCESS[7:6] = 2'b01 (append)
    L2FE_L2E_FDB_ACCESS[5] = 1 (delete oldest entry, in case of collision)

*/
ca_status_t aal_fdb_mac_limit_exceed(
    CA_IN  ca_device_id_t              device_id,
    CA_IN  ca_uint32_t              lpid,
    CA_IN  aal_fdb_entry_cfg_t      *fdb_entry
)
{
    ca_status_t ret = CA_E_OK;
    ca_uint32_t full_ctrl;
    aal_l2_vlan_default_cfg_t vlan_cfg;

    L2_DEBUG("Enter\r\n");
    AAL_ASSERT_RET(fdb_entry, CA_E_PARAM);

    aal_fdb_limit_excd_fwd_ctrl_get(0, &full_ctrl);
    if (full_ctrl == SA_LRN_FWD_CTRL_DROP){
        L2_DEBUG("limit_excd_fwd_ctrl is drop\r\n");
        return CA_E_OK;
    }

    memset(&vlan_cfg, 0, sizeof(aal_l2_vlan_default_cfg_t));
    ret = aal_l2_vlan_default_cfg_get(device_id, &vlan_cfg);

    if (vlan_cfg.vlan_mode == AAL_L2_VLAN_MODE_SVL){
        fdb_entry->key_sc_ind = 0;
        fdb_entry->key_vid = 0;
    }

    ret = aal_fdb_delete_oldest_entry_by_port(device_id, lpid);

    if (ret != CA_E_OK){
        L2_DEBUG("flush oldest base port fail\r\n");
        return ret;
    }

    ret = aal_fdb_entry_add_replace(device_id, fdb_entry);

    if (ret != CA_E_OK){
        L2_DEBUG("add entry fail\r\n");
        return ret;
    }

    /* send event out */
    ret = ca_event_send(device_id, CA_EVENT_L2_FDB_LEARN_LIMIT, (void *)fdb_entry, sizeof(aal_fdb_entry_cfg_t));

    return CA_E_OK;
}

/*
3. SW learning
1. Add new FDB entry
    Put FDB parameters in L2FE_L2E_FDB_DATA0-3
    L2FE_L2E_FDB_ACCESS[2:0] = 3'b101 (update FDB)
    L2FE_L2E_FDB_ACCESS[7:6] = 2'b01 (append)

2. Check return code for add failure
    L2FE_L2E_FDB_CMD_RETURN.status[3:0] = 4'b1011 (entry updated; However port MAC limit check failure detected)

3. In case of MAC limit failure, follow MAC limit handling to delete one entry (no need to add again)

*/
ca_status_t aal_fdb_sw_learning(
    CA_IN  ca_device_id_t              device_id,
    CA_IN  aal_fdb_entry_cfg_t      *fdb_entry
)
{
    ca_status_t ret = CA_E_OK;
    L2FE_L2E_FDB_CMD_RETURN_t cmd_ret;
    aal_l2_vlan_default_cfg_t vlan_cfg;

    L2_DEBUG("Enter\r\n");
    AAL_ASSERT_RET(fdb_entry, CA_E_PARAM);

    memset(&vlan_cfg, 0, sizeof(aal_l2_vlan_default_cfg_t));
    ret = aal_l2_vlan_default_cfg_get(device_id, &vlan_cfg);

    if (vlan_cfg.vlan_mode == AAL_L2_VLAN_MODE_SVL){
        fdb_entry->key_sc_ind = 0;
        fdb_entry->key_vid = 0;
    }

    ret = aal_fdb_entry_add(device_id, fdb_entry);

    if (device_id == 0){
        cmd_ret.wrd = CA_NE_REG_READ(L2FE_L2E_FDB_CMD_RETURN);
    }
#ifdef CONFIG_ARCH_CORTINA_G3HGU
    else if (device_id == 1){
        cmd_ret.wrd = CA_8279_NE_REG_READ(L2FE_L2E_FDB_CMD_RETURN);
    }
#endif
    if (cmd_ret.bf.status == CA_AAL_FDB_ACCESS_MAC_LIMIT){
        /* mac limit check failure */
        L2_DEBUG("mac limit check failure\r\n");
        ret = aal_fdb_delete_oldest_entry_by_port(device_id, fdb_entry->port_id);
    }

    L2_DEBUG("ret:%d, port_id:%d\r\n", ret, fdb_entry->port_id);
    return ret;
}


/*
cpu_special_packet_lrn_ena
To CPU packet learning enable/disable: this is supported in G3 HW (using special packet learning control)
L2FE_PLA_SPB_DATA. spb_learn_dis (in most cases, to CPU packets are trapped and forwarded by special packet HW module)
 */

ca_status_t aal_fdb_cpu_pkt_lrn_set(
    CA_IN ca_device_id_t device_id,
    CA_IN ca_boolean_t flag
)
{
    ca_status_t                 ret = CA_E_OK;
    ca_uint32_t                 spb_idx = 0;
    aal_l2_specpkt_type_t       pkt_type;
    aal_l2_specpkt_ctrl_mask_t  l2_ctrl_mask;
    aal_l2_specpkt_ctrl_t       l2_ctrl;

    l2_ctrl_mask.u32 = 0;

    l2_ctrl_mask.s.learn_dis = 1;

    l2_ctrl.learn_dis = !flag;

    for (spb_idx = 0; spb_idx <= AAL_L2_SPECPKT_TBL_IDX_MAX; spb_idx++) {
        for (pkt_type = AAL_L2_SPECPKT_TYPE_NORMAL; pkt_type <= AAL_L2_SPECPKT_TYPE_MAX; pkt_type++){
            ret = aal_l2_specpkt_ctrl_set(device_id, spb_idx,
                pkt_type,
                l2_ctrl_mask, &l2_ctrl);

            if (CA_E_OK != ret) {
                L2_DEBUG("Invoke aal_l2_specpkt_ctrl_set return %#x.\n", ret);
                return ret;
            }
        }
    }
    L2_DEBUG("ret:%d, flag:%d\r\n", ret, flag);

    return ret;

}


ca_status_t aal_fdb_cpu_pkt_lrn_get(
    CA_IN ca_device_id_t device_id,
    CA_OUT ca_boolean_t *flag
)
{
    ca_status_t                 ret = CA_E_OK;
    aal_l2_specpkt_ctrl_t       l2_pkt_cfg;

    AAL_ASSERT_RET(flag, CA_E_PARAM);

    memset(&l2_pkt_cfg, 0, sizeof(l2_pkt_cfg));
    ret = aal_l2_specpkt_ctrl_get(device_id,
        0, 0, &l2_pkt_cfg);

    *flag = !l2_pkt_cfg.learn_dis;

    L2_DEBUG("ret:%d, flag:%d\r\n", ret, *flag);

    return ret;

}

ca_status_t aal_fdb_limit_excd_fwd_ctrl_get(
    CA_IN ca_device_id_t device_id,
    CA_OUT ca_uint32_t *lrn_ctrl
)
{
    ca_status_t                 ret = CA_E_OK;
    L2FE_PLC_LRN_FWD_CTRL_1_t   plc_lrn_fwd_ctrl;

    AAL_ASSERT_RET(lrn_ctrl, CA_E_PARAM);

    if (device_id == 0){
        plc_lrn_fwd_ctrl.wrd = CA_NE_REG_READ(L2FE_PLC_LRN_FWD_CTRL_1);
    }
#ifdef CONFIG_ARCH_CORTINA_G3HGU
    else if (device_id == 1){
        plc_lrn_fwd_ctrl.wrd = CA_8279_NE_REG_READ(L2FE_PLC_LRN_FWD_CTRL_1);
    }
#endif
    *lrn_ctrl = plc_lrn_fwd_ctrl.bf.sa_limit_excd_fwd_ctrl;
    L2_DEBUG("ret:%d, lrn_ctrl:%d\r\n", ret, *lrn_ctrl);

    return ret;
}
ca_status_t aal_fdb_addr_delete_all(
    CA_IN ca_device_id_t             device_id,
    CA_IN ca_uint32_t flag
)
{
    aal_fdb_cpu_cmd_word4_msk_t  mask;
    aal_fdb_flush_cfg_t cfg;
    ca_status_t ret;

    mask.wrd = 0xffffffff;
    memset((void *)&cfg, 0, sizeof(aal_fdb_flush_cfg_t));

    aal_fdb_cmp_cfg_load(&cfg);

    if (flag == CA_L2_ADDR_OP_STATIC){
        mask.bf.static_flag = 0;  /* join */
        cfg.static_flag = 1;
    }
    else if (flag == CA_L2_ADDR_OP_DYNAMIC){
        mask.bf.static_flag = 0;
        cfg.static_flag = 0;
    }
    else if (flag == CA_L2_ADDR_OP_BOTH){
    /* flush all, include static */
        mask.bf.static_flag = 1;
        cfg.static_flag = 0;
    }

    aal_fdb_scan_flush_field_set(0, mask, &cfg);

    ret = aal_fdb_entry_flush(0);

    if (ret == CA_E_OK && (flag == CA_L2_ADDR_OP_STATIC || flag == CA_L2_ADDR_OP_BOTH))
        ret = aal_fdb_tbl_del_all();

    L2_DEBUG("ret:%d\r\n", ret);

    return ret;
}
#else//sd1 uboot for 98f
#endif//sd1 uboot for 98f

#if 0
ca_status_t aal_fdb_hash_cllsn_update(
    CA_IN  ca_device_id_t              device_id,
    CA_IN  aal_fdb_entry_cfg_t      *fdb_entry
)
{
    AAL_ASSERT_RET(fdb_entry, CA_E_PARAM);
    return aal_fdb_entry_add(device_id, fdb_entry);
}
//#else
ca_status_t aal_fdb_hash_cllsn_update(
    CA_IN  ca_device_id_t              device_id,
    CA_IN  aal_fdb_entry_cfg_t      *fdb_entry
)
{
    L2FE_L2E_HASH_CLLSN_STTS_t hash_cllsn_stts;
    ca_uint32_t             hash_addr;
    ca_uint32_t             oldest = (ca_uint32_t)-1;
    aal_fdb_entry_cfg_t     fdb_entry, oldest_entry;
    int i;
    ca_status_t ret = CA_E_OK;

    AAL_ASSERT_RET(fdb_entry, CA_E_PARAM);

    hash_cllsn_stts.wrd =  CA_NE_REG_READ(L2FE_L2E_HASH_CLLSN_STTS);

    if (!hash_cllsn_stts.bf.cllsn_det){

        ca_printf("no collision detected!\r\n");
        return CA_E_OK;
    }

    memset(&fdb_entry, 0, sizeof(aal_fdb_entry_cfg_t));
    memset(&oldest_entry, 0, sizeof(aal_fdb_entry_cfg_t));

    hash_addr = hash_cllsn_stts.bf.cllsn_addr;

    /* change l2e cpu to indirect mode*/
    __aal_l2e_cpu_op_mode(CA_AAL_L2E_INDIRECT_MODE);

    for (i = 0; i < 8; i++){
        ret = aal_fdb_get_by_hash_addr(device_id, hash_addr, i, &fdb_entry);
        if (ret != CA_E_OK){
            ca_printf("hash entry not full!\r\n");
            break;
        }

        L2_DEBUG("i:%d, mac:0x%x:%x:%x:%x:%x:%x, key_vid:%d, sc_ind:%d, dot1p:%d, lrnt_vid:%d, sa_permit:%d, da_permit:%d, static_flag:%d, aging_sts:%d, mc:%d, pid:%d\r\n",
                    i + 1,
                    fdb_entry.mac[0], fdb_entry.mac[1], fdb_entry.mac[2],
                    fdb_entry.mac[3], fdb_entry.mac[4], fdb_entry.mac[5],
                    fdb_entry.key_vid    ,
                    fdb_entry.key_sc_ind ,
                    fdb_entry.dot1p      ,
                    fdb_entry.lrnt_vid   ,
                    fdb_entry.sa_permit  ,
                    fdb_entry.da_permit  ,
                    fdb_entry.static_flag,
                    fdb_entry.aging_sts  ,
                    fdb_entry.mc_flag    ,
                    fdb_entry.port_id);

        if (fdb_entry.aging_sts < oldest){ /* find minimum aging entry */
            memcpy(&oldest_entry, &fdb_entry, sizeof(aal_fdb_entry_cfg_t));
            oldest = fdb_entry.aging_sts;
        }
    }

    __aal_l2e_cpu_op_mode(CA_AAL_L2E_CMD_MODE);

    if (ret == CA_E_OK){
        aal_fdb_entry_del(device_id, &oldest_entry);
    }

    /* TODO, add new entry */

    return ret;
}
#endif


#if 1
/* aal cli debug purpose */
ca_mac_addr_t    dbg_mac = {0x00, 0xbe, 0xef, 0x00, 0x00, 0x01};
ca_uint32_t dbg_mac_cnt = 0;
ca_uint32_t dbg_vid = 10;

#if 0//yocto
ca_status_t aal_fdb_dbg_add_get(
    CA_IN ca_device_id_t             device_id,
    CA_IN ca_uint32_t             port_id,
    CA_IN ca_uint32_t             static_flag
)
{
    ca_status_t ret;
    aal_fdb_entry_cfg_t fdb_entry;


    memset((void *)&fdb_entry, 0, sizeof(aal_fdb_entry_cfg_t));
    dbg_mac[4] += dbg_mac_cnt / 256;
    dbg_mac[5] += dbg_mac_cnt % 256;
    dbg_mac_cnt++;

    memcpy(fdb_entry.mac, dbg_mac, sizeof(ca_mac_addr_t));
    fdb_entry.key_vid = dbg_vid;
    fdb_entry.key_sc_ind = 1;
    fdb_entry.lrnt_vid = dbg_vid;
    fdb_entry.lrnt_sc_ind = 1;
    fdb_entry.dot1p = 5;
    fdb_entry.static_flag = static_flag;
    fdb_entry.sa_permit = 1;
    fdb_entry.da_permit = 1;
    fdb_entry.port_id = port_id;
    fdb_entry.valid = 1;

    fdb_entry.tmp_flag = 0;
    fdb_entry.aging_sts = 255;
    fdb_entry.mc_flag = 0;
    fdb_entry.aged_flag = 0;

    L2_DEBUG("mac:0x%x:%x:%x:%x:%x:%x, vid:%d, static:%d\r\n", dbg_mac[0], dbg_mac[1], dbg_mac[2], dbg_mac[3], dbg_mac[4], dbg_mac[5],
            dbg_vid, static_flag);

    ret = aal_fdb_entry_add(0, &fdb_entry);

    L2_DEBUG("ret:%d\r\n", ret);
    L2_DEBUG("key_vid:%d, sc_ind:%d, dot1p:%d, lrnt_vid:%d, sa_permit:%d, da_permit:%d, static_flag:%d, aging_sts:%d, mc:%d, pid:%d\r\n",
            fdb_entry.key_vid    ,
            fdb_entry.key_sc_ind ,
            fdb_entry.dot1p      ,
            fdb_entry.lrnt_vid   ,
            fdb_entry.sa_permit  ,
            fdb_entry.da_permit  ,
            fdb_entry.static_flag,
            fdb_entry.aging_sts  ,
            fdb_entry.mc_flag    ,
            fdb_entry.port_id);

    return CA_E_OK;
}
#else//sd1 uboot for 98f
#endif//sd1 uboot for 98f

#if 0//yocto
ca_status_t aal_fdb_entry_addr_get(
    CA_IN ca_device_id_t            device_id,
    CA_IN ca_mac_addr_t          mac,
    CA_IN ca_uint16_t            key_vid
)
{
    aal_fdb_entry_cfg_t  fdb_entry;
    ca_status_t ret = CA_E_OK;

    memset((void *)&fdb_entry, 0, sizeof(aal_fdb_entry_cfg_t));

    L2_DEBUG("vid:%d, mac:0x%x:%x:%x:%x:%x:%x\r\n", key_vid, mac[0], mac[1], mac[2],
                                mac[3], mac[4], mac[5]);

    ret = aal_fdb_entry_get(0, mac, key_vid, &fdb_entry);

    L2_DEBUG("ret:%d\r\n", ret);
    if (ret == CA_E_OK){
        L2_DEBUG("key_vid:%d, sc_ind:%d, dot1p:%d, lrnt_vid:%d, sa_permit:%d, da_permit:%d, static_flag:%d, aging_sts:%d, mc:%d, pid:%d\r\n",
                fdb_entry.key_vid    ,
                fdb_entry.key_sc_ind ,
                fdb_entry.dot1p      ,
                fdb_entry.lrnt_vid   ,
                fdb_entry.sa_permit  ,
                fdb_entry.da_permit  ,
                fdb_entry.static_flag,
                fdb_entry.aging_sts  ,
                fdb_entry.mc_flag    ,
                fdb_entry.port_id);
    }

    return ret;
}
ca_status_t aal_fdb_entry_addr_set (
    CA_IN ca_device_id_t             device_id,
    CA_IN ca_mac_addr_t           mac,
    CA_IN ca_uint16_t             key_vid,
    CA_IN aal_fdb_entry_cfg_t *fdb_entry
)
{
    ca_status_t ret;
    /*
    aal_fdb_entry_cfg_t fdb_entry;

    L2_ASSERT_RET(addr != NULL, CA_E_PARAM);

    memset(&fdb_entry, 0, sizeof(aal_fdb_entry_cfg_t));

    memcpy(fdb_entry.mac, addr->mac_addr, sizeof(ca_mac_addr_t));
    fdb_entry.key_vid = addr->key_vid;
    fdb_entry.lrnt_vid = addr->vid;
    fdb_entry.dot1p = addr->dot1p;
    fdb_entry.static_flag = addr->static_flag;
    fdb_entry.sa_permit = addr->sa_permit;
    fdb_entry.da_permit = addr->da_permit;
    fdb_entry.port_id = CA_PORT_INDEX_GET(addr->port_id);
    //fdb_entry. = addr->mc_group_id;
    fdb_entry.aging_sts = addr->aging_status;
    */
    ret = aal_fdb_entry_add(0, fdb_entry);

    L2_DEBUG("ret:%d\r\n", ret);
    L2_DEBUG("key_vid:%d, sc_ind:%d, dot1p:%d, lrnt_vid:%d, sa_permit:%d, da_permit:%d, static_flag:%d, aging_sts:%d, mc:%d, pid:%d\r\n",
            fdb_entry->key_vid    ,
            fdb_entry->key_sc_ind ,
            fdb_entry->dot1p      ,
            fdb_entry->lrnt_vid   ,
            fdb_entry->sa_permit  ,
            fdb_entry->da_permit  ,
            fdb_entry->static_flag,
            fdb_entry->aging_sts  ,
            fdb_entry->mc_flag    ,
            fdb_entry->port_id);

    return CA_E_OK;
}

ca_status_t aal_fdb_addr_delete_get(
    CA_IN ca_device_id_t             device_id,
    CA_IN aal_fdb_entry_cfg_t *fdb_entry
)
{
    ca_status_t ret;
    /*
    aal_fdb_entry_cfg_t fdb_entry;

    L2_ASSERT_RET(addr != NULL, CA_E_PARAM);

    memset(&fdb_entry, 0, sizeof(aal_fdb_entry_cfg_t));

    memcpy(fdb_entry.mac, addr->mac_addr, sizeof(ca_mac_addr_t));
    fdb_entry.key_vid = addr->key_vid;
    fdb_entry.lrnt_vid = addr->vid;
    fdb_entry.dot1p = addr->dot1p;
    fdb_entry.static_flag = addr->static_flag;
    fdb_entry.sa_permit = addr->sa_permit;
    fdb_entry.da_permit = addr->da_permit;
    fdb_entry.port_id = CA_PORT_INDEX_GET(addr->port_id);
    //fdb_entry. = addr->mc_group_id;
    fdb_entry.aging_sts = addr->aging_status;
    */
    ret = aal_fdb_entry_del(0, fdb_entry);

    L2_DEBUG("ret:%d\r\n", ret);
    L2_DEBUG("key_vid:%d, sc_ind:%d, dot1p:%d, lrnt_vid:%d, sa_permit:%d, da_permit:%d, static_flag:%d, aging_sts:%d, mc:%d, pid:%d\r\n",
            fdb_entry->key_vid    ,
            fdb_entry->key_sc_ind ,
            fdb_entry->dot1p      ,
            fdb_entry->lrnt_vid   ,
            fdb_entry->sa_permit  ,
            fdb_entry->da_permit  ,
            fdb_entry->static_flag,
            fdb_entry->aging_sts  ,
            fdb_entry->mc_flag    ,
            fdb_entry->port_id);

    return CA_E_OK;
}

ca_status_t aal_fdb_addr_delete_all_get (
    CA_IN ca_device_id_t             device_id,
    CA_IN ca_uint32_t flag
)
{
    return aal_fdb_addr_delete_all(device_id, flag);
}
#else//sd1 uboot for 98f
#endif//sd1 uboot for 98f

#define __FDB_IS_VALID(fdb) ((fdb).valid && ((fdb).aging_sts > 0))


#define AAL_FDB_FLAG_DYNAMIC 0
#define AAL_FDB_FLAG_STATIC  1
#define AAL_FDB_FLAG_BOTH    2

ca_status_t aal_fdb_table_get(
    CA_IN ca_device_id_t             device_id,
    CA_IN ca_uint32_t             flag
)
{
    aal_fdb_entry_cfg_t  fdb_entry;
    ca_uint32_t next_addr = 0, start_addr = 0;
    ca_status_t ret = CA_E_OK;
    ca_uint32_t item_cnt = 0;
    ca_uint32_t find_cnt = 0;
    aal_fdb_flush_cfg_t cfg;
    aal_fdb_cpu_cmd_word4_msk_t  mask;

    memset((void *)&cfg, 0, sizeof(aal_fdb_flush_cfg_t));

    aal_fdb_cmp_cfg_load(&cfg);

    mask.wrd = 0xffffffff;
    aal_fdb_scan_flush_field_set(0, mask, &cfg);

    memset((void *)&fdb_entry, 0, sizeof(aal_fdb_entry_cfg_t));

    while (item_cnt++ < AAL_FDB_MAX_NUM) {

        ret = aal_fdb_scan(device_id,
                start_addr, &next_addr,
                &fdb_entry);

        if (ret != CA_E_OK ) {
            L2_DEBUG("search end\r\n");
            break;
        }

        if (__FDB_IS_VALID(fdb_entry)) {
            if (0 == find_cnt) {

                ca_printf("    lrn-vid  key-vid                  static  mc   SA     DA             \n");
                ca_printf("port (s/c)   (s/c)          MAC        flag  flag  permit permit age addr\n");
                ca_printf("---- ------- ------- ----------------- ----- ---- ------ ------  --- -----\n");
            }

            find_cnt++;

            if ((flag == AAL_FDB_FLAG_DYNAMIC && !fdb_entry.static_flag) ||
                (flag == AAL_FDB_FLAG_STATIC && fdb_entry.static_flag) ||
                (flag == AAL_FDB_FLAG_BOTH)) {
                ca_printf("%-4u %4u(%-1s) %4u(%-1s) %02x:%02x:%02x:%02x:%02x:%02x %-5u  %-3u   %-4u   %-4u  %-3u %u\n",
                    fdb_entry.port_id,
                    fdb_entry.lrnt_vid,
                    fdb_entry.lrnt_sc_ind ? "s" : "c",
                    fdb_entry.key_vid ,
                    fdb_entry.key_sc_ind ? "s" : "c",
                    fdb_entry.mac[0], fdb_entry.mac[1], fdb_entry.mac[2], fdb_entry.mac[3], fdb_entry.mac[4], fdb_entry.mac[5],
                    fdb_entry.static_flag,
                    fdb_entry.mc_flag,
                    fdb_entry.sa_permit  ,
                    fdb_entry.da_permit  ,
                    FDB_AGING_TIME(fdb_entry.aging_sts),
                    next_addr - 1
                    );

                if (0 == (find_cnt % 15)) {
//                    ca_sleep(1);
                    mdelay(1);
                }

            }
        }

        if (start_addr == next_addr){
            L2_DEBUG("start_addr == next_addr, LAST valid entry\r\n");
            break;
        }

        start_addr = next_addr;

    }

    ca_printf("\r\nTotal FDB count : %u\n", find_cnt);

    return CA_E_OK;

}

ca_status_t aal_fdb_disable_table_aging(
        CA_IN ca_device_id_t device_id,
        CA_IN bool enable
)
{
        L2FE_L2E_CTRL_t l2fe_l2e_ctrl;

        l2fe_l2e_ctrl.wrd = CA_NE_REG_READ(L2FE_L2E_CTRL);
        l2fe_l2e_ctrl.bf.aging_en = enable;
        FDB_REG_WRITE(L2FE_L2E_CTRL, l2fe_l2e_ctrl.wrd);

        return CA_E_OK;
}

ca_status_t aal_fdb_enable_table_indirect_access(
        CA_IN ca_device_id_t device_id,
        CA_IN bool enable
)
{
        L2FE_L2E_CTRL_t l2fe_l2e_ctrl;

        l2fe_l2e_ctrl.wrd = CA_NE_REG_READ(L2FE_L2E_CTRL);
        l2fe_l2e_ctrl.bf.cpu_opt_mode = ~enable;
        FDB_REG_WRITE(L2FE_L2E_CTRL, l2fe_l2e_ctrl.wrd);

        return CA_E_OK;
}

#if 0
//+Andy
#define LOOP_TIMES 100
ca_status_t aal_fdb_sram_rw_test(
        CA_IN ca_device_id_t             device_id,
        CA_IN ca_uint32       		reg_value
)
{
        L2FE_L2E_CTRL_t l2fe_l2e_ctrl;

        L2FE_L2E_FDB_ACCESS_t l2_fdb_access;
        L2FE_L2E_FDB_DATA0_t fdb_data0;
        L2FE_L2E_FDB_DATA1_t fdb_data1;
        L2FE_L2E_FDB_DATA2_t fdb_data2;
        L2FE_L2E_FDB_DATA3_t fdb_data3;
        ca_uint32_t i, temp;

        ca_uint32_t item_cnt = 0;
        ca_uint32_t error_cnt = 0;
        ca_uint32_t data3_mask = 0x7;

#if 1	
	ca_printf("User-defined reg_value = 0x%x\n", reg_value);
#endif
	temp = reg_value;
	
	//disable fdb aging
	aal_fdb_disable_table_aging(device_id, DISABLE);
	
	//enable
	aal_fdb_enable_table_indirect_access(device_id, ENABLE);

	while (item_cnt < AAL_FDB_MAX_NUM) {
	
		for(i = 0; i < LOOP_TIMES; i++)
		{		
#if 0//test
ca_printf("\t[%s(%d)]\n", __func__, __LINE__);
ca_printf("\tWritten value = %#x\t[%s(%d)]\n", (unsigned int)temp, __func__, __LINE__);
ca_printf("\tWritten value = %#x\t[%s(%d)]\n", (ca_uint32)temp, __func__, __LINE__);
ca_printf("\tWritten value = %#x\t[%s(%d)]\n", temp, __func__, __LINE__);
ca_printf("\tWritten value = 0x%x\t[%s(%d)]\n", temp, __func__, __LINE__);
#endif//test
				FDB_REG_WRITE(L2FE_L2E_FDB_DATA0, temp);
				FDB_REG_WRITE(L2FE_L2E_FDB_DATA1, temp);
				FDB_REG_WRITE(L2FE_L2E_FDB_DATA2, temp);
				FDB_REG_WRITE(L2FE_L2E_FDB_DATA3, temp);

				l2_fdb_access.wrd = 0xc0000000 | item_cnt;
				FDB_REG_WRITE(L2FE_L2E_FDB_ACCESS, l2_fdb_access.wrd);
				
				do{
					l2_fdb_access.wrd = CA_NE_REG_READ(L2FE_L2E_FDB_ACCESS);
				} while (l2_fdb_access.bf.access);
				
				//read out data
				l2_fdb_access.wrd = 0x80000000 | item_cnt;
				FDB_REG_WRITE(L2FE_L2E_FDB_ACCESS, l2_fdb_access.wrd);
				
				do{
					l2_fdb_access.wrd = CA_NE_REG_READ(L2FE_L2E_FDB_ACCESS);
				} while (l2_fdb_access.bf.access);
				
				fdb_data3.wrd = CA_NE_REG_READ(L2FE_L2E_FDB_DATA3);
				fdb_data2.wrd = CA_NE_REG_READ(L2FE_L2E_FDB_DATA2);
				fdb_data1.wrd = CA_NE_REG_READ(L2FE_L2E_FDB_DATA1);
				fdb_data0.wrd = CA_NE_REG_READ(L2FE_L2E_FDB_DATA0);
				
				if( (fdb_data3.wrd != (temp & data3_mask)) ||
					(fdb_data2.wrd != temp) ||
					(fdb_data1.wrd != temp) ||
					(fdb_data0.wrd != temp)){
				
				
					ca_printf("Write fail occurred at fdb entry = %d, times = %d \n", item_cnt, i);
					ca_printf("Written value = 0x%x \n", temp);
					ca_printf("Data 3~0 of this fdb entry:\n");
					ca_printf("L2FE_L2E_FDB_DATA3 = 0x%x \n", fdb_data3.wrd);
					ca_printf("L2FE_L2E_FDB_DATA2 = 0x%x \n", fdb_data2.wrd);
					ca_printf("L2FE_L2E_FDB_DATA1 = 0x%x \n", fdb_data1.wrd);
					ca_printf("L2FE_L2E_FDB_DATA0 = 0x%x \n\n", fdb_data0.wrd);
					
					error_cnt ++;
					break;
				}
				
				if((i%2) == 1)
					temp = (~reg_value);
		}
		item_cnt++;
		
	}
	
#if 0
	//enable fdb aging
	l2_fdb_access.wrd = CA_NE_REG_READ(L2FE_L2E_CTRL);	
	l2_fdb_access.wrd |= 0x1;
	FDB_REG_WRITE(L2FE_L2E_CTRL, l2_fdb_access.wrd);
#endif	

	if(error_cnt == 0)
		return CA_E_OK;
	else
		return error_cnt;
}
#endif

#if 1
ca_status_t aal_fdb_sram_rw_test(
	CA_IN ca_device_id_t             device_id,
    CA_IN ca_uint32       		reg_value
)
{
		L2FE_L2E_CTRL_t l2fe_l2e_ctrl;
	
	L2FE_L2E_FDB_ACCESS_t l2_fdb_access;
	L2FE_L2E_FDB_DATA0_t fdb_data0;
    L2FE_L2E_FDB_DATA1_t fdb_data1;
    L2FE_L2E_FDB_DATA2_t fdb_data2;
    L2FE_L2E_FDB_DATA3_t fdb_data3;
	ca_uint32_t i, temp_3, temp_2, temp_1, temp_0;
	
	ca_uint32_t item_cnt = 0;
	ca_uint32_t error_cnt = 0;
	ca_uint32_t data3_mask = 0x7;
	
	//disable fdb aging
	aal_fdb_disable_table_aging(device_id, DISABLE);
	
	//enable
	aal_fdb_enable_table_indirect_access(device_id, ENABLE);
	
	temp_3 = reg_value;
	temp_2 = reg_value;
	temp_1 = reg_value;
	temp_0 = reg_value;

	printf("Start 1st round test...\n\n");
	
	while (item_cnt < AAL_FDB_MAX_NUM) {
	
		FDB_REG_WRITE(L2FE_L2E_FDB_DATA0, temp_0);
		FDB_REG_WRITE(L2FE_L2E_FDB_DATA1, temp_1);
		FDB_REG_WRITE(L2FE_L2E_FDB_DATA2, temp_2);
		FDB_REG_WRITE(L2FE_L2E_FDB_DATA3, temp_3);
		
		l2_fdb_access.wrd = 0xc0000000 | item_cnt;
		FDB_REG_WRITE(L2FE_L2E_FDB_ACCESS, l2_fdb_access.wrd);
		
		do{
			l2_fdb_access.wrd = CA_NE_REG_READ(L2FE_L2E_FDB_ACCESS);
		} while (l2_fdb_access.bf.access);
		
		//printf("reg_value = 0x%x \n",temp);
		
		temp_3 = (temp_3 << 1) | (temp_2 & 0x80000000) >> 31 ;
		temp_2 = (temp_2 << 1) | (temp_1 & 0x80000000) >> 31 ;
		temp_1 = (temp_1 << 1) | (temp_0 & 0x80000000) >> 31 ;
		temp_0 = (temp_0 << 1);
			
		if(((item_cnt+1)%99) == 0)
		{
			temp_3 = reg_value;
			temp_2 = reg_value;
			temp_1 = reg_value;
			temp_0 = reg_value;
		}
		
		item_cnt++;
	}
	
	
	item_cnt = 0;
	temp_3 = reg_value;
	temp_2 = reg_value;
	temp_1 = reg_value;
	temp_0 = reg_value;;
	//ca_printf("Start checking the correctness of each entry...\n");

	while (item_cnt < AAL_FDB_MAX_NUM) {
	
		l2_fdb_access.wrd = 0x80000000 | item_cnt;
		
		FDB_REG_WRITE(L2FE_L2E_FDB_ACCESS, l2_fdb_access.wrd);
		
		do{
			l2_fdb_access.wrd = CA_NE_REG_READ(L2FE_L2E_FDB_ACCESS);
		} while (l2_fdb_access.bf.access);
		
		fdb_data3.wrd = CA_NE_REG_READ(L2FE_L2E_FDB_DATA3);
		fdb_data2.wrd = CA_NE_REG_READ(L2FE_L2E_FDB_DATA2);
		fdb_data1.wrd = CA_NE_REG_READ(L2FE_L2E_FDB_DATA1);
		fdb_data0.wrd = CA_NE_REG_READ(L2FE_L2E_FDB_DATA0);

		if( (fdb_data3.wrd != (temp_3 & data3_mask)) ||
			(fdb_data2.wrd != temp_2) ||
			(fdb_data1.wrd != temp_1) ||
			(fdb_data0.wrd != temp_0)){
		
			ca_printf("Write fail occurred at fdb entry %d \n", item_cnt);
			ca_printf("Tested reg_value for DATA3 = 0x%x \n", (temp_3 & data3_mask));
			ca_printf("Tested reg_value for DATA2 = 0x%x \n", temp_2);
			ca_printf("Tested reg_value for DATA1 = 0x%x \n", temp_1);
			ca_printf("Tested reg_value for DATA0 = 0x%x \n", temp_0);
			ca_printf("Data 3~0 of this fdb entry:\n");
			ca_printf("L2FE_L2E_FDB_DATA3 = 0x%x \n", fdb_data3.wrd);
			ca_printf("L2FE_L2E_FDB_DATA2 = 0x%x \n", fdb_data2.wrd);
			ca_printf("L2FE_L2E_FDB_DATA1 = 0x%x \n", fdb_data1.wrd);
			ca_printf("L2FE_L2E_FDB_DATA0 = 0x%x \n\n", fdb_data0.wrd);
	
			error_cnt ++;
		}
		
		temp_3 = (temp_3 << 1) | (temp_2 & 0x80000000) >> 31 ;
		temp_2 = (temp_2 << 1) | (temp_1 & 0x80000000) >> 31 ;
		temp_1 = (temp_1 << 1) | (temp_0 & 0x80000000) >> 31 ;
		temp_0 = (temp_0 << 1);
	
		if(((item_cnt+1)%99) == 0)
		{
			temp_3 = reg_value;
			temp_2 = reg_value;
			temp_1 = reg_value;
			temp_0 = reg_value;
		}
		
		item_cnt++;
	}
	
	item_cnt = 0;
	reg_value = ~reg_value;
	temp_3 = reg_value;
	temp_2 = reg_value;
	temp_1 = reg_value;
	temp_0 = reg_value;;
	
	printf("Start 2nd round test...\n\n");
	
	while (item_cnt < AAL_FDB_MAX_NUM) {
	
		FDB_REG_WRITE(L2FE_L2E_FDB_DATA0, temp_0);
		FDB_REG_WRITE(L2FE_L2E_FDB_DATA1, temp_1);
		FDB_REG_WRITE(L2FE_L2E_FDB_DATA2, temp_2);
		FDB_REG_WRITE(L2FE_L2E_FDB_DATA3, temp_3);
		
		l2_fdb_access.wrd = 0xc0000000 | item_cnt;
		FDB_REG_WRITE(L2FE_L2E_FDB_ACCESS, l2_fdb_access.wrd);
		
		do{
			l2_fdb_access.wrd = CA_NE_REG_READ(L2FE_L2E_FDB_ACCESS);
		} while (l2_fdb_access.bf.access);

		temp_3 = (temp_3 << 1) | (temp_2 & 0x80000000) >> 31 ;
		temp_2 = (temp_2 << 1) | (temp_1 & 0x80000000) >> 31 ;
		temp_1 = (temp_1 << 1) | (temp_0 & 0x80000000) >> 31 ;
		temp_0 = (temp_0 << 1) | 0x1;
					
		if(((item_cnt+1)%99) == 0)
		{
			temp_3 = reg_value;
			temp_2 = reg_value;
			temp_1 = reg_value;
			temp_0 = reg_value;
		}
		
		item_cnt++;
	}
	
	item_cnt = 0;
	temp_3 = reg_value;
	temp_2 = reg_value;
	temp_1 = reg_value;
	temp_0 = reg_value;;
	//ca_printf("Start checking the correctness of each entry...\n");

	while (item_cnt < AAL_FDB_MAX_NUM) {
	
		l2_fdb_access.wrd = 0x80000000 | item_cnt;
		FDB_REG_WRITE(L2FE_L2E_FDB_ACCESS, l2_fdb_access.wrd);
		
		do{
			l2_fdb_access.wrd = CA_NE_REG_READ(L2FE_L2E_FDB_ACCESS);
		} while (l2_fdb_access.bf.access);
		
		fdb_data3.wrd = CA_NE_REG_READ(L2FE_L2E_FDB_DATA3);
		fdb_data2.wrd = CA_NE_REG_READ(L2FE_L2E_FDB_DATA2);
		fdb_data1.wrd = CA_NE_REG_READ(L2FE_L2E_FDB_DATA1);
		fdb_data0.wrd = CA_NE_REG_READ(L2FE_L2E_FDB_DATA0);

		if( (fdb_data3.wrd != (temp_3 & data3_mask)) ||
			(fdb_data2.wrd != temp_2) ||
			(fdb_data1.wrd != temp_1) ||
			(fdb_data0.wrd != temp_0)){
		
			ca_printf("Write fail occurred at fdb entry %d \n", item_cnt);
			ca_printf("Tested reg_value for DATA3 = 0x%x \n", (temp_3 & data3_mask));
			ca_printf("Tested reg_value for DATA2 = 0x%x \n", temp_2);
			ca_printf("Tested reg_value for DATA1 = 0x%x \n", temp_1);
			ca_printf("Tested reg_value for DATA0 = 0x%x \n", temp_0);
			ca_printf("Data 3~0 of this fdb entry:\n");
			ca_printf("L2FE_L2E_FDB_DATA3 = 0x%x \n", fdb_data3.wrd);
			ca_printf("L2FE_L2E_FDB_DATA2 = 0x%x \n", fdb_data2.wrd);
			ca_printf("L2FE_L2E_FDB_DATA1 = 0x%x \n", fdb_data1.wrd);
			ca_printf("L2FE_L2E_FDB_DATA0 = 0x%x \n\n", fdb_data0.wrd);
			
			error_cnt ++;
		}
		
		temp_3 = (temp_3 << 1) | (temp_2 & 0x80000000) >> 31 ;
		temp_2 = (temp_2 << 1) | (temp_1 & 0x80000000) >> 31 ;
		temp_1 = (temp_1 << 1) | (temp_0 & 0x80000000) >> 31 ;
		temp_0 = (temp_0 << 1) | 0x1;
					
		if(((item_cnt+1)%99) == 0)
		{
			temp_3 = reg_value;
			temp_2 = reg_value;
			temp_1 = reg_value;
			temp_0 = reg_value;
		}
		
		item_cnt++;
	}
	
	if(error_cnt == 0)
		return CA_E_OK;
	else
		return error_cnt;
}
#endif

#if 0//yocto
ca_status_t aal_fdb_addr_traverse_get (
    CA_IN ca_device_id_t             device_id,
    CA_IN ca_uint32_t             static_flag
)
{
    aal_fdb_entry_cfg_t  fdb_entry;
    ca_l2_addr_entry_t addr;
    ca_uint32 hit_addr = 0, start_addr = 0;
    ca_uint32 ret = 0, cmd_ret = 0;
    ca_uint32 item_cnt = 0;
    aal_fdb_flush_cfg_t cfg;
    aal_fdb_cpu_cmd_word4_msk_t  mask;

    memset((void *)&fdb_entry, 0, sizeof(aal_fdb_entry_cfg_t));
    memset((void *)&addr, 0, sizeof(ca_l2_addr_entry_t));
    L2_DEBUG("\r\n");

    /* load cfg */
    memset((void *)&cfg, 0, sizeof(aal_fdb_flush_cfg_t));

    aal_fdb_cmp_cfg_load(&cfg);

    mask.wrd = 0xffffffff;
    /* only join compare lpid */
    mask.bf.lpid = 0;

    aal_fdb_scan_flush_field_set(0, mask, &cfg);

    L2_DEBUG("please make sure call function aal_fdb_scan_flush_field_set to set word0--word4\r\n");

    while (1) {
        ret = aal_fdb_entry_scan(0,
                                start_addr, &hit_addr,
                                &fdb_entry, &cmd_ret);

        if (ret != CA_E_OK || cmd_ret != CA_AAL_FDB_ACCESS_HIT) {
            L2_DEBUG("search end\r\n");
            break;
        }

        aal_fdb_entry_2_l2_addr(&addr, &fdb_entry);

        L2_DEBUG("start_addr:%d, hit_addr:%d, ret:%d\r\n", start_addr, hit_addr, ret);
        L2_DEBUG("mac:0x%x:%x:%x:%x:%x:%x, key_vid:%d, sc_ind:%d, dot1p:%d, lrnt_vid:%d, sa:%d, da:%d, static:%d, aging:%d, mc:%d, pid:%d, valid:%d, tmp:%d\r\n",
                fdb_entry.mac[0], fdb_entry.mac[1], fdb_entry.mac[2], fdb_entry.mac[3], fdb_entry.mac[4], fdb_entry.mac[5],
                fdb_entry.key_vid    ,
                fdb_entry.key_sc_ind ,
                fdb_entry.dot1p      ,
                fdb_entry.lrnt_vid   ,
                fdb_entry.sa_permit  ,
                fdb_entry.da_permit  ,
                fdb_entry.static_flag,
                fdb_entry.aging_sts  ,
                fdb_entry.mc_flag    ,
                fdb_entry.port_id,
                fdb_entry.valid,
                fdb_entry.tmp_flag);
        if (0 == item_cnt){
            printk(" idx  pid      mac           key vid lrnt vid dot1p sa  da static age   mc  valid tmp\r\n");
            printk("----+----+------------------+-------+-------+-----+---+---+-----+----+----+----+-----\r\n");
        }
        printk("%-5d%-5d%02x:%02x:%02x:%02x:%02x:%02x%4d/%c  %4d/%c      %-6d%-4d%-4d%-5d%-5d%-5d%-5d%-6d\r\n",
                hit_addr, fdb_entry.port_id,
                fdb_entry.mac[0], fdb_entry.mac[1], fdb_entry.mac[2], fdb_entry.mac[3], fdb_entry.mac[4], fdb_entry.mac[5],
                fdb_entry.key_vid    ,
                fdb_entry.key_sc_ind ? 's' : 'c',
                fdb_entry.lrnt_vid   ,
                fdb_entry.lrnt_sc_ind ? 's' : 'c',
                fdb_entry.dot1p      ,
                fdb_entry.sa_permit  ,
                fdb_entry.da_permit  ,
                fdb_entry.static_flag,
                FDB_AGING_TIME(fdb_entry.aging_sts)  ,
                fdb_entry.mc_flag    ,
                fdb_entry.valid,
                fdb_entry.tmp_flag);
        if (start_addr == hit_addr + 1){
            L2_DEBUG("start_addr == hit_addr + 1\r\n");
            break;
        }

        start_addr = hit_addr + 1;
        ++item_cnt;
    }

    ca_printf("\r\nTotal FDB entry : %u\n", item_cnt);

   ////////
/*
    item_cnt = 0;
    L2_DEBUG("\r\n");
    start_addr = 0;

    while (1) {
        ret = aal_fdb_entry_scan(0,
                                start_addr, &hit_addr,
                                &fdb_entry, &cmd_ret);

        if (ret != CA_E_OK || cmd_ret != CA_AAL_FDB_ACCESS_HIT) {
            L2_DEBUG("search end\r\n");
            break;
        }

        aal_fdb_entry_2_l2_addr(&addr, &fdb_entry);

        L2_DEBUG("start_addr:%d, hit_addr:%d, ret:%d\r\n", start_addr, hit_addr, ret);
        L2_DEBUG("mac:0x%x:%x:%x:%x:%x:%x, key_vid:%d, sc_ind:%d, dot1p:%d, lrnt_vid:%d, sa:%d, da:%d, static:%d, aging:%d, mc:%d, pid:%d\r\n",
                fdb_entry.mac[0], fdb_entry.mac[1], fdb_entry.mac[2], fdb_entry.mac[3], fdb_entry.mac[4], fdb_entry.mac[5],
                fdb_entry.key_vid    ,
                fdb_entry.key_sc_ind ,
                fdb_entry.dot1p      ,
                fdb_entry.lrnt_vid   ,
                fdb_entry.sa_permit  ,
                fdb_entry.da_permit  ,
                fdb_entry.static_flag,
                fdb_entry.aging_sts  ,
                fdb_entry.mc_flag    ,
                fdb_entry.port_id);

        if (start_addr == hit_addr + 1){
            L2_DEBUG("start_addr == hit_addr + 1\r\n");
            break;
        }

        start_addr = hit_addr + 1;
        if (++item_cnt > 10) break;
    }
    */

    L2_DEBUG("RB Tree: static table show\r\n");

    if (static_flag){
        aal_fdb_entry_cfg_t *entry_p = NULL;
        ca_int32_t  cnt = 1;

        entry_p = aal_fdb_static_getfirst(fdb_tbl_p);

        if(entry_p){
            ca_printf("index:%d\r\n      mac:%02x:%02x:%02x:%02x:%02x:%02x, vlan:%d, port:%d\r\n",
                cnt,
                entry_p->mac[0],entry_p->mac[1],entry_p->mac[2],entry_p->mac[3],entry_p->mac[4],entry_p->mac[5],
                entry_p->key_vid, entry_p->port_id);
        }

        while (entry_p) {
            cnt++;
            entry_p = aal_fdb_static_getnext(fdb_tbl_p);
            if(entry_p){
                ca_printf("index:%d\r\n      mac:%02x:%02x:%02x:%02x:%02x:%02x, vlan:%d, port:%d\r\n",
                    cnt,
                    entry_p->mac[0],entry_p->mac[1],entry_p->mac[2],entry_p->mac[3],entry_p->mac[4],entry_p->mac[5],
                    entry_p->key_vid, entry_p->port_id);            }
        }

        /* SVL tree */
        cnt = 1;
        entry_p = aal_fdb_static_getfirst(fdb_tbl_svl_p);

        if(entry_p){
            ca_printf("SVL TREE \r\nindex:%d\r\n      mac:%02x:%02x:%02x:%02x:%02x:%02x, vlan:%d, port:%d\r\n",
                cnt,
                entry_p->mac[0],entry_p->mac[1],entry_p->mac[2],entry_p->mac[3],entry_p->mac[4],entry_p->mac[5],
                entry_p->key_vid, entry_p->port_id);
        }

        while (entry_p) {
            cnt++;
            entry_p = aal_fdb_static_getnext(fdb_tbl_svl_p);
            if(entry_p){
                ca_printf("index:%d\r\n      mac:%02x:%02x:%02x:%02x:%02x:%02x, vlan:%d, port:%d\r\n",
                    cnt,
                    entry_p->mac[0],entry_p->mac[1],entry_p->mac[2],entry_p->mac[3],entry_p->mac[4],entry_p->mac[5],
                    entry_p->key_vid, entry_p->port_id);            }
        }


    }
    return CA_E_OK;
}

ca_status_t aal_fdb_addr_delete_by_port_get (
    CA_IN ca_device_id_t           device_id,
    CA_IN ca_port_id_t          aal_port,
    CA_IN ca_uint32_t           flag
)
{

    ca_printf("flag: 0--static, 1--dynamic, 2--both\r\n");

    return __aal_fdb_addr_delete_by_port(device_id, aal_port, flag);

}


ca_status_t aal_fdb_addr_iterate_entry_cnt_get (
    CA_IN ca_device_id_t             device_id,
    CA_IN ca_uint32_t             entry_count
)
{
    ca_iterator_t entry;
    ca_l2_addr_entry_t *l2_addr, *cfg;
    ca_status_t ret = CA_E_OK;
    int cnt = 0, j;
    int sz = 0;

    printk("ca_l2_addr_iterate, get once\r\n");
    memset(&entry, 0, sizeof(ca_iterator_t));
    entry.entry_count = entry_count;
    sz = sizeof(ca_l2_addr_entry_t) * entry.entry_count;

    l2_addr = ca_malloc(sz);

    memset(l2_addr, 0, sz);
    entry.p_entry_data = l2_addr;
    entry.user_buffer_size = sizeof(ca_l2_addr_entry_t) * entry.entry_count;

    /*while (1)*/{
        ret = ca_l2_addr_iterate(0, &entry);

        if (ret == CA_E_UNAVAIL){
            printk("end search\r\n");
      //      break;
        }

    }

    printk("entry_count = %#x \n",entry.entry_count);
    printk("p_entry_data = %p\n",entry.p_entry_data);
    printk("user_buffer_size = %#x \n",entry.user_buffer_size);
    printk("p_prev_handle = %p, %d\n",entry.p_prev_handle, (ca_uint32_t)entry.p_prev_handle);

    for (cnt = 0; cnt < entry.entry_count; cnt++){
        cfg = l2_addr + cnt;
        printk(" mac:");
        for(j = 0 ; j < 6; j++){
            printk("%x:", cfg->mac_addr[j]);
        }
        printk(" key_vid:%d, lrn_vid:%d, dot1p:%d, static:%d, sa:%d, da:%d, port_id:%#x, mc:%d, age:%d\r\n",
            cfg->vid, cfg->fwd_vid, cfg->dot1p, cfg->static_flag,
            cfg->sa_permit, cfg->da_permit, cfg->port_id, cfg->mc_group_id, cfg->aging_timer);
    }

    ca_free(l2_addr);

    return CA_E_OK;
}

ca_status_t aal_fdb_addr_iterate_loop_count_get (
    CA_IN ca_device_id_t             device_id,
    CA_IN ca_uint32_t             loop_count
)
{
    ca_iterator_t entry;
    ca_l2_addr_entry_t *l2_addr, *cfg;
    ca_status_t ret = CA_E_OK;
    int cnt = 0, j;
    int sz = 0;

    printk("ca_l2_addr_iterate: entry_count=1, loop up to loop_count\r\n");
    memset(&entry, 0, sizeof(ca_iterator_t));
    entry.entry_count = 1;
    sz = sizeof(ca_l2_addr_entry_t) * entry.entry_count;

    l2_addr = ca_malloc(sz);

    memset(l2_addr, 0, sz);
    entry.p_entry_data = l2_addr;
    entry.user_buffer_size = sizeof(ca_l2_addr_entry_t) * entry.entry_count;

    for(cnt = 0; cnt < loop_count; cnt++){
        ret = ca_l2_addr_iterate(0, &entry);

        if (ret == CA_E_UNAVAIL){
            printk("end search\r\n");
            break;
        }


        printk("entry_count = %#x \n",entry.entry_count);
        printk("p_entry_data = %p\n",entry.p_entry_data);
        printk("user_buffer_size = %#x \n",entry.user_buffer_size);
        printk("p_prev_handle = %p, %d\n",entry.p_prev_handle, (ca_uint32_t)entry.p_prev_handle);

        cfg = l2_addr;
        printk(" mac:");
        for(j = 0 ; j < 6; j++){
            printk("%x:", cfg->mac_addr[j]);
        }
        printk(" key_vid:%d, lrn_vid:%d, dot1p:%d, static:%d, sa:%d, da:%d, port_id:%#x, mc:%d, age:%d\r\n",
            cfg->vid, cfg->fwd_vid, cfg->dot1p, cfg->static_flag,
            cfg->sa_permit, cfg->da_permit, cfg->port_id, cfg->mc_group_id, cfg->aging_timer);

    }

    ca_free(l2_addr);

    return CA_E_OK;
}

ca_mac_addr_t    dbg_filter_mac = {0x00, 0xca, 0xfe, 0x00, 0x00, 0x01};

ca_status_t aal_fdb_mac_filter_add_get (
    CA_IN ca_device_id_t            device_id,
    CA_IN ca_int32_t             lpid,
    CA_IN ca_int32_t             mac_id
)
{
    aal_tbl_mac_filter_entry_t filter_entry;

    ca_status_t ret = CA_E_OK;
    //ca_int32_t lpid = CA_PORT_INDEX_GET(port_id);

    //L2_ASSERT_RET(entry != NULL, CA_E_PARAM);

    //CA_PORT_ETHERNET_CHECK(port_id);

    L2_DEBUG("port_id:%d\r\n", lpid);


    memset(&filter_entry, 0, sizeof(aal_tbl_mac_filter_entry_t));

    filter_entry.mask = CA_L2_MAC_FILTER_MASK_MAC_VLAN;
    filter_entry.mac_flag = 0;
    filter_entry.vid = 11;
    filter_entry.drop_flag = 0;
    filter_entry.port_id = lpid;
    dbg_filter_mac[5] = mac_id;
    memcpy(&filter_entry.mac, &dbg_filter_mac, CA_ETH_ADDR_LEN);

    L2_DEBUG("mac:%02x:%02x:%02x:%02x:%02x:%02x, vlan:%d, port:0x%x\r\n", filter_entry.mac[0],filter_entry.mac[1],filter_entry.mac[2],filter_entry.mac[3],filter_entry.mac[4],filter_entry.mac[5],
        filter_entry.vid, filter_entry.port_id);

    ret = aal_fdb_mac_filter_add(0, lpid, &filter_entry);

    return ret;
}


ca_status_t aal_fdb_mac_filter_delete_get (
    CA_IN ca_device_id_t            device_id,
    CA_IN ca_int32               lpid,
    CA_IN ca_int32_t             mac_id
)
{
    aal_tbl_mac_filter_entry_t filter_entry;
    ca_status_t ret = CA_E_OK;
    //ca_int32 lpid = CA_PORT_INDEX_GET(port_id);

    //L2_ASSERT_RET(entry != NULL, CA_E_PARAM);

    L2_DEBUG("port_id:%d\r\n", lpid);

    /*
    CA_PORT_ETHERNET_CHECK(port_id);
    */
    memset(&filter_entry, 0, sizeof(aal_tbl_mac_filter_entry_t));

    filter_entry.mask = CA_L2_MAC_FILTER_MASK_MAC_VLAN;
    filter_entry.mac_flag = 0;
    filter_entry.vid = 11;
    filter_entry.drop_flag = 0;
    filter_entry.port_id = lpid;
    dbg_filter_mac[5] = mac_id;
    memcpy(&filter_entry.mac, &dbg_filter_mac, CA_ETH_ADDR_LEN);

    L2_DEBUG("mac:%02x:%02x:%02x:%02x:%02x:%02x, vlan:%d, port:%d\r\n",
        filter_entry.mac[0],filter_entry.mac[1],filter_entry.mac[2],filter_entry.mac[3],filter_entry.mac[4],filter_entry.mac[5],
        filter_entry.vid, filter_entry.port_id);

    ret = aal_fdb_mac_filter_delete(0, lpid, &filter_entry);

    return ret;
}


ca_status_t aal_fdb_mac_filter_delete_all_get (
    CA_IN ca_device_id_t             device_id,
    CA_IN ca_int32 lpid
)
{
    ca_status_t ret = CA_E_OK;
    //ca_int32 lpid = CA_PORT_INDEX_GET(port_id);

    //CA_PORT_ETHERNET_CHECK(port_id);

    ret = aal_fdb_mac_filter_delete_all(0, lpid);
    L2_DEBUG("port_id:%d\r\n", lpid);

    return ret;
}

ca_status_t aal_fdb_mac_filter_traverse_get (
    CA_IN ca_device_id_t             device_id
)
{
    aal_tbl_mac_filter_entry_t *entry_p = NULL;
    ca_int32_t  cnt = 1;
    ca_int32_t  i;

    entry_p = aal_fdb_mac_filter_getfirst();

    if(entry_p){
        ca_printf("index:%d\r\n      mac:%02x:%02x:%02x:%02x:%02x:%02x, vlan:%d, mask:%d, mac_flag:%d, drop_flag:%d, port_id:%d, entry:%d, rule:%d\r\n",
            cnt,
            entry_p->mac[0],entry_p->mac[1],entry_p->mac[2],entry_p->mac[3],entry_p->mac[4],entry_p->mac[5],
            entry_p->vid, entry_p->mask, entry_p->mac_flag, entry_p->drop_flag, entry_p->port_id, entry_p->entry_id, entry_p->rule_id);
    }

    while (entry_p) {
        cnt++;
        entry_p = aal_fdb_mac_filter_getnext();
        if(entry_p){
            ca_printf("index:%d\r\n      mac:%02x:%02x:%02x:%02x:%02x:%02x, vlan:%d, mask:%d, mac_flag:%d, drop_flag:%d, port_id:%d, entry:%d, rule:%d\r\n",
                cnt,
                entry_p->mac[0],entry_p->mac[1],entry_p->mac[2],entry_p->mac[3],entry_p->mac[4],entry_p->mac[5],
                entry_p->vid, entry_p->mask, entry_p->mac_flag, entry_p->drop_flag, entry_p->port_id, entry_p->entry_id, entry_p->rule_id);
        }
    }

    ca_printf("default action:\r\n");
    for (i = 0; i < CA_AAL_FDB_ETH_PORT_NUM; i++){
        ca_printf("port:%d:  entry:%d, action:%s\r\n", i, CLS_ENTRY_ID(i, CA_AAL_FDB_MAC_FILTER_DFT_ID),
            mac_filter_dft[i] ? "TRUE" : "FALSE");
    }

    return CA_E_OK;
}


/* debug code for mac filter covert id
ca_status_t aal_fdb_mac_filter_id_get()
{
    int i, partner_id, rule_0, rule_1;
    int port_id, rule_id, entry_id;

    for (i = 1; i <= 16; i++){
        partner_id = CA_AAL_RULE_ID_PARTNER(i);
        printf("i:%d, partner:%d\r\n", i, partner_id);

        rule_0 = CLS_IDX_IN_ENTRY(i);
        rule_1 = CA_AAL_RULE_ID_PARTNER(i);
    }

    for (i = 1; i <= 16; i++){
        partner_id = CA_AAL_RULE_ID_PARTNER(i);
        rule_0 = CLS_IDX_IN_ENTRY(i);
        rule_1 = CLS_IDX_IN_ENTRY(partner_id);
        printf("rule_id:%d, rule_0/1:%d/%d\r\n", i, rule_0, rule_1);
    }

    for (port_id = 0; port_id < 4; port_id ++)
        for (rule_id = 1; rule_id <= 16; rule_id++){
            entry_id = CLS_ENTRY_ID(port_id, rule_id);
            printf("port,rule-->entry: [%d, %d] --> %d\r\n", port_id, rule_id, entry_id);
        }
    return 0;
}

*/

ca_status_t aal_fdb_port_limit_init_get(
    CA_IN ca_device_id_t device_id,
    CA_IN ca_int32    port_id
)
{
    return aal_fdb_port_limit_init(device_id, port_id);
}

ca_status_t aal_fdb_port_limit_inc_get(
    CA_IN ca_device_id_t device_id,
    CA_IN ca_int32    port_id
)
{
    return aal_fdb_port_limit_inc(device_id, port_id);
}

ca_status_t aal_fdb_port_limit_dec_get(
    CA_IN ca_device_id_t device_id,
    CA_IN ca_int32    port_id
)
{
    return aal_fdb_port_limit_dec(device_id, port_id);
}

ca_status_t aal_fdb_port_cnt_init_get(
    CA_IN ca_device_id_t device_id,
    CA_IN ca_int32    port_id
)
{
    return aal_fdb_port_cnt_init(device_id, port_id);
}

ca_status_t aal_fdb_port_cnt_inc_get(
    CA_IN ca_device_id_t     device_id,
    CA_IN ca_int32        port_id
)
{
    return aal_fdb_port_cnt_inc(device_id, port_id);
}
ca_status_t aal_fdb_port_cnt_dec_get(
    CA_IN ca_device_id_t     device_id,
    CA_IN ca_int32        port_id
)
{
    return aal_fdb_port_cnt_dec(device_id, port_id);
}

#endif

#ifdef __KERNEL__

EXPORT_SYMBOL(aal_fdb_debug_set);
EXPORT_SYMBOL(aal_fdb_debug_get);
EXPORT_SYMBOL(aal_fdb_cmp_cfg_load);
EXPORT_SYMBOL(aal_fdb_entry_del);
EXPORT_SYMBOL(aal_fdb_entry_get);
EXPORT_SYMBOL(aal_fdb_port_limit_set);
EXPORT_SYMBOL(aal_fdb_port_cnt_inc);
EXPORT_SYMBOL(aal_fdb_mac_filter_delete);
EXPORT_SYMBOL(aal_fdb_ctrl_set);
EXPORT_SYMBOL(aal_fdb_mac_filter_delete_all_get);
EXPORT_SYMBOL(aal_fdb_port_limit_inc);
EXPORT_SYMBOL(aal_fdb_scan_flush_field_set);
EXPORT_SYMBOL(aal_fdb_port_limit_init_get);
EXPORT_SYMBOL(aal_fdb_port_cnt_init_get);
EXPORT_SYMBOL(aal_fdb_port_cnt_init);
EXPORT_SYMBOL(aal_fdb_mac_filter_add);
EXPORT_SYMBOL(aal_fdb_mac_filter_add_get);
EXPORT_SYMBOL(aal_fdb_entry_add);
EXPORT_SYMBOL(aal_fdb_mac_filter_default_get);
EXPORT_SYMBOL(aal_fdb_entry_addr_set);
EXPORT_SYMBOL(aal_fdb_port_cnt_set);
EXPORT_SYMBOL(aal_fdb_mac_filter_getnext);
EXPORT_SYMBOL(aal_fdb_entry_scan);
EXPORT_SYMBOL(aal_fdb_glb_intr_enable_set);
EXPORT_SYMBOL(aal_fdb_entry_flush);
EXPORT_SYMBOL(aal_fdb_mac_filter_delete_all);
EXPORT_SYMBOL(aal_fdb_port_cnt_dec);
EXPORT_SYMBOL(aal_fdb_port_limit_get);
EXPORT_SYMBOL(aal_fdb_port_limit_dec_get);
EXPORT_SYMBOL(aal_fdb_table_get);
EXPORT_SYMBOL(aal_fdb_port_limit_dec);
EXPORT_SYMBOL(aal_fdb_dbg_add_get);
EXPORT_SYMBOL(aal_fdb_entry_addr_get);
EXPORT_SYMBOL(aal_fdb_mac_filter_traverse_get);
EXPORT_SYMBOL(aal_fdb_learn_status_get);
EXPORT_SYMBOL(aal_fdb_addr_delete_get);
EXPORT_SYMBOL(aal_fdb_port_cnt_get);
EXPORT_SYMBOL(aal_fdb_mac_filter_getfirst);
EXPORT_SYMBOL(aal_fdb_addr_traverse_get);
EXPORT_SYMBOL(aal_fdb_addr_delete_by_port_get);
EXPORT_SYMBOL(aal_fdb_addr_iterate_loop_count_get);
EXPORT_SYMBOL(aal_fdb_addr_iterate_entry_cnt_get);
EXPORT_SYMBOL(aal_fdb_glb_intr_enable_get);
EXPORT_SYMBOL(aal_fdb_glb_intr_status_get);
EXPORT_SYMBOL(aal_fdb_mac_filter_init);
EXPORT_SYMBOL(aal_fdb_ctrl_get);
EXPORT_SYMBOL(aal_fdb_tbl_del_all);
EXPORT_SYMBOL(aal_fdb_addr_delete_all_get);
EXPORT_SYMBOL(aal_fdb_init);
EXPORT_SYMBOL(aal_fdb_mac_filter_default_set);
EXPORT_SYMBOL(aal_fdb_scan_flush_field_init);
EXPORT_SYMBOL(aal_fdb_port_limit_inc_get);
EXPORT_SYMBOL(aal_fdb_scan_flush_field_get);
EXPORT_SYMBOL(aal_fdb_port_limit_init);
EXPORT_SYMBOL(aal_fdb_port_cnt_inc_get);
EXPORT_SYMBOL(aal_fdb_mac_filter_delete_get);
EXPORT_SYMBOL(aal_fdb_port_cnt_dec_get);

EXPORT_SYMBOL(aal_fdb_lrn_fwd_ctrl_set);
EXPORT_SYMBOL(aal_fdb_lrn_fwd_ctrl_get);

EXPORT_SYMBOL(aal_fdb_cpu_pkt_lrn_set);
EXPORT_SYMBOL(aal_fdb_cpu_pkt_lrn_get);
EXPORT_SYMBOL(aal_fdb_limit_excd_fwd_ctrl_get);
EXPORT_SYMBOL(aal_fdb_ilpb_unknw_pol_idx_map_set);
EXPORT_SYMBOL(aal_fdb_ilpb_unknw_pol_idx_map_get);
EXPORT_SYMBOL(aal_fdb_ilpb_unknw_pol_idx_cb_get);
EXPORT_SYMBOL(aal_fdb_addr_aged_entry_get);

#else//sd1 uboot for 98f
#endif//sd1 uboot for 98f

#endif


