#ifndef _APOLLOMP_RAW_ACL_H_
#define _APOLLOMP_RAW_ACL_H_

/****************************************************************/
/* Header File inclusion                                        */
/****************************************************************/
#include <dal/apollomp/raw/apollomp_raw.h>

/****************************************************************/
/* Constant Definition                                          */
/****************************************************************/
#define APOLLOMP_ACLRULETBLEN				    5


/****************************************************************/
/* Type Definition                                              */
/****************************************************************/
typedef enum apollomp_raw_acl_modeTypes_e
{
    APOLLOMP_ACL_MODE_64ENTRIES = 0,
    APOLLOMP_ACL_MODE_128ENTRIES,
    APOLLOMP_ACL_MODE_END
}apollomp_raw_acl_modeTypes_t;


typedef struct apollomp_raw_acl_rule_s
{
	uint32 active_portmsk;
	uint32 type;
	uint32 tag_exist;
	uint32 field[APOLLOMP_MAX_NUM_OF_ACL_RULE_FIELD];
    
}apollomp_raw_acl_rule_t;

typedef struct apollomp_raw_acl_ruleEntry_s
{
    uint32 idx;
    apollomp_raw_acl_modeTypes_t mode;
    
    apollomp_raw_acl_rule_t data_bits;
	uint32 valid;
    apollomp_raw_acl_rule_t care_bits;

}apollomp_raw_acl_ruleEntry_t;




extern int32 apollomp_raw_acl_rule_set(apollomp_raw_acl_ruleEntry_t *pAclRule);
extern int32 apollomp_raw_acl_rule_get(apollomp_raw_acl_ruleEntry_t *pAclRule);


#endif /*_APOLLOMP_RAW_ACL_H_*/


