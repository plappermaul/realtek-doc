/*
 * Copyright (c) Cortina-Access Limited 2015.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
/*
 * aal_common.c: Common functions used by all AAL modules
 *
 */

#include "ca_ne_autoconf.h"
//#include <linux/crc32.h>
//#include <linux/crc-itu-t.h>
//#include <linux/crc-ccitt.h>
#include "osal_common.h"
#include "aal_common.h"
#include "aal_table.h"
#include "ca_aal_proc.h"
#include "scfg.h"

/***************************************************************************
 * hash related
 ***************************************************************************/
static unsigned short crctable_ccitt16[256] =
{
 0x0000, 0x1189, 0x2312, 0x329B, 0x4624, 0x57AD, 0x6536, 0x74BF,
 0x8C48, 0x9DC1, 0xAF5A, 0xBED3, 0xCA6C, 0xDBE5, 0xE97E, 0xF8F7,
 0x1081, 0x0108, 0x3393, 0x221A, 0x56A5, 0x472C, 0x75B7, 0x643E,
 0x9CC9, 0x8D40, 0xBFDB, 0xAE52, 0xDAED, 0xCB64, 0xF9FF, 0xE876,
 0x2102, 0x308B, 0x0210, 0x1399, 0x6726, 0x76AF, 0x4434, 0x55BD,
 0xAD4A, 0xBCC3, 0x8E58, 0x9FD1, 0xEB6E, 0xFAE7, 0xC87C, 0xD9F5,
 0x3183, 0x200A, 0x1291, 0x0318, 0x77A7, 0x662E, 0x54B5, 0x453C,
 0xBDCB, 0xAC42, 0x9ED9, 0x8F50, 0xFBEF, 0xEA66, 0xD8FD, 0xC974,
 0x4204, 0x538D, 0x6116, 0x709F, 0x0420, 0x15A9, 0x2732, 0x36BB,
 0xCE4C, 0xDFC5, 0xED5E, 0xFCD7, 0x8868, 0x99E1, 0xAB7A, 0xBAF3,
 0x5285, 0x430C, 0x7197, 0x601E, 0x14A1, 0x0528, 0x37B3, 0x263A,
 0xDECD, 0xCF44, 0xFDDF, 0xEC56, 0x98E9, 0x8960, 0xBBFB, 0xAA72,
 0x6306, 0x728F, 0x4014, 0x519D, 0x2522, 0x34AB, 0x0630, 0x17B9,
 0xEF4E, 0xFEC7, 0xCC5C, 0xDDD5, 0xA96A, 0xB8E3, 0x8A78, 0x9BF1,
 0x7387, 0x620E, 0x5095, 0x411C, 0x35A3, 0x242A, 0x16B1, 0x0738,
 0xFFCF, 0xEE46, 0xDCDD, 0xCD54, 0xB9EB, 0xA862, 0x9AF9, 0x8B70,
 0x8408, 0x9581, 0xA71A, 0xB693, 0xC22C, 0xD3A5, 0xE13E, 0xF0B7,
 0x0840, 0x19C9, 0x2B52, 0x3ADB, 0x4E64, 0x5FED, 0x6D76, 0x7CFF,
 0x9489, 0x8500, 0xB79B, 0xA612, 0xD2AD, 0xC324, 0xF1BF, 0xE036,
 0x18C1, 0x0948, 0x3BD3, 0x2A5A, 0x5EE5, 0x4F6C, 0x7DF7, 0x6C7E,
 0xA50A, 0xB483, 0x8618, 0x9791, 0xE32E, 0xF2A7, 0xC03C, 0xD1B5,
 0x2942, 0x38CB, 0x0A50, 0x1BD9, 0x6F66, 0x7EEF, 0x4C74, 0x5DFD,
 0xB58B, 0xA402, 0x9699, 0x8710, 0xF3AF, 0xE226, 0xD0BD, 0xC134,
 0x39C3, 0x284A, 0x1AD1, 0x0B58, 0x7FE7, 0x6E6E, 0x5CF5, 0x4D7C,
 0xC60C, 0xD785, 0xE51E, 0xF497, 0x8028, 0x91A1, 0xA33A, 0xB2B3,
 0x4A44, 0x5BCD, 0x6956, 0x78DF, 0x0C60, 0x1DE9, 0x2F72, 0x3EFB,
 0xD68D, 0xC704, 0xF59F, 0xE416, 0x90A9, 0x8120, 0xB3BB, 0xA232,
 0x5AC5, 0x4B4C, 0x79D7, 0x685E, 0x1CE1, 0x0D68, 0x3FF3, 0x2E7A,
 0xE70E, 0xF687, 0xC41C, 0xD595, 0xA12A, 0xB0A3, 0x8238, 0x93B1,
 0x6B46, 0x7ACF, 0x4854, 0x59DD, 0x2D62, 0x3CEB, 0x0E70, 0x1FF9,
 0xF78F, 0xE606, 0xD49D, 0xC514, 0xB1AB, 0xA022, 0x92B9, 0x8330,
 0x7BC7, 0x6A4E, 0x58D5, 0x495C, 0x3DE3, 0x2C6A, 0x1EF1, 0x0F78
};

static unsigned long  crctable_32[256] =
{
 0x00000000L, 0x77073096L, 0xEE0E612CL, 0x990951BAL,
 0x076DC419L, 0x706AF48FL, 0xE963A535L, 0x9E6495A3L,
 0x0EDB8832L, 0x79DCB8A4L, 0xE0D5E91EL, 0x97D2D988L,
 0x09B64C2BL, 0x7EB17CBDL, 0xE7B82D07L, 0x90BF1D91L,
 0x1DB71064L, 0x6AB020F2L, 0xF3B97148L, 0x84BE41DEL,
 0x1ADAD47DL, 0x6DDDE4EBL, 0xF4D4B551L, 0x83D385C7L,
 0x136C9856L, 0x646BA8C0L, 0xFD62F97AL, 0x8A65C9ECL,
 0x14015C4FL, 0x63066CD9L, 0xFA0F3D63L, 0x8D080DF5L,
 0x3B6E20C8L, 0x4C69105EL, 0xD56041E4L, 0xA2677172L,
 0x3C03E4D1L, 0x4B04D447L, 0xD20D85FDL, 0xA50AB56BL,
 0x35B5A8FAL, 0x42B2986CL, 0xDBBBC9D6L, 0xACBCF940L,
 0x32D86CE3L, 0x45DF5C75L, 0xDCD60DCFL, 0xABD13D59L,
 0x26D930ACL, 0x51DE003AL, 0xC8D75180L, 0xBFD06116L,
 0x21B4F4B5L, 0x56B3C423L, 0xCFBA9599L, 0xB8BDA50FL,
 0x2802B89EL, 0x5F058808L, 0xC60CD9B2L, 0xB10BE924L,
 0x2F6F7C87L, 0x58684C11L, 0xC1611DABL, 0xB6662D3DL,
 0x76DC4190L, 0x01DB7106L, 0x98D220BCL, 0xEFD5102AL,
 0x71B18589L, 0x06B6B51FL, 0x9FBFE4A5L, 0xE8B8D433L,
 0x7807C9A2L, 0x0F00F934L, 0x9609A88EL, 0xE10E9818L,
 0x7F6A0DBBL, 0x086D3D2DL, 0x91646C97L, 0xE6635C01L,
 0x6B6B51F4L, 0x1C6C6162L, 0x856530D8L, 0xF262004EL,
 0x6C0695EDL, 0x1B01A57BL, 0x8208F4C1L, 0xF50FC457L,
 0x65B0D9C6L, 0x12B7E950L, 0x8BBEB8EAL, 0xFCB9887CL,
 0x62DD1DDFL, 0x15DA2D49L, 0x8CD37CF3L, 0xFBD44C65L,
 0x4DB26158L, 0x3AB551CEL, 0xA3BC0074L, 0xD4BB30E2L,
 0x4ADFA541L, 0x3DD895D7L, 0xA4D1C46DL, 0xD3D6F4FBL,
 0x4369E96AL, 0x346ED9FCL, 0xAD678846L, 0xDA60B8D0L,
 0x44042D73L, 0x33031DE5L, 0xAA0A4C5FL, 0xDD0D7CC9L,
 0x5005713CL, 0x270241AAL, 0xBE0B1010L, 0xC90C2086L,
 0x5768B525L, 0x206F85B3L, 0xB966D409L, 0xCE61E49FL,
 0x5EDEF90EL, 0x29D9C998L, 0xB0D09822L, 0xC7D7A8B4L,
 0x59B33D17L, 0x2EB40D81L, 0xB7BD5C3BL, 0xC0BA6CADL,
 0xEDB88320L, 0x9ABFB3B6L, 0x03B6E20CL, 0x74B1D29AL,
 0xEAD54739L, 0x9DD277AFL, 0x04DB2615L, 0x73DC1683L,
 0xE3630B12L, 0x94643B84L, 0x0D6D6A3EL, 0x7A6A5AA8L,
 0xE40ECF0BL, 0x9309FF9DL, 0x0A00AE27L, 0x7D079EB1L,
 0xF00F9344L, 0x8708A3D2L, 0x1E01F268L, 0x6906C2FEL,
 0xF762575DL, 0x806567CBL, 0x196C3671L, 0x6E6B06E7L,
 0xFED41B76L, 0x89D32BE0L, 0x10DA7A5AL, 0x67DD4ACCL,
 0xF9B9DF6FL, 0x8EBEEFF9L, 0x17B7BE43L, 0x60B08ED5L,
 0xD6D6A3E8L, 0xA1D1937EL, 0x38D8C2C4L, 0x4FDFF252L,
 0xD1BB67F1L, 0xA6BC5767L, 0x3FB506DDL, 0x48B2364BL,
 0xD80D2BDAL, 0xAF0A1B4CL, 0x36034AF6L, 0x41047A60L,
 0xDF60EFC3L, 0xA867DF55L, 0x316E8EEFL, 0x4669BE79L,
 0xCB61B38CL, 0xBC66831AL, 0x256FD2A0L, 0x5268E236L,
 0xCC0C7795L, 0xBB0B4703L, 0x220216B9L, 0x5505262FL,
 0xC5BA3BBEL, 0xB2BD0B28L, 0x2BB45A92L, 0x5CB36A04L,
 0xC2D7FFA7L, 0xB5D0CF31L, 0x2CD99E8BL, 0x5BDEAE1DL,
 0x9B64C2B0L, 0xEC63F226L, 0x756AA39CL, 0x026D930AL,
 0x9C0906A9L, 0xEB0E363FL, 0x72076785L, 0x05005713L,
 0x95BF4A82L, 0xE2B87A14L, 0x7BB12BAEL, 0x0CB61B38L,
 0x92D28E9BL, 0xE5D5BE0DL, 0x7CDCEFB7L, 0x0BDBDF21L,
 0x86D3D2D4L, 0xF1D4E242L, 0x68DDB3F8L, 0x1FDA836EL,
 0x81BE16CDL, 0xF6B9265BL, 0x6FB077E1L, 0x18B74777L,
 0x88085AE6L, 0xFF0F6A70L, 0x66063BCAL, 0x11010B5CL,
 0x8F659EFFL, 0xF862AE69L, 0x616BFFD3L, 0x166CCF45L,
 0xA00AE278L, 0xD70DD2EEL, 0x4E048354L, 0x3903B3C2L,
 0xA7672661L, 0xD06016F7L, 0x4969474DL, 0x3E6E77DBL,
 0xAED16A4AL, 0xD9D65ADCL, 0x40DF0B66L, 0x37D83BF0L,
 0xA9BCAE53L, 0xDEBB9EC5L, 0x47B2CF7FL, 0x30B5FFE9L,
 0xBDBDF21CL, 0xCABAC28AL, 0x53B39330L, 0x24B4A3A6L,
 0xBAD03605L, 0xCDD70693L, 0x54DE5729L, 0x23D967BFL,
 0xB3667A2EL, 0xC4614AB8L, 0x5D681B02L, 0x2A6F2B94L,
 0xB40BBE37L, 0xC30C8EA1L, 0x5A05DF1BL, 0x2D02EF8DL
};

typedef int TYPE;
static TYPE reverse_n(TYPE x, int bits);
static int bit_reverse(unsigned int *uint_ptr, unsigned int data_len);
static uint32_t _ca_crc32(unsigned char *buff, size_t size);
static uint16_t _ca_crc16(unsigned char *buff, size_t size);

static TYPE reverse_n(TYPE x, int bits)
{
	TYPE m = ~0;
	switch (bits) {
	case 64:
		x = (x & 0xFFFFFFFF00000000 & m) >> 16 |
			(x & 0x00000000FFFFFFFF & m) << 16;
	case 32:
		x = (x & 0xFFFF0000FFFF0000 & m) >> 16 |
			(x & 0x0000FFFF0000FFFF & m) << 16;
	case 16:
		x = (x & 0xFF00FF00FF00FF00 & m) >> 8 |
			(x & 0x00FF00FF00FF00FF & m) << 8;
	case 8:
		x = (x & 0xF0F0F0F0F0F0F0F0 & m) >> 4 |
			(x & 0x0F0F0F0F0F0F0F0F & m) << 4;
		x = (x & 0xCCCCCCCCCCCCCCCC & m) >> 2 |
			(x & 0x3333333333333333 & m) << 2;
		x = (x & 0xAAAAAAAAAAAAAAAA & m) >> 1 |
			(x & 0x5555555555555555 & m) << 1;
	}
	return x;
}

static int bit_reverse(unsigned int *uint_ptr, unsigned int data_len) {
        unsigned int tmp, i;
        unsigned int array_size;

        if (data_len & 0x3) /* only deal with %4 = 0 */
                return AAL_E_INVALID;

        array_size = data_len / sizeof(unsigned int);

        for (i = 0; i < array_size / 2; i++){
                tmp = uint_ptr[i];
                uint_ptr[i] = reverse_n(uint_ptr[array_size - 1 - i], 32);
                uint_ptr[array_size - 1 - i] = reverse_n(tmp, 32);
        }

        if (array_size & 0x1) /* odd */
                uint_ptr[i] = reverse_n(uint_ptr[i], 32);

        return AAL_E_OK;
}

static uint32_t _ca_crc32(unsigned char *buff, size_t size)
{
	uint32_t crc32 = 0xFFFFFFFF;
	uint16_t i;

	for (i = 0; i < size; i++) {
		crc32 = crctable_32[(crc32 ^ (unsigned char)buff[i]) & 0xFFL] ^
			(crc32 >> 8);
	}
	crc32 = reverse_n(crc32, 32);

	return crc32;
}

static uint16_t _ca_crc16(unsigned char *buff, size_t size)
{
	uint16_t crc16 = 0xFFFF;
	uint16_t i;

	for (i = 0; i < size; i++)
		crc16 = crctable_ccitt16[(crc16 ^
				((unsigned char)buff[i])) & 0xFFL] ^
			(crc16 >> 8);

	crc16 = reverse_n(crc16, 16);

	return crc16;
}

/* CCITT-CRC32 polynomial:
 *   X^32 + X^26 + X^23 + X^22 + X^16 + X^12 + X^11 + X^10 + X^8 + X^7 + X^5 + X^4 + X^2 + X^1 + 1
 */
uint32_t ca_crc32(uint32_t crc, unsigned char *p, size_t len)
{
	/* There are many variations of CRC32, for example:
	 * normal case: uses seed ~0, xor's with ~0.
	 * G2: uses seed ~0, doesn't xor with ~0.
	 * other: uses seed 0, doesn't xor with ~0.
	 */
	return crc32(~0L, p, len) ^ ~0L;
}

/* CCITT-CRC16 polynomial:
 *   X^16 + X^12 + X^5 + 1
 */
uint16_t ca_crc16(uint16_t crc, unsigned char *p, size_t len)
{
	/* According to http://srecord.sourceforge.net/crc16-ccitt.html and
	 * the result of Linux crc_ccitt() matches the bad result of the page.
	 * Refer to http://www.lammertbies.nl/comm/info/crc-calculation.html ,
	 * Linux crc_itu_t() matches the result of CRC-CCITT(init value).
	 * Whatever, G2's function result does not match any of them.
	 */
//	return crc_ccitt(0xFFFF, p, len);
	return crc_itu_t(0x1D0F, p, len);
}

/* unmask all fields in hash mask */
inline void hash_mask_unmask(aal_hash_mask_t *mask)
{
	if (mask == NULL) return;
	memset(mask, -1, sizeof(aal_hash_mask_t));
	mask->ip_sa = 0;
	mask->ip_da = 0;
	mask->ip_ttl = 0;
}

void hash_key_dump(aal_hash_key_t *hash_key)
{
	if (hash_key  == NULL) return;

//	printf(KERN_INFO "<Hash Control>\n");
	printf("<Hash Control>\n");
	if (hash_key->table_id)
//		printf(KERN_INFO " table_id(4): 0x%x\n", hash_key->table_id);
		printf(" table_id(4): 0x%x\n", hash_key->table_id);
	if (hash_key->ctrl_set_id)
//		printf(KERN_INFO " ctrl_set_id(4): 0x%x\n", hash_key->ctrl_set_id);
		printf(" ctrl_set_id(4): 0x%x\n", hash_key->ctrl_set_id);
	if (hash_key->hkey_id)
//		printf(KERN_INFO " hkey_id(4): 0x%x\n", hash_key->hkey_id);
		printf(" hkey_id(4): 0x%x\n", hash_key->hkey_id);

//	printf(KERN_INFO "<SPID>\n");
	printf("<SPID>\n");
	if (hash_key->lspid)
//		printf(KERN_INFO " lspid(6): 0x%02x\n", hash_key->lspid);
		printf(" lspid(6): 0x%02x\n", hash_key->lspid);
	if (hash_key->orig_lspid)
//		printf(KERN_INFO " orig_lspid(6): 0x%02x\n", hash_key->orig_lspid);
		printf(" orig_lspid(6): 0x%02x\n", hash_key->orig_lspid);
	if (hash_key->mc_idx_vld)
//		printf(KERN_INFO " mc_idx_vld: %u\n", hash_key->mc_idx_vld);
		printf(" mc_idx_vld: %u\n", hash_key->mc_idx_vld);

//	printf(KERN_INFO "<DPID>\n");
	printf("<DPID>\n");
	if ((hash_key->mc) || (hash_key->mcgid)) {
//		printf(KERN_INFO " mc: %u\n", hash_key->mc);
		printf(" mc: %u\n", hash_key->mc);
//		printf(KERN_INFO " mcgid/ldpid(10): 0x%03x\n", hash_key->mcgid);
		printf(" mcgid/ldpid(10): 0x%03x\n", hash_key->mcgid);
	}

//	printf(KERN_INFO "<COS>\n");
	printf("<COS>\n");
	if (hash_key->cos)
//		printf(KERN_INFO " cos(3): %u\n", hash_key->cos);
		printf(" cos(3): %u\n", hash_key->cos);

//	printf(KERN_INFO "<POL_ID>\n");
	printf("<POL_ID>\n");
	if (hash_key->pol_id)
//		printf(KERN_INFO " pol_id(9): 0x%03x\n", hash_key->pol_id);
		printf(" pol_id(9): 0x%03x\n", hash_key->pol_id);
	if (hash_key->pol_grp_id)
//		printf(KERN_INFO " pol_grp_id(3): %u\n", hash_key->pol_grp_id);
		printf(" pol_grp_id(3): %u\n", hash_key->pol_grp_id);
	if (hash_key->qos_premark)
//		printf(KERN_INFO " qos_premark: %u\n", hash_key->qos_premark);
		printf(" qos_premark: %u\n", hash_key->qos_premark);

//	printf(KERN_INFO "<MDATA>\n");
	printf("<MDATA>\n");
	if (hash_key->mdata)
//		printf(KERN_INFO " mdata: 0x%016llx\n", hash_key->mdata);
		printf(" mdata: 0x%016llx\n", hash_key->mdata);

//	printf(KERN_INFO "<Special Packet>\n");
	printf("<Special Packet>\n");
	if (hash_key->spcl_pkt_enc)
//		printf(KERN_INFO" spcl_pkt_enc(6): 0x%02x\n", hash_key->spcl_pkt_enc);
		printf(" spcl_pkt_enc(6): 0x%02x\n", hash_key->spcl_pkt_enc);
	if (hash_key->spcl_pkt_hdr_mtch)
//		printf(KERN_INFO" spcl_pkt_hdr_mtch(8): 0x%02x\n", hash_key->spcl_pkt_hdr_mtch);
		printf(" spcl_pkt_hdr_mtch(8): 0x%02x\n", hash_key->spcl_pkt_hdr_mtch);

//	printf(KERN_INFO "<L2>\n");
	printf("<L2>\n");
	if ((hash_key->mac_da_0) || (hash_key->mac_da_1) ||
	    (hash_key->mac_da_2) || (hash_key->mac_da_3) ||
	    (hash_key->mac_da_4) || (hash_key->mac_da_5)) {
//		printf(KERN_INFO " mac_da: %02x-%02x-%02x-%02x-%02x-%02x\n",
//		    hash_key->mac_da_5, hash_key->mac_da_4,
//		    hash_key->mac_da_3, hash_key->mac_da_2,
//		    hash_key->mac_da_1, hash_key->mac_da_0);
		printf(" mac_da: %02x-%02x-%02x-%02x-%02x-%02x\n",
		    hash_key->mac_da_5, hash_key->mac_da_4,
		    hash_key->mac_da_3, hash_key->mac_da_2,
		    hash_key->mac_da_1, hash_key->mac_da_0);
	}
	if (hash_key->mac_da_an_sel)
//		printf(KERN_INFO " mac_da_an_sel(4): 0x%x\n", hash_key->mac_da_an_sel);
		printf(" mac_da_an_sel(4): 0x%x\n", hash_key->mac_da_an_sel);
	if (hash_key->mac_da_ip_mc)
//		printf(KERN_INFO " mac_da_ip_mc: %u\n", hash_key->mac_da_ip_mc);
		printf(" mac_da_ip_mc: %u\n", hash_key->mac_da_ip_mc);
	if (hash_key->mac_da_rng)
//		printf(KERN_INFO " mac_da_rng: %u\n", hash_key->mac_da_rng);
		printf(" mac_da_rng: %u\n", hash_key->mac_da_rng);
	if (hash_key->mac_da_rsvd)
//		printf(KERN_INFO " mac_da_rsvd: %u\n", hash_key->mac_da_rsvd);
		printf(" mac_da_rsvd: %u\n", hash_key->mac_da_rsvd);
	if ((hash_key->mac_sa_0) || (hash_key->mac_sa_1) ||
	    (hash_key->mac_sa_2) || (hash_key->mac_sa_3) ||
	    (hash_key->mac_sa_4) || (hash_key->mac_sa_5)) {
//		printf(KERN_INFO " mac_sa: %02x-%02x-%02x-%02x-%02x-%02x\n",
//		    hash_key->mac_sa_5, hash_key->mac_sa_4,
//		    hash_key->mac_sa_3, hash_key->mac_sa_2,
//		    hash_key->mac_sa_1, hash_key->mac_sa_0);
		printf(" mac_sa: %02x-%02x-%02x-%02x-%02x-%02x\n",
		    hash_key->mac_sa_5, hash_key->mac_sa_4,
		    hash_key->mac_sa_3, hash_key->mac_sa_2,
		    hash_key->mac_sa_1, hash_key->mac_sa_0);
	}
	if (hash_key->ethertype)
//		printf(KERN_INFO " ethertype(16): 0x%04x\n", hash_key->ethertype);
		printf(" ethertype(16): 0x%04x\n", hash_key->ethertype);
	if (hash_key->ethertype_enc)
//		printf(KERN_INFO " ethertype_enc(6): 0x%02x\n", hash_key->ethertype_enc);
		printf(" ethertype_enc(6): 0x%02x\n", hash_key->ethertype_enc);

//	printf(KERN_INFO "<L2 Format>\n");
	printf("<L2 Format>\n");
	if (hash_key->len_encoded)
//		printf(KERN_INFO " len_encoded: %u\n", hash_key->len_encoded);
		printf(" len_encoded: %u\n", hash_key->len_encoded);
	if (hash_key->pktlen_rng_match_vec)
//		printf(KERN_INFO " pktlen_rng_match_vec(4): 0x%x\n", hash_key->pktlen_rng_match_vec);
		printf(" pktlen_rng_match_vec(4): 0x%x\n", hash_key->pktlen_rng_match_vec);
	if (hash_key->llc_snap)
//		printf(KERN_INFO " llc_snap(2): %u\n", hash_key->llc_snap);
		printf(" llc_snap(2): %u\n", hash_key->llc_snap);
	if (hash_key->llc_type_enc)
//		printf(KERN_INFO " llc_type_enc(2): %u\n", hash_key->llc_type_enc);
		printf(" llc_type_enc(2): %u\n", hash_key->llc_type_enc);

//	printf(KERN_INFO "<VLAN>\n");
	printf("<VLAN>\n");
	if (hash_key->vlan_cnt)
//		printf(KERN_INFO " vlan_cnt(2): %u\n", hash_key->vlan_cnt);
		printf(" vlan_cnt(2): %u\n", hash_key->vlan_cnt);
	if (hash_key->top_tpid_enc)
//		printf(KERN_INFO " top_tpid_enc(3): %u\n", hash_key->top_tpid_enc);
		printf(" top_tpid_enc(3): %u\n", hash_key->top_tpid_enc);
	if (hash_key->top_vid)
//		printf(KERN_INFO " top_vid(12): 0x%03x\n", hash_key->top_vid);
		printf(" top_vid(12): 0x%03x\n", hash_key->top_vid);
	if (hash_key->top_8021p)
//		printf(KERN_INFO " top_8021p(3): %u\n", hash_key->top_8021p);
		printf(" top_8021p(3): %u\n", hash_key->top_8021p);
	if (hash_key->top_dei)
//		printf(KERN_INFO " top_dei: %u\n", hash_key->top_dei);
		printf(" top_dei: %u\n", hash_key->top_dei);
	if (hash_key->inner_tpid_enc)
//		printf(KERN_INFO " inner_tpid_enc(3): %u\n", hash_key->inner_tpid_enc);
		printf(" inner_tpid_enc(3): %u\n", hash_key->inner_tpid_enc);
	if (hash_key->inner_vid)
//		printf(KERN_INFO " inner_vid(12): 0x%03x\n", hash_key->inner_vid);
		printf(" inner_vid(12): 0x%03x\n", hash_key->inner_vid);
	if (hash_key->inner_8021p)
//		printf(KERN_INFO " inner_8021p(3): %u\n", hash_key->inner_8021p);
		printf(" inner_8021p(3): %u\n", hash_key->inner_8021p);
	if (hash_key->inner_dei)
//		printf(KERN_INFO " inner_dei: %u\n", hash_key->inner_dei);
		printf(" inner_dei: %u\n", hash_key->inner_dei);

//	printf(KERN_INFO "<PPP / PPPoE>\n");
	printf("<PPP / PPPoE>\n");
	if (hash_key->pppoe_type)
//		printf(KERN_INFO " pppoe_type(2): %u\n", hash_key->pppoe_type);
		printf(" pppoe_type(2): %u\n", hash_key->pppoe_type);
	if (hash_key->pppoe_code_enc)
//		printf(KERN_INFO " pppoe_code_enc(4): 0x%x\n", hash_key->pppoe_code_enc);
		printf(" pppoe_code_enc(4): 0x%x\n", hash_key->pppoe_code_enc);
	if (hash_key->pppoe_session_id)
//		printf(KERN_INFO " pppoe_session_id(16): 0x%04x\n", hash_key->pppoe_session_id);
		printf(" pppoe_session_id(16): 0x%04x\n", hash_key->pppoe_session_id);
	if (hash_key->ppp_protocol_enc)
//		printf(KERN_INFO " ppp_protocol_enc(4): 0x%x\n", hash_key->ppp_protocol_enc);
		printf(" ppp_protocol_enc(4): 0x%x\n", hash_key->ppp_protocol_enc);

//	printf(KERN_INFO "<L3>\n");
	printf("<L3>\n");
	if (hash_key->ip_vld)
//		printf(KERN_INFO " ip_vld: %u\n", hash_key->ip_vld);
		printf(" ip_vld: %u\n", hash_key->ip_vld);
	if (hash_key->ip_ver)
//		printf(" ip_ver: %u\n", hash_key->ip_ver);
		printf(" ip_ver: %u\n", hash_key->ip_ver);
	if (hash_key->ip_dscp)
//		printf(KERN_INFO " ip_dscp(6): 0x%02x\n", hash_key->ip_dscp);
		printf(" ip_dscp(6): 0x%02x\n", hash_key->ip_dscp);
	if (hash_key->ip_ecn)
//		printf(KERN_INFO " ip_ecn(2): %u\n", hash_key->ip_ecn);
		printf(" ip_ecn(2): %u\n", hash_key->ip_ecn);
	if (hash_key->ip_protocol)
//		printf(KERN_INFO " ip_protocol(8): 0x%02x\n", hash_key->ip_protocol);
		printf(" ip_protocol(8): 0x%02x\n", hash_key->ip_protocol);
	if (hash_key->ip_l4_type)
//		printf(KERN_INFO " ip_l4_type(3): %u\n", hash_key->ip_l4_type);
		printf(" ip_l4_type(3): %u\n", hash_key->ip_l4_type);
	if ((hash_key->ip_sa_0) || (hash_key->ip_sa_1) || (hash_key->ip_sa_2) || (hash_key->ip_sa_3)) {
//		printf(KERN_INFO " ip_sa: %08x-%08x-%08x-%08x\n",
//		    hash_key->ip_sa_3, hash_key->ip_sa_2, hash_key->ip_sa_1, hash_key->ip_sa_0);
		printf(" ip_sa: %08x-%08x-%08x-%08x\n",
		    hash_key->ip_sa_3, hash_key->ip_sa_2, hash_key->ip_sa_1, hash_key->ip_sa_0);
	}
	if ((hash_key->ip_da_0) || (hash_key->ip_da_1) || (hash_key->ip_da_2) || (hash_key->ip_da_3)) {
//		printf(KERN_INFO " ip_da: %08x-%08x-%08x-%08x\n",
//		    hash_key->ip_da_3, hash_key->ip_da_2, hash_key->ip_da_1, hash_key->ip_da_0);
		printf(" ip_da: %08x-%08x-%08x-%08x\n",
		    hash_key->ip_da_3, hash_key->ip_da_2, hash_key->ip_da_1, hash_key->ip_da_0);
	}
	if (hash_key->ipv6_flow_lbl)
//		printf(KERN_INFO " ipv6_flow_lbl(20): 0x%05x\n", hash_key->ipv6_flow_lbl);
		printf(" ipv6_flow_lbl(20): 0x%05x\n", hash_key->ipv6_flow_lbl);
	if (hash_key->ip_ttl)
//		printf(KERN_INFO " ip_ttl(8): 0x%02x\n", hash_key->ip_ttl);
		printf(" ip_ttl(8): 0x%02x\n", hash_key->ip_ttl);
	if (hash_key->ip_options)
//		printf(KERN_INFO " ip_options: %u\n", hash_key->ip_options);
		printf(" ip_options: %u\n", hash_key->ip_options);
	if (hash_key->ip_da_sa_equal)
//		printf(KERN_INFO " ip_da_sa_equal: %u\n", hash_key->ip_da_sa_equal);
		printf(" ip_da_sa_equal: %u\n", hash_key->ip_da_sa_equal);
	if (hash_key->ip_fragment_flag)
//		printf(KERN_INFO " ip_fragment_flag: %u\n", hash_key->ip_fragment_flag);
		printf(" ip_fragment_flag: %u\n", hash_key->ip_fragment_flag);
	if (hash_key->ipv6_hbh)
//		printf(KERN_INFO " ipv6_hbh: %u\n", hash_key->ipv6_hbh);
		printf(" ipv6_hbh: %u\n", hash_key->ipv6_hbh);
	if (hash_key->ipv6_rh)
//		printf(KERN_INFO " ipv6_rh: %u\n", hash_key->ipv6_rh);
		printf(" ipv6_rh: %u\n", hash_key->ipv6_rh);
	if (hash_key->ipv6_doh)
//		printf(KERN_INFO " ipv6_doh: %u\n", hash_key->ipv6_doh);
		printf(" ipv6_doh: %u\n", hash_key->ipv6_doh);
	if (hash_key->icmp_vld)
//		printf(KERN_INFO " icmp_vld(3): %u\n", hash_key->icmp_vld);
		printf(" icmp_vld(3): %u\n", hash_key->icmp_vld);
	if (hash_key->icmp_type)
//		printf(KERN_INFO " icmp_type(8): 0x%02x\n", hash_key->icmp_type);
		printf(" icmp_type(8): 0x%02x\n", hash_key->icmp_type);
	if (hash_key->spi_vld)
//		printf(KERN_INFO " spi_vld(3): %u\n", hash_key->spi_vld);
		printf(" spi_vld(3): %u\n", hash_key->spi_vld);
	if (hash_key->spi)
//		printf(KERN_INFO " spi(32): 0x%08x\n", hash_key->spi);
		printf(" spi(32): 0x%08x\n", hash_key->spi);
	if (hash_key->l3_chksum_err)
//		printf(KERN_INFO " l3_chksum_err: %u\n", hash_key->l3_chksum_err);
		printf(" l3_chksum_err: %u\n", hash_key->l3_chksum_err);

//	printf(KERN_INFO "<L4>\n");
	printf("<L4>\n");
	if (hash_key->l4_sp_exact_range)
//		printf(KERN_INFO " l4_sp_exact_ragne(16): 0x%04x\n", hash_key->l4_sp_exact_range);
		printf(" l4_sp_exact_ragne(16): 0x%04x\n", hash_key->l4_sp_exact_range);
	if (hash_key->l4_dp_exact_range)
//		printf(KERN_INFO " l4_dp_exact_ragne(16): 0x%04x\n", hash_key->l4_dp_exact_range);
		printf(" l4_dp_exact_ragne(16): 0x%04x\n", hash_key->l4_dp_exact_range);
	if (hash_key->tcp_rdp_ctrl)
//		printf(KERN_INFO " tcp_rdp_ctrl(9): 0x%03x\n", hash_key->tcp_rdp_ctrl);
		printf(" tcp_rdp_ctrl(9): 0x%03x\n", hash_key->tcp_rdp_ctrl);
	if (hash_key->l4_chksum_zero)
//		printf(KERN_INFO " l4_chksum_zero: %u\n", hash_key->l4_chksum_zero);
		printf(" l4_chksum_zero: %u\n", hash_key->l4_chksum_zero);
}

/* 1. Validate and correct the hash key by hash mask
 * 2. Calculate the CRC value
 * 3. Set the hash_value
 */
int hash_value_calculate(aal_hash_key_t *hash_key, aal_hash_mask_t *hash_mask, aal_hash_value_t *hash_value)
{
	unsigned int masklen;
	aal_hash_key_t hash_key_bit_reverse;
	aal_hash_mask_t tmp_mask1, tmp_mask2;

	memset(&hash_key_bit_reverse, 0, sizeof(hash_key_bit_reverse));

	if ((hash_key == NULL) || (hash_mask == NULL) || (hash_value == NULL))
		return AAL_E_NULLPTR;

	/*** Hash Control ***/

	/* obsolete, hash_mask->table_id must be 1 */
	if (hash_mask->table_id) hash_key->table_id = 0;
	else return AAL_E_INVALID;

	if (hash_mask->ctrl_set_id) hash_key->ctrl_set_id = 0;

	if (hash_mask->hkey_id) {
		hash_key->hkey_id = 0;
	} else {
		/* Compare the hashmask that indexed by hkey_id in Hash or HashLite.
		 * One of them must be same as the parameter 'hash_mask'.
		 */

		aal_entry_get_by_idx(AAL_TABLE_HASH_MASK_TBL, hash_key->hkey_id, &tmp_mask1);
		aal_entry_get_by_idx(AAL_TABLE_HASHLITE_MASK_TBL, hash_key->hkey_id, &tmp_mask2);

		if ( (memcmp(hash_mask, &tmp_mask1, sizeof(tmp_mask1)) != 0) && (memcmp(hash_mask, &tmp_mask2, sizeof(tmp_mask2)) != 0) ) {
//			printf(KERN_WARNING "WARNING! The hash key 'hkey_id' is wrong, none hashmask in Hash and HashLite matches.\n");
			printf("WARNING! The hash key 'hkey_id' is wrong, none hashmask in Hash and HashLite matches.\n");
		}
	}

	/*** Source Port ID ***/

	if (hash_mask->lspid) hash_key->lspid = 0;
	if (hash_mask->orig_lspid) hash_key->orig_lspid = 0;
	if (hash_mask->mc_idx_vld) hash_key->mc_idx_vld = 0;

	/*** Dest Port ID ***/

	if (hash_mask->mc) hash_key->mc = 0;

	hash_key->mcgid &= ~(hash_mask->mcgid);

	/*** COS ***/

	if (hash_mask->cos) hash_key->cos = 0;

	/*** POL ID ***/

	if (hash_mask->pol_id) hash_key->pol_id = 0;

	if (hash_mask->pol_grp_id) hash_key->pol_grp_id = 0;

	if (hash_mask->qos_premark) hash_key->qos_premark = 0;

	/*** MDATA ***/

	hash_key->mdata &= ~(hash_mask->mdata);

	/*** Special Packet ***/

	if (hash_mask->spcl_pkt_enc) hash_key->spcl_pkt_enc = 0;

	hash_key->spcl_pkt_hdr_mtch &= ~(hash_mask->spcl_pkt_hdr_mtch);

	/*** L2 ***/

	if (hash_mask->mac_da & 0x1) hash_key->mac_da_0 = 0;
	if (hash_mask->mac_da & 0x2) hash_key->mac_da_1 = 0;
	if (hash_mask->mac_da & 0x4) hash_key->mac_da_2 = 0;
	if (hash_mask->mac_da & 0x8) hash_key->mac_da_3 = 0;
	if (hash_mask->mac_da & 0x10) hash_key->mac_da_4 = 0;
	if (hash_mask->mac_da & 0x20) hash_key->mac_da_5 = 0;

	if (hash_mask->mac_da_an_sel) hash_key->mac_da_an_sel = 0;
	if (hash_mask->mac_da_ip_mc) hash_key->mac_da_ip_mc = 0;
	if (hash_mask->mac_da_rng) hash_key->mac_da_rng = 0;
	if (hash_mask->mac_da_rsvd) hash_key->mac_da_rsvd = 0;

	if (hash_mask->mac_sa & 0x1) hash_key->mac_sa_0 = 0;
	if (hash_mask->mac_sa & 0x2) hash_key->mac_sa_1 = 0;
	if (hash_mask->mac_sa & 0x4) hash_key->mac_sa_2 = 0;
	if (hash_mask->mac_sa & 0x8) hash_key->mac_sa_3 = 0;
	if (hash_mask->mac_sa & 0x10) hash_key->mac_sa_4 = 0;
	if (hash_mask->mac_sa & 0x20) hash_key->mac_sa_5 = 0;

	if (hash_mask->ethertype) hash_key->ethertype = 0;
	if (hash_mask->ethertype_enc) hash_key->ethertype_enc = 0;

	/*** L2 Format ***/

	if (hash_mask->len_encoded) hash_key->len_encoded = 0;

	if (hash_mask->pktlen_rng_match_vec) hash_key->pktlen_rng_match_vec = 0;

	if (hash_mask->llc_snap) hash_key->llc_snap = 0;

	if (hash_mask->llc_type_enc) hash_key->llc_type_enc = 0;

	/*** VLAN ***/

	if (hash_mask->vlan_cnt) hash_key->vlan_cnt = 0;

	if (hash_mask->top_tpid_enc) hash_key->top_tpid_enc = 0;
	if (hash_mask->top_vid) hash_key->top_vid = 0;
	if (hash_mask->top_8021p) hash_key->top_8021p = 0;
	if (hash_mask->top_dei) hash_key->top_dei = 0;

	if (hash_mask->inner_tpid_enc) hash_key->inner_tpid_enc = 0;
	if (hash_mask->inner_vid) hash_key->inner_vid = 0;
	if (hash_mask->inner_8021p) hash_key->inner_8021p = 0;
	if (hash_mask->inner_dei) hash_key->inner_dei = 0;

	/*** PPPoE ***/

	if (hash_mask->pppoe_type) hash_key->pppoe_type = 0;
	if (hash_mask->pppoe_code_enc) hash_key->pppoe_code_enc = 0;
	if (hash_mask->pppoe_session_id) hash_key->pppoe_session_id = 0;
	if (hash_mask->ppp_protocol_enc) hash_key->ppp_protocol_enc = 0;

	/*** L3 ***/

	if (hash_mask->ip_vld) hash_key->ip_vld = 0;
	if (hash_mask->ip_ver) hash_key->ip_ver = 0;

	hash_key->ip_dscp &= ~(hash_mask->ip_dscp);
	hash_key->ip_ecn &= ~(hash_mask->ip_ecn);

	if (hash_mask->ip_protocol) hash_key->ip_protocol = 0;
	if (hash_mask->ip_l4_type) hash_key->ip_l4_type = 0;

	masklen = hash_mask->ip_sa & 0xFF;
	if (masklen > 128)
		masklen = 128;
	if ((masklen > 32) && (hash_mask->ip_ver == 0) && (hash_key->ip_ver == 0))
		masklen = 32;
	if ((hash_mask->ip_sa & 0x100) == 0) {
		/* hash_mask->ip_sa[7:0] is prefix len */
		if ((hash_mask->ip_ver == 0) && (hash_key->ip_ver == 0)) {
			/* IPv4 */
			hash_key->ip_sa_0 &= ~(0xFFFFFFFFULL >> masklen);
			hash_key->ip_sa_1 = 0;
			hash_key->ip_sa_2 = 0;
			hash_key->ip_sa_3 = 0;
		} else {
			/* IPv6 or unknown IP version (hash_mask->ip_ver is unmasked) */
			if (masklen > 96)
				hash_key->ip_sa_0 &= ~(0xFFFFFFFFULL >> (masklen - 96));
			else
				hash_key->ip_sa_0 = 0;

			if ((masklen > 64) && (masklen <= 96))
				hash_key->ip_sa_1 &= ~(0xFFFFFFFFULL >> (masklen - 64));
			else if (masklen <= 64)
				hash_key->ip_sa_1 = 0;

			if ((masklen > 32) && (masklen <= 64))
				hash_key->ip_sa_2 &= ~(0xFFFFFFFFULL >> (masklen - 32));
			else if (masklen <= 32) {
				hash_key->ip_sa_2 = 0;
				hash_key->ip_sa_3 &= ~(0xFFFFFFFFULL >> masklen);
			}
		}
	}
	if ((hash_mask->ip_sa & 0x100) == 1) {
		/* hash_mask->ip_sa[7:0] is suffix len */
		if (masklen > 96)
			hash_key->ip_sa_3 &= ~(0xFFFFFFFFULL << (masklen - 96));
		else
			hash_key->ip_sa_3 = 0;

		if ((masklen > 64) && (masklen <= 96))
			hash_key->ip_sa_2 &= ~(0xFFFFFFFFULL << (masklen - 64));
		else if (masklen <= 64)
			hash_key->ip_sa_2 = 0;

		if ((masklen > 32) && (masklen <= 64))
			hash_key->ip_sa_1 &= ~(0xFFFFFFFFULL << (masklen - 32));
		else if (masklen <= 32) {
			hash_key->ip_sa_1 = 0;
			hash_key->ip_sa_0 &= ~(0xFFFFFFFFULL << masklen);
		}
	}

	masklen = hash_mask->ip_da & 0xFF;
	if (masklen > 128)
		masklen = 128;
	if ((masklen > 32) && (hash_mask->ip_ver == 0) && (hash_key->ip_ver == 0))
		masklen = 32;
	if ((hash_mask->ip_da & 0x100) == 0) {
		/* hash_mask->ip_da[7:0] is prefix len */
		if ((hash_mask->ip_ver == 0) && (hash_key->ip_ver == 0)) {
			/* IPv4 */
			hash_key->ip_da_0 &= (~(0xFFFFFFFFULL >> masklen));
			hash_key->ip_da_1 = 0;
			hash_key->ip_da_2 = 0;
			hash_key->ip_da_3 = 0;
		} else {
			/* IPv6 or unknown IP version (hash_mask->ip_ver is unmasked) */
			if (masklen > 96)
				hash_key->ip_da_0 &= ~(0xFFFFFFFFULL >> (masklen - 96));
			else
				hash_key->ip_da_0 = 0;

			if ((masklen > 64) && (masklen <= 96))
				hash_key->ip_da_1 &= ~(0xFFFFFFFFULL >> (masklen - 64));
			else if (masklen <= 64)
				hash_key->ip_da_1 = 0;

			if ((masklen > 32) && (masklen <= 64))
				hash_key->ip_da_2 &= ~(0xFFFFFFFFULL >> (masklen - 32));
			else if (masklen <= 32) {
				hash_key->ip_da_2 = 0;
				hash_key->ip_da_3 &= ~(0xFFFFFFFFULL >> masklen);
			}
		}
	}
	if ((hash_mask->ip_da & 0x100) == 1) {
		/* hash_mask->ip_da[7:0] is suffix len */
		if (masklen > 96)
			hash_key->ip_da_3 &= ~(0xFFFFFFFFULL << (masklen - 96));
		else
			hash_key->ip_da_3 = 0;

		if ((masklen > 64) && (masklen <= 96))
			hash_key->ip_da_2 &= ~(0xFFFFFFFFULL << (masklen - 64));
		else if (masklen <= 64)
			hash_key->ip_da_2 = 0;

		if ((masklen > 32) && (masklen <= 64))
			hash_key->ip_da_1 &= ~(0xFFFFFFFFULL << (masklen - 32));
		else if (masklen <= 32) {
			hash_key->ip_da_1 = 0;
			hash_key->ip_da_0 &= ~(0xFFFFFFFFULL << masklen);
		}
	}

	if (hash_mask->ipv6_flow_lbl) hash_key->ipv6_flow_lbl = 0;

	switch (hash_mask->ip_ttl) {
	case 0:
	case 1:
		hash_key->ip_ttl = 0;
		break;

	case 2:
		/* keep original value */
		break;

	case 3:
		if (hash_key->ip_ttl > 10) return AAL_E_OUTRANGE;
		break;
	}

	if (hash_mask->ip_options) hash_key->ip_options = 0;

	if (hash_mask->ip_fragment_flag) hash_key->ip_fragment_flag = 0;

	if (hash_mask->ipv6_hbh) hash_key->ipv6_hbh = 0;
	if (hash_mask->ipv6_rh) hash_key->ipv6_rh = 0;
	if (hash_mask->ipv6_doh) hash_key->ipv6_doh = 0;

	/* Four combinations of (MASK_icmp_vld, MASK_icmp_type):
	 *   (1, 1): Don't care; set (KEY_icmp_vld, KEY_icmp_type) to (0, 0).
	 *   (1, 0): User forgot to set MASK_icmp_vld. Set KEY_icmp_vld to 0.
	 *   (0, 1): Just check ICMP valid or invalid. Set KEY_icmp_type to 0.
	 *   (0, 0): ICMP must be valid and match the type. It is invalid case while ICMP field is invalid but ICMP type is assigned.
	 */
	if (hash_mask->icmp_vld) hash_key->icmp_vld = 0;
	if (hash_mask->icmp_type) hash_key->icmp_type = 0;
	if ((hash_mask->icmp_type == 0) && (hash_mask->icmp_vld == 0) && (hash_key->icmp_vld == 0)) return AAL_E_INVALID;

	/* It is invalid case while SPI field is invalid but SPI value is assigned. */
	if (hash_mask->spi_vld) hash_key->spi_vld = 0;
	if (hash_mask->spi) hash_key->spi = 0;
	if ((hash_mask->spi == 0) && (hash_mask->spi_vld == 0) && (hash_key->spi_vld == 0)) return AAL_E_INVALID;

	if (hash_mask->l3_chksum_err) hash_key->l3_chksum_err = 0;

	/*** L4 ***/

	hash_key->l4_dp_exact_range &= ~(hash_mask->l4_dp_exact_range);
	hash_key->l4_sp_exact_range &= ~(hash_mask->l4_sp_exact_range);

	hash_key->tcp_rdp_ctrl &= ~(hash_mask->tcp_rdp_ctrl);

	if (hash_mask->l4_chksum_zero) hash_key->l4_chksum_zero = 0;

	if (ca_aal_debug & AAL_DBG_HASH || ca_aal_debug & AAL_DBG_HASHLITE) {
//		printf(KERN_INFO "********************* Start Hash Key ***********************\n");
		printf("********************* Start Hash Key ***********************\n");
		hash_key_dump(hash_key);
//		printf(KERN_INFO "********************* End Hash Key ***********************\n");
		printf("********************* End Hash Key ***********************\n");
	}

	/*** Calculate the CRC value */
	memcpy(&hash_key_bit_reverse, hash_key, sizeof(aal_hash_key_t));
	bit_reverse((unsigned int *) &hash_key_bit_reverse, sizeof(aal_hash_key_t));
	hash_value->crc32 = _ca_crc32((unsigned char *) &hash_key_bit_reverse, sizeof(aal_hash_key_t));
	hash_value->crc16 = _ca_crc16((unsigned char *) &hash_key_bit_reverse, sizeof(aal_hash_key_t));
	hash_value->valid = 1;

	if (ca_aal_debug & AAL_DBG_HASH || ca_aal_debug & AAL_DBG_HASHLITE) {
//		printf(KERN_INFO "CRC32=0x%08x, CRC16=0x%04x\n", hash_value->crc32, hash_value->crc16);
		printf("CRC32=0x%08x, CRC16=0x%04x\n", hash_value->crc32, hash_value->crc16);
	}

	return AAL_E_OK;
}

/***************************************************************************
 * scfg related
 ***************************************************************************/
ca_status_t aal_scfg_read(ca_uint8_t *id, int len, void *value)
{
	int i;
	ca_status_t ret;

	ret = scfg_read(0, id, len, value);
	if (ret != (int) SCFG_OK) {
		printf("%s: Fail to read scfg id %s. (ret=%d)\n", __func__, id, ret);
		return CA_E_ERROR;
	}

	if (ca_aal_debug) {
		if (len == 1) {
			printf("%s: %s=%u\n", __func__, id, *(ca_uint8_t *)value);
		}
		if (len == 4) {
			printf("%s: %s=%u\n", __func__, id, *(ca_uint32_t *)value);
		}
		if (len > 4) {
			printf("%s: %s=", __func__, id);
			for (i = 0; i < len; i++) {
				printf("%u ", *((ca_uint8_t *)value+i));
			}
			printf("\n");
		}
	}

	return CA_E_OK;
}

int aal_hw_info_get(aal_info_t *info)
{
	L2FE_GLB_VER_CTRL_t cfg;

	if (info == NULL)
		return AAL_E_NULLPTR;

	cfg.wrd = CA_L2FE_REG_READ(L2FE_GLB_VER_CTRL);

	info->hardware_id = (ca_uint32_t)(cfg.wrd & 0x0000FFFF);
	info->hardware_rev = (ca_uint32_t)((cfg.wrd >> 16) & 0x0000FFFF);
	/* FIXME: While G3 tapeout need to modify.
	   Chip release date. For example, hardware_data will be
	   0x161220 if the release date is Dec. 20, 2016*/
	info->hardware_date = 0x161220;

	return AAL_E_OK;
}

/***************************************************************************
 * FPGA only
 ***************************************************************************/

#ifdef CONFIG_CORTINA_BOARD_FPGA
bool aal_soc_has_module(unsigned int module)
{
#if defined(CONFIG_ARCH_CORTINA_G3) || defined(CONFIG_ARCH_CORTINA_G3HGU)
	GLOBAL_FPGA_OPTION_t  fpga_opt;

	fpga_opt.wrd = CA_GLB_NE_INTR_REG_READ(GLOBAL_FPGA_OPTION);
	return ((fpga_opt.wrd & module) != 0);
#else
	return true;
#endif
}
#else /* ! CONFIG_CORTINA_BOARD_FPGA */
bool aal_soc_has_module(unsigned int module)
{
	return true;
}
#endif /* CONFIG_CORTINA_BOARD_FPGA */

/***************************************************************************
 * base address
 ***************************************************************************/

void *g3_ne_reg_iobase = NULL;
#if defined(CONFIG_ARCH_CORTINA_G3HGU)
void *hgu_8279_ne_reg_iobase = NULL;
#endif
void *g3_glb_ne_intr_reg_iobase = NULL;
void *g3_dma_reg_iobase = NULL;
void *g3_glb_reg_iobase = NULL;
void *g3_xram_reg_iobase = NULL;
void *g3_per_mdio_reg_iobase = NULL;

void aal_ne_reg_base_init(void *iobase)
{
	g3_ne_reg_iobase = iobase;
}

#ifndef CONFIG_CORTINA_BOARD_FPGA
void *g3_ne_phy_reg_iobase = NULL;
void aal_ne_phy_reg_base_init(void *iobase)
{
        g3_ne_phy_reg_iobase = iobase;
}
#endif

#if defined(CONFIG_CA_NE_AAL_XFI)
void *g3_ne_xfi_reg_iobase = NULL;
void aal_ne_xfi_reg_base_init(void *iobase)
{
        g3_ne_xfi_reg_iobase = iobase;
}
#endif

void aal_dma_reg_base_init(void *iobase)
{
	g3_dma_reg_iobase = iobase;
}

void aal_glb_reg_base_init(void *iobase)
{
	g3_glb_reg_iobase = iobase;
}

void aal_xram_reg_base_init(void *iobase)
{
        g3_xram_reg_iobase = iobase;
}

void aal_per_mdio_reg_base_init(void *iobase)
{
	g3_per_mdio_reg_iobase = iobase;
}

void aal_glb_ne_intr_reg_base_init(void *iobase)
{
	g3_glb_ne_intr_reg_iobase = iobase;
}

