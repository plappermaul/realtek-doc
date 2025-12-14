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
#include <dal/apollomp/dal_apollomp.h>
#include <dal/apollomp/dal_apollomp_acl.h>
#include <hal/mac/reg.h>
#include <hal/mac/drv.h>
#include <dal/apollomp/raw/apollomp_raw_acl.h>
#include <dal/apollomp/dal_apollomp_svlan.h>
#include <dal/apollomp/dal_apollomp_vlan.h>
#include <rtk/irq.h>
#include <rtk/intr.h>
#include <dal/apollomp/dal_apollomp_intr.h>

/*
 * Symbol Definition
 */
static uint32 apollomp_acl_init = {INIT_NOT_COMPLETED};


static const apollomp_dal_acl_fieldTypes_t apollomp_dal_template_init[4][8] =
    {{APOLLOMP_DAL_ACL_DMAC0,           APOLLOMP_DAL_ACL_DMAC1,         APOLLOMP_DAL_ACL_DMAC2,         APOLLOMP_DAL_ACL_STAG,          APOLLOMP_DAL_ACL_SMAC0,         APOLLOMP_DAL_ACL_SMAC1,         APOLLOMP_DAL_ACL_SMAC2,         APOLLOMP_DAL_ACL_ETHERTYPE},
     {APOLLOMP_DAL_ACL_CTAG,            APOLLOMP_DAL_ACL_IP4SIP0,       APOLLOMP_DAL_ACL_IP4SIP1,       APOLLOMP_DAL_ACL_VIDRANGE,      APOLLOMP_DAL_ACL_IPRANGE,       APOLLOMP_DAL_ACL_PORTRANGE,     APOLLOMP_DAL_ACL_IP4DIP0,       APOLLOMP_DAL_ACL_IP4DIP1},
     {APOLLOMP_DAL_ACL_FIELD_SELECT00,  APOLLOMP_DAL_ACL_FIELD_SELECT01,APOLLOMP_DAL_ACL_FIELD_SELECT02,APOLLOMP_DAL_ACL_FIELD_SELECT03,APOLLOMP_DAL_ACL_FIELD_SELECT04,APOLLOMP_DAL_ACL_FIELD_SELECT05,APOLLOMP_DAL_ACL_FIELD_SELECT06,APOLLOMP_DAL_ACL_FIELD_SELECT07},
     {APOLLOMP_DAL_ACL_FIELD_SELECT08,  APOLLOMP_DAL_ACL_FIELD_SELECT09,APOLLOMP_DAL_ACL_FIELD_SELECT10,APOLLOMP_DAL_ACL_FIELD_SELECT11,APOLLOMP_DAL_ACL_FIELD_SELECT12,APOLLOMP_DAL_ACL_FIELD_SELECT13,APOLLOMP_DAL_ACL_FIELD_SELECT14,APOLLOMP_DAL_ACL_FIELD_SELECT15}};

static uint32 _actionIntCnt=0;

/*
 * Macro Declaration
 */
#define APOLLOMP_ACL_ACTION_DATA_LEN      2

static uint32 dal_apollomp_acl_isr_init(void);
static uint32 dal_apollomp_acl_isr_entry(void);

/*
 * Function Declaration
 */
static int32 _dal_apollomp_acl_ruleField_check(rtk_acl_template_t *pTemplate, rtk_acl_field_type_t fieldType, uint32 start, uint32 end, uint32 *pIndex)
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

static int32 _dal_apollomp_acl_ruleField_add(apollomp_raw_acl_ruleEntry_t *pRawRule, rtk_acl_template_t *pTemplate, rtk_acl_field_t *pField)
{
    int32   ret;
    uint32  val;
    uint32  fieldIndexStart;
    uint32  fieldIndexEnd;
    uint32  fieldIndexOffset;
    uint32  hitIndex;



    if(pRawRule->mode == APOLLOMP_ACL_MODE_64ENTRIES)
    {
        fieldIndexStart = 0;
        fieldIndexEnd = 7;

        fieldIndexOffset = 0;
    }
    else if(pRawRule->mode == APOLLOMP_ACL_MODE_128ENTRIES)
    {
        if(pRawRule->idx >= 64)
        {
            fieldIndexStart = 0;
            fieldIndexEnd = 2;

            fieldIndexOffset = 0;
        }
        else
        {
            fieldIndexStart = 4;
            fieldIndexEnd = 7;

            fieldIndexOffset = 4;
        }
    } 
	else
	{
		return RT_ERR_CHIP_NOT_SUPPORTED;
	}

    switch(pField->fieldType)
    {
        case ACL_FIELD_CTAG:
        case ACL_FIELD_STAG:
            if ((ret = _dal_apollomp_acl_ruleField_check(pTemplate, pField->fieldType,fieldIndexStart, fieldIndexEnd, &hitIndex)) != RT_ERR_OK)
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
        case ACL_FIELD_EXT_PORTMASK:
        case ACL_FIELD_IPV6_NEXT_HEADER:
		case ACL_FIELD_IPV4_PROTOCOL:
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
            if ((ret = _dal_apollomp_acl_ruleField_check(pTemplate, pField->fieldType,fieldIndexStart, fieldIndexEnd, &hitIndex)) != RT_ERR_OK)
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
            if ((ret = _dal_apollomp_acl_ruleField_check(pTemplate, ACL_FIELD_DMAC0,fieldIndexStart, fieldIndexEnd, &hitIndex)) != RT_ERR_OK)
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

            if ((ret = _dal_apollomp_acl_ruleField_check(pTemplate, ACL_FIELD_DMAC1,fieldIndexStart, fieldIndexEnd, &hitIndex)) != RT_ERR_OK)
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

            if ((ret = _dal_apollomp_acl_ruleField_check(pTemplate, ACL_FIELD_DMAC2,fieldIndexStart, fieldIndexEnd, &hitIndex)) != RT_ERR_OK)
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
            if ((ret = _dal_apollomp_acl_ruleField_check(pTemplate, ACL_FIELD_DMAC0,fieldIndexStart, fieldIndexEnd, &hitIndex)) != RT_ERR_OK)
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
            if ((ret = _dal_apollomp_acl_ruleField_check(pTemplate, ACL_FIELD_DMAC1,fieldIndexStart, fieldIndexEnd, &hitIndex)) != RT_ERR_OK)
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
            if ((ret = _dal_apollomp_acl_ruleField_check(pTemplate, ACL_FIELD_DMAC2,fieldIndexStart, fieldIndexEnd, &hitIndex)) != RT_ERR_OK)
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
            if ((ret = _dal_apollomp_acl_ruleField_check(pTemplate, ACL_FIELD_SMAC0,fieldIndexStart, fieldIndexEnd, &hitIndex)) != RT_ERR_OK)
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

            if ((ret = _dal_apollomp_acl_ruleField_check(pTemplate, ACL_FIELD_SMAC1,fieldIndexStart, fieldIndexEnd, &hitIndex)) != RT_ERR_OK)
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

            if ((ret = _dal_apollomp_acl_ruleField_check(pTemplate, ACL_FIELD_SMAC2,fieldIndexStart, fieldIndexEnd, &hitIndex)) != RT_ERR_OK)
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
            if ((ret = _dal_apollomp_acl_ruleField_check(pTemplate, ACL_FIELD_SMAC0,fieldIndexStart, fieldIndexEnd, &hitIndex)) != RT_ERR_OK)
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
            if ((ret = _dal_apollomp_acl_ruleField_check(pTemplate, ACL_FIELD_SMAC1,fieldIndexStart, fieldIndexEnd, &hitIndex)) != RT_ERR_OK)
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
            if ((ret = _dal_apollomp_acl_ruleField_check(pTemplate, ACL_FIELD_SMAC2,fieldIndexStart, fieldIndexEnd, &hitIndex)) != RT_ERR_OK)
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
            if ((ret = _dal_apollomp_acl_ruleField_check(pTemplate, ACL_FIELD_IPV4_SIP0,fieldIndexStart, fieldIndexEnd, &hitIndex)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
                return ret;
            }
            val = pField->fieldUnion.ip.value & 0xFFFF;
            pRawRule->data_bits.field[hitIndex - fieldIndexOffset] = val;

            val = pField->fieldUnion.ip.mask & 0xFFFF;
            pRawRule->care_bits.field[hitIndex - fieldIndexOffset] = val;

            if ((ret = _dal_apollomp_acl_ruleField_check(pTemplate, ACL_FIELD_IPV4_SIP1,fieldIndexStart, fieldIndexEnd, &hitIndex)) != RT_ERR_OK)
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
            if ((ret = _dal_apollomp_acl_ruleField_check(pTemplate, ACL_FIELD_IPV4_SIP0,fieldIndexStart, fieldIndexEnd, &hitIndex)) != RT_ERR_OK)
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
            if ((ret = _dal_apollomp_acl_ruleField_check(pTemplate, ACL_FIELD_IPV4_SIP1,fieldIndexStart, fieldIndexEnd, &hitIndex)) != RT_ERR_OK)
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
            if ((ret = _dal_apollomp_acl_ruleField_check(pTemplate, ACL_FIELD_IPV4_DIP0,fieldIndexStart, fieldIndexEnd, &hitIndex)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
                return ret;
            }
            val = pField->fieldUnion.ip.value & 0xFFFF;
            pRawRule->data_bits.field[hitIndex - fieldIndexOffset] = val;

            val = pField->fieldUnion.ip.mask & 0xFFFF;
            pRawRule->care_bits.field[hitIndex - fieldIndexOffset] = val;

            if ((ret = _dal_apollomp_acl_ruleField_check(pTemplate, ACL_FIELD_IPV4_DIP1,fieldIndexStart, fieldIndexEnd, &hitIndex)) != RT_ERR_OK)
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
            if ((ret = _dal_apollomp_acl_ruleField_check(pTemplate, ACL_FIELD_IPV4_DIP0,fieldIndexStart, fieldIndexEnd, &hitIndex)) != RT_ERR_OK)
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
            if ((ret = _dal_apollomp_acl_ruleField_check(pTemplate, ACL_FIELD_IPV4_DIP1,fieldIndexStart, fieldIndexEnd, &hitIndex)) != RT_ERR_OK)
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
            if ((ret = _dal_apollomp_acl_ruleField_check(pTemplate, ACL_FIELD_IPV6_SIP0,fieldIndexStart, fieldIndexEnd, &hitIndex)) != RT_ERR_OK)
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

            if ((ret = _dal_apollomp_acl_ruleField_check(pTemplate, ACL_FIELD_IPV6_SIP1,fieldIndexStart, fieldIndexEnd, &hitIndex)) != RT_ERR_OK)
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
            if ((ret = _dal_apollomp_acl_ruleField_check(pTemplate, ACL_FIELD_IPV6_SIP0,fieldIndexStart, fieldIndexEnd, &hitIndex)) != RT_ERR_OK)
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
            if ((ret = _dal_apollomp_acl_ruleField_check(pTemplate, ACL_FIELD_IPV6_SIP1,fieldIndexStart, fieldIndexEnd, &hitIndex)) != RT_ERR_OK)
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
            if ((ret = _dal_apollomp_acl_ruleField_check(pTemplate, ACL_FIELD_IPV6_DIP0,fieldIndexStart, fieldIndexEnd, &hitIndex)) != RT_ERR_OK)
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

            if ((ret = _dal_apollomp_acl_ruleField_check(pTemplate, ACL_FIELD_IPV6_DIP1,fieldIndexStart, fieldIndexEnd, &hitIndex)) != RT_ERR_OK)
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
            if ((ret = _dal_apollomp_acl_ruleField_check(pTemplate, ACL_FIELD_IPV6_DIP0,fieldIndexStart, fieldIndexEnd, &hitIndex)) != RT_ERR_OK)
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
            if ((ret = _dal_apollomp_acl_ruleField_check(pTemplate, ACL_FIELD_IPV6_DIP1,fieldIndexStart, fieldIndexEnd, &hitIndex)) != RT_ERR_OK)
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




#if defined(CONFIG_SDK_KERNEL_LINUX)

uint32 dal_apollomp_acl_isr_entry(void)
{
    int32  ret;

    /* switch interrupt clear ACL state */
    if((ret=dal_apollomp_intr_ims_clear(INTR_TYPE_ACL_ACTION)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
        return ret;
	}
    
	_actionIntCnt ++;
    
    return RT_ERR_OK; 
}


uint32 dal_apollomp_acl_isr_init(void)
{
    int32  ret;
  
	/*register ACL isr*/
	if((ret = rtk_irq_isr_register(INTR_TYPE_ACL_ACTION,dal_apollomp_acl_isr_entry)) != RT_ERR_OK)
	{
		RT_ERR(ret,(MOD_EPON | MOD_DAL), "");
		return ret;
	}

    if((ret = dal_apollomp_intr_imr_set(INTR_TYPE_ACL_ACTION,ENABLED)) != RT_ERR_OK)
    {
    	RT_ERR(ret,(MOD_EPON | MOD_DAL), "");
		return ret;
    }

    return RT_ERR_OK;    
}    
#endif


/* Function Name:
 *      dal_apollomp_acl_dbgInfo_get
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
int32 dal_apollomp_acl_dbgInfo_get(rtk_acl_dbgCnt_t *pDbgCnt)
{
    /* check Init status */
    RT_INIT_CHK(apollomp_acl_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pDbgCnt), RT_ERR_NULL_POINTER);

    /* function body */
    pDbgCnt->actionIntCnt = _actionIntCnt;

    /*reset counter*/
    _actionIntCnt=0;

    return RT_ERR_OK;
}   /* end of dal_apollomp_acl_dbgInfo_get */


/* Function Name:
 *      dal_apollomp_acl_init
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
dal_apollomp_acl_init(void)
{
    int32   ret;
    uint32  val;
    uint32  index;
    uint32  fieldIndex;
    rtk_acl_igr_rule_mode_t mode;
    rtk_enable_t state;
    rtk_port_t port;
	rtk_acl_field_entry_t fieldSelect;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_ACL),"%s",__FUNCTION__);

    apollomp_acl_init = INIT_COMPLETED;

    mode = ACL_IGR_RULE_MODE_0;
    if ((ret = dal_apollomp_acl_igrRuleMode_set(mode)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        apollomp_acl_init = INIT_NOT_COMPLETED;
        return ret;
    }

    HAL_SCAN_ALL_PORT(port)
    {
        state = DISABLED;
        if ((ret = dal_apollomp_acl_igrState_set(port, state)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
            apollomp_acl_init = INIT_NOT_COMPLETED;
            return ret;
        }

        state = ENABLED;
        if ((ret = dal_apollomp_acl_igrUnmatchAction_set(port, state)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
            apollomp_acl_init = INIT_NOT_COMPLETED;
            return ret;
        }
    }

    for(index = 0; index < HAL_MAX_NUM_OF_ACL_TEMPLATE(); index ++)
    {
        for(fieldIndex = 0; fieldIndex < HAL_MAX_NUM_OF_ACL_RULE_FIELD(); fieldIndex ++)
        {
            val = (uint32)apollomp_dal_template_init[index][fieldIndex];

    	    if ((ret = reg_array_field_write(APOLLOMP_ACL_TEMPLATE_CTRLr, index, fieldIndex, APOLLOMP_FIELDf, &val)) != RT_ERR_OK )
            {
                RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
                apollomp_acl_init = INIT_NOT_COMPLETED;
                return ret;
            }
        }
    }


	fieldSelect.offset = 0;
	fieldSelect.format = ACL_FORMAT_DEFAULT;
    for(fieldSelect.index = 0; fieldSelect.index < HAL_MAX_NUM_OF_FIELD_SELECTOR(); fieldSelect.index ++)
    {
	    if ((ret = dal_apollomp_acl_fieldSelect_set(&fieldSelect)) != RT_ERR_OK )
        {
            RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
            apollomp_acl_init = INIT_NOT_COMPLETED;
            return ret;
        }
    }

    if ((ret = dal_apollomp_acl_igrRuleEntry_delAll()) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        apollomp_acl_init = INIT_NOT_COMPLETED;
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_acl_init */

/* Function Name:
 *      dal_apollomp_acl_template_set
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
dal_apollomp_acl_template_set(rtk_acl_template_t *pTemplate)
{
    int32   ret;
    uint32  index;
    uint32  fieldIndex;
    uint32  val;
    uint32  type;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_ACL),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_acl_init);

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

                val = APOLLOMP_DAL_ACL_UNUSED;
                break;
            case ACL_FIELD_DMAC0:

                val = APOLLOMP_DAL_ACL_DMAC0;
                break;
            case ACL_FIELD_DMAC1:

                val = APOLLOMP_DAL_ACL_DMAC1;
                break;
            case ACL_FIELD_DMAC2:

                val = APOLLOMP_DAL_ACL_DMAC2;
                break;
            case ACL_FIELD_SMAC0:

                val = APOLLOMP_DAL_ACL_SMAC0;
                break;
            case ACL_FIELD_SMAC1:

                val = APOLLOMP_DAL_ACL_SMAC1;
                break;
            case ACL_FIELD_SMAC2:

                val = APOLLOMP_DAL_ACL_SMAC2;
                break;
            case ACL_FIELD_ETHERTYPE:

                val = APOLLOMP_DAL_ACL_ETHERTYPE;
                break;
            case ACL_FIELD_CTAG:

                val = APOLLOMP_DAL_ACL_CTAG;
                break;
            case ACL_FIELD_STAG:

                val = APOLLOMP_DAL_ACL_STAG;
                break;
            case ACL_FIELD_GEMPORT:

                val = APOLLOMP_DAL_ACL_GEMIDXLLIDX;
                break;
            case ACL_FIELD_IPV4_SIP0:

                val = APOLLOMP_DAL_ACL_IP4SIP0;
                break;
            case ACL_FIELD_IPV4_SIP1:

                val = APOLLOMP_DAL_ACL_IP4SIP1;
                break;
            case ACL_FIELD_IPV4_DIP0:

                val = APOLLOMP_DAL_ACL_IP4DIP0;
                break;
            case ACL_FIELD_IPV4_DIP1:

                val = APOLLOMP_DAL_ACL_IP4DIP1;
                break;
            case ACL_FIELD_IPV6_SIP0:

                val = APOLLOMP_DAL_ACL_IP6SIP0WITHIPV4;
                break;
            case ACL_FIELD_IPV6_SIP1:

                val = APOLLOMP_DAL_ACL_IP6SIP1WITHIPV4;
                break;
            case ACL_FIELD_IPV6_DIP0:

                val = APOLLOMP_DAL_ACL_IP6DIP0WITHIPV4;
                break;
            case ACL_FIELD_IPV6_DIP1:

                val = APOLLOMP_DAL_ACL_IP6DIP1WITHIPV4;
                break;
            case ACL_FIELD_IPV6_NEXT_HEADER:
			case ACL_FIELD_IPV4_PROTOCOL:

                val = APOLLOMP_DAL_ACL_IP6NH;
                break;
            case ACL_FIELD_VID_RANGE:

                val = APOLLOMP_DAL_ACL_VIDRANGE;
                break;
            case ACL_FIELD_IP_RANGE:

                val = APOLLOMP_DAL_ACL_IPRANGE;
                break;
            case ACL_FIELD_PORT_RANGE:

                val = APOLLOMP_DAL_ACL_PORTRANGE;
                break;
            case ACL_FIELD_PKT_LEN_RANGE:

                val = APOLLOMP_DAL_ACL_PKTLENRANGE;
                break;
            case ACL_FIELD_EXT_PORTMASK:

                val = APOLLOMP_DAL_ACL_EXT_PMSK;
                break;
            case ACL_FIELD_USER_VALID:

                val = APOLLOMP_DAL_ACL_FIELD_VALID;
                break;
            case ACL_FIELD_USER_DEFINED00:

                val = APOLLOMP_DAL_ACL_FIELD_SELECT00;
                break;
            case ACL_FIELD_USER_DEFINED01:

                val = APOLLOMP_DAL_ACL_FIELD_SELECT01;
                break;
            case ACL_FIELD_USER_DEFINED02:

                val = APOLLOMP_DAL_ACL_FIELD_SELECT02;
                break;
            case ACL_FIELD_USER_DEFINED03:

                val = APOLLOMP_DAL_ACL_FIELD_SELECT03;
                break;
            case ACL_FIELD_USER_DEFINED04:

                val = APOLLOMP_DAL_ACL_FIELD_SELECT04;
                break;
            case ACL_FIELD_USER_DEFINED05:

                val = APOLLOMP_DAL_ACL_FIELD_SELECT05;
                break;
            case ACL_FIELD_USER_DEFINED06:

                val = APOLLOMP_DAL_ACL_FIELD_SELECT06;
                break;
            case ACL_FIELD_USER_DEFINED07:

                val = APOLLOMP_DAL_ACL_FIELD_SELECT07;
                break;
            case ACL_FIELD_USER_DEFINED08:

                val = APOLLOMP_DAL_ACL_FIELD_SELECT08;
                break;
            case ACL_FIELD_USER_DEFINED09:

                val = APOLLOMP_DAL_ACL_FIELD_SELECT09;
                break;
            case ACL_FIELD_USER_DEFINED10:

                val = APOLLOMP_DAL_ACL_FIELD_SELECT10;
                break;
            case ACL_FIELD_USER_DEFINED11:

                val = APOLLOMP_DAL_ACL_FIELD_SELECT11;
                break;
            case ACL_FIELD_USER_DEFINED12:

                val = APOLLOMP_DAL_ACL_FIELD_SELECT12;
                break;
            case ACL_FIELD_USER_DEFINED13:

                val = APOLLOMP_DAL_ACL_FIELD_SELECT13;
                break;
            case ACL_FIELD_USER_DEFINED14:

                val = APOLLOMP_DAL_ACL_FIELD_SELECT14;
                break;
            case ACL_FIELD_USER_DEFINED15:

                val = APOLLOMP_DAL_ACL_FIELD_SELECT15;
                break;
            default:

                return RT_ERR_CHIP_NOT_SUPPORTED;
        }

        if((ret = reg_array_field_write(APOLLOMP_ACL_TEMPLATE_CTRLr, index, fieldIndex, APOLLOMP_FIELDf, &val)) != RT_ERR_OK )
        {
            RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
            return ret;
        }
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_acl_template_set */


/* Function Name:
 *      dal_apollomp_acl_template_get
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
dal_apollomp_acl_template_get(rtk_acl_template_t *pTemplate)
{
    int32   ret;
    uint32  index;
    uint32  val;
    uint32  fieldIndex;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_ACL),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_acl_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pTemplate), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pTemplate->index >= HAL_MAX_NUM_OF_ACL_TEMPLATE()), RT_ERR_OUT_OF_RANGE);

    index = pTemplate->index;

    for (fieldIndex = 0; fieldIndex < HAL_MAX_NUM_OF_ACL_RULE_FIELD(); fieldIndex ++)
    {
        if ((ret = reg_array_field_read(APOLLOMP_ACL_TEMPLATE_CTRLr, index, fieldIndex, APOLLOMP_FIELDf, &val)) != RT_ERR_OK )
        {
            RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
            return ret;
        }

        /*change RTK to DAL definition*/
        switch(val)
        {
            case APOLLOMP_DAL_ACL_UNUSED:

                pTemplate->fieldType[fieldIndex] = ACL_FIELD_UNUSED;
                break;
            case APOLLOMP_DAL_ACL_DMAC0:

                pTemplate->fieldType[fieldIndex] = ACL_FIELD_DMAC0;
                break;
            case APOLLOMP_DAL_ACL_DMAC1:

                pTemplate->fieldType[fieldIndex] = ACL_FIELD_DMAC1;
                break;
            case APOLLOMP_DAL_ACL_DMAC2:

                pTemplate->fieldType[fieldIndex] = ACL_FIELD_DMAC2;
                break;
            case APOLLOMP_DAL_ACL_SMAC0:

                pTemplate->fieldType[fieldIndex] = ACL_FIELD_SMAC0;
                break;
            case APOLLOMP_DAL_ACL_SMAC1:

                pTemplate->fieldType[fieldIndex] = ACL_FIELD_SMAC1;
                break;
            case APOLLOMP_DAL_ACL_SMAC2:

                pTemplate->fieldType[fieldIndex] = ACL_FIELD_SMAC2;
                break;
            case APOLLOMP_DAL_ACL_ETHERTYPE:

                pTemplate->fieldType[fieldIndex] = ACL_FIELD_ETHERTYPE;
                break;
            case APOLLOMP_DAL_ACL_CTAG:

                pTemplate->fieldType[fieldIndex] = ACL_FIELD_CTAG;
                break;
            case APOLLOMP_DAL_ACL_STAG:

                pTemplate->fieldType[fieldIndex] = ACL_FIELD_STAG;
                break;
            case APOLLOMP_DAL_ACL_GEMIDXLLIDX:

                pTemplate->fieldType[fieldIndex] = ACL_FIELD_GEMPORT;
                break;
            case APOLLOMP_DAL_ACL_IP4SIP0:

                pTemplate->fieldType[fieldIndex] = ACL_FIELD_IPV4_SIP0;
                break;
            case APOLLOMP_DAL_ACL_IP4SIP1:

                pTemplate->fieldType[fieldIndex] = ACL_FIELD_IPV4_SIP1;
                break;
            case APOLLOMP_DAL_ACL_IP4DIP0:

                pTemplate->fieldType[fieldIndex] = ACL_FIELD_IPV4_DIP0;
                break;
            case APOLLOMP_DAL_ACL_IP4DIP1:

                pTemplate->fieldType[fieldIndex] = ACL_FIELD_IPV4_DIP1;
                break;
            case APOLLOMP_DAL_ACL_IP6SIP0WITHIPV4:

                pTemplate->fieldType[fieldIndex] = ACL_FIELD_IPV6_SIP0;
                break;
            case APOLLOMP_DAL_ACL_IP6SIP1WITHIPV4:

                pTemplate->fieldType[fieldIndex] = ACL_FIELD_IPV6_SIP1;
                break;
            case APOLLOMP_DAL_ACL_IP6DIP0WITHIPV4:

                pTemplate->fieldType[fieldIndex] = ACL_FIELD_IPV6_DIP0;
                break;
            case APOLLOMP_DAL_ACL_IP6DIP1WITHIPV4:

                pTemplate->fieldType[fieldIndex] = ACL_FIELD_IPV6_DIP1;
                break;
            case APOLLOMP_DAL_ACL_IP6NH:

                pTemplate->fieldType[fieldIndex] = ACL_FIELD_IPV6_NEXT_HEADER;
                break;
            case APOLLOMP_DAL_ACL_VIDRANGE:

                pTemplate->fieldType[fieldIndex] = ACL_FIELD_VID_RANGE;
                break;
            case APOLLOMP_DAL_ACL_IPRANGE:

                pTemplate->fieldType[fieldIndex] = ACL_FIELD_IP_RANGE;
                break;
            case APOLLOMP_DAL_ACL_PORTRANGE:

                pTemplate->fieldType[fieldIndex] = ACL_FIELD_PORT_RANGE;
                break;
            case APOLLOMP_DAL_ACL_PKTLENRANGE:

                pTemplate->fieldType[fieldIndex] = ACL_FIELD_PKT_LEN_RANGE;
                break;
            case APOLLOMP_DAL_ACL_FIELD_VALID:

                pTemplate->fieldType[fieldIndex] = ACL_FIELD_USER_VALID;
                break;
            case APOLLOMP_DAL_ACL_EXT_PMSK:

                pTemplate->fieldType[fieldIndex] = ACL_FIELD_EXT_PORTMASK;
                break;

            case APOLLOMP_DAL_ACL_FIELD_SELECT00:

                pTemplate->fieldType[fieldIndex] = ACL_FIELD_USER_DEFINED00;
                break;
            case APOLLOMP_DAL_ACL_FIELD_SELECT01:

                pTemplate->fieldType[fieldIndex] = ACL_FIELD_USER_DEFINED01;
                break;
            case APOLLOMP_DAL_ACL_FIELD_SELECT02:

                pTemplate->fieldType[fieldIndex] = ACL_FIELD_USER_DEFINED02;
                break;
            case APOLLOMP_DAL_ACL_FIELD_SELECT03:

                pTemplate->fieldType[fieldIndex] = ACL_FIELD_USER_DEFINED03;
                break;
            case APOLLOMP_DAL_ACL_FIELD_SELECT04:

                pTemplate->fieldType[fieldIndex] = ACL_FIELD_USER_DEFINED04;
                break;
            case APOLLOMP_DAL_ACL_FIELD_SELECT05:

                pTemplate->fieldType[fieldIndex] = ACL_FIELD_USER_DEFINED05;
                break;
            case APOLLOMP_DAL_ACL_FIELD_SELECT06:

                pTemplate->fieldType[fieldIndex] = ACL_FIELD_USER_DEFINED06;
                break;
            case APOLLOMP_DAL_ACL_FIELD_SELECT07:

                pTemplate->fieldType[fieldIndex] = ACL_FIELD_USER_DEFINED07;
                break;
            case APOLLOMP_DAL_ACL_FIELD_SELECT08:

                pTemplate->fieldType[fieldIndex] = ACL_FIELD_USER_DEFINED08;
                break;
            case APOLLOMP_DAL_ACL_FIELD_SELECT09:

                pTemplate->fieldType[fieldIndex] = ACL_FIELD_USER_DEFINED09;
                break;
            case APOLLOMP_DAL_ACL_FIELD_SELECT10:

                pTemplate->fieldType[fieldIndex] = ACL_FIELD_USER_DEFINED10;
                break;
            case APOLLOMP_DAL_ACL_FIELD_SELECT11:

                pTemplate->fieldType[fieldIndex] = ACL_FIELD_USER_DEFINED11;
                break;
            case APOLLOMP_DAL_ACL_FIELD_SELECT12:

                pTemplate->fieldType[fieldIndex] = ACL_FIELD_USER_DEFINED12;
                break;
            case APOLLOMP_DAL_ACL_FIELD_SELECT13:

                pTemplate->fieldType[fieldIndex] = ACL_FIELD_USER_DEFINED13;
                break;
            case APOLLOMP_DAL_ACL_FIELD_SELECT14:

                pTemplate->fieldType[fieldIndex] = ACL_FIELD_USER_DEFINED14;
                break;
            case APOLLOMP_DAL_ACL_FIELD_SELECT15:

                pTemplate->fieldType[fieldIndex] = ACL_FIELD_USER_DEFINED15;
                break;
            default:

                return RT_ERR_CHIP_NOT_SUPPORTED;
        }
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_acl_template_get */

/* Function Name:
 *      dal_apollomp_acl_fieldSelect_set
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
dal_apollomp_acl_fieldSelect_set(rtk_acl_field_entry_t *pFieldEntry)
{
    int32   ret;
    uint32  offset;
    uint32  format;


    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_ACL),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_acl_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pFieldEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(pFieldEntry->index >= HAL_MAX_NUM_OF_FIELD_SELECTOR(), RT_ERR_OUT_OF_RANGE);

    switch(pFieldEntry->format)
    {
        case ACL_FORMAT_DEFAULT:

            format = APOLLOMP_DAL_FIELD_FORMAT_DEFAULT;
            break;
        case ACL_FORMAT_RAW:

            format = APOLLOMP_DAL_FIELD_FORMAT_RAW;
            break;
        case ACL_FORMAT_LLC:

            format = APOLLOMP_DAL_FIELD_FORMAT_LLC;
            break;
        case ACL_FORMAT_IPV4:

            format = APOLLOMP_DAL_FIELD_FORMAT_IPV4;
            break;
        case ACL_FORMAT_ARP:

            format = APOLLOMP_DAL_FIELD_FORMAT_ARP;
            break;
        case ACL_FORMAT_IPV6:

            format = APOLLOMP_DAL_FIELD_FORMAT_IPV6;
            break;
        case ACL_FORMAT_IPPAYLOAD:

            format = APOLLOMP_DAL_FIELD_FORMAT_IPPAYLOAD;
            break;
        case ACL_FORMAT_L4PAYLOAD:

            format = APOLLOMP_DAL_FIELD_FORMAT_L4PAYLOAD;
            break;

        default:

            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    offset = pFieldEntry->offset;

    if ((ret = reg_array_field_write(APOLLOMP_PARSER_FIELD_SELTOR_CTRLr, REG_ARRAY_INDEX_NONE, pFieldEntry->index, APOLLOMP_OFFSETf, &offset)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
    }

    if ((ret = reg_array_field_write(APOLLOMP_PARSER_FIELD_SELTOR_CTRLr, REG_ARRAY_INDEX_NONE,  pFieldEntry->index, APOLLOMP_FMTf, &format)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_acl_fieldSelect_set */

/* Function Name:
 *      dal_apollomp_acl_fieldSelect_get
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
dal_apollomp_acl_fieldSelect_get(rtk_acl_field_entry_t *pFieldEntry)
{
    int32   ret;
    uint32  offset;
    uint32  format;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_ACL),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_acl_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pFieldEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(pFieldEntry->index >= HAL_MAX_NUM_OF_FIELD_SELECTOR(), RT_ERR_OUT_OF_RANGE);

    if ((ret = reg_array_field_read(APOLLOMP_PARSER_FIELD_SELTOR_CTRLr, REG_ARRAY_INDEX_NONE, pFieldEntry->index, APOLLOMP_OFFSETf, &offset)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
    }
    pFieldEntry->offset = offset;

    if ((ret = reg_array_field_read(APOLLOMP_PARSER_FIELD_SELTOR_CTRLr, REG_ARRAY_INDEX_NONE, pFieldEntry->index, APOLLOMP_FMTf, &format)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
    }

    switch(format)
    {
        case APOLLOMP_DAL_FIELD_FORMAT_DEFAULT:

            pFieldEntry->format = ACL_FORMAT_DEFAULT;
            break;
        case APOLLOMP_DAL_FIELD_FORMAT_RAW:

            pFieldEntry->format = ACL_FORMAT_RAW;
            break;
        case APOLLOMP_DAL_FIELD_FORMAT_LLC:

            pFieldEntry->format = ACL_FORMAT_LLC;
            break;
        case APOLLOMP_DAL_FIELD_FORMAT_IPV4:

            pFieldEntry->format = ACL_FORMAT_IPV4;
            break;
        case APOLLOMP_DAL_FIELD_FORMAT_ARP:

            pFieldEntry->format = ACL_FORMAT_ARP;
            break;
        case APOLLOMP_DAL_FIELD_FORMAT_IPV6:

            pFieldEntry->format = ACL_FORMAT_IPV6;
            break;
        case APOLLOMP_DAL_FIELD_FORMAT_IPPAYLOAD:

            pFieldEntry->format = ACL_FORMAT_IPPAYLOAD;
            break;
        case APOLLOMP_DAL_FIELD_FORMAT_L4PAYLOAD:

            pFieldEntry->format = ACL_FORMAT_L4PAYLOAD;
            break;

        default:

            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_acl_fieldSelect_get */
/* Function Name:
 *      dal_apollomp_acl_igrRuleAction_set
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
dal_apollomp_acl_igrRuleAction_set(rtk_acl_ingress_entry_t *pAclRule)
{
    int32  ret;
    uint32 val;
    uint32 index;
    uint32 actionData[APOLLOMP_ACL_ACTION_DATA_LEN];
    uint32 vidx;
    uint32 valAct;
	rtk_acl_igr_act_type_t checkIdx;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_ACL),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_acl_init);

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

    val = pAclRule->valid;
    if ((ret = reg_array_field_write(APOLLOMP_ACL_ACTIONr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_VALIDf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    val = pAclRule->invert;
    if ((ret = reg_array_field_write(APOLLOMP_ACL_ACTIONr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_NOTf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }
	
    val = pAclRule->act.enableAct[ACL_IGR_INTR_ACT];
    if ((ret = reg_array_field_write(APOLLOMP_ACL_ACTIONr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_INT_CFf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    val = pAclRule->act.enableAct[ACL_IGR_FORWARD_ACT];
    if ((ret = reg_array_field_write(APOLLOMP_ACL_ACTIONr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_FWDf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    val = pAclRule->act.enableAct[ACL_IGR_LOG_ACT];
    if ((ret = reg_array_field_write(APOLLOMP_ACL_ACTIONr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_POLICINGf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    val = pAclRule->act.enableAct[ACL_IGR_PRI_ACT];
    if ((ret = reg_array_field_write(APOLLOMP_ACL_ACTIONr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_PRIf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    val = pAclRule->act.enableAct[ACL_IGR_SVLAN_ACT];
    if ((ret = reg_array_field_write(APOLLOMP_ACL_ACTIONr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_SVLANf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    val = pAclRule->act.enableAct[ACL_IGR_CVLAN_ACT];
    if ((ret = reg_array_field_write(APOLLOMP_ACL_ACTIONr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_CVLANf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    /*retrieve action configuration*/
	osal_memset(actionData,0,sizeof(actionData));


    /*Interrupt and extend action control*/
	if(pAclRule->act.enableAct[ACL_IGR_INTR_ACT])
	{
	    switch(pAclRule->act.extendAct.act)
	    {
			case ACL_IGR_EXTEND_EXT_ACT:
				valAct = APOLLOMP_DAL_ACL_IGR_EXTEND_EXT_ACT;
			    val = pAclRule->act.extendAct.portMask.bits[0];
				break;

			case ACL_IGR_EXTEND_LLID_ACT:
				valAct = APOLLOMP_DAL_ACL_IGR_EXTEND_LLID_ACT;
			    val = pAclRule->act.extendAct.index;
				break;
			case ACL_IGR_EXTEND_SID_ACT:
				valAct = APOLLOMP_DAL_ACL_IGR_EXTEND_SID_ACT;
			    val = pAclRule->act.extendAct.index;
				break;
			case ACL_IGR_EXTEND_NONE_ACT:
				valAct = APOLLOMP_DAL_ACL_IGR_EXTEND_NONE_ACT;
			    val = pAclRule->act.extendAct.index;
				break;
			default:
				return RT_ERR_CHIP_NOT_SUPPORTED;
				break;
	   	}
		
		if ((ret = table_field_set(APOLLOMP_ACL_ACTION_TABLEt, APOLLOMP_ACL_ACTION_TABLE_CFACTtf, &valAct, actionData)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
	        return ret;
	    }

		if ((ret = table_field_set(APOLLOMP_ACL_ACTION_TABLEt, APOLLOMP_ACL_ACTION_TABLE_CFIDXtf, &val, actionData)) != RT_ERR_OK)
		{
				RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
				return ret;
		}

	    val = pAclRule->act.aclLatch;
		if ((ret = table_field_set(APOLLOMP_ACL_ACTION_TABLEt, APOLLOMP_ACL_ACTION_TABLE_HITtf, &val, actionData)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
	        return ret;
	    }

	    val = pAclRule->act.aclInterrupt;
		if ((ret = table_field_set(APOLLOMP_ACL_ACTION_TABLEt, APOLLOMP_ACL_ACTION_TABLE_ACLINTtf, &val, actionData)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
	        return ret;
	    }
	}

    /*Priority action control*/
    if(pAclRule->act.enableAct[ACL_IGR_PRI_ACT])
    {
	    switch(pAclRule->act.priAct.act)
	    {
	        case ACL_IGR_PRI_DSCP_REMARK_ACT:
				valAct = APOLLOMP_DAL_ACL_IGR_PRI_DSCP_REMARK_ACT;
	            val = pAclRule->act.priAct.dscp;
	            break;

	        case ACL_IGR_PRI_1P_REMARK_ACT:
				valAct = APOLLOMP_DAL_ACL_IGR_PRI_1P_REMARK_ACT;
	            val = pAclRule->act.priAct.dot1p;
	            break;

	        case ACL_IGR_PRI_POLICING_ACT:
				valAct = APOLLOMP_DAL_ACL_IGR_PRI_POLICING_ACT;
	            val = pAclRule->act.priAct.meter;
	            break;

	        case ACL_IGR_PRI_MIB_ACT:
				valAct = APOLLOMP_DAL_ACL_IGR_PRI_MIB_ACT;
	            val = pAclRule->act.priAct.mib;
	            break;

	        case ACL_IGR_PRI_ACL_PRI_ASSIGN_ACT:
				valAct = APOLLOMP_DAL_ACL_IGR_PRI_ACL_PRI_ASSIGN_ACT;
	            val = pAclRule->act.priAct.aclPri;
	            break;

	        case ACL_IGR_PRI_ROUTE_ACT:
				valAct = APOLLOMP_DAL_ACL_IGR_PRI_ROUTE_ACT;
				switch(DAL_UTIL_CHIP_REV)
			    {
			        case CHIP_REV_ID_A:
			            return RT_ERR_CHIP_NOT_SUPPORTED;
                        break;
			        case CHIP_REV_ID_B:
			        default:
			            val = pAclRule->act.priAct.nexthop;
			            break;
			    }
	            break;
			default:
				return RT_ERR_CHIP_NOT_SUPPORTED;
				break;
	    }
		
		if ((ret = table_field_set(APOLLOMP_ACL_ACTION_TABLEt, APOLLOMP_ACL_ACTION_TABLE_PRIACTtf, &valAct, actionData)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
	        return ret;
	    }

		if ((ret = table_field_set(APOLLOMP_ACL_ACTION_TABLEt, APOLLOMP_ACL_ACTION_TABLE_PRIDXtf, &val, actionData)) != RT_ERR_OK)
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
			    valAct = APOLLOMP_DAL_ACL_IGR_FORWARD_COPY_ACT;
				val = pAclRule->act.forwardAct.portMask.bits[0];
	            break;
	        case ACL_IGR_FORWARD_REDIRECT_ACT:
			    valAct = APOLLOMP_DAL_ACL_IGR_FORWARD_REDIRECT_ACT;
				val = pAclRule->act.forwardAct.portMask.bits[0];
	            break;
	        case ACL_IGR_FORWARD_IGR_MIRROR_ACT:
			    valAct = APOLLOMP_DAL_ACL_IGR_FORWARD_IGR_MIRROR_ACT;
				val = pAclRule->act.forwardAct.portMask.bits[0];
	            break;
			case ACL_IGR_FORWARD_TRAP_ACT:
			    valAct = APOLLOMP_DAL_ACL_IGR_FORWARD_TRAP_ACT;
				val = 0;
	            break;
	        case ACL_IGR_FORWARD_DROP_ACT:
			    valAct = APOLLOMP_DAL_ACL_IGR_FORWARD_REDIRECT_ACT;
				val = 0;
	            break;
			default:
				return RT_ERR_CHIP_NOT_SUPPORTED;
				break;
	    }

		if ((ret = table_field_set(APOLLOMP_ACL_ACTION_TABLEt, APOLLOMP_ACL_ACTION_TABLE_FWDACTtf, &valAct, actionData)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
	        return ret;
	    }

		if ((ret = table_field_set(APOLLOMP_ACL_ACTION_TABLEt, APOLLOMP_ACL_ACTION_TABLE_FWD_PMSKtf, &val, actionData)) != RT_ERR_OK)
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
				valAct = APOLLOMP_DAL_ACL_IGR_LOG_POLICING_ACT;
	            val = pAclRule->act.logAct.meter;
	            break;

	        case ACL_IGR_LOG_MIB_ACT:
				valAct = APOLLOMP_DAL_ACL_IGR_LOG_MIB_ACT;
	            val = pAclRule->act.logAct.mib;
	            break;

			default:
				return RT_ERR_CHIP_NOT_SUPPORTED;
				break;
	    }
		
		if ((ret = table_field_set(APOLLOMP_ACL_ACTION_TABLEt, APOLLOMP_ACL_ACTION_TABLE_POLICACTtf, &valAct, actionData)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
	        return ret;
	    }

		if ((ret = table_field_set(APOLLOMP_ACL_ACTION_TABLEt, APOLLOMP_ACL_ACTION_TABLE_METER_IDXtf, &val, actionData)) != RT_ERR_OK)
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
	        case ACL_IGR_SVLAN_POLICING_ACT:
				valAct = APOLLOMP_DAL_ACL_IGR_SVLAN_POLICING_ACT;
	            val = pAclRule->act.svlanAct.meter;
	            break;
	        case ACL_IGR_SVLAN_MIB_ACT:
				valAct = APOLLOMP_DAL_ACL_IGR_SVLAN_MIB_ACT;
	            val = pAclRule->act.svlanAct.mib;
	            break;
	        case ACL_IGR_SVLAN_1P_REMARK_ACT:
				valAct = APOLLOMP_DAL_ACL_IGR_SVLAN_1P_REMARK_ACT;
	            val = pAclRule->act.svlanAct.dot1p;
	            break;
	        case ACL_IGR_SVLAN_DSCP_REMARK_ACT:
				valAct = APOLLOMP_DAL_ACL_IGR_SVLAN_DSCP_REMARK_ACT;
	            val = pAclRule->act.svlanAct.dscp;
	            break;
	        case ACL_IGR_SVLAN_IGR_SVLAN_ACT:
				valAct = APOLLOMP_DAL_ACL_IGR_SVLAN_IGR_SVLAN_ACT;
	            if ((ret = dal_apollomp_svlan_checkAndCreate(pAclRule->act.svlanAct.svid, &vidx)) != RT_ERR_OK)
	            {
	                RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
	                return ret;
	            }

	            val = vidx;
	            break;
	        case ACL_IGR_SVLAN_EGR_SVLAN_ACT:
				valAct = APOLLOMP_DAL_ACL_IGR_SVLAN_EGR_SVLAN_ACT;
	            if ((ret = dal_apollomp_svlan_checkAndCreate(pAclRule->act.svlanAct.svid, &vidx)) != RT_ERR_OK)
	            {
	                RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
	                return ret;
	            }

	            val = vidx;
	            break;
	        case ACL_IGR_SVLAN_US_CVID_ACT:
				valAct = APOLLOMP_DAL_ACL_IGR_SVLAN_US_CVID_ACT;
				val = 0;
				break;
	        case ACL_IGR_SVLAN_ROUTE_ACT:
				valAct = APOLLOMP_DAL_ACL_IGR_SVLAN_ROUTE_ACT;

				switch(DAL_UTIL_CHIP_REV)
			    {
			        case CHIP_REV_ID_A:
			            return RT_ERR_CHIP_NOT_SUPPORTED;
			            break;
			        case CHIP_REV_ID_B:
			        default:
			            val = pAclRule->act.svlanAct.nexthop;
			            break;
			    }
				break;
	        default:
				return RT_ERR_CHIP_NOT_SUPPORTED;
				break;
	    }
		
		if ((ret = table_field_set(APOLLOMP_ACL_ACTION_TABLEt, APOLLOMP_ACL_ACTION_TABLE_SACTtf, &valAct, actionData)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
	        return ret;
	    }


	    if ((ret = table_field_set(APOLLOMP_ACL_ACTION_TABLEt, APOLLOMP_ACL_ACTION_TABLE_SVIDXtf, &val, actionData)) != RT_ERR_OK)
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
	        case ACL_IGR_CVLAN_POLICING_ACT:
				valAct = APOLLOMP_DAL_ACL_IGR_CVLAN_POLICING_ACT;
	            val = pAclRule->act.cvlanAct.meter;
	            break;
	        case ACL_IGR_CVLAN_MIB_ACT:
				valAct = APOLLOMP_DAL_ACL_IGR_CVLAN_MIB_ACT;
	            val = pAclRule->act.cvlanAct.mib;
	            break;
	        case ACL_IGR_CVLAN_1P_REMARK_ACT:
				valAct = APOLLOMP_DAL_ACL_IGR_CVLAN_1P_REMARK_ACT;
	            val = pAclRule->act.cvlanAct.dot1p;
	            break;
	        case ACL_IGR_CVLAN_IGR_CVLAN_ACT:
				valAct = APOLLOMP_DAL_ACL_IGR_CVLAN_IGR_CVLAN_ACT;
				if((ret = dal_apollomp_vlan_existCheck(pAclRule->act.cvlanAct.cvid)) != RT_ERR_OK)
		        {
					if((ret = dal_apollomp_vlan_create(pAclRule->act.cvlanAct.cvid)) != RT_ERR_OK)
					{
						RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
						return ret;
					}
		        }

	            if ((ret = dal_apollomp_vlan_checkAndCreate(pAclRule->act.cvlanAct.cvid, &vidx)) != RT_ERR_OK)
	            {
	                RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
	                return ret;
	            }

	            val = vidx;
	            break;
	        case ACL_IGR_CVLAN_EGR_CVLAN_ACT:
				valAct = APOLLOMP_DAL_ACL_IGR_CVLAN_EGR_CVLAN_ACT;
				if((ret = dal_apollomp_vlan_existCheck(pAclRule->act.cvlanAct.cvid)) != RT_ERR_OK)
		        {
					if((ret = dal_apollomp_vlan_create(pAclRule->act.cvlanAct.cvid)) != RT_ERR_OK)
					{
						RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
						return ret;
					}
		        }

	            if ((ret = dal_apollomp_vlan_checkAndCreate(pAclRule->act.cvlanAct.cvid, &vidx)) != RT_ERR_OK)
	            {
	                RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
	                return ret;
	            }

	            val = vidx;
	            break;
	        case ACL_IGR_CVLAN_DS_SVID_ACT:
				valAct = APOLLOMP_DAL_ACL_IGR_CVLAN_DS_SVID_ACT;
	            val = 0;
	            break;
	        default:
				return RT_ERR_CHIP_NOT_SUPPORTED;
				break;
	    }

		if ((ret = table_field_set(APOLLOMP_ACL_ACTION_TABLEt, APOLLOMP_ACL_ACTION_TABLE_CACTtf, &valAct, actionData)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
	        return ret;
	    }

	    if ((ret = table_field_set(APOLLOMP_ACL_ACTION_TABLEt, APOLLOMP_ACL_ACTION_TABLE_CVIDXtf, &val, actionData)) != RT_ERR_OK)
		{
				RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
				return ret;
		}
    }

	if ((ret = table_write(APOLLOMP_ACL_ACTION_TABLEt, index, actionData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_acl_igrRuleAction_set */


/* Function Name:
 *      dal_apollomp_acl_igrRuleAction_get
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
dal_apollomp_acl_igrRuleAction_get(rtk_acl_ingress_entry_t *pAclRule)
{
    int32  ret;
    uint32 val;
    uint32 index;
    uint32 actionData[APOLLOMP_ACL_ACTION_DATA_LEN];
    uint32 vid;
    uint32 valAct;

    /* check Init status */
    RT_INIT_CHK(apollomp_acl_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pAclRule), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pAclRule->index >= HAL_MAX_NUM_OF_ACL_ACTION()), RT_ERR_OUT_OF_RANGE);

    index = (uint32)pAclRule->index;

    if ((ret = reg_array_field_read(APOLLOMP_ACL_ACTIONr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_VALIDf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }
    pAclRule->valid = (rtk_enable_t)val;

    if ((ret = reg_array_field_read(APOLLOMP_ACL_ACTIONr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_NOTf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }
    pAclRule->invert = (rtk_acl_invert_t)val;

    if ((ret = reg_array_field_read(APOLLOMP_ACL_ACTIONr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_INT_CFf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }
    pAclRule->act.enableAct[ACL_IGR_INTR_ACT] = (rtk_enable_t)val;

    if ((ret = reg_array_field_read(APOLLOMP_ACL_ACTIONr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_FWDf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }
    pAclRule->act.enableAct[ACL_IGR_FORWARD_ACT] = (rtk_enable_t)val;

    if ((ret = reg_array_field_read(APOLLOMP_ACL_ACTIONr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_POLICINGf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }
    pAclRule->act.enableAct[ACL_IGR_LOG_ACT] = (rtk_enable_t)val;

    if ((ret = reg_array_field_read(APOLLOMP_ACL_ACTIONr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_PRIf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }
    pAclRule->act.enableAct[ACL_IGR_PRI_ACT] = (rtk_enable_t)val;

    if ((ret = reg_array_field_read(APOLLOMP_ACL_ACTIONr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_SVLANf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }
    pAclRule->act.enableAct[ACL_IGR_SVLAN_ACT] = (rtk_enable_t)val;

    if ((ret = reg_array_field_read(APOLLOMP_ACL_ACTIONr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_CVLANf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }
    pAclRule->act.enableAct[ACL_IGR_CVLAN_ACT] = (rtk_enable_t)val;

    /*retrieve action configuration*/
	osal_memset(actionData,0,sizeof(actionData));

	if ((ret = table_read(APOLLOMP_ACL_ACTION_TABLEt, index, actionData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    /*Interrupt and extend action control*/
	if ((ret = table_field_get(APOLLOMP_ACL_ACTION_TABLEt, APOLLOMP_ACL_ACTION_TABLE_CFACTtf, &valAct, actionData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }
	
	if ((ret = table_field_get(APOLLOMP_ACL_ACTION_TABLEt, APOLLOMP_ACL_ACTION_TABLE_CFIDXtf, &val, actionData)) != RT_ERR_OK)
	{
			RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
			return ret;
	}

    switch(valAct)
    {
		case APOLLOMP_DAL_ACL_IGR_EXTEND_EXT_ACT:
			pAclRule->act.extendAct.act = ACL_IGR_EXTEND_EXT_ACT;
			pAclRule->act.extendAct.portMask.bits[0] = val;
			break;
		case APOLLOMP_DAL_ACL_IGR_EXTEND_SID_ACT:
			pAclRule->act.extendAct.act = ACL_IGR_EXTEND_SID_ACT;
			pAclRule->act.extendAct.index = (uint8)val;
			break;
		case APOLLOMP_DAL_ACL_IGR_EXTEND_LLID_ACT:
			pAclRule->act.extendAct.act = ACL_IGR_EXTEND_LLID_ACT;
			pAclRule->act.extendAct.index = (uint8)val;
			break;
		case APOLLOMP_DAL_ACL_IGR_EXTEND_NONE_ACT:
			pAclRule->act.extendAct.act = ACL_IGR_EXTEND_NONE_ACT;
			pAclRule->act.extendAct.index = (uint8)val;
			break;
        default:
			return RT_ERR_CHIP_NOT_SUPPORTED;
            break;
   	}

	if ((ret = table_field_get(APOLLOMP_ACL_ACTION_TABLEt, APOLLOMP_ACL_ACTION_TABLE_HITtf, &val, actionData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }
    pAclRule->act.aclLatch = (rtk_enable_t)val;

	if ((ret = table_field_get(APOLLOMP_ACL_ACTION_TABLEt, APOLLOMP_ACL_ACTION_TABLE_ACLINTtf, &val, actionData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }
    pAclRule->act.aclInterrupt = (rtk_enable_t)val;

    /*Priority action control*/
	if ((ret = table_field_get(APOLLOMP_ACL_ACTION_TABLEt, APOLLOMP_ACL_ACTION_TABLE_PRIACTtf, &valAct, actionData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

	if ((ret = table_field_get(APOLLOMP_ACL_ACTION_TABLEt, APOLLOMP_ACL_ACTION_TABLE_PRIDXtf, &val, actionData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    switch(valAct)
    {
        case APOLLOMP_DAL_ACL_IGR_PRI_DSCP_REMARK_ACT:
			pAclRule->act.priAct.act = ACL_IGR_PRI_DSCP_REMARK_ACT;
            pAclRule->act.priAct.dscp = (uint8)val;
            break;

        case APOLLOMP_DAL_ACL_IGR_PRI_1P_REMARK_ACT:
			pAclRule->act.priAct.act = ACL_IGR_PRI_1P_REMARK_ACT;
            pAclRule->act.priAct.dot1p = (uint8)val;
            break;

        case APOLLOMP_DAL_ACL_IGR_PRI_POLICING_ACT:
			pAclRule->act.priAct.act = ACL_IGR_PRI_POLICING_ACT;
            pAclRule->act.priAct.meter = (uint8)val;
            break;

        case APOLLOMP_DAL_ACL_IGR_PRI_MIB_ACT:
			pAclRule->act.priAct.act = ACL_IGR_PRI_MIB_ACT;
            pAclRule->act.priAct.mib = (uint8)val;
            break;

        case APOLLOMP_DAL_ACL_IGR_PRI_ACL_PRI_ASSIGN_ACT:
			pAclRule->act.priAct.act = ACL_IGR_PRI_ACL_PRI_ASSIGN_ACT;
            pAclRule->act.priAct.aclPri = (uint8)val;
            break;
			
        case APOLLOMP_DAL_ACL_IGR_PRI_ROUTE_ACT:
			pAclRule->act.priAct.act = ACL_IGR_PRI_ROUTE_ACT;
			switch(DAL_UTIL_CHIP_REV)
		    {
		        case CHIP_REV_ID_A:
        			return RT_ERR_CHIP_NOT_SUPPORTED;
		            break;

		        case CHIP_REV_ID_B:
		        default:
                    pAclRule->act.priAct.nexthop = (uint8)val;
		            break;
		    }
            break;

        default:
			return RT_ERR_CHIP_NOT_SUPPORTED;
            break;
    }

    /*Forward action control*/
	if ((ret = table_field_get(APOLLOMP_ACL_ACTION_TABLEt, APOLLOMP_ACL_ACTION_TABLE_FWDACTtf, &valAct, actionData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }
    switch(valAct)
    {
        case APOLLOMP_DAL_ACL_IGR_FORWARD_COPY_ACT:
			pAclRule->act.forwardAct.act = ACL_IGR_FORWARD_COPY_ACT;
            break;
			
		case APOLLOMP_DAL_ACL_IGR_FORWARD_REDIRECT_ACT:
			pAclRule->act.forwardAct.act = ACL_IGR_FORWARD_REDIRECT_ACT;
			break;
			
		case APOLLOMP_DAL_ACL_IGR_FORWARD_IGR_MIRROR_ACT:
			pAclRule->act.forwardAct.act = ACL_IGR_FORWARD_IGR_MIRROR_ACT;
			break;
			
		case APOLLOMP_DAL_ACL_IGR_FORWARD_TRAP_ACT:
			pAclRule->act.forwardAct.act = ACL_IGR_FORWARD_TRAP_ACT;
			break;
			
        default:
			return RT_ERR_CHIP_NOT_SUPPORTED;
            break;
    }

	if ((ret = table_field_get(APOLLOMP_ACL_ACTION_TABLEt, APOLLOMP_ACL_ACTION_TABLE_FWD_PMSKtf, &val, actionData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }
    pAclRule->act.forwardAct.portMask.bits[0] = val;

    /*Logging or Policing action control*/
	if ((ret = table_field_get(APOLLOMP_ACL_ACTION_TABLEt, APOLLOMP_ACL_ACTION_TABLE_POLICACTtf, &valAct, actionData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }
	
	if ((ret = table_field_get(APOLLOMP_ACL_ACTION_TABLEt, APOLLOMP_ACL_ACTION_TABLE_METER_IDXtf, &val, actionData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }


    switch(valAct)
    {
        case APOLLOMP_DAL_ACL_IGR_LOG_POLICING_ACT:
			pAclRule->act.logAct.act = ACL_IGR_LOG_POLICING_ACT;
			pAclRule->act.logAct.meter = (uint8)val;
            break;
			
		case APOLLOMP_DAL_ACL_IGR_LOG_MIB_ACT:
			pAclRule->act.logAct.act = ACL_IGR_LOG_MIB_ACT;
			pAclRule->act.logAct.mib = (uint8)val;
			break;
			
        default:
			return RT_ERR_CHIP_NOT_SUPPORTED;
            break;
    }


    /*svlan action control*/
	if ((ret = table_field_get(APOLLOMP_ACL_ACTION_TABLEt, APOLLOMP_ACL_ACTION_TABLE_SACTtf, &valAct, actionData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    if ((ret = table_field_get(APOLLOMP_ACL_ACTION_TABLEt, APOLLOMP_ACL_ACTION_TABLE_SVIDXtf, &val, actionData)) != RT_ERR_OK)
	{
			RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
			return ret;
	}

    switch(valAct)
    {
        case APOLLOMP_DAL_ACL_IGR_SVLAN_POLICING_ACT:
			pAclRule->act.svlanAct.act = ACL_IGR_SVLAN_POLICING_ACT;
            pAclRule->act.svlanAct.meter = (uint8)val;
            break;
			
        case APOLLOMP_DAL_ACL_IGR_SVLAN_MIB_ACT:
			pAclRule->act.svlanAct.act = ACL_IGR_SVLAN_MIB_ACT;
            pAclRule->act.svlanAct.mib = (uint8)val;
            break;
			
        case APOLLOMP_DAL_ACL_IGR_SVLAN_1P_REMARK_ACT:
			pAclRule->act.svlanAct.act = ACL_IGR_SVLAN_1P_REMARK_ACT;
            pAclRule->act.svlanAct.dot1p = (uint8)val;
            break;
			
        case APOLLOMP_DAL_ACL_IGR_SVLAN_DSCP_REMARK_ACT:
			pAclRule->act.svlanAct.act = ACL_IGR_SVLAN_DSCP_REMARK_ACT;
            pAclRule->act.svlanAct.dscp = (uint8)val;
            break;
			
        case APOLLOMP_DAL_ACL_IGR_SVLAN_IGR_SVLAN_ACT:
			pAclRule->act.svlanAct.act = ACL_IGR_SVLAN_IGR_SVLAN_ACT;
            if ((ret = reg_array_field_read(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, val, APOLLOMP_SVIDf, &vid)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                return ret;
            }
            pAclRule->act.svlanAct.svid = vid;
            break;
			
        case APOLLOMP_DAL_ACL_IGR_SVLAN_EGR_SVLAN_ACT:
			pAclRule->act.svlanAct.act = ACL_IGR_SVLAN_EGR_SVLAN_ACT;
            if ((ret = reg_array_field_read(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, val, APOLLOMP_SVIDf, &vid)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                return ret;
            }
            pAclRule->act.svlanAct.svid = vid;
            break;
			
        case APOLLOMP_DAL_ACL_IGR_SVLAN_US_CVID_ACT:
			pAclRule->act.svlanAct.act = ACL_IGR_SVLAN_US_CVID_ACT;
            pAclRule->act.svlanAct.svid = 0;
            break;
			
        case APOLLOMP_DAL_ACL_IGR_SVLAN_ROUTE_ACT:
			pAclRule->act.svlanAct.act = ACL_IGR_SVLAN_ROUTE_ACT;
			switch(DAL_UTIL_CHIP_REV)
		    {
		        case CHIP_REV_ID_A:
		            return RT_ERR_CHIP_NOT_SUPPORTED;
		            break;

		        case CHIP_REV_ID_B:
		        default:
                    pAclRule->act.svlanAct.nexthop = (uint8)val;
		            break;
		    }
			break;
			
        default:
			return RT_ERR_CHIP_NOT_SUPPORTED;
            break;
    }

    /*cvlan action control*/
	if ((ret = table_field_get(APOLLOMP_ACL_ACTION_TABLEt, APOLLOMP_ACL_ACTION_TABLE_CACTtf, &valAct, actionData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    if ((ret = table_field_get(APOLLOMP_ACL_ACTION_TABLEt, APOLLOMP_ACL_ACTION_TABLE_CVIDXtf, &val, actionData)) != RT_ERR_OK)
	{
			RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
			return ret;
	}

    switch(valAct)
    {
        case APOLLOMP_DAL_ACL_IGR_CVLAN_POLICING_ACT:
			pAclRule->act.cvlanAct.act = ACL_IGR_CVLAN_POLICING_ACT;
            pAclRule->act.cvlanAct.meter = (uint8)val;
            break;
			
        case APOLLOMP_DAL_ACL_IGR_CVLAN_MIB_ACT:
			pAclRule->act.cvlanAct.act = ACL_IGR_CVLAN_MIB_ACT;
            pAclRule->act.cvlanAct.mib = (uint8)val;
            break;
			
        case APOLLOMP_DAL_ACL_IGR_CVLAN_1P_REMARK_ACT:
			pAclRule->act.cvlanAct.act = ACL_IGR_CVLAN_1P_REMARK_ACT;
            pAclRule->act.cvlanAct.dot1p = (uint8)val;
            break;
			
        case APOLLOMP_DAL_ACL_IGR_CVLAN_IGR_CVLAN_ACT:
			pAclRule->act.cvlanAct.act = ACL_IGR_CVLAN_IGR_CVLAN_ACT;
            if ((ret = reg_array_field_read(APOLLOMP_VLAN_MBR_CFGr,REG_ARRAY_INDEX_NONE, val, APOLLOMP_EVIDf, &vid)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
                return ret;
            }
            pAclRule->act.cvlanAct.cvid = vid;
            break;
			
        case APOLLOMP_DAL_ACL_IGR_CVLAN_EGR_CVLAN_ACT:
			pAclRule->act.cvlanAct.act = ACL_IGR_CVLAN_EGR_CVLAN_ACT;
            if ((ret = reg_array_field_read(APOLLOMP_VLAN_MBR_CFGr,REG_ARRAY_INDEX_NONE, val, APOLLOMP_EVIDf, &vid)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
                return ret;
            }
            pAclRule->act.cvlanAct.cvid = vid;
            break;
			
        case APOLLOMP_DAL_ACL_IGR_CVLAN_DS_SVID_ACT:
			pAclRule->act.cvlanAct.act = ACL_IGR_CVLAN_DS_SVID_ACT;
            pAclRule->act.cvlanAct.cvid = 0;
            break;
			
        default:
			return RT_ERR_CHIP_NOT_SUPPORTED;
            break;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_acl_igrRuleAction_get */

/* Function Name:
 *      dal_apollomp_acl_igrRuleEntry_get
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
dal_apollomp_acl_igrRuleEntry_get(rtk_acl_ingress_entry_t *pAclRule)
{
    int32   ret;
    uint32  val;
    rtk_acl_igr_rule_mode_t mode;
    apollomp_raw_acl_ruleEntry_t rawRule;
    uint16 fieldIndex;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_ACL),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_acl_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pAclRule), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_ACL_CFGr, APOLLOMP_MODEf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }
    mode = (rtk_acl_igr_rule_mode_t)val;

    RT_PARAM_CHK((mode != ACL_IGR_RULE_MODE_0) && (mode != ACL_IGR_RULE_MODE_1), RT_ERR_CHIP_NOT_SUPPORTED);
    RT_PARAM_CHK((mode == ACL_IGR_RULE_MODE_0) &&
                 (pAclRule->index >= HAL_MAX_NUM_OF_ACL_RULE_ENTRY()), RT_ERR_OUT_OF_RANGE);

    RT_PARAM_CHK((mode == ACL_IGR_RULE_MODE_1) &&
                 (pAclRule->index >= HAL_MAX_NUM_OF_ACL_ACTION()), RT_ERR_OUT_OF_RANGE);

    /*retrive rule*/
    rawRule.idx = (uint32)pAclRule->index;
    if(mode == ACL_IGR_RULE_MODE_0)
    {
        rawRule.mode = APOLLOMP_ACL_MODE_64ENTRIES;
    }
    else
    {
        rawRule.mode = APOLLOMP_ACL_MODE_128ENTRIES;
    }

    if((ret = apollomp_raw_acl_rule_get(&rawRule) != RT_ERR_OK ))
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

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
    if ((ret = dal_apollomp_acl_igrRuleAction_get(pAclRule)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_acl_igrRuleEntry_get */


/* Function Name:
 *      dal_apollomp_acl_igrRuleField_add
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
 *      caller should not free (*pAclField) before dal_apollomp_acl_igrRuleEntry_add is called
 */
int32
dal_apollomp_acl_igrRuleField_add(rtk_acl_ingress_entry_t *pAclRule, rtk_acl_field_t *pAclField)
{
	rtk_acl_field_t *tailPtr;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_ACL),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_acl_init);

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
} /* end of dal_apollomp_acl_igrRuleField_add */


/* Function Name:
 *      dal_apollomp_acl_igrRuleEntry_add
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
dal_apollomp_acl_igrRuleEntry_add(rtk_acl_ingress_entry_t *pAclRule)
{
    int32   ret;
    uint32  val;
    rtk_acl_igr_rule_mode_t mode;
    apollomp_raw_acl_ruleEntry_t rawRule;
    rtk_acl_template_t aclTemplate;
    rtk_acl_field_t  *aclField;


    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_ACL),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_acl_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pAclRule), RT_ERR_NULL_POINTER);

    osal_memset(&rawRule,0x0,sizeof(apollomp_raw_acl_ruleEntry_t));

    rawRule.idx = (uint32)pAclRule->index;

    if ((ret = reg_field_read(APOLLOMP_ACL_CFGr, APOLLOMP_MODEf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }
    mode = (rtk_acl_igr_rule_mode_t)val;

    switch(mode)
    {
        case ACL_IGR_RULE_MODE_0:

            RT_PARAM_CHK((pAclRule->index >= HAL_MAX_NUM_OF_ACL_RULE_ENTRY()), RT_ERR_OUT_OF_RANGE);

            rawRule.mode = APOLLOMP_ACL_MODE_64ENTRIES;
            rawRule.valid = ENABLED;
            break;
        case ACL_IGR_RULE_MODE_1:

            RT_PARAM_CHK((pAclRule->index >= HAL_MAX_NUM_OF_ACL_ACTION()), RT_ERR_OUT_OF_RANGE);

            rawRule.mode = APOLLOMP_ACL_MODE_128ENTRIES;
            rawRule.valid = ENABLED;
            break;
        default:

            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    /*check active ports for acl rule*/
    rawRule.data_bits.active_portmsk = pAclRule->activePorts.bits[0];
	rawRule.care_bits.active_portmsk = 0x7F;
    /*check template index*/
    RT_PARAM_CHK((pAclRule->templateIdx >= HAL_MAX_NUM_OF_ACL_TEMPLATE()), RT_ERR_OUT_OF_RANGE);
    rawRule.data_bits.type =  pAclRule->templateIdx;
    rawRule.care_bits.type =  0x3;

    aclTemplate.index = pAclRule->templateIdx;
    if ((ret = dal_apollomp_acl_template_get(&aclTemplate)) != RT_ERR_OK)
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
        if ((ret = _dal_apollomp_acl_ruleField_add(&rawRule, &aclTemplate, aclField)) != RT_ERR_OK )
        {
            RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
            return ret;
        }

        aclField = aclField->next;
        while(aclField != NULL)
        {
            if ((ret = _dal_apollomp_acl_ruleField_add(&rawRule, &aclTemplate, aclField)) != RT_ERR_OK )
            {
                RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
                return ret;
            }
            aclField = aclField->next;
        }
    }

    if((ret = apollomp_raw_acl_rule_set(&rawRule) != RT_ERR_OK ))
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    if ((ret = dal_apollomp_acl_igrRuleAction_set(pAclRule)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_acl_igrRuleEntry_add */

/* Function Name:
 *      dal_apollomp_acl_igrRuleEntry_del
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
dal_apollomp_acl_igrRuleEntry_del(uint32 index)
{
    int32   ret;
    uint32  val;
    rtk_acl_igr_rule_mode_t mode;
    uint32  actionData[APOLLOMP_ACL_ACTION_DATA_LEN];
    apollomp_raw_acl_ruleEntry_t rawRule;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_ACL),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_acl_init);

    /* parameter check */
    RT_PARAM_CHK((index >= HAL_MAX_NUM_OF_ACL_ACTION()), RT_ERR_OUT_OF_RANGE);


    if((ret = dal_apollomp_acl_igrRuleMode_get(&mode)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
		return ret;
    }

    osal_memset(&rawRule,0x0,sizeof(apollomp_raw_acl_ruleEntry_t));
    rawRule.idx = index;
    rawRule.mode = mode;

    if(mode == ACL_IGR_RULE_MODE_0)
        rawRule.valid = DISABLED;
    else
        rawRule.valid = ENABLED;

    if((ret = apollomp_raw_acl_rule_set(&rawRule)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
		return ret;
    }

    /*clear action*/
	osal_memset(actionData, 0, sizeof(actionData));
    if ((ret = table_write(APOLLOMP_ACL_ACTION_TABLEt, index, actionData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    /*clear action contril bits*/
    /* valid and not = DISABLED, others = ENABLED*/
    val = 0x3F;

    if((ret = reg_array_write(APOLLOMP_ACL_ACTIONr, REG_ARRAY_INDEX_NONE, index, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
		return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_acl_igrRuleEntry_del */

/* Function Name:
 *      dal_apollomp_acl_igrRuleEntry_delAll
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
dal_apollomp_acl_igrRuleEntry_delAll(void)
{
    int32   ret;
    uint32  index;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_ACL),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_acl_init);


    /*clear rule*/
    index = HAL_MAX_NUM_OF_ACL_RULE_ENTRY() - 1;

    if((ret = table_clear(APOLLOMP_ACL_DATAt,0,index)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
		return ret;
    }

    if((ret = table_clear(APOLLOMP_ACL_MASKt,0,index)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
		return ret;
    }

    /*clear action*/
    index = HAL_MAX_NUM_OF_ACL_ACTION() - 1;

    if((ret = table_clear(APOLLOMP_ACL_ACTION_TABLEt,0,index)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
		return ret;
    }

    /*clear action contril bits*/
    /* valid and not = DISABLED, others = ENABLED*/
    val = 0x3F;

    for(index = 0; index < HAL_MAX_NUM_OF_ACL_ACTION(); index ++)
    {
        if((ret = reg_array_write(APOLLOMP_ACL_ACTIONr, REG_ARRAY_INDEX_NONE, index, &val)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
    		return ret;
        }
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_acl_igrRuleEntry_delAll */


/* Function Name:
 *      dal_apollomp_acl_igrUnmatchAction_set
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
dal_apollomp_acl_igrUnmatchAction_set(rtk_port_t port, rtk_filter_unmatch_action_type_t action)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_ACL),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_acl_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);

    switch(action)
    {
        case FILTER_UNMATCH_DROP:
        case FILTER_UNMATCH_PERMIT:
            val = (uint32)action;
        	if ((ret = reg_array_field_write(APOLLOMP_ACL_PERMITr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_PERMITf, &val)) != RT_ERR_OK)
        	{
                RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        		return ret;
        	}
            break;

        default:

            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_acl_igrUnmatchAction_set */

/* Function Name:
 *      dal_apollomp_acl_igrUnmatchAction_get
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
dal_apollomp_acl_igrUnmatchAction_get(rtk_port_t port, rtk_filter_unmatch_action_type_t *pAction)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_ACL),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_acl_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

	if ((ret = reg_array_field_read(APOLLOMP_ACL_PERMITr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_PERMITf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
		return ret;
	}
    *pAction = (rtk_filter_unmatch_action_type_t)val;

    return RT_ERR_OK;
} /* end of dal_apollomp_acl_igrUnmatchAction_get */


/* Function Name:
 *      dal_apollomp_acl_igrState_set
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
dal_apollomp_acl_igrState_set(rtk_port_t port, rtk_enable_t state)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_ACL),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_acl_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTK_ENABLE_END <= state), RT_ERR_INPUT);

    val = (uint32)state;
    if ((ret = reg_array_field_write(APOLLOMP_ACL_ENr, port, REG_ARRAY_INDEX_NONE,APOLLOMP_ENf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_acl_igrState_set */


/* Function Name:
 *      dal_apollomp_acl_igrState_get
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
dal_apollomp_acl_igrState_get(rtk_port_t port, rtk_enable_t *pState)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_ACL),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_acl_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(APOLLOMP_ACL_ENr, port, REG_ARRAY_INDEX_NONE,APOLLOMP_ENf, &val))!=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }
    *pState = (rtk_enable_t)val;

    return RT_ERR_OK;
} /* end of dal_apollomp_acl_igrState_get */


/* Function Name:
 *      dal_apollomp_acl_ipRange_set
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
dal_apollomp_acl_ipRange_set(rtk_acl_rangeCheck_ip_t *pRangeEntry)
{
    int32   ret;
    uint32  index;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_ACL),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_acl_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pRangeEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(pRangeEntry->index >= HAL_MAX_NUM_OF_RANGE_CHECK_IP(), RT_ERR_OUT_OF_RANGE);

    index = (uint32)pRangeEntry->index;

    switch(pRangeEntry->type)
    {
        case IPRANGE_UNUSED:

            val = APOLLOMP_DAL_ACL_IPRANGE_UNUSED;
            break;
        case IPRANGE_IPV4_SIP:

            val = APOLLOMP_DAL_ACL_IPRANGE_IPV4_SIP;
            break;
        case IPRANGE_IPV4_DIP:

            val = APOLLOMP_DAL_ACL_IPRANGE_IPV4_DIP;
            break;
        case IPRANGE_IPV6_SIP:

            val = APOLLOMP_DAL_ACL_IPRANGE_IPV6_SIP;
            break;
        case IPRANGE_IPV6_DIP:

            val = APOLLOMP_DAL_ACL_IPRANGE_IPV6_DIP;
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

	if ((ret = reg_array_field_write(APOLLOMP_RNG_CHK_IP_RNGr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_TYPEf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    val = (uint32)pRangeEntry->upperIp;
	if ((ret = reg_array_field_write(APOLLOMP_RNG_CHK_IP_RNGr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_IP_UPPERf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    val = (uint32)pRangeEntry->lowerIp;
	if((ret = reg_array_field_write(APOLLOMP_RNG_CHK_IP_RNGr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_IP_LOWERf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_acl_ipRange_set */


/* Function Name:
 *      dal_apollomp_acl_ipRange_get
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
dal_apollomp_acl_ipRange_get(rtk_acl_rangeCheck_ip_t *pRangeEntry)
{
    int32   ret;
    uint32  index;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_ACL),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_acl_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pRangeEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(pRangeEntry->index >= HAL_MAX_NUM_OF_RANGE_CHECK_IP(), RT_ERR_OUT_OF_RANGE);

    index = (uint32)pRangeEntry->index;

	if ((ret = reg_array_field_read(APOLLOMP_RNG_CHK_IP_RNGr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_TYPEf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }
    switch(val)
    {
        case APOLLOMP_DAL_ACL_IPRANGE_UNUSED:

            pRangeEntry->type = IPRANGE_UNUSED;
            break;
        case APOLLOMP_DAL_ACL_IPRANGE_IPV4_SIP:

            pRangeEntry->type = IPRANGE_IPV4_SIP;
            break;
        case APOLLOMP_DAL_ACL_IPRANGE_IPV4_DIP:

            pRangeEntry->type = IPRANGE_IPV4_DIP;
            break;
        case APOLLOMP_DAL_ACL_IPRANGE_IPV6_SIP:

            pRangeEntry->type = IPRANGE_IPV6_SIP;
            break;
        case APOLLOMP_DAL_ACL_IPRANGE_IPV6_DIP:

            pRangeEntry->type = IPRANGE_IPV6_DIP;
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

	if ((ret = reg_array_field_read(APOLLOMP_RNG_CHK_IP_RNGr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_IP_UPPERf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }
    pRangeEntry->upperIp = (ipaddr_t)val;

	if((ret = reg_array_field_read(APOLLOMP_RNG_CHK_IP_RNGr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_IP_LOWERf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }
    pRangeEntry->lowerIp = (ipaddr_t)val;

    return RT_ERR_OK;
} /* end of dal_apollomp_acl_ipRange_get */

/* Function Name:
 *      dal_apollomp_acl_vidRange_set
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
dal_apollomp_acl_vidRange_set(rtk_acl_rangeCheck_vid_t *pRangeEntry)
{
    int32   ret;
    uint32  index;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_ACL),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_acl_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pRangeEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(pRangeEntry->index >= HAL_MAX_NUM_OF_RANGE_CHECK_VID(), RT_ERR_OUT_OF_RANGE);

    index = (uint32)pRangeEntry->index;

    switch(pRangeEntry->type)
    {
        case VIDRANGE_UNUSED:

            val = APOLLOMP_DAL_ACL_VIDRANGE_UNUSED;
            break;
        case VIDRANGE_CVID:

            val = APOLLOMP_DAL_ACL_VIDRANGE_CVID;
            break;
        case VIDRANGE_SVID:

            val = APOLLOMP_DAL_ACL_VIDRANGE_SVID;
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if ((ret = reg_array_field_write(APOLLOMP_RNG_CHK_VID_RNGr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_TYPEf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    val = (uint32)pRangeEntry->upperVid;
	if ((ret = reg_array_field_write(APOLLOMP_RNG_CHK_VID_RNGr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_VID_UPPERf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    val = (uint32)pRangeEntry->lowerVid;
	if ((ret = reg_array_field_write(APOLLOMP_RNG_CHK_VID_RNGr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_VID_LOWERf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_acl_vidRange_set */

/* Function Name:
 *      dal_apollomp_acl_vidRange_get
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
dal_apollomp_acl_vidRange_get(rtk_acl_rangeCheck_vid_t *pRangeEntry)
{
    int32   ret;
    uint32  index;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_ACL),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_acl_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pRangeEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(pRangeEntry->index >= HAL_MAX_NUM_OF_RANGE_CHECK_VID(), RT_ERR_OUT_OF_RANGE);

    index = (uint32)pRangeEntry->index;

    if ((ret = reg_array_field_read(APOLLOMP_RNG_CHK_VID_RNGr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_TYPEf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    switch(val)
    {
        case APOLLOMP_DAL_ACL_VIDRANGE_UNUSED:

            pRangeEntry->type = VIDRANGE_UNUSED;
            break;
        case APOLLOMP_DAL_ACL_VIDRANGE_CVID:

            pRangeEntry->type = VIDRANGE_CVID;
            break;
        case APOLLOMP_DAL_ACL_VIDRANGE_SVID:

            pRangeEntry->type = VIDRANGE_SVID;
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

	if ((ret = reg_array_field_read(APOLLOMP_RNG_CHK_VID_RNGr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_VID_UPPERf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }
    pRangeEntry->upperVid = (uint16)val;

	if ((ret = reg_array_field_read(APOLLOMP_RNG_CHK_VID_RNGr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_VID_LOWERf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }
    pRangeEntry->lowerVid = (uint16)val;


    return RT_ERR_OK;
} /* end of dal_apollomp_acl_vidRange_get */


/* Function Name:
 *      dal_apollomp_acl_portRange_set
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
dal_apollomp_acl_portRange_set(rtk_acl_rangeCheck_l4Port_t *pRangeEntry)
{
    int32   ret;
    uint32  index;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_ACL),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_acl_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pRangeEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(pRangeEntry->index >= HAL_MAX_NUM_OF_RANGE_CHECK_L4PORT(), RT_ERR_OUT_OF_RANGE);

    index = (uint32)pRangeEntry->index;

    switch(pRangeEntry->type)
    {
        case PORTRANGE_UNUSED:

            val = APOLLOMP_DAL_ACL_PORTRANGE_UNUSED;
            break;
        case PORTRANGE_SPORT:

            val = APOLLOMP_DAL_ACL_PORTRANGE_SPORT;
            break;
        case PORTRANGE_DPORT:

            val = APOLLOMP_DAL_ACL_PORTRANGE_DPORT;
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

	if ((ret = reg_array_field_write(APOLLOMP_RNG_CHK_L4PORT_RNGr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_TYPEf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    val = (uint32)pRangeEntry->upper_bound;
	if ((ret = reg_array_field_write(APOLLOMP_RNG_CHK_L4PORT_RNGr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_L4PORT_UPPERf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    val = (uint32)pRangeEntry->lower_bound;
	if ((ret = reg_array_field_write(APOLLOMP_RNG_CHK_L4PORT_RNGr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_L4PORT_LOWERf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_acl_portRange_set */


/* Function Name:
 *      dal_apollomp_acl_portRange_get
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
dal_apollomp_acl_portRange_get(rtk_acl_rangeCheck_l4Port_t *pRangeEntry)
{
    int32   ret;
    uint32  index;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_ACL),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_acl_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pRangeEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(pRangeEntry->index >= HAL_MAX_NUM_OF_RANGE_CHECK_L4PORT(), RT_ERR_OUT_OF_RANGE);

    index = (uint32)pRangeEntry->index;

	if ((ret = reg_array_field_read(APOLLOMP_RNG_CHK_L4PORT_RNGr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_TYPEf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    switch(val)
    {
        case APOLLOMP_DAL_ACL_PORTRANGE_UNUSED:

            pRangeEntry->type = PORTRANGE_UNUSED;
            break;
        case APOLLOMP_DAL_ACL_PORTRANGE_SPORT:

            pRangeEntry->type = PORTRANGE_SPORT;
            break;
        case APOLLOMP_DAL_ACL_PORTRANGE_DPORT:

            pRangeEntry->type = PORTRANGE_DPORT;
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

	if ((ret = reg_array_field_read(APOLLOMP_RNG_CHK_L4PORT_RNGr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_L4PORT_UPPERf,  &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }
    pRangeEntry->upper_bound = (uint16)val;

	if((ret = reg_array_field_read(APOLLOMP_RNG_CHK_L4PORT_RNGr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_L4PORT_LOWERf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }
    pRangeEntry->lower_bound = (uint16)val;


    return RT_ERR_OK;
} /* end of dal_apollomp_acl_portRange_get */


/* Function Name:
 *      dal_apollomp_acl_packetLengthRange_set
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
dal_apollomp_acl_packetLengthRange_set(rtk_acl_rangeCheck_pktLength_t *pRangeEntry)
{
    int32   ret;
    uint32  index;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_ACL),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_acl_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pRangeEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pRangeEntry->index >= HAL_MAX_NUM_OF_RANGE_CHECK_PKTLEN()), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((pRangeEntry->type >= PKTLENRANGE_END), RT_ERR_OUT_OF_RANGE);

    index = (uint32)pRangeEntry->index;

    val = (uint32)pRangeEntry->type;
	if ((ret = reg_array_field_write(APOLLOMP_RNG_CHK_PKTLEN_RNGr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_TYPEf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    val = (uint32)pRangeEntry->upper_bound;
	if ((ret = reg_array_field_write(APOLLOMP_RNG_CHK_PKTLEN_RNGr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_PKTLEN_UPPERf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    val = (uint32)pRangeEntry->lower_bound;
	if((ret = reg_array_field_write(APOLLOMP_RNG_CHK_PKTLEN_RNGr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_PKTLEN_LOWERf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_acl_packetLengthRange_set */


/* Function Name:
 *      dal_apollomp_acl_packetLengthRange_get
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
dal_apollomp_acl_packetLengthRange_get(rtk_acl_rangeCheck_pktLength_t *pRangeEntry)
{
    int32   ret;
    uint32  index;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_ACL),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_acl_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pRangeEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pRangeEntry->index >= HAL_MAX_NUM_OF_RANGE_CHECK_PKTLEN()), RT_ERR_OUT_OF_RANGE);

    index = (uint32)pRangeEntry->index;

	if ((ret = reg_array_field_read(APOLLOMP_RNG_CHK_PKTLEN_RNGr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_TYPEf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }
    pRangeEntry->type = (rtk_acl_pktlenrange_type_t)val;

	if ((ret = reg_array_field_read(APOLLOMP_RNG_CHK_PKTLEN_RNGr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_PKTLEN_UPPERf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }
    pRangeEntry->upper_bound = (uint16)val;

	if((ret = reg_array_field_read(APOLLOMP_RNG_CHK_PKTLEN_RNGr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_PKTLEN_LOWERf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }
    pRangeEntry->lower_bound = (uint16)val;

    return RT_ERR_OK;
} /* end of dal_apollomp_acl_packetLengthRange_get */


/* Function Name:
 *      dal_apollomp_acl_igrRuleMode_set
 * Description:
 *      Set ingress ACL rule mode
 * Input:
 *      mode - ingress ACL rule mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_INPUT           - Input error
 * Note:
 *          - ACL_IGR_RULE_MODE_0, 64  rules, the size each rule is 16x8 bits
 *          - ACL_IGR_RULE_MODE_1, 128 rules,
 *          -               the size each rule is 16x4 bits(entry 0~63)
 *          -               the size each rule is 16x3 bits(entry 64~127)
 */
int32
dal_apollomp_acl_igrRuleMode_set(rtk_acl_igr_rule_mode_t mode)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_ACL),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_acl_init);

    switch(mode)
    {
        case ACL_IGR_RULE_MODE_0:
        case ACL_IGR_RULE_MODE_1:

            val = (uint32)mode;
            if ((ret = reg_field_write(APOLLOMP_ACL_CFGr, APOLLOMP_MODEf, &val)) != RT_ERR_OK )
            {
                RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
                return ret;
            }
            break;
       default:

        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_acl_igrRuleMode_set */



/* Function Name:
 *      dal_apollomp_acl_igrRuleMode_get
 * Description:
 *      Get ingress ACL rule mode
 * Input:
 *      None
 * Output:
 *      pMode - ingress ACL rule mode
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_INPUT           - Input error
 * Note:
 *          - ACL_IGR_RULE_MODE_0, 64  rules, the size each rule is 16x8 bits
 *          - ACL_IGR_RULE_MODE_1, 128 rules,
 *          -               the size of each rule is 16x4 bits(entry 0~63)
 *          -               the size of each rule is 16x3 bits(entry 64~127)
 *          Mode chaged all template/rule will be cleared
 */
int32
dal_apollomp_acl_igrRuleMode_get(rtk_acl_igr_rule_mode_t *pMode)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_ACL),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_acl_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pMode), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_ACL_CFGr, APOLLOMP_MODEf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        return ret;
    }

    *pMode = (rtk_acl_igr_rule_mode_t)val;

    return RT_ERR_OK;
} /* end of dal_apollomp_acl_igrRuleMode_get */

/* Function Name:
 *      dal_apollomp_acl_igrPermitState_set
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
dal_apollomp_acl_igrPermitState_set(rtk_port_t port, rtk_enable_t state)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_ACL),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_acl_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTK_ENABLE_END <= state), RT_ERR_INPUT);

    val = (uint32)state;
	if ((ret = reg_array_field_write(APOLLOMP_ACL_PERMITr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_PERMITf, &val)) != RT_ERR_OK)
	{
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
		return ret;
	}

    return RT_ERR_OK;
} /* end of dal_apollomp_acl_igrPermitState_set */


/* Function Name:
 *      dal_apollomp_acl_igrPermitState_get
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
dal_apollomp_acl_igrPermitState_get(rtk_port_t port, rtk_enable_t *pState)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_ACL),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_acl_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

	if ((ret = reg_array_field_read(APOLLOMP_ACL_PERMITr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_PERMITf, &val)) != RT_ERR_OK)
	{
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
		return ret;
	}

    *pState = (rtk_enable_t)val;

    return RT_ERR_OK;
} /* end of dal_apollomp_acl_igrPermitState_get */

/* Function Name:
 *      dal_apollomp_acl_dbgHitReason_get
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
int32 dal_apollomp_acl_dbgHitReason_get(rtk_acl_debug_reason_t *pDbgReason)
{
    int32   ret;
	rtk_acl_igr_act_type_t 	type;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_ACL),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_acl_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pDbgReason), RT_ERR_NULL_POINTER);

	for(type = ACL_IGR_CVLAN_ACT; type < ACL_IGR_ACT_END; type++)
	{
		pDbgReason->hitAct[type] = DISABLED;
		pDbgReason->index[type] = 0;
	}

	if ((ret = reg_array_field_read(APOLLOMP_STAT_ACL_REASONr, REG_ARRAY_INDEX_NONE, 0, APOLLOMP_ACL_HIT_INFOf, &val)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
		return ret;
	}

	if(val & 0x80)
	{
		pDbgReason->hitAct[ACL_IGR_CVLAN_ACT] = ENABLED;
		pDbgReason->index[ACL_IGR_CVLAN_ACT] = val &0x7F;	
	}

	if ((ret = reg_array_field_read(APOLLOMP_STAT_ACL_REASONr, REG_ARRAY_INDEX_NONE, 1, APOLLOMP_ACL_HIT_INFOf, &val)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
		return ret;
	}

	if(val & 0x80)
	{
		pDbgReason->hitAct[ACL_IGR_SVLAN_ACT] = ENABLED;
		pDbgReason->index[ACL_IGR_SVLAN_ACT] = val &0x7F;	
	}

	if ((ret = reg_array_field_read(APOLLOMP_STAT_ACL_REASONr, REG_ARRAY_INDEX_NONE, 2, APOLLOMP_ACL_HIT_INFOf, &val)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
		return ret;
	}

	if(val & 0x80)
	{
		pDbgReason->hitAct[ACL_IGR_PRI_ACT] = ENABLED;
		pDbgReason->index[ACL_IGR_PRI_ACT] = val &0x7F;	
	}

	if ((ret = reg_array_field_read(APOLLOMP_STAT_ACL_REASONr, REG_ARRAY_INDEX_NONE, 3, APOLLOMP_ACL_HIT_INFOf, &val)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
		return ret;
	}

	if(val & 0x80)
	{
		pDbgReason->hitAct[ACL_IGR_LOG_ACT] = ENABLED;
		pDbgReason->index[ACL_IGR_LOG_ACT] = val &0x7F;	
	}

	if ((ret = reg_array_field_read(APOLLOMP_STAT_ACL_REASONr, REG_ARRAY_INDEX_NONE, 5, APOLLOMP_ACL_HIT_INFOf, &val)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
		return ret;
	}

	if(val & 0x80)
	{
		pDbgReason->hitAct[ACL_IGR_FORWARD_ACT] = ENABLED;
		pDbgReason->index[ACL_IGR_FORWARD_ACT] = val &0x7F;	
	}
	
	if ((ret = reg_array_field_read(APOLLOMP_STAT_ACL_REASONr, REG_ARRAY_INDEX_NONE, 4, APOLLOMP_ACL_HIT_INFOf, &val)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
		return ret;
	}

	if(val & 0x80)
	{
		pDbgReason->hitAct[ACL_IGR_INTR_ACT] = ENABLED;
		pDbgReason->index[ACL_IGR_INTR_ACT] = val &0x7F;	
	}

    return RT_ERR_OK;
}   /* end of dal_apollomp_acl_dbgHitReason_get */




