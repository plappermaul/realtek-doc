/******************************************************************************
 *
 * Copyright(c) 2019 Realtek Corporation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 ******************************************************************************/

#include "fwdl.h"
#include "fwcmd.h"
#include "trx_desc.h"
#include "../mac_reg.h"

#define FWDL_WAIT_CNT	2000

#define FWDL_SECTION_MAX_NUM 		10
#define FWDL_SECTION_CHKSUM_LEN		8
#define FWDL_SECTION_PER_PKE_LEN	2040

struct fwhdr_section_info {
	u8 redl;
	u8 *addr;
	u32 len;
	u32 dladdr;
};

struct fw_bin_info {
	u8 section_num;
	u32 hdr_len;
	struct fwhdr_section_info section_info[FWDL_SECTION_MAX_NUM];
};

static inline void fwhdr_section_parser(struct fwhdr_section_t *section,
					struct fwhdr_section_info *info)
{
	u32 hdr_val;
	u32 section_len;

	hdr_val = le32_to_cpu(section->dword1);
	section_len = GET_FIELD(hdr_val, SECTION_INFO_SEC_SIZE);
	if (hdr_val & SECTION_INFO_CHECKSUM)
		section_len += FWDL_SECTION_CHKSUM_LEN;
	info->len = section_len;
	info->redl = (hdr_val & SECTION_INFO_REDL) ? 1 : 0;

	info->dladdr = (GET_FIELD(le32_to_cpu(section->dword0), SECTION_INFO_SEC_DL_ADDR))& 0x1FFFFFFF;
}

static inline void fwhdr_hdr_parser(struct fwhdr_hdr_t *hdr,
				    struct fw_bin_info *info)
{
	u32 hdr_val;

	hdr_val = le32_to_cpu(hdr->dword6);
	info->section_num = GET_FIELD(hdr_val, FWHDR_SEC_NUM);
	info->hdr_len = FWHDR_HDR_LEN + info->section_num * FWHDR_SECTION_LEN;

	/* fill HALMAC information */
	hdr_val = le32_to_cpu(hdr->dword7);
	hdr_val = SET_CLR_WORD(hdr_val, FWDL_SECTION_PER_PKE_LEN,
			       FWHDR_FW_PART_SZ);
	hdr->dword7 = cpu_to_le32(hdr_val);
}

static u32 fwhdr_parser(struct mac_ax_adapter * adapter, u8 *fw, u32 len,
			struct fw_bin_info *info)
{
	u32 i;
	u8 *fw_end = fw + len;
	u8 *bin_ptr;
	struct fwhdr_section_info *cur_section_info;

	if (!info)
		return MACNPTR;

	fwhdr_hdr_parser((struct fwhdr_hdr_t *)fw, info);
	bin_ptr = fw + info->hdr_len;

	/* jump to section header */
	fw += FWHDR_HDR_LEN;
	cur_section_info = info->section_info;
	for (i = 0; i < info->section_num; i++) {
		fwhdr_section_parser((struct fwhdr_section_t *)fw,
				     cur_section_info);
		cur_section_info->addr = bin_ptr;
		bin_ptr += cur_section_info->len;
		fw += FWHDR_SECTION_LEN;
		cur_section_info++;
	}

	if (fw_end != bin_ptr) {
		PLTFM_MSG_ERR("[ERR]fw bin size\n");
		return MACFWBIN;
	}

	return 0;
}

static inline u32 update_fw_ver(struct mac_ax_adapter * adapter,
				struct fwhdr_hdr_t *hdr)
{
	u32 hdr_val;
	struct mac_ax_fw_info *info = adapter->fw_info;

	if (!info) {
		info = (struct mac_ax_fw_info *)
				PLTFM_MALLOC(sizeof(struct mac_ax_fw_info));
		if (!info)
			return MACNPTR;
	}

	hdr_val = le32_to_cpu(hdr->dword1);
	info->ver = GET_FIELD(hdr_val, FWHDR_VERSION);
	info->sub_ver = GET_FIELD(hdr_val, FWHDR_SUBVERSION);
	info->sub_idex= GET_FIELD(hdr_val, FWHDR_SUBINDEX);

	hdr_val = le32_to_cpu(hdr->dword5);
	info->build_year = GET_FIELD(hdr_val, FWHDR_YEAR);

	hdr_val = le32_to_cpu(hdr->dword4);
	info->build_mon = GET_FIELD(hdr_val, FWHDR_MONTH);
	info->build_date = GET_FIELD(hdr_val, FWHDR_DATE);
	info->build_hour = GET_FIELD(hdr_val, FWHDR_HOUR);
	info->build_min = GET_FIELD(hdr_val, FWHDR_MIN);

	return 0;
}

static inline void __reset_fw_ctrl(struct mac_ax_adapter * adapter, u8 cpu_en)
{
	u8 val8;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	val8 = MAC_REG_R8(R_AX_WCPU_FW_CTRL);
	val8 &= ~(B_AX_WCPU_FWDL_EN | B_AX_H2C_PATH_RDY | B_AX_FWDL_PATH_RDY);
	val8 = SET_CLR_WORD(val8, S_AX_WCPU_FWDL_STS_INIT, B_AX_WCPU_FWDL_STS);
	MAC_REG_W8(R_AX_WCPU_FW_CTRL, val8);

	val8 = MAC_REG_R8(R_AX_BOOT_REASON);
	val8 = SET_CLR_WORD(val8, S_AX_BOOT_REASON_PWR_ON, B_AX_BOOT_REASON);
	MAC_REG_W8(R_AX_BOOT_REASON, val8);

	if (cpu_en)
		MAC_REG_W8(R_AX_WCPU_FW_CTRL, val8 | B_AX_WCPU_FWDL_EN);
}

static u32 __fwhdr_download(struct mac_ax_adapter *adapter, u8 *fw, u32 hdr_len)
{
	u8 *buf;
	u32 ret;
	struct h2c_buf *h2cb;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_DATA);
	if (!h2cb)
		return MACNPTR;

	buf = h2cb_put(h2cb, hdr_len);
	if (!buf)
		return MACNPTR;

	PLTFM_MEMCPY(buf, fw, hdr_len);

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C,
			      FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_FWDL,
			      FWCMD_H2C_FUNC_FWDL,
			      0,
			      0);
	if (ret)
		return ret;

	ret = h2c_pkt_build_txd(adapter, h2cb);
	if (ret)
		return ret;

	ret = PLTFM_TX(h2cb->data, h2cb->len);
	if (ret) {
		PLTFM_MSG_ERR("[ERR]platform tx\n");
		return ret;
	}

	h2cb_free(adapter, h2cb);

	return 0;
}

static u32 __sections_build_txd(struct mac_ax_adapter *adapter,
				struct h2c_buf *h2cb)
{
	u8 *buf;
	u32 ret;
	u32 txd_len;
	struct mac_ax_txpkt_info info;

	info.type = MAC_AX_PKT_FWDL;
	info.pktsize = h2cb->len;
	txd_len = mac_txdesc_len(adapter, &info);

	buf = h2cb_push(h2cb, txd_len);
	if (!buf)
		return MACNPTR;

	ret = mac_build_txdesc(adapter, &info, buf, txd_len);
	if (ret)
		return ret;

	return 0;
}

static u32 __sections_download(struct mac_ax_adapter *adapter,
			       struct fwhdr_section_info *info)
{
	u8 *section = info->addr;
	u32 residue_len = info->len;
	u32 pkt_len;
	u8 *buf;
	u32 ret;
	struct h2c_buf *h2cb;

	while (residue_len) {
		if (residue_len >= FWDL_SECTION_PER_PKE_LEN)
			pkt_len = FWDL_SECTION_PER_PKE_LEN;
		else
			pkt_len = residue_len;

		h2cb = h2cb_alloc(adapter, H2CB_CLASS_LONG_DATA);
		if (!h2cb)
			return MACNPTR;

		buf = h2cb_put(h2cb, pkt_len);
		if (!buf)
			return MACNPTR;

		PLTFM_MEMCPY(buf, section, pkt_len);

		ret = __sections_build_txd(adapter, h2cb);
		if (ret)
			return ret;

		ret = PLTFM_TX(h2cb->data, h2cb->len);
		if (ret) {
			PLTFM_MSG_ERR("[ERR]platform tx\n");
			return ret;
		}

		h2cb_free(adapter, h2cb);

		section += pkt_len;
		residue_len -= pkt_len;
	}

	return 0;
}

static u32 fwdl_phase0(struct mac_ax_adapter *adapter)
{
	u32 cnt = FWDL_WAIT_CNT;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	__reset_fw_ctrl(adapter, 1);

	while (cnt--) {
		if (MAC_REG_R8(R_AX_WCPU_FW_CTRL) & B_AX_H2C_PATH_RDY)
			break;
		PLTFM_DELAY_US(1);
	}

	if (!cnt) {
		PLTFM_MSG_ERR("[ERR]H2C path ready\n");
		return MACPOLLTO;
	}

	adapter->sm.fwdl = MAC_AX_FWDL_H2C_PATH_RDY;

	return 0;
}

static u32 fwdl_phase1(struct mac_ax_adapter *adapter, u8 *fw, u32 hdr_len)
{
	u32 ret;
	u32 cnt = FWDL_WAIT_CNT;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	ret = __fwhdr_download(adapter, fw, hdr_len);
	if (ret) {
		PLTFM_MSG_ERR("[ERR]FW header download\n");
		return ret;
	}

	while (cnt--) {
		if (MAC_REG_R8(R_AX_WCPU_FW_CTRL) & B_AX_FWDL_PATH_RDY)
			break;
		PLTFM_DELAY_US(1);
	}

	if (!cnt) {
		PLTFM_MSG_ERR("[ERR]FWDL path ready\n");
		return MACPOLLTO;
	}

	adapter->sm.fwdl = MAC_AX_FWDL_PATH_RDY;

	return 0;
}

static u32 fwdl_phase2(struct mac_ax_adapter *adapter, u8 *fw,
		       struct fw_bin_info *info)
{
	u8 val8;
	u32 ret;
	u32 cnt = FWDL_WAIT_CNT;
	u32 section_num = info->section_num;
	struct fwhdr_section_info *section_info = info->section_info;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	for (; section_num <= 0; section_num--) {
		ret = __sections_download(adapter, section_info);
		section_info++;
	}

	while (cnt--) {
		val8 = GET_FIELD(MAC_REG_R8(R_AX_WCPU_FW_CTRL),
				 B_AX_WCPU_FWDL_STS);
		if (val8 == S_AX_WCPU_FWDL_STS_INIRDY)
			break;
		PLTFM_DELAY_US(1);
	}

	if (!cnt) {
		PLTFM_MSG_ERR("[ERR]FWDL init ready\n");
		return MACPOLLTO;
	}

	adapter->sm.fwdl = MAC_AX_FWDL_INIT_RDY;

	return 0;
}

u32 mac_fwdl(struct mac_ax_adapter *adapter, u8 *fw, u32 len)
{
	u8 val8;
	u32 ret;
	struct fw_bin_info info;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	u8 *Content = NULL;
	u32 index, index1,DLSize,TargetAddr,WriteAddr,SegSize,SegBytes;
	struct mac_ax_ops *mac_ops = adapter_to_mac_ops(adapter);

	ret = fwhdr_parser(adapter, fw, len, &info);
	if (ret) {
		PLTFM_MSG_ERR("[ERR]fw header parsing\n");
		goto fwdl_err;
	}

	for(index=0;index<info.section_num;index++){
		PLTFM_MSG_ERR("addr: 0x%x, dladdr: 0x%x, len: 0x%x\n", info.section_info[index].addr,info.section_info[index].dladdr, info.section_info[index].len);

		DLSize = info.section_info[index].len;
		TargetAddr = info.section_info[index].dladdr;
		Content = (u8 *)PLTFM_MALLOC(DLSize);
		PLTFM_MEMCPY(Content, info.section_info[index].addr, DLSize);
		index1 = 0;		
		while(DLSize != 0) {
			MAC_REG_W32(0xC04, TargetAddr);
			WriteAddr = 0x40000;
		
			if(DLSize >= ROMDL_SEG_LEN) {
				SegSize = ROMDL_SEG_LEN;
			}
			else {
				SegSize = DLSize;
			}
		
			SegBytes = SegSize;
		
			while(SegBytes != 0){
				MAC_REG_W32(WriteAddr, cpu_to_le32(*((u32*)(Content+index1))));
		
				SegBytes -= 4;
				WriteAddr += 4;
				index1 += 4;
			}
		
			TargetAddr += SegSize;
			DLSize -= SegSize;
		}
		PLTFM_FREE(Content, info.section_info[index].len);

	}

	MAC_REG_W8(R_AX_WCPU_FW_CTRL, 0xAB);

#if 0
	ret = update_fw_ver(adapter, (struct fwhdr_hdr_t *)fw);
	if (ret)
		goto fwdl_err;

	ret = fwdl_phase0(adapter);
	if (ret) {
		PLTFM_MSG_ERR("[ERR]fwdl phase0\n");
		goto fwdl_err;
	}

	ret = fwdl_phase1(adapter, fw, info.hdr_len);
	if (ret) {
		PLTFM_MSG_ERR("[ERR]fwdl phase1\n");
		goto fwdl_err;
	}

	ret = fwdl_phase2(adapter, fw, &info);
	if (ret) {
		PLTFM_MSG_ERR("[ERR]fwdl phase1\n");
		goto fwdl_err;
	}
#endif
	return 0;

fwdl_err:
	PLTFM_MSG_ERR("[ERR]fwdl ret = %d\n", ret);
	val8 = MAC_REG_R8(R_AX_WCPU_FW_CTRL);
	val8 &= ~B_AX_WCPU_FWDL_EN;
	MAC_REG_W8(R_AX_WCPU_FW_CTRL, val8);
	PLTFM_MSG_ERR("[ERR]fwdl 0x1E0 = %x\n", val8);

	return ret;
}

u32 mac_romdl(struct mac_ax_adapter *adapter, u8 *ROM, u32 ROMAddr, u32 Len)
{
	u8 *Content = NULL;
	u32 DLSize;
	u32 TargetAddr, WriteAddr;
	u32 SegSize, SegBytes;
	u32 val32;
	u32 index = 0;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	if(ROM == NULL)
		return MACNOFW;

	val32 = MAC_REG_R32(0xC00);

	if(val32 & BIT(0)) {
		Content = (u8 *)PLTFM_MALLOC(Len);
		PLTFM_MEMCPY(Content, ROM, Len);

		DLSize = Len;
		TargetAddr = ROMAddr;

		while(DLSize != 0) {
			MAC_REG_W32(0xC04, TargetAddr);
			WriteAddr = 0x40000;

			if(DLSize >= ROMDL_SEG_LEN) {
				SegSize = ROMDL_SEG_LEN;
			}
			else {
				SegSize = DLSize;
			}

			SegBytes = SegSize;

			while(SegBytes != 0){
				MAC_REG_W32(WriteAddr, cpu_to_le32(*((u32*)(Content+index))));

				SegBytes -= 4;
				WriteAddr += 4;
				index += 4;
			}

			TargetAddr += SegSize;
			DLSize -= SegSize;
		}
	}
	else {
		PLTFM_MSG_ERR("[ERR]secure enabled, indirect access disabled\n");
		return MACSECUREON;
	}

	PLTFM_FREE(Content, Len);

	val32 = MAC_REG_R32(0x88);
	val32 &= ~BIT(0);
	MAC_REG_W32(0x88, val32);

	val32 |= BIT(0);
	MAC_REG_W32(0x88, val32);

	return 0;

}

