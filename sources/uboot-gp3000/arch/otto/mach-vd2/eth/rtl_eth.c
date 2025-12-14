#include <config.h>
#include <common.h>
#include <malloc.h>
#include <net.h>
#include <command.h>
#include "asicregs.h"
#include "swCore.h"
#include "swNic_poll.h"

static int rtl8676_init(struct eth_device* dev, bd_t * bd)
{
    //u8 mac[6] = { 0x00, 0x05, 0x1d, 0x01, 0x01, 0x01 };
    //u8 *m = dev->enetaddr;

    //printf("%s(%d): %02x:%02x:%02x:%02x:%02x:%02x\n",__FUNCTION__,__LINE__, m[0],m[1],m[2],m[3],m[4],m[5]);
    //swInit(dev->enetaddr);
    //REG32(SIRR) |= TRXRDY;

    REG32(PCRP0) |= EnablePHYIf;
    REG32(PCRP1) |= EnablePHYIf;
    REG32(PCRP2) |= EnablePHYIf;
    REG32(PCRP3) |= EnablePHYIf;
    REG32(PCRP4) |= EnablePHYIf;

    mdelay(1);

    /* Start normal TX and RX */
    REG32(SIRR) |= TRXRDY;

    //puts("Enalbe PHY and TRXRDY\n");

    return 0;
}

static int rtl8676_send(struct eth_device* dev, void *packet, int length)
{
    swNic_send((void *)packet, length);
    swNic_txDone();
    return 0;
}

static int rtl8676_recv(struct eth_device* dev)
{
    u8 *pData;
    uint Len;
    int ret;

    ret = swNic_receive((void **)&pData, &Len);
    if (0==ret)  {
        net_process_received_packet(pData, Len);
    }

    return 0;
}

static void rtl8676_halt(struct eth_device* dev)
{
    //REG32(SIRR) &= ~TRXRDY;

    REG32(PCRP0) &= ~EnablePHYIf;
    REG32(PCRP1) &= ~EnablePHYIf;
    REG32(PCRP2) &= ~EnablePHYIf;
    REG32(PCRP3) &= ~EnablePHYIf;
    REG32(PCRP4) &= ~EnablePHYIf;

    //puts("Disable PHY\n");
}

static int rtl8676_write_hwaddr(struct eth_device *netdev)
{
    u8 *m = netdev->enetaddr;
    printf("Mac: %02x:%02x:%02x:%02x:%02x:%02x\n", m[0],m[1],m[2],m[3],m[4],m[5]);
    swNic_netifSetMac(netdev->enetaddr);
    return 0;
}

static int rtl8676_eth_init(bd_t *bis)
{
    struct eth_device* dev;
    char mac[6] = { 0x00, 0x05, 0x1d, 0x01, 0x01, 0x01 };

    if ((dev = (struct eth_device*)malloc(sizeof *dev)) == NULL) {
        puts ("malloc failed\n");
        return -1;
    }

    memset(dev, 0, sizeof *dev);
    sprintf(dev->name, "RTL SwCore");
    dev->iobase = 0;
    dev->priv   = 0;
    dev->init   = rtl8676_init;
    dev->send   = rtl8676_send;
    dev->recv   = rtl8676_recv;
    dev->halt	= rtl8676_halt;
    dev->write_hwaddr = rtl8676_write_hwaddr;

    eth_register(dev);

    swInit(mac);

    return 1;
}

int cpu_eth_init(bd_t *bis)
{
    return rtl8676_eth_init(bis);
}
