
#ifndef _RLX5281_H_
#define _RLX5281_H_
/* CP3 Performance Counter Event list */
#define CP3_COUTER3(event)         (event << 24)
#define CP3_COUTER2(event)         (event << 16)
#define CP3_COUTER1(event)         (event << 8 )
#define CP3_COUTER0(event)         (event << 0 )

//#ifdef CONFIG_RTL8198
#define INST_FEATCH                (0x01)
#define ICACHE_MISS                (0x02)
#define ICACHE_MISS_CYCLE          (0x03)
#define STORE_INST                 (0x04)
#define LOAD_INST                  (0x05)
#define LORDSTROE_INST             (0x06)
#define COMPLETED_INST             (0x07)
#define CYCLES                     (0x08)
#define DCACHE_MISSE               (0x0A)
#define DCACHE_MISS_CYCLE          (0x0B)
#define L2_CACHE_HIT               (0x0C)
#define L2_CACHE_HIT_CYCLE         (0x0D)
#define L2_CACHE_MISS              (0x0E)
#define L2_CACHE_MISS_CYCLE        (0x0F)
#define BRANCEH_PREDICTION         (0x10)
#define BRANCEH_PREDICTION_MISS    (0x11)
#define BTB_MISS                   (0x12)
#define BTB_TARGET_MISS            (0x13)
#define CBP_MISS                   (0x14)
#define RS_POP                     (0x15)
#define RS_MISS                    (0x16)
#define IBUF_FULL_CYCLE            (0x18)
#define IBUF_EMPTY_CYCLE           (0x19)
#define ZERO_INST_COMMIT_CYCLE     (0x20)
#define PIPE_A_INST_COMMIT_CYCLE   (0x21)
#define PIPE_B_INST_COMMIT_CYCLE   (0x22)
#define DUAL_INST_COMMIT_CYCLE     (0x23)
#define OCP_BUS_READS              (0x24)
#define OCP_BUS_WRITES             (0x25)
#define OCP_BUS_UC_READ            (0x26)
#define OCP_BUS_UC_WRITE           (0x27)
#define OCP_BUS_DATA_READ          (0x28)
#define OCP_BUS_DATA_WRITE         (0x29)
#define OCP_BUS_INS_READ           (0x2A)
//#endif


#if defined(CONFIG_RTL8196) || defined(CONFIG_RTL8672)
#define INST_FEATCH                (0x11)
#define ICACHE_MISS                (0x12)
#define ICACHE_MISS_CYCLE          (0x13)
#define STORE_INST                 (0x14)
#define LOAD_INST                  (0x15)
#define LORDSTROE_INST             (0x16)
#define COMPLETED_INST             (0x17)
#define CYCLES                     (0x10)
#define DCACHE_MISSE               (0x1A)
#define DCACHE_MISS_CYCLE          (0x1B)
#endif


#ifndef __ASSEMBLY__
/*CP3 utilities*/
extern void enable_CP3(void);
extern void disable_IMEM0(void);
extern void disable_IMEM1(void);
extern void disable_DMEM0(void);
extern void disable_DMEM1(void);
extern void disable_L2MEM(void);
extern void enable_IMEM0(void);
extern void enable_IMEM1(void);
extern void enable_DMEM0(void);
extern void enable_DMEM1(void);
extern void enable_L2MEM(void);
extern void load_imem0Range(unsigned int top, unsigned int base);
extern void load_imem1Range(unsigned int top, unsigned int base);
extern void load_dmem0Range(unsigned int top, unsigned int base);
extern void load_dmem1Range(unsigned int top, unsigned int base);
extern void load_l2memRange(unsigned int top, unsigned int base);
extern void IMEM0_refill(void);
extern void IMEM1_refill(void);
extern void clear_CP3_counter(void);
extern void set_and_start_counter(unsigned int ctr_event);
extern void stop_counter(void);
extern void save_counter(unsigned int c0_addr, unsigned int c1_addr, unsigned int c2_addr, unsigned int c3_addr);
/*cache utilities*/
extern void invalidate_cache(void);
extern void invalidate_icache(void);
extern void write_back_invalidate_dcache(void);
extern void write_back_dcache(void);
extern void read_imem_8bytes(unsigned int *addr, unsigned int *buff);
extern void write_imem_8bytes(unsigned int hi, unsigned int low, unsigned int *addr);
extern unsigned int _read_cp0_pid(void);
#endif //__ASSEMBLY__

#endif //_RLX5281_H_
