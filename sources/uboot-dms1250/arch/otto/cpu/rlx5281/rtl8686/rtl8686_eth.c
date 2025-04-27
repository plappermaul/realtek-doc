
#include <config.h>
#include <common.h>
#include <malloc.h>
#include <net.h>
#include <command.h>
#include <asm/arch/bspchip.h>
#include "re8670poll.h"

static int rtl8676_init(struct eth_device* dev, bd_t * bd)
{
	//u8 *m = dev->enetaddr;
	
	//printf("%s(%d): %02x:%02x:%02x:%02x:%02x:%02x\n",__FUNCTION__,__LINE__, m[0],m[1],m[2],m[3],m[4],m[5]);
	//REG32(SIRR) |= TRXRDY;
	Lan_RXENABLE();
	return 0;
}

static int rtl8676_send(struct eth_device* dev, volatile void *packet, int length)
{	
	Lan_Transmit( packet, length);
	return 0;	
}

static int rtl8676_recv(struct eth_device* dev) 
{
	u8 *pData;
	uint Len;
	int ret;
	
	ret = Lan_Receive(&pData, &Len);	
	if (0==ret)  {
		NetReceive(pData, Len);		
	}
	
	return 0;
}

static void rtl8676_halt(struct eth_device* dev)
{
	//REG32(SIRR) &= ~TRXRDY;
	Lan_RXDISABLE();
}

#if 1
static int rtl8676_write_hwaddr(struct eth_device *netdev)
{
	u8 *m = netdev->enetaddr;
	//printf("Mac: %02x:%02x:%02x:%02x:%02x:%02x\n", m[0],m[1],m[2],m[3],m[4],m[5]);
	//swNic_netifSetMac(netdev->enetaddr);
	Lan_WriteMac(m);
	return 0;
}
#endif

static int rtl8676_eth_init(bd_t *bis) 
{
	struct eth_device* dev;
	u8 mac[6] = { 0x00, 0x05, 0x1d, 0x01, 0x01, 0x01 };	
	
	if ((dev = (struct eth_device*)malloc(sizeof *dev)) == NULL) {
		puts ("malloc failed\n");
		return -1;
	}

	memset(dev, 0, sizeof *dev);
	sprintf(dev->name, "RTL8686 GMAC ");
	dev->iobase = 0;
	dev->priv   = 0;
	dev->init   = rtl8676_init;
	dev->send   = rtl8676_send;
	dev->recv   = rtl8676_recv;
	dev->halt	= rtl8676_halt;
	dev->write_hwaddr = rtl8676_write_hwaddr;
	
	eth_register(dev);
	
	//swInit(mac);
	Lan_Initialize(NULL);
	
	return 1;
}

int cpu_eth_init(bd_t *bis)
{
	return rtl8676_eth_init(bis);
}
