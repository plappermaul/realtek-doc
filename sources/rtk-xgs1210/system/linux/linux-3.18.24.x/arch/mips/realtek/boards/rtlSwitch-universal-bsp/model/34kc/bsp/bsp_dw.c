/* alan test */

#include <linux/smp.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

#include <linux/uaccess.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/spinlock.h>
#include <linux/irq.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <asm/gic.h>

static ssize_t bsp_dw_write_proc(struct file *file, const char __user *buffer,
             size_t count, loff_t *ppos)
{
   char tmpbuf[32];
   unsigned int addr = 0;
   unsigned int length = 0;
   unsigned type = 0;
   int *ptr = NULL;
   int i = 0;
    /* no data be written */
    if (!count) {
        printk("count is 0\n");
        return 0;
    }

    /* Input size is too large to write our buffer(num) */
    if (count > (sizeof(tmpbuf) - 1)) {
        printk("input is too large\n");
        return -EINVAL;
    }

    if (copy_from_user(tmpbuf, buffer, count)) {
        printk("copy from user failed\n");
        return -EFAULT;
    }
    i = sscanf(tmpbuf, "0x%x 0x%x %d", &addr, &length, &type);

    printk("[CPU%d]input addr: %x, length: %x \n", smp_processor_id(),addr, length);

    if( (addr >> 28) == 0 ){
      return -EINVAL;
    }


    ptr = (int *)addr;
    if( i == 2 ){
      for(i=0;i<length;i++){
      printk("   %p : 0x%x \n", (ptr+i),*(ptr+i));
      }
    }else
    if( i == 3){
        printk("   %p <= 0x%x \n", (ptr), length);
        *ptr = length;
    }
    else{
      printk("%p : 0x%x \n", ptr, *ptr);
    }

    return count;
}
// 	ssize_t (*read) (struct file *, char __user *, size_t, loff_t *);
// 	ssize_t (*write) (struct file *, const char __user *, size_t, loff_t *);

static int bsp_proc_show(struct seq_file *m, void *v)
{

    extern unsigned long ebase;
    seq_printf(m, "Cause: 0x%08x\n", read_c0_cause());
    seq_printf(m, "Config: 0x%08x\n", read_c0_config());
    seq_printf(m, "Config1: 0x%08x\n", read_c0_config1());
    seq_printf(m, "Config7: 0x%08x\n", read_c0_config7());
    seq_printf(m, "EBASE: 0x%08lx\n", ebase);
    seq_printf(m, "[cpu%d]GCR - Global Config Register : %x ; \n", smp_processor_id(), *(unsigned int *)(KSEG1|(GCMP_BASE_ADDR)) );
    seq_printf(m, "[cpu%d]GIC - Global Config Register : %x ; \n", smp_processor_id(), *(unsigned int *)(KSEG1|(GIC_BASE_ADDR)) );

    return 0;
}

static int bsp_proc_open(struct inode *inode, struct file *file)
{
    return single_open(file, bsp_proc_show, NULL);
}

static const struct file_operations bsp_dw_proc_fops = {
    .open		= bsp_proc_open,
    .read		= seq_read,
    .write          = bsp_dw_write_proc,
    .llseek		= seq_lseek,
    .release	= single_release,
};


/****************L2 cache ********************************/
#define M_PERFCTL_EXL			(1	<<  0)
#define M_PERFCTL_KERNEL		(1	<<  1)
#define M_PERFCTL_SUPERVISOR		(1	<<  2)
#define M_PERFCTL_USER			(1	<<  3)
#define M_PERFCTL_INTERRUPT_ENABLE	(1	<<  4)
#define M_PERFCTL_EVENT(event)		(((event) & 0x3ff)  << 5)
#define M_PERFCTL_VPEID(vpe)		((vpe)	  << 16)

#define M_PERFCTL_MT_EN(filter)		((filter) << 20)

#define	   M_TC_EN_ALL			M_PERFCTL_MT_EN(0)
#define	   M_TC_EN_VPE			M_PERFCTL_MT_EN(1)
#define	   M_TC_EN_TC			M_PERFCTL_MT_EN(2)
#define M_PERFCTL_TCID(tcid)		((tcid)	  << 22)
#define M_PERFCTL_WIDE			(1	<< 30)
#define M_PERFCTL_MORE			(1	<< 31)
#define M_PERFCTL_TC			(1	<< 30)

#define M_PERFCTL_COUNT_EVENT_WHENEVER	(M_PERFCTL_EXL |		\
                    M_PERFCTL_KERNEL |		\
                    M_PERFCTL_USER |		\
                    M_PERFCTL_SUPERVISOR |		\
                    M_PERFCTL_INTERRUPT_ENABLE)

#define M_PERFCTL_COUNT_EVENT_WHENEVER_NIE	(M_PERFCTL_EXL |		\
                    M_PERFCTL_KERNEL |		\
                    M_PERFCTL_USER |		\
                    M_PERFCTL_SUPERVISOR )


static struct task_struct *l2m_threads[NR_CPUS];

struct l2m_test_data {
    unsigned int		events;
    int			cpu;
    int			cnt;
    int                     date;
};

static struct l2m_test_data l2m_data[NR_CPUS];

/*******   On count0   *************/
#define loads_completed            15     //On count0, Per-Thread
#define Dcache_miss_cycles          39     //On count0, Per-Processor
#define Load_to_Use_stall_cycles   45     //On count0, Per-Thread


/*******   On count1   *************/
#define stores_completed           15     //On count1, Per-Thread
#define Dcache_miss_stall_cycles   37     //On count1, Per-Thread
#define Long_stall_cycles          44     //On count1, Per-Thread

static inline void l2_reset_counts(void){
              instruction_hazard();
          write_c0_perfcntr0(0); //reset
          write_c0_perfcntr1(0); //reset
}

static void l2_set_perfconunt(void){
        switch (smp_processor_id())
       {
    case 0: //CPU-0
          write_c0_perfctrl0(M_PERFCTL_EVENT(22) | M_PERFCTL_COUNT_EVENT_WHENEVER_NIE);
          //for count0 22 : L2 cache miss
          write_c0_perfctrl1(M_PERFCTL_EVENT(21) | M_PERFCTL_COUNT_EVENT_WHENEVER_NIE);
          //for count1 21 : L2 cache accesses
              break;

    case 1: //CPU-1
          write_c0_perfctrl0(M_PERFCTL_EVENT(Dcache_miss_cycles) | M_PERFCTL_COUNT_EVENT_WHENEVER_NIE);
          //for count0, Dcache miss cycles
          write_c0_perfctrl1(M_PERFCTL_EVENT(Dcache_miss_stall_cycles) | M_PERFCTL_COUNT_EVENT_WHENEVER_NIE);
          //for count1, Dcache miss stall cycles
              break;

    case 2: //CPU-2
          write_c0_perfctrl0(M_PERFCTL_EVENT(loads_completed) | M_PERFCTL_COUNT_EVENT_WHENEVER_NIE);
          //for count0 , Loads completed
          write_c0_perfctrl1(M_PERFCTL_EVENT(stores_completed) | M_PERFCTL_COUNT_EVENT_WHENEVER_NIE);
          //for count1 , Store completed
              break;

    case 3: //CPU-3
          write_c0_perfctrl0(M_PERFCTL_EVENT(Load_to_Use_stall_cycles) | M_PERFCTL_COUNT_EVENT_WHENEVER_NIE);
          //for count0, Load to use stall sycles
          write_c0_perfctrl1(M_PERFCTL_EVENT(Long_stall_cycles) | M_PERFCTL_COUNT_EVENT_WHENEVER_NIE);
          //for count1, Long stall cycles
          break;

    }
    l2_reset_counts();
}



void l2_cache_perf(struct l2m_test_data *data){
     int count0;
     int count1;
     int cache_miss_rate = 0;
    count0 = read_c0_perfcntr0();
    count1 = read_c0_perfcntr1();

    switch (smp_processor_id())
    {
    case 0:
          if ( (count0 > 0) && (count1 > 0))
          cache_miss_rate = (count0*100/count1);
              printk("CPU%d: L2 cache misses(count0) = %d, L2 cache access(count1)  = %d, [ %d ]\n", smp_processor_id(), count0, count1, cache_miss_rate);
              break;
    case 1:
          printk("CPU%d: Dcache miss_cycles(count0) = %d, Dcache miss stall cycles(count1)  = %d\n", smp_processor_id(), count0, count1);
              break;
    case 2:
          printk("CPU%d: load_completed(count0) = %d, stores_completed(count1)  = %d, [ %d ]\n", smp_processor_id(), count0, count1, cache_miss_rate);
              break;
    case 3:
          printk("CPU%d: Load_to_Use_stall_cycles(count0) = %d, Long_stall_cycles(count1)  = %d\n", smp_processor_id(), count0, count1);
          break;
    }
    l2_reset_counts();

}

int seconds = 60;
static  int l2_monitor(void *arg)
{
       struct l2m_test_data *data = arg;

       l2_set_perfconunt();
    while (!kthread_should_stop()) {
        l2_cache_perf(data);
        data->cnt++;
            set_current_state(TASK_INTERRUPTIBLE);
        /* Now sleep */
        schedule_timeout(seconds*HZ);
    }

    return 0;
}

static void l2m_thread_init(void){
    int cpu = 0;
    for_each_online_cpu(cpu) {

        l2m_threads[cpu] = kthread_create(l2_monitor, &l2m_data[cpu],
                         "l2cache_m/%d", cpu);
        if (WARN_ON(!l2m_threads[cpu])) {
            printk("CPU%d create l2m_thread failed!", cpu);
            goto out_free;
        }

        kthread_bind(l2m_threads[cpu], cpu);
        wake_up_process(l2m_threads[cpu]);
    }
#if defined(TEST_AUTO_RESET)
          restart_threads= kthread_create(bsp_auto_reset, NULL,
                         "bsp_auto_reset/%d", 0);
        kthread_bind(restart_threads, 0);
        wake_up_process(restart_threads);
#endif

out_free:
   return ;

}
static void l2m_thread_stop(void){
    int cpu = 0;
    for_each_online_cpu(cpu) {

      kthread_stop(l2m_threads[cpu]);

    }
}


static ssize_t bsp_l2cache_write_proc(struct file *file, const char __user *buffer,
             size_t count, loff_t *ppos)
{
      char tmpbuf[10];
      int eventid = 0;


    /* no data be written */
    if (!count) {
        printk("count is 0\n");
        return 0;
    }

    /* Input size is too large to write our buffer(num) */
    if (count > (sizeof(tmpbuf) - 1)) {
        printk("input is too large\n");
        return -EINVAL;
    }

    if (copy_from_user(tmpbuf, buffer, count)) {
        printk("copy from user failed\n");
        return -EFAULT;
    }
    sscanf(tmpbuf, "%d", &eventid);


    printk("cpu(%d), input %d: \n", smp_processor_id(),eventid);
   if(eventid){
        if( (eventid > 0x10)){
       seconds = eventid;
    }
    l2m_thread_init();
   }else{
        l2m_thread_stop();
   }
    return count;
}


static int bsp_l2cache_proc_show(struct seq_file *m, void *v)
{

    extern unsigned long ebase;
    seq_printf(m, "GCR_BASE: 0x%08x\n", *(int *)0xBFBF8008);
    seq_printf(m, "Config2: 0x%08x\n", read_c0_config2());
    //#define read_c0_perfcntr0()	__read_32bit_c0_register($25, 1)
    seq_printf(m, "Perf count0( L2 cache miss) : %u\n", read_c0_perfcntr0());
    seq_printf(m, "Perf count1(L2 cache access) : %u\n", read_c0_perfcntr1());

    return 0;
}

static int bsp_l2cache_proc_open(struct inode *inode, struct file *file)
{
    return single_open(file, bsp_l2cache_proc_show, NULL);
}

static const struct file_operations bsp_l2cache_proc_fops = {
    .open		= bsp_l2cache_proc_open,
    .read		= seq_read,
    .write          = bsp_l2cache_write_proc,
    .llseek		= seq_lseek,
    .release	= single_release,
};


/*************Kthread test **************************/
unsigned long long testvalue = 0;
unsigned long long testvalue1 = 0;
#define TX 4
#define TY 4
#define TESTINCR 0x10
unsigned long long testcache[TX][TY] = {{0}}; // 4 *4 * 8

static DEFINE_SPINLOCK(smp_test_lock1);
static DEFINE_SPINLOCK(smp_test_lock2);
static struct task_struct *testcache_threads[NR_CPUS];
struct l1cache_test_data {
    unsigned int		events;
    int			cpu;
    int			cnt;
    int                     date;
};
static struct l1cache_test_data testcache_data[NR_CPUS];

void l1_filldata(unsigned long long value){
    int i,j;
  unsigned long long tmp;
  tmp = value + TESTINCR;
  for (i=0; i<TX; i++){
    for (j=0; j<TY;j++){
        testcache[i][j] = tmp;
    tmp++;
    }
  }
}
void l1_checkdata(unsigned long long value){
  int i,j;
  unsigned long long tmp;
  if(testcache[0][0] ==0)
       return;

  tmp = value + TESTINCR;
  for (i=0; i<TX; i++){
    for (j=0; j<TY;j++){
        if(testcache[i][j] != tmp)
        printk("CPU: %d,l2_checkdata data[%d,%d]:%llu!=%llu )\n", smp_processor_id(), i,j,testcache[i][j], tmp);
    tmp++;
    }
  }


}

int testcache_seconds = 60;

static void l1_cache_perf(struct l1cache_test_data *data){
        spin_lock(&smp_test_lock1);
    testvalue1++;
        spin_unlock(&smp_test_lock1);

    spin_lock(&smp_test_lock2);
    testvalue++;
    l1_filldata(testvalue);
    l1_checkdata(testvalue);
    spin_unlock(&smp_test_lock2);


}

static  int test_pcache(void *arg)
{

       struct l1cache_test_data *data = arg;
      printk("smp_test_lock1=%p;smp_test_lock2=%p\n", &smp_test_lock1 ,&smp_test_lock2);
    while (!kthread_should_stop()) {
        l1_cache_perf(data);
        data->cnt++;
            set_current_state(TASK_INTERRUPTIBLE);
        /* Now sleep between a min of 100-300us and a max of 1ms */
        schedule_timeout(testcache_seconds*HZ);
    }

    return 0;
}

static void testcache_thread_init(void){
    int cpu = 0;
    printk("smp_test_lock1=%p;smp_test_lock2=%p\n", &smp_test_lock1 ,&smp_test_lock2);
    for_each_online_cpu(cpu) {
        testcache_threads[cpu] = kthread_create(test_pcache, &testcache_data[cpu],
                         "testcache/%d", cpu);
        if (WARN_ON(!testcache_threads[cpu])) {
            printk("CPU%d create testcache_threads failed!", cpu);
            goto out_free;
        }

        kthread_bind(testcache_threads[cpu], cpu);
        wake_up_process(testcache_threads[cpu]);
    }


out_free:
   return ;

}
static void testcache_thread_stop(void){
    int cpu = 0;
    for_each_online_cpu(cpu) {

      kthread_stop(testcache_threads[cpu]);

    }
}


static ssize_t bsp_testcache_write_proc(struct file *file, const char __user *buffer,
             size_t count, loff_t *ppos)
{
      char tmpbuf[10];
      int para0 = 0;


    /* no data be written */
    if (!count) {
        printk("count is 0\n");
        return 0;
    }

    /* Input size is too large to write our buffer(num) */
    if (count > (sizeof(tmpbuf) - 1)) {
        printk("input is too large\n");
        return -EINVAL;
    }

    if (copy_from_user(tmpbuf, buffer, count)) {
        printk("copy from user failed\n");
        return -EFAULT;
    }
    sscanf(tmpbuf, "%d", &para0);


    printk("cpu(%d), input %d: \n", smp_processor_id(),para0);
   if(para0){
        if( (para0 > 1)){
       testcache_seconds = para0;
    }
    testcache_thread_init();
   }else{
        testcache_thread_stop();
   }
    return count;
}

static int bsp_testcache_proc_show(struct seq_file *m, void *v)
{

    extern unsigned long ebase;
    seq_printf(m, "GCR_BASE: 0x%08x\n", *(int *)0xBFBF8008);
    seq_printf(m, "Config2: 0x%08x\n", read_c0_config2());
    //#define read_c0_perfcntr0()	__read_32bit_c0_register($25, 1)



    return 0;
}

static int bsp_testcache_proc_open(struct inode *inode, struct file *file)
{
    return single_open(file, bsp_testcache_proc_show, NULL);
}

static const struct file_operations bsp_testcache_proc_fops = {
    .open		= bsp_testcache_proc_open,
    .read		= seq_read,
    .write          = bsp_testcache_write_proc,
    .llseek		= seq_lseek,
    .release	= single_release,
};


/******************ITC Test**********************************/
#include <asm/mipsmtregs.h>
#include <asm/r4kcache.h>
#include <asm/cacheflush.h>
#include <asm/cacheops.h>

#define ITC_Bypass_View	        (0x0<<3)
#define ITC_Control_View	(0x1<<3)

#define ITC_EF_Sync_View	(0x2<<3)
#define ITC_EF_Try_View	        (0x3<<3)

#define ITC_PV_Sync_View	(0x4<<3)
#define ITC_PV_Try_View	        (0x5<<3)

#define BSP_ITC_LOCK  bsp_itc_lock(21, &flag)
#define BSP_ITC_UNLOCK  bsp_itc_unlock(16, &flag)


#define ITC_PhyAddr  0x9000000  /* 1K alignment */
int ITC_CELL = 0;
// int ITC_PITCH = 256;
//int ITC_PITCH = 0x1000;//4096

//EntryGrain[2 : 0] = 0x0  =>> 128B
/* EntryGrain[2 : 0] = 0x3  =>> 1KB          */
int ITC_PITCH = 0x400;//1024
int EntryGrain = 0x3;

int ITC_addr_map = 0x00000c00; //4KB Range
unsigned int* ITC_BlcokNC;
/*Use physical address to itc_bse */
static void bsp_itc_init(unsigned itc_base){
        if( itc_base & 0x3FF  ){
      printk("ITC BaseAddress must be 1024B alignment, input(0x%4x)\n", itc_base);
      return;
    }


    if (itc_base != 0) {
        /*
         * Configure ITC mapping.  This code is very
         * specific to the 34K core family, which uses
         * a special mode bit ("ITC") in the ErrCtl
         * register to enable access to ITC control
         * registers via cache "tag" operations.
         */
        unsigned long ectlval;
        unsigned long itcblkgrn;

        /* ErrCtl register is known as "ecc" to Linux */
        ectlval = read_c0_ecc();
        write_c0_ecc(ectlval | (0x1 << 26));
        ehb();
#define INDEX_0 (0x80000000)
#define INDEX_8 (0x80000008)
        /* Read "cache tag" for Dcache pseudo-index 8 */
        cache_op(Index_Load_Tag_D, INDEX_8);
        ehb();
        itcblkgrn = read_c0_dtaglo();
        printk("Got ITCAddressMap1 Register =0x%lx\n", itcblkgrn);
        itcblkgrn &= 0xfffe0000;


        /* Set for 128 byte pitch of ITC cells */
        /* AddrMask  [16:10] = 2000 => 8192B             */
        /* AddrMask  [16:10] = 0x3f => 128KB             */


// 		itcblkgrn |= 0x00000c00;
// 		itcblkgrn |= (0x3f<<10);

        /* 0x2000  8KB */
// 		itcblkgrn |= 0x2000;

        /* 0x2000  32KB */
        itcblkgrn |= ITC_addr_map;


            /* EntryGrain[2 : 0] = 1  =>> 256B          */
        /* EntryGrain[2 : 0] = 0x5  =>> 4KB          */
        /* EntryGrain[2 : 0] = 0x3  =>> 1KB          */
        itcblkgrn |= EntryGrain;
        /* Stage in Tag register */
        write_c0_dtaglo(itcblkgrn);
        printk("Set ITCAddressMap1 Register =0x%lx\n", itcblkgrn);
        ehb();
        /* Write out to ITU with CACHE op */
        cache_op(Index_Store_Tag_D, INDEX_8);


        /* Now set base address, and turn ITC on with 0x1 bit */
        /* Physical_address + enable_bit                    */
        write_c0_dtaglo((itc_base & 0xfffffc00) | 0x1 );
        ehb();
        /* Write out to ITU with CACHE op */
        cache_op(Index_Store_Tag_D, INDEX_0);
        write_c0_ecc(ectlval);
        ehb();
        printk("Set ITCAddressMap0 Register =0x%x\n", (itc_base & 0xfffffc00) | 0x1 );
        printk("Mapped %ld ITC cells starting at 0x%08x\n",
            ((itcblkgrn & 0x7fe00000) >> 20), itc_base);

        ITC_CELL = ((itcblkgrn & 0x7fe00000) >> 20);

        ITC_BlcokNC = (unsigned int*)(CKSEG1ADDR(itc_base));
        printk("ITC_BlcokNC=%p\n", ITC_BlcokNC);
    }
}




static void bsp_itc_entry_init(void){
  unsigned int* ITC_Cell;
  unsigned int offset;
   unsigned int* ITC_Cell_ByPass;
   int index = 0;
   unsigned int cell_tag;
  for(index=0; index < 32; index++){
      offset = index*ITC_PITCH;
      /* control_view to change tag */
      ITC_Cell = (unsigned int* )(((unsigned int)ITC_BlcokNC + offset) | ITC_Control_View);
      cell_tag = *ITC_Cell;
      printk("[CPU%d  %s] Index: %d, %p, ITC_CELL_TAG=0x%x\n", smp_processor_id(), __func__, index, ITC_Cell, cell_tag);

    /*******  FIFO *********************************************
     * FIFODepth[31:28] = 0x2, four-entry FIFO cells
     * FIFO[17] = 0x1 , FIFO type(0, single-entry semaphore cells
     * Default value = 0x20020000
     *********************************************************/
    if(  (cell_tag & 0xFFFE0000) == 0x20020000){
      /* FIFO */
      *ITC_Cell |= 0x1;
      printk("[ bsp_itc_entry_init- index:%d, Addr:%p, type:FIFO      ] Inited ITC_CELL_TAG=0x%x\n", index, ITC_Cell, *ITC_Cell);
    }else{
      /* single-entry Semaphore cells. */
      *ITC_Cell = 0; //INIT semp
      printk("[ bsp_itc_entry_init- index:%d, Addr:%p, type:Semaphore ] Inited ITC_CELL_TAG=0x%x\n", index, ITC_Cell, *ITC_Cell);

      /* ITC_PV_Sync_View to set default value */
//       ITC_Cell = (unsigned int* )(((unsigned int)ITC_BlcokNC + offset) | ITC_PV_Sync_View);
//       *ITC_Cell = 1; //init value=1; not lock!
      ITC_Cell_ByPass   = (unsigned int* )(((unsigned int)ITC_BlcokNC + offset) | ITC_Bypass_View);
      *ITC_Cell_ByPass = 1;
      printk("         Index: %d, ITC_Cell_ByPass(%p)=0x%x\n", index, ITC_Cell_ByPass, *ITC_Cell_ByPass);
   }
  }
}



static void bsp_itc_entry_list(void){
  unsigned int* ITC_Cell;
  unsigned int offset;

  int i;
  for(i=0; i<ITC_CELL; i++){
    offset = i*ITC_PITCH;
    /* control_view to change tag */
    ITC_Cell = (unsigned int* )(((unsigned int)ITC_BlcokNC + offset) | ITC_Control_View);
    printk("CELL(%d):%p   Tag = 0x%4x\n", i, ITC_Cell, *ITC_Cell);
    ITC_Cell = (unsigned int* )(((unsigned int)ITC_BlcokNC + offset) | ITC_Bypass_View);
    printk("CELL(%d):%p  Data = 0x%4x\n", i, ITC_Cell, *ITC_Cell);
  }

}

static void bsp_itc_lock(int index, int *flag){
  unsigned int *ITC_Cell;
  unsigned int *ITC_Cell_ByPass;
    unsigned int offset;
     int tmp;
    offset = index*ITC_PITCH;
    printk("CPU%d: bsp_itc_lock: %d\n", smp_processor_id() ,index);
      ITC_Cell        = (unsigned int* )(((unsigned int)ITC_BlcokNC + offset) | ITC_PV_Sync_View);
      ITC_Cell_ByPass = (unsigned int* )(((unsigned int)ITC_BlcokNC + offset) | ITC_Bypass_View);
        printk("[CPU%d : bsp_itc_lock] ITC_Cell | ITC_Bypass_View = %p, value=0x%x\n", smp_processor_id(), ITC_Cell_ByPass, *ITC_Cell_ByPass);
    tmp = *ITC_Cell;
        printk("[CPU%d : bsp_itc_lock] ITC_Cell | ITC_PV_Sync_View = %p,tmp=%d\n", smp_processor_id(), ITC_Cell, tmp);
    printk("[CPU%d : bsp_itc_lock] ITC_Cell | ITC_Bypass_View = %p, value=0x%x\n", smp_processor_id(), ITC_Cell_ByPass, *ITC_Cell_ByPass);
    printk("[CPU%d : bsp_itc_lock] ITC_Cell | ITC_Bypass_View = %p, value=0x%x\n", smp_processor_id(), ITC_Cell_ByPass, *ITC_Cell_ByPass);
    *flag = tmp;
}

static void bsp_itc_unlock(int index, int *flag){
  unsigned int *ITC_Cell;
  unsigned int offset;
  unsigned int *ITC_Cell_ByPass;
      int tmp;
    offset = index*ITC_PITCH;
     tmp = *flag;
    ITC_Cell_ByPass = (unsigned int* )(((unsigned int)ITC_BlcokNC + offset) | ITC_Bypass_View);
    printk("[CPU%d : bsp_itc_unlock] ITC_Cell | ITC_Bypass_View = %p, value=0x%x, tmp=%d\n", smp_processor_id(), ITC_Cell_ByPass, *ITC_Cell_ByPass, tmp);

    ITC_Cell = (unsigned int* )(((unsigned int)ITC_BlcokNC + offset)| ITC_PV_Sync_View);
    *ITC_Cell = tmp;
    printk("[CPU%d : bsp_itc_unlock] ITC_Cell | ITC_PV_Sync_View = %p, value=0x%x\n", smp_processor_id(),ITC_Cell, *flag);
    printk("[CPU%d : bsp_itc_unlock] ITC_Cell | ITC_Bypass_View = %p, value=0x%x, tmp=%d\n", smp_processor_id(), ITC_Cell_ByPass, *ITC_Cell_ByPass, tmp);

}



struct itc_count_s {
    int			cpu;
    unsigned long long testitc_v;
    int                     date;
};

struct itc_count_s itc_count;
#define ITC_TX 4
#define ITC_TY 4
#define ITC_TESTINCR 0x10
unsigned long long itctest_data[ITC_TX][ITC_TY] = {{0}}; // 4 *4 * 8

static struct task_struct *testitc_threads[NR_CPUS];
struct itc_test_threaddata {
    unsigned int		events;
    int			cpu;
    unsigned long long	cnt;
    int                     date;
};
static struct itc_test_threaddata itc_test_data[NR_CPUS];

void itc_filldata(unsigned long long value){
    int i,j;
  unsigned long long tmp;
  tmp = value + ITC_TESTINCR;
  for (i=0; i<TX; i++){
    for (j=0; j<TY;j++){
        itctest_data[i][j] = tmp;
    tmp++;
    }
  }
}
void itc_checkdata(unsigned long long value){
  int i,j;
  unsigned long long tmp;
  if(itctest_data[0][0] ==0)
       return;

  tmp = value + ITC_TESTINCR;
  for (i=0; i<TX; i++){
    for (j=0; j<TY;j++){
        if(itctest_data[i][j] != tmp)
        printk("CPU: %d,ITC checkdata data[%d,%d]:%llu!=%llu )\n", smp_processor_id(), i,j,testcache[i][j], tmp);
    tmp++;
    }
  }


}

int testitc_seconds = 20;

static void itctest_perf(struct itc_test_threaddata *data){
 unsigned int flag;
 unsigned long long tmp;
  BSP_ITC_LOCK;
        itc_count.cpu = smp_processor_id();
        tmp = itc_count.testitc_v;

    itc_count.testitc_v++;
    itc_filldata(data->cnt);
    itc_checkdata(data->cnt);

  if((tmp +1) != itc_count.testitc_v){
    printk("CPU: %d, %s :%llu!=%llu ,itc_count.cpu=%d\n", smp_processor_id(), __func__, tmp+1, itc_count.testitc_v, itc_count.cpu);
  }

  BSP_ITC_UNLOCK;

  if((data->cnt & 0xf) == 12){
       printk("CPU%d: %s Running!\n", smp_processor_id(), __func__);
  }
}

static  int test_itc(void *arg)
{

       struct itc_test_threaddata *data = arg;

    while (!kthread_should_stop()) {
        itctest_perf(data);
        data->cnt++;
            set_current_state(TASK_INTERRUPTIBLE);
        /* Now sleep between a min of 100-300us and a max of 1ms */
        schedule_timeout(testitc_seconds*HZ);
    }

    return 0;
}

// static int itc_init[NR_CPUS]  = { 0 };


static void bsp_itc_base_init_percpu(void *info){

    bsp_itc_init(ITC_PhyAddr);
    bsp_itc_entry_init();
}

static void bsp_itc_base_init( int entryg, int itc_addr){
 int cpu = 0;
    if(entryg >= 0){
      ITC_PITCH = (1 <<(entryg + 7));
      EntryGrain = entryg;
    }

    if(itc_addr >= 0){
      ITC_addr_map = itc_addr;
    }


    printk("[CPU%d: %s] ITC_PITCH= %d, itc_addr=0x%x\n", cpu, __func__, ITC_PITCH, ITC_addr_map);
    for_each_online_cpu(cpu){
     smp_call_function_single(cpu, bsp_itc_base_init_percpu, NULL,1);
    }
}


static void testitc_thread_init(int start){
    int cpu = 0;

    for_each_online_cpu(cpu) {
        if(cpu <start)
           continue;

        testitc_threads[cpu] = kthread_create(test_itc, &itc_test_data[cpu],
                         "testITC/%d", cpu);
        if (WARN_ON(!testitc_threads[cpu])) {
            printk("CPU%d create testitc_threads failed!", cpu);
            goto out_free;
        }

        kthread_bind(testitc_threads[cpu], cpu);
        wake_up_process(testitc_threads[cpu]);
    }

out_free:
   return ;

}

static void testitc_thread_init3(int start){
    int cpu = 0;

    for_each_online_cpu(cpu) {
        if(cpu == start)
           continue;

        testitc_threads[cpu] = kthread_create(test_itc, &itc_test_data[cpu],
                         "testITC/%d", cpu);
        if (WARN_ON(!testitc_threads[cpu])) {
            printk("CPU%d create testitc_threads failed!", cpu);
            goto out_free;
        }

        kthread_bind(testitc_threads[cpu], cpu);
        wake_up_process(testitc_threads[cpu]);
    }

out_free:
   return ;

}

static void testitc_thread_stop(void){
    int cpu = 0;
    for_each_online_cpu(cpu) {
      if(testitc_threads[cpu] != NULL){
        kthread_stop(testitc_threads[cpu]);
      }
    }
}

static void testitc_lock_forever(void){
        int flag;
    BSP_ITC_LOCK;
    testitc_thread_init(0);

}

static void testitc_lock_case3(void){
        int flag;
    BSP_ITC_LOCK;
    testitc_thread_init3(smp_processor_id());

}
static void testitc_lock_one_entry(int index){
        int flag;
    bsp_itc_lock(index, &flag);

}

static void testitc_unlock_one_entry(int index){
        int flag;
    flag = 1;
    printk("%s-%d: %d\n", __func__, __LINE__, index);
    bsp_itc_unlock(index, &flag);

}

/****FIFI TEST
 *  FIFO Index: 0
 *
 */
int testitc_fifo_sec = 30;

struct itc_perf_data {
    unsigned int		events;
    int			cpu;
    unsigned int            count0;
    unsigned int            count1;
};

struct itc_perf_data testitc_fifo_perfcount[NR_CPUS];

static struct task_struct *testitc_fifo_threads[NR_CPUS];

#define Instructions_completed 	  1  //count0
#define ITC_stall_cycles         40 //count1
static void itc_test_fifo_perfcount_set(void){

      write_c0_perfctrl0(M_PERFCTL_EVENT(Instructions_completed) | M_PERFCTL_COUNT_EVENT_WHENEVER_NIE);
      write_c0_perfctrl1(M_PERFCTL_EVENT(ITC_stall_cycles) | M_PERFCTL_COUNT_EVENT_WHENEVER_NIE);
      instruction_hazard();
      write_c0_perfcntr0(0); //reset
      write_c0_perfcntr1(0); //reset
}



static void itc_perf_show(void){
     int cpu = smp_processor_id();
     testitc_fifo_perfcount[cpu].count0 = read_c0_perfcntr0();
     testitc_fifo_perfcount[cpu].count1 = read_c0_perfcntr1();
     printk("CPU%d: Instructions_completed(count0) = %u, ITC_stall_cycles(count1)  = %u\n", smp_processor_id(), testitc_fifo_perfcount[cpu].count0, testitc_fifo_perfcount[cpu].count1);
}
static void itc_perf_show1(void){
     int cpu = smp_processor_id();
     unsigned int tmp = 0;
     tmp = read_c0_perfcntr0();
     printk("CPU%d: Instructions_completed(count0) Add = %u, Current=%u\n", smp_processor_id(), tmp - testitc_fifo_perfcount[cpu].count0, tmp);
     testitc_fifo_perfcount[cpu].count0 = 0;

     tmp = read_c0_perfcntr1();
     printk("CPU%d: ITC_stall_cycles(count1)   ADD    = %u, Current=%u\n", smp_processor_id(), tmp - testitc_fifo_perfcount[cpu].count1, tmp);
     testitc_fifo_perfcount[cpu].count1 = 0;

}

static void itctest_fifo(int index){
  unsigned int* ITC_Cell;
  unsigned int offset;
//   unsigned int* ITC_Cell_ByPass;
  unsigned int cell_tag;
  unsigned int cell_data;
  unsigned long flag;
      offset = index*ITC_PITCH;
      local_irq_save(flag);
      /* control_view to change tag */
      ITC_Cell = (unsigned int* )(((unsigned int)ITC_BlcokNC + offset) | ITC_Control_View);
      cell_tag = *ITC_Cell;

      printk("[cpu%d]itctest_fifo: index: %d, %p, ITC_CELL_TAG=0x%x, NumOfData=%d\n", smp_processor_id() ,index, ITC_Cell, cell_tag, (cell_tag >> 18)&0x7 );

      ITC_Cell = (unsigned int* )(((unsigned int)ITC_BlcokNC + offset) | ITC_EF_Sync_View);
      itc_test_fifo_perfcount_set();
      itc_perf_show();
      local_irq_restore(flag);
      cell_data = *ITC_Cell;
      local_irq_save(flag);
      itc_perf_show1();
      printk("[cpu%d]itctest_fifo: index: %d, %p, ITC_CELL_DATA = 0x%x\n", smp_processor_id() ,index, ITC_Cell, cell_data);
      local_irq_restore(flag);

}

static  int test_itc_fifo(void *arg)
{
      int index = 0;
//       bsp_itc_base_init(-1);
      itc_test_fifo_perfcount_set();
      printk("[CPU%d] %s\n", smp_processor_id(), __func__);
    while (!kthread_should_stop()) {
        itctest_fifo(index);
        index = (index + 1) & 0xf;
            set_current_state(TASK_INTERRUPTIBLE);
        /* Now sleep between a min of 100-300us and a max of 1ms */
        schedule_timeout(testitc_fifo_sec*HZ);
    }
    return 0;
}



static void testitc_FIFO_test(void){
    int cpu;
        cpu =3;

    testitc_fifo_threads[cpu] = kthread_create(test_itc_fifo, NULL, "test_ITC_FIFO/%d", cpu);
        if (WARN_ON(!testitc_fifo_threads[cpu])) {
            printk("[CPU%d] Create CPU%d's test_itc_fifo failed!", smp_processor_id(), cpu);
            goto out_free;
        }

        kthread_bind(testitc_fifo_threads[cpu], cpu);
        wake_up_process(testitc_fifo_threads[cpu]);
out_free:
   return ;

}


/**** semp
*
****************/
static void itctest_semp(int index){
  unsigned int* ITC_Cell;
  unsigned int offset;
//   unsigned int* ITC_Cell_ByPass;
  unsigned int cell_tag;
  unsigned int cell_data;
  unsigned long flag;
      offset = index*ITC_PITCH;
      local_irq_save(flag);
      /* control_view to change tag */
      ITC_Cell = (unsigned int* )(((unsigned int)ITC_BlcokNC + offset) | ITC_Control_View);
      cell_tag = *ITC_Cell;

      printk("[cpu%d]%s: index: %d, %p, ITC_CELL_TAG=0x%x\n", smp_processor_id(), __func__ ,index, ITC_Cell, cell_tag );

      ITC_Cell = (unsigned int* )(((unsigned int)ITC_BlcokNC + offset) | ITC_PV_Sync_View);
      itc_test_fifo_perfcount_set();
      itc_perf_show();

      cell_data = *ITC_Cell;

      itc_perf_show1();
      printk("[cpu%d]%s: index: %d, %p, ITC_CELL_DATA = 0x%x\n", smp_processor_id(), __func__ ,index, ITC_Cell, cell_data);
      local_irq_restore(flag);

}

static  int test_itc_semp(void *arg)
{
      int index = 0;
//       bsp_itc_base_init(-1);
      itc_test_fifo_perfcount_set();
      printk("[CPU%d] %s\n", smp_processor_id(), __func__);
    while (!kthread_should_stop()) {
        itctest_semp(index);
        index = (index + 1) & 0xf;
            set_current_state(TASK_INTERRUPTIBLE);
        /* Now sleep between a min of 100-300us and a max of 1ms */
        schedule_timeout(testitc_fifo_sec*HZ);
    }
    return 0;
}

static void testitc_semp_test(void){
    int cpu;
        cpu =3;

    testitc_fifo_threads[cpu] = kthread_create(test_itc_semp, NULL, "test_ITC_semp/%d", cpu);
        if (WARN_ON(!testitc_fifo_threads[cpu])) {
            printk("[CPU%d] Create CPU%d's test_itc_semp failed!", smp_processor_id(), cpu);
            goto out_free;
        }

        kthread_bind(testitc_fifo_threads[cpu], cpu);
        wake_up_process(testitc_fifo_threads[cpu]);
out_free:
   return ;

}



/****************************
 * ITC Proc for Write
 * ************************/
static ssize_t bsp_testitc_write_proc(struct file *file, const char __user *buffer,
             size_t count, loff_t *ppos)
{
      char tmpbuf[32];
      int para0 = 0;
      int para1 = -1;
      int para2 = -1;
      int para_count = 0;
    /* no data be written */
    if (!count) {
        printk("count is 0\n");
        return 0;
    }

    /* Input size is too large to write our buffer(num) */
    if (count > (sizeof(tmpbuf) - 1)) {
        printk("input is too large\n");
        return -EINVAL;
    }

    if (copy_from_user(tmpbuf, buffer, count)) {
        printk("copy from user failed\n");
        return -EFAULT;
    }
    para_count = sscanf(tmpbuf, "%d %d 0x%x", &para0, &para1, &para2);

    printk("cpu(%d) input: para0 = %d, para1 = %d, para2 = 0x%x\n", smp_processor_id(),para0 , para1,  para2);

    if(para_count == 1){
      para1 = -1;
    }

   if(para0){
        bsp_itc_base_init(para1, para2);

    /***** FIFO TEST *************/
    if( para0 == 0x5 ){
      printk("cpu(%d), input %d: ITC FIFO TEST!\n\n", smp_processor_id(),para0);
      testitc_FIFO_test();
      goto out;
    }
    /***** SEMP TEST *************/
    if( para0 == 0x6 ){
      printk("cpu(%d), input %d: ITC SEMP TEST!\n\n", smp_processor_id(),para0);
      testitc_semp_test();
      goto out;
    }
    /****************************/
    if( para0 == 0x4 ){
      printk("cpu(%d), input %d: Just ITC Init!\n\n", smp_processor_id(),para0);
      goto out;
    }
        if( para0 == 0x2 ){
      printk("cpu(%d), input %d: testitc_lock_forever!\n\n", smp_processor_id(),para0);
      testitc_lock_forever();
      goto out;
    }
        if( para0 == 0x3 ){
      printk("cpu(%d), input %d: testitc_lock_case3!\n\n", smp_processor_id(),para0);
      testitc_lock_case3();
      goto out;
    }

        if( (para0 > 0xf) && (para0 < 0x20)){
      printk("cpu(%d), input %d: one etnry(%d) lock!\n\n", smp_processor_id(),para0, para0);
      testitc_lock_one_entry(para0);
      goto out;
    }

    if( (para0 > 0x10f) && (para0 < 0x120)){
      printk("cpu(%d), input %d: one etnry(%d) unlock!\n", smp_processor_id(),para0, (para0& 0xff));
      testitc_unlock_one_entry( (para0& 0xff));
      goto out;
    }

        if( (para0 > 0x1f) ){
       printk("cpu(%d), input %d: Setting sleep seconds!\n\n", smp_processor_id(),para0);
       testitc_seconds = para0;
    }

    testitc_thread_init(0);
   }else{
        testitc_thread_stop();
   }

out:
    return count;
}


static int bsp_testitc_proc_show(struct seq_file *m, void *v)
{

        unsigned long ectlval;
        unsigned long itcblkgrn;
        unsigned long itcbase;

        /* ErrCtl register is known as "ecc" to Linux */
        ectlval = read_c0_ecc();
        write_c0_ecc(ectlval | (0x1 << 26));
        ehb();
#define INDEX_0 (0x80000000)
#define INDEX_8 (0x80000008)
        /* Read "cache tag" for Dcache pseudo-index 8 */
        cache_op(Index_Load_Tag_D, INDEX_8);
        ehb();
        itcblkgrn = read_c0_dtaglo();
        printk("Got ITCAddressMap1 Register =0x%08lx\n", itcblkgrn);

        /*Read to ITU with CACHE op */
        cache_op(Index_Load_Tag_D, INDEX_0);
        ehb();
        itcbase = read_c0_dtaglo();
        printk("Got ITCAddressMap0 Register(BaseAddress) =0x%08lx\n", itcblkgrn);


        write_c0_ecc(ectlval);
        ehb();



    seq_printf(m, "ITC BaseAddress : 0x%08lx\n", itcbase);
    seq_printf(m, "ITC BaseAddress : %s\n", (itcbase & 0x1)?"enable":"disable");

    seq_printf(m, "ITCAddressMap1: 0x%08lx\n", itcblkgrn);
    seq_printf(m, "ITC NumEntries: 0x%lx\n", (itcblkgrn>>20) & 0x7ff);
    seq_printf(m, "ITC AddrMask  : 0x%lx\n", (itcblkgrn>>10) & 0x7f);
    seq_printf(m, "ITC EntryGrain: 0x%lx\n", (itcblkgrn&0x3));




    return 0;
}

static int bsp_testitc_proc_open(struct inode *inode, struct file *file)
{
    return single_open(file, bsp_testitc_proc_show, NULL);
}

static const struct file_operations bsp_testitc_proc_fops = {
    .open		= bsp_testitc_proc_open,
    .read		= seq_read,
    .write          = bsp_testitc_write_proc,
    .llseek		= seq_lseek,
    .release	= single_release,
};


/*************ITC FIFO TEST**********************
 *
 ***********************************************/

static void bsp_itcfifo_write(int index, unsigned int data){
    int offset = 0;

      unsigned int *ITC_Cell = NULL;
      unsigned int *ITC_Cell_Tag = NULL;
      unsigned int ITC_CELL_DATA = 0;
      offset = index*ITC_PITCH;
     ITC_Cell_Tag        = (unsigned int* )(((unsigned int)ITC_BlcokNC + offset) | ITC_Control_View);
     ITC_CELL_DATA = *ITC_Cell_Tag;
     printk("[CPU%d : %s] ITC_Cell | ITC_Control_View = %p,index=%d, Tag=0x%x\n", smp_processor_id(), __func__ ,ITC_Cell_Tag, index, ITC_CELL_DATA);
     printk("              Cell(%d) : FIFOPtr=%d, FULL=%d, EMPTY=%d\n", index, (ITC_CELL_DATA>>18)&0x7, (ITC_CELL_DATA>>1)&0x1, ITC_CELL_DATA&0x1);


    ITC_Cell        = (unsigned int* )(((unsigned int)ITC_BlcokNC + offset) | ITC_EF_Sync_View);

    *ITC_Cell = data;

     ITC_CELL_DATA = *ITC_Cell_Tag;
     printk("              Cell(%d) : FIFOPtr=%d, FULL=%d, EMPTY=%d\n", index, (ITC_CELL_DATA>>18)&0x7, (ITC_CELL_DATA>>1)&0x1, ITC_CELL_DATA&0x1);

}


static void bsp_itcfifo_get(int index){
    int offset = 0;

      unsigned int *ITC_Cell = NULL;
      unsigned int *ITC_Cell_Tag = NULL;
      unsigned int ITC_CELL_DATA = 0;
      offset = index*ITC_PITCH;
     ITC_Cell_Tag        = (unsigned int* )(((unsigned int)ITC_BlcokNC + offset) | ITC_Control_View);
     ITC_CELL_DATA = *ITC_Cell_Tag;
     printk("[CPU%d : %s] ITC_Cell | ITC_Control_View = %p,index=%d, Tag=0x%x\n", smp_processor_id(), __func__,ITC_Cell_Tag, index, ITC_CELL_DATA);
     printk("              Cell_TAG(%d) : FIFOPtr=%d, FULL=%d, EMPTY=%d\n", index, (ITC_CELL_DATA>>18)&0x7, (ITC_CELL_DATA>>1)&0x1, ITC_CELL_DATA&0x1);


    ITC_Cell        = (unsigned int* )(((unsigned int)ITC_BlcokNC + offset) | ITC_EF_Sync_View);

    ITC_CELL_DATA = *ITC_Cell;
     printk("              Cell(%d)=%p,  FIFO DATA: 0x%x\n",  index, ITC_Cell, ITC_CELL_DATA);

     ITC_CELL_DATA = *ITC_Cell_Tag;
     printk("              Cell_TAG(%d): FIFOPtr=%d, FULL=%d, EMPTY=%d\n", index, (ITC_CELL_DATA>>18)&0x7, (ITC_CELL_DATA>>1)&0x1, ITC_CELL_DATA&0x1);

}

static void bsp_itcfifo_read_first(int index){
    int offset = 0;

      unsigned int *ITC_Cell = NULL;
      unsigned int *ITC_Cell_Tag = NULL;
      unsigned int ITC_CELL_DATA = 0;
      offset = index*ITC_PITCH;
     ITC_Cell_Tag        = (unsigned int* )(((unsigned int)ITC_BlcokNC + offset) | ITC_Control_View);
     ITC_CELL_DATA = *ITC_Cell_Tag;
     printk("[CPU%d : %s] ITC_Cell | ITC_Control_View = %p,index=%d, Tag=0x%x\n", smp_processor_id(), __func__,  ITC_Cell_Tag, index, ITC_CELL_DATA);
     printk("              Cell_TAG(%d) : FIFOPtr=%d, FULL=%d, EMPTY=%d\n", index, (ITC_CELL_DATA>>18)&0x7, (ITC_CELL_DATA>>1)&0x1, ITC_CELL_DATA&0x1);



    ITC_Cell        = (unsigned int* )(((unsigned int)ITC_BlcokNC + offset) | ITC_Bypass_View);

    ITC_CELL_DATA = *ITC_Cell;
     printk("              Cell(%d)=%p,  FIFO DATA: 0x%x\n",  index, ITC_Cell, ITC_CELL_DATA);

     ITC_CELL_DATA = *ITC_Cell_Tag;
     printk("              Cell_TAG(%d): FIFOPtr=%d, FULL=%d, EMPTY=%d\n", index, (ITC_CELL_DATA>>18)&0x7, (ITC_CELL_DATA>>1)&0x1, ITC_CELL_DATA&0x1);

}
/****************************
 * ITC Proc for Write
 * ************************/
static ssize_t bsp_itcfifo_write_proc(struct file *file, const char __user *buffer,
             size_t count, loff_t *ppos)
{
   char tmpbuf[32];
   unsigned int index = 0;
   unsigned int data = 0;
   unsigned int type = 0;

   int i = 0;
    /* no data be written */
    if (!count) {
        printk("count is 0\n");
        return 0;
    }

    /* Input size is too large to write our buffer(num) */
    if (count > (sizeof(tmpbuf) - 1)) {
        printk("input is too large\n");
        return -EINVAL;
    }

    if (copy_from_user(tmpbuf, buffer, count)) {
        printk("copy from user failed\n");
        return -EFAULT;
    }
    i = sscanf(tmpbuf, "%d %d 0x%x" , &type, &index, &data);

    printk("[CPU%d]input type:%u index:%u, data: %x \n", smp_processor_id(), type, index,  data);
    if( i <2 ){
        return -EINVAL;
    }


    if(type == 0){
      bsp_itcfifo_read_first(index);
    }
    else
    if(type == 1){
      bsp_itcfifo_get(index);
    }
    else
    if((type == 2) && i == 3){
      bsp_itcfifo_write(index, data);
    }


    return count;
}


static int bsp_itcfifo_proc_show(struct seq_file *m, void *v)
{




  unsigned int* ITC_Cell;
  unsigned int offset;

  int i;
  for(i=0; i<ITC_CELL; i++){
    offset = i*ITC_PITCH;
    /* control_view to change tag */
    ITC_Cell = (unsigned int* )(((unsigned int)ITC_BlcokNC + offset) | ITC_Control_View);
   seq_printf(m, "CELL(%d):%p   Tag = 0x%4x\n", i, ITC_Cell, *ITC_Cell);
    ITC_Cell = (unsigned int* )(((unsigned int)ITC_BlcokNC + offset) | ITC_Bypass_View);
  seq_printf(m, "CELL(%d):%p  Data = 0x%4x\n", i, ITC_Cell, *ITC_Cell);
  }

    return 0;
}



static int bsp_itcfifo_proc_open(struct inode *inode, struct file *file)
{
    return single_open(file, bsp_itcfifo_proc_show, NULL);
}

static const struct file_operations bsp_itcfifo_proc_fops = {
    .open		= bsp_itcfifo_proc_open,
    .read		= seq_read,
    .write          = bsp_itcfifo_write_proc,
    .llseek		= seq_lseek,
    .release	= single_release,
};



/**************************************************/



/*************ITC Semp TEST**********************
 *
 ***********************************************/

static void bsp_itcsemp_write(int index, unsigned int data){
    int offset = 0;

      unsigned int *ITC_Cell = NULL;
      unsigned int *ITC_Cell_Tag = NULL;
      unsigned int ITC_CELL_DATA = 0;
      offset = index*ITC_PITCH;
     ITC_Cell_Tag        = (unsigned int* )(((unsigned int)ITC_BlcokNC + offset) | ITC_Control_View);
     ITC_CELL_DATA = *ITC_Cell_Tag;
     printk("[CPU%d : %s] ITC_Cell | ITC_Control_View = %p,index=%d, Tag=0x%x\n", smp_processor_id(), __func__ ,ITC_Cell_Tag, index, ITC_CELL_DATA);

    ITC_Cell        = (unsigned int* )(((unsigned int)ITC_BlcokNC + offset) | ITC_PV_Sync_View);

    *ITC_Cell = data;

     printk("[CPU%d : %s] ITC_Cell | ITC_PV_Sync_View = %p,index=%d, Data <= 0x%x\n", smp_processor_id(), __func__ ,ITC_Cell, index, data);

}


static void bsp_itcsemp_get(int index){
    int offset = 0;

      unsigned int *ITC_Cell = NULL;
      unsigned int *ITC_Cell_Tag = NULL;
      unsigned int ITC_CELL_DATA = 0;
      offset = index*ITC_PITCH;
     ITC_Cell_Tag        = (unsigned int* )(((unsigned int)ITC_BlcokNC + offset) | ITC_Control_View);
     ITC_CELL_DATA = *ITC_Cell_Tag;
     printk("[CPU%d : %s] ITC_Cell | ITC_Control_View = %p,index=%d, Tag=0x%x\n", smp_processor_id(), __func__,ITC_Cell_Tag, index, ITC_CELL_DATA);


    ITC_Cell        = (unsigned int* )(((unsigned int)ITC_BlcokNC + offset) | ITC_PV_Sync_View);

    ITC_CELL_DATA = *ITC_Cell;
     printk("              Cell(%d)=%p,   DATA: 0x%x\n",  index, ITC_Cell, ITC_CELL_DATA);

     ITC_CELL_DATA = *ITC_Cell_Tag;
     printk("[CPU%d : %s] ITC_Cell | ITC_Control_View = %p,index=%d, Tag=0x%x\n", smp_processor_id(), __func__,ITC_Cell_Tag, index, ITC_CELL_DATA);

}

static void bsp_itcsemp_read_first(int index){
    int offset = 0;

      unsigned int *ITC_Cell = NULL;
      unsigned int *ITC_Cell_Tag = NULL;
      unsigned int ITC_CELL_DATA = 0;
      offset = index*ITC_PITCH;
     ITC_Cell_Tag        = (unsigned int* )(((unsigned int)ITC_BlcokNC + offset) | ITC_Control_View);
     ITC_CELL_DATA = *ITC_Cell_Tag;
     printk("[CPU%d : %s] ITC_Cell | ITC_Control_View = %p,index=%d, Tag=0x%x, type=%s\n", smp_processor_id(), __func__,  ITC_Cell_Tag, index, ITC_CELL_DATA,
        (ITC_CELL_DATA&0x20000)?"FIFO":"SEMP");


    ITC_Cell        = (unsigned int* )(((unsigned int)ITC_BlcokNC + offset) | ITC_Bypass_View);

    ITC_CELL_DATA = *ITC_Cell;
     printk("  ITC_Bypass_View  Cell(%d)=%p,  DATA: 0x%x\n",  index, ITC_Cell, ITC_CELL_DATA);

}
/****************************
 * ITC Proc for Write
 * ************************/
static ssize_t bsp_itcsemp_write_proc(struct file *file, const char __user *buffer,
             size_t count, loff_t *ppos)
{
   char tmpbuf[32];
   unsigned int index = 0;
   unsigned int data = 0;
   unsigned int type = 0;

   int i = 0;
    /* no data be written */
    if (!count) {
        printk("count is 0\n");
        return 0;
    }

    /* Input size is too large to write our buffer(num) */
    if (count > (sizeof(tmpbuf) - 1)) {
        printk("input is too large\n");
        return -EINVAL;
    }

    if (copy_from_user(tmpbuf, buffer, count)) {
        printk("copy from user failed\n");
        return -EFAULT;
    }
    i = sscanf(tmpbuf, "%d %d 0x%x" , &type, &index, &data);

    printk("[CPU%d]input type:%u index:%u, data: %x \n", smp_processor_id(), type, index,  data);
    if( i <2 ){
        return -EINVAL;
    }


    if(type == 0){
      bsp_itcsemp_read_first(index);
    }
    else
    if(type == 1){
      bsp_itcsemp_get(index);
    }
    else
    if((type == 2) && i == 3){
      bsp_itcsemp_write(index, data);
    }


    return count;
}


static int bsp_itcsemp_proc_show(struct seq_file *m, void *v)
{
  unsigned int* ITC_Cell;
  unsigned int offset;

  int i;
  for(i=0; i<ITC_CELL; i++){
    offset = i*ITC_PITCH;
    /* control_view to change tag */
    ITC_Cell = (unsigned int* )(((unsigned int)ITC_BlcokNC + offset) | ITC_Control_View);
    seq_printf(m, "CELL(%d):%p   Tag = 0x%4x, type=%s\n", i, ITC_Cell, *ITC_Cell, (*ITC_Cell)&0x20000?"FIFO":"Semaphore" );
    ITC_Cell = (unsigned int* )(((unsigned int)ITC_BlcokNC + offset) | ITC_Bypass_View);
    seq_printf(m, "CELL(%d):%p  Data = 0x%4x\n", i, ITC_Cell, *ITC_Cell);
  }

    return 0;
}



static int bsp_itcsemp_proc_open(struct inode *inode, struct file *file)
{
    return single_open(file, bsp_itcsemp_proc_show, NULL);
}

static const struct file_operations bsp_itcsemp_proc_fops = {
    .open		= bsp_itcsemp_proc_open,
    .read		= seq_read,
    .write          = bsp_itcsemp_write_proc,
    .llseek		= seq_lseek,
    .release	= single_release,
};






/**************************************************/
static int __init init_modules(void)
{

   proc_create("bsp_dw", 0444, NULL, &bsp_dw_proc_fops);
   proc_create("bsp_l2cache", 0444, NULL, &bsp_l2cache_proc_fops);
   proc_create("bsp_PCache_test", 0444, NULL, &bsp_testcache_proc_fops);
   proc_create("bsp_ITC_test", 0444, NULL, &bsp_testitc_proc_fops);

   proc_create("bsp_ITC_fifo", 0444, NULL, &bsp_itcfifo_proc_fops);

   proc_create("bsp_ITC_semp", 0444, NULL, &bsp_itcsemp_proc_fops);

    return 0;
}

static void __exit exit_modules(void)
{

}

module_init(init_modules);
module_exit(exit_modules);
