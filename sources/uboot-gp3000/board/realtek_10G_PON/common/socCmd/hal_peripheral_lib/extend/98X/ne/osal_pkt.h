#ifndef _OSAL_PKT_H_
#define _OSAL_PKT_H_

#define pkt_t 		struct sk_buff
#define PKTFREE(p)	dev_kfree_skb_any(p)
#define PKTDATA(p)	p->data
#define PKTLEN(p)	p->len

#endif

