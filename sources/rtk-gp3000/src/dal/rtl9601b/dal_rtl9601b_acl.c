/*
 * Copyright (C) 2012 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Definition of ACL API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) ACL rule action configure and modification
 *
 */

/*
 * Include Files
 */
#include <rtk/port.h>
#include <rtk/acl.h>
#include <dal/rtl9601b/dal_rtl9601b.h>
#include <dal/rtl9601b/dal_rtl9601b_acl.h>
#include <hal/mac/reg.h>
#include <hal/mac/drv.h>
#include <rtk/irq.h>
#include <rtk/intr.h>
#include <dal/rtl9601b/dal_rtl9601b_intr.h>

/*
 * Symbol Definition
 */
static uint32 rtl9601b_acl_init = {INIT_NOT_COMPLETED};


static const rtl9601b_dal_acl_fieldTypes_t rtl9601b_dal_template_init[4][8] =
    {{RTL9601B_DAL_ACL_DMAC0,           RTL9601B_DAL_ACL_DMAC1,         RTL9601B_DAL_ACL_DMAC2,         RTL9601B_DAL_ACL_STAG,          RTL9601B_DAL_ACL_SMAC0,         RTL9601B_DAL_ACL_SMAC1,         RTL9601B_DAL_ACL_SMAC2,         RTL9601B_DAL_ACL_ETHERTYPE},
     {RTL9601B_DAL_ACL_CTAG,            RTL9601B_DAL_ACL_IP4SIP0,       RTL9601B_DAL_ACL_IP4SIP1,       RTL9601B_DAL_ACL_VIDRANGE,      RTL9601B_DAL_ACL_IPRANGE,       RTL9601B_DAL_ACL_PORTRANGE,     RTL9601B_DAL_ACL_IP4DIP0,       RTL9601B_DAL_ACL_IP4DIP1},
     {RTL9601B_DAL_ACL_FIELD_SELECT00,  RTL9601B_DAL_ACL_FIELD_SELECT01,RTL9601B_DAL_ACL_FIELD_SELECT02,RTL9601B_DAL_ACL_FIELD_SELECT03,RTL9601B_DAL_ACL_FIELD_SELECT04,RTL9601B_DAL_ACL_FIELD_SELECT05,RTL9601B_DAL_ACL_FIELD_SELECT06,RTL9601B_DAL_ACL_FIELD_SELECT07},
     {RTL9601B_DAL_ACL_FIELD_SELECT08,  RTL9601B_DAL_ACL_FIELD_SELECT09,RTL9601B_DAL_ACL_FIELD_SELECT10,RTL9601B_DAL_ACL_FIELD_SELECT11,RTL9601B_DAL_ACL_FIELD_SELECT12,RTL9601B_DAL_ACL_FIELD_SELECT13,RTL9601B_DAL_ACL_FIELD_SELECT14,RTL9601B_DAL_ACL_FIELD_SELECT15}};

static uint32 _actionIntCnt=0;

/*
 * Macro Declaration
 */
#define RTL9601B_ACL_ACTION_DATA_LEN      1
static uint32 dal_rtl9601b_acl_isr_init(void);
static uint32 dal_rtl9601b_acl_isr_entry(void);


/*
 * Function Declaration
 */
static int32 _dal_rtl9601b_acl_ruleField_check(rtk_acl_template_t *pTemplate, rtk_acl_field_type_t fieldType, uint32 start, uint32 end, uint32 *pIndex)
{
    uint32 templateIndx;

    for(templateIndx = start; templateIndx <= end; templateIndx ++)
    {
        if(pTemplate->fieldType[templateIndx] == fieldType)
        {
            *pIndex = templateIndx;
            return RT_ERR_OK;
        }
    }

    return RT_ERR_FAILED;
}

static int32 _dal_rtl9601b_acl_ruleField_add(rtl9601b_raw_acl_ruleEntry_t *pRawRule, rtk_acl_template_t *pTemplate, rtk_acl_field_t *pField)
{
    int32   ret;
    uint32  val;
    uint32  fieldIndexStart;
    uint32  fieldIndexEnd;
    uint32  fieldIndexOffset;
    uint32  hitIndex;

    fieldIndexStart = 0;
    fieldIndexEnd = 7;

    fieldIndexOffset = 0;

    switch(pField->fieldType)
    {
        case ACL_FIELD_CTAG:
        case ACL_FIELD_STAG:
            if ((ret = _dal_rtl9601b_acl_ruleField_check(pTemplate, pField->fieldType,fieldIndexStart, fieldIndexEnd, &hitIndex)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
                return ret;
            }

            val = pField->fieldUnion.l2tag.pri.value;
            val = (val << 1) | pField->fieldUnion.l2tag.cfi_dei.value;
            val = (val << 12) | pField->fieldUnion.l2tag.vid.value;
            pRawRule->data_bits.field[hitIndex - fieldIndexOffset] = val;

            val = pField->fieldUnion.l2tag.pri.mask;
            val = (val << 1) | pField->fieldUnion.l2tag.cfi_dei.mask;
            val = (val << 12) | pField->fieldUnion.l2tag.vid.mask;
            pRawRule->care_bits.field[hitIndex - fieldIndexOffset] = val;

            break;
        case ACL_FIELD_ETHERTYPE:
        case ACL_FIELD_GEMPORT:
        case ACL_FIELD_VID_RANGE:
        case ACL_FIELD_IP_RANGE:
        case ACL_FIELD_PORT_RANGE:
        case ACL_FIELD_PKT_LEN_RANGE:
        case ACL_FIELD_USER_VALID:
        case ACL_FIELD_USER_DEFINED00:
        case ACL_FIELD_USER_DEFINED01:
        case ACL_FIELD_USER_DEFINED02:
        case ACL_FIELD_USER_DEFINED03:
        case ACL_FIELD_USER_DEFINED04:
        case ACL_FIELD_USER_DEFINED05:
        case ACL_FIELD_USER_DEFINED06:
        case ACL_FIELD_USER_DEFINED07:
        case ACL_FIELD_USER_DEFINED08:
        case ACL_FIELD_USER_DEFINED09:
        case ACL_FIELD_USER_DEFINED10:
        case ACL_FIELD_USER_DEFINED11:
        case ACL_FIELD_USER_DEFINED12:
        case ACL_FIELD_USER_DEFINED13:
        case ACL_FIELD_USER_DEFINED14:
        case ACL_FIELD_USER_DEFINED15:
            if ((ret = _dal_rtl9601b_acl_ruleField_check(pTemplate, pField->fieldType,fieldIndexStart, fieldIndexEnd, &hitIndex)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
                return ret;
            }
            pRawRule->data_bits.field[hitIndex - fieldIndexOffset] = pField->fieldUnion.data.value;
            pRawRule->care_bits.field[hitIndex - fieldIndexOffset] = pField->fieldUnion.data.mask;

            break;
        case ACL_FIELD_PATTERN_MATCH:
            if(pField->fieldUnion.pattern.fieldIdx > (fieldIndexEnd - fieldIndexStart))
            {
                return RT_ERR_CHIP_NOT_SUPPORTED;
            }

            pRawRule->data_bits.field[pField->fieldUnion.pattern.fieldIdx] = pField->fieldUnion.pattern.data.value;
            pRawRule->care_bits.field[pField->fieldUnion.pattern.fieldIdx] = pField->fieldUnion.pattern.data.mask;

            break;
        case ACL_FIELD_DMAC:
            if ((ret = _dal_rtl9601b_acl_ruleField_check(pTemplate, ACL_FIELD_DMAC0,fieldIndexStart, fieldIndexEnd, &hitIndex)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
                return ret;
            }
            val = pField->fieldUnion.mac.value.octet[4];
            val = (val << 8) | pField->fieldUnion.mac.value.octet[5];
            pRawRule->data_bits.field[hitIndex - fieldIndexOffset] = val;

            val = pField->fieldUnion.mac.mask.octet[4];
            val = (val << 8) | pField->fieldUnion.mac.mask.octet[5];
            pRawRule->care_bits.field[hitIndex - fieldIndexOffset] = val;

            if ((ret = _dal_rtl9601b_acl_ruleField_check(pTemplate, ACL_FIELD_DMAC1,fieldIndexStart, fieldIndexEnd, &hitIndex)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
                return ret;
            }
            val = pField->fieldUnion.mac.value.octet[2];
            val = (val << 8) | pField->fieldUnion.mac.value.octet[3];
            pRawRule->data_bits.field[hitIndex - fieldIndexOffset] = val;

            val = pField->fieldUnion.mac.mask.octet[2];
            val = (val << 8) | pField->fieldUnion.mac.mask.octet[3];
            pRawRule->care_bits.field[hitIndex - fieldIndexOffset] = val;

            if ((ret = _dal_rtl9601b_acl_ruleField_check(pTemplate, ACL_FIELD_DMAC2,fieldIndexStart, fieldIndexEnd, &hitIndex)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
                return ret;
            }
            val = pField->fieldUnion.mac.value.octet[0];
            val = (val << 8) | pField->fieldUnion.mac.value.octet[1];
            pRawRule->data_bits.field[hitIndex - fieldIndexOffset] = val;

            val = pField->fieldUnion.mac.mask.octet[0];
            val = (val << 8) | pField->fieldUnion.mac.mask.octet[1];
            pRawRule->care_bits.field[hitIndex - fieldIndexOffset] = val;

            break;
        case ACL_FIELD_DMAC0:
            if ((ret = _dal_rtl9601b_acl_ruleField_check(pTemplate, ACL_FIELD_DMAC0,fieldIndexStart, fieldIndexEnd, &hitIndex)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
                return ret;
            }
            val = pField->fieldUnion.mac.value.octet[4];
            val = (val << 8) | pField->fieldUnion.mac.value.octet[5];
            pRawRule->data_bits.field[hitIndex - fieldIndexOffset] = val;

            val = pField->fieldUnion.mac.mask.octet[4];
            val = (val << 8) | pField->fieldUnion.mac.mask.octet[5];
            pRawRule->care_bits.field[hitIndex - fieldIndexOffset] = val;

            break;
        case ACL_FIELD_DMAC1:
            if ((ret = _dal_rtl9601b_acl_ruleField_check(pTemplate, ACL_FIELD_DMAC1,fieldIndexStart, fieldIndexEnd, &hitIndex)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
                return ret;
            }
            val = pField->fieldUnion.mac.value.octet[2];
            val = (val << 8) | pField->fieldUnion.mac.value.octet[3];
            pRawRule->data_bits.field[hitIndex - fieldIndexOffset] = val;

            val = pField->fieldUnion.mac.mask.octet[2];
            val = (val << 8) | pField->fieldUnion.mac.mask.octet[3];
            pRawRule->care_bits.field[hitIndex - fieldIndexOffset] = val;

            break;
        case ACL_FIELD_DMAC2:
            if ((ret = _dal_rtl9601b_acl_ruleField_check(pTemplate, ACL_FIELD_DMAC2,fieldIndexStart, fieldIndexEnd, &hitIndex)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
                return ret;
            }
            val = pField->fieldUnion.mac.value.octet[0];
            val = (val << 8) | pField->fieldUnion.mac.value.octet[1];
            pRawRule->data_bits.field[hitIndex - fieldIndexOffset] = val;

            val = pField->fieldUnion.mac.mask.octet[0];
            val = (val << 8) | pField->fieldUnion.mac.mask.octet[1];
            pRawRule->care_bits.field[hitIndex - fieldIndexOffset] = val;

            break;
        case ACL_FIELD_SMAC:
            if ((ret = _dal_rtl9601b_acl_ruleField_check(pTemplate, ACL_FIELD_SMAC0,fieldIndexStart, fieldIndexEnd, &hitIndex)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
                return ret;
            }
            val = pField->fieldUnion.mac.value.octet[4];
            val = (val << 8) | pField->fieldUnion.mac.value.octet[5];
            pRawRule->data_bits.field[hitIndex - fieldIndexOffset] = val;

            val = pField->fieldUnion.mac.mask.octet[4];
            val = (val << 8) | pField->fieldUnion.mac.mask.octet[5];
            pRawRule->care_bits.field[hitIndex - fieldIndexOffset] = val;

            if ((ret = _dal_rtl9601b_acl_ruleField_check(pTemplate, ACL_FIELD_SMAC1,fieldIndexStart, fieldIndexEnd, &hitIndex)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
                return ret;
            }
            val = pField->fieldUnion.mac.value.octet[2];
            val = (val << 8) | pField->fieldUnion.mac.value.octet[3];
            pRawRule->data_bits.field[hitIndex - fieldIndexOffset] = val;

            val = pField->fieldUnion.mac.mask.octet[2];
            val = (val << 8) | pField->fieldUnion.mac.mask.octet[3];
            pRawRule->care_bits.field[hitIndex - fieldIndexOffset] = val;

            if ((ret = _dal_rtl9601b_acl_ruleField_check(pTemplate, ACL_FIELD_SMAC2,fieldIndexStart, fieldIndexEnd, &hitIndex)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
                return ret;
            }
            val = pField->fieldUnion.mac.value.octet[0];
            val = (val << 8) | pField->fieldUnion.mac.value.octet[1];
            pRawRule->data_bits.field[hitIndex - fieldIndexOffset] = val;

            val = pField->fieldUnion.mac.mask.octet[0];
            val = (val << 8) | pField->fieldUnion.mac.mask.octet[1];
            pRawRule->care_bits.field[hitIndex - fieldIndexOffset] = val;

            break;
        case ACL_FIELD_SMAC0:
            if ((ret = _dal_rtl9601b_acl_ruleField_check(pTemplate, ACL_FIELD_SMAC0,fieldIndexStart, fieldIndexEnd, &hitIndex)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
                return ret;
            }
            val = pField->fieldUnion.mac.value.octet[4];
            val = (val << 8) | pField->fieldUnion.mac.value.octet[5];
            pRawRule->data_bits.field[hitIndex - fieldIndexOffset] = val;

            val = pField->fieldUnion.mac.mask.octet[4];
            val = (val << 8) | pField->fieldUnion.mac.mask.octet[5];
            pRawRule->care_bits.field[hitIndex - fieldIndexOffset] = val;

            break;
        case ACL_FIELD_SMAC1:
            if ((ret = _dal_rtl9601b_acl_ruleField_check(pTemplate, ACL_FIELD_SMAC1,fieldIndexStart, fieldIndexEnd, &hitIndex)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
                return ret;
            }
            val = pField->fieldUnion.mac.value.octet[2];
            val = (val << 8) | pField->fieldUnion.mac.value.octet[3];
            pRawRule->data_bits.field[hitIndex - fieldIndexOffset] = val;

            val = pField->fieldUnion.mac.mask.octet[2];
            val = (val << 8) | pField->fieldUnion.mac.mask.octet[3];
            pRawRule->care_bits.field[hitIndex - fieldIndexOffset] = val;

            break;
        case ACL_FIELD_SMAC2:
            if ((ret = _dal_rtl9601b_acl_ruleField_check(pTemplate, ACL_FIELD_SMAC2,fieldIndexStart, fieldIndexEnd, &hitIndex)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
                return ret;
            }
            val = pField->fieldUnion.mac.value.octet[0];
            val = (val << 8) | pField->fieldUnion.mac.value.octet[1];
            pRawRule->data_bits.field[hitIndex - fieldIndexOffset] = val;

            val = pField->fieldUnion.mac.mask.octet[0];
            val = (val << 8) | pField->fieldUnion.mac.mask.octet[1];
            pRawRule->care_bits.field[hitIndex - fieldIndexOffset] = val;

            break;
        case ACL_FIELD_IPV4_SIP:
            if ((ret = _dal_rtl9601b_acl_ruleField_check(pTemplate, ACL_FIELD_IPV4_SIP0,fieldIndexStart, fieldIndexEnd, &hitIndex)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
                return ret;
            }
            val = pField->fieldUnion.ip.value & 0xFFFF;
            pRawRule->data_bits.field[hitIndex - fieldIndexOffset] = val;

            val = pField->fieldUnion.ip.mask & 0xFFFF;
            pRawRule->care_bits.field[hitIndex - fieldIndexOffset] = val;

            if ((ret = _dal_rtl9601b_acl_ruleField_check(pTemplate, ACL_FIELD_IPV4_SIP1,fieldIndexStart, fieldIndexEnd, &hitIndex)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
                return ret;
            }
            val = pField->fieldUnion.ip.value >> 16;
            pRawRule->data_bits.field[hitIndex - fieldIndexOffset] = val;

            val = pField->fieldUnion.ip.mask >> 16;
            pRawRule->care_bits.field[hitIndex - fieldIndexOffset] = val;

            break;
        case ACL_FIELD_IPV4_SIP0:
            if ((ret = _dal_rtl9601b_acl_ruleField_check(pTemplate, ACL_FIELD_IPV4_SIP0,fieldIndexStart, fieldIndexEnd, &hitIndex)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
                return ret;
            }
            val = pField->fieldUnion.ip.value & 0xFFFF;
            pRawRule->data_bits.field[hitIndex - fieldIndexOffset] = val;

            val = pField->fieldUnion.ip.mask & 0xFFFF;
            pRawRule->care_bits.field[hitIndex - fieldIndexOffset] = val;

            break;
        case ACL_FIELD_IPV4_SIP1:
            if ((ret = _dal_rtl9601b_acl_ruleField_check(pTemplate, ACL_FIELD_IPV4_SIP1,fieldIndexStart, fieldIndexEnd, &hitIndex)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
                return ret;
            }
            val = pField->fieldUnion.ip.value >> 16;
            pRawRule->data_bits.field[hitIndex - fieldIndexOffset] = val;

            val = pField->fieldUnion.ip.mask >> 16;
            pRawRule->care_bits.field[hitIndex - fieldIndexOffset] = val;

            break;
        case ACL_FIELD_IPV4_DIP:
            if ((ret = _dal_rtl9601b_acl_ruleField_check(pTemplate, ACL_FIELD_IPV4_DIP0,fieldIndexStart, fieldIndexEnd, &hitIndex)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
                return ret;
            }
            val = pField->fieldUnion.ip.value & 0xFFFF;
            pRawRule->data_bits.field[hitIndex - fieldIndexOffset] = val;

            val = pField->fieldUnion.ip.mask & 0xFFFF;
            pRawRule->care_bits.field[hitIndex - fieldIndexOffset] = val;

            if ((ret = _dal_rtl9601b_acl_ruleField_check(pTemplate, ACL_FIELD_IPV4_DIP1,fieldIndexStart, fieldIndexEnd, &hitIndex)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
                return ret;
            }
            val = pField->fieldUnion.ip.value >> 16;
            pRawRule->data_bits.field[hitIndex - fieldIndexOffset] = val;

            val = pField->fieldUnion.ip.mask >> 16;
            pRawRule->care_bits.field[hitIndex - fieldIndexOffset] = val;

            break;
        case ACL_FIELD_IPV4_DIP0:
            if ((ret = _dal_rtl9601b_acl_ruleField_check(pTemplate, ACL_FIELD_IPV4_DIP0,fieldIndexStart, fieldIndexEnd, &hitIndex)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
                return ret;
            }
            val = pField->fieldUnion.ip.value & 0xFFFF;
            pRawRule->data_bits.field[hitIndex - fieldIndexOffset] = val;

            val = pField->fieldUnion.ip.mask & 0xFFFF;
            pRawRule->care_bits.field[hitIndex - fieldIndexOffset] = val;

            break;
        case ACL_FIELD_IPV4_DIP1:
            if ((ret = _dal_rtl9601b_acl_ruleField_check(pTemplate, ACL_FIELD_IPV4_DIP1,fieldIndexStart, fieldIndexEnd, &hitIndex)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
                return ret;
            }
            val = pField->fieldUnion.ip.value >> 16;
            pRawRule->data_bits.field[hitIndex - fieldIndexOffset] = val;

            val = pField->fieldUnion.ip.mask >> 16;
            pRawRule->care_bits.field[hitIndex - fieldIndexOffset] = val;

            break;
       case ACL_FIELD_IPV6_SIP:
            if ((ret = _dal_rtl9601b_acl_ruleField_check(pTemplate, ACL_FIELD_IPV6_SIP0,fieldIndexStart, fieldIndexEnd, &hitIndex)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
                return ret;
            }
            val = pField->fieldUnion.ip6.value.ipv6_addr[14];
            val = (val << 8) | pField->fieldUnion.ip6.value.ipv6_addr[15];
            pRawRule->data_bits.field[hitIndex - fieldIndexOffset] = val;

            val = pField->fieldUnion.ip6.mask.ipv6_addr[14];
            val = (val << 8) | pField->fieldUnion.ip6.mask.ipv6_addr[15];
            pRawRule->care_bits.field[hitIndex - fieldIndexOffset] = val;

            if ((ret = _dal_rtl9601b_acl_ruleField_check(pTemplate, ACL_FIELD_IPV6_SIP1,fieldIndexStart, fieldIndexEnd, &hitIndex)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
                return ret;
            }
            val = pField->fieldUnion.ip6.value.ipv6_addr[12];
            val = (val << 8) | pField->fieldUnion.ip6.value.ipv6_addr[13];
            pRawRule->data_bits.field[hitIndex - fieldIndexOffset] = val;

            val = pField->fieldUnion.ip6.mask.ipv6_addr[12];
            val = (val << 8) | pField->fieldUnion.ip6.mask.ipv6_addr[13];
            pRawRule->care_bits.field[hitIndex - fieldIndexOffset] = val;

            break;
       case ACL_FIELD_IPV6_SIP0:
            if ((ret = _dal_rtl9601b_acl_ruleField_check(pTemplate, ACL_FIELD_IPV6_SIP0,fieldIndexStart, fieldIndexEnd, &hitIndex)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
                return ret;
            }
            val = pField->fieldUnion.ip6.value.ipv6_addr[14];
            val = (val << 8) | pField->fieldUnion.ip6.value.ipv6_addr[15];
            pRawRule->data_bits.field[hitIndex - fieldIndexOffset] = val;

            val = pField->fieldUnion.ip6.mask.ipv6_addr[14];
            val = (val << 8) | pField->fieldUnion.ip6.mask.ipv6_addr[15];
            pRawRule->care_bits.field[hitIndex - fieldIndexOffset] = val;

            break;
       case ACL_FIELD_IPV6_SIP1:
            if ((ret = _dal_rtl9601b_acl_ruleField_check(pTemplate, ACL_FIELD_IPV6_SIP1,fieldIndexStart, fieldIndexEnd, &hitIndex)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
                return ret;
            }
            val = pField->fieldUnion.ip6.value.ipv6_addr[12];
            val = (val << 8) | pField->fieldUnion.ip6.value.ipv6_addr[13];
            pRawRule->data_bits.field[hitIndex - fieldIndexOffset] = val;

            val = pField->fieldUnion.ip6.mask.ipv6_addr[12];
            val = (val << 8) | pField->fieldUnion.ip6.mask.ipv6_addr[13];
            pRawRule->care_bits.field[hitIndex - fieldIndexOffset] = val;

            break;
       case ACL_FIELD_IPV6_DIP:
            if ((ret = _dal_rtl9601b_acl_ruleField_check(pTemplate, ACL_FIELD_IPV6_DIP0,fieldIndexStart, fieldIndexEnd, &hitIndex)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
                return ret;
            }
            val = pField->fieldUnion.ip6.value.ipv6_addr[14];
            val = (val << 8) | pField->fieldUnion.ip6.value.ipv6_addr[15];
            pRawRule->data_bits.field[hitIndex - fieldIndexOffset] = val;

            val = pField->fieldUnion.ip6.mask.ipv6_addr[14];
            val = (val << 8) | pField->fieldUnion.ip6.mask.ipv6_addr[15];
            pRawRule->care_bits.field[hitIndex - fieldIndexOffset] = val;

            if ((ret = _dal_rtl9601b_acl_ruleField_check(pTemplate, ACL_FIELD_IPV6_DIP1,fieldIndexStart, fieldIndexEnd, &hitIndex)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
                return ret;
            }
            val = pField->fieldUnion.ip6.value.ipv6_addr[12];
            val = (val << 8) | pField->fieldUnion.ip6.value.ipv6_addr[13];
            pRawRule->data_bits.field[hitIndex - fieldIndexOffset] = val;

            val = pField->fieldUnion.ip6.mask.ipv6_addr[12];
            val = (val << 8) | pField->fieldUnion.ip6.mask.ipv6_addr[13];
            pRawRule->care_bits.field[hitIndex - fieldIndexOffset] = val;

            break;
       case ACL_FIELD_IPV6_DIP0:
            if ((ret = _dal_rtl9601b_acl_ruleField_check(pTemplate, ACL_FIELD_IPV6_DIP0,fieldIndexStart, fieldIndexEnd, &hitIndex)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
                return ret;
            }
            val = pField->fieldUnion.ip6.value.ipv6_addr[14];
            val = (val << 8) | pField->fieldUnion.ip6.value.ipv6_addr[15];
            pRawRule->data_bits.field[hitIndex - fieldIndexOffset] = val;

            val = pField->fieldUnion.ip6.mask.ipv6_addr[14];
            val = (val << 8) | pField->fieldUnion.ip6.mask.ipv6_addr[15];
            pRawRule->care_bits.field[hitIndex - fieldIndexOffset] = val;

            break;
       case ACL_FIELD_IPV6_DIP1:
            if ((ret = _dal_rtl9601b_acl_ruleField_check(pTemplate, ACL_FIELD_IPV6_DIP1,fieldIndexStart, fieldIndexEnd, &hitIndex)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
                return ret;
            }
            val = pField->fieldUnion.ip6.value.ipv6_addr[12];
            val = (val << 8) | pField->fieldUnion.ip6.value.ipv6_addr[13];
            pRawRule->data_bits.field[hitIndex - fieldIndexOffset] = val;

            val = pField->fieldUnion.ip6.mask.ipv6_addr[12];
            val = (val << 8) | pField->fieldUnion.ip6.mask.ipv6_addr[13];
            pRawRule->care_bits.field[hitIndex - fieldIndexOffset] = val;

            break;

        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    return RT_ERR_OK;
}

static int32 _raw_rtl9601b_acl_rule_set(rtl9601b_raw_acl_ruleEntry_t *pAclRule)
{
	int32 ret = RT_ERR_FAILED;
	uint32 i;
	uint32 fieldData[RTL9601B_ACLRULETBLEN];
    uint32 fieldMask[RTL9601B_ACLRULETBLEN];
    uint32 fieldData2[RTL9601B_ACLRULETBLEN];
    uint32 fieldMask2[RTL9601B_ACLRULETBLEN];
	uint32 tmpData;

	RT_PARAM_CHK(NULL == pAclRule , RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(1 < pAclRule->valid, RT_ERR_OUT_OF_RANGE);


    RT_PARAM_CHK(HAL_MAX_NUM_OF_ACL_RULE_ENTRY() < pAclRule->idx, RT_ERR_OUT_OF_RANGE);
    osal_memset(fieldData, 0, sizeof(fieldData));
    for (i=0; i<HAL_MAX_NUM_OF_ACL_RULE_FIELD(); i++)
    {
        if ((ret = table_field_set(RTL9601B_ACL_DATAt, RTL9601B_ACL_DATA_FIELD0tf-i, &pAclRule->data_bits.field[i], fieldData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
            return ret;
        }
    }
    if ((ret = table_field_set(RTL9601B_ACL_DATAt,RTL9601B_ACL_DATA_PMSKtf, &(pAclRule->data_bits.active_portmsk), fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_set(RTL9601B_ACL_DATAt, RTL9601B_ACL_DATA_TYPEtf, &(pAclRule->data_bits.type),fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    tmpData = pAclRule->data_bits.tag_exist & 0x7F;
    if ((ret = table_field_set(RTL9601B_ACL_DATAt, RTL9601B_ACL_DATA_TAGStf, &tmpData,fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    tmpData = 0;
    if ((ret = table_field_set(RTL9601B_ACL_DATAt, RTL9601B_ACL_DATA_RSVtf, &tmpData,fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    osal_memset(fieldMask,0,sizeof(fieldMask));
    for (i=0; i<HAL_MAX_NUM_OF_ACL_RULE_FIELD(); i++)
    {
        if ((ret = table_field_set(RTL9601B_ACL_MASKt, RTL9601B_ACL_MASK_FIELD0tf-i, &pAclRule->care_bits.field[i], fieldMask)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
            return ret;
        }

    }
    if ((ret = table_field_set(RTL9601B_ACL_MASKt, RTL9601B_ACL_MASK_PMSKtf, &(pAclRule->care_bits.active_portmsk), fieldMask)) != RT_ERR_OK)
    {
         RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
         return ret;
    }
    if ((ret = table_field_set(RTL9601B_ACL_MASKt, RTL9601B_ACL_MASK_TYPEtf, &(pAclRule->care_bits.type),fieldMask)) != RT_ERR_OK)
    {
         RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
         return ret;
    }

    tmpData = pAclRule->care_bits.tag_exist & 0x7F;
    if ((ret = table_field_set(RTL9601B_ACL_MASKt, RTL9601B_ACL_MASK_TAGStf, &tmpData,fieldMask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    tmpData = 0;
    if ((ret = table_field_set(RTL9601B_ACL_MASKt, RTL9601B_ACL_MASK_RSVtf, &tmpData,fieldMask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    for (i = 0; i < RTL9601B_ACLRULETBLEN; i++)
    {
        fieldData2[i] = fieldMask[i]&fieldData[i];
        fieldMask2[i] = fieldMask[i]&~fieldData[i];
    }

    if ((table_field_set(RTL9601B_ACL_DATAt, RTL9601B_ACL_DATA_VALIDtf, &pAclRule->valid, fieldData2)) != RT_ERR_OK)
    {
         RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
         return ret;
    }
    if ((ret = table_write(RTL9601B_ACL_DATAt, pAclRule->idx, fieldData2)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }
  
    if ((ret = table_write(RTL9601B_ACL_MASKt, pAclRule->idx, fieldMask2)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}

static int32 _raw_rtl9601b_acl_rule_get(rtl9601b_raw_acl_ruleEntry_t *pAclRule)
{
	int32 ret = RT_ERR_FAILED;
	uint32 i;
    uint32 fieldData[RTL9601B_ACLRULETBLEN];
    uint32 fieldMask[RTL9601B_ACLRULETBLEN];

	RT_PARAM_CHK(NULL == pAclRule, RT_ERR_NULL_POINTER);


    RT_PARAM_CHK(pAclRule->idx > HAL_MAX_NUM_OF_ACL_RULE_ENTRY(), RT_ERR_OUT_OF_RANGE);

    osal_memset(fieldData, 0x00, sizeof(fieldData));
    if ((ret = table_read(RTL9601B_ACL_DATAt, pAclRule->idx, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    for (i=0; i<HAL_MAX_NUM_OF_ACL_RULE_FIELD() ;i++)
	{
        if((ret = table_field_get(RTL9601B_ACL_DATAt, RTL9601B_ACL_DATA_FIELD0tf-i, &pAclRule->data_bits.field[i], fieldData)) != RT_ERR_OK)
  	    {
            RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
            return ret;
       	}
    }
	if ((ret = table_field_get(RTL9601B_ACL_DATAt, RTL9601B_ACL_DATA_PMSKtf, &(pAclRule->data_bits.active_portmsk), fieldData)) != RT_ERR_OK)
	{
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
	}

    if ((ret = table_field_get(RTL9601B_ACL_DATAt, RTL9601B_ACL_DATA_TYPEtf, &(pAclRule->data_bits.type), fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }
	if ((ret = table_field_get(RTL9601B_ACL_DATAt, RTL9601B_ACL_DATA_TAGStf, &(pAclRule->data_bits.tag_exist), fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    #if 0
	if ((ret = table_field_get(RTL9601B_ACL_DATAt, RTL9601B_ACL_DATA_RSVtf, &tmpData, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }
    #endif

    /*retrive valit bit from acl data bits*/
    if ((ret = table_field_get(RTL9601B_ACL_DATAt, RTL9601B_ACL_DATA_VALIDtf, &pAclRule->valid, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    osal_memset(fieldMask, 0x00, sizeof(fieldMask));
    if ((ret = table_read(RTL9601B_ACL_MASKt, pAclRule->idx, fieldMask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }
    for (i = 0; i < RTL9601B_ACLRULETBLEN; i++)
        fieldMask[i] = fieldMask[i]^fieldData[i];

  	for (i = 0; i < HAL_MAX_NUM_OF_ACL_RULE_FIELD(); i++)
    {
        if ((ret = table_field_get(RTL9601B_ACL_MASKt, RTL9601B_ACL_MASK_FIELD0tf-i, &pAclRule->care_bits.field[i], fieldMask)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
            return ret;

        }
    }
    if ((ret = table_field_get(RTL9601B_ACL_MASKt, RTL9601B_ACL_MASK_PMSKtf, &(pAclRule->care_bits.active_portmsk), fieldMask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_get(RTL9601B_ACL_MASKt, RTL9601B_ACL_MASK_TYPEtf, &(pAclRule->care_bits.type), fieldMask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }
	if ((ret = table_field_get(RTL9601B_ACL_MASKt, RTL9601B_ACL_MASK_TAGStf, &(pAclRule->care_bits.tag_exist), fieldMask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }
    #if 0
	if ((ret = table_field_get(RTL9601B_ACL_MASKt, RTL9601B_ACL_DATA_RSVtf, &tmpData, fieldMask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }
    #endif

	return RT_ERR_OK;
}

#if defined(CONFIG_SDK_KERNEL_LINUX)

uint32 dal_rtl9601b_acl_isr_entry(void)
{
    int32  ret;

    /* switch interrupt clear ACL state */
    if((ret=dal_rtl9601b_intr_ims_clear(INTR_TYPE_ACL_ACTION)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_ACL | MOD_DAL), "");
        return ret;
	}

	_actionIntCnt ++;
    
    return RT_ERR_OK; 
}


uint32 dal_rtl9601b_acl_isr_init(void)
{
    int32  ret;
  
	/*register ACL isr*/
	if((ret = rtk_irq_isr_register(INTR_TYPE_ACL_ACTION,dal_rtl9601b_acl_isr_entry)) != RT_ERR_OK)
	{
		RT_ERR(ret,(MOD_ACL | MOD_DAL), "");
		return ret;
	}

    if((ret = dal_rtl9601b_intr_imr_set(INTR_TYPE_ACL_ACTION,ENABLED)) != RT_ERR_OK)
    {
    	RT_ERR(ret,(MOD_ACL | MOD_DAL), "");
		return ret;
    }

    return RT_ERR_OK;    
}    
#endif


/* Function Name:
 *      dal_rtl9601b_acl_dbgInfo_get
 * Description:
 *      Get ACL debug information
 * Input:
 *	  intrType: type of interrupt
 * Output:
 *      pDbgCnt: point of relative debug counter
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 dal_rtl9601b_acl_dbgInfo_get(rtk_acl_dbgCnt_t *pDbgCnt)
{
    /* check Init status */
    RT_INIT_CHK(rtl9601b_acl_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pDbgCnt), RT_ERR_NULL_POINTER);

    /* function body */
    pDbgCnt->actionIntCnt = _actionIntCnt;

    /*reset counter*/
    _actionIntCnt=0;

    return RT_ERR_OK;
}   /* end of dal_rtl9601b_acl_dbgInfo_get */

/* Function Name:
 *      dal_rtl9601b_acl_init
 * Description:
 *      Initialize ACL module.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize ACL module before calling any ACL APIs.
 *      Apollo init acl mode as ACL_IGR_RULE_MODE_0 and clear all entries
 */
int32
dal_rtl9601b_acl_init(void)
{
    int32   ret;
    uint32  val;
    uint32  index;
    uint32  fieldIndex;
    rtk_enable_t state;
    rtk_port_t port;
	rtk_acl_field_entry_t fieldSelect;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_ACL),"%s",__FUNCTION__);

    rtl9601b_acl_init = INIT_COMPLETED;

    HAL_SCAN_ALL_PORT(port)
    {
        state = DISABLED;
        if ((ret = dal_rtl9601b_acl_igrState_set(port, state)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
            rtl9601b_acl_init = INIT_NOT_COMPLETED;
            return ret;
        }

        state = ENABLED;
        if ((ret = dal_rtl9601b_acl_igrUnmatchAction_set(port, state)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
            rtl9601b_acl_init = INIT_NOT_COMPLETED;
            return ret;
        }
    }

    for(index = 0; index < HAL_MAX_NUM_OF_ACL_TEMPLATE(); index ++)
    {
        for(fieldIndex = 0; fieldIndex < HAL_MAX_NUM_OF_ACL_RULE_FIELD(); fieldIndex ++)
        {
            val = (uint32)rtl9601b_dal_template_init[index][fieldIndex];

    	    if ((ret = reg_array_field_write(RTL9601B_ACL_TEMPLATE_CTRLr, index, fieldIndex, RTL9601B_FIELDf, &val)) != RT_ERR_OK )
            {
                RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
                rtl9601b_acl_init = INIT_NOT_COMPLETED;
                return ret;
            }
        }
    }


	fieldSelect.offset = 0;
	fieldSelect.format = ACL_FORMAT_DEFAULT;
    for(fieldSelect.index = 0; fieldSelect.index < HAL_MAX_NUM_OF_FIELD_SELECTOR(); fieldSelect.index ++)
    {
	    if ((ret = dal_rtl9601b_acl_fieldSelect_set(&fieldSelect)) != RT_ERR_OK )
        {
            RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
            rtl9601b_acl_init = INIT_NOT_COMPLETED;
            return ret;
        }
    }

    if ((ret = dal_rtl9601b_acl_igrRuleEntry_delAll()) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        rtl9601b_acl_init = INIT_NOT_COMPLETED;
        return ret;
    }
	
#if defined(CONFIG_SDK_KERNEL_LINUX)
    if ((ret = dal_rtl9601b_acl_isr_init()) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL | MOD_DAL), "");
        return ret;
    } 
#endif

    return RT_ERR_OK;
} /* end of dal_rtl9601b_acl_init */

/* Function Name:
 *      dal_rtl9601b_acl_template_set
 * Description:
 *      Set template of ingress ACL.
 * Input:
 *      pTemplate - Ingress ACL template
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_INPUT           - Invalid input parameters.
 * Note:
 *      This function set ACL template.
 */
int32
dal_rtl9601b_acl_template_set(rtk_acl_template_t *pTemplate)
{
    int32   ret;
    uint32  index;
    uint32  fieldIndex;
    uint32  val;
    uint32  type;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_ACL),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9601b_acl_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pTemplate), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pTemplate->index >= HAL_MAX_NUM_OF_ACL_TEMPLATE()), RT_ERR_OUT_OF_RANGE);

    index = pTemplate->index;

	for (fieldIndex = 0; fieldIndex < HAL_MAX_NUM_OF_ACL_RULE_FIELD(); fieldIndex ++)
    {
        /*change RTK to DAL definition*/
        type = pTemplate->fieldType[fieldIndex];

        switch(type)
        {
            case ACL_FIELD_UNUSED:

                val = RTL9601B_DAL_ACL_UNUSED;
                break;
            case ACL_FIELD_DMAC0:

                val = RTL9601B_DAL_ACL_DMAC0;
                break;
            case ACL_FIELD_DMAC1:

                val = RTL9601B_DAL_ACL_DMAC1;
                break;
            case ACL_FIELD_DMAC2:

                val = RTL9601B_DAL_ACL_DMAC2;
                break;
            case ACL_FIELD_SMAC0:

                val = RTL9601B_DAL_ACL_SMAC0;
                break;
            case ACL_FIELD_SMAC1:

                val = RTL9601B_DAL_ACL_SMAC1;
                break;
            case ACL_FIELD_SMAC2:

                val = RTL9601B_DAL_ACL_SMAC2;
                break;
            case ACL_FIELD_ETHERTYPE:

                val = RTL9601B_DAL_ACL_ETHERTYPE;
                break;
            case ACL_FIELD_CTAG:

                val = RTL9601B_DAL_ACL_CTAG;
                break;
            case ACL_FIELD_STAG:

                val = RTL9601B_DAL_ACL_STAG;
                break;
            case ACL_FIELD_GEMPORT:

                val = RTL9601B_DAL_ACL_GEMIDXLLIDX;
                break;
            case ACL_FIELD_IPV4_SIP0:

                val = RTL9601B_DAL_ACL_IP4SIP0;
                break;
            case ACL_FIELD_IPV4_SIP1:

                val = RTL9601B_DAL_ACL_IP4SIP1;
                break;
            case ACL_FIELD_IPV4_DIP0:

                val = RTL9601B_DAL_ACL_IP4DIP0;
                break;
            case ACL_FIELD_IPV4_DIP1:

                val = RTL9601B_DAL_ACL_IP4DIP1;
                break;
            case ACL_FIELD_IPV6_SIP0:

                val = RTL9601B_DAL_ACL_IP6SIP0WITHIPV4;
                break;
            case ACL_FIELD_IPV6_SIP1:

                val = RTL9601B_DAL_ACL_IP6SIP1WITHIPV4;
                break;
            case ACL_FIELD_IPV6_DIP0:

                val = RTL9601B_DAL_ACL_IP6DIP0WITHIPV4;
                break;
            case ACL_FIELD_IPV6_DIP1:

                val = RTL9601B_DAL_ACL_IP6DIP1WITHIPV4;
                break;
#if 0
            case ACL_FIELD_IPV6_NEXT_HEADER:

                val = RTL9601B_DAL_ACL_IP6NH;
                break;
#endif
            case ACL_FIELD_VID_RANGE:

                val = RTL9601B_DAL_ACL_VIDRANGE;
                break;
            case ACL_FIELD_IP_RANGE:

                val = RTL9601B_DAL_ACL_IPRANGE;
                break;
            case ACL_FIELD_PORT_RANGE:

                val = RTL9601B_DAL_ACL_PORTRANGE;
                break;
            case ACL_FIELD_PKT_LEN_RANGE:

                val = RTL9601B_DAL_ACL_PKTLENRANGE;
                break;
            case ACL_FIELD_USER_VALID:

                val = RTL9601B_DAL_ACL_FIELD_VALID;
                break;
            case ACL_FIELD_USER_DEFINED00:

                val = RTL9601B_DAL_ACL_FIELD_SELECT00;
                break;
            case ACL_FIELD_USER_DEFINED01:

                val = RTL9601B_DAL_ACL_FIELD_SELECT01;
                break;
            case ACL_FIELD_USER_DEFINED02:

                val = RTL9601B_DAL_ACL_FIELD_SELECT02;
                break;
            case ACL_FIELD_USER_DEFINED03:

                val = RTL9601B_DAL_ACL_FIELD_SELECT03;
                break;
            case ACL_FIELD_USER_DEFINED04:

                val = RTL9601B_DAL_ACL_FIELD_SELECT04;
                break;
            case ACL_FIELD_USER_DEFINED05:

                val = RTL9601B_DAL_ACL_FIELD_SELECT05;
                break;
            case ACL_FIELD_USER_DEFINED06:

                val = RTL9601B_DAL_ACL_FIELD_SELECT06;
                break;
            case ACL_FIELD_USER_DEFINED07:

                val = RTL9601B_DAL_ACL_FIELD_SELECT07;
                break;
            case ACL_FIELD_USER_DEFINED08:

                val = RTL9601B_DAL_ACL_FIELD_SELECT08;
                break;
            case ACL_FIELD_USER_DEFINED09:

                val = RTL9601B_DAL_ACL_FIELD_SELECT09;
                break;
            case ACL_FIELD_USER_DEFINED10:

                val = RTL9601B_DAL_ACL_FIELD_SELECT10;
                break;
            case ACL_FIELD_USER_DEFINED11:

                val = RTL9601B_DAL_ACL_FIELD_SELECT11;
                break;
            case ACL_FIELD_USER_DEFINED12:

                val = RTL9601B_DAL_ACL_FIELD_SELECT12;
                break;
            case ACL_FIELD_USER_DEFINED13:

                val = RTL9601B_DAL_ACL_FIELD_SELECT13;
                break;
            case ACL_FIELD_USER_DEFINED14:

                val = RTL9601B_DAL_ACL_FIELD_SELECT14;
                break;
            case ACL_FIELD_USER_DEFINED15:

                val = RTL9601B_DAL_ACL_FIELD_SELECT15;
                break;
            default:

                return RT_ERR_CHIP_NOT_SUPPORTED;
        }

        if((ret = reg_array_field_write(RTL9601B_ACL_TEMPLATE_CTRLr, index, fieldIndex, RTL9601B_FIELDf, &val)) != RT_ERR_OK )
        {
            RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
            return ret;
        }
    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_acl_template_set */


/* Function Name:
 *      dal_rtl9601b_acl_template_get
 * Description:
 *      Get template of ingress ACL.
 * Input:
 *      pTemplate - Ingress ACL template
 * Output:
 *      pTemplate - Ingress ACL template
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_INPUT           - Invalid input parameters.
 * Note:
 *      This function get ACL template.
 */
int32
dal_rtl9601b_acl_template_get(rtk_acl_template_t *pTemplate)
{
    int32   ret;
    uint32  index;
    uint32  val;
    uint32  fieldIndex;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_ACL),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9601b_acl_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pTemplate), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pTemplate->index >= HAL_MAX_NUM_OF_ACL_TEMPLATE()), RT_ERR_OUT_OF_RANGE);

    index = pTemplate->index;

    for (fieldIndex = 0; fieldIndex < HAL_MAX_NUM_OF_ACL_RULE_FIELD(); fieldIndex ++)
    {
        if ((ret = reg_array_field_read(RTL9601B_ACL_TEMPLATE_CTRLr, index, fieldIndex, RTL9601B_FIELDf, &val)) != RT_ERR_OK )
        {
            RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
            return ret;
        }

        /*change RTK to DAL definition*/
        switch(val)
        {
            case RTL9601B_DAL_ACL_UNUSED:

                pTemplate->fieldType[fieldIndex] = ACL_FIELD_UNUSED;
                break;
            case RTL9601B_DAL_ACL_DMAC0:

                pTemplate->fieldType[fieldIndex] = ACL_FIELD_DMAC0;
                break;
            case RTL9601B_DAL_ACL_DMAC1:

                pTemplate->fieldType[fieldIndex] = ACL_FIELD_DMAC1;
                break;
            case RTL9601B_DAL_ACL_DMAC2:

                pTemplate->fieldType[fieldIndex] = ACL_FIELD_DMAC2;
                break;
            case RTL9601B_DAL_ACL_SMAC0:

                pTemplate->fieldType[fieldIndex] = ACL_FIELD_SMAC0;
                break;
            case RTL9601B_DAL_ACL_SMAC1:

                pTemplate->fieldType[fieldIndex] = ACL_FIELD_SMAC1;
                break;
            case RTL9601B_DAL_ACL_SMAC2:

                pTemplate->fieldType[fieldIndex] = ACL_FIELD_SMAC2;
                break;
            case RTL9601B_DAL_ACL_ETHERTYPE:

                pTemplate->fieldType[fieldIndex] = ACL_FIELD_ETHERTYPE;
                break;
            case RTL9601B_DAL_ACL_CTAG:

                pTemplate->fieldType[fieldIndex] = ACL_FIELD_CTAG;
                break;
            case RTL9601B_DAL_ACL_STAG:

                pTemplate->fieldType[fieldIndex] = ACL_FIELD_STAG;
                break;
            case RTL9601B_DAL_ACL_GEMIDXLLIDX:

                pTemplate->fieldType[fieldIndex] = ACL_FIELD_GEMPORT;
                break;
            case RTL9601B_DAL_ACL_IP4SIP0:

                pTemplate->fieldType[fieldIndex] = ACL_FIELD_IPV4_SIP0;
                break;
            case RTL9601B_DAL_ACL_IP4SIP1:

                pTemplate->fieldType[fieldIndex] = ACL_FIELD_IPV4_SIP1;
                break;
            case RTL9601B_DAL_ACL_IP4DIP0:

                pTemplate->fieldType[fieldIndex] = ACL_FIELD_IPV4_DIP0;
                break;
            case RTL9601B_DAL_ACL_IP4DIP1:

                pTemplate->fieldType[fieldIndex] = ACL_FIELD_IPV4_DIP1;
                break;
            case RTL9601B_DAL_ACL_IP6SIP0WITHIPV4:

                pTemplate->fieldType[fieldIndex] = ACL_FIELD_IPV6_SIP0;
                break;
            case RTL9601B_DAL_ACL_IP6SIP1WITHIPV4:

                pTemplate->fieldType[fieldIndex] = ACL_FIELD_IPV6_SIP1;
                break;
            case RTL9601B_DAL_ACL_IP6DIP0WITHIPV4:

                pTemplate->fieldType[fieldIndex] = ACL_FIELD_IPV6_DIP0;
                break;
            case RTL9601B_DAL_ACL_IP6DIP1WITHIPV4:

                pTemplate->fieldType[fieldIndex] = ACL_FIELD_IPV6_DIP1;
                break;
#if 0				
            case RTL9601B_DAL_ACL_IP6NH:

                pTemplate->fieldType[fieldIndex] = ACL_FIELD_IPV6_NEXT_HEADER;
                break;
#endif
			case RTL9601B_DAL_ACL_VIDRANGE:

                pTemplate->fieldType[fieldIndex] = ACL_FIELD_VID_RANGE;
                break;
            case RTL9601B_DAL_ACL_IPRANGE:

                pTemplate->fieldType[fieldIndex] = ACL_FIELD_IP_RANGE;
                break;
            case RTL9601B_DAL_ACL_PORTRANGE:

                pTemplate->fieldType[fieldIndex] = ACL_FIELD_PORT_RANGE;
                break;
            case RTL9601B_DAL_ACL_PKTLENRANGE:

                pTemplate->fieldType[fieldIndex] = ACL_FIELD_PKT_LEN_RANGE;
                break;
            case RTL9601B_DAL_ACL_FIELD_VALID:

                pTemplate->fieldType[fieldIndex] = ACL_FIELD_USER_VALID;
                break;

            case RTL9601B_DAL_ACL_FIELD_SELECT00:

                pTemplate->fieldType[fieldIndex] = ACL_FIELD_USER_DEFINED00;
                break;
            case RTL9601B_DAL_ACL_FIELD_SELECT01:

                pTemplate->fieldType[fieldIndex] = ACL_FIELD_USER_DEFINED01;
                break;
            case RTL9601B_DAL_ACL_FIELD_SELECT02:

                pTemplate->fieldType[fieldIndex] = ACL_FIELD_USER_DEFINED02;
                break;
            case RTL9601B_DAL_ACL_FIELD_SELECT03:

                pTemplate->fieldType[fieldIndex] = ACL_FIELD_USER_DEFINED03;
                break;
            case RTL9601B_DAL_ACL_FIELD_SELECT04:

                pTemplate->fieldType[fieldIndex] = ACL_FIELD_USER_DEFINED04;
                break;
            case RTL9601B_DAL_ACL_FIELD_SELECT05:

                pTemplate->fieldType[fieldIndex] = ACL_FIELD_USER_DEFINED05;
                break;
            case RTL9601B_DAL_ACL_FIELD_SELECT06:

                pTemplate->fieldType[fieldIndex] = ACL_FIELD_USER_DEFINED06;
                break;
            case RTL9601B_DAL_ACL_FIELD_SELECT07:

                pTemplate->fieldType[fieldIndex] = ACL_FIELD_USER_DEFINED07;
                break;
            case RTL9601B_DAL_ACL_FIELD_SELECT08:

                pTemplate->fieldType[fieldIndex] = ACL_FIELD_USER_DEFINED08;
                break;
            case RTL9601B_DAL_ACL_FIELD_SELECT09:

                pTemplate->fieldType[fieldIndex] = ACL_FIELD_USER_DEFINED09;
                break;
            case RTL9601B_DAL_ACL_FIELD_SELECT10:

                pTemplate->fieldType[fieldIndex] = ACL_FIELD_USER_DEFINED10;
                break;
            case RTL9601B_DAL_ACL_FIELD_SELECT11:

                pTemplate->fieldType[fieldIndex] = ACL_FIELD_USER_DEFINED11;
                break;
            case RTL9601B_DAL_ACL_FIELD_SELECT12:

                pTemplate->fieldType[fieldIndex] = ACL_FIELD_USER_DEFINED12;
                break;
            case RTL9601B_DAL_ACL_FIELD_SELECT13:

                pTemplate->fieldType[fieldIndex] = ACL_FIELD_USER_DEFINED13;
                break;
            case RTL9601B_DAL_ACL_FIELD_SELECT14:

                pTemplate->fieldType[fieldIndex] = ACL_FIELD_USER_DEFINED14;
                break;
            case RTL9601B_DAL_ACL_FIELD_SELECT15:

                pTemplate->fieldType[fieldIndex] = ACL_FIELD_USER_DEFINED15;
                break;
            default:

                return RT_ERR_CHIP_NOT_SUPPORTED;
        }
    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_acl_template_get */

/* Function Name:
 *      dal_rtl9601b_acl_fieldSelect_set
 * Description:
 *      Set user defined field selectors in HSB
 * Input:
 *      pFieldEntry 	- pointer of field selector entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 * Note:
 *      System support 16 user defined field selctors.
 * 		Each selector can be enabled or disable.
 *      User can defined retrieving 16-bits in many predefiend
 * 		standard l2/l3/l4 payload.
 */
int32
dal_rtl9601b_acl_fieldSelect_set(rtk_acl_field_entry_t *pFieldEntry)
{
    int32   ret;
    uint32  offset;
    uint32  format;


    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_ACL),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9601b_acl_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pFieldEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(pFieldEntry->index >= HAL_MAX_NUM_OF_FIELD_SELECTOR(), RT_ERR_OUT_OF_RANGE);

    switch(pFieldEntry->format)
    {
        case ACL_FORMAT_DEFAULT:

            format = RTL9601B_DAL_FIELD_FORMAT_DEFAULT;
            break;
        case ACL_FORMAT_RAW:

            format = RTL9601B_DAL_FIELD_FORMAT_RAW;
            break;
        case ACL_FORMAT_LLC:

            format = RTL9601B_DAL_FIELD_FORMAT_LLC;
            break;
        case ACL_FORMAT_IPV4:

            format = RTL9601B_DAL_FIELD_FORMAT_IPV4;
            break;
        case ACL_FORMAT_ARP:

            format = RTL9601B_DAL_FIELD_FORMAT_ARP;
            break;
        case ACL_FORMAT_IPV6:

            format = RTL9601B_DAL_FIELD_FORMAT_IPV6;
            break;
        case ACL_FORMAT_IPPAYLOAD:

            format = RTL9601B_DAL_FIELD_FORMAT_IPPAYLOAD;
            break;
        case ACL_FORMAT_L4PAYLOAD:

            format = RTL9601B_DAL_FIELD_FORMAT_L4PAYLOAD;
            break;

        default:

            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    offset = pFieldEntry->offset;

    if ((ret = reg_array_field_write(RTL9601B_PARSER_FIELD_SELTOR_CTRLr, REG_ARRAY_INDEX_NONE, pFieldEntry->index, RTL9601B_OFFSETf, &offset)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
    }

    if ((ret = reg_array_field_write(RTL9601B_PARSER_FIELD_SELTOR_CTRLr, REG_ARRAY_INDEX_NONE,  pFieldEntry->index, RTL9601B_FMTf, &format)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_acl_fieldSelect_set */

/* Function Name:
 *      dal_rtl9601b_acl_fieldSelect_get
 * Description:
 *      Get user defined field selectors in HSB
 * Input:
 *      None
 * Output:
 *      pFieldEntry 	- pointer of field selector entry
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 * Note:
 *      None.
 */
int32
dal_rtl9601b_acl_fieldSelect_get(rtk_acl_field_entry_t *pFieldEntry)
{
    int32   ret;
    uint32  offset;
    uint32  format;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_ACL),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9601b_acl_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pFieldEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(pFieldEntry->index >= HAL_MAX_NUM_OF_FIELD_SELECTOR(), RT_ERR_OUT_OF_RANGE);

    if ((ret = reg_array_field_read(RTL9601B_PARSER_FIELD_SELTOR_CTRLr, REG_ARRAY_INDEX_NONE, pFieldEntry->index, RTL9601B_OFFSETf, &offset)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
    }
    pFieldEntry->offset = offset;

    if ((ret = reg_array_field_read(RTL9601B_PARSER_FIELD_SELTOR_CTRLr, REG_ARRAY_INDEX_NONE, pFieldEntry->index, RTL9601B_FMTf, &format)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
    }

    switch(format)
    {
        case RTL9601B_DAL_FIELD_FORMAT_DEFAULT:

            pFieldEntry->format = ACL_FORMAT_DEFAULT;
            break;
        case RTL9601B_DAL_FIELD_FORMAT_RAW:

            pFieldEntry->format = ACL_FORMAT_RAW;
            break;
        case RTL9601B_DAL_FIELD_FORMAT_LLC:

            pFieldEntry->format = ACL_FORMAT_LLC;
            break;
        case RTL9601B_DAL_FIELD_FORMAT_IPV4:

            pFieldEntry->format = ACL_FORMAT_IPV4;
            break;
        case RTL9601B_DAL_FIELD_FORMAT_ARP:

            pFieldEntry->format = ACL_FORMAT_ARP;
            break;
        case RTL9601B_DAL_FIELD_FORMAT_IPV6:

            pFieldEntry->format = ACL_FORMAT_IPV6;
            break;
        case RTL9601B_DAL_FIELD_FORMAT_IPPAYLOAD:

            pFieldEntry->format = ACL_FORMAT_IPPAYLOAD;
            break;
        case RTL9601B_DAL_FIELD_FORMAT_L4PAYLOAD:

            pFieldEntry->format = ACL_FORMAT_L4PAYLOAD;
            break;

        default:

            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_acl_fieldSelect_get */
/* Function Name:
 *      dal_rtl9601b_acl_igrRuleAction_set
 * Description:
 *      Get an ACL action from ASIC
 * Input:
 *      pAclRule     - The ACL configuration that this function will add comparison rule
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK              					- OK
 *      RT_ERR_FAILED          					- Failed
 *      RT_ERR_NULL_POINTER    					- Pointer pAclRule point to NULL.
 *      RT_ERR_INPUT 							- Invalid input parameters.
 * Note:
 *      use this API to set action control and data
 */
int32
dal_rtl9601b_acl_igrRuleAction_set(rtk_acl_ingress_entry_t *pAclRule)
{
    int32  ret;
    uint32 val;
    uint32 index;
    uint32 actionData[RTL9601B_ACL_ACTION_DATA_LEN];
    uint32 valAct;
	rtk_acl_igr_act_type_t checkIdx;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_ACL),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9601b_acl_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pAclRule), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pAclRule->index >= HAL_MAX_NUM_OF_ACL_ACTION()), RT_ERR_OUT_OF_RANGE);

	//action type supporting checking
	for(checkIdx = ACL_IGR_INTR_ACT; checkIdx < ACL_IGR_ACT_END; checkIdx ++)
	{
		if(ENABLED == pAclRule->act.enableAct[checkIdx])
		{
			switch(checkIdx)
			{
				case ACL_IGR_CVLAN_ACT:
				case ACL_IGR_SVLAN_ACT:
				case ACL_IGR_PRI_ACT:
				case ACL_IGR_LOG_ACT:
				case ACL_IGR_FORWARD_ACT:
				case ACL_IGR_INTR_ACT:
					break;
				default:
					return RT_ERR_CHIP_NOT_SUPPORTED;
					break;
			}
		}
	}

    index = (uint32)pAclRule->index;

    val = pAclRule->invert;
    if ((ret = reg_array_field_write(RTL9601B_ACL_ACTIONr, REG_ARRAY_INDEX_NONE, index, RTL9601B_NOTf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    val = pAclRule->act.enableAct[ACL_IGR_INTR_ACT];
    if ((ret = reg_array_field_write(RTL9601B_ACL_ACTIONr, REG_ARRAY_INDEX_NONE, index, RTL9601B_INT_CFf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    val = pAclRule->act.enableAct[ACL_IGR_FORWARD_ACT];
    if ((ret = reg_array_field_write(RTL9601B_ACL_ACTIONr, REG_ARRAY_INDEX_NONE, index, RTL9601B_FWDf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    val = pAclRule->act.enableAct[ACL_IGR_LOG_ACT];
    if ((ret = reg_array_field_write(RTL9601B_ACL_ACTIONr, REG_ARRAY_INDEX_NONE, index, RTL9601B_POLICINGf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    val = pAclRule->act.enableAct[ACL_IGR_PRI_ACT];
    if ((ret = reg_array_field_write(RTL9601B_ACL_ACTIONr, REG_ARRAY_INDEX_NONE, index, RTL9601B_PRIf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    val = pAclRule->act.enableAct[ACL_IGR_SVLAN_ACT];
    if ((ret = reg_array_field_write(RTL9601B_ACL_ACTIONr, REG_ARRAY_INDEX_NONE, index, RTL9601B_RMK_DSCPf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    val = pAclRule->act.enableAct[ACL_IGR_CVLAN_ACT];
    if ((ret = reg_array_field_write(RTL9601B_ACL_ACTIONr, REG_ARRAY_INDEX_NONE, index, RTL9601B_RMK_1Pf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    /*retrieve action configuration*/
	osal_memset(actionData,0,sizeof(actionData));


    /*Interrupt and extend action control*/
	if(pAclRule->act.enableAct[ACL_IGR_INTR_ACT])
	{

	    val = pAclRule->act.aclLatch;
		if ((ret = table_field_set(RTL9601B_ACL_ACTION_TABLEt, RTL9601B_ACL_ACTION_TABLE_HITtf, &val, actionData)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
	        return ret;
	    }

	    val = pAclRule->act.aclInterrupt;
		if ((ret = table_field_set(RTL9601B_ACL_ACTION_TABLEt, RTL9601B_ACL_ACTION_TABLE_ACLINTtf, &val, actionData)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
	        return ret;
	    }
	}
    /*Priority action control*/
    if(pAclRule->act.enableAct[ACL_IGR_PRI_ACT])
    {
	    val = pAclRule->act.priAct.act;
		if ((ret = table_field_set(RTL9601B_ACL_ACTION_TABLEt, RTL9601B_ACL_ACTION_TABLE_ACLPRItf, &val, actionData)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
	        return ret;
	    }

	    switch(pAclRule->act.priAct.act)
	    {
	        case ACL_IGR_PRI_ACL_PRI_ASSIGN_ACT:

	            val = pAclRule->act.priAct.aclPri;
	            break;
			default:
				return RT_ERR_CHIP_NOT_SUPPORTED;
				break;
	    }

		if ((ret = table_field_set(RTL9601B_ACL_ACTION_TABLEt, RTL9601B_ACL_ACTION_TABLE_ACLPRItf, &val, actionData)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
	        return ret;
	    }
    }

    /*Forward action control*/
    if(pAclRule->act.enableAct[ACL_IGR_FORWARD_ACT])
    {
	    switch(pAclRule->act.forwardAct.act)
	    {
	        case ACL_IGR_FORWARD_COPY_ACT:
                val = RTL9601B_IGR_FORWARD_COPY;
                break;
	        case ACL_IGR_FORWARD_DROP_ACT:
                val = RTL9601B_IGR_FORWARD_DROP;
                break;
	        case ACL_IGR_FORWARD_TRAP_ACT:
                val = RTL9601B_IGR_FORWARD_TRAP;
                break;
			case ACL_IGR_FORWARD_NOP_ACT:
				val = RTL9601B_IGR_FORWARD_NOP;
				break;
			default:
				return RT_ERR_CHIP_NOT_SUPPORTED;
				break;
	    }

		if ((ret = table_field_set(RTL9601B_ACL_ACTION_TABLEt, RTL9601B_ACL_ACTION_TABLE_FWDACTtf, &val, actionData)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
	        return ret;
	    }
   	}

    /*Logging or Policing action control*/
    if(pAclRule->act.enableAct[ACL_IGR_LOG_ACT])
    {
	    switch(pAclRule->act.logAct.act)
	    {
	        case ACL_IGR_LOG_POLICING_ACT:
				valAct = RTL9601B_DAL_ACL_IGR_LOG_POLICING_ACT;
	            val = pAclRule->act.logAct.meter;
	            break;

	        case ACL_IGR_LOG_MIB_ACT:
				valAct = RTL9601B_DAL_ACL_IGR_LOG_MIB_ACT;
	            val = pAclRule->act.logAct.mib;	
	            break;

			default:
				return RT_ERR_CHIP_NOT_SUPPORTED;
				break;
	    }

		if ((ret = table_field_set(RTL9601B_ACL_ACTION_TABLEt, RTL9601B_ACL_ACTION_TABLE_POLICACTtf, &valAct, actionData)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
	        return ret;
	    }

		if ((ret = table_field_set(RTL9601B_ACL_ACTION_TABLEt, RTL9601B_ACL_ACTION_TABLE_METER_IDXtf, &val, actionData)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
	        return ret;
	    }
    }

    /*svlan action control*/
    if(pAclRule->act.enableAct[ACL_IGR_SVLAN_ACT])
    {
	    switch(pAclRule->act.svlanAct.act)
	    {
	        case ACL_IGR_SVLAN_DSCP_REMARK_ACT:

	            val = pAclRule->act.svlanAct.dscp;
	            break;
	        default:
				return RT_ERR_CHIP_NOT_SUPPORTED;
				break;
	    }

	    if ((ret = table_field_set(RTL9601B_ACL_ACTION_TABLEt, RTL9601B_ACL_ACTION_TABLE_RMK_DSCPtf, &val, actionData)) != RT_ERR_OK)
		{
				RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
				return ret;
		}
    }

    /*cvlan action control*/
    if(pAclRule->act.enableAct[ACL_IGR_CVLAN_ACT])
    {
	    switch(pAclRule->act.cvlanAct.act)
	    {
	        case ACL_IGR_CVLAN_1P_REMARK_ACT:

	            val = pAclRule->act.cvlanAct.dot1p;
	            break;

	        default:
				return RT_ERR_CHIP_NOT_SUPPORTED;
				break;
	    }

	    if ((ret = table_field_set(RTL9601B_ACL_ACTION_TABLEt, RTL9601B_ACL_ACTION_TABLE_RMK_1Ptf, &val, actionData)) != RT_ERR_OK)
		{
				RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
				return ret;
		}
    }

	if ((ret = table_write(RTL9601B_ACL_ACTION_TABLEt, index, actionData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_acl_igrRuleAction_set */


/* Function Name:
 *      dal_rtl9601b_acl_igrRuleAction_get
 * Description:
 *      Get an ACL action from ASIC
 * Input:
 *      None.
 * Output:
 *      pAclRule     - The ACL configuration that this function will add comparison rule
 * Return:
 *      RT_ERR_OK              					- OK
 *      RT_ERR_FAILED          					- Failed
 *      RT_ERR_NULL_POINTER    					- Pointer pAclRule point to NULL.
 *      RT_ERR_INPUT 							- Invalid input parameters.
 * Note:
 *      use this API to get action control and data
 */
int32
dal_rtl9601b_acl_igrRuleAction_get(rtk_acl_ingress_entry_t *pAclRule)
{
    int32  ret;
    uint32 val;
    uint32 index;
    uint32 actionData[RTL9601B_ACL_ACTION_DATA_LEN];
    uint32 valAct;

    /* check Init status */
    RT_INIT_CHK(rtl9601b_acl_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pAclRule), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pAclRule->index >= HAL_MAX_NUM_OF_ACL_ACTION()), RT_ERR_OUT_OF_RANGE);

    index = (uint32)pAclRule->index;

    if ((ret = reg_array_field_read(RTL9601B_ACL_ACTIONr, REG_ARRAY_INDEX_NONE, index, RTL9601B_NOTf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }
    pAclRule->invert = (rtk_acl_invert_t)val;

    if ((ret = reg_array_field_read(RTL9601B_ACL_ACTIONr, REG_ARRAY_INDEX_NONE, index, RTL9601B_INT_CFf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }
    pAclRule->act.enableAct[ACL_IGR_INTR_ACT] = (rtk_enable_t)val;

    if ((ret = reg_array_field_read(RTL9601B_ACL_ACTIONr, REG_ARRAY_INDEX_NONE, index, RTL9601B_FWDf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }
    pAclRule->act.enableAct[ACL_IGR_FORWARD_ACT] = (rtk_enable_t)val;

    if ((ret = reg_array_field_read(RTL9601B_ACL_ACTIONr, REG_ARRAY_INDEX_NONE, index, RTL9601B_POLICINGf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }
    pAclRule->act.enableAct[ACL_IGR_LOG_ACT] = (rtk_enable_t)val;

    if ((ret = reg_array_field_read(RTL9601B_ACL_ACTIONr, REG_ARRAY_INDEX_NONE, index, RTL9601B_PRIf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }
    pAclRule->act.enableAct[ACL_IGR_PRI_ACT] = (rtk_enable_t)val;

    if ((ret = reg_array_field_read(RTL9601B_ACL_ACTIONr, REG_ARRAY_INDEX_NONE, index, RTL9601B_RMK_DSCPf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }
    pAclRule->act.enableAct[ACL_IGR_SVLAN_ACT] = (rtk_enable_t)val;

    if ((ret = reg_array_field_read(RTL9601B_ACL_ACTIONr, REG_ARRAY_INDEX_NONE, index, RTL9601B_RMK_1Pf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }
    pAclRule->act.enableAct[ACL_IGR_CVLAN_ACT] = (rtk_enable_t)val;

    /*retrieve action configuration*/
	osal_memset(actionData,0,sizeof(actionData));

	if ((ret = table_read(RTL9601B_ACL_ACTION_TABLEt, index, actionData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    /*Interrupt and extend action control*/

	if ((ret = table_field_get(RTL9601B_ACL_ACTION_TABLEt, RTL9601B_ACL_ACTION_TABLE_HITtf, &val, actionData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }
    pAclRule->act.aclLatch = (rtk_enable_t)val;

	if ((ret = table_field_get(RTL9601B_ACL_ACTION_TABLEt, RTL9601B_ACL_ACTION_TABLE_ACLINTtf, &val, actionData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }
    pAclRule->act.aclInterrupt = (rtk_enable_t)val;

    /*Priority action control*/
	if ((ret = table_field_get(RTL9601B_ACL_ACTION_TABLEt, RTL9601B_ACL_ACTION_TABLE_ACLPRItf, &val, actionData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    pAclRule->act.priAct.act = ACL_IGR_PRI_ACL_PRI_ASSIGN_ACT;
    pAclRule->act.priAct.aclPri = (uint8)val;

    /*Forward action control*/
	if ((ret = table_field_get(RTL9601B_ACL_ACTION_TABLEt, RTL9601B_ACL_ACTION_TABLE_FWDACTtf, &val, actionData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    switch(val)
    {
        case RTL9601B_IGR_FORWARD_COPY:
            pAclRule->act.forwardAct.act = ACL_IGR_FORWARD_COPY_ACT;
            break;
        case RTL9601B_IGR_FORWARD_DROP:
            pAclRule->act.forwardAct.act = ACL_IGR_FORWARD_DROP_ACT;
            break;
        case RTL9601B_IGR_FORWARD_TRAP:
            pAclRule->act.forwardAct.act = ACL_IGR_FORWARD_TRAP_ACT;
            break;
		case RTL9601B_IGR_FORWARD_NOP:
			pAclRule->act.forwardAct.act = ACL_IGR_FORWARD_NOP_ACT;
			break;
		default:
			return RT_ERR_CHIP_NOT_SUPPORTED;
			break;
    }

    /*Logging or Policing action control*/
	if ((ret = table_field_get(RTL9601B_ACL_ACTION_TABLEt, RTL9601B_ACL_ACTION_TABLE_POLICACTtf, &valAct, actionData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }
	
	if ((ret = table_field_get(RTL9601B_ACL_ACTION_TABLEt, RTL9601B_ACL_ACTION_TABLE_METER_IDXtf, &val, actionData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }
	
	switch(valAct)
	{
		case RTL9601B_DAL_ACL_IGR_LOG_POLICING_ACT:
			pAclRule->act.logAct.act = ACL_IGR_LOG_POLICING_ACT;
			pAclRule->act.logAct.meter = (uint8)val;
			break;
		case RTL9601B_DAL_ACL_IGR_LOG_MIB_ACT:
			pAclRule->act.logAct.act = ACL_IGR_LOG_MIB_ACT;
			pAclRule->act.logAct.mib = (uint8)val;
			break;
		default:
			return RT_ERR_CHIP_NOT_SUPPORTED;
			break;
	}

    /*svlan action control*/
    if ((ret = table_field_get(RTL9601B_ACL_ACTION_TABLEt, RTL9601B_ACL_ACTION_TABLE_RMK_DSCPtf, &val, actionData)) != RT_ERR_OK)
	{
			RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
			return ret;
	}

    pAclRule->act.svlanAct.act = ACL_IGR_SVLAN_DSCP_REMARK_ACT;
    pAclRule->act.svlanAct.dscp = (uint8)val;

    /*cvlan action control*/
    if ((ret = table_field_get(RTL9601B_ACL_ACTION_TABLEt, RTL9601B_ACL_ACTION_TABLE_RMK_1Ptf, &val, actionData)) != RT_ERR_OK)
	{
			RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
			return ret;
	}

    pAclRule->act.cvlanAct.act = ACL_IGR_CVLAN_1P_REMARK_ACT;
    pAclRule->act.cvlanAct.dot1p = (uint8)val;

    return RT_ERR_OK;
} /* end of dal_rtl9601b_acl_igrRuleAction_get */

/* Function Name:
 *      dal_rtl9601b_acl_igrRuleEntry_get
 * Description:
 *      Get an ACL entry from ASIC
 * Input:
 *      None.
 * Output:
 *      pAclRule     - The ACL configuration that this function will add comparison rule
 * Return:
 *      RT_ERR_OK              					- OK
 *      RT_ERR_FAILED          					- Failed
 *      RT_ERR_NULL_POINTER    					- Pointer pAclRule point to NULL.
 *      RT_ERR_INPUT 							- Invalid input parameters.
 * Note:
 *      use this API to get rule entry the field data will return in raw format
 *      raw data is return in pAclRule->field.readField
 */
int32
dal_rtl9601b_acl_igrRuleEntry_get(rtk_acl_ingress_entry_t *pAclRule)
{
    int32   ret;
    rtl9601b_raw_acl_ruleEntry_t rawRule;
    uint16 fieldIndex;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_ACL),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9601b_acl_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pAclRule), RT_ERR_NULL_POINTER);

    RT_PARAM_CHK(pAclRule->index >= HAL_MAX_NUM_OF_ACL_RULE_ENTRY(), RT_ERR_OUT_OF_RANGE);

    /*retrive rule*/
    rawRule.idx = (uint32)pAclRule->index;


    if((ret = _raw_rtl9601b_acl_rule_get(&rawRule) != RT_ERR_OK ))
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    pAclRule->valid = rawRule.valid;

    pAclRule->activePorts.bits[0]= rawRule.data_bits.active_portmsk;

    pAclRule->templateIdx = rawRule.data_bits.type;

    pAclRule->careTag.tags[ACL_CARE_TAG_PPPOE].value = rawRule.data_bits.tag_exist & 1;
    pAclRule->careTag.tags[ACL_CARE_TAG_PPPOE].mask = rawRule.care_bits.tag_exist & 1;
    pAclRule->careTag.tags[ACL_CARE_TAG_CTAG].value = (rawRule.data_bits.tag_exist >> 1) & 1;
    pAclRule->careTag.tags[ACL_CARE_TAG_CTAG].mask = (rawRule.care_bits.tag_exist >> 1) & 1;
    pAclRule->careTag.tags[ACL_CARE_TAG_STAG].value = (rawRule.data_bits.tag_exist >> 2) & 1;
    pAclRule->careTag.tags[ACL_CARE_TAG_STAG].mask = (rawRule.care_bits.tag_exist >> 2) & 1;
    pAclRule->careTag.tags[ACL_CARE_TAG_IPV4].value = (rawRule.data_bits.tag_exist >> 3) & 1;
    pAclRule->careTag.tags[ACL_CARE_TAG_IPV4].mask = (rawRule.care_bits.tag_exist >> 3) & 1;
    pAclRule->careTag.tags[ACL_CARE_TAG_IPV6].value = (rawRule.data_bits.tag_exist >> 4) & 1;
    pAclRule->careTag.tags[ACL_CARE_TAG_IPV6].mask = (rawRule.care_bits.tag_exist >> 4) & 1;
    pAclRule->careTag.tags[ACL_CARE_TAG_TCP].value = (rawRule.data_bits.tag_exist >> 5) & 1;
    pAclRule->careTag.tags[ACL_CARE_TAG_TCP].mask = (rawRule.care_bits.tag_exist >> 5) & 1;
    pAclRule->careTag.tags[ACL_CARE_TAG_UDP].value = (rawRule.data_bits.tag_exist >> 6) & 1;
    pAclRule->careTag.tags[ACL_CARE_TAG_UDP].mask = (rawRule.care_bits.tag_exist >> 6) & 1;

    for(fieldIndex = 0; fieldIndex < HAL_MAX_NUM_OF_ACL_RULE_FIELD(); fieldIndex ++)
    {
        pAclRule->readField.fieldRaw[fieldIndex].value = rawRule.data_bits.field[fieldIndex];
        pAclRule->readField.fieldRaw[fieldIndex].mask = rawRule.care_bits.field[fieldIndex];
    }

    /*retrive action control*/
    if ((ret = dal_rtl9601b_acl_igrRuleAction_get(pAclRule)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_acl_igrRuleEntry_get */


/* Function Name:
 *      dal_rtl9601b_acl_igrRuleField_add
 * Description:
 *      Add comparison rule to an ACL configuration
 * Input:
 *      pAclEntry     - The ACL configuration that this function will add comparison rule
 *      pAclField   - The comparison rule that will be added.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK              	- OK
 *      RT_ERR_FAILED          	- Failed
 *      RT_ERR_NULL_POINTER    	- Pointer pFilter_field or pFilter_cfg point to NULL.
 *      RT_ERR_INPUT 			- Invalid input parameters.
 * Note:
 *      This function add a comparison rule (*pAclField) to an ACL configuration (*pAclEntry).
 *      Pointer pFilter_cfg points to an ACL configuration structure, this structure keeps multiple ACL
 *      comparison rules by means of linked list. Pointer pAclField will be added to linked
 *      list keeped by structure that pAclEntry points to.
 *      caller should not free (*pAclField) before dal_rtl9601b_acl_igrRuleEntry_add is called
 */
int32
dal_rtl9601b_acl_igrRuleField_add(rtk_acl_ingress_entry_t *pAclRule, rtk_acl_field_t *pAclField)
{
	rtk_acl_field_t *tailPtr;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_ACL),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9601b_acl_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pAclRule), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pAclField), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pAclRule->templateIdx >= HAL_MAX_NUM_OF_ACL_TEMPLATE()), RT_ERR_OUT_OF_RANGE);

    if(NULL == pAclRule->pFieldHead)
    {
        pAclRule->pFieldHead = pAclField;
    }
    else
    {
        if ( pAclRule->pFieldHead->next == NULL)
        {
             pAclRule->pFieldHead->next = pAclField;
        }
        else
        {
            tailPtr = pAclRule->pFieldHead->next;
            while( tailPtr->next != NULL)
            {
                tailPtr = tailPtr->next;
            }
            tailPtr->next = pAclField;
        }
    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_acl_igrRuleField_add */


/* Function Name:
 *      dal_rtl9601b_acl_igrRuleEntry_add
 * Description:
 *      Add an ACL configuration to ASIC
 * Input:
 *      pAclRule   - ACL ingress filter rule configuration.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              					- OK
 *      RT_ERR_FAILED          					- Failed
 *      RT_ERR_NULL_POINTER    					- Pointer pAclrule point to NULL.
 *      RT_ERR_INPUT 							- Invalid input parameters.
 *      RT_ERR_ENTRY_INDEX 						- Invalid entryIdx .
 * Note:
 *      None
 */
int32
dal_rtl9601b_acl_igrRuleEntry_add(rtk_acl_ingress_entry_t *pAclRule)
{
    int32   ret;
    rtl9601b_raw_acl_ruleEntry_t rawRule;
    rtk_acl_template_t aclTemplate;
    rtk_acl_field_t  *aclField;


    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_ACL),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9601b_acl_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pAclRule), RT_ERR_NULL_POINTER);

    osal_memset(&rawRule,0x0,sizeof(rtl9601b_raw_acl_ruleEntry_t));

    rawRule.idx = (uint32)pAclRule->index;

    RT_PARAM_CHK((pAclRule->index >= HAL_MAX_NUM_OF_ACL_RULE_ENTRY()), RT_ERR_OUT_OF_RANGE);

    ///rawRule.valid = ENABLED;
    rawRule.valid = pAclRule->valid;

    /*check active ports for acl rule*/
    rawRule.data_bits.active_portmsk = pAclRule->activePorts.bits[0];
	rawRule.care_bits.active_portmsk = 0x7;
    /*check template index*/
    RT_PARAM_CHK((pAclRule->templateIdx >= HAL_MAX_NUM_OF_ACL_TEMPLATE()), RT_ERR_OUT_OF_RANGE);
    rawRule.data_bits.type =  pAclRule->templateIdx;
    rawRule.care_bits.type =  0x3;

    aclTemplate.index = pAclRule->templateIdx;
    if ((ret = dal_rtl9601b_acl_template_get(&aclTemplate)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }


    /*check care-tags for acl rule*/
    if(pAclRule->careTag.tags[ACL_CARE_TAG_PPPOE].value)
    {
        rawRule.data_bits.tag_exist |= 1;
    }

    if(pAclRule->careTag.tags[ACL_CARE_TAG_PPPOE].mask)
    {
        rawRule.care_bits.tag_exist |= 1;
    }

    if(pAclRule->careTag.tags[ACL_CARE_TAG_CTAG].value)
    {
        rawRule.data_bits.tag_exist |= (1 << 1);
    }

    if(pAclRule->careTag.tags[ACL_CARE_TAG_CTAG].mask)
    {
        rawRule.care_bits.tag_exist |= (1 << 1);
    }

    if(pAclRule->careTag.tags[ACL_CARE_TAG_STAG].value)
    {
        rawRule.data_bits.tag_exist |= (1 << 2);
    }

    if(pAclRule->careTag.tags[ACL_CARE_TAG_STAG].mask)
    {
        rawRule.care_bits.tag_exist |= (1 << 2);
    }

    if(pAclRule->careTag.tags[ACL_CARE_TAG_IPV4].value)
    {
        rawRule.data_bits.tag_exist |= (1 << 3);
    }

    if(pAclRule->careTag.tags[ACL_CARE_TAG_IPV4].mask)
    {
        rawRule.care_bits.tag_exist |= (1 << 3);
    }

    if(pAclRule->careTag.tags[ACL_CARE_TAG_IPV6].value)
    {
        rawRule.data_bits.tag_exist |= (1 << 4);
    }

    if(pAclRule->careTag.tags[ACL_CARE_TAG_IPV6].mask)
    {
        rawRule.care_bits.tag_exist |= (1 << 4);
    }

    if(pAclRule->careTag.tags[ACL_CARE_TAG_TCP].value)
    {
        rawRule.data_bits.tag_exist |= (1 << 5);
    }

    if(pAclRule->careTag.tags[ACL_CARE_TAG_TCP].mask)
    {
        rawRule.care_bits.tag_exist |= (1 << 5);
    }

    if(pAclRule->careTag.tags[ACL_CARE_TAG_UDP].value)
    {
        rawRule.data_bits.tag_exist |= (1 << 6);
    }

    if(pAclRule->careTag.tags[ACL_CARE_TAG_UDP].mask)
    {
        rawRule.care_bits.tag_exist |= (1 << 6);
    }

    /*check field for ACL rule*/
    if(pAclRule->pFieldHead != NULL)
    {
        aclField = (rtk_acl_field_t*)pAclRule->pFieldHead;
        if ((ret = _dal_rtl9601b_acl_ruleField_add(&rawRule, &aclTemplate, aclField)) != RT_ERR_OK )
        {
            RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
            return ret;
        }

        aclField = aclField->next;
        while(aclField != NULL)
        {
            if ((ret = _dal_rtl9601b_acl_ruleField_add(&rawRule, &aclTemplate, aclField)) != RT_ERR_OK )
            {
                RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
                return ret;
            }
            aclField = aclField->next;
        }
    }

    if ((ret = dal_rtl9601b_acl_igrRuleAction_set(pAclRule)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    if((ret = _raw_rtl9601b_acl_rule_set(&rawRule) != RT_ERR_OK ))
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }


    return RT_ERR_OK;
} /* end of dal_rtl9601b_acl_igrRuleEntry_add */

/* Function Name:
 *      dal_rtl9601b_acl_igrRuleEntry_del
 * Description:
 *      Delete an ACL configuration from ASIC
 * Input:
 *      pAclrule   - ACL ingress filter rule configuration.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              					- OK
 *      RT_ERR_FAILED          					- Failed
 *      RT_ERR_ENTRY_INDEX 						- Invalid entryIdx .
 * Note:
 *      None
 */
int32
dal_rtl9601b_acl_igrRuleEntry_del(uint32 index)
{
    int32   ret;
    uint32  val;
    uint32  actionData[RTL9601B_ACL_ACTION_DATA_LEN];
    rtl9601b_raw_acl_ruleEntry_t rawRule;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_ACL),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9601b_acl_init);

    /* parameter check */
    RT_PARAM_CHK((index >= HAL_MAX_NUM_OF_ACL_ACTION()), RT_ERR_OUT_OF_RANGE);


    osal_memset(&rawRule,0x0,sizeof(rtl9601b_raw_acl_ruleEntry_t));
    rawRule.idx = index;

    rawRule.valid = DISABLED;

    if((ret = _raw_rtl9601b_acl_rule_set(&rawRule)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
		return ret;
    }

    /*clear action*/
	osal_memset(actionData, 0, sizeof(actionData));
    if ((ret = table_write(RTL9601B_ACL_ACTION_TABLEt, index, actionData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    /*clear action contril bits*/
    /* valid and not = DISABLED, others = ENABLED*/
    val = 0x3F;

    if((ret = reg_array_write(RTL9601B_ACL_ACTIONr, REG_ARRAY_INDEX_NONE, index, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
		return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_acl_igrRuleEntry_del */

/* Function Name:
 *      dal_rtl9601b_acl_igrRuleEntry_delAll
 * Description:
 *      Delete all ACL configuration from ASIC
 * Input:
 *      pAclrule   - ACL ingress filter rule configuration.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              					- OK
 *      RT_ERR_FAILED          					- Failed
 * Note:
 *      None
 */
int32
dal_rtl9601b_acl_igrRuleEntry_delAll(void)
{
    int32   ret;
    uint32  index;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_ACL),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9601b_acl_init);


    /*clear rule*/
    index = HAL_MAX_NUM_OF_ACL_RULE_ENTRY() - 1;

    if((ret = table_clear(RTL9601B_ACL_DATAt,0,index)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
		return ret;
    }

    if((ret = table_clear(RTL9601B_ACL_MASKt,0,index)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
		return ret;
    }

    /*clear action*/
    index = HAL_MAX_NUM_OF_ACL_ACTION() - 1;

    if((ret = table_clear(RTL9601B_ACL_ACTION_TABLEt,0,index)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
		return ret;
    }

    /*clear action contril bits*/
    /* valid and not = DISABLED, others = ENABLED*/
    val = 0x3F;

    for(index = 0; index < HAL_MAX_NUM_OF_ACL_ACTION(); index ++)
    {
        if((ret = reg_array_write(RTL9601B_ACL_ACTIONr, REG_ARRAY_INDEX_NONE, index, &val)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
    		return ret;
        }
    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_acl_igrRuleEntry_delAll */


/* Function Name:
 *      dal_rtl9601b_acl_igrUnmatchAction_set
 * Description:
 *      Apply action to packets when no ACL configuration match
 * Input:
 *      port    - Port id.
 *      action - Action.
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - Invalid port id.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This function gets action of packets when no ACL configruation matches.
 */
int32
dal_rtl9601b_acl_igrUnmatchAction_set(rtk_port_t port, rtk_filter_unmatch_action_type_t action)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_ACL),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9601b_acl_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);

    switch(action)
    {
        case FILTER_UNMATCH_DROP:
        case FILTER_UNMATCH_PERMIT:
            val = (uint32)action;
        	if ((ret = reg_array_field_write(RTL9601B_ACL_PERMITr, port, REG_ARRAY_INDEX_NONE, RTL9601B_PERMITf, &val)) != RT_ERR_OK)
        	{
                RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        		return ret;
        	}
            break;

        default:

            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_acl_igrUnmatchAction_set */

/* Function Name:
 *      dal_rtl9601b_acl_igrUnmatchAction_get
 * Description:
 *      Get action to packets when no ACL configuration match
 * Input:
 *      port    - Port id.
 * Output:
 *      pAction - Action.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port id.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This function gets action of packets when no ACL configruation matches.
 */
int32
dal_rtl9601b_acl_igrUnmatchAction_get(rtk_port_t port, rtk_filter_unmatch_action_type_t *pAction)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_ACL),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9601b_acl_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

	if ((ret = reg_array_field_read(RTL9601B_ACL_PERMITr, port, REG_ARRAY_INDEX_NONE, RTL9601B_PERMITf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
		return ret;
	}
    *pAction = (rtk_filter_unmatch_action_type_t)val;

    return RT_ERR_OK;
} /* end of dal_rtl9601b_acl_igrUnmatchAction_get */


/* Function Name:
 *      dal_rtl9601b_acl_igrState_set
 * Description:
 *      Set state of ingress ACL.
 * Input:
 *      port    - Port id.
 *      state   - Ingress ACL state.
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - Invalid port id.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This function set per-port ACL filtering state.
 */
int32
dal_rtl9601b_acl_igrState_set(rtk_port_t port, rtk_enable_t state)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_ACL),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9601b_acl_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTK_ENABLE_END <= state), RT_ERR_INPUT);

    val = (uint32)state;
    if ((ret = reg_array_field_write(RTL9601B_ACL_ENr, port, REG_ARRAY_INDEX_NONE,RTL9601B_ENf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_acl_igrState_set */


/* Function Name:
 *      dal_rtl9601b_acl_igrState_get
 * Description:
 *      Get state of ingress ACL.
 * Input:
 *      port    - Port id.
 * Output:
 *      pState  - Ingress ACL state.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - Invalid port id.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This function gets action of packets when no ACL configruation matches.
 */
int32
dal_rtl9601b_acl_igrState_get(rtk_port_t port, rtk_enable_t *pState)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_ACL),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9601b_acl_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(RTL9601B_ACL_ENr, port, REG_ARRAY_INDEX_NONE,RTL9601B_ENf, &val))!=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }
    *pState = (rtk_enable_t)val;

    return RT_ERR_OK;
} /* end of dal_rtl9601b_acl_igrState_get */


/* Function Name:
 *      dal_rtl9601b_acl_ipRange_set
 * Description:
 *      Set IP Range check
 * Input:
 *      pRangeEntry - IP Range entry
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_OUT_OF_RANGE    - The parameter is out of range
 *      RT_ERR_INPUT           - Input error
 * Note:
 *      upper Ip must be larger or equal than lowerIp.
 */
int32
dal_rtl9601b_acl_ipRange_set(rtk_acl_rangeCheck_ip_t *pRangeEntry)
{
    int32   ret;
    uint32  index;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_ACL),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9601b_acl_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pRangeEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(pRangeEntry->index >= HAL_MAX_NUM_OF_RANGE_CHECK_IP(), RT_ERR_OUT_OF_RANGE);

    index = (uint32)pRangeEntry->index;

    switch(pRangeEntry->type)
    {
        case IPRANGE_UNUSED:

            val = RTL9601B_DAL_ACL_IPRANGE_UNUSED;
            break;
        case IPRANGE_IPV4_SIP:

            val = RTL9601B_DAL_ACL_IPRANGE_IPV4_SIP;
            break;
        case IPRANGE_IPV4_DIP:

            val = RTL9601B_DAL_ACL_IPRANGE_IPV4_DIP;
            break;
        case IPRANGE_IPV6_SIP:

            val = RTL9601B_DAL_ACL_IPRANGE_IPV6_SIP;
            break;
        case IPRANGE_IPV6_DIP:

            val = RTL9601B_DAL_ACL_IPRANGE_IPV6_DIP;
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

	if ((ret = reg_array_field_write(RTL9601B_RNG_CHK_IP_RNGr, REG_ARRAY_INDEX_NONE, index, RTL9601B_TYPEf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    val = (uint32)pRangeEntry->upperIp;
	if ((ret = reg_array_field_write(RTL9601B_RNG_CHK_IP_RNGr, REG_ARRAY_INDEX_NONE, index, RTL9601B_IP_UPPERf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    val = (uint32)pRangeEntry->lowerIp;
	if((ret = reg_array_field_write(RTL9601B_RNG_CHK_IP_RNGr, REG_ARRAY_INDEX_NONE, index, RTL9601B_IP_LOWERf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_acl_ipRange_set */


/* Function Name:
 *      dal_rtl9601b_acl_ipRange_get
 * Description:
 *      Set IP Range check
 * Input:
 *      None.
 * Output:
 *      pRangeEntry - IP Range entry
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_OUT_OF_RANGE    - The parameter is out of range
 * Note:
 *      None.
 */
int32
dal_rtl9601b_acl_ipRange_get(rtk_acl_rangeCheck_ip_t *pRangeEntry)
{
    int32   ret;
    uint32  index;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_ACL),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9601b_acl_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pRangeEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(pRangeEntry->index >= HAL_MAX_NUM_OF_RANGE_CHECK_IP(), RT_ERR_OUT_OF_RANGE);

    index = (uint32)pRangeEntry->index;

	if ((ret = reg_array_field_read(RTL9601B_RNG_CHK_IP_RNGr, REG_ARRAY_INDEX_NONE, index, RTL9601B_TYPEf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }
    switch(val)
    {
        case RTL9601B_DAL_ACL_IPRANGE_UNUSED:

            pRangeEntry->type = IPRANGE_UNUSED;
            break;
        case RTL9601B_DAL_ACL_IPRANGE_IPV4_SIP:

            pRangeEntry->type = IPRANGE_IPV4_SIP;
            break;
        case RTL9601B_DAL_ACL_IPRANGE_IPV4_DIP:

            pRangeEntry->type = IPRANGE_IPV4_DIP;
            break;
        case RTL9601B_DAL_ACL_IPRANGE_IPV6_SIP:

            pRangeEntry->type = IPRANGE_IPV6_SIP;
            break;
        case RTL9601B_DAL_ACL_IPRANGE_IPV6_DIP:

            pRangeEntry->type = IPRANGE_IPV6_DIP;
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

	if ((ret = reg_array_field_read(RTL9601B_RNG_CHK_IP_RNGr, REG_ARRAY_INDEX_NONE, index, RTL9601B_IP_UPPERf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }
    pRangeEntry->upperIp = (ipaddr_t)val;

	if((ret = reg_array_field_read(RTL9601B_RNG_CHK_IP_RNGr, REG_ARRAY_INDEX_NONE, index, RTL9601B_IP_LOWERf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }
    pRangeEntry->lowerIp = (ipaddr_t)val;

    return RT_ERR_OK;
} /* end of dal_rtl9601b_acl_ipRange_get */

/* Function Name:
 *      dal_rtl9601b_acl_vidRange_set
 * Description:
 *      Set VID Range check
 * Input:
 *      pRangeEntry - VLAN id Range entry
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_OUT_OF_RANGE    - The parameter is out of range
 *      RT_ERR_INPUT           - Input error
 * Note:
 *      upper Vid must be larger or equal than lowerVid.
 */
int32
dal_rtl9601b_acl_vidRange_set(rtk_acl_rangeCheck_vid_t *pRangeEntry)
{
    int32   ret;
    uint32  index;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_ACL),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9601b_acl_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pRangeEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(pRangeEntry->index >= HAL_MAX_NUM_OF_RANGE_CHECK_VID(), RT_ERR_OUT_OF_RANGE);

    index = (uint32)pRangeEntry->index;

    switch(pRangeEntry->type)
    {
        case VIDRANGE_UNUSED:

            val = RTL9601B_DAL_ACL_VIDRANGE_UNUSED;
            break;
        case VIDRANGE_CVID:

            val = RTL9601B_DAL_ACL_VIDRANGE_CVID;
            break;
        case VIDRANGE_SVID:

            val = RTL9601B_DAL_ACL_VIDRANGE_SVID;
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if ((ret = reg_array_field_write(RTL9601B_RNG_CHK_VID_RNGr, REG_ARRAY_INDEX_NONE, index, RTL9601B_TYPEf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    val = (uint32)pRangeEntry->upperVid;
	if ((ret = reg_array_field_write(RTL9601B_RNG_CHK_VID_RNGr, REG_ARRAY_INDEX_NONE, index, RTL9601B_VID_UPPERf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    val = (uint32)pRangeEntry->lowerVid;
	if ((ret = reg_array_field_write(RTL9601B_RNG_CHK_VID_RNGr, REG_ARRAY_INDEX_NONE, index, RTL9601B_VID_LOWERf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_acl_vidRange_set */

/* Function Name:
 *      dal_rtl9601b_acl_vidRange_get
 * Description:
 *      Get VID Range check
 * Input:
 *      None.
 * Output:
 *      pRangeEntry - VLAN id Range entry
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_OUT_OF_RANGE    - The parameter is out of range
 * Note:
 *      None.
 */
int32
dal_rtl9601b_acl_vidRange_get(rtk_acl_rangeCheck_vid_t *pRangeEntry)
{
    int32   ret;
    uint32  index;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_ACL),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9601b_acl_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pRangeEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(pRangeEntry->index >= HAL_MAX_NUM_OF_RANGE_CHECK_VID(), RT_ERR_OUT_OF_RANGE);

    index = (uint32)pRangeEntry->index;

    if ((ret = reg_array_field_read(RTL9601B_RNG_CHK_VID_RNGr, REG_ARRAY_INDEX_NONE, index, RTL9601B_TYPEf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    switch(val)
    {
        case RTL9601B_DAL_ACL_VIDRANGE_UNUSED:

            pRangeEntry->type = VIDRANGE_UNUSED;
            break;
        case RTL9601B_DAL_ACL_VIDRANGE_CVID:

            pRangeEntry->type = VIDRANGE_CVID;
            break;
        case RTL9601B_DAL_ACL_VIDRANGE_SVID:

            pRangeEntry->type = VIDRANGE_SVID;
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

	if ((ret = reg_array_field_read(RTL9601B_RNG_CHK_VID_RNGr, REG_ARRAY_INDEX_NONE, index, RTL9601B_VID_UPPERf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }
    pRangeEntry->upperVid = (uint16)val;

	if ((ret = reg_array_field_read(RTL9601B_RNG_CHK_VID_RNGr, REG_ARRAY_INDEX_NONE, index, RTL9601B_VID_LOWERf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }
    pRangeEntry->lowerVid = (uint16)val;


    return RT_ERR_OK;
} /* end of dal_rtl9601b_acl_vidRange_get */


/* Function Name:
 *      dal_rtl9601b_acl_portRange_set
 * Description:
 *      Set Port Range check
 * Input:
 *      pRangeEntry - L4 Port Range entry
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_OUT_OF_RANGE    - The parameter is out of range
 *      RT_ERR_INPUT           - Input error
 * Note:
 *      upper Port must be larger or equal than lowerPort.
 */
int32
dal_rtl9601b_acl_portRange_set(rtk_acl_rangeCheck_l4Port_t *pRangeEntry)
{
    int32   ret;
    uint32  index;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_ACL),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9601b_acl_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pRangeEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(pRangeEntry->index >= HAL_MAX_NUM_OF_RANGE_CHECK_L4PORT(), RT_ERR_OUT_OF_RANGE);

    index = (uint32)pRangeEntry->index;

    switch(pRangeEntry->type)
    {
        case PORTRANGE_UNUSED:

            val = RTL9601B_DAL_ACL_PORTRANGE_UNUSED;
            break;
        case PORTRANGE_SPORT:

            val = RTL9601B_DAL_ACL_PORTRANGE_SPORT;
            break;
        case PORTRANGE_DPORT:

            val = RTL9601B_DAL_ACL_PORTRANGE_DPORT;
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

	if ((ret = reg_array_field_write(RTL9601B_RNG_CHK_L4PORT_RNGr, REG_ARRAY_INDEX_NONE, index, RTL9601B_TYPEf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    val = (uint32)pRangeEntry->upper_bound;
	if ((ret = reg_array_field_write(RTL9601B_RNG_CHK_L4PORT_RNGr, REG_ARRAY_INDEX_NONE, index, RTL9601B_L4PORT_UPPERf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    val = (uint32)pRangeEntry->lower_bound;
	if ((ret = reg_array_field_write(RTL9601B_RNG_CHK_L4PORT_RNGr, REG_ARRAY_INDEX_NONE, index, RTL9601B_L4PORT_LOWERf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_acl_portRange_set */


/* Function Name:
 *      dal_rtl9601b_acl_portRange_get
 * Description:
 *      Set Port Range check
 * Input:
 *      None
 * Output:
 *      pRangeEntry - L4 Port Range entry
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_OUT_OF_RANGE    - The parameter is out of range
 *      RT_ERR_INPUT           - Input error
 * Note:
 *      None.
 */
int32
dal_rtl9601b_acl_portRange_get(rtk_acl_rangeCheck_l4Port_t *pRangeEntry)
{
    int32   ret;
    uint32  index;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_ACL),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9601b_acl_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pRangeEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(pRangeEntry->index >= HAL_MAX_NUM_OF_RANGE_CHECK_L4PORT(), RT_ERR_OUT_OF_RANGE);

    index = (uint32)pRangeEntry->index;

	if ((ret = reg_array_field_read(RTL9601B_RNG_CHK_L4PORT_RNGr, REG_ARRAY_INDEX_NONE, index, RTL9601B_TYPEf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    switch(val)
    {
        case RTL9601B_DAL_ACL_PORTRANGE_UNUSED:

            pRangeEntry->type = PORTRANGE_UNUSED;
            break;
        case RTL9601B_DAL_ACL_PORTRANGE_SPORT:

            pRangeEntry->type = PORTRANGE_SPORT;
            break;
        case RTL9601B_DAL_ACL_PORTRANGE_DPORT:

            pRangeEntry->type = PORTRANGE_DPORT;
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

	if ((ret = reg_array_field_read(RTL9601B_RNG_CHK_L4PORT_RNGr, REG_ARRAY_INDEX_NONE, index, RTL9601B_L4PORT_UPPERf,  &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }
    pRangeEntry->upper_bound = (uint16)val;

	if((ret = reg_array_field_read(RTL9601B_RNG_CHK_L4PORT_RNGr, REG_ARRAY_INDEX_NONE, index, RTL9601B_L4PORT_LOWERf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }
    pRangeEntry->lower_bound = (uint16)val;


    return RT_ERR_OK;
} /* end of dal_rtl9601b_acl_portRange_get */


/* Function Name:
 *      dal_rtl9601b_acl_packetLengthRange_set
 * Description:
 *      Set packet length Range check
 * Input:
 *      pRangeEntry - packet length range entry
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_OUT_OF_RANGE    - The parameter is out of range
 *      RT_ERR_INPUT           - Input error
 * Note:
 *      upper length must be larger or equal than lower length.
 */
int32
dal_rtl9601b_acl_packetLengthRange_set(rtk_acl_rangeCheck_pktLength_t *pRangeEntry)
{
    int32   ret;
    uint32  index;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_ACL),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9601b_acl_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pRangeEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pRangeEntry->index >= HAL_MAX_NUM_OF_RANGE_CHECK_PKTLEN()), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((pRangeEntry->type >= PKTLENRANGE_END), RT_ERR_OUT_OF_RANGE);

    index = (uint32)pRangeEntry->index;

    val = (uint32)pRangeEntry->type;
	if ((ret = reg_array_field_write(RTL9601B_RNG_CHK_PKTLEN_RNGr, REG_ARRAY_INDEX_NONE, index, RTL9601B_TYPEf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    val = (uint32)pRangeEntry->upper_bound;
	if ((ret = reg_array_field_write(RTL9601B_RNG_CHK_PKTLEN_RNGr, REG_ARRAY_INDEX_NONE, index, RTL9601B_PKTLEN_UPPERf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    val = (uint32)pRangeEntry->lower_bound;
	if((ret = reg_array_field_write(RTL9601B_RNG_CHK_PKTLEN_RNGr, REG_ARRAY_INDEX_NONE, index, RTL9601B_PKTLEN_LOWERf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_acl_packetLengthRange_set */


/* Function Name:
 *      dal_rtl9601b_acl_packetLengthRange_get
 * Description:
 *      Set packet length Range check
 * Input:
 *      None
 * Output:
 *      pRangeEntry - packet length range entry
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_OUT_OF_RANGE    - The parameter is out of range
 *      RT_ERR_INPUT           - Input error
 * Note:
 *      None.
 */
int32
dal_rtl9601b_acl_packetLengthRange_get(rtk_acl_rangeCheck_pktLength_t *pRangeEntry)
{
    int32   ret;
    uint32  index;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_ACL),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9601b_acl_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pRangeEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pRangeEntry->index >= HAL_MAX_NUM_OF_RANGE_CHECK_PKTLEN()), RT_ERR_OUT_OF_RANGE);

    index = (uint32)pRangeEntry->index;

	if ((ret = reg_array_field_read(RTL9601B_RNG_CHK_PKTLEN_RNGr, REG_ARRAY_INDEX_NONE, index, RTL9601B_TYPEf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }
    pRangeEntry->type = (rtk_acl_pktlenrange_type_t)val;

	if ((ret = reg_array_field_read(RTL9601B_RNG_CHK_PKTLEN_RNGr, REG_ARRAY_INDEX_NONE, index, RTL9601B_PKTLEN_UPPERf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }
    pRangeEntry->upper_bound = (uint16)val;

	if((ret = reg_array_field_read(RTL9601B_RNG_CHK_PKTLEN_RNGr, REG_ARRAY_INDEX_NONE, index, RTL9601B_PKTLEN_LOWERf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }
    pRangeEntry->lower_bound = (uint16)val;

    return RT_ERR_OK;
} /* end of dal_rtl9601b_acl_packetLengthRange_get */


/* Function Name:
 *      dal_rtl9601b_acl_igrPermitState_set
 * Description:
 *      Set permit state of ingress ACL.
 * Input:
 *      port    - Port id.
 *      state  - Ingress ACL state.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - Invalid port id.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This function set action of packets when no ACL configruation matches.
 */
int32
dal_rtl9601b_acl_igrPermitState_set(rtk_port_t port, rtk_enable_t state)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_ACL),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9601b_acl_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTK_ENABLE_END <= state), RT_ERR_INPUT);

    val = (uint32)state;
	if ((ret = reg_array_field_write(RTL9601B_ACL_PERMITr, port, REG_ARRAY_INDEX_NONE, RTL9601B_PERMITf, &val)) != RT_ERR_OK)
	{
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
		return ret;
	}

    return RT_ERR_OK;
} /* end of dal_rtl9601b_acl_igrPermitState_set */


/* Function Name:
 *      dal_rtl9601b_acl_igrPermitState_get
 * Description:
 *      Get state of ingress ACL.
 * Input:
 *      port    - Port id.
 * Output:
 *      pState  - Ingress ACL state.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - Invalid port id.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This function gets action of packets when no ACL configruation matches.
 */
int32
dal_rtl9601b_acl_igrPermitState_get(rtk_port_t port, rtk_enable_t *pState)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_ACL),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9601b_acl_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

	if ((ret = reg_array_field_read(RTL9601B_ACL_PERMITr, port, REG_ARRAY_INDEX_NONE, RTL9601B_PERMITf, &val)) != RT_ERR_OK)
	{
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
		return ret;
	}

    *pState = (rtk_enable_t)val;

    return RT_ERR_OK;
} /* end of dal_rtl9601b_acl_igrPermitState_get */

/* Function Name:
 *      dal_rtl9601b_acl_dbgHitReason_get
 * Description:
 *      Get ACL hit reason information
 * Input:
 *	  	None
 * Output:
 *      pDbgReason: point of relative debug reason and index
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 dal_rtl9601b_acl_dbgHitReason_get(rtk_acl_debug_reason_t *pDbgReason)
{
    int32   ret;
	rtk_acl_igr_act_type_t 	type;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_ACL),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9601b_acl_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pDbgReason), RT_ERR_NULL_POINTER);

	for(type = ACL_IGR_CVLAN_ACT; type < ACL_IGR_ACT_END; type++)
	{
		pDbgReason->hitAct[type] = DISABLED;
		pDbgReason->index[type] = 0;
	}

	if ((ret = reg_array_field_read(RTL9601B_STAT_ACL_REASONr, REG_ARRAY_INDEX_NONE, 0, RTL9601B_ACL_HIT_INFOf, &val)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
		return ret;
	}

	if(val & 0x40)
	{
		pDbgReason->hitAct[ACL_IGR_CVLAN_ACT] = ENABLED;
		pDbgReason->index[ACL_IGR_CVLAN_ACT] = val &0x3F;	
	}

	if ((ret = reg_array_field_read(RTL9601B_STAT_ACL_REASONr, REG_ARRAY_INDEX_NONE, 1, RTL9601B_ACL_HIT_INFOf, &val)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
		return ret;
	}

	if(val & 0x40)
	{
		pDbgReason->hitAct[ACL_IGR_SVLAN_ACT] = ENABLED;
		pDbgReason->index[ACL_IGR_SVLAN_ACT] = val &0x3F;	
	}

	if ((ret = reg_array_field_read(RTL9601B_STAT_ACL_REASONr, REG_ARRAY_INDEX_NONE, 2, RTL9601B_ACL_HIT_INFOf, &val)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
		return ret;
	}

	if(val & 0x40)
	{
		pDbgReason->hitAct[ACL_IGR_PRI_ACT] = ENABLED;
		pDbgReason->index[ACL_IGR_PRI_ACT] = val &0x3F;	
	}

	if ((ret = reg_array_field_read(RTL9601B_STAT_ACL_REASONr, REG_ARRAY_INDEX_NONE, 3, RTL9601B_ACL_HIT_INFOf, &val)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
		return ret;
	}

	if(val & 0x40)
	{
		pDbgReason->hitAct[ACL_IGR_LOG_ACT] = ENABLED;
		pDbgReason->index[ACL_IGR_LOG_ACT] = val &0x3F;	
	}

	if ((ret = reg_array_field_read(RTL9601B_STAT_ACL_REASONr, REG_ARRAY_INDEX_NONE, 5, RTL9601B_ACL_HIT_INFOf, &val)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
		return ret;
	}

	if(val & 0x40)
	{
		pDbgReason->hitAct[ACL_IGR_FORWARD_ACT] = ENABLED;
		pDbgReason->index[ACL_IGR_FORWARD_ACT] = val &0x3F;	
	}
	
	if ((ret = reg_array_field_read(RTL9601B_STAT_ACL_REASONr, REG_ARRAY_INDEX_NONE, 4, RTL9601B_ACL_HIT_INFOf, &val)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
		return ret;
	}

	if(val & 0x40)
	{
		pDbgReason->hitAct[ACL_IGR_INTR_ACT] = ENABLED;
		pDbgReason->index[ACL_IGR_INTR_ACT] = val &0x3F;	
	}

    return RT_ERR_OK;
}   /* end of dal_rtl9601b_acl_dbgHitReason_get */

