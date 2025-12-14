#include "rtk_crypto_base_common.h"
#include "rtk_crypto_base.h"
#include "rtk_crypto_base_hal.h"
#include "rtk_crypto_base_type.h"
#include "rtk_crypto_base_ctrl.h"

#include <cpu_func.h>

/**
 * Define Crypto HAL APIs for one thread access.
 * If users want to access Crypto HAL APIs in multi-threads,
 * then need to build a critical section mechanism in Application layer.
 */
#define CRYPTO_THREAD_MODE 0

#if CRYPTO_THREAD_MODE
#include "cmsis_os.h"
#endif

volatile int crypto_done;
volatile int crc_done;

#if defined(CONFIG_BUILD_NONSECURE) && (CONFIG_BUILD_NONSECURE==1)
hal_crypto_adapter_t g_rtl_cryptoEngine_ns;
extern hal_crypto_func_stubs_t hal_crypto_stubs_ns;
#define HAL_CRYPTO_ADAPTER      (g_rtl_cryptoEngine_ns)
#define HAL_CRYPTO_FUNC_STUBS   (hal_crypto_stubs_ns)
#else
hal_crypto_adapter_t g_rtl_cryptoEngine_s;//input data, outdata, reselut ...
extern hal_crypto_func_stubs_t hal_crypto_stubs_s;
#define HAL_CRYPTO_ADAPTER      (g_rtl_cryptoEngine_s)
#define HAL_CRYPTO_FUNC_STUBS   (hal_crypto_stubs_s)
#endif

/*
 * 32bytes-aligned buffer that store mix mode hash padding initial values.
 */
const uint8_t mix_mode_hash_padding[64] __attribute__((aligned(64))) =
{
	0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

/*
 * 32bytes-aligned buffer that store mix mode hash padding values.
 */
volatile uint8_t hash_padding[96] __attribute__((aligned(64))) = {0x0}; //Actual usage maximum size is (120 - 56)+ 8 = 72 bytes

/*
 * 32bytes-aligned buffer that store mix mode encrypt padding values.
 */
volatile uint8_t enc_padding[32] __attribute__((aligned(64))) = {0x0};

//
void hal_crypto_irq_enable(hal_crypto_adapter_t *pcrypto_adapter, void *handler);
void hal_crypto_irq_disable(hal_crypto_adapter_t *pcrypto_adapter);

/** @} */ /* End of group hs_hal_crypto */

//
#if CRYPTO_THREAD_MODE

#define CRYPTO_TIMEOUT 1000

osMutexDef(crypto_mutex);
static osMutexId crypto_mutex_id;
static int critical_init(void)
{
    crypto_mutex_id = osMutexCreate( osMutex(crypto_mutex) );
    if (crypto_mutex_id == NULL) {
        printf("%s : mutex id is NULL \r\n", __func__);
        return -1;
    }

    return SUCCESS;
}

static int critical_enter(void)
{
    osStatus status;

    if (crypto_mutex_id == NULL) {
        printf("%s : mutex id is NULL \r\n", __func__);
        return -1;
    }

    status = osMutexWait(crypto_mutex_id, CRYPTO_TIMEOUT);
    if (status != osOK) {
        printf("hal_crypto: %s : mutex wait failed, status=0x%x \r\n",__func__, status);
        return -1;
    }
    return SUCCESS;
}


static int critical_leave(void)
{
    osStatus status;

    if (crypto_mutex_id == NULL) {
        printf("%s : mutex id is NULL \r\n", __func__);
        return -1;
    }

    status = osMutexRelease(crypto_mutex_id);
    if (status != osOK) {
        printf("%s : mutex release failed, status=0x%x \r\n",__func__, status);
        return -1;
    }

    return SUCCESS;
}

static int critical_deinit(void)
{
    osStatus status;

    if (crypto_mutex_id == NULL) {
        printf("%s : mutex id is NULL \r\n", __func__);
        return -1;
    }

    if (crypto_mutex_id != NULL) {
        status = osMutexDelete( osMutex(crypto_mutex) );

        if (status != osOK) {
            printf("%s : mutex delete fail \r\n", __func__);
            return -1;
        }
    }
    return SUCCESS;
}

#endif


#if CRYPTO_THREAD_MODE

#define MAX_THREADS 8
#define CRYPTO_SIGNAL 0x0100

static osThreadId g_thread_id[MAX_THREADS];
static int g_thread_list_head = 0;
static int g_thread_list_tail = 0;
static osThreadId g_crc_thread_id;


static osThreadId thread_list_get(void)
{
    osThreadId id;

    if (g_thread_list_head == g_thread_list_tail) {
        return NULL;
    }

    id = g_thread_id[g_thread_list_head];
    g_thread_list_head = (g_thread_list_head + 1) % MAX_THREADS;

    return id;
}

static int thread_list_add(osThreadId id)
{
    int next = (g_thread_list_tail + 1) % MAX_THREADS;

    if (next == g_thread_list_head) {
        printf("%s : FULL \r\n", __func__);
        return -1;
    }
    g_thread_id[g_thread_list_tail] = id;
    g_thread_list_tail = next;

    return 0;
}

#endif

/**
 * @addtogroup hs_hal_crypto CRYPTO
 * @{
 */
#if defined(CONFIG_BUILD_NONSECURE) && (CONFIG_BUILD_NONSECURE==1)
#undef CRYPTO_MODULE
#define CRYPTO_MODULE (CRYPTO_NS_MODULE)

#undef CRYPTO_REG_BASE
#define CRYPTO_REG_BASE (CRYPTO_NS_BASE)

#else
#undef CRYPTO_MODULE
#define CRYPTO_MODULE (CRYPTO_S_MODULE)

#undef CRYPTO_REG_BASE
#define CRYPTO_REG_BASE (CRYPTO_S_BASE)
#endif

#define ACCESS_EXTERNAL_MEM                 1

#if defined(CONFIG_BUILD_NONSECURE) && (CONFIG_BUILD_NONSECURE==1)

#define INITMAP_REG_NS_HAL_CALL		        (1 << 0)
#define INITMAP_DEREG_NS_HAL_CALL		    (0 << 0)

#else
#define INITMAP_REG_S_HAL_CALL		        (1 << 0)
#define INITMAP_DEREG_S_HAL_CALL		    (0 << 0)
#define INITMAP_REG_S_PLATFORM		        (1 << 1)
#define INITMAP_REG_NS_PLATFORM		        (1 << 2)
#define INITMAP_DEREG_S_PLATFORM            0x5
#define INITMAP_DEREG_NS_PLATFORM           0x3

#define INITMAP_S_REG		                0x2
#define INITMAP_NS_REG		                0x4
#endif

#define HASH_PADDING_PUT_UINT32_LE(n,b,i)                       \
{                                                               \
    (b)[(i)    ] = (unsigned char) ( ( (n)       ) & 0xFF );   \
    (b)[(i) + 1] = (unsigned char) ( ( (n) >>  8 ) & 0xFF );   \
    (b)[(i) + 2] = (unsigned char) ( ( (n) >> 16 ) & 0xFF );   \
    (b)[(i) + 3] = (unsigned char) ( ( (n) >> 24 ) & 0xFF );   \
}

#define HASH_PADDING_PUT_UINT32_BE(n,b,i)               \
{                                                       \
    (b)[(i)    ] = (unsigned char) ( (n) >> 24 );       \
    (b)[(i) + 1] = (unsigned char) ( (n) >> 16 );       \
    (b)[(i) + 2] = (unsigned char) ( (n) >>  8 );       \
    (b)[(i) + 3] = (unsigned char) ( (n)       );       \
}

/**
 *  @brief To notify a thread that crypto engine or CRC engine has already calculated a result.
 *  @param[in]  ok_int_cnt  counter value of crypto engine calculated a result interrupt
 *  @param[in]  isCrc       flag value of CRC engine calculated a result interrupt
 *  @return     void
 */
void g_crypto_handler(int ok_int_cnt, int isCrc)
{
    //printf("%s : ok_int_cnt = %d , isCrc=%d\r\n",
    //		__FUNCTION__, ok_int_cnt, isCrc);

    printf("g_crypto_handler: ok_int_cnt = %d\r\n", ok_int_cnt);

#if CRYPTO_THREAD_MODE
    osThreadId id;
    int i, loops;

    loops = ok_int_cnt;

    for (i = 0; i < loops; i++) {
        id = thread_list_get();
        if (id != NULL) {
            osSignalSet(id, CRYPTO_SIGNAL);
        } else {
            printf("%s: list get NULL \r\n", __func__);
        }
    }
#else
    if (ok_int_cnt > 0) {
        crypto_done = 1;
    }
#endif

    if (isCrc) {
#if CRYPTO_THREAD_MODE
        osSignalSet(g_crc_thread_id, CRYPTO_SIGNAL);
#else
        crc_done = 1;
#endif
    }
}

/**
 *  @brief To initialize the notified flag of crypto engine.
 *  @param[in]  pIE1  Crypto adapter object
 *  @return     value == 0     success
 *  @return     value < 0      fail
 */
int g_crypto_pre_exec(void *pIE1)
{
    (void)(pIE1);
#if CRYPTO_THREAD_MODE
    {
        int32_t signal;
        osThreadId curID = osThreadGetId();
        int ret1;

        signal = osSignalClear(curID, CRYPTO_SIGNAL);
        if (signal != osOK) {
            printf("osSignalClear failed, signal= 0x%x \r\n",signal);
            return -1;
        }
        ret1 = thread_list_add(curID);
        if (ret1 != 0) {
            printf("%s : thread_list_add FULL \r\n", __func__);
            return -1;
        }
    }
#else
    crypto_done = 0;
#endif

    return SUCCESS;
}

/**
 *  @brief To wait to be notified that the crypto engine calculates a result. If crypto calculating result
 *         interrupt arrives, it will trigger interrupt service routine which notifies this information to a thread.
 *  @param[in]  pIE1  Crypto adapter object
 *  @return     value == 0     success
 *  @return     value < 0      fail
 */
int g_crypto_wait_done(void *pIE1)
{
    uint32_t loopWait;
    volatile uint32_t ips_err;
    volatile uint32_t ips_10_status;
    hal_crypto_adapter_t *pcrypto_adapter = (hal_crypto_adapter_t *)pIE1;
    (void)(pcrypto_adapter);
    loopWait = 1000000; /* hope long enough */
#if CRYPTO_THREAD_MODE
    {
        osEvent evt;

        evt = osSignalWait(CRYPTO_SIGNAL, 100); // wait for 100 ms

        if ((evt.status != osOK) && (evt.status != osEventSignal)) {
            printf("osSignalWait failed, evt.status = 0x%x \r\n", evt.status);
            return -1;
        }
    }
#else
    if (pcrypto_adapter->isIntMode) {
        //while ( crypto_done == 0 );
        // TODO: can add some Delay / and loops limitation
        while (crypto_done == 0) {
            loopWait--;
            if (loopWait == 0) {
                ips_10_status = (CRYPTO_MODULE->ipscsr_reset_isr_conf_reg);
                ips_err = (CRYPTO_MODULE->ipscsr_err_stats_reg);
                DBG_CRYPTO_ERR("Crypto interrupt doesn't arrive.\r\n");
                DBG_CRYPTO_ERR("Wait Timeout ips status = 0x%08x, ips err = 0x%08x\r\n", ips_10_status, ips_err);
                return FAIL; /* error occurs */
            }
        }
    }
#endif
    // if not interrupt mode, suggest to use default one
    return SUCCESS;
}

#if defined(CONFIG_BUILD_SECURE)
/**
 *  @brief      Enable/Disable the clock of IPsec in RAM code.
 *  @param[in]  en  set the clock of IPsec state: 1=Enable, 0=Disable 
 *  @return     void
 */
void hal_crypto_engine_init_platform_en_ctrl(const int en){

    hal_crypto_adapter_t *pcrypto_adapter = &g_rtl_cryptoEngine_s;
    DBG_CRYPTO_INFO("en = %d,initmap = 0x%x\r\n", en, pcrypto_adapter->initmap);
    if (en) {
        if (!((pcrypto_adapter->initmap) & INITMAP_S_REG)) {
           hal_crypto_stubs_s.hal_crypto_engine_platform_en_ctrl(1);
        }
        (pcrypto_adapter->initmap) |= INITMAP_REG_NS_PLATFORM; //register non_s init
    } else {
        if (!((pcrypto_adapter->initmap) & INITMAP_S_REG)) {
           hal_crypto_stubs_s.hal_crypto_engine_platform_en_ctrl(0);
        }
        (pcrypto_adapter->initmap) &= INITMAP_DEREG_NS_PLATFORM; //deregister non_s init
    }
    DBG_CRYPTO_INFO("inimap = 0x%x\r\n", pcrypto_adapter->initmap);
}

SECTION_NS_ENTRY_FUNC void NS_ENTRY hal_crypto_engine_init_platform_nsc(const int en)
{
    hal_crypto_engine_init_platform_en_ctrl(en);
}
#endif

/* Enable EL2 PMU CNT */
static inline void armv8pmu_pmccfiltr_write(u32 val)
{
	asm volatile("msr pmccfiltr_el0, %0" :: "r" (val));
}
static inline u32 armv8pmu_pmccfiltr_read(void)
{
	u32 val;
	asm volatile("mrs %0, pmccfiltr_el0" : "=r" (val));
	return val;
}

int hal_crypto_engine_init(void)
{
    int ret = SUCCESS;
    hal_crypto_adapter_t *pcrypto_adapter = &HAL_CRYPTO_ADAPTER;

    if (pcrypto_adapter->isInit) {
        DBG_CRYPTO_WARN("Crypto engine has initialized!\r\n");
        return SUCCESS;
    }

    /* EN PMU EL2 cyccle count  */
    armv8pmu_pmccfiltr_write(armv8pmu_pmccfiltr_read()| (1<<27) );
    /*  Allow Cotex to access Secure Crypto Addr */
	/* PE_CTRL_CRYPTO_PROT */
#if defined(CONFIG_TARGET_RTK_ELNATH)
    writel(0x10, 0x9030305c);
#else
    writel(0x10, 0x9030304c);
#endif
    //For enable/disable __dbg_mem_dump()
    pcrypto_adapter->isMemDump = 0;

    pcrypto_adapter->pre_exec_func = g_crypto_pre_exec;
    pcrypto_adapter->wait_done_func = NULL; // g_crypto_wait_done for ISR mode, NULL for non-ISR mode
#if defined(CONFIG_BUILD_NONSECURE) && (CONFIG_BUILD_NONSECURE==1)

    hal_crypto_engine_init_platform_nsc(1);
    (pcrypto_adapter->initmap) |= INITMAP_REG_NS_HAL_CALL;

#else
    DBG_CRYPTO_INFO("hal_s_crypto_adapter->initmap = 0x%x\r\n",pcrypto_adapter->initmap);
    if (!((pcrypto_adapter->initmap) & INITMAP_NS_REG)) {
        hal_crypto_stubs_s.hal_crypto_engine_platform_en_ctrl(1);
    }
    (pcrypto_adapter->initmap) |= INITMAP_REG_S_PLATFORM;   //register s init
    (pcrypto_adapter->initmap) |= INITMAP_REG_S_HAL_CALL;   //Notify init s from hal   
    DBG_CRYPTO_INFO("hal_s_crypto_adapter->initmap = 0x%x\r\n",pcrypto_adapter->initmap);
#endif

    ret = (HAL_CRYPTO_FUNC_STUBS.hal_crypto_engine_init)(pcrypto_adapter);
    if (ret != SUCCESS) {
        return ret;
    }
#if ACCESS_EXTERNAL_MEM
    // YR pcrypto_adapter->arch_clean_dcache_by_size = (void (*)(uint32_t , int32_t ))hal_cache_stubs.dcache_clean_by_addr;
    // YR pcrypto_adapter->arch_invalidate_dcache_by_size = (void (*)(uint32_t , int32_t ))hal_cache_stubs.dcache_invalidate_by_addr;
    pcrypto_adapter->arch_clean_dcache_by_size = (void (*)(uint32_t , int32_t ))flush_dcache_range;
    pcrypto_adapter->arch_invalidate_dcache_by_size = (void (*)(uint32_t , int32_t ))invalidate_dcache_range;
#else
    pcrypto_adapter->arch_clean_dcache_by_size = NULL;
    pcrypto_adapter->arch_invalidate_dcache_by_size = NULL;
#endif

#if CRYPTO_THREAD_MODE
    critical_init();
#endif
    //hal_crypto_irq_enable(pcrypto_adapter, (void *)g_crypto_handler);
    return ret;
}

int hal_crypto_engine_deinit(void)
{
    int ret = SUCCESS;
    hal_crypto_adapter_t *pcrypto_adapter = &HAL_CRYPTO_ADAPTER;

    if (pcrypto_adapter->isInit == 0) {
        DBG_CRYPTO_WARN("Crypto engine doesn't initialize!\r\n");
        return SUCCESS;
    }

    pcrypto_adapter->pre_exec_func = NULL;
    pcrypto_adapter->wait_done_func = NULL;
    pcrypto_adapter->arch_clean_dcache_by_size = NULL;
    pcrypto_adapter->arch_invalidate_dcache_by_size = NULL;
    hal_crypto_irq_disable(pcrypto_adapter);
    ret = (HAL_CRYPTO_FUNC_STUBS.hal_crypto_engine_deinit)(pcrypto_adapter);
    if (ret != SUCCESS) {
        return ret;
    }
#if defined(CONFIG_BUILD_NONSECURE) && (CONFIG_BUILD_NONSECURE==1)
    (pcrypto_adapter->initmap) &= INITMAP_DEREG_NS_HAL_CALL;
    hal_crypto_engine_init_platform_nsc(0);
#else
    DBG_CRYPTO_INFO("hal_s_crypto_adapter->initmap = 0x%x\r\n",pcrypto_adapter->initmap);
    if (!((pcrypto_adapter->initmap) & INITMAP_NS_REG)) {
        hal_crypto_stubs_s.hal_crypto_engine_platform_en_ctrl(0);
    }
    (pcrypto_adapter->initmap) &= INITMAP_DEREG_S_PLATFORM; //deregister s init
    (pcrypto_adapter->initmap) &= INITMAP_DEREG_S_HAL_CALL;
    DBG_CRYPTO_INFO("hal_s_crypto_adapter->initmap = 0x%x\r\n",pcrypto_adapter->initmap);
#endif

#if CRYPTO_THREAD_MODE
    critical_deinit();
#endif
    return ret;
}


//
// IRQ
//
/**
 *  @brief The CRYPTO interrupt service routine function pointer type.
 */
typedef void (*crypto_isr_t)(int, int);

//
crypto_isr_t irq_handle;

/**
 *  @brief The CRYPTO interrupt handler. It processes interrupt events which are crypto engine calculated a result or
 *         CRC engine calculated a result.
 *  @return     void
 */
void crypto_handler(void)
{
    uint32_t ok_int_cnt;

    //DBG_CRYPTO_INFO("crypto_handler\n");
    ok_int_cnt = (CRYPTO_MODULE->ipscsr_reset_isr_conf_reg_b.ok_intr_cnt);
    //DBG_CRYPTO_INFO("ok_int_cnt = %d\n", ok_int_cnt);
    if (ok_int_cnt > 0) {
        (CRYPTO_MODULE->ipscsr_reset_isr_conf_reg_b.clear_ok_intr_num) = ok_int_cnt;
        (CRYPTO_MODULE->ipscsr_reset_isr_conf_reg_b.cmd_ok) = 1;
    }

    crypto_done = 1;
#if 0
    crypto_isr_t handler = (crypto_isr_t)irq_handle;

    //uint32_t ips_10_status, ips_err;
    volatile uint32_t ok_int_cnt;
    //volatile uint32_t val;

    ok_int_cnt = (CRYPTO_MODULE->ipscsr_reset_isr_conf_reg_b.ok_intr_cnt);
    if (ok_int_cnt > 0) {
        (CRYPTO_MODULE->ipscsr_reset_isr_conf_reg_b.clear_ok_intr_num) = ok_int_cnt;
        (CRYPTO_MODULE->ipscsr_reset_isr_conf_reg_b.cmd_ok) = 1;
    }
    //val = (CRYPTO_MODULE->ipscsr_reset_isr_conf_reg).w;
    //ips_10_status = (CRYPTO_MODULE->ipscsr_reset_isr_conf_reg).w;
    //ips_err = (CRYPTO_MODULE->ipscsr_err_stats_reg).w;
    //if ( ips_err != 0 )
    //    printf("- ips status =0x%08x, ips err = 0x%08x\r\n", ips_10_status, ips_err);

    //printf("ok_int_cnt : %d \r\n", ok_int_cnt);

    // for CRC
    {
        int isCrc = 0;        
#if defined(CONFIG_BUILD_NONSECURE) && (CONFIG_BUILD_NONSECURE==1)
        volatile uint32_t val;
        //Only Non-secure support crc engine
        val = (CRYPTO_MODULE->crc_stat_reg_b.crc_ok);

        if (val) {
            (CRYPTO_MODULE->crc_stat_reg_b.crc_ok) = 1;
            isCrc = 1;
        }
#endif
        handler(ok_int_cnt, isCrc);
    }

	// To make sure the cmd_ok has been written
    // YR __DSB();
#endif
}

/**
 *  @brief To register a interrupt handler for the crypto engine and initialize the related crypto interrupt settings.
 *  @param[in]  pcrypto_adapter   Crypto adapter object
 *  @param[in]  handler Interrupt service routine function
 *  @return     void
 */
void hal_crypto_irq_enable(hal_crypto_adapter_t *pcrypto_adapter, void *handler)
{
    IRQ_HANDLE pIrqHandle;

    // Register IRQ
    pIrqHandle.Data = (u32)NULL;
    pIrqHandle.IrqNum = CRYPTO_IRQ;
    pIrqHandle.IrqFun = (IRQ_FUN)crypto_handler;
    pIrqHandle.Priority = 0;
    (CRYPTO_MODULE->ipscsr_reset_isr_conf_reg_b.intr_mode) = 1;
    (CRYPTO_MODULE->ipscsr_int_mask_reg_b.cmd_ok_m) = 0;
    InterruptRegister(&pIrqHandle);
    //InterruptEn(&pIrqHandle);

#if 0 // YR TODO
    irq_handle = (crypto_isr_t)handler;
#if defined(CONFIG_BUILD_NONSECURE) && (CONFIG_BUILD_NONSECURE==1)
    hal_irq_set_vector(Crypto_IRQn, (uint32_t)crypto_handler);
    hal_irq_clear_pending(Crypto_IRQn);
    hal_irq_set_priority(Crypto_IRQn, Crypto_IRQPri);

    (CRYPTO_MODULE->ipscsr_reset_isr_conf_reg_b.intr_mode) = 1;
    (CRYPTO_MODULE->ipscsr_int_mask_reg_b.cmd_ok_m) = 0;
    hal_irq_enable(Crypto_IRQn);
#else
    hal_irq_set_vector(SCrypto_IRQn, (uint32_t)crypto_handler);
    hal_irq_clear_pending(SCrypto_IRQn);
    hal_irq_set_priority(SCrypto_IRQn, SCrypto_IRQPri);

    (CRYPTO_MODULE->ipscsr_reset_isr_conf_reg_b.intr_mode) = 1;
    (CRYPTO_MODULE->ipscsr_int_mask_reg_b.cmd_ok_m) = 0;
    hal_irq_enable(SCrypto_IRQn);
#endif
#endif
    pcrypto_adapter->isIntMode = 1;
}

/**
 *  @brief To un-register a interrupt handler for the crypto engine and enable interrupt mask,
 *         then inactive interrupt mode state.
 *  @param[in]  pcrypto_adapter  Crypto adapter object
 *  @return     void
 */
void hal_crypto_irq_disable(hal_crypto_adapter_t *pcrypto_adapter)
{
    if (pcrypto_adapter->isIntMode) {
#if 0 // YR TODO
        (CRYPTO_MODULE->ipscsr_int_mask_reg_b.cmd_ok_m) = 1;

#if defined(CONFIG_BUILD_NONSECURE) && (CONFIG_BUILD_NONSECURE==1)
        hal_irq_disable(Crypto_IRQn);
        hal_irq_set_vector(Crypto_IRQn, (uint32_t)NULL);
#else
        hal_irq_disable(SCrypto_IRQn);
        hal_irq_set_vector(SCrypto_IRQn, (uint32_t)NULL);
#endif        
#endif
        pcrypto_adapter->isIntMode = 0;
    }
}

/** @} */ /* End of group hs_hal_crypto */

//
// Authentication
//

// MD5
int hal_crypto_md5(IN const u8 *message, IN const u32 msglen, OUT u8 *pDigest)
{
    int ret;
#if CRYPTO_THREAD_MODE
    critical_enter();
#endif
    hal_crypto_adapter_t *pcrypto_adapter = &HAL_CRYPTO_ADAPTER;

    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (pDigest == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (message == NULL) {
        if (msglen > 0) {
            return _ERRNO_CRYPTO_NULL_POINTER;
        } else {
            rtlc_memcpy(pDigest, md5_null_msg_result, 16);
            return SUCCESS;
        }
    }
    if (msglen == 0) {
        rtlc_memcpy(pDigest, md5_null_msg_result, 16);
        return SUCCESS;
    }
    if (msglen > CRYPTO_MAX_MSG_LENGTH) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }

    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_auth(pcrypto_adapter, AUTH_TYPE_MD5, message, msglen, NULL, 0, pDigest);
#if CRYPTO_THREAD_MODE
    critical_leave();
#endif
    return ret;
}

int hal_crypto_md5_init(void)
{
    int ret;
#if CRYPTO_THREAD_MODE
    critical_enter();
#endif
    hal_crypto_adapter_t *pcrypto_adapter = &HAL_CRYPTO_ADAPTER;

    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_auth_init(pcrypto_adapter, AUTH_TYPE_MD5, NULL, 0);
    if (ret != SUCCESS) {
#if CRYPTO_THREAD_MODE
        critical_leave();
#endif
    }
    return ret;
}

int hal_crypto_md5_process(
    IN const u8 *message, IN const u32 msglen,
    OUT u8 *pDigest)
{
    int ret;
    hal_crypto_adapter_t *pcrypto_adapter = &HAL_CRYPTO_ADAPTER;

    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (pDigest == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (message == NULL) {
        if (msglen > 0) {
            return _ERRNO_CRYPTO_NULL_POINTER;
        } else {
            rtlc_memcpy(pDigest, md5_null_msg_result, 16);
            return SUCCESS;
        }
    }
    if (msglen == 0) {
        rtlc_memcpy(pDigest, md5_null_msg_result, 16);
        return SUCCESS;
    }
    if (msglen > CRYPTO_MAX_MSG_LENGTH) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }

    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_auth_process(pcrypto_adapter, AUTH_TYPE_MD5, message, msglen, pDigest);
#if CRYPTO_THREAD_MODE
    critical_leave();
#endif
    return ret;
}

int hal_crypto_md5_update(IN const u8 *message, IN const u32 msglen)
{
    int ret;
    hal_crypto_adapter_t *pcrypto_adapter = &HAL_CRYPTO_ADAPTER;

    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (message == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (msglen == 0) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }
    if (msglen > CRYPTO_MAX_MSG_LENGTH) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }
#if (CHIP_VER == CHIP_A_CUT) && (defined(CONFIG_BUILD_RAM))
    ret = hal_crypto_auth_update_rtl8710c_patch(pcrypto_adapter, AUTH_TYPE_MD5, message, msglen);
#else
    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_auth_update(pcrypto_adapter, AUTH_TYPE_MD5, message, msglen);
#endif
    if (ret != SUCCESS) {
#if CRYPTO_THREAD_MODE
        critical_leave();
#endif
    }
    return ret;
}

int hal_crypto_md5_final(OUT u8 *pDigest)
{
    int ret;
    hal_crypto_adapter_t *pcrypto_adapter = &HAL_CRYPTO_ADAPTER;

    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (pDigest == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
#if (CHIP_VER == CHIP_A_CUT) && (defined(CONFIG_BUILD_RAM))
    ret = hal_crypto_auth_final_rtl8710c_patch(pcrypto_adapter, AUTH_TYPE_MD5, pDigest);
#else
    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_auth_final(pcrypto_adapter, AUTH_TYPE_MD5, pDigest);
#endif

#if CRYPTO_THREAD_MODE
    critical_leave();
#endif
    return ret;
}

// SHA1
int hal_crypto_sha1(IN const u8 *message, IN const u32 msglen, OUT u8 *pDigest)
{
    int ret;

#if CRYPTO_THREAD_MODE
    critical_enter();
#endif
    hal_crypto_adapter_t *pcrypto_adapter = &HAL_CRYPTO_ADAPTER;

    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (pDigest == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (message == NULL) {
        if (msglen > 0) {
            return _ERRNO_CRYPTO_NULL_POINTER;
        } else {
            rtlc_memcpy(pDigest, sha1_null_msg_result, 20);
            return SUCCESS;
        }
    }
    if (msglen == 0) {
        rtlc_memcpy(pDigest, sha1_null_msg_result, 20);
        return SUCCESS;
    }
    if (msglen > CRYPTO_MAX_MSG_LENGTH) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }

    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_auth(pcrypto_adapter, AUTH_TYPE_SHA1, message, msglen, NULL, 0, pDigest);
#if CRYPTO_THREAD_MODE
    critical_leave();
#endif

    return ret;
}

int hal_crypto_sha1_init(void)
{
    int ret;

#if CRYPTO_THREAD_MODE
    critical_enter();
#endif
    hal_crypto_adapter_t *pcrypto_adapter = &HAL_CRYPTO_ADAPTER;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_auth_init(pcrypto_adapter, AUTH_TYPE_SHA1, NULL, 0);
    if ( ret != SUCCESS ) {
#if CRYPTO_THREAD_MODE
        critical_leave();
#endif
    }
    return ret;
}

int hal_crypto_sha1_process(
    IN const u8 *message, IN const u32 msglen,
    OUT u8 *pDigest)
{
    int ret;
    hal_crypto_adapter_t *pcrypto_adapter = &HAL_CRYPTO_ADAPTER;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (pDigest == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (message == NULL) {
        if (msglen > 0) {
            return _ERRNO_CRYPTO_NULL_POINTER;
        } else {
            rtlc_memcpy(pDigest, sha1_null_msg_result, 20);
            return SUCCESS;
        }
    }
    if (msglen == 0) {
        rtlc_memcpy(pDigest, sha1_null_msg_result, 20);
        return SUCCESS;
    }
    if (msglen > CRYPTO_MAX_MSG_LENGTH) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }

    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_auth_process(pcrypto_adapter, AUTH_TYPE_SHA1, message, msglen, pDigest);
#if CRYPTO_THREAD_MODE
    critical_leave();
#endif
    return ret;
}

int hal_crypto_sha1_update(IN const u8 *message, IN const u32 msglen)
{
    int ret;
    hal_crypto_adapter_t *pcrypto_adapter = &HAL_CRYPTO_ADAPTER;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (message == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (msglen == 0) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }
    if (msglen > CRYPTO_MAX_MSG_LENGTH) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }

#if (CHIP_VER == CHIP_A_CUT) && (defined(CONFIG_BUILD_RAM))
    ret = hal_crypto_auth_update_rtl8710c_patch(pcrypto_adapter, AUTH_TYPE_SHA1, message, msglen);
#else
    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_auth_update(pcrypto_adapter, AUTH_TYPE_SHA1, message, msglen);
#endif
    if (ret != SUCCESS) {
#if CRYPTO_THREAD_MODE
        critical_leave();
#endif
    }
    return ret;
}

int hal_crypto_sha1_final(OUT u8 *pDigest)
{
    int ret;
    hal_crypto_adapter_t *pcrypto_adapter = &HAL_CRYPTO_ADAPTER;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (pDigest == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }

#if (CHIP_VER == CHIP_A_CUT) && (defined(CONFIG_BUILD_RAM))
    ret = hal_crypto_auth_final_rtl8710c_patch(pcrypto_adapter, AUTH_TYPE_SHA1, pDigest);
#else
    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_auth_final(pcrypto_adapter, AUTH_TYPE_SHA1, pDigest);
#endif

#if CRYPTO_THREAD_MODE
    critical_leave();
#endif

    return ret;
}

// SHA2-224
int hal_crypto_sha2_224(IN const u8 *message, IN const u32 msglen, OUT u8 *pDigest)
{
    int ret;
#if CRYPTO_THREAD_MODE
    critical_enter();
#endif
    hal_crypto_adapter_t *pcrypto_adapter = &HAL_CRYPTO_ADAPTER;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (pDigest == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (message == NULL) {
        if (msglen > 0) {
            return _ERRNO_CRYPTO_NULL_POINTER;
        } else {
            rtlc_memcpy(pDigest, sha2_224_null_msg_result, 28);
            return SUCCESS;
        }
    }
    if (msglen == 0) {
        rtlc_memcpy(pDigest, sha2_224_null_msg_result, 28);
        return SUCCESS;
    }
    if (msglen > CRYPTO_MAX_MSG_LENGTH) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }

    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_auth(pcrypto_adapter, AUTH_TYPE_SHA2_224_ALL, message, msglen, NULL, 0, pDigest);
#if CRYPTO_THREAD_MODE
    critical_leave();
#endif
    return ret;
}

int hal_crypto_sha2_224_init(void)
{
    int ret;
#if CRYPTO_THREAD_MODE
    critical_enter();
#endif
    hal_crypto_adapter_t *pcrypto_adapter = &HAL_CRYPTO_ADAPTER;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_auth_init(pcrypto_adapter, AUTH_TYPE_SHA2_224_ALL, NULL, 0);
    if ( ret != SUCCESS ) {
#if CRYPTO_THREAD_MODE
        critical_leave();
#endif
    }
    return ret;
}

int hal_crypto_sha2_224_process(
    IN const u8 *message, IN const u32 msglen,
    OUT u8 *pDigest)
{
    int ret;
    hal_crypto_adapter_t *pcrypto_adapter = &HAL_CRYPTO_ADAPTER;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (pDigest == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (message == NULL) {
        if (msglen > 0) {
            return _ERRNO_CRYPTO_NULL_POINTER;
        } else {
            rtlc_memcpy(pDigest, sha2_224_null_msg_result, 28);
            return SUCCESS;
        }
    }
    if (msglen == 0) {
        rtlc_memcpy(pDigest, sha2_224_null_msg_result, 28);
        return SUCCESS;
    }
    if (msglen > CRYPTO_MAX_MSG_LENGTH) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }

    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_auth_process(pcrypto_adapter, AUTH_TYPE_SHA2_224_ALL, message, msglen, pDigest);
#if CRYPTO_THREAD_MODE
    critical_leave();
#endif
    return ret;
}

int hal_crypto_sha2_224_update(IN const u8 *message, IN const u32 msglen)
{
    int ret;
    hal_crypto_adapter_t *pcrypto_adapter = &HAL_CRYPTO_ADAPTER;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (message == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (msglen == 0) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }
    if (msglen > CRYPTO_MAX_MSG_LENGTH) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }

#if (CHIP_VER == CHIP_A_CUT) && (defined(CONFIG_BUILD_RAM))
    ret = hal_crypto_auth_update_rtl8710c_patch(pcrypto_adapter, AUTH_TYPE_SHA2_224_ALL, message, msglen);
#else
    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_auth_update(pcrypto_adapter, AUTH_TYPE_SHA2_224_ALL, message, msglen);
#endif
    if (ret != SUCCESS) {
#if CRYPTO_THREAD_MODE
        critical_leave();
#endif
    }
    return ret;
}

int hal_crypto_sha2_224_final(OUT u8 *pDigest)
{
    int ret;
    hal_crypto_adapter_t *pcrypto_adapter = &HAL_CRYPTO_ADAPTER;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (pDigest == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }

#if (CHIP_VER == CHIP_A_CUT) && (defined(CONFIG_BUILD_RAM))
    ret = hal_crypto_auth_final_rtl8710c_patch(pcrypto_adapter, AUTH_TYPE_SHA2_224_ALL, pDigest);
#else
    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_auth_final(pcrypto_adapter, AUTH_TYPE_SHA2_224_ALL, pDigest);
#endif

#if CRYPTO_THREAD_MODE
    critical_leave();
#endif
    return ret;
}


// SHA2-256
int hal_crypto_sha2_256(IN const u8 *message, IN const u32 msglen, OUT u8 *pDigest)
{
    int ret;
#if CRYPTO_THREAD_MODE
    critical_enter();
#endif
    hal_crypto_adapter_t *pcrypto_adapter = &HAL_CRYPTO_ADAPTER;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (pDigest == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (message == NULL) {
        if (msglen > 0) {
            return _ERRNO_CRYPTO_NULL_POINTER;
        } else {
            rtlc_memcpy(pDigest, sha2_256_null_msg_result, 32);
            return SUCCESS;
        }
    }
    if (msglen == 0) {
        rtlc_memcpy(pDigest, sha2_256_null_msg_result, 32);
        return SUCCESS;
    }
    if (msglen > CRYPTO_MAX_MSG_LENGTH) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }

    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_auth(pcrypto_adapter, AUTH_TYPE_SHA2_256_ALL, message, msglen, NULL, 0, pDigest);
#if CRYPTO_THREAD_MODE
    critical_leave();
#endif
    return ret;
}

int hal_crypto_sha2_256_init(void)
{
    int ret;
#if CRYPTO_THREAD_MODE
    critical_enter();
#endif
    hal_crypto_adapter_t *pcrypto_adapter = &HAL_CRYPTO_ADAPTER;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_auth_init(pcrypto_adapter, AUTH_TYPE_SHA2_256_ALL, NULL, 0);
    if (ret != SUCCESS) {
#if CRYPTO_THREAD_MODE
        critical_leave();
#endif
    }
    return ret;
}

int hal_crypto_sha2_256_process(
    IN const u8 *message, IN const u32 msglen,
    OUT u8 *pDigest)
{
    int ret;
    hal_crypto_adapter_t *pcrypto_adapter = &HAL_CRYPTO_ADAPTER;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (pDigest == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (message == NULL) {
        if (msglen > 0) {
            return _ERRNO_CRYPTO_NULL_POINTER;
        } else {
            rtlc_memcpy(pDigest, sha2_256_null_msg_result, 32);
            return SUCCESS;
        }
    }
    if (msglen == 0) {
        rtlc_memcpy(pDigest, sha2_256_null_msg_result, 32);
        return SUCCESS;
    }
    if (msglen > CRYPTO_MAX_MSG_LENGTH) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }

    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_auth_process(pcrypto_adapter, AUTH_TYPE_SHA2_256_ALL, message, msglen, pDigest);
#if CRYPTO_THREAD_MODE
    critical_leave();
#endif
    return ret;
}

int hal_crypto_sha2_256_update(IN const u8 *message, IN const u32 msglen)
{
    int ret;
    hal_crypto_adapter_t *pcrypto_adapter = &HAL_CRYPTO_ADAPTER;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (message == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (msglen == 0) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }
    if (msglen > CRYPTO_MAX_MSG_LENGTH) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }

#if (CHIP_VER == CHIP_A_CUT) && (defined(CONFIG_BUILD_RAM))
    ret = hal_crypto_auth_update_rtl8710c_patch(pcrypto_adapter, AUTH_TYPE_SHA2_256_ALL, message, msglen);
#else
    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_auth_update(pcrypto_adapter, AUTH_TYPE_SHA2_256_ALL, message, msglen);
#endif
    if (ret != SUCCESS) {
#if CRYPTO_THREAD_MODE
        critical_leave();
#endif
    }
    return ret;
}

int hal_crypto_sha2_256_final(OUT u8 *pDigest)
{
    int ret;
    hal_crypto_adapter_t *pcrypto_adapter = &HAL_CRYPTO_ADAPTER;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (pDigest == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }

#if (CHIP_VER == CHIP_A_CUT) && (defined(CONFIG_BUILD_RAM))
    ret = hal_crypto_auth_final_rtl8710c_patch(pcrypto_adapter, AUTH_TYPE_SHA2_256_ALL, pDigest);
#else
    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_auth_final(pcrypto_adapter, AUTH_TYPE_SHA2_256_ALL, pDigest);
#endif
#if CRYPTO_THREAD_MODE
    critical_leave();
#endif
    return ret;
}


// HMAC-md5
int hal_crypto_hmac_md5(IN const u8 *message, IN const u32 msglen,
                        IN const u8 *key, IN const u32 keylen, OUT u8 *pDigest)
{
    int ret;
#if CRYPTO_THREAD_MODE
    critical_enter();
#endif
    hal_crypto_adapter_t *pcrypto_adapter = &HAL_CRYPTO_ADAPTER;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (key == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (check_4byte_align((size_t)(key))) {
        return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
    }
    if (keylen <= 0) {
        return _ERRNO_CRYPTO_KEY_OutRange;
    }
    if (keylen > CRYPTO_AUTH_PADDING) {
        return _ERRNO_CRYPTO_KEY_OutRange;
    }
    if (pDigest == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (message == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (msglen == 0) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }
    if (msglen > CRYPTO_MAX_MSG_LENGTH) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }

    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_auth(pcrypto_adapter, AUTH_TYPE_HMAC_MD5, message, msglen, key, keylen, pDigest);
#if CRYPTO_THREAD_MODE
    critical_leave();
#endif
    return ret;
}

int hal_crypto_hmac_md5_init(IN const u8 *key, IN const u32 keylen)
{
    int ret;
#if CRYPTO_THREAD_MODE
    critical_enter();
#endif
    hal_crypto_adapter_t *pcrypto_adapter = &HAL_CRYPTO_ADAPTER;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (key == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (check_4byte_align((size_t)(key))) {
        return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
    }
    if (keylen <= 0) {
        return _ERRNO_CRYPTO_KEY_OutRange;
    }
    if (keylen > CRYPTO_AUTH_PADDING ) {
        return _ERRNO_CRYPTO_KEY_OutRange;
    }

    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_auth_init(pcrypto_adapter, AUTH_TYPE_HMAC_MD5, key, keylen);
    if (ret != SUCCESS) {
#if CRYPTO_THREAD_MODE
        critical_leave();
#endif
    }
    return ret;
}

int hal_crypto_hmac_md5_process(IN const u8 *message, IN const u32 msglen, OUT u8 *pDigest)
{
    int ret;
    hal_crypto_adapter_t *pcrypto_adapter = &HAL_CRYPTO_ADAPTER;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (pDigest == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (message == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (msglen == 0) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }
    if (msglen > CRYPTO_MAX_MSG_LENGTH) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }

    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_auth_process(pcrypto_adapter, AUTH_TYPE_HMAC_MD5, message, msglen, pDigest);
#if CRYPTO_THREAD_MODE
    critical_leave();
#endif
    return ret;
}

int hal_crypto_hmac_md5_update(IN const u8 *message, IN const u32 msglen)
{
    int ret;
    hal_crypto_adapter_t *pcrypto_adapter = &HAL_CRYPTO_ADAPTER;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (message == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (msglen == 0) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }
    if (msglen > CRYPTO_MAX_MSG_LENGTH) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }

#if (CHIP_VER == CHIP_A_CUT) && (defined(CONFIG_BUILD_RAM))
    ret = hal_crypto_auth_update_rtl8710c_patch(pcrypto_adapter, AUTH_TYPE_HMAC_MD5, message, msglen);
#else
    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_auth_update(pcrypto_adapter, AUTH_TYPE_HMAC_MD5, message, msglen);
#endif
    if (ret != SUCCESS) {
#if CRYPTO_THREAD_MODE
        critical_leave();
#endif
    }
    return ret;
}

int hal_crypto_hmac_md5_final(OUT u8 *pDigest)
{
    int ret;
    hal_crypto_adapter_t *pcrypto_adapter = &HAL_CRYPTO_ADAPTER;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (pDigest == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }

#if (CHIP_VER == CHIP_A_CUT) && (defined(CONFIG_BUILD_RAM))
    ret = hal_crypto_auth_final_rtl8710c_patch(pcrypto_adapter, AUTH_TYPE_HMAC_MD5, pDigest);
#else
    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_auth_final(pcrypto_adapter, AUTH_TYPE_HMAC_MD5, pDigest);
#endif
#if CRYPTO_THREAD_MODE
    critical_leave();
#endif
    return ret;
}


// HMAC-sha1
int hal_crypto_hmac_sha1(IN const u8 *message, IN const u32 msglen,
                         IN const u8 *key, IN const u32 keylen, OUT u8 *pDigest)
{
    int ret;
#if CRYPTO_THREAD_MODE
    critical_enter();
#endif
    hal_crypto_adapter_t *pcrypto_adapter = &HAL_CRYPTO_ADAPTER;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (key == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (check_4byte_align((size_t)(key))) {
        return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
    }
    if (keylen <= 0) {
        return _ERRNO_CRYPTO_KEY_OutRange;
    }
    if (keylen > CRYPTO_AUTH_PADDING) {
        return _ERRNO_CRYPTO_KEY_OutRange;
    }
    if (pDigest == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (message == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (msglen == 0) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }
    if (msglen > CRYPTO_MAX_MSG_LENGTH) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }

    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_auth(pcrypto_adapter, AUTH_TYPE_HMAC_SHA1, message, msglen, key, keylen, pDigest);
#if CRYPTO_THREAD_MODE
    critical_leave();
#endif
    return ret;
}

int hal_crypto_hmac_sha1_init(IN const u8 *key, IN const u32 keylen)
{
    int ret;
#if CRYPTO_THREAD_MODE
    critical_enter();
#endif
    hal_crypto_adapter_t *pcrypto_adapter = &HAL_CRYPTO_ADAPTER;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (key == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (check_4byte_align((size_t)(key))) {
        return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
    }
    if (keylen <= 0) {
        return _ERRNO_CRYPTO_KEY_OutRange;
    }
    if (keylen > CRYPTO_AUTH_PADDING) {
        return _ERRNO_CRYPTO_KEY_OutRange;
    }

    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_auth_init(pcrypto_adapter, AUTH_TYPE_HMAC_SHA1, key, keylen);
    if (ret != SUCCESS) {
#if CRYPTO_THREAD_MODE
        critical_leave();
#endif
    }
    return ret;
}

int hal_crypto_hmac_sha1_process(IN const u8 *message, IN const u32 msglen, OUT u8 *pDigest)
{
    int ret;
    hal_crypto_adapter_t *pcrypto_adapter = &HAL_CRYPTO_ADAPTER;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (pDigest == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (message == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (msglen == 0) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }
    if (msglen > CRYPTO_MAX_MSG_LENGTH) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }

    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_auth_process(pcrypto_adapter, AUTH_TYPE_HMAC_SHA1, message, msglen, pDigest);
#if CRYPTO_THREAD_MODE
    critical_leave();
#endif
    return ret;
}

int hal_crypto_hmac_sha1_update(IN const u8 *message, IN const u32 msglen)
{
    int ret;
    hal_crypto_adapter_t *pcrypto_adapter = &HAL_CRYPTO_ADAPTER;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (message == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (msglen == 0) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }
    if (msglen > CRYPTO_MAX_MSG_LENGTH) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }

#if (CHIP_VER == CHIP_A_CUT) && (defined(CONFIG_BUILD_RAM))
    ret = hal_crypto_auth_update_rtl8710c_patch(pcrypto_adapter, AUTH_TYPE_HMAC_SHA1, message, msglen);
#else
    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_auth_update(pcrypto_adapter, AUTH_TYPE_HMAC_SHA1, message, msglen);
#endif
    if (ret != SUCCESS) {
#if CRYPTO_THREAD_MODE
        critical_leave();
#endif
    }
    return ret;
}

int hal_crypto_hmac_sha1_final(OUT u8 *pDigest)
{
    int ret;
    hal_crypto_adapter_t *pcrypto_adapter = &HAL_CRYPTO_ADAPTER;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (pDigest == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
#if (CHIP_VER == CHIP_A_CUT) && (defined(CONFIG_BUILD_RAM))
    ret = hal_crypto_auth_final_rtl8710c_patch(pcrypto_adapter, AUTH_TYPE_HMAC_SHA1, pDigest);
#else
    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_auth_final(pcrypto_adapter, AUTH_TYPE_HMAC_SHA1, pDigest);
#endif
#if CRYPTO_THREAD_MODE
    critical_leave();
#endif
    return ret;
}


// HMAC-sha2
// -- 224
int hal_crypto_hmac_sha2_224( IN const u8 *message, IN const u32 msglen,
                              IN const u8 *key, IN const u32 keylen, OUT u8 *pDigest)
{
    int ret;
#if CRYPTO_THREAD_MODE
    critical_enter();
#endif
    hal_crypto_adapter_t *pcrypto_adapter = &HAL_CRYPTO_ADAPTER;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (key == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (check_4byte_align((size_t)(key))) {
        return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
    }
    if (keylen <= 0) {
        return _ERRNO_CRYPTO_KEY_OutRange;
    }
    if (keylen > CRYPTO_AUTH_PADDING) {
        return _ERRNO_CRYPTO_KEY_OutRange;
    }
    if (pDigest == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (message == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (msglen == 0) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }
    if (msglen > CRYPTO_MAX_MSG_LENGTH) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }

    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_auth(pcrypto_adapter, AUTH_TYPE_HMAC_SHA2_224_ALL, message, msglen, key, keylen, pDigest);
#if CRYPTO_THREAD_MODE
    critical_leave();
#endif
    return ret;
}

int hal_crypto_hmac_sha2_224_init(IN const u8 *key, IN const u32 keylen)
{
    int ret;
#if CRYPTO_THREAD_MODE
    critical_enter();
#endif
    hal_crypto_adapter_t *pcrypto_adapter = &HAL_CRYPTO_ADAPTER;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (key == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (check_4byte_align((size_t)(key))) {
        return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
    }
    if (keylen <= 0) {
        return _ERRNO_CRYPTO_KEY_OutRange;
    }
    if (keylen > CRYPTO_AUTH_PADDING) {
        return _ERRNO_CRYPTO_KEY_OutRange;
    }

    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_auth_init(pcrypto_adapter, AUTH_TYPE_HMAC_SHA2_224_ALL, key, keylen);
    if (ret != SUCCESS) {
#if CRYPTO_THREAD_MODE
        critical_leave();
#endif
    }
    return ret;
}

int hal_crypto_hmac_sha2_224_process(IN const u8 *message, IN const u32 msglen, OUT u8 *pDigest)
{
    int ret;
    hal_crypto_adapter_t *pcrypto_adapter = &HAL_CRYPTO_ADAPTER;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (pDigest == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (message == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (msglen == 0) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }
    if (msglen > CRYPTO_MAX_MSG_LENGTH) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }

    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_auth_process(pcrypto_adapter, AUTH_TYPE_HMAC_SHA2_224_ALL, message, msglen, pDigest);
#if CRYPTO_THREAD_MODE
    critical_leave();
#endif
    return ret;
}

int hal_crypto_hmac_sha2_224_update(IN const u8 *message, IN const u32 msglen)
{
    int ret;
    hal_crypto_adapter_t *pcrypto_adapter = &HAL_CRYPTO_ADAPTER;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (message == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (msglen == 0) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }
    if (msglen > CRYPTO_MAX_MSG_LENGTH) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }

#if (CHIP_VER == CHIP_A_CUT) && (defined(CONFIG_BUILD_RAM))
    ret = hal_crypto_auth_update_rtl8710c_patch(pcrypto_adapter, AUTH_TYPE_HMAC_SHA2_224_ALL, message, msglen);
#else
    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_auth_update(pcrypto_adapter, AUTH_TYPE_HMAC_SHA2_224_ALL, message, msglen);
#endif
    if (ret != SUCCESS) {
#if CRYPTO_THREAD_MODE
        critical_leave();
#endif
    }
    return ret;
}

int hal_crypto_hmac_sha2_224_final(OUT u8 *pDigest)
{
    int ret;
    hal_crypto_adapter_t *pcrypto_adapter = &HAL_CRYPTO_ADAPTER;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (pDigest == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }

#if (CHIP_VER == CHIP_A_CUT) && (defined(CONFIG_BUILD_RAM))
    ret = hal_crypto_auth_final_rtl8710c_patch(pcrypto_adapter, AUTH_TYPE_HMAC_SHA2_224_ALL, pDigest);
#else
    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_auth_final(pcrypto_adapter, AUTH_TYPE_HMAC_SHA2_224_ALL, pDigest);
#endif
#if CRYPTO_THREAD_MODE
    critical_leave();
#endif
    return ret;
}

// -- 256
int hal_crypto_hmac_sha2_256( IN const u8 *message, IN const u32 msglen,
                              IN const u8 *key, IN const u32 keylen, OUT u8 *pDigest)
{
    int ret;
#if CRYPTO_THREAD_MODE
    critical_enter();
#endif
    hal_crypto_adapter_t *pcrypto_adapter = &HAL_CRYPTO_ADAPTER;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (key == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (check_4byte_align((size_t)(key))) {
        return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
    }
    if (keylen <= 0) {
        return _ERRNO_CRYPTO_KEY_OutRange;
    }
    if (keylen > CRYPTO_AUTH_PADDING) {
        return _ERRNO_CRYPTO_KEY_OutRange;
    }
    if (pDigest == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (message == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (msglen == 0) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }
    if (msglen > CRYPTO_MAX_MSG_LENGTH) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }

    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_auth(pcrypto_adapter, AUTH_TYPE_HMAC_SHA2_256_ALL, message, msglen, key, keylen, pDigest);
#if CRYPTO_THREAD_MODE
    critical_leave();
#endif
    return ret;
}

int hal_crypto_hmac_sha2_256_init(IN const u8 *key, IN const u32 keylen)
{
    int ret;
#if CRYPTO_THREAD_MODE
    critical_enter();
#endif
    hal_crypto_adapter_t *pcrypto_adapter = &HAL_CRYPTO_ADAPTER;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (key == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (check_4byte_align((size_t)(key))) {
        return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
    }
    if (keylen <= 0) {
        return _ERRNO_CRYPTO_KEY_OutRange;
    }
    if (keylen > CRYPTO_AUTH_PADDING) {
        return _ERRNO_CRYPTO_KEY_OutRange;
    }

    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_auth_init(pcrypto_adapter, AUTH_TYPE_HMAC_SHA2_256_ALL, key, keylen);
    if (ret != SUCCESS) {
#if CRYPTO_THREAD_MODE
        critical_leave();
#endif
    }
    return ret;
}

int hal_crypto_hmac_sha2_256_process(IN const u8 *message, IN const u32 msglen, OUT u8 *pDigest)
{
    int ret;
    hal_crypto_adapter_t *pcrypto_adapter = &HAL_CRYPTO_ADAPTER;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (pDigest == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (message == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (msglen == 0) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }
    if (msglen > CRYPTO_MAX_MSG_LENGTH) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }

    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_auth_process(pcrypto_adapter, AUTH_TYPE_HMAC_SHA2_256_ALL, message, msglen, pDigest);
#if CRYPTO_THREAD_MODE
    critical_leave();
#endif
    return ret;
}

int hal_crypto_hmac_sha2_256_update(IN const u8 *message, IN const u32 msglen)
{
    int ret;
    hal_crypto_adapter_t *pcrypto_adapter = &HAL_CRYPTO_ADAPTER;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }   
    if (message == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (msglen == 0) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }
    if (msglen > CRYPTO_MAX_MSG_LENGTH) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }

#if (CHIP_VER == CHIP_A_CUT) && (defined(CONFIG_BUILD_RAM))
    ret = hal_crypto_auth_update_rtl8710c_patch(pcrypto_adapter, AUTH_TYPE_HMAC_SHA2_256_ALL, message, msglen);
#else
    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_auth_update(pcrypto_adapter, AUTH_TYPE_HMAC_SHA2_256_ALL, message, msglen);
#endif
    if (ret != SUCCESS) {
#if CRYPTO_THREAD_MODE
        critical_leave();
#endif
    }
    return ret;
}

int hal_crypto_hmac_sha2_256_final(OUT u8 *pDigest)
{
    int ret;
    hal_crypto_adapter_t *pcrypto_adapter = &HAL_CRYPTO_ADAPTER;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (pDigest == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }

#if (CHIP_VER == CHIP_A_CUT) && (defined(CONFIG_BUILD_RAM))
    ret = hal_crypto_auth_final_rtl8710c_patch(pcrypto_adapter, AUTH_TYPE_HMAC_SHA2_256_ALL, pDigest);
#else
    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_auth_final(pcrypto_adapter, AUTH_TYPE_HMAC_SHA2_256_ALL, pDigest);
#endif
#if CRYPTO_THREAD_MODE
    critical_leave();
#endif
    return ret;
}

//
// Cipher
//

// DES-CBC

int hal_crypto_des_cbc_init(IN const u8 *key, IN const u32 keylen)
{
    int ret;
#if CRYPTO_THREAD_MODE
    critical_enter();
#endif
    hal_crypto_adapter_t *pcrypto_adapter = &g_rtl_cryptoEngine_s;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (key == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (check_4byte_align((size_t)(key))) {
        return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
    }
    if (keylen != CRYPTO_DES_KEY_LENGTH) {
        return _ERRNO_CRYPTO_KEY_OutRange;
    }

    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_cipher_init(pcrypto_adapter, CIPHER_TYPE_DES_CBC, key, keylen);
    return ret;
}

int hal_crypto_des_cbc_encrypt(
    IN const u8 *message,   IN const u32 msglen,
    IN const u8 *iv,        IN const u32 ivlen,
    OUT u8 *pResult)
{
    int ret;
    hal_crypto_adapter_t *pcrypto_adapter = &g_rtl_cryptoEngine_s;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (iv == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (check_4byte_align((size_t)(iv))) {
        return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
    }
    if (ivlen != CRYPTO_DES_IV_LENGTH) {
        return _ERRNO_CRYPTO_IV_OutRange;
    }
    if (pResult == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (message == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (msglen == 0) {
        return 0;
    }
    if (msglen > CRYPTO_MAX_MSG_LENGTH) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }
    if ((msglen % CRYPTO_DES_IV_LENGTH) != 0) {
        return _ERRNO_CRYPTO_DES_MSGLEN_NOT_8Byte_Aligned;
    }

    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_cipher_encrypt(pcrypto_adapter, 
        CIPHER_TYPE_DES_CBC, message, msglen, iv, ivlen, NULL, 0, pResult, NULL);
#if CRYPTO_THREAD_MODE
    critical_leave();
#endif
    return ret;
}

int hal_crypto_des_cbc_decrypt(
    IN const u8 *message,   IN const u32 msglen,
    IN const u8 *iv,        IN const u32 ivlen,
    OUT u8 *pResult)
{
    int ret;
    hal_crypto_adapter_t *pcrypto_adapter = &g_rtl_cryptoEngine_s;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (iv == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (check_4byte_align((size_t)(iv))) {
        return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
    }
    if (ivlen != CRYPTO_DES_IV_LENGTH) {
        return _ERRNO_CRYPTO_IV_OutRange;
    }
    if (pResult == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (message == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (msglen == 0) {
        return 0;
    }
    if (msglen > CRYPTO_MAX_MSG_LENGTH) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }
    if ((msglen % CRYPTO_DES_IV_LENGTH) != 0) {
        return _ERRNO_CRYPTO_DES_MSGLEN_NOT_8Byte_Aligned;
    }

    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_cipher_decrypt(pcrypto_adapter, 
        CIPHER_TYPE_DES_CBC, message, msglen, iv, ivlen, NULL, 0, pResult, NULL);
#if CRYPTO_THREAD_MODE
    critical_leave();
#endif
    return ret;
}

// DES-ECB

int hal_crypto_des_ecb_init(IN const u8 *key, IN const u32 keylen)
{
    int ret;
#if CRYPTO_THREAD_MODE
    critical_enter();
#endif
    hal_crypto_adapter_t *pcrypto_adapter = &g_rtl_cryptoEngine_s;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if ( key == NULL ) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (check_4byte_align((size_t)(key))) {
        return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
    }
    if (keylen != CRYPTO_DES_KEY_LENGTH) {
        return _ERRNO_CRYPTO_KEY_OutRange;
    }

    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_cipher_init(pcrypto_adapter, CIPHER_TYPE_DES_ECB, key, keylen);
    return ret;
}

int hal_crypto_des_ecb_encrypt(
    IN const u8 *message,   IN const u32 msglen,
    IN const u8 *iv,        IN const u32 ivlen,
    OUT u8 *pResult)
{
    int ret;
    hal_crypto_adapter_t *pcrypto_adapter = &g_rtl_cryptoEngine_s;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    iv = NULL;
    if (ivlen != 0) {
        return _ERRNO_CRYPTO_IV_OutRange;
    }
    if (pResult == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (message == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (msglen == 0) {
        return 0;
    }
    if (msglen > CRYPTO_MAX_MSG_LENGTH) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }
    if ((msglen % CRYPTO_DES_IV_LENGTH) != 0) {
        return _ERRNO_CRYPTO_DES_MSGLEN_NOT_8Byte_Aligned;
    }

    ret =  HAL_CRYPTO_FUNC_STUBS.hal_crypto_cipher_encrypt(pcrypto_adapter, 
        CIPHER_TYPE_DES_ECB, message, msglen, iv, ivlen, NULL, 0, pResult, NULL);
#if CRYPTO_THREAD_MODE
    critical_leave();
#endif
    return ret;
}

int hal_crypto_des_ecb_decrypt(
    IN const u8 *message,   IN const u32 msglen,
    IN const u8 *iv,        IN const u32 ivlen,
    OUT u8 *pResult)
{
    int ret;

    hal_crypto_adapter_t *pcrypto_adapter = &g_rtl_cryptoEngine_s;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    iv = NULL;
    if (ivlen != 0) {
        return _ERRNO_CRYPTO_IV_OutRange;
    }
    if (pResult == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (message == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (msglen == 0) {
        return 0;
    }
    if (msglen > CRYPTO_MAX_MSG_LENGTH) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }
    if ((msglen % CRYPTO_DES_IV_LENGTH) != 0) {
        return _ERRNO_CRYPTO_DES_MSGLEN_NOT_8Byte_Aligned;
    }

    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_cipher_decrypt(pcrypto_adapter, 
        CIPHER_TYPE_DES_ECB, message, msglen, iv, ivlen, NULL, 0, pResult, NULL);
#if CRYPTO_THREAD_MODE
    critical_leave();
#endif
    return ret;
}

// DES-CFB

int hal_crypto_des_cfb_init(IN const u8 *key, IN const u32 keylen)
{
    int ret;
#if CRYPTO_THREAD_MODE
    critical_enter();
#endif
    hal_crypto_adapter_t *pcrypto_adapter = &g_rtl_cryptoEngine_s;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (key == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (check_4byte_align((size_t)(key))) {
        return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
    }
    if (keylen != CRYPTO_DES_KEY_LENGTH) {
        return _ERRNO_CRYPTO_KEY_OutRange;
    }

    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_cipher_init(pcrypto_adapter, CIPHER_TYPE_DES_CFB, key, keylen);
    return ret;
}

int hal_crypto_des_cfb_encrypt(
    IN const u8 *message,   IN const u32 msglen,
    IN const u8 *iv,        IN const u32 ivlen,
    OUT u8 *pResult)
{
    int ret;
    hal_crypto_adapter_t *pcrypto_adapter = &g_rtl_cryptoEngine_s;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (iv == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (check_4byte_align((size_t)(iv))) {
        return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
    }
    if (ivlen != CRYPTO_DES_IV_LENGTH) {
        return _ERRNO_CRYPTO_IV_OutRange;
    }
    if (pResult == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (message == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (msglen == 0) {
        return 0;
    }
    if (msglen > CRYPTO_MAX_MSG_LENGTH) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }
    if ((msglen % CRYPTO_DES_IV_LENGTH) != 0) {
        return _ERRNO_CRYPTO_DES_MSGLEN_NOT_8Byte_Aligned;
    }

    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_cipher_encrypt(pcrypto_adapter, 
        CIPHER_TYPE_DES_CFB, message, msglen, iv, ivlen, NULL, 0, pResult, NULL);
#if CRYPTO_THREAD_MODE
    critical_leave();
#endif
    return ret;
}

int hal_crypto_des_cfb_decrypt(
    IN const u8 *message,   IN const u32 msglen,
    IN const u8 *iv,        IN const u32 ivlen,
    OUT u8 *pResult)
{
    int ret;
    hal_crypto_adapter_t *pcrypto_adapter = &g_rtl_cryptoEngine_s;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (iv == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (check_4byte_align((size_t)(iv))) {
        return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
    }
    if (ivlen != CRYPTO_DES_IV_LENGTH) {
        return _ERRNO_CRYPTO_IV_OutRange;
    }
    if (pResult == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (message == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (msglen == 0) {
        return 0;
    }
    if (msglen > CRYPTO_MAX_MSG_LENGTH) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }
    if ((msglen % CRYPTO_DES_IV_LENGTH) != 0) {
        return _ERRNO_CRYPTO_DES_MSGLEN_NOT_8Byte_Aligned;
    }

    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_cipher_decrypt(pcrypto_adapter, 
        CIPHER_TYPE_DES_CFB, message, msglen, iv, ivlen, NULL, 0, pResult, NULL);
#if CRYPTO_THREAD_MODE
    critical_leave();
#endif
    return ret;
}

// DES-OFB

int hal_crypto_des_ofb_init(IN const u8 *key, IN const u32 keylen)
{
    int ret;
#if CRYPTO_THREAD_MODE
    critical_enter();
#endif
    hal_crypto_adapter_t *pcrypto_adapter = &g_rtl_cryptoEngine_s;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (key == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (check_4byte_align((size_t)(key))) {
        return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
    }
    if (keylen != CRYPTO_DES_KEY_LENGTH) {
        return _ERRNO_CRYPTO_KEY_OutRange;
    }

    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_cipher_init(pcrypto_adapter, CIPHER_TYPE_DES_OFB, key, keylen);
    return ret;
}

int hal_crypto_des_ofb_encrypt(
    IN const u8 *message,   IN const u32 msglen,
    IN const u8 *iv,        IN const u32 ivlen,
    OUT u8 *pResult)
{
    int ret;
    hal_crypto_adapter_t *pcrypto_adapter = &g_rtl_cryptoEngine_s;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (iv == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (check_4byte_align((size_t)(iv))) {
        return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
    }
    if (ivlen != CRYPTO_DES_IV_LENGTH) {
        return _ERRNO_CRYPTO_IV_OutRange;
    }
    if (pResult == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (message == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (msglen == 0) {
        return 0;
    }
    if (msglen > CRYPTO_MAX_MSG_LENGTH) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }
    if ((msglen % CRYPTO_DES_IV_LENGTH) != 0) {
        return _ERRNO_CRYPTO_DES_MSGLEN_NOT_8Byte_Aligned;
    }

    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_cipher_encrypt(pcrypto_adapter, 
        CIPHER_TYPE_DES_OFB, message, msglen, iv, ivlen, NULL, 0, pResult, NULL);
#if CRYPTO_THREAD_MODE
    critical_leave();
#endif
    return ret;
}

int hal_crypto_des_ofb_decrypt(
    IN const u8 *message,   IN const u32 msglen,
    IN const u8 *iv,        IN const u32 ivlen,
    OUT u8 *pResult)
{
    int ret;
    hal_crypto_adapter_t *pcrypto_adapter = &g_rtl_cryptoEngine_s;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (iv == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (check_4byte_align((size_t)(iv))) {
        return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
    }
    if (ivlen != CRYPTO_DES_IV_LENGTH) {
        return _ERRNO_CRYPTO_IV_OutRange;
    }
    if (pResult == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (message == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (msglen == 0) {
        return 0;
    }
    if (msglen > CRYPTO_MAX_MSG_LENGTH) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }
    if ((msglen % CRYPTO_DES_IV_LENGTH) != 0) {
        return _ERRNO_CRYPTO_DES_MSGLEN_NOT_8Byte_Aligned;
    }

    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_cipher_decrypt(pcrypto_adapter, 
        CIPHER_TYPE_DES_OFB, message, msglen, iv, ivlen, NULL, 0, pResult, NULL);
#if CRYPTO_THREAD_MODE
    critical_leave();
#endif
    return ret;
}

// DES-CTR

int hal_crypto_des_ctr_init(IN const u8 *key, IN const u32 keylen)
{
    int ret;
#if CRYPTO_THREAD_MODE
    critical_enter();
#endif
    hal_crypto_adapter_t *pcrypto_adapter = &g_rtl_cryptoEngine_s;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (key == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (check_4byte_align((size_t)(key))) {
        return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
    }
    if (keylen != CRYPTO_DES_KEY_LENGTH) {
        return _ERRNO_CRYPTO_KEY_OutRange;
    }

    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_cipher_init(pcrypto_adapter, CIPHER_TYPE_DES_CTR, key, keylen);
    return ret;
}

int hal_crypto_des_ctr_encrypt(
    IN const u8 *message,   IN const u32 msglen,
    IN const u8 *iv,        IN const u32 ivlen,
    OUT u8 *pResult)
{
    int ret;
    hal_crypto_adapter_t *pcrypto_adapter = &g_rtl_cryptoEngine_s;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (iv == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (check_4byte_align((size_t)(iv))) {
        return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
    }
    if (ivlen != CRYPTO_DES_IV_LENGTH) {
        return _ERRNO_CRYPTO_IV_OutRange;
    }
    if (pResult == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (message == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (msglen == 0) {
        return 0;
    }
    if (msglen > CRYPTO_MAX_MSG_LENGTH) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }
    if ((msglen % CRYPTO_DES_IV_LENGTH) != 0) {
        return _ERRNO_CRYPTO_DES_MSGLEN_NOT_8Byte_Aligned;
    }

    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_cipher_encrypt(pcrypto_adapter, 
        CIPHER_TYPE_DES_CTR, message, msglen, iv, ivlen, NULL, 0, pResult, NULL);
#if CRYPTO_THREAD_MODE
    critical_leave();
#endif
    return ret;
}

int hal_crypto_des_ctr_decrypt(
    IN const u8 *message,   IN const u32 msglen,
    IN const u8 *iv,        IN const u32 ivlen,
    OUT u8 *pResult)
{
    int ret;
    hal_crypto_adapter_t *pcrypto_adapter = &g_rtl_cryptoEngine_s;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (iv == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (check_4byte_align((size_t)(iv))) {
        return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
    }
    if (ivlen != CRYPTO_DES_IV_LENGTH) {
        return _ERRNO_CRYPTO_IV_OutRange;
    }
    if (pResult == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (message == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (msglen == 0) {
        return 0;
    }
    if (msglen > CRYPTO_MAX_MSG_LENGTH) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }
    if ((msglen % CRYPTO_DES_IV_LENGTH) != 0) {
        return _ERRNO_CRYPTO_DES_MSGLEN_NOT_8Byte_Aligned;
    }

    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_cipher_decrypt(pcrypto_adapter, 
        CIPHER_TYPE_DES_CTR, message, msglen, iv, ivlen, NULL, 0, pResult, NULL);
#if CRYPTO_THREAD_MODE
    critical_leave();
#endif
    return ret;
}

// 3DES-CBC
int hal_crypto_3des_cbc_init(IN const u8 *key, IN const u32 keylen)
{
    int ret;
#if CRYPTO_THREAD_MODE
    critical_enter();
#endif
    hal_crypto_adapter_t *pcrypto_adapter = &g_rtl_cryptoEngine_s;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (key == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (check_4byte_align((size_t)(key))) {
        return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
    }
    if (keylen != CRYPTO_3DES_KEY_LENGTH) {
        return _ERRNO_CRYPTO_KEY_OutRange;
    }

    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_cipher_init(pcrypto_adapter, CIPHER_TYPE_3DES_CBC, key, keylen);
    return ret;
}

int hal_crypto_3des_cbc_encrypt(
    IN const u8 *message,   IN const u32 msglen,
    IN const u8 *iv,        IN const u32 ivlen,
    OUT u8 *pResult)
{
    int ret;
    hal_crypto_adapter_t *pcrypto_adapter = &g_rtl_cryptoEngine_s;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (iv == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (check_4byte_align((size_t)(iv))) {
        return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
    }
    if (ivlen != CRYPTO_DES_IV_LENGTH) {
        return _ERRNO_CRYPTO_IV_OutRange;
    }
    if (pResult == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (message == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (msglen == 0) {
        return 0;
    }
    if (msglen > CRYPTO_MAX_MSG_LENGTH) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }
    if ((msglen % CRYPTO_DES_IV_LENGTH) != 0) {
        return _ERRNO_CRYPTO_DES_MSGLEN_NOT_8Byte_Aligned;
    }

    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_cipher_encrypt(pcrypto_adapter, 
        CIPHER_TYPE_3DES_CBC, message, msglen, iv, ivlen, NULL, 0, pResult, NULL);
#if CRYPTO_THREAD_MODE
    critical_leave();
#endif
    return ret;
}

int hal_crypto_3des_cbc_decrypt(
    IN const u8 *message,   IN const u32 msglen,
    IN const u8 *iv,        IN const u32 ivlen,
    OUT u8 *pResult)
{
    int ret;
    hal_crypto_adapter_t *pcrypto_adapter = &g_rtl_cryptoEngine_s;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (iv == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (check_4byte_align((size_t)(iv))) {
        return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
    }
    if (ivlen != CRYPTO_DES_IV_LENGTH) {
        return _ERRNO_CRYPTO_IV_OutRange;
    }
    if (pResult == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (message == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (msglen == 0) {
        return 0;
    }
    if (msglen > CRYPTO_MAX_MSG_LENGTH) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }
    if ((msglen % CRYPTO_DES_IV_LENGTH) != 0) {
        return _ERRNO_CRYPTO_DES_MSGLEN_NOT_8Byte_Aligned;
    }

    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_cipher_decrypt(pcrypto_adapter, 
        CIPHER_TYPE_3DES_CBC, message, msglen, iv, ivlen, NULL, 0, pResult, NULL);
#if CRYPTO_THREAD_MODE
    critical_leave();
#endif
    return ret;
}

// 3DES-ECB

int hal_crypto_3des_ecb_init(IN const u8 *key, IN const u32 keylen)
{
    int ret;
#if CRYPTO_THREAD_MODE
    critical_enter();
#endif
    hal_crypto_adapter_t *pcrypto_adapter = &g_rtl_cryptoEngine_s;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (key == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (check_4byte_align((size_t)(key))) {
        return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
    }
    if (keylen != CRYPTO_3DES_KEY_LENGTH) {
        return _ERRNO_CRYPTO_KEY_OutRange;
    }

    ret =  HAL_CRYPTO_FUNC_STUBS.hal_crypto_cipher_init(pcrypto_adapter, CIPHER_TYPE_3DES_ECB, key, keylen);
    return ret;
}

int hal_crypto_3des_ecb_encrypt(
    IN const u8 *message,   IN const u32 msglen,
    IN const u8 *iv,        IN const u32 ivlen,
    OUT u8 *pResult)
{
    int ret;

    hal_crypto_adapter_t *pcrypto_adapter = &g_rtl_cryptoEngine_s;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    iv = NULL;
    if (ivlen != 0) {
        return _ERRNO_CRYPTO_IV_OutRange;
    }
    if (pResult == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (message == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (msglen == 0) {
        return 0;
    }
    if (msglen > CRYPTO_MAX_MSG_LENGTH) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }
    if ((msglen % CRYPTO_DES_IV_LENGTH) != 0) {
        return _ERRNO_CRYPTO_DES_MSGLEN_NOT_8Byte_Aligned;
    }

    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_cipher_encrypt(pcrypto_adapter, 
        CIPHER_TYPE_3DES_ECB, message, msglen, iv, ivlen, NULL, 0, pResult, NULL);
#if CRYPTO_THREAD_MODE
    critical_leave();
#endif
    return ret;
}

int hal_crypto_3des_ecb_decrypt(
    IN const u8 *message,   IN const u32 msglen,
    IN const u8 *iv,        IN const u32 ivlen,
    OUT u8 *pResult)
{
    int ret;

    hal_crypto_adapter_t *pcrypto_adapter = &g_rtl_cryptoEngine_s;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    iv = NULL;
    if (ivlen != 0) {
        return _ERRNO_CRYPTO_IV_OutRange;
    }
    if (pResult == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (message == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (msglen == 0) {
        return 0;
    }
    if (msglen > CRYPTO_MAX_MSG_LENGTH) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }
    if ((msglen % CRYPTO_DES_IV_LENGTH) != 0) {
        return _ERRNO_CRYPTO_DES_MSGLEN_NOT_8Byte_Aligned;
    }

    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_cipher_decrypt(pcrypto_adapter, 
        CIPHER_TYPE_3DES_ECB, message, msglen, iv, ivlen, NULL, 0, pResult, NULL);
#if CRYPTO_THREAD_MODE
    critical_leave();
#endif
    return ret;
}

// 3DES-CFB
int hal_crypto_3des_cfb_init(IN const u8 *key, IN const u32 keylen)
{
    int ret;
#if CRYPTO_THREAD_MODE
    critical_enter();
#endif
    hal_crypto_adapter_t *pcrypto_adapter = &g_rtl_cryptoEngine_s;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (key == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (check_4byte_align((size_t)(key))) {
        return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
    }
    if (keylen != CRYPTO_3DES_KEY_LENGTH) {
        return _ERRNO_CRYPTO_KEY_OutRange;
    }

    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_cipher_init(pcrypto_adapter, CIPHER_TYPE_3DES_CFB, key, keylen);
    return ret;
}

int hal_crypto_3des_cfb_encrypt(
    IN const u8 *message,   IN const u32 msglen,
    IN const u8 *iv,        IN const u32 ivlen,
    OUT u8 *pResult)
{
    int ret;
    hal_crypto_adapter_t *pcrypto_adapter = &g_rtl_cryptoEngine_s;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (iv == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (check_4byte_align((size_t)(iv))) {
        return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
    }
    if (ivlen != CRYPTO_DES_IV_LENGTH) {
        return _ERRNO_CRYPTO_IV_OutRange;
    }
    if (pResult == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (message == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (msglen == 0) {
        return 0;
    }
    if (msglen > CRYPTO_MAX_MSG_LENGTH) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }
    if ((msglen % CRYPTO_DES_IV_LENGTH) != 0) {
        return _ERRNO_CRYPTO_DES_MSGLEN_NOT_8Byte_Aligned;
    }

    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_cipher_encrypt(pcrypto_adapter, 
        CIPHER_TYPE_3DES_CFB, message, msglen, iv, ivlen, NULL, 0, pResult, NULL);
#if CRYPTO_THREAD_MODE
    critical_leave();
#endif
    return ret;
}

int hal_crypto_3des_cfb_decrypt(
    IN const u8 *message,   IN const u32 msglen,
    IN const u8 *iv,        IN const u32 ivlen,
    OUT u8 *pResult)
{
    int ret;
    hal_crypto_adapter_t *pcrypto_adapter = &g_rtl_cryptoEngine_s;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (iv == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (check_4byte_align((size_t)(iv))) {
        return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
    }
    if (ivlen != CRYPTO_DES_IV_LENGTH) {
        return _ERRNO_CRYPTO_IV_OutRange;
    }
    if (pResult == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (message == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (msglen == 0) {
        return 0;
    }
    if (msglen > CRYPTO_MAX_MSG_LENGTH) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }
    if ((msglen % CRYPTO_DES_IV_LENGTH) != 0) {
        return _ERRNO_CRYPTO_DES_MSGLEN_NOT_8Byte_Aligned;
    }

    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_cipher_decrypt(pcrypto_adapter, 
        CIPHER_TYPE_3DES_CFB, message, msglen, iv, ivlen, NULL, 0, pResult, NULL);
#if CRYPTO_THREAD_MODE
    critical_leave();
#endif
    return ret;
}

// 3DES-OFB
int hal_crypto_3des_ofb_init(IN const u8 *key, IN const u32 keylen)
{
    int ret;
#if CRYPTO_THREAD_MODE
    critical_enter();
#endif
    hal_crypto_adapter_t *pcrypto_adapter = &g_rtl_cryptoEngine_s;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (key == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (check_4byte_align((size_t)(key))) {
        return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
    }
    if (keylen != CRYPTO_3DES_KEY_LENGTH) {
        return _ERRNO_CRYPTO_KEY_OutRange;
    }

    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_cipher_init(pcrypto_adapter, CIPHER_TYPE_3DES_OFB, key, keylen);
    return ret;
}

int hal_crypto_3des_ofb_encrypt(
    IN const u8 *message,   IN const u32 msglen,
    IN const u8 *iv,        IN const u32 ivlen,
    OUT u8 *pResult)
{
    int ret;
    hal_crypto_adapter_t *pcrypto_adapter = &g_rtl_cryptoEngine_s;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (iv == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (check_4byte_align((size_t)(iv))) {
        return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
    }
    if (ivlen != CRYPTO_DES_IV_LENGTH) {
        return _ERRNO_CRYPTO_IV_OutRange;
    }
    if (pResult == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (message == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (msglen == 0) {
        return 0;
    }
    if (msglen > CRYPTO_MAX_MSG_LENGTH) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }
    if ((msglen % CRYPTO_DES_IV_LENGTH) != 0) {
        return _ERRNO_CRYPTO_DES_MSGLEN_NOT_8Byte_Aligned;
    }

    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_cipher_encrypt(pcrypto_adapter, 
        CIPHER_TYPE_3DES_OFB, message, msglen, iv, ivlen, NULL, 0, pResult, NULL);
#if CRYPTO_THREAD_MODE
    critical_leave();
#endif
    return ret;
}

int hal_crypto_3des_ofb_decrypt(
    IN const u8 *message,   IN const u32 msglen,
    IN const u8 *iv,        IN const u32 ivlen,
    OUT u8 *pResult)
{
    int ret;
    hal_crypto_adapter_t *pcrypto_adapter = &g_rtl_cryptoEngine_s;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (iv == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (check_4byte_align((size_t)(iv))) {
        return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
    }
    if (ivlen != CRYPTO_DES_IV_LENGTH) {
        return _ERRNO_CRYPTO_IV_OutRange;
    }
    if (pResult == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (message == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (msglen == 0) {
        return 0;
    }
    if (msglen > CRYPTO_MAX_MSG_LENGTH) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }
    if ((msglen % CRYPTO_DES_IV_LENGTH) != 0) {
        return _ERRNO_CRYPTO_DES_MSGLEN_NOT_8Byte_Aligned;
    }

    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_cipher_decrypt(pcrypto_adapter, 
        CIPHER_TYPE_3DES_OFB, message, msglen, iv, ivlen, NULL, 0, pResult, NULL);
#if CRYPTO_THREAD_MODE
    critical_leave();
#endif
    return ret;
}

// 3DES-CTR
int hal_crypto_3des_ctr_init(IN const u8 *key, IN const u32 keylen)
{
    int ret;
#if CRYPTO_THREAD_MODE
    critical_enter();
#endif
    hal_crypto_adapter_t *pcrypto_adapter = &g_rtl_cryptoEngine_s;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (key == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (check_4byte_align((size_t)(key))) {
        return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
    }
    if (keylen != CRYPTO_3DES_KEY_LENGTH) {
        return _ERRNO_CRYPTO_KEY_OutRange;
    }

    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_cipher_init(pcrypto_adapter, CIPHER_TYPE_3DES_CTR, key, keylen);
    return ret;
}

int hal_crypto_3des_ctr_encrypt(
    IN const u8 *message,   IN const u32 msglen,
    IN const u8 *iv,        IN const u32 ivlen,
    OUT u8 *pResult)
{
    int ret;
    hal_crypto_adapter_t *pcrypto_adapter = &g_rtl_cryptoEngine_s;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (iv == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (check_4byte_align((size_t)(iv))) {
        return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
    }
    if (ivlen != CRYPTO_DES_IV_LENGTH) {
        return _ERRNO_CRYPTO_IV_OutRange;
    }
    if (pResult == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (message == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (msglen == 0) {
        return 0;
    }
    if (msglen > CRYPTO_MAX_MSG_LENGTH) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }
    if ((msglen % CRYPTO_DES_IV_LENGTH) != 0) {
        return _ERRNO_CRYPTO_DES_MSGLEN_NOT_8Byte_Aligned;
    }

    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_cipher_encrypt(pcrypto_adapter, 
        CIPHER_TYPE_3DES_CTR, message, msglen, iv, ivlen, NULL, 0, pResult, NULL);
#if CRYPTO_THREAD_MODE
    critical_leave();
#endif
    return ret;
}

int hal_crypto_3des_ctr_decrypt(
    IN const u8 *message,   IN const u32 msglen,
    IN const u8 *iv,        IN const u32 ivlen,
    OUT u8 *pResult)
{
    int ret;
    hal_crypto_adapter_t *pcrypto_adapter = &g_rtl_cryptoEngine_s;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (iv == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (check_4byte_align((size_t)(iv))) {
        return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
    }
    if (ivlen != CRYPTO_DES_IV_LENGTH) {
        return _ERRNO_CRYPTO_IV_OutRange;
    }
    if (pResult == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (message == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (msglen == 0) {
        return 0;
    }
    if (msglen > CRYPTO_MAX_MSG_LENGTH) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }
    if ((msglen % CRYPTO_DES_IV_LENGTH) != 0) {
        return _ERRNO_CRYPTO_DES_MSGLEN_NOT_8Byte_Aligned;
    }

    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_cipher_decrypt(pcrypto_adapter, 
        CIPHER_TYPE_3DES_CTR, message, msglen, iv, ivlen, NULL, 0, pResult, NULL);
#if CRYPTO_THREAD_MODE
    critical_leave();
#endif
    return ret;
}

// AES-CBC

int hal_crypto_aes_cbc_init(IN const u8 *key, IN const u32 keylen)
{
    int ret;
#if CRYPTO_THREAD_MODE
    critical_enter();
#endif
    hal_crypto_adapter_t *pcrypto_adapter = &HAL_CRYPTO_ADAPTER;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (key == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (check_4byte_align((size_t)(key))) {
        return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
    }
    if ((keylen != CRYPTO_AES128_KEY_LENGTH) && (keylen != CRYPTO_AES192_KEY_LENGTH) &&
         (keylen != CRYPTO_AES256_KEY_LENGTH)) {
         return _ERRNO_CRYPTO_KEY_OutRange;
    }

    ret =  HAL_CRYPTO_FUNC_STUBS.hal_crypto_cipher_init(pcrypto_adapter, CIPHER_TYPE_AES_CBC, key, keylen);
    return ret;
}

int hal_crypto_aes_cbc_encrypt(
    IN const u8 *message, 	IN const u32 msglen,
    IN const u8 *iv, 		IN const u32 ivlen,
    OUT u8 *pResult)
{
    int ret;
    hal_crypto_adapter_t *pcrypto_adapter = &HAL_CRYPTO_ADAPTER;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (iv == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (check_4byte_align((size_t)(iv))) {
        return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
    }
    if (ivlen != CRYPTO_AES_IV_LENGTH) {
        return _ERRNO_CRYPTO_IV_OutRange;
    }
    if (pResult == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (message == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (msglen == 0) {
        return 0;
    }
    if (msglen > CRYPTO_MAX_MSG_LENGTH) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }
    if ((msglen % CRYPTO_AES_IV_LENGTH) != 0) {
        return _ERRNO_CRYPTO_AES_MSGLEN_NOT_16Byte_Aligned;
    }

    ret =  HAL_CRYPTO_FUNC_STUBS.hal_crypto_cipher_encrypt(pcrypto_adapter, CIPHER_TYPE_AES_CBC, message, msglen, iv, ivlen,
                                                           NULL, 0, pResult, NULL);
#if CRYPTO_THREAD_MODE
    critical_leave();
#endif
    return ret;
}

int hal_crypto_aes_cbc_decrypt(
    IN const u8 *message, 	IN const u32 msglen,
    IN const u8 *iv, 		IN const u32 ivlen,
    OUT u8 *pResult)
{
    int ret;
    hal_crypto_adapter_t *pcrypto_adapter = &HAL_CRYPTO_ADAPTER;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (iv == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (check_4byte_align((size_t)(iv))) {
        return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
    }
    if (ivlen != CRYPTO_AES_IV_LENGTH) {
        return _ERRNO_CRYPTO_IV_OutRange;
    }
    if (pResult == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (message == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (msglen == 0) {
        return 0;
    }
    if (msglen > CRYPTO_MAX_MSG_LENGTH) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }
    if ((msglen % CRYPTO_AES_IV_LENGTH) != 0) {
        return _ERRNO_CRYPTO_AES_MSGLEN_NOT_16Byte_Aligned;
    }

    ret =  HAL_CRYPTO_FUNC_STUBS.hal_crypto_cipher_decrypt(pcrypto_adapter, CIPHER_TYPE_AES_CBC, message, msglen, iv, ivlen,
                                                           NULL, 0, pResult, NULL);
#if CRYPTO_THREAD_MODE
    critical_leave();
#endif
    return ret;
}

// AES-ECB

int hal_crypto_aes_ecb_init(IN const u8 *key, IN const u32 keylen)
{
    int ret;
#if CRYPTO_THREAD_MODE
    critical_enter();
#endif
    hal_crypto_adapter_t *pcrypto_adapter = &HAL_CRYPTO_ADAPTER;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (key == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (check_4byte_align((size_t)(key))) {
        return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
    }
    if ((keylen != CRYPTO_AES128_KEY_LENGTH) && (keylen != CRYPTO_AES192_KEY_LENGTH) &&
        (keylen != CRYPTO_AES256_KEY_LENGTH)) {
         return _ERRNO_CRYPTO_KEY_OutRange;
    }

    ret =  HAL_CRYPTO_FUNC_STUBS.hal_crypto_cipher_init(pcrypto_adapter, CIPHER_TYPE_AES_ECB, key, keylen);
    return ret;
}

int hal_crypto_aes_ecb_encrypt(
    IN const u8 *message, 	IN const u32 msglen,
    IN const u8 *iv, 		IN const u32 ivlen,
    OUT u8 *pResult)
{
    int ret;
    hal_crypto_adapter_t *pcrypto_adapter = &HAL_CRYPTO_ADAPTER;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    iv = NULL;
    if (ivlen != 0) {
        return _ERRNO_CRYPTO_IV_OutRange;
    }
    if (pResult == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (message == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (msglen == 0) {
        return 0;
    }
    if (msglen > CRYPTO_MAX_MSG_LENGTH) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }
    if ((msglen % CRYPTO_AES_IV_LENGTH) != 0) {
        return _ERRNO_CRYPTO_AES_MSGLEN_NOT_16Byte_Aligned;
    }

    ret =  HAL_CRYPTO_FUNC_STUBS.hal_crypto_cipher_encrypt(pcrypto_adapter, CIPHER_TYPE_AES_ECB, message, msglen, iv, ivlen,
                                                           NULL, 0, pResult, NULL);
#if CRYPTO_THREAD_MODE
    critical_leave();
#endif
    return ret;
}

int hal_crypto_aes_ecb_decrypt(
    IN const u8 *message, 	IN const u32 msglen,
    IN const u8 *iv, 		IN const u32 ivlen,
    OUT u8 *pResult)
{
    int ret;
    hal_crypto_adapter_t *pcrypto_adapter = &HAL_CRYPTO_ADAPTER;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    iv = NULL;
    if (ivlen != 0) {
        return _ERRNO_CRYPTO_IV_OutRange;
    }
    if (pResult == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (message == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (msglen == 0) {
        return 0;
    }
    if (msglen > CRYPTO_MAX_MSG_LENGTH) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }
    if ((msglen % CRYPTO_AES_IV_LENGTH) != 0) {
        return _ERRNO_CRYPTO_AES_MSGLEN_NOT_16Byte_Aligned;
    }

    ret =  HAL_CRYPTO_FUNC_STUBS.hal_crypto_cipher_decrypt(pcrypto_adapter, CIPHER_TYPE_AES_ECB, message, msglen, iv, ivlen,
                                                           NULL, 0, pResult, NULL);
#if CRYPTO_THREAD_MODE
    critical_leave();
#endif
    return ret;
}

// AES-CTR

int hal_crypto_aes_ctr_init(IN const u8 *key, IN const u32 keylen)
{
    int ret;
#if CRYPTO_THREAD_MODE
    critical_enter();
#endif
    hal_crypto_adapter_t *pcrypto_adapter = &HAL_CRYPTO_ADAPTER;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (key == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (check_4byte_align((size_t)(key))) {
        return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
    }
    if ((keylen != CRYPTO_AES128_KEY_LENGTH) && (keylen != CRYPTO_AES192_KEY_LENGTH) &&
        (keylen != CRYPTO_AES256_KEY_LENGTH)) {
        return _ERRNO_CRYPTO_KEY_OutRange;
    }

    ret =  HAL_CRYPTO_FUNC_STUBS.hal_crypto_cipher_init(pcrypto_adapter, CIPHER_TYPE_AES_CTR, key, keylen);
    return ret;
}

int hal_crypto_aes_ctr_encrypt(
    IN const u8 *message, 	IN const u32 msglen,
    IN const u8 *iv, 		IN const u32 ivlen,
    OUT u8 *pResult)
{
    int ret;
    hal_crypto_adapter_t *pcrypto_adapter = &HAL_CRYPTO_ADAPTER;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (iv == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (check_4byte_align((size_t)(iv))) {
        return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
    }
    if (ivlen != CRYPTO_AES_IV_LENGTH) {
        return _ERRNO_CRYPTO_IV_OutRange;
    }
    if (pResult == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (message == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (msglen == 0) {
        return 0;
    }
    if (msglen > CRYPTO_MAX_MSG_LENGTH) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }
    if ((msglen % CRYPTO_AES_IV_LENGTH) != 0) {
        return _ERRNO_CRYPTO_AES_MSGLEN_NOT_16Byte_Aligned;
    }

    ret =  HAL_CRYPTO_FUNC_STUBS.hal_crypto_cipher_encrypt(pcrypto_adapter, CIPHER_TYPE_AES_CTR, message, msglen, iv, ivlen,
                                                           NULL, 0, pResult, NULL);
#if CRYPTO_THREAD_MODE
    critical_leave();
#endif
    return ret;
}

int hal_crypto_aes_ctr_decrypt(
    IN const u8 *message, 	IN const u32 msglen,
    IN const u8 *iv, 		IN const u32 ivlen,
    OUT u8 *pResult)
{
    int ret;
    hal_crypto_adapter_t *pcrypto_adapter = &HAL_CRYPTO_ADAPTER;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (iv == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (check_4byte_align((size_t)(iv))) {
        return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
    }
    if (ivlen != CRYPTO_AES_IV_LENGTH) {
        return _ERRNO_CRYPTO_IV_OutRange;
    }
    if (pResult == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (message == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (msglen == 0) {
        return 0;
    }
    if (msglen > CRYPTO_MAX_MSG_LENGTH) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }
    if ((msglen % CRYPTO_AES_IV_LENGTH) != 0) {
        return _ERRNO_CRYPTO_AES_MSGLEN_NOT_16Byte_Aligned;
    }

    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_cipher_decrypt(pcrypto_adapter, CIPHER_TYPE_AES_CTR, message, msglen, iv, ivlen,
                                                          NULL, 0, pResult, NULL);
#if CRYPTO_THREAD_MODE
    critical_leave();
#endif
    return ret;
}

// AES-CFB

int hal_crypto_aes_cfb_init(IN const u8 *key, IN const u32 keylen)
{
    int ret;
#if CRYPTO_THREAD_MODE
    critical_enter();
#endif
    hal_crypto_adapter_t *pcrypto_adapter = &HAL_CRYPTO_ADAPTER;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (key == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (check_4byte_align((size_t)(key))) {
        return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
    }
    if ((keylen != CRYPTO_AES128_KEY_LENGTH) && (keylen != CRYPTO_AES192_KEY_LENGTH) &&
        (keylen != CRYPTO_AES256_KEY_LENGTH)) {
        return _ERRNO_CRYPTO_KEY_OutRange;
    }

    ret =  HAL_CRYPTO_FUNC_STUBS.hal_crypto_cipher_init(pcrypto_adapter, CIPHER_TYPE_AES_CFB, key, keylen);
    return ret;
}

int hal_crypto_aes_cfb_encrypt(
    IN const u8 *message, 	IN const u32 msglen,
    IN const u8 *iv, 		IN const u32 ivlen,
    OUT u8 *pResult)
{
    int ret;
    hal_crypto_adapter_t *pcrypto_adapter = &HAL_CRYPTO_ADAPTER;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (iv == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (check_4byte_align((size_t)(iv))) {
        return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
    }
    if (ivlen != CRYPTO_AES_IV_LENGTH) {
        return _ERRNO_CRYPTO_IV_OutRange;
    }
    if (pResult == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (message == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (msglen == 0) {
        return 0;
    }
    if (msglen > CRYPTO_MAX_MSG_LENGTH) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }
    if ((msglen % CRYPTO_AES_IV_LENGTH) != 0) {
        return _ERRNO_CRYPTO_AES_MSGLEN_NOT_16Byte_Aligned;
    }

    ret =  HAL_CRYPTO_FUNC_STUBS.hal_crypto_cipher_encrypt(pcrypto_adapter, CIPHER_TYPE_AES_CFB, message, msglen, iv, ivlen,
                                                           NULL, 0, pResult, NULL);
#if CRYPTO_THREAD_MODE
    critical_leave();
#endif
    return ret;
}

int hal_crypto_aes_cfb_decrypt(
    IN const u8 *message, 	IN const u32 msglen,
    IN const u8 *iv, 		IN const u32 ivlen,
    OUT u8 *pResult)
{
    int ret;
    hal_crypto_adapter_t *pcrypto_adapter = &HAL_CRYPTO_ADAPTER;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (iv == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (check_4byte_align((size_t)(iv))) {
        return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
    }
    if (ivlen != CRYPTO_AES_IV_LENGTH) {
        return _ERRNO_CRYPTO_IV_OutRange;
    }
    if (pResult == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (message == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (msglen == 0) {
        return 0;
    }
    if (msglen > CRYPTO_MAX_MSG_LENGTH) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }
    if ((msglen % CRYPTO_AES_IV_LENGTH) != 0) {
        return _ERRNO_CRYPTO_AES_MSGLEN_NOT_16Byte_Aligned;
    }

    ret =  HAL_CRYPTO_FUNC_STUBS.hal_crypto_cipher_decrypt(pcrypto_adapter, CIPHER_TYPE_AES_CFB, message, msglen, iv, ivlen,
                                                           NULL, 0, pResult, NULL);
#if CRYPTO_THREAD_MODE
    critical_leave();
#endif
    return ret;
}

// AES-OFB

int hal_crypto_aes_ofb_init(IN const u8 *key, IN const u32 keylen)
{
    int ret;
#if CRYPTO_THREAD_MODE
    critical_enter();
#endif
    hal_crypto_adapter_t *pcrypto_adapter = &HAL_CRYPTO_ADAPTER;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (key == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (check_4byte_align((size_t)(key))) {
        return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
    }
    if ((keylen != CRYPTO_AES128_KEY_LENGTH) && (keylen != CRYPTO_AES192_KEY_LENGTH) &&
        (keylen != CRYPTO_AES256_KEY_LENGTH)) {
        return _ERRNO_CRYPTO_KEY_OutRange;
    }

    ret =  HAL_CRYPTO_FUNC_STUBS.hal_crypto_cipher_init(pcrypto_adapter, CIPHER_TYPE_AES_OFB, key, keylen);
    return ret;
}

int hal_crypto_aes_ofb_encrypt(
    IN const u8 *message, 	IN const u32 msglen,
    IN const u8 *iv, 		IN const u32 ivlen,
    OUT u8 *pResult)
{
    int ret;
    hal_crypto_adapter_t *pcrypto_adapter = &HAL_CRYPTO_ADAPTER;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (iv == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (check_4byte_align((size_t)(iv))) {
        return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
    }
    if (ivlen != CRYPTO_AES_IV_LENGTH) {
        return _ERRNO_CRYPTO_IV_OutRange;
    }
    if (pResult == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (message == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (msglen == 0) {
        return 0;
    }
    if (msglen > CRYPTO_MAX_MSG_LENGTH) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }
    if ((msglen % CRYPTO_AES_IV_LENGTH) != 0) {
        return _ERRNO_CRYPTO_AES_MSGLEN_NOT_16Byte_Aligned;
    }

    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_cipher_encrypt(pcrypto_adapter, CIPHER_TYPE_AES_OFB, message, msglen, iv, ivlen,
                                                          NULL, 0, pResult, NULL);
#if CRYPTO_THREAD_MODE
    critical_leave();
#endif
    return ret;
}

int hal_crypto_aes_ofb_decrypt(
    IN const u8 *message, 	IN const u32 msglen,
    IN const u8 *iv, 		IN const u32 ivlen,
    OUT u8 *pResult)
{
    int ret;
    hal_crypto_adapter_t *pcrypto_adapter = &HAL_CRYPTO_ADAPTER;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (iv == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (check_4byte_align((size_t)(iv))) {
        return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
    }
    if (ivlen != CRYPTO_AES_IV_LENGTH) {
        return _ERRNO_CRYPTO_IV_OutRange;
    }
    if (pResult == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (message == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (msglen == 0) {
        return 0;
    }
    if (msglen > CRYPTO_MAX_MSG_LENGTH) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }
    if ((msglen % CRYPTO_AES_IV_LENGTH) != 0) {
        return _ERRNO_CRYPTO_AES_MSGLEN_NOT_16Byte_Aligned;
    }

    ret =  HAL_CRYPTO_FUNC_STUBS.hal_crypto_cipher_decrypt(pcrypto_adapter, CIPHER_TYPE_AES_OFB, message, msglen, iv, ivlen,
                                                           NULL, 0, pResult, NULL);
#if CRYPTO_THREAD_MODE
    critical_leave();
#endif
    return ret;
}

//AES GHASH

int hal_crypto_aes_ghash(
    IN const u8 *message, IN const u32 msglen,
    IN const u8 *key, IN const u32 keylen,
    OUT u8 *pDigest)
{
    int ret;
#if CRYPTO_THREAD_MODE
    critical_enter();
#endif
    hal_crypto_adapter_t *pcrypto_adapter = &HAL_CRYPTO_ADAPTER;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (key == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (check_4byte_align((size_t)(key))) {
        return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
    }
    if (keylen != CRYPTO_AES128_KEY_LENGTH) {
         return _ERRNO_CRYPTO_KEY_OutRange;
    }
    if (pDigest == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (message == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (msglen == 0) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }
    if (msglen > CRYPTO_MAX_MSG_LENGTH) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }

    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_cipher_init(pcrypto_adapter, CIPHER_TYPE_AES_GHASH, key, keylen);
    if (ret != SUCCESS) {
        return ret;
    }
    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_cipher_decrypt(pcrypto_adapter, CIPHER_TYPE_AES_GHASH, message, msglen,
                                                       NULL, 0, NULL, 0, NULL, pDigest);
    
#if CRYPTO_THREAD_MODE
    critical_leave();
#endif
    return ret;
}

int hal_crypto_aes_ghash_init(IN const u8 *key, IN const u32 keylen)
{
    int ret;
#if CRYPTO_THREAD_MODE
    critical_enter();
#endif
    hal_crypto_adapter_t *pcrypto_adapter = &HAL_CRYPTO_ADAPTER;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (key == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (check_4byte_align((size_t)(key))) {
        return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
    }
    if (keylen != CRYPTO_AES128_KEY_LENGTH) {
         return _ERRNO_CRYPTO_KEY_OutRange;
    }

    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_cipher_init(pcrypto_adapter, CIPHER_TYPE_AES_GHASH, key, keylen);

    return ret;
}

int hal_crypto_aes_ghash_process(IN const u8 *message, IN const u32 msglen, OUT u8 *pDigest)
{
    int ret;
    hal_crypto_adapter_t *pcrypto_adapter = &HAL_CRYPTO_ADAPTER;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (pDigest == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (message == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (msglen == 0) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }
    if (msglen > CRYPTO_MAX_MSG_LENGTH) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }

    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_cipher_decrypt(pcrypto_adapter, CIPHER_TYPE_AES_GHASH, message, msglen,
                                                       NULL, 0, NULL, 0, NULL, pDigest);
#if CRYPTO_THREAD_MODE
    critical_leave();
#endif
    return ret;
}

//AES GMAC

int hal_crypto_aes_gmac(
    IN const u8 *message, IN const u32 msglen,
    IN const u8 *key, IN const u32 keylen,
    IN const u8 *iv,
    IN const u8 *aad, IN const u32 aadlen, OUT u8 *pTag)
{
    int ret;
#if CRYPTO_THREAD_MODE
    critical_enter();
#endif
    hal_crypto_adapter_t *pcrypto_adapter = &HAL_CRYPTO_ADAPTER;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (key == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (check_4byte_align((size_t)(key))) {
        return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
    }
    if ((keylen != CRYPTO_AES128_KEY_LENGTH) && (keylen != CRYPTO_AES192_KEY_LENGTH) &&
        (keylen != CRYPTO_AES256_KEY_LENGTH)) {
        return _ERRNO_CRYPTO_KEY_OutRange;
    }
    if (iv == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (check_4byte_align((size_t)(iv))) {
        return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
    }
    if (check_4byte_align((size_t)(aad))) {
        return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
    }
    if (aadlen > CRYPTO_MAX_AAD_LENGTH) {
        return _ERRNO_CRYPTO_AAD_OutRange;
    }
    if (pTag == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (message == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (msglen == 0) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }
    if (msglen > CRYPTO_MAX_MSG_LENGTH) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }

    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_cipher_init(pcrypto_adapter, CIPHER_TYPE_AES_GMAC, key, keylen);
    if (ret != SUCCESS) {
        return ret;
    }
    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_cipher_decrypt(pcrypto_adapter, CIPHER_TYPE_AES_GMAC, message, msglen,
                                                       iv, 0, aad, aadlen, NULL, pTag);
    
#if CRYPTO_THREAD_MODE
    critical_leave();
#endif
    return ret;
}

int hal_crypto_aes_gmac_init(IN const u8 *key, IN const u32 keylen)
{
    int ret;
#if CRYPTO_THREAD_MODE
    critical_enter();
#endif
    hal_crypto_adapter_t *pcrypto_adapter = &HAL_CRYPTO_ADAPTER;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (key == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (check_4byte_align((size_t)(key))) {
        return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
    }
    if ((keylen != CRYPTO_AES128_KEY_LENGTH) && (keylen != CRYPTO_AES192_KEY_LENGTH) &&
        (keylen != CRYPTO_AES256_KEY_LENGTH)) {
        return _ERRNO_CRYPTO_KEY_OutRange;
    }

    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_cipher_init(pcrypto_adapter, CIPHER_TYPE_AES_GMAC, key, keylen);

    return ret;
}

int hal_crypto_aes_gmac_process(
    IN const u8 *message, IN const u32 msglen,
    IN const u8 *iv, IN const u8 *aad, IN const u32 aadlen, OUT u8 *pTag)
{
    int ret;
    hal_crypto_adapter_t *pcrypto_adapter = &HAL_CRYPTO_ADAPTER;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (iv == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (check_4byte_align((size_t)(iv))) {
        return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
    }
    if (check_4byte_align((size_t)(aad))) {
        return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
    }
    if (aadlen > CRYPTO_MAX_AAD_LENGTH) {
        return _ERRNO_CRYPTO_AAD_OutRange;
    }
    if (pTag == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (message == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (msglen == 0) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }
    if (msglen > CRYPTO_MAX_MSG_LENGTH) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }

    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_cipher_decrypt(pcrypto_adapter, CIPHER_TYPE_AES_GMAC, message, msglen,
                                                       iv, 0, aad, aadlen, NULL, pTag);
#if CRYPTO_THREAD_MODE
    critical_leave();
#endif
    return ret;
}

//AES-GCTR

int hal_crypto_aes_gctr_init(IN const u8 *key, IN const u32 keylen)
{
    int ret;
#if CRYPTO_THREAD_MODE
    critical_enter();
#endif
    hal_crypto_adapter_t *pcrypto_adapter = &HAL_CRYPTO_ADAPTER;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (key == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (check_4byte_align((size_t)(key))) {
        return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
    }
    if ((keylen != CRYPTO_AES128_KEY_LENGTH) && (keylen != CRYPTO_AES192_KEY_LENGTH) &&
        (keylen != CRYPTO_AES256_KEY_LENGTH)) {
        return _ERRNO_CRYPTO_KEY_OutRange;
    }

    ret =  HAL_CRYPTO_FUNC_STUBS.hal_crypto_cipher_init(pcrypto_adapter, CIPHER_TYPE_AES_GCTR, key, keylen);
    return ret;
}

int hal_crypto_aes_gctr_encrypt(
    IN const u8 *message, 	IN const u32 msglen,
    IN const u8 *iv, OUT u8 *pResult)
{
    int ret;
    hal_crypto_adapter_t *pcrypto_adapter = &HAL_CRYPTO_ADAPTER;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (iv == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (check_4byte_align((size_t)(iv))) {
        return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
    }
    if (pResult == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (message == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (msglen == 0) {
        return 0;
    }
    if (msglen > CRYPTO_MAX_MSG_LENGTH) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }
    if ((msglen % CRYPTO_AES_IV_LENGTH) != 0) {
        return _ERRNO_CRYPTO_AES_MSGLEN_NOT_16Byte_Aligned;
    }

    ret =  HAL_CRYPTO_FUNC_STUBS.hal_crypto_cipher_encrypt(pcrypto_adapter, CIPHER_TYPE_AES_GCTR, message, msglen, iv, 0,
                                                           NULL, 0, pResult, NULL);
#if CRYPTO_THREAD_MODE
    critical_leave();
#endif
    return ret;
}

int hal_crypto_aes_gctr_decrypt(
    IN const u8 *message, 	IN const u32 msglen,
    IN const u8 *iv, OUT u8 *pResult)
{
    int ret;
    hal_crypto_adapter_t *pcrypto_adapter = &HAL_CRYPTO_ADAPTER;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (iv == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (check_4byte_align((size_t)(iv))) {
        return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
    }
    if (pResult == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (message == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (msglen == 0) {
        return 0;
    }
    if (msglen > CRYPTO_MAX_MSG_LENGTH) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }
    if ((msglen % CRYPTO_AES_IV_LENGTH) != 0) {
        return _ERRNO_CRYPTO_AES_MSGLEN_NOT_16Byte_Aligned;
    }

    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_cipher_decrypt(pcrypto_adapter, CIPHER_TYPE_AES_GCTR, message, msglen, iv, 0,
                                                          NULL, 0, pResult, NULL);
#if CRYPTO_THREAD_MODE
    critical_leave();
#endif
    return ret;
}

// AES-GCM

int hal_crypto_aes_gcm_init(IN const u8 *key, IN const u32 keylen)
{
    int ret;
#if CRYPTO_THREAD_MODE
    critical_enter();
#endif
    hal_crypto_adapter_t *pcrypto_adapter = &HAL_CRYPTO_ADAPTER;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (key == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (check_4byte_align((size_t)(key))) {
        return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
    }
    if ((keylen != CRYPTO_AES128_KEY_LENGTH) && (keylen != CRYPTO_AES192_KEY_LENGTH) &&
        (keylen != CRYPTO_AES256_KEY_LENGTH)) {
        return _ERRNO_CRYPTO_KEY_OutRange;
    }

    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_cipher_init(pcrypto_adapter, CIPHER_TYPE_AES_GCM, key, keylen);
    return ret;
}

int hal_crypto_aes_gcm_encrypt(
    IN const u8 *message, 	IN const u32 msglen,
    IN const u8 *iv,
    IN const u8 *aad,		IN const u32 aadlen,
    OUT u8 *pResult, OUT u8 *pTag)
{
    int ret;
    hal_crypto_adapter_t *pcrypto_adapter = &HAL_CRYPTO_ADAPTER;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (iv == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (check_4byte_align((size_t)(iv))) {
        return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
    }
    if (check_4byte_align((size_t)(aad))) {
        return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
    }
    if (aadlen > CRYPTO_MAX_AAD_LENGTH) {
        return _ERRNO_CRYPTO_AAD_OutRange;
    }
    if (pTag == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (pResult == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (message == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (msglen == 0) {
        return 0;
    }
    if (msglen > CRYPTO_MAX_MSG_LENGTH) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }
    if ((msglen % CRYPTO_AES_IV_LENGTH) != 0) {
        return _ERRNO_CRYPTO_AES_MSGLEN_NOT_16Byte_Aligned;
    }

    ret =  HAL_CRYPTO_FUNC_STUBS.hal_crypto_cipher_encrypt(pcrypto_adapter, CIPHER_TYPE_AES_GCM, message, msglen, iv, 0,
                                                           aad, aadlen, pResult, pTag);
#if CRYPTO_THREAD_MODE
    critical_leave();
#endif
    return ret;
}


int hal_crypto_aes_gcm_decrypt(
    IN const u8 *message, 	IN const u32 msglen,
    IN const u8 *iv,
    IN const u8 *aad,		IN const u32 aadlen,
    OUT u8 *pResult, OUT u8 *pTag)
{
    int ret;
    hal_crypto_adapter_t *pcrypto_adapter = &HAL_CRYPTO_ADAPTER;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (iv == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (check_4byte_align((size_t)(iv))) {
        return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
    }
    if (check_4byte_align((size_t)(aad))) {
        return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
    }
    if (aadlen > CRYPTO_MAX_AAD_LENGTH) {
        return _ERRNO_CRYPTO_AAD_OutRange;
    }
    if (pTag == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (pResult == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (message == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (msglen == 0) {
        return 0;
    }
    if (msglen > CRYPTO_MAX_MSG_LENGTH) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }
    if ((msglen % CRYPTO_AES_IV_LENGTH) != 0) {
        return _ERRNO_CRYPTO_AES_MSGLEN_NOT_16Byte_Aligned;
    }

    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_cipher_decrypt(pcrypto_adapter, CIPHER_TYPE_AES_GCM, message, msglen, iv, 0,
                                                          aad, aadlen, pResult, pTag);
#if CRYPTO_THREAD_MODE
    critical_leave();
#endif
    return ret;
}

uint32_t hal_crypto_mix_mode_calculate_hash_padding(hal_crypto_adapter_t *pcrypto_adapter,
                                                    IN const u32 auth_type,IN const u32 msglen,IN const u32 aadlen)
{
    uint64_t hash_total_len;
    uint32_t low, high, last, padn, hash_padlen;
    uint8_t hash_msglen[8] = {0x0};

    /*
     * Calculate hash padding values and size
     */
    rtlc_memset((void *)hash_padding, 0x0, sizeof(hash_padding));
    if (pcrypto_adapter->isHMAC) {
        hash_total_len = (msglen + aadlen) + 64;
        //DBG_CRYPTO_INFO("hmac add 64bytes key pad\r\n");
    } else {
        hash_total_len = (msglen + aadlen);
    }

    /* Note:
     * AND 0x3F -> Know the last data size which need to be calculated.
     * Use the last data size to calculate how many bytes of padding.
     * Because there're reserved 8 bytes to represent total msglen in bits, so (64 - 8) = 56 or (64*2 - 8) = 120
     */
    last = ((msglen + aadlen) & 0x3F);
    padn = (last < 56) ? (56 - last) : (120 - last);
    rtlc_memcpy((void *)hash_padding, (void *)mix_mode_hash_padding, padn);

    /* Total hash msglen use 8bytes to represent!!!seperate low 4 bytes, high 4 bytes!!
     * Note: 
     * Because when store value to low, hash_total_len[31:0] need to left shift 3 bits(means 1byte to 8bits)
     * so if there are values in b'31:29, the 32bits low value can't store over 32 bits, so leave b'31:29 of 
     * hash_total_len[31:0] to high value.
     */
    high = ((hash_total_len & (0xFFFFFFFF)) >> 29)
         | (((hash_total_len >> 32) & (0xFFFFFFFF)) <<  3);
    low  = ((hash_total_len & (0xFFFFFFFF)) <<  3);
    if ((AUTH_TYPE_MD5 == auth_type) || (AUTH_TYPE_HMAC_MD5 == auth_type)) {
        HASH_PADDING_PUT_UINT32_LE(low,  hash_msglen, 0);
        HASH_PADDING_PUT_UINT32_LE(high, hash_msglen, 4);
    } else {
        HASH_PADDING_PUT_UINT32_BE(high, hash_msglen, 0);
        HASH_PADDING_PUT_UINT32_BE(low , hash_msglen, 4);
    }
    rtlc_memcpy((void *)(hash_padding + padn), (void *)hash_msglen, 8);
    hash_padlen = (padn + 8);

    return hash_padlen;
}

uint32_t hal_crypto_mix_mode_calculate_enc_padding(hal_crypto_adapter_t *pcrypto_adapter,
                                                   IN const u32 auth_type,IN const u32 msglen)
{
    uint32_t new_enc_last_data_size = 0,enc_padlen = 0, mac_len = 0;

    /*
     * Calculate enc padding size & set values
     */
    if ((AUTH_TYPE_MD5 == auth_type) || (AUTH_TYPE_HMAC_MD5 == auth_type)) {
        mac_len = CRYPTO_MD5_DIGEST_LENGTH;
    } else if ((AUTH_TYPE_SHA1 == auth_type) || (AUTH_TYPE_HMAC_SHA1 == auth_type)) {
        mac_len = CRYPTO_SHA1_DIGEST_LENGTH;
    } else if ((AUTH_TYPE_SHA2_224_ALL == auth_type) || (AUTH_TYPE_HMAC_SHA2_224_ALL == auth_type)) {
        mac_len = SHA2_224;
    } else if ((AUTH_TYPE_SHA2_256_ALL == auth_type) || (AUTH_TYPE_HMAC_SHA2_256_ALL == auth_type)) {
        mac_len = SHA2_256;
    }
    // AES block size is 16bytes
    // Even if (plaintext msglen + mac_len) is cipher aligned, it still needs to pad a pair data of cipher aligned
    // because of crypto engine degisn.
    new_enc_last_data_size = (msglen + mac_len) % 16;
    enc_padlen = (16 - new_enc_last_data_size) % 16;
    if (enc_padlen == 0) {
        enc_padlen = 16;
    }

    rtlc_memset((void *)enc_padding, 0x0F, sizeof(enc_padding));

    return enc_padlen;
}

int hal_crypto_mix_mode_init(IN const u32 cipher_type, IN const u32 auth_type, 
                             IN const u8 *cipher_key, IN const u32 cipher_keylen,
                             IN const u8 *auth_key, IN const u32 auth_keylen)
{
#if CRYPTO_THREAD_MODE
    critical_enter();
#endif
    int ret;
    hal_crypto_adapter_t *pcrypto_adapter = &HAL_CRYPTO_ADAPTER;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (cipher_key == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (check_4byte_align((size_t)(cipher_key))) {
        return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
    }
    // Ameba_zii only support AES algorithm
    if ((cipher_keylen != CRYPTO_AES128_KEY_LENGTH) && (cipher_keylen != CRYPTO_AES192_KEY_LENGTH) &&
        (cipher_keylen != CRYPTO_AES256_KEY_LENGTH)) {
        return _ERRNO_CRYPTO_KEY_OutRange;
    }
    if (auth_keylen != 0) {
        if (auth_keylen > CRYPTO_AUTH_PADDING) {
            return _ERRNO_CRYPTO_KEY_OutRange;
        }
        if (auth_key == NULL) {
            return _ERRNO_CRYPTO_NULL_POINTER;
        }
        if (check_4byte_align((size_t)(auth_key))) {
            return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
        }
    }

    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_mix_mode_init(pcrypto_adapter, cipher_type, auth_type, cipher_key, cipher_keylen,
                                                         auth_key, auth_keylen);
    return ret;
}

int hal_crypto_mix_ssh_encrypt(IN const u32 cipher_type,
                               IN const u8 *message, IN const u32 msglen,
                               IN const u8 *iv, IN const u32 ivlen,
                               IN const u8 *aad, IN const u32 aadlen,
                               IN const u32 auth_type,
                               OUT u8 *pResult, OUT u8 *pTag)
{
    int ret;
    uint32_t hash_padlen = 0;
    hal_crypto_adapter_t *pcrypto_adapter = &HAL_CRYPTO_ADAPTER;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (iv != NULL) {
        if (check_4byte_align((size_t)(iv))) {
            return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
        }
    }
    if (check_4byte_align((size_t)(aad))) {
        return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
    }
    if (aadlen > CRYPTO_MAX_AAD_LENGTH) {
        return _ERRNO_CRYPTO_AAD_OutRange;
    }
    if (pTag == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (pResult == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (message == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (msglen == 0) {
        return 0;
    }
    if (msglen > CRYPTO_MAX_MSG_LENGTH) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }
    if ((msglen % CRYPTO_AES_IV_LENGTH) != 0) {
        return _ERRNO_CRYPTO_AES_MSGLEN_NOT_16Byte_Aligned;
    }

    hash_padlen = hal_crypto_mix_mode_calculate_hash_padding(pcrypto_adapter,auth_type,msglen,aadlen);

    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_mix_mode_process(pcrypto_adapter, CRYPTO_MIX_MODE_SSH_ENC,
                                                            cipher_type, message, msglen, iv, ivlen,
                                                            aad, aadlen, auth_type, (const u8 *)hash_padding, hash_padlen,
                                                            NULL, 0, pResult, pTag);
#if CRYPTO_THREAD_MODE
    critical_leave();
#endif
    return ret;
}

int hal_crypto_mix_ssh_decrypt(IN const u32 cipher_type,
                               IN const u8 *message, IN const u32 msglen,
                               IN const u8 *iv, IN const u32 ivlen,
                               IN const u8 *aad, IN const u32 aadlen,
                               IN const u32 auth_type,
                               OUT u8 *pResult, OUT u8 *pTag)
{
    int ret;
    uint32_t hash_padlen = 0;
    hal_crypto_adapter_t *pcrypto_adapter = &HAL_CRYPTO_ADAPTER;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (iv != NULL) {
        if (check_4byte_align((size_t)(iv))) {
            return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
        }
    }
    if (check_4byte_align((size_t)(aad))) {
        return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
    }
    if (aadlen > CRYPTO_MAX_AAD_LENGTH) {
        return _ERRNO_CRYPTO_AAD_OutRange;
    }
    if (pTag == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (pResult == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (message == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (msglen == 0) {
        return 0;
    }
    if (msglen > CRYPTO_MAX_MSG_LENGTH) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }
    if ((msglen % CRYPTO_AES_IV_LENGTH) != 0) {
        return _ERRNO_CRYPTO_AES_MSGLEN_NOT_16Byte_Aligned;
    }

    hash_padlen = hal_crypto_mix_mode_calculate_hash_padding(pcrypto_adapter,auth_type,msglen,aadlen);

    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_mix_mode_process(pcrypto_adapter, CRYPTO_MIX_MODE_SSH_DEC,
                                                            cipher_type, message, msglen, iv, ivlen,
                                                            aad, aadlen, auth_type, (const u8 *)hash_padding, hash_padlen,
                                                            NULL, 0, pResult, pTag);
#if CRYPTO_THREAD_MODE
    critical_leave();
#endif
    return ret;
}

int hal_crypto_mix_esp_encrypt(IN const u32 cipher_type,
                               IN const u8 *message, IN const u32 msglen,
                               IN const u8 *iv, IN const u32 ivlen,
                               IN const u8 *aad, IN const u32 aadlen,
                               IN const u32 auth_type,
                               OUT u8 *pResult, OUT u8 *pTag)
{
    int ret;
    uint32_t hash_padlen = 0;
    hal_crypto_adapter_t *pcrypto_adapter = &HAL_CRYPTO_ADAPTER;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (iv != NULL) {
        if (check_4byte_align((size_t)(iv))) {
            return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
        }
    }
    if (check_4byte_align((size_t)(aad))) {
        return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
    }
    if (aadlen > CRYPTO_MAX_AAD_LENGTH) {
        return _ERRNO_CRYPTO_AAD_OutRange;
    }
    if (pTag == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (pResult == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (message == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (msglen == 0) {
        return 0;
    }
    if (msglen > CRYPTO_MAX_MSG_LENGTH) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }
    if ((msglen % CRYPTO_AES_IV_LENGTH) != 0) {
        return _ERRNO_CRYPTO_AES_MSGLEN_NOT_16Byte_Aligned;
    }

    hash_padlen = hal_crypto_mix_mode_calculate_hash_padding(pcrypto_adapter,auth_type,msglen,aadlen);

    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_mix_mode_process(pcrypto_adapter, CRYPTO_MIX_MODE_ESP_ENC,
                                                            cipher_type, message, msglen, iv, ivlen,
                                                            aad, aadlen, auth_type, (const u8 *)hash_padding, hash_padlen,
                                                            NULL, 0, pResult, pTag);
#if CRYPTO_THREAD_MODE
    critical_leave();
#endif
    return ret;
}

int hal_crypto_mix_esp_decrypt(IN const u32 cipher_type,
                               IN const u8 *message, IN const u32 msglen,
                               IN const u8 *iv, IN const u32 ivlen,
                               IN const u8 *aad, IN const u32 aadlen,
                               IN const u32 auth_type,
                               OUT u8 *pResult, OUT u8 *pTag)
{
    int ret;
    uint32_t hash_padlen = 0;
    hal_crypto_adapter_t *pcrypto_adapter = &HAL_CRYPTO_ADAPTER;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (iv != NULL) {
        if (check_4byte_align((size_t)(iv))) {
            return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
        }
    }
    if (check_4byte_align((size_t)(aad))) {
        return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
    }
    if (aadlen > CRYPTO_MAX_AAD_LENGTH) {
        return _ERRNO_CRYPTO_AAD_OutRange;
    }
    if (pTag == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (pResult == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (message == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (msglen == 0) {
        return 0;
    }
    if (msglen > CRYPTO_MAX_MSG_LENGTH) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }
    if ((msglen % CRYPTO_AES_IV_LENGTH) != 0) {
        return _ERRNO_CRYPTO_AES_MSGLEN_NOT_16Byte_Aligned;
    }

    hash_padlen = hal_crypto_mix_mode_calculate_hash_padding(pcrypto_adapter,auth_type,msglen,aadlen);

    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_mix_mode_process(pcrypto_adapter, CRYPTO_MIX_MODE_ESP_DEC,
                                                            cipher_type, message, msglen, iv, ivlen,
                                                            aad, aadlen, auth_type, (const u8 *)hash_padding, hash_padlen,
                                                            NULL, 0, pResult, pTag);
#if CRYPTO_THREAD_MODE
    critical_leave();
#endif
    return ret;
}

int hal_crypto_mix_ssl_tls_encrypt(IN const u32 cipher_type,
                                   IN const u8 *message, IN const u32 msglen,
                                   IN const u8 *iv, IN const u32 ivlen,
                                   IN const u8 *aad, IN const u32 aadlen,
                                   IN const u32 auth_type,
                                   OUT u8 *pResult)
{
    int ret;
    u8 *pTag = NULL;
    uint32_t hash_padlen = 0, enc_padlen = 0;
    hal_crypto_adapter_t *pcrypto_adapter = &HAL_CRYPTO_ADAPTER;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (iv != NULL) {
        if (check_4byte_align((size_t)(iv))) {
            return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
        }
    }
    if (check_4byte_align((size_t)(aad))) {
        return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
    }
    if (aadlen > CRYPTO_MAX_AAD_LENGTH) {
        return _ERRNO_CRYPTO_AAD_OutRange;
    }
    if (pResult == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (message == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (msglen == 0) {
        return 0;
    }
    if (msglen > CRYPTO_MAX_MSG_LENGTH) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }
    if ((msglen % CRYPTO_AES_IV_LENGTH) != 0) {
        return _ERRNO_CRYPTO_AES_MSGLEN_NOT_16Byte_Aligned;
    }

    hash_padlen = hal_crypto_mix_mode_calculate_hash_padding(pcrypto_adapter, auth_type, msglen, aadlen);

    enc_padlen  = hal_crypto_mix_mode_calculate_enc_padding(pcrypto_adapter, auth_type, msglen);

    ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_mix_mode_process(pcrypto_adapter, CRYPTO_MIX_MODE_SSL_TLS_ENC,
                                                            cipher_type, message, msglen, iv, ivlen,
                                                            aad, aadlen, auth_type, (const u8 *)hash_padding, hash_padlen,
                                                            (const u8 *)enc_padding, enc_padlen, pResult, pTag);
#if CRYPTO_THREAD_MODE
    critical_leave();
#endif
    return ret;
}

//
// Key storage
//

int hal_crypto_key_storage_write(
    IN const u8* keyin,
    IN const u8 index, 
    IN const u8 keylock, 
    IN const u32 keylen
)
{
    int ret, keylock_result;
#if CRYPTO_THREAD_MODE
    critical_enter();
#endif
    hal_crypto_adapter_t *pcrypto_adapter = &HAL_CRYPTO_ADAPTER;

    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (keyin == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (check_4byte_align((size_t)(keyin))) {
        return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
    }
    if (index < 0 || (index > KEY_STORAGE_MAX_NUM - 1)) {
        return _ERRNO_CRYPTO_KEY_STORAGE_WRONG_NUMBER;
    }
    //printf("keylock = %d\n", keylock);
    //if (keylock != 0 || keylock != 1) {
    //    return _ERRNO_CRYPTO_KEY_STORAGE_WRONG_KEY_LOCK;
    //}
    if (keylen <= 0 || keylen > CRYPTO_MAX_KEY_LENGTH) {
        return _ERRNO_CRYPTO_KEY_OutRange;
    }

    keylock_result = hal_crypto_key_storage_write_lock_result_rtl8277c(pcrypto_adapter, index);
    if (keylock_result == 0) {
        ret = hal_crypto_key_storage_write_rtl8277c(pcrypto_adapter, keyin, index, keylock, keylen);
    } else {
        return _ERRNO_CRYPTO_KEY_STORAGE_WRITE_LOCKED;
    }
    //ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_auth(pcrypto_adapter, AUTH_TYPE_MD5, message, msglen, NULL, 0, pDigest);
#if CRYPTO_THREAD_MODE
    critical_leave();
#endif
    return ret;
}

int hal_crypto_key_storage_deusage(
    void
)
{
    int ret;
    hal_crypto_adapter_t *pcrypto_adapter = &HAL_CRYPTO_ADAPTER;

    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }

    ret = hal_crypto_key_storage_deusage_rtl8277c(pcrypto_adapter);

    return ret;
}

int hal_crypto_key_storage_usage(
    IN const u8 index
)
{
    int ret;
    hal_crypto_adapter_t *pcrypto_adapter = &HAL_CRYPTO_ADAPTER;

    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if ((index < 0) && (index > KEY_STORAGE_MAX_NUM - 1)) {
        if (index == SKDR_14) {
            // otp_key_reg0
        } else if (index == SKDR_15) {
            // otp_key_reg1
        } else {
            return _ERRNO_CRYPTO_KEY_STORAGE_WRONG_NUMBER;
        }
    }

    ret = hal_crypto_key_storage_usage_rtl8277c(pcrypto_adapter, index);

    return ret;
}

int hal_crypto_key_storage_writeback(IN const u8 index)
{
    int ret;

    hal_crypto_adapter_t *pcrypto_adapter = &HAL_CRYPTO_ADAPTER;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if ((index < 0) || (index > KEY_STORAGE_MAX_NUM - 1)) {
        return _ERRNO_CRYPTO_KEY_STORAGE_WRONG_NUMBER;
    }

    ret = hal_crypto_key_storage_writeback_rtl8277c(pcrypto_adapter, index);

    return ret;
}

int hal_crypto_key_storage_dewriteback(void)
{
    int ret;

    hal_crypto_adapter_t *pcrypto_adapter = &HAL_CRYPTO_ADAPTER;
    
    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }

    ret = hal_crypto_key_storage_dewriteback_rtl8277c(pcrypto_adapter);

    return ret;
}

int hal_crypto_keypad_storage_write(
    IN const u8* keypadin,
    IN const u8 index, 
    IN const u8 keypadlock, 
    IN const u32 keypadlen
)
{
    int ret, keypadlock_result;
#if CRYPTO_THREAD_MODE
    critical_enter();
#endif
    hal_crypto_adapter_t *pcrypto_adapter = &HAL_CRYPTO_ADAPTER;

    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (keypadin == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (check_4byte_align((size_t)(keypadin))) {
        return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned;
    }
    if (index < 0 || (index > KEYPAD_STORAGE_MAX_NUM - 1)) {
        return _ERRNO_CRYPTO_KEYPAD_STORAGE_WRONG_NUMBER;
    }
    //printf("keylock = %d\n", keylock);
    //if (keylock != 0 || keylock != 1) {
    //    return _ERRNO_CRYPTO_KEY_STORAGE_WRONG_KEY_LOCK;
    //}
    if (keypadlen < 1 || keypadlen > CRYPTO_PADSIZE) { // keypadlen is 8-byte unit
        return _ERRNO_CRYPTO_KEY_OutRange;
    }

    keypadlock_result = hal_crypto_keypad_storage_write_lock_result_rtl8277c(pcrypto_adapter, index);
    if (keypadlock_result == 0) {
        ret = hal_crypto_keypad_storage_write_rtl8277c(
            pcrypto_adapter, keypadin, index, keypadlock, keypadlen);
    } else {
        return _ERRNO_CRYPTO_KEYPAD_STORAGE_WRITE_LOCKED;
    }
    //ret = HAL_CRYPTO_FUNC_STUBS.hal_crypto_auth(pcrypto_adapter, AUTH_TYPE_MD5, message, msglen, NULL, 0, pDigest);
#if CRYPTO_THREAD_MODE
    critical_leave();
#endif
    return ret;
}

int hal_crypto_keypad_storage_usage(
    IN const u8 index,
    IN const u8 keypadlen
)
{
    int ret;
    hal_crypto_adapter_t *pcrypto_adapter = &HAL_CRYPTO_ADAPTER;

    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (index < 0 || (index > KEYPAD_STORAGE_MAX_NUM - 1)) {
        return _ERRNO_CRYPTO_KEY_STORAGE_WRONG_NUMBER;
    }
    if (keypadlen < 1 || keypadlen > CRYPTO_PADSIZE) {
        return _ERRNO_CRYPTO_KEY_OutRange;
    }

    ret = hal_crypto_keypad_storage_usage_rtl8277c(pcrypto_adapter, index, keypadlen);

    return ret;
}

int hal_crypto_keypad_storage_deusage(
    IN const u8 index
)
{
    int ret;
    hal_crypto_adapter_t *pcrypto_adapter = &HAL_CRYPTO_ADAPTER;

    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (index < 0 || (index > KEYPAD_STORAGE_MAX_NUM - 1)) {
        return _ERRNO_CRYPTO_KEY_STORAGE_WRONG_NUMBER;
    }

    ret = hal_crypto_keypad_storage_deusage_rtl8277c(pcrypto_adapter, index);

    return ret;
}

#if defined(CONFIG_BUILD_NONSECURE) && (CONFIG_BUILD_NONSECURE==1)

//crc
int hal_crypto_crc32_cmd(IN const u8 *message, IN const u32 msglen, OUT u32 *pCrc)
{
#if CRYPTO_THREAD_MODE
    osThreadId curID = osThreadGetId();
#endif
    hal_crypto_adapter_t *pcrypto_adapter = &g_rtl_cryptoEngine_ns;
    int ret = SUCCESS;

    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (pCrc == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (message == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (msglen == 0) {
        return 0;
    }
    if (msglen > CRC_MAX_MSG_LENGTH) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }

#if CRYPTO_THREAD_MODE
    critical_enter();
    g_crc_thread_id = curID;
#else
    crc_done = 0;
#endif

    //DBG_CRYPTO_INFO("start crc32 HW\r\n");
    ret = hal_crypto_stubs_ns.hal_crypto_crc32_cmd(pcrypto_adapter, message, msglen);
    if (ret != SUCCESS) {
#if CRYPTO_THREAD_MODE
    critical_leave();
#endif
        return ret;
    }

    /*
     * Doesn't need to wait crc_done or signal, because of using polling way
     ***/

    *pCrc = hal_crypto_stubs_ns.hal_crypto_crc_get_result(pcrypto_adapter);
#if CRYPTO_THREAD_MODE
    critical_leave();
#endif
    return ret;
}

int hal_crypto_crc32_dma(IN const u8 *message, IN const u32 msglen, OUT u32 *pCrc)
{
#if CRYPTO_THREAD_MODE
    osThreadId curID = osThreadGetId();
#endif
    hal_crypto_adapter_t *pcrypto_adapter = &g_rtl_cryptoEngine_ns;
    int ret = SUCCESS;
    uint32_t loopWait;
    volatile uint32_t crcstat_err;

    loopWait = 10000;/* hope long enough */

    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (pCrc == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (message == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (msglen == 0) {
        return 0;
    }
    if (msglen > CRC_MAX_MSG_LENGTH) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }

#if CRYPTO_THREAD_MODE
    critical_enter();
    g_crc_thread_id = curID;
#else
    crc_done = 0;
#endif

    //DBG_CRYPTO_INFO("start crc32 HW\r\n");
    ret = hal_crypto_stubs_ns.hal_crypto_crc32_dma(pcrypto_adapter, message, msglen);
    if (ret != SUCCESS) {
#if CRYPTO_THREAD_MODE
        critical_leave();
#endif
        return ret;
    }

    //DBG_CRYPTO_INFO("wait\r\n");
#if CRYPTO_THREAD_MODE
    {
        osEvent evt;

        //DBG_CRYPTO_INFO("wait for signal , curID: 0x%x\r\n", curID);
        evt = osSignalWait(CRYPTO_SIGNAL, osWaitForever); // wait for 100 ms

        if ((evt.status != osOK) && (evt.status != osEventSignal)) {
            DBG_CRYPTO_ERR("osSignalWait failed, evt.status = 0x%x \r\n", evt.status);
            return -1;
        }
    }
#else
    if (pcrypto_adapter->isIntMode) {
        //while ( crc_done == 0 );
        //TODO: can add some Delay / and loops limitation
        while (crc_done == 0) {
            loopWait--;
            if (loopWait == 0) {
                crcstat_err = (CRYPTO_MODULE->crc_stat_reg);
                DBG_CRYPTO_ERR("CRC interrupt doesn't arrive.Wait Timeout crc status =0x%08x\r\n", crcstat_err);
                return FAIL; /* error occurs */
            }
        }
    }
#endif

    *pCrc = hal_crypto_stubs_ns.hal_crypto_crc_get_result(pcrypto_adapter);
#if CRYPTO_THREAD_MODE
    critical_leave();
#endif
    return ret;
}

int hal_crypto_crc_setting(int order, unsigned long polynom,
                           unsigned long crcinit, unsigned long crcxor, int refin, int refout)
{
#if CRYPTO_THREAD_MODE
    osThreadId curID = osThreadGetId();
#endif
    hal_crypto_adapter_t *pcrypto_adapter = &g_rtl_cryptoEngine_ns;
    int ret = SUCCESS;

    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }

#if CRYPTO_THREAD_MODE
    critical_enter();
    g_crc_thread_id = curID;
#else
    crc_done = 0;
#endif

    ret = hal_crypto_stubs_ns.hal_crypto_crc_setting(pcrypto_adapter, order, polynom, crcinit, crcxor, refin, refout);

    if (ret != SUCCESS) {
#if CRYPTO_THREAD_MODE
        critical_leave();
#endif
    }
    return ret;
}

int hal_crypto_crc_cmd(IN const u8 *message, IN const u32 msglen, OUT u32 *pCrc)
{
    hal_crypto_adapter_t *pcrypto_adapter = &g_rtl_cryptoEngine_ns;
    int ret = SUCCESS;

    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (pCrc == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (message == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (msglen == 0) {
        return 0;
    }
    if (msglen > CRC_MAX_MSG_LENGTH) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }

    (void)(pcrypto_adapter);
    ret = hal_crypto_stubs_ns.hal_crypto_crc_cmd(pcrypto_adapter, message, msglen);
    if (ret != SUCCESS) {
#if CRYPTO_THREAD_MODE
        critical_leave();
#endif
        return ret;
    }
    /*
     * Doesn't need to wait crc_done or signal, because of using polling way
     ***/

    *pCrc = hal_crypto_stubs_ns.hal_crypto_crc_get_result(pcrypto_adapter);
    //*pCrc = hal_crypto_stubs_ns.hal_crypto_crc_get_result();

#if CRYPTO_THREAD_MODE
    critical_leave();
#endif

    return ret;
}

int hal_crypto_crc_dma(IN const u8 *message, IN const u32 msglen, OUT u32 *pCrc)
{
    hal_crypto_adapter_t *pcrypto_adapter = &g_rtl_cryptoEngine_ns;
    int ret = SUCCESS;
    uint32_t loopWait;
    volatile uint32_t crcstat_err;

    loopWait = 10000;/* hope long enough */

    if (pcrypto_adapter->isInit != _TRUE) {
        return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    }
    if (pCrc == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (message == NULL) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }
    if (msglen == 0) {
        return 0;
    }
    if (msglen > CRC_MAX_MSG_LENGTH) {
        return _ERRNO_CRYPTO_MSG_OutRange;
    }

    (void)(pcrypto_adapter);
    ret = hal_crypto_stubs_ns.hal_crypto_crc_dma(pcrypto_adapter, message, msglen);
    if (ret != SUCCESS) {
#if CRYPTO_THREAD_MODE
        critical_leave();
#endif
        return ret;
    }

    //DBG_CRYPTO_INFO("wait\r\n");
#if CRYPTO_THREAD_MODE
    {
        osEvent evt;

        //DBG_CRYPTO_INFO("wait for signal , curID: 0x%x\r\n", curID);
        evt = osSignalWait(CRYPTO_SIGNAL, osWaitForever); // wait for 100 ms

        if ((evt.status != osOK) && (evt.status != osEventSignal)) {
            DBG_CRYPTO_ERR("osSignalWait failed, evt.status = 0x%x \r\n", evt.status);
            return -1;
        }
    }
#else
    if (pcrypto_adapter->isIntMode) {
        //while ( crc_done == 0 );
        //TODO: can add some Delay / and loops limitation
        while (crc_done == 0) {
            loopWait--;
            if (loopWait == 0) {
                crcstat_err = (CRYPTO_MODULE->crc_stat_reg);
                DBG_CRYPTO_ERR("CRC interrupt doesn't arrive.Wait Timeout crc status =0x%08x\r\n", crcstat_err);
                return FAIL; /* error occurs */
            }
        }
    }
#endif

    *pCrc = hal_crypto_stubs_ns.hal_crypto_crc_get_result(pcrypto_adapter);
    //*pCrc = hal_crypto_stubs_ns.hal_crypto_crc_get_result();

#if CRYPTO_THREAD_MODE
    critical_leave();
#endif

    return ret;
}
#endif

#if 0
//
// debug
//

void rtl_crypto_set_debug(int val)
{
    __rtl_crypto_set_debug(val);
}
#endif
