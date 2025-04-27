#ifndef __MT_RAM_TEST_H__
#define __MT_RAM_TEST_H__


/* Memory test status */
#define MT_SUCCESS (0)
#define MT_FAIL    (-1)

enum WR_ADDR_CASE
{
    MT_UW_UR=0, //Uncache Write, Uncache Read
    MT_UW_CR=1, //Uncache Write, Cache Read
    MT_CW_CR=2, //Cache Write, Cache Read
    MT_CW_UR=3, //Cache Write, Uncache Read
};



typedef unsigned int (mt_ram_wrcase_t)(u32_t ram_start_addr, u32_t size_per_pat, u32_t ram_test_range, enum WR_ADDR_CASE wr_case);
typedef unsigned int (mt_ram_t)(u32_t ram_start_addr, u32_t size_per_pat, u32_t ram_test_range);

int mt_normal_patterns(u32_t ram_start_addr, u32_t size_per_pat, u32_t ram_test_range, enum WR_ADDR_CASE wr_case);
int mt_word_pattern_rotate(u32_t ram_start_addr, u32_t size_per_pat, u32_t ram_test_range, enum WR_ADDR_CASE wr_case);
int mt_halfword_pattern_rotate(u32_t ram_start_addr, u32_t size_per_pat, u32_t ram_test_range, enum WR_ADDR_CASE wr_case);
int mt_byte_pattern_rotate(u32_t ram_start_addr, u32_t size_per_pat, u32_t ram_test_range, enum WR_ADDR_CASE wr_case);
int mt_unaligned_wr(u32_t ram_start_addr, u32_t size_per_pat, u32_t ram_test_range, enum WR_ADDR_CASE wr_case);
int mt_com_addr_rot(u32_t ram_start_addr, u32_t size_per_pat, u32_t ram_test_range);
int mt_walking_of_1(u32_t ram_start_addr, u32_t size_per_pat, u32_t ram_test_range);
int mt_walking_of_0(u32_t ram_start_addr, u32_t size_per_pat, u32_t ram_test_range);
int mt_addr_rot(u32_t ram_start_addr, u32_t size_per_pat, u32_t ram_test_range);
int mt_memcpy(u32_t ram_start_addr, u32_t size_per_pat, u32_t ram_test_range);

#endif // __MT_RAM_TEST_H__

