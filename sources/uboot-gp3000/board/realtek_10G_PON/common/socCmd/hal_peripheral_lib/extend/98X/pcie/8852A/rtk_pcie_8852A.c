#include "common.h"
#include "../../../../base/pcie/dw_4_60a/dw_pcie_base.h"
#include "./halmac/txdesc.h"
#include "./include/mac_ax_addrinfo_n.h"
#include "./include/mac_ax_addrinfo_t.h"
#include "./include/mac_ax_bd_info.h"
#include "./include/mac_ax_rxbd_pkt.h"
#include "./include/mac_ax_rxbd_sepn.h"
#include "./include/mac_ax_rxbd_sept.h"
#include "./include/mac_ax_txbd.h"
#include "rtk_pcie_8852A.h"
#include "wlan_ctrl.h"

// Global buffer for TXBD/RXBD
unsigned char PCIEDMA_TXBD[PCIEDMA_CH_TXBD_TOTAL_SIZE * PCIEDMA_TX_CH_NUM] __attribute__ ((aligned (TXBD_4K_BOUNDARY)));
unsigned char PCIEDMA_RXBD[PCIEDMA_CH_RXBD_TOTAL_SIZE * PCIEDMA_RX_CH_NUM] __attribute__ ((aligned (CACHE_LINE_SIZE)));

// Global buffer for TX
unsigned char PCIEDMA_TX_BUF[PCIEDMA_TX_BUF_TOTAL_SIZE]  __attribute__ ((aligned (CACHE_LINE_SIZE)));

// Global buffer for RX
#if defined(CONFIG_SEPARATION_MODE)
// Note: current code doesn't support separation mode for RPQ now!
// Need define new RXBD structure for RPQ
unsigned char PCIEDMA_RX_BUF_P1[PCIEDMA_RX_BUF_P1_TOTAL_SIZE]  __attribute__ ((aligned (CACHE_LINE_SIZE)));
#endif
unsigned char PCIEDMA_RX_BUF_P2[PCIEDMA_RX_BUF_P2_TOTAL_SIZE * PCIEDMA_RX_CH_NUM]  __attribute__ ((aligned (CACHE_LINE_SIZE)));

static void PCIEDMA_TXBD_fill(PCIEDMA_TX_PARA* tx_para_p, u32 txbd_index)
{
    u32 base_addr, index;
    u32 ls, len, addr;
    u32 channel = tx_para_p->channel;
    u32 used_txbd_num = tx_para_p->used_txbd_num;
    TXBD_TYPE *txbd_p;

    u32 txbd_base_addr[PCIEDMA_TX_CH_NUM] = {   PCIEDMA_TXBD_ACH0_BASEADDR,
                                                PCIEDMA_TXBD_ACH1_BASEADDR,
                                                PCIEDMA_TXBD_CH12_BASEADDR};

    base_addr = txbd_base_addr[channel];
    txbd_p = (TXBD_TYPE*) base_addr;
    txbd_p += txbd_index;

    // Set TXBD Format
    for(index = 0 ; index < used_txbd_num ; index++)
    {
        ls = (u32)tx_para_p->ls[index];
        addr = (u32)tx_para_p->buf_addr[index];
        len =  (u32)tx_para_p->buf_len[index];

#if !defined(CONFIG_TX_TRUNC_MODE)
        FILL_TXBD(txbd_p, len, ls, vir2phy(addr), 0);
#else
        FILL_TXBD(txbd_p, len, ls, vir2phy(addr));
#endif
        if(((txbd_index + index + 1) % PCIEDMA_CH_TXBD_NUM) == 0){
            txbd_p = (TXBD_TYPE*) base_addr;
        }else{
            txbd_p++; 
        }
    }

    // Flush all TXBD one time
    flush_dcache_range(txbd_base_addr[channel], txbd_base_addr[channel] + PCIEDMA_CH_TXBD_TOTAL_SIZE);
}

static void PCIEDMA_RXBD_fill(u32 channel)
{
    u32 base_addr, index;
#if defined(CONFIG_SEPARATION_MODE)
    u32 len1, addr1;
#endif
    u32 len2, addr2;
    RXBD_TYPE *rxbd_p;

    u32 rxbd_base_addr[PCIEDMA_RX_CH_NUM] = {   PCIEDMA_RXBD_RXQ_BASEADDR,
                                                PCIEDMA_RXBD_RPQ_BASEADDR};
#if defined(CONFIG_SEPARATION_MODE)
    u32 rx_buf1_ba =    PCIEDMA_RX_BUF_P1_BASEADDR;
    u32 rx_buf1_size =  PCIEDMA_RX_BUF_P1_SIZE;
#endif
    u32 rx_buf2_ba[PCIEDMA_RX_CH_NUM] =     {   PCIEDMA_RXQ_BUF_P2_BASEADDR,
                                                PCIEDMA_RPQ_BUF_P2_BASEADDR};
    u32 rx_buf2_size[PCIEDMA_RX_CH_NUM] =   {   PCIEDMA_RX_BUF_P2_SIZE,
                                                PCIEDMA_RX_BUF_P2_SIZE};

    base_addr = rxbd_base_addr[channel];
    rxbd_p = (RXBD_TYPE*) base_addr;

#if defined(CONFIG_SEPARATION_MODE)
    len1 = rx_buf1_size;
    addr1 = rx_buf1_ba;
#endif
    len2 = rx_buf2_size[channel];
    addr2 = rx_buf2_ba[channel];

    for(index = 0; index < PCIEDMA_CH_RXBD_NUM; index++)
    {
        if(index == PCIEDMA_RX_RPQ)
        {
            // Don't support separation mode for RPQ
#if !defined(CONFIG_RX_TRUNC_MODE)
            FILL_RXBD(rxbd_p, len2, vir2phy(addr2), 0);
#elif defined(CONFIG_RX_TRUNC_MODE)
            FILL_RXBD(rxbd_p, len2, vir2phy(addr2));
#endif
        }else{
#if !defined(CONFIG_RX_TRUNC_MODE) && !defined(CONFIG_SEPARATION_MODE)
            FILL_RXBD(rxbd_p, len2, vir2phy(addr2), 0);
#elif defined(CONFIG_RX_TRUNC_MODE) && !defined(CONFIG_SEPARATION_MODE)
            FILL_RXBD(rxbd_p, len2, vir2phy(addr2));
#elif !defined(CONFIG_RX_TRUNC_MODE) && defined(CONFIG_SEPARATION_MODE)
            FILL_RXBD(rxbd_p, len1, len2, vir2phy(addr1), 0, vir2phy(addr2), 0);
#elif defined(CONFIG_RX_TRUNC_MODE) && defined(CONFIG_SEPARATION_MODE)
            FILL_RXBD(rxbd_p, len1, len2, vir2phy(addr1), vir2phy(addr2));
#endif
        }

        rxbd_p++;
#if defined(CONFIG_SEPARATION_MODE)
        addr1 += len1;
#endif
        addr2 += len2;
    }
    // Flush all RXBD one time
    flush_dcache_range(rxbd_base_addr[channel], rxbd_base_addr[channel] + PCIEDMA_CH_RXBD_TOTAL_SIZE);
}

static void PCIEDMA_TX_interrupt_en(u32 channel)
{
    u32 reg_value32;
    u32 tx_dok_himr[PCIEDMA_TX_CH_NUM] =    {   B_AX_TXDMA_ACH0_INT_EN,
                                                B_AX_TXDMA_ACH1_INT_EN,
                                                B_AX_TXDMA_CH12_INT_EN};
    u32 tx_dok_hisr[PCIEDMA_TX_CH_NUM] =    {   B_AX_TXDMA_ACH0_INT,
                                                B_AX_TXDMA_ACH1_INT,
                                                B_AX_TXDMA_CH12_INT};

    // Clear interrupt (write 1 clear)
    reg_value32 = HalPCIeEPMemRead32(PCIE_MODULE_SEL_0, R_AX_HISR0);
    reg_value32 |= (tx_dok_hisr[channel]);
    HalPCIeEPMemWrite32(PCIE_MODULE_SEL_0, R_AX_HISR0, reg_value32);

    // Clear mask
    reg_value32 = HalPCIeEPMemRead32(PCIE_MODULE_SEL_0, R_AX_HIMR0);
    reg_value32 &= ~(tx_dok_himr[channel]);
    HalPCIeEPMemWrite32(PCIE_MODULE_SEL_0, R_AX_HIMR0, reg_value32);
}

static void PCIEDMA_RX_interrupt_en(u32 channel)
{
    u32 reg_value32;
    u32 rx_dok_himr[PCIEDMA_RX_CH_NUM] =    {   B_AX_RXDMA_INT_EN,
                                                B_AX_RPQDMA_INT_EN};
    u32 rx_dok_hisr[PCIEDMA_RX_CH_NUM] =    {   B_AX_RXDMA_INT,
                                                B_AX_RPQDMA_INT};

    // Clear interrupt (write 1 clear)
    reg_value32 = HalPCIeEPMemRead32(PCIE_MODULE_SEL_0, R_AX_HISR0);
    reg_value32 |= (rx_dok_hisr[channel]);
    HalPCIeEPMemWrite32(PCIE_MODULE_SEL_0, R_AX_HISR0, reg_value32);

    // Clear mask
    reg_value32 = HalPCIeEPMemRead32(PCIE_MODULE_SEL_0, R_AX_HIMR0);
    reg_value32 &= ~(rx_dok_himr[channel]);
    HalPCIeEPMemWrite32(PCIE_MODULE_SEL_0, R_AX_HIMR0, reg_value32);
}

static void tx_channel_setting(void)
{
    u32 reg_value32, base_addr;
    u32 channel;
#if 1
    u32 bdram_min, bdram_max, bdram_sidx;
#endif
    u32 txbd_base_l_reg[PCIEDMA_TX_CH_NUM] =    {   R_AX_ACH0_TXBD_DESA_L,
                                                    R_AX_ACH1_TXBD_DESA_L,
                                                    R_AX_CH12_TXBD_DESA_L};
    u32 txbd_base_h_reg[PCIEDMA_TX_CH_NUM] =    {   R_AX_ACH0_TXBD_DESA_H,
                                                    R_AX_ACH1_TXBD_DESA_H,
                                                    R_AX_CH12_TXBD_DESA_H};
    u32 txbd_base_addr[PCIEDMA_TX_CH_NUM] =     {   PCIEDMA_TXBD_ACH0_BASEADDR,
                                                    PCIEDMA_TXBD_ACH1_BASEADDR,
                                                    PCIEDMA_TXBD_CH12_BASEADDR};
    u32 txbd_num_reg[PCIEDMA_TX_CH_NUM] =       {   R_AX_ACH0_TXBD_NUM,
                                                    R_AX_ACH1_TXBD_NUM,
                                                    R_AX_CH12_TXBD_NUM};
    u32 txbd_num_sh[PCIEDMA_TX_CH_NUM] =        {   B_AX_ACH0_DESC_NUM_SH,
                                                    B_AX_ACH1_DESC_NUM_SH,
                                                    B_AX_CH12_DESC_NUM_SH};
    u32 txbd_num_msk[PCIEDMA_TX_CH_NUM] =       {   B_AX_ACH0_DESC_NUM_MSK,
                                                    B_AX_ACH1_DESC_NUM_MSK,
                                                    B_AX_CH12_DESC_NUM_MSK};
#if 1
    u32 tx_bdram_ctrl_reg[PCIEDMA_TX_CH_NUM] =  {   R_AX_ACH0_BDRAM_CTRL,
                                                    R_AX_ACH1_BDRAM_CTRL,
                                                    R_AX_CH12_BDRAM_CTRL};
    u32 tx_bdram_min_sh[PCIEDMA_TX_CH_NUM] =    {   B_AX_ACH0_BDRAM_MIN_SH,
                                                    B_AX_ACH1_BDRAM_MIN_SH,
                                                    B_AX_CH12_BDRAM_MIN_SH};
    u32 tx_bdram_min_msk[PCIEDMA_TX_CH_NUM] =    {  B_AX_ACH0_BDRAM_MIN_MSK,
                                                    B_AX_ACH1_BDRAM_MIN_MSK,
                                                    B_AX_CH12_BDRAM_MIN_MSK};
    u32 tx_bdram_max_sh[PCIEDMA_TX_CH_NUM] =    {   B_AX_ACH0_BDRAM_MAX_SH,
                                                    B_AX_ACH1_BDRAM_MAX_SH,
                                                    B_AX_CH12_BDRAM_MAX_SH};
    u32 tx_bdram_max_msk[PCIEDMA_TX_CH_NUM] =    {  B_AX_ACH0_BDRAM_MAX_MSK,
                                                    B_AX_ACH1_BDRAM_MAX_MSK,
                                                    B_AX_CH12_BDRAM_MAX_MSK};
    u32 tx_bdram_sidx_sh[PCIEDMA_TX_CH_NUM] =    {  B_AX_ACH0_BDRAM_SIDX_SH,
                                                    B_AX_ACH1_BDRAM_SIDX_SH,
                                                    B_AX_CH12_BDRAM_SIDX_SH};
    u32 tx_bdram_sidx_msk[PCIEDMA_TX_CH_NUM] =    { B_AX_ACH0_BDRAM_SIDX_MSK,
                                                    B_AX_ACH1_BDRAM_SIDX_MSK,
                                                    B_AX_CH12_BDRAM_SIDX_MSK};
#endif
    u32 stop_tx[PCIEDMA_TX_CH_NUM] =            {   B_AX_STOP_ACH0,
                                                    B_AX_STOP_ACH1,
                                                    B_AX_STOP_CH12};

#if defined(CONFIG_TX_TRUNC_MODE)
    reg_value32 = HalPCIeEPMemRead32(PCIE_MODULE_SEL_0, R_AX_PCIE_INIT_CFG1);
    reg_value32 |= B_AX_TX_TRUNC_MODE;
    HalPCIeEPMemWrite32(PCIE_MODULE_SEL_0, R_AX_PCIE_INIT_CFG1, reg_value32);

    reg_value32 = 0;
    HalPCIeEPMemWrite32(PCIE_MODULE_SEL_0, R_AX_TXDMA_ADDR_H, reg_value32);
#endif

#if 1
    bdram_min = PCIEDMA_TX_BDRAM_MIN_SIZE;
    bdram_max = PCIEDMA_TX_BDRAM_NUM/PCIEDMA_TX_CH_NUM;
    bdram_sidx = 0;
#endif

    for(channel = 0; channel < PCIEDMA_TX_CH_NUM; channel++)
    {
        //1. Set TXBD base address for each queue
        base_addr =     txbd_base_addr[channel];
        reg_value32 =   vir2phy((u32)base_addr);
        HalPCIeEPMemWrite32(PCIE_MODULE_SEL_0, txbd_base_l_reg[channel], reg_value32);

        reg_value32 = 0;
        HalPCIeEPMemWrite32(PCIE_MODULE_SEL_0, txbd_base_h_reg[channel], reg_value32);

        //2. Set TXBD number for each queue
        reg_value32 =   SET_WORD(PCIEDMA_CH_TXBD_NUM, txbd_num_msk[channel], txbd_num_sh[channel]);
        HalPCIeEPMemWrite16(PCIE_MODULE_SEL_0, txbd_num_reg[channel], reg_value32);
#if 1
        //4. BDRAM
        reg_value32 = SET_WORD(bdram_min, tx_bdram_min_msk[channel], tx_bdram_min_sh[channel]);
        reg_value32 |= SET_WORD(bdram_max, tx_bdram_max_msk[channel], tx_bdram_max_sh[channel]);
        reg_value32 |= SET_WORD(bdram_sidx, tx_bdram_sidx_msk[channel], tx_bdram_sidx_sh[channel]);
        HalPCIeEPMemWrite32(PCIE_MODULE_SEL_0, tx_bdram_ctrl_reg[channel], reg_value32);
        bdram_sidx += bdram_max;
#endif
        //7. Start TXDMA
        reg_value32 = HalPCIeEPMemRead32(PCIE_MODULE_SEL_0, R_AX_PCIE_DMA_STOP1);
        reg_value32 &= (~stop_tx[channel]);
        HalPCIeEPMemWrite32(PCIE_MODULE_SEL_0, R_AX_PCIE_DMA_STOP1, reg_value32);

        //9. Enable interrupt
        PCIEDMA_TX_interrupt_en(channel);
    }

    //3. Set TX Burst Size
    // Default: Multi tag=256, Single tag=2048

    //5. Enable multi-tag
    reg_value32 = HalPCIeEPMemRead32(PCIE_MODULE_SEL_0, R_AX_PCIE_INIT_CFG1);
    reg_value32 |= B_AX_LATENCY_CONTROL;
    HalPCIeEPMemWrite32(PCIE_MODULE_SEL_0, R_AX_PCIE_INIT_CFG1, reg_value32);

    //6. Setup HCI flow control
    // Use Default now

    //8. Enable TX HCI interface
    reg_value32 = HalPCIeEPMemRead32(PCIE_MODULE_SEL_0, R_AX_PCIE_INIT_CFG1);
    reg_value32 |= B_AX_TXHCI_EN;
    HalPCIeEPMemWrite32(PCIE_MODULE_SEL_0, R_AX_PCIE_INIT_CFG1, reg_value32);
}

static void rx_channel_setting(void)
{
    u32 reg_value32, base_addr;
    u32 channel;

    u32 rxbd_base_l_reg[PCIEDMA_RX_CH_NUM] =    {   R_AX_RXQ_RXBD_DESA_L,
                                                    R_AX_RPQ_RXBD_DESA_L};
    u32 rxbd_base_h_reg[PCIEDMA_RX_CH_NUM] =    {   R_AX_RXQ_RXBD_DESA_H,
                                                    R_AX_RPQ_RXBD_DESA_H};
    u32 rxbd_base_addr[PCIEDMA_RX_CH_NUM] =     {   PCIEDMA_RXBD_RXQ_BASEADDR,
                                                    PCIEDMA_RXBD_RPQ_BASEADDR};
    u32 rxbd_num_reg[PCIEDMA_RX_CH_NUM] =       {   R_AX_RXQ_RXBD_NUM,
                                                    R_AX_RPQ_RXBD_NUM};
    u32 rxbd_num_sh[PCIEDMA_RX_CH_NUM] =        {   B_AX_RXQ_DESC_NUM_SH,
                                                    B_AX_RPQ_DESC_NUM_SH};
    u32 rxbd_num_msk[PCIEDMA_RX_CH_NUM] =       {   B_AX_RXQ_DESC_NUM_MSK,
                                                    B_AX_RPQ_DESC_NUM_MSK};

#if defined(CONFIG_RX_TRUNC_MODE)
    reg_value32 = HalPCIeEPMemRead32(PCIE_MODULE_SEL_0, R_AX_PCIE_INIT_CFG1);
    reg_value32 |= B_AX_RX_TRUNC_MODE;
    HalPCIeEPMemWrite32(PCIE_MODULE_SEL_0, R_AX_PCIE_INIT_CFG1, reg_value32);

    reg_value32 = 0;
    HalPCIeEPMemWrite32(PCIE_MODULE_SEL_0, R_AX_RXDMA_ADDR_H, reg_value32);
#endif

    //1. Select RXBD mode: Separation mode or Packet mode
#if defined(CONFIG_SEPARATION_MODE)
    reg_value32 = HalPCIeEPMemRead32(PCIE_MODULE_SEL_0, R_AX_PCIE_INIT_CFG1);
    reg_value32 |= B_AX_RXBD_MODE;
    HalPCIeEPMemWrite32(PCIE_MODULE_SEL_0, R_AX_PCIE_INIT_CFG1, reg_value32);

    //2. Set append length
    reg_value32 = SET_WORD(PCIEDMA_RX_APPLEN, B_AX_PCIE_RX_APPLEN_MSK, B_AX_PCIE_RX_APPLEN_SH);
    HalPCIeEPMemWrite32(PCIE_MODULE_SEL_0, R_AX_PCIE_INIT_CFG2, reg_value32);
#else
    reg_value32 = HalPCIeEPMemRead32(PCIE_MODULE_SEL_0, R_AX_PCIE_INIT_CFG1);
    reg_value32 &= (~B_AX_RXBD_MODE);
    HalPCIeEPMemWrite32(PCIE_MODULE_SEL_0, R_AX_PCIE_INIT_CFG1, reg_value32);
#endif

    for(channel = 0; channel < PCIEDMA_RX_CH_NUM; channel++)
    {
        //3. Set RXBD base address for each queue
        base_addr =     rxbd_base_addr[channel];
        reg_value32 =   vir2phy((u32)base_addr);
        HalPCIeEPMemWrite32(PCIE_MODULE_SEL_0, rxbd_base_l_reg[channel], reg_value32);

        reg_value32 = 0;
        HalPCIeEPMemWrite32(PCIE_MODULE_SEL_0, rxbd_base_h_reg[channel], reg_value32);

        //4. Set RXBD number for each queue
        reg_value32 =   SET_WORD(PCIEDMA_CH_RXBD_NUM, rxbd_num_msk[channel], rxbd_num_sh[channel]);
        HalPCIeEPMemWrite16(PCIE_MODULE_SEL_0, rxbd_num_reg[channel], reg_value32);

        //8. Fill RXBD structure for each channel
        PCIEDMA_RXBD_fill(channel);

        //9. Enable interrupt
        PCIEDMA_RX_interrupt_en(channel);
    
    }
    //5. Enable RX pre-fetch
    reg_value32 = HalPCIeEPMemRead32(PCIE_MODULE_SEL_0, R_AX_PCIE_INIT_CFG1);
    reg_value32 &= (~B_AX_DIS_RXDMA_PRE);
    HalPCIeEPMemWrite32(PCIE_MODULE_SEL_0, R_AX_PCIE_INIT_CFG1, reg_value32);

    //6. Set RX Burst Size
    // Default: 128 byte

    //7. Enable RX HCI interface
    reg_value32 = HalPCIeEPMemRead32(PCIE_MODULE_SEL_0, R_AX_PCIE_INIT_CFG1);
    reg_value32 |= B_AX_RXHCI_EN;
    HalPCIeEPMemWrite32(PCIE_MODULE_SEL_0, R_AX_PCIE_INIT_CFG1, reg_value32);
}

static void process_rx_packet(u32 channel, RXBD_TYPE* rxbd_p)
{
    u32 tag, fs, ls;
    u32 rx_buf_addr, rx_buf_size;
    RX_BD_INFO_TYPE* rx_bd_info_p;
    u32 i, addr;
#if defined(CONFIG_SEPARATION_MODE)
    u32 addr1;
#endif
    u32 addr2;

    if(channel == PCIEDMA_RX_RPQ)
    {
        // Don't support separation mode for RPQ
        addr2= GET_RXBD_ADDR_L(rxbd_p);
    }else{
#if defined(CONFIG_SEPARATION_MODE)
        addr1 = GET_RXBD_ADDR1_L(rxbd_p);
        addr2 = GET_RXBD_ADDR2_L(rxbd_p);
#else
        addr2= GET_RXBD_ADDR_L(rxbd_p);
#endif
    }

    // Don't support check separation part now
    rx_buf_addr = phy2vir(addr2);

    rx_bd_info_p = (RX_BD_INFO_TYPE*)rx_buf_addr;
    tag = (rx_bd_info_p->dword0 >> AX_BD_INFO_TAG_SH) & AX_BD_INFO_TAG_MSK;
    fs = ((rx_bd_info_p->dword0 & AX_BD_INFO_FS) ? 1 : 0);
    ls = ((rx_bd_info_p->dword0 & AX_BD_INFO_LS) ? 1: 0);
    rx_buf_size = GET_RX_BD_INFO_SIZE(rx_bd_info_p);
    printf("Addr = 0x%08x: tag = %d, fs = %d, ls = %d, rx_buf_size = 0x%x\n", rx_buf_addr, tag, fs, ls, rx_buf_size);

    addr = rx_buf_addr;
    for(i=0; i<rx_buf_size; i=i+16){
        printf("0x%03x: %08x %08x %08x %08x\n", i, *(u32 *)addr, *(u32 *)(addr+4), *(u32 *)(addr+8), *(u32 *)(addr+12));
        addr = addr+16;
    }
}

static void PCIEDMA_TX_ISR(u32 channel)
{
    u32 reg_value32, i;
    u32 host_index, hw_index;
    u32 txbd_idx_reg[PCIEDMA_TX_CH_NUM] =       {   R_AX_ACH0_TXBD_IDX,
                                                    R_AX_ACH1_TXBD_IDX,
                                                    R_AX_CH12_TXBD_IDX};
    u32 tx_host_idx_sh[PCIEDMA_TX_CH_NUM] =     {   B_AX_ACH0_HOST_IDX_SH,
                                                    B_AX_ACH1_HOST_IDX_SH,
                                                    B_AX_CH12_HOST_IDX_SH};
    u32 tx_host_idx_msk[PCIEDMA_TX_CH_NUM] =    {   B_AX_ACH0_HOST_IDX_MSK,
                                                    B_AX_ACH1_HOST_IDX_MSK,
                                                    B_AX_CH12_HOST_IDX_MSK};
    u32 tx_hw_idx_sh[PCIEDMA_TX_CH_NUM] =       {   B_AX_ACH0_HW_IDX_SH,
                                                    B_AX_ACH1_HW_IDX_SH,
                                                    B_AX_CH12_HW_IDX_SH};
    u32 tx_hw_idx_msk[PCIEDMA_TX_CH_NUM] =      {   B_AX_ACH0_HW_IDX_MSK,
                                                    B_AX_ACH1_HW_IDX_MSK,
                                                    B_AX_CH12_HW_IDX_MSK};

    i = 0;
    while(1){
        reg_value32 = HalPCIeEPMemRead32(PCIE_MODULE_SEL_0, txbd_idx_reg[channel]);
        host_index = GET_FIELD(reg_value32, tx_host_idx_msk[channel], tx_host_idx_sh[channel]);
        hw_index = GET_FIELD(reg_value32, tx_hw_idx_msk[channel], tx_hw_idx_sh[channel]);
        if(host_index == hw_index) {
            printf("DMA TX OK! host_index = 0x%x, hw_index = 0x%x!\n!", host_index, hw_index);
            break;
        } else {
            if(i == 10){
                printf("DMA TX Timeout, host_index = 0x%x, hw_index = 0x%x!\n!", host_index, hw_index);
                return;
            } else {
                mdelay(5);
            }
        }
        i++;
    }
}

static void PCIEDMA_RX_ISR(u32 channel)
{
    u32 reg_value32;
    u32 host_index, hw_index;
    RXBD_TYPE *rxbd_p, *cur_rxbd_p;
#if defined(CONFIG_SEPARATION_MODE)
    u32 len1, addr1;
#endif
    u32 len2, addr2;
    static u32 sw_close_tag[PCIEDMA_RX_CH_NUM] =    {1, 1};
    u32 rxbd_base_l_reg[PCIEDMA_RX_CH_NUM] =    {   R_AX_RXQ_RXBD_DESA_L,
                                                    R_AX_RPQ_RXBD_DESA_L};
    u32 rxbd_idx_reg[PCIEDMA_RX_CH_NUM] =       {   R_AX_RXQ_RXBD_IDX,
                                                    R_AX_RPQ_RXBD_IDX};
    u32 rx_host_idx_sh[PCIEDMA_RX_CH_NUM] =     {   B_AX_RXQ_HOST_IDX_SH,
                                                    B_AX_RPQ_HOST_IDX_SH};
    u32 rx_host_idx_msk[PCIEDMA_RX_CH_NUM] =    {   B_AX_RXQ_HOST_IDX_MSK,
                                                    B_AX_RPQ_HOST_IDX_MSK};
    u32 rx_hw_idx_sh[PCIEDMA_RX_CH_NUM] =       {   B_AX_RXQ_HW_IDX_SH,
                                                    B_AX_RPQ_HW_IDX_SH};
    u32 rx_hw_idx_msk[PCIEDMA_RX_CH_NUM] =      {   B_AX_RXQ_HW_IDX_MSK,
                                                    B_AX_RPQ_HW_IDX_MSK};

    rxbd_p = (RXBD_TYPE*)phy2vir(HalPCIeEPMemRead32(PCIE_MODULE_SEL_0, rxbd_base_l_reg[channel]));    

    while(1)
    {
        reg_value32 = HalPCIeEPMemRead32(PCIE_MODULE_SEL_0, rxbd_idx_reg[channel]);
        host_index = GET_FIELD(reg_value32, rx_host_idx_msk[channel], rx_host_idx_sh[channel]);
        hw_index = GET_FIELD(reg_value32, rx_hw_idx_msk[channel], rx_hw_idx_sh[channel]);
        if(host_index != hw_index)
        {
            cur_rxbd_p = (RXBD_TYPE*)((u32)rxbd_p + host_index * PCIEDMA_RXBD_TYPE_SIZE);
            if(channel == PCIEDMA_RX_RPQ)
            {
                // Don't support separation mode for RPQ
                len2 = GET_RXBD_SIZE(cur_rxbd_p);
                addr2= GET_RXBD_ADDR_L(cur_rxbd_p);
            }else{
#if defined(CONFIG_SEPARATION_MODE)
                len1 = GET_RXBD_SIZE1(cur_rxbd_p);
                len2 = GET_RXBD_SIZE2(cur_rxbd_p);
                addr1 = GET_RXBD_ADDR1_L(cur_rxbd_p);
                addr2 = GET_RXBD_ADDR2_L(cur_rxbd_p);
                invalidate_dcache_range(phy2vir(addr1), phy2vir(addr1) + len1);
#else
                len2 = GET_RXBD_SIZE(cur_rxbd_p);
                addr2= GET_RXBD_ADDR_L(cur_rxbd_p);
#endif
            }
            invalidate_dcache_range(phy2vir(addr2), phy2vir(addr2) + len2);

            RX_BD_INFO_TYPE* rx_bd_info_p = (RX_BD_INFO_TYPE*)phy2vir(addr2);

            if(GET_RX_BD_INFO_TAG(rx_bd_info_p) == sw_close_tag[channel])
            {
                process_rx_packet(channel, cur_rxbd_p);

                sw_close_tag[channel]++;
                if(sw_close_tag[channel] > AX_BD_INFO_TAG_MSK){
                    sw_close_tag[channel] = 1;
                }

                host_index = (host_index + 1) % PCIEDMA_CH_RXBD_NUM;
                reg_value32 = SET_CLR_WORD(reg_value32, host_index, rx_host_idx_msk[channel], rx_host_idx_sh[channel]);
                HalPCIeEPMemWrite32(PCIE_MODULE_SEL_0, rxbd_idx_reg[channel], reg_value32);
            }else{
                printf("RX tag inconsistent!hw tag = 0x%x, sw tag = 0x%x\n", GET_RX_BD_INFO_TAG(rx_bd_info_p), sw_close_tag[channel]);
                return;
            }
        }else{
            break;
        }
    }
}

void PCIEDMA_ISR(void* data)
{
    u32 reg_value32, channel;
    u32 tx_dok_hisr[PCIEDMA_TX_CH_NUM] =     {  B_AX_TXDMA_ACH0_INT,
                                                B_AX_TXDMA_ACH1_INT,
                                                B_AX_TXDMA_CH12_INT};
    u32 rx_dok_hisr[PCIEDMA_RX_CH_NUM] =     {  B_AX_RXDMA_INT,
                                                B_AX_RPQDMA_INT};

    // Check HISR
    for(channel = 0; channel < PCIEDMA_TX_CH_NUM; channel++)
    {
        reg_value32 = HalPCIeEPMemRead32(PCIE_MODULE_SEL_0, R_AX_HISR0);
        if((reg_value32 & tx_dok_hisr[channel]) != 0)
        {
            // Check DMA OK or not (interrupt mode )
            PCIEDMA_TX_ISR(channel);

            // Clear interrupt (write 1 clear)
            reg_value32 = (tx_dok_hisr[channel]);
            HalPCIeEPMemWrite32(PCIE_MODULE_SEL_0, R_AX_HISR0, reg_value32);
        }
    }

    for(channel = 0; channel < PCIEDMA_RX_CH_NUM; channel++)
    {
        reg_value32 = HalPCIeEPMemRead32(PCIE_MODULE_SEL_0, R_AX_HISR0);
        if((reg_value32 & rx_dok_hisr[channel]) != 0)
        {
            // Check DMA OK or not (interrupt mode )
            PCIEDMA_RX_ISR(channel);

            // Clear interrupt (write 1 clear)
            reg_value32 = (rx_dok_hisr[channel]);
            HalPCIeEPMemWrite32(PCIE_MODULE_SEL_0, R_AX_HISR0, reg_value32);
        }
    }

    // clear interrupt for A17!
    HAL_WRITE32_MEM(0xf4326000, 0x00000003);	// G3: 0x00000100

    // clear interrupt for Taroko!
    HAL_WRITE32_MEM(0xf4326008, 0x00000003);	// G3: 0x00000100
}

static void PCIEDMA_init(void)
{
    u32 reg_value32;
    IRQ_HANDLE	PCIeIrqHandle;

    // Host Power On MAC
    WLAN_init();

    // Buffer init.
    memset((void *)PCIEDMA_TX_BUF_BASEADDR, 0, PCIEDMA_TX_BUF_TOTAL_SIZE);
#if defined(CONFIG_SEPARATION_MODE)
    memset((void *)PCIEDMA_RX_BUF_P1_BASEADDR, 0, PCIEDMA_RX_BUF_P1_TOTAL_SIZE);
    flush_dcache_range(PCIEDMA_RX_BUF_P1_BASEADDR, PCIEDMA_RX_BUF_P1_BASEADDR + PCIEDMA_RX_BUF_P1_TOTAL_SIZE);
#endif
    memset((void *)PCIEDMA_RXQ_BUF_P2_BASEADDR, 0, PCIEDMA_RX_BUF_P2_TOTAL_SIZE * PCIEDMA_RX_CH_NUM);
    flush_dcache_range(PCIEDMA_RXQ_BUF_P2_BASEADDR, PCIEDMA_RXQ_BUF_P2_BASEADDR + PCIEDMA_RX_BUF_P2_TOTAL_SIZE * PCIEDMA_RX_CH_NUM);
    
    // Stop TXDMA/RXDMA
	reg_value32 = B_AX_STOP_ACH0 | B_AX_STOP_ACH1 | B_AX_STOP_CH12;
	HalPCIeEPMemWrite32(PCIE_MODULE_SEL_0, R_AX_PCIE_DMA_STOP1, reg_value32);

    // Init TXBD
    tx_channel_setting();

    // Init RQPN, to do
    // init_page_info();

    // Init RXBD
    rx_channel_setting();

	// Register IRQ
    PCIeIrqHandle.Data = (u32)NULL;
    PCIeIrqHandle.IrqNum = g3_pcie_1_core_top;
    PCIeIrqHandle.IrqFun = (IRQ_FUN)PCIEDMA_ISR;
    PCIeIrqHandle.Priority = 0;
    InterruptRegister(&PCIeIrqHandle);
}

static void PCIEDMA_TX_start(PCIEDMA_TX_PARA* tx_para_p)
{
    u32 reg_value32;
    u32 host_index, hw_index;
    u32 channel = tx_para_p->channel;
    u32 used_txbd_num = tx_para_p->used_txbd_num;
    u32 txbd_idx_reg[PCIEDMA_TX_CH_NUM] =       {   R_AX_ACH0_TXBD_IDX,
                                                    R_AX_ACH1_TXBD_IDX,
                                                    R_AX_CH12_TXBD_IDX};
    u32 tx_host_idx_sh[PCIEDMA_TX_CH_NUM] =     {   B_AX_ACH0_HOST_IDX_SH,
                                                    B_AX_ACH1_HOST_IDX_SH,
                                                    B_AX_CH12_HOST_IDX_SH};
    u32 tx_host_idx_msk[PCIEDMA_TX_CH_NUM] =    {   B_AX_ACH0_HOST_IDX_MSK,
                                                    B_AX_ACH1_HOST_IDX_MSK,
                                                    B_AX_CH12_HOST_IDX_MSK};
    u32 tx_hw_idx_sh[PCIEDMA_TX_CH_NUM] =       {   B_AX_ACH0_HW_IDX_SH,
                                                    B_AX_ACH1_HW_IDX_SH,
                                                    B_AX_CH12_HW_IDX_SH};
    u32 tx_hw_idx_msk[PCIEDMA_TX_CH_NUM] =      {   B_AX_ACH0_HW_IDX_MSK,
                                                    B_AX_ACH1_HW_IDX_MSK,
                                                    B_AX_CH12_HW_IDX_MSK};
    static u32 txbd_index[PCIEDMA_TX_CH_NUM] =  {   0,
                                                    0,
                                                    0};

    if(channel >= PCIEDMA_TX_CH_NUM){
        // invalid channel
        printf("Invalid channel %d\n", channel);
        return;
    }

    if((used_txbd_num < 1) || (used_txbd_num >= PCIEDMA_CH_TXBD_NUM)){
        printf("Unsupported used txbd %d\n", used_txbd_num);
        return;
    }

    //1. ~ 9. msut be done in tx_channel_setting()

    //10. Fill TXBD structure & content for each channel
    PCIEDMA_TXBD_fill(tx_para_p, txbd_index[channel]);

    //11. Update TXBD host index
    reg_value32 = HalPCIeEPMemRead32(PCIE_MODULE_SEL_0, txbd_idx_reg[channel]);
    host_index = GET_FIELD(reg_value32, tx_host_idx_msk[channel], tx_host_idx_sh[channel]);
    hw_index = GET_FIELD(reg_value32, tx_hw_idx_msk[channel], tx_hw_idx_sh[channel]);
    if(host_index != hw_index) {
        printf("host_id(%d) is not equal to hw_id(%d)!\n", host_index, hw_index);
        return;
    }
    host_index = (host_index + used_txbd_num) % PCIEDMA_CH_TXBD_NUM;
    reg_value32 = SET_CLR_WORD(reg_value32, host_index, tx_host_idx_msk[channel], tx_host_idx_sh[channel]);
    HalPCIeEPMemWrite32(PCIE_MODULE_SEL_0, txbd_idx_reg[channel], reg_value32);

    //12. Check DMA OK or not (polling mode)
    if(tx_para_p->mode == PCIEDMA_MODE_POLLING){
        PCIEDMA_TX_ISR(channel);
    }

    // Update txbd index
    txbd_index[channel] = (txbd_index[channel] + used_txbd_num) % PCIEDMA_CH_TXBD_NUM;
}

static void PCIEDMA_TX_test(u32 channel, u32 item, u32 mode)
{
    PCIEDMA_TX_PARA tx_para;
    txdesc_t *txdes_p;
    ADDR_INFO_TYPE *addr_info;
    u32 qsel, channel_dma, pktsize, wdpage, pkt_offset, wdinfo_en, fwdl_en;
    u8 *data_addr;
    static u8 counter = 0;
    u32 i, index, data_len, payload_start_addr, payload_size, total_len = 0;
    static u32 pcie_seq_num_0 = 0;

    switch(channel)
    {
        case PCIEDMA_TX_ACH0: // go through
        case PCIEDMA_TX_ACH1: // go through
            // item 0: CMAC loopback
            if(item != 0){
                printf("Unsupported item %d!\n", item);
                return;
            }

            tx_para.used_txbd_num = 1;
            tx_para.buf_len[0] = PCIEDMA_TX_BUF_TYPE_WD_BODY_SIZE + PCIEDMA_TX_BUF_TYPE_WD_INFO_SIZE + PCIEDMA_TX_BUF_TYPE_WP_INFO_SIZE;
            tx_para.buf_addr[0] = (u8 *)PCIEDMA_TX_BUF_BASEADDR;
            tx_para.ls[0] = 1;
            tx_para.channel = channel;
            tx_para.mode = mode;

            total_len = tx_para.buf_len[0];
            memset(tx_para.buf_addr[0], 0, tx_para.buf_len[0]);

            txdes_p = (txdesc_t *)tx_para.buf_addr[0];
            // Fill WD body/WD info Content
            WLAN_fill_WD((u8*)txdes_p);

            // Fill WP info Content
            txdes_p->dword12 = SET_CLR_WORD(txdes_p->dword12, pcie_seq_num_0, TXD_PCIE_SEQ_NUM_0_MSK, TXD_PCIE_SEQ_NUM_0_SH);
            txdes_p->dword12 |= TXD_VALID_0;
            pcie_seq_num_0++;

            addr_info = (ADDR_INFO_TYPE *)&(txdes_p->dword14);
            data_addr = (tx_para.buf_addr[0] + tx_para.buf_len[0]);
            // Address info
            payload_size = 0x40;
#if !defined(CONFIG_TX_TRUNC_MODE)
            FILL_ADDR_INFO(addr_info, payload_size, 1, 0, 1, vir2phy((u32)data_addr), 0);
#else
            FILL_ADDR_INFO(addr_info, payload_size, 1, 0, 1, vir2phy((u32)data_addr));
#endif
            // Fill data Content
            for(i=0; i<payload_size ; i++)
            {
                *data_addr = counter;
                data_addr++;
                counter++;
            }

            // Dump data Content
            data_addr = tx_para.buf_addr[0] + tx_para.buf_len[0];
            data_len = payload_size;
            printf("Payload_Addr = 0x%08x: len = 0x%x\n", (u32)data_addr, data_len);
            for(i=0 ; i<data_len ; i=i+16)
            {
                printf("0x%03x: %08x %08x %08x %08x\n", i, *(u32 *)data_addr, *(u32 *)(data_addr+4), *(u32 *)(data_addr+8), *(u32 *)(data_addr+12));
                data_addr = data_addr + 16;
            }

            break;
        case PCIEDMA_TX_CH12:
            // item 0: txbd_num = 1, payload = 64 byte
            // item 1: txbd_num = 2, payload = 64*2 byte
            // item 2: txbd_num = 4, payload = 64*4 byte
            // item 3: txbd_num = 8, payload = 64*8 byte
            // item 4: txbd_num = 16, payload = 64*16 byte
            // item 5: FWDL, txbd_num = 1, payload = 64 byte
            // item 6: FWDL, txbd_num = 2, payload = 64*2 byte
            // item 7: FWDL, txbd_num = 4, payload = 64*4 byte
            // item 8: FWDL, txbd_num = 8, payload = 64*8 byte
            // item 9: FWDL, txbd_num = 16, payload = 64*16 byte

            if(item > 9){
                printf("Unsupported item %d!\n", item);
            }
            //information for TXBD
            tx_para.used_txbd_num = 1<<(item%5);
            for(index=0; index<tx_para.used_txbd_num; index++)
            {
                if(index == 0){
                    tx_para.buf_len[0] = PCIEDMA_TX_BUF_TYPE_WD_BODY_SIZE + PCIEDMA_TX_BUF_TYPE_PAYLOAD_SIZE;
                    tx_para.buf_addr[0] = (u8 *)PCIEDMA_TX_BUF_BASEADDR;
                }else{
                    tx_para.buf_len[index] = PCIEDMA_TX_BUF_TYPE_PAYLOAD_SIZE;
                    tx_para.buf_addr[index] = (u8 *)tx_para.buf_addr[index-1] + tx_para.buf_len[index-1];
                }
                tx_para.ls[index] = 0;
                memset(tx_para.buf_addr[index], 0, tx_para.buf_len[index]);
                total_len += tx_para.buf_len[index];
            }
            tx_para.ls[index-1] = 1;
            tx_para.channel = channel;
            tx_para.mode = mode;

            // Fill WD body Content
            txdes_p = (txdesc_t *)tx_para.buf_addr[0];
            // qsel
            qsel = 0;
            txdes_p->dword2 = SET_CLR_WORD(txdes_p->dword2, qsel, TXD_QSEL_MSK, TXD_QSEL_SH);

            // channel_dma
            if(channel == PCIEDMA_TX_CH12){
                channel_dma = 12;
            }else{
                channel_dma = channel;
            }
            txdes_p->dword0 = SET_CLR_WORD(txdes_p->dword0, channel_dma, TXD_CH_DMA_MSK, TXD_CH_DMA_SH);

            // pktsize
            pktsize = total_len - PCIEDMA_TX_BUF_TYPE_WD_BODY_SIZE;
            txdes_p->dword2 = SET_CLR_WORD(txdes_p->dword2, pktsize, TXD_TXPKTSIZE_MSK, TXD_TXPKTSIZE_SH);

            // wdpage, use 64 bytes now
            wdpage = 0;
            txdes_p->dword0 |= (wdpage ? TXD_WD_PAGE : 0);

            // pkt_offset
            pkt_offset = 0;
            txdes_p->dword0 |= (pkt_offset ? TXD_PKT_OFFSET : 0);

            //wdinfo_en
            wdinfo_en = 0;
            txdes_p->dword0 |= (wdinfo_en ? TXD_WDINFO_EN : 0);

            if((item >=5) && (item <=9)){
                fwdl_en = 1;
            }else{
                fwdl_en = 0;
            }
            txdes_p->dword0 |= (fwdl_en ? TXD_FWDL_EN : 0);

            // Fill data Content
            for(index=0; index<tx_para.used_txbd_num; index++)
            {
                if(index == 0){
                    payload_start_addr = PCIEDMA_TX_BUF_TYPE_WD_BODY_SIZE;
                }else{
                    payload_start_addr = 0;
                }
                data_addr = tx_para.buf_addr[index] + payload_start_addr;
                for(i=payload_start_addr ; i<tx_para.buf_len[index] ; i++)
                {
                    *data_addr = counter;
                    data_addr++;
                    counter++;
                }
            }
            break;
        default:
            printf("Unsupported channel %d!\n", channel);
            return;
    }

    // Dump content
    for(index=0; index<tx_para.used_txbd_num; index++)
    {
        data_addr = tx_para.buf_addr[index];
        data_len = tx_para.buf_len[index];
        printf("(TXBD %d)Addr = 0x%08x: len = 0x%x\n", index, (u32)data_addr, data_len);
        for(i=0 ; i<data_len ; i=i+16)
        {
            printf("0x%03x: %08x %08x %08x %08x\n", i, *(u32 *)data_addr, *(u32 *)(data_addr+4), *(u32 *)(data_addr+8), *(u32 *)(data_addr+12));
            data_addr = data_addr + 16;
        }
    }

    // Flush TX buff one time
    flush_dcache_range((unsigned long)(tx_para.buf_addr[0]), ALIGN(((int)tx_para.buf_addr[0] + total_len), CACHE_LINE_SIZE));

    // AXIDMA start
    PCIEDMA_TX_start(&tx_para);
}

static void PCIEDMA_RX_start(u32 channel, u32 mode)
{
    if(channel >= PCIEDMA_RX_CH_NUM){
        // invalid channel
        printf("Invalid channel %d\n", channel);
        return;
    }

    //1. ~ 9. msut be done in rx_channel_setting()

    if(mode == PCIEDMA_MODE_POLLING){
        PCIEDMA_RX_ISR(channel);
    }
}

void PCIeTest8852A(u32* data)
{
    u32 type, channel, item, mode;

    type = data[0];
    channel = data[1];
    item = data[2];
    mode = data[3];

    switch(type)
    {
        case PCIE8852A_VERI_TYPE_INIT:
            PCIEDMA_init();
            break;
        case PCIE8852A_VERI_TYPE_TX:
            PCIEDMA_TX_test(channel, item, mode);
            break;
        case PCIE8852A_VERI_TYPE_RX:
            PCIEDMA_RX_start(channel, mode);
            break;
        case PCIE8852A_VERI_TYPE_FWDL:
            WLAN_FWDL(item, mode);
            break;
        default:
            break;
    }
}

