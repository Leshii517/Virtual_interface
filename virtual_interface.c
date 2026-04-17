#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/skbudd.h>
#include <linux/proc_fs.h>
#include <linux/inet.h>
#include <linux/ip.h>
#include <linux/icmp.h>
#include <linux/if_arp.h>


static struct net_device *virtnet_dev;
static struct proc_dir_entry *proc_dir;
static struct proc_dir_entry *proc_ip_file;


static __be32 virtual_ip = 0; // переменная для определения ip 


// функция чтения из /proc
static ssize_t virtual_ip_read(struct file *file, char __user *buf, size_t count, loff_t *pos){
    char buf[32];
    int length_ip;

    length_ip = sprintf(buf, sizeof(tmp), "%pI4\n", &virtual_ip);

    *pos = length_ip;

    return length_ip;

}

static ssize_t virtual_ip_write(struct file *file, char __user *buf, size_t count, loff_t *pos){

    char buf[16];
    __be32 new_ip;

    buf[count] = '\0';

    virtual_ip = new_ip;

    printk(KERNEL_INFO "IP изменен на %pI4\n", &virtual_ip);

    return count;


}

static netdev_tx_t vping_push_pull_packege(struct sk_buff * skb,struct net_device *dev){

{
    char tmp[16];
    __be32 new_ip;

    if (count > 15)
        count = 15;

    if (copy_from_user(tmp, buf, count))
        return -EFAULT;

    tmp[count] = '\0';

    if (in4_pton(tmp, -1, (u8 *)&new_ip, '\0', NULL) == 0)
        return -EINVAL;

    vping_ip = new_ip;
    printk(KERN_INFO "vping: IP изменён на %pI4\n", &vping_ip);

    return count;
}

static const struct proc_ops vping_ip_fops = {
    .proc_read  = vping_ip_read,
    .proc_write = vping_ip_write,
};

static netdev_tx_t vping_xmit(struct sk_buff *skb, struct net_device *dev)
{
    struct ethhdr *eth = eth_hdr(skb);

    dev->stats.tx_packets++;
    dev->stats.tx_bytes += skb->len;

    if (vping_ip == 0) {
        /* IP ещё не задан */
        goto drop;
    }

    /* === ARP === */
    if (skb->protocol == htons(ETH_P_ARP)) {
        struct arphdr *arp = arp_hdr(skb);
        unsigned char *arp_ptr = (unsigned char *)(arp + 1);
        __be32 *sip = (__be32 *)(arp_ptr + 2 * ETH_ALEN);
        __be32 *tip = (__be32 *)(arp_ptr + 2 * ETH_ALEN + 4);

        if (arp->ar_op == htons(ARPOP_REQUEST) && *tip == vping_ip) {
            /* Строим ARP Reply */
            arp->ar_op = htons(ARPOP_REPLY);

            /* MAC */
            memcpy(arp_ptr + ETH_ALEN, arp_ptr, ETH_ALEN);           /* target MAC = sender MAC */
            memcpy(arp_ptr, dev->dev_addr, ETH_ALEN);                /* sender MAC = наш MAC */

            /* IP */
            *tip = *sip;
            *sip = vping_ip;

            /* Ethernet */
            memcpy(eth->h_dest, eth->h_source, ETH_ALEN);
            memcpy(eth->h_source, dev->dev_addr, ETH_ALEN);

            goto rx_back;
        }
    }

    /* === ICMP Echo Request === */
    else if (skb->protocol == htons(ETH_P_IP)) {
        struct iphdr *iph = ip_hdr(skb);
        if (iph->protocol == IPPROTO_ICMP) {
            struct icmphdr *icmph = icmp_hdr(skb);

            if (icmph->type == ICMP_ECHO && iph->daddr == vping_ip) {
                /* Строим Echo Reply */
                __be32 tmp = iph->saddr;
                iph->saddr = iph->daddr;
                iph->daddr = tmp;

                icmph->type = ICMP_ECHOREPLY;
                icmph->checksum = 0;
                icmph->checksum = ip_compute_csum((unsigned char *)icmph,
                                                  skb->len - (iph->ihl << 2));

                /* Ethernet */
                memcpy(eth->h_dest, eth->h_source, ETH_ALEN);
                memcpy(eth->h_source, dev->dev_addr, ETH_ALEN);

                goto rx_back;
            }
        }
    }

drop:
    dev_kfree_skb(skb);
    return NETDEV_TX_OK;

rx_back:
    /* Возвращаем пакет в стек как входящий */
    skb->pkt_type = PACKET_HOST;
    skb->ip_summed = CHECKSUM_UNNECESSARY;

    dev->stats.rx_packets++;
    dev->stats.rx_bytes += skb->len;

    netif_rx(skb);
    return NETDEV_TX_OK;
}


static int vping_open(struct net_device *dev){

    netinf_start_queue( net_device );
    return 0;

}


static int vping_close(struct net_device *dev){

    netinf_stop_queue(net_device);
    return 0;

}

static const struct net_device_ops vping_netdev_ops = {
    .ndo_open       = vping_open,
    .ndo_stop       = vping_stop,
    .ndo_start_xmit = vping_xmit,
};

static int __init vping_init(void)
{
    vping_dev = alloc_etherdev(0);
    if (!vping_dev)
        return -ENOMEM;

    vping_dev->netdev_ops = &vping_netdev_ops;
    strcpy(vping_dev->name, "vping%d");

    eth_hw_addr_random(vping_dev);

    if (register_netdev(vping_dev)) {
        free_netdev(vping_dev);
        return -EIO;
    }

    /* procfs */
    proc_dir = proc_mkdir("vping", NULL);
    if (proc_dir)
        proc_ip_file = proc_create("ip", 0644, proc_dir, &vping_ip_fops);



    return 0;
}

static void __exit vping_exit(void)
{
    if (proc_ip_file)
        remove_proc_entry("ip", proc_dir);
    if (proc_dir)
        remove_proc_entry("vping", NULL);

    unregister_netdev(vping_dev);
    free_netdev(vping_dev);
}

module_init(vping_init);

