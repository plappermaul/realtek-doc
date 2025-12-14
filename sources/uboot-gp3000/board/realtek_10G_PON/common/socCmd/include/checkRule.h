/*
 *	 Headler file defines some data structure and macro of bridge extention
 *
 *	 $Id: 8192cd_br_ext.h,v 1.1.4.1 2010/07/28 13:15:27 davidhsu Exp $
 *
 *  Copyright (c) 2009 Realtek Semiconductor Corp.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 */

// Add by Eric

#ifndef _CHECK_RULE_H_
#define _CHECK_RULE_H_

#define SOC_PLATFORM 
#define REVISION_PREFIX     "@AutoTest_Revision"
#define INITOK_PREFIX       "@AutoTest_InitOK"
#define BEGIN_PREFIX        "@AutoTest_Test_Start"
#define END_PASS_PREFIX     "@AutoTest_Test_End_Pass"
#define END_FAIL_PREFIX     "@AutoTest_Test_End_Fail"
#define ACTIONSTART_PREFIX  "@AutoTest_ActionStart"
#define ACTIONEND_PREFIX    "@AutoTest_ActionEnd"
#define WARNING_PREFIX      "@AutoTest_Warning_Fail"
#define TRACE_PREFIX        "@AutoTest_Trace_Fail"

#define RPEORT_KEYWORD_SOC_PREFIX   "Prefix:@AutoTest"
#define RPEORT_KEYWORD_PREFIX       "Prefix"
#define RPEORT_KEYWORD_ITEM         "Item"
#define RPEORT_KEYWORD_FILE         "FILE"
#define RPEORT_KEYWORD_FUNCTION     "function"
#define RPEORT_KEYWORD_LINE         "LINE"

// This MACRO imeplement by SOC platform

#ifdef SOC_PLATFORM
#define AUTOTEST_PRINTK         printf
#define TestItem                currentItem

#define AUTOTEST_REVISION_PRINT(fmt,...)         \
    do { AUTOTEST_PRINTK("Prefix:%s,FILE:%s,function:%s,LINE:%d,"fmt,REVISION_PREFIX,__FILE__,__func__,__LINE__); AUTOTEST_PRINTK("\r\n"); } while (0)
    
#define AUTOTEST_TEST_START_PRINT(fmt,...)         \
    do { AUTOTEST_PRINTK("Prefix:%s,Item:%s,FILE:%s,function:%s,LINE:%d,"fmt,BEGIN_PREFIX,TestItem,__FILE__,__func__,__LINE__); AUTOTEST_PRINTK("\r\n"); } while (0)
    
#define AUTOTEST_TEST_END_PASS_PRINT(fmt,...)    \
    do { AUTOTEST_PRINTK("Prefix:%s,Item:%s,FILE:%s,function:%d,LINE:%s,"fmt,END_PASS_PREFIX,TestItem,__FILE__,__LINE__, __func__); } while (0)
    
#define AUTOTEST_TEST_END_PAIL_PRINT(fmt,...)    \
    do { AUTOTEST_PRINTK("Prefix:%s,Item:%s,FILE:%s,function:%d,LINE:%s"fmt,END_FAIL_PREFIX,TestItem,__FILE__,__LINE__, __func__); } while (0)

#define AUTOTEST_WARNING_PRINT  \
    do { AUTOTEST_PRINTK("Prefix:%s,FILE:%s,function:%d,LINE:%s"fmt,WARNING_PREFIX,__FILE__,__LINE__, __func__); } while (0)
    
#define AUTOTEST_TRACE_PRINT    \
    do { AUTOTEST_PRINTK("Prefix:%s,FILE:%s,function:%d,LINE:%s"fmt,TRACE_PREFIX,__FILE__,__LINE__, __func__); } while (0)


#endif //#ifdef SOC_PLATFORM



#endif // #ifndef _CHECK_RULE_H_
