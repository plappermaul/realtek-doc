// ------------------------------------------------------------------------
//
//                (C) COPYRIGHT 2011 - 2015 SYNOPSYS, INC.
//                          ALL RIGHTS RESERVED
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  version 2 as published by the Free Software Foundation.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, see <https://gnu.org/licenses/>.
//
// ------------------------------------------------------------------------

#ifndef ELP_MPM_H
#define ELP_MPM_H

//#include "elppdu.h"
//#include "elpspacc.h"

/* fields of a PDU buffer */
#define PDU_DESC_SRC_PTR            0x00/4
#define PDU_DESC_DST_PTR            0x04/4
#define PDU_DESC_OFFSET             0x08/4
#define PDU_DESC_PRE_AAD_LEN        0x0C/4
#define PDU_DESC_POST_AAD_LEN       0x10/4
#define PDU_DESC_PROC_LEN           0x14/4
#define PDU_DESC_ICV_LEN            0x18/4
#define PDU_DESC_ICV_OFFSET         0x1C/4
#define PDU_DESC_IV_OFFSET          0x20/4
#define PDU_DESC_AUX_INFO           0x24/4
#define PDU_DESC_CTRL               0x28/4
#define PDU_DESC_KEY_PTR            0x2C/4
#define PDU_DESC_NEXT_PDU           0x30/4
#define PDU_DESC_CIPH_IV            0x40/4
#define PDU_DESC_HASH_IV            0x50/4
#define PDU_DESC_STATUS             0x70/4

//typedef uint32_t pdubuf[0x80/4];

// fields inside

#define _PDU_DESC_CTRL_CIPH_ALG      0
#define _PDU_DESC_CTRL_HASH_ALG      4
#define _PDU_DESC_CTRL_CIPH_MODE     8
#define _PDU_DESC_CTRL_HASH_MODE    12

#define _PDU_DESC_CTRL_CIPH_ALG_415      0
#define _PDU_DESC_CTRL_HASH_ALG_415      3
#define _PDU_DESC_CTRL_CIPH_MODE_415     8
#define _PDU_DESC_CTRL_HASH_MODE_415    12

#define _PDU_DESC_CTRL_ENCRYPT      24
#define _PDU_DESC_CTRL_AAD_COPY     25
#define _PDU_DESC_CTRL_ICV_PT       26
#define _PDU_DESC_CTRL_ICV_ENC      27
#define _PDU_DESC_CTRL_ICV_APPEND   28
#define _PDU_DESC_CTRL_DEMAND       29

#define PDU_DESC_CTRL_ENCRYPT       (1UL << _PDU_DESC_CTRL_ENCRYPT)
#define PDU_DESC_CTRL_AAD_COPY      (1UL << _PDU_DESC_CTRL_AAD_COPY)
#define PDU_DESC_CTRL_ICV_PT        (1UL << _PDU_DESC_CTRL_ICV_PT)
#define PDU_DESC_CTRL_ICV_ENC       (1UL << _PDU_DESC_CTRL_ICV_ENC)
#define PDU_DESC_CTRL_ICV_APPEND    (1UL << _PDU_DESC_CTRL_ICV_APPEND)
#define PDU_DESC_CTRL_DEMAND        (1UL << _PDU_DESC_CTRL_DEMAND)

#define _PDU_DESC_STATUS_CTX_ID     0
#define _PDU_DESC_STATUS_CACHE_CODE 8
#define _PDU_DESC_STATUS_KEY_EXP    10
#define _PDU_DESC_STATUS_DONE       16
#define _PDU_DESC_STATUS_RET_CODE   24

#define PDU_DESC_STATUS_CTX_ID(x)      ((x >> _PDU_DESC_STATUS_CTX_IDX) & 0x0F)
#define PDU_DESC_STATUS_CACHE_CODE(x)  ((x >> _PDU_DESC_STATUS_CACHE_CODE) & 0x03)
#define PDU_DESC_STATUS_KEY_EXP        (1UL << _PDU_DESC_STATUS_KEY_EXP)
#define PDU_DESC_STATUS_DONE           (1UL << _PDU_DESC_STATUS_DONE)
#define PDU_DESC_STATUS_RET_CODE(x)    ((x >> _PDU_DESC_STATUS_RET_CODE) & 0xFF)


/* fields of a key buffer */
#define KEY_BUF_CTRL                0x00
#define KEY_BUF_CIPH_KEY            0x40
#define KEY_BUF_HASH_KEY            0x80

/* fields inside the fields ... */
#define _KEY_BUF_CTRL_CKEY_SZ       0
#define _KEY_BUF_CTRL_HKEY_SZ       8
#define _KEY_BUF_CTRL_CACHED        29
#define _KEY_BUF_CTRL_NO_CACHE      30
#define _KEY_BUF_CTRL_INV_CKEY      31

#define KEY_BUF_CTRL_CKEY_SZ(x)     (x << _KEY_BUF_CTRL_CKEY_SZ)
#define KEY_BUF_CTRL_HKEY_SZ(x)     (x << _KEY_BUF_CTRL_HKEY_SZ)
#define KEY_BUF_CTRL_NOCACHE        (1UL << _KEY_BUF_CTRL_NO_CACHE)
#define KEY_BUF_CTRL_INV_CKEY       (1UL << _KEY_BUF_CTRL_INV_CKEY)

//typedef uint32_t keybuf[64];

/* MPM registers */

#define MPM_IRQ_EN                  0x00
#define MPM_IRQ_STAT                0x04
#define MPM_IRQ_CTRL                0x08
#define MPM_START                   0x20
#define MPM_STAT                    0x24
#define MPM_STAT_CNT_ACTIVE         0x30
#define MPM_STAT_CNT_WAIT           0x34
#define MPM_STAT_SPACC_ACTIVE       0x38
#define MPM_STAT_IRQ_ASSERT         0x3C
#define MPM_CTX_CMD                 0x40
#define MPM_CTX_STAT                0x44

#define _MPM_IRQ_EN_DEMAND          0
#define _MPM_IRQ_EN_CNT             1
#define _MPM_IRQ_EN_EOL             2
#define _MPM_IRQ_EN_GLBL           31
#define MPM_IRQ_EN_DEMAND           (1UL << _MPM_IRQ_EN_DEMAND)
#define MPM_IRQ_EN_CNT              (1UL << _MPM_IRQ_EN_CNT)
#define MPM_IRQ_EN_EOL              (1UL << _MPM_IRQ_EN_EOL)
#define MPM_IRQ_EN_GLBL             (1UL << _MPM_IRQ_EN_GLBL)

#define _MPM_IRQ_STAT_DEMAND        0
#define _MPM_IRQ_STAT_CNT           1
#define _MPM_IRQ_STAT_EOL           2
#define MPM_IRQ_STAT_DEMAND         (1UL << _MPM_IRQ_STAT_DEMAND)
#define MPM_IRQ_STAT_CNT            (1UL << _MPM_IRQ_STAT_CNT)
#define MPM_IRQ_STAT_EOL            (1UL << _MPM_IRQ_STAT_EOL)

#define _MPM_STAT_BUSY              0
#define _MPM_STAT_CTX_ERR           1
#define _MPM_STAT_PTR               7
#define MPM_STAT_BUSY               (1UL << _MPM_STAT_BUSY)
#define MPM_STAT_CTX_ERR            (1UL << _MPM_STAT_CTX_ERR)
#define MPM_STAT_PTR(x)             ((x) >> _MPM_STAT_PTR)

#define _MPM_CTX_CMD_IDX            0
#define _MPM_CTX_CMD_ALLOC         31
#define MPM_CTX_CMD_REQ(idx,alloc) (((uint32_t)(idx) << _MPM_CTX_CMD_IDX) \
                                   | ((uint32_t)(alloc) << _MPM_CTX_CMD_ALLOC))

#if 0
enum {
   CHAIN_FREE=0,
   CHAIN_BUILDING,
   CHAIN_BUILT,
   CHAIN_RUNNING,
   CHAIN_DONE,
   CHAIN_PROCESSING,
};

typedef void (*mpm_callback)(void *mpm_dev, void *data, int pdu_idx, int key_idx, uint32_t status);
typedef void (*mpm_chain_callback)(void *mpm_dev, void *chain_id);

typedef struct {
   int state,
       curidx;

   int *pdus;
   int *keys;
   mpm_callback *callbacks;
   void        **callback_data;

   mpm_chain_callback per_chain_callback;
   void               *per_chain_id;

   uint32_t slottime[2];
} mpm_chain;

#ifdef MPM_PERF_MON
#define MPM_PERF_INCR(x, v) x += v
#else
#define MPM_PERF_INCR(x, v)
#endif

typedef struct {
   void         *regmap;
   spacc_device *spacc;

   /* total pool of PDUs and KEY buffers that have been pre allocated */
   struct {
      pdubuf         *pdus;               // array of PDU descriptors (must be contiguous in physical memory)
      PDU_DMA_ADDR_T  pdus_phys;          // address of first in physical memory
      int *freepdu, freepdu_idx;  // stack of free PDU indecies

      int *ondemand, ondemand_idx; // ondemand list, defaults to -1 to indicate not a job
                                   // 'ondemand' must be small since the IRQ searches the entire array
      mpm_callback *ondemand_cb;   // callback list for ondemand jobs
      void        **ondemand_cb_data;

      int *ondemand_mask;          // bitmask for ENTIRE pdu array (#pdus), set to -1 to indicate the PDU entry is not demand
                                   // set to >= 0 as an index into ondemand/ondemand_cb
   } pdu;

   struct {
      keybuf         *keys;
      PDU_DMA_ADDR_T  keys_phys;
      int *freekey, freekey_idx;
      int *key_ref_cnt;
   } key;

   mpm_chain *chains;
   volatile int
              chain_idx,          // idx of where we can put new jobs in (or pointer to a FREE one)
              active_chain_idx;   // last issued idx for IRQ handler to mark it as cleared

   struct {
      int no_chains,
          no_pdus,
          no_keys,
          no_chain_links,
          no_demand,
          id;
   } config;

   struct {
      uint32_t
          eol_irqs,               // count of irqs
          demand_irqs,            // count of demand irqs
          job_inserted,           // count of jobs inserted into chains
          job_cleared,            // count of jobs cleared from chains

          key_full_hit,           // KEY full hit to context, no MPM traffic to use key
          key_partial_hit,        // KEY partial hit, some re-loading (IV/etc)
          key_miss,               // KEY miss, MPM had to fully load key/iv

          demand_caught,          // jobs caught during a DEMAND IRQ
          demand_downgrade,       // count of demand packets that had to drop down in status because the demand queue was full
          demand_missed;

      uint64_t
          active_counter,         // last reading of active counter
          spacc_counter,
          irq_counter,
          idle_counter;           // last reading of idle counter

   } perf;

   /* amount of bytes required by both */
   size_t pdus_mem_req,
          keys_mem_req;

   /* master lock */
   PDU_LOCK_TYPE_BH lock;

   /* busy */
   volatile int busy,
                work_to_clear;
} mpm_device;

mpm_device *mpm_get_device(void);
mpm_device *mpm_get_device_by_num(int id);

int mpm_init(mpm_device *mpm, void *regmap, spacc_device *spacc, int no_chains, int no_pdus, int no_keys, int no_chain_links, int no_demand);
int mpm_deinit(mpm_device *mpm);

int mpm_req_spacc_ctx(mpm_device *mpm, int no_ctx, int *ctxs);
int mpm_free_spacc_ctx(mpm_device *mpm, int no_ctx, int *ctxs);

void mpm_cleanup(mpm_device *mpm);

int mpm_alloc_pdu(mpm_device *mpm);
int mpm_alloc_key(mpm_device *mpm);
int mpm_free_key(mpm_device *mpm, int key_idx);

int mpm_set_key(mpm_device *mpm, int key_idx, int ckey_sz, int ckey_copysz, int hkey_sz, int hkey_copysz, int no_cache, int inv_ckey, unsigned char *ckey, unsigned char *hkey);
int mpm_insert_pdu(mpm_device *mpm,
                   int pdu_idx, int key_idx, int ondemand,
                   mpm_callback cb, void *cb_data,
                   pdu_ddt *src, pdu_ddt *dst, uint32_t pre_aad_len, uint32_t post_aad_len, uint32_t proc_len,
                   uint32_t icv_len, uint32_t icv_offset, uint32_t iv_offset, uint32_t aux_info, uint32_t ctrl,
                   unsigned char *ciph_iv, unsigned char *hash_iv);

int mpm_enqueue_chain(mpm_device *mpm, mpm_chain_callback cb, void *id);

uint32_t mpm_get_status_word(mpm_device *mpm, int pdu_idx);

void mpm_process_int_demand(mpm_device *mpm);
void mpm_process_int_eol(mpm_device *mpm);


char *mpm_stats(mpm_device *mpm);
#define mpm_reset_stats(mpm) memset(&(mpm->perf), 0, sizeof(mpm->perf))

// helper to build ctrl
#define MPM_SET_CTRL(mpm, ciph, hash, ciphmode, hashmode, encrypt, aadcopy, icv_pt, icv_enc, icv_append) \
   ((ciph)       <<  ((mpm)->spacc->config.version <= 0x4E ? _PDU_DESC_CTRL_CIPH_ALG : _PDU_DESC_CTRL_CIPH_ALG_415))  | \
   ((hash)       <<  ((mpm)->spacc->config.version <= 0x4E ? _PDU_DESC_CTRL_HASH_ALG : _PDU_DESC_CTRL_HASH_ALG_415))  | \
   ((ciphmode)   <<  ((mpm)->spacc->config.version <= 0x4E ? _PDU_DESC_CTRL_CIPH_MODE : _PDU_DESC_CTRL_CIPH_MODE_415)) | \
   ((hashmode)   <<  ((mpm)->spacc->config.version <= 0x4E ? _PDU_DESC_CTRL_HASH_MODE : _PDU_DESC_CTRL_HASH_MODE_415))  | \
   ((encrypt)    << _PDU_DESC_CTRL_ENCRYPT)    | \
   ((aadcopy)    << _PDU_DESC_CTRL_AAD_COPY)   | \
   ((icv_pt)     << _PDU_DESC_CTRL_ICV_PT)     | \
   ((icv_enc)    << _PDU_DESC_CTRL_ICV_ENC)    | \
   ((icv_append) << _PDU_DESC_CTRL_ICV_APPEND)

/* called internally */
void mpm_sync_chain_for_device(mpm_device *mpm, int idx);
void mpm_sync_chain_for_cpu(mpm_device *mpm, int idx);
int mpm_clear_chains(mpm_device *mpm);
void mpm_kernel_schedule_tasklet(void);
void mpm_kernel_schedule_tasklet_by_num(int id);
#endif

#endif

