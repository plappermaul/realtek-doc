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
#ifndef __CA_ETHTOOL_H__
#define __CA_ETHTOOL_H__

#if 0//yocto
#else//sd1 uboot for 98f
#define ETH_GSTRING_LEN		32
#endif

#define CA_REGDUMP_LEN	(4 * 312)

typedef enum {
	RXUCPKTCNT = 0,
	RXMCFRMCNT,
	RXBCFRMCNT,
	RXOAMFRMCNT,
	RXJUMBOFRMCNT,
	RXPAUSEFRMCNT,
	RXUNKNOWNOCFRMCNT,
	RXCRCERRFRMCNT,
	RXUNDERSIZEFRMCNT,
	RXRUNTFRMCNT,
	RXOVSIZEFRMCNT,
	RXJABBERFRMCNT,
	RXINVALIDFRMCNT,
	RXSTATSFRM64OCT,
	RXSTATSFRM65TO127OCT,
	RXSTATSFRM128TO255OCT,
	RXSTATSFRM256TO511OCT,
	RXSTATSFRM512TO1023OCT,
	RXSTATSFRM1024TO1518OCT,
	RXSTATSFRM1519TO2100OCT,
	RXSTATSFRM2101TO9200OCT,
	RXSTATSFRM9201TOMAXOCT,
	RXBYTECOUNT_LO,
	RXBYTECOUNT_HI,
} ni_rxmib_id_t;

typedef enum {
	TXUCPKTCNT = 0,
	TXMCFRMCNT,
	TXBCFRMCNT,
	TXOAMFRMCNT,
	TXJUMBOFRMCNT,
	TXPAUSEFRMCNT,
	TXCRCERRFRMCNT,
	TXOVSIZEFRMCNT,
	TXSINGLECOLFRM,
	TXMULTICOLFRM,
	TXLATECOLFRM,
	TXEXESSCOLFRM,
	/* counter_id 0x0C is not defined. */
	TXSTATSFRM64OCT = 0x0D,
	TXSTATSFRM65TO127OCT,
	TXSTATSFRM128TO255OCT,
	TXSTATSFRM256TO511OCT,
	TXSTATSFRM512TO1023OCT,
	TXSTATSFRM1024TO1518OCT,
	TXSTATSFRM1519TO2100OCT,
	TXSTATSFRM2101TO9200OCT,
	TXSTATSFRM9201TOMAXOCT,
	TXBYTECOUNT_LO,
	TXBYTECOUNT_HI,
} ni_txmib_id_t;

/* Statistics counters collected by the MAC */
struct ca_ethtool_stats {
	u64 rxucpktcnt;
	u64 rxmcfrmcnt;
	u64 rxbcfrmcnt;
	u64 rxoamfrmcnt;
	u64 rxjumbofrmcnt;
	u64 rxpausefrmcnt;
	u64 rxunknownocfrmcnt;
	u64 rxcrcerrfrmcnt;
	u64 rxundersizefrmcnt;
	u64 rxruntfrmcnt;
	u64 rxovsizefrmcnt;
	u64 rxjabberfrmcnt;
	u64 rxinvalidfrmcnt;
	u64 rxstatsfrm64oct;
	u64 rxstatsfrm65to127oct;
	u64 rxstatsfrm128to255oct;
	u64 rxstatsfrm256to511oct;
	u64 rxstatsfrm512to1023oct;
	u64 rxstatsfrm1024to1518oct;
	u64 rxstatsfrm1519to2100oct;
	u64 rxstatsfrm2101to9200oct;
	u64 rxstatsfrm9201tomaxoct;
	u64 rxbytecount_lo;
	u64 rxbytecount_hi;
	u64 txucpktcnt;
	u64 txmcfrmcnt;
	u64 txbcfrmcnt;
	u64 txoamfrmcnt;
	u64 txjumbofrmcnt;
	u64 txpausefrmcnt;
	u64 txcrcerrfrmcnt;
	u64 txovsizefrmcnt;
	u64 txsinglecolfrm;
	u64 txmulticolfrm;
	u64 txlatecolfrm;
	u64 txexesscolfrm;
	u64 txstatsfrm64oct;
	u64 txstatsfrm65to127oct;
	u64 txstatsfrm128to255oct;
	u64 txstatsfrm256to511oct;
	u64 txstatsfrm512to1023oct;
	u64 txstatsfrm1024to1518oct;
	u64 txstatsfrm1519to2100oct;
	u64 txstatsfrm2101to9200oct;
	u64 txstatsfrm9201tomaxoct;
	u64 txbytecount_lo;
	u64 txbytecount_hi;
};

/* number of ETHTOOL_GSTATS u64's */
#define CA_NUM_STATS		(sizeof(struct ca_ethtool_stats)/sizeof(u64))

static const struct {
	const char string[ETH_GSTRING_LEN];
} ethtool_stats_keys[CA_NUM_STATS] = {
	{ "RxUCPktCnt" },
	{ "RxMCFrmCnt" },
	{ "RxBCFrmCnt" },
	{ "RxOAMFrmCnt" },
	{ "RxJumboFrmCnt" },
	{ "RxPauseFrmCnt" },
	{ "RxUnKnownOCFrmCnt" },
	{ "RxCrcErrFrmCnt" },
	{ "RxUndersizeFrmCnt" },
	{ "RxRuntFrmCnt" },
	{ "RxOvSizeFrmCnt" },
	{ "RxJabberFrmCnt" },
	{ "RxInvalidFrmCnt" },
	{ "RxStatsFrm64oct" },
	{ "RxStatsFrm65to127oct" },
	{ "RxStatsFrm128to255oct" },
	{ "RxStatsFrm256to511oct" },
	{ "RxStatsFrm512to1023oct" },
	{ "RxStatsFrm1024to1518oct" },
	{ "RxStatsFrm1519to2100oct" },
	{ "RxStatsFrm2101to9200oct" },
	{ "RxStatsFrm9201toMaxoct" },
	{ "RxByteCount_Lo" },
	{ "RxByteCount_Hi" },
	{ "TxUCPktCnt" },
	{ "TxMCFrmCnt" },
	{ "TxBCFrmCnt" },
	{ "TxOAMFrmCnt" },
	{ "TxJumboFrmCnt" },
	{ "TxPauseFrmCnt" },
	{ "TxCrcErrFrmCnt" },
	{ "TxOvSizeFrmCnt" },
	{ "TxSingleColFrm" },
	{ "TxMultiColFrm" },
	{ "TxLateColFrm" },
	{ "TxExessColFrm" },
	{ "TxStatsFrm64oct" },
	{ "TxStatsFrm65to127oct" },
	{ "TxStatsFrm128to255oct" },
	{ "TxStatsFrm256to511oct" },
	{ "TxStatsFrm512to1023oct" },
	{ "TxStatsFrm1024to1518oct" },
	{ "TxStatsFrm1519to2100oct" },
	{ "TxStatsFrm2101to9200oct" },
	{ "TxStatsFrm9201toMaxoct" },
	{ "TxByteCount_Lo" },
	{ "TxByteCount_Hi" }
};

void ca_ni_set_ethtool_ops(struct net_device *dev);

#endif /* __CA_ETHTOOL_H__ */

