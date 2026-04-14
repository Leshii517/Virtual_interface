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

    struct ethhdr *eth = eth_hdr(skb);

    

}


static int vping_open(struct net_device *dev){


}


static int vping_close(struct net_device *dev){


}


static int __init vping_init(void){





}

module_init(vping_init);

