#include <linux/module.h>
#include <linux/netlink.h>
#include <net/netlink.h>
#include <net/net_namespace.h>
#include "netlink_kern.h"
#include "../device/dev_kern.h"

#define NETLINK_USER 31

MODULE_LICENSE("GPL");
MODULE_AUTHOR("KaliGo");
MODULE_VERSION("0.1");


static struct sock *nl_sk = NULL;
int pid = -1;
static void nl_recv_msg(struct sk_buff *skb) {
    struct nlmsghdr *nlh;
    struct sk_buff *skb_out;
    char *msg;
    char *reply;
    int msg_size, reply_size;
    int res;

    nlh = (struct nlmsghdr *) skb->data;
    pid = nlh->nlmsg_pid; 
    msg = (char *)nlmsg_data(nlh);
    if (strcmp(msg, "Hello") == 0) {
        reply = "Hello from kernel";
    } else {
        reply = "No such functionality";
    }

    reply_size = strlen(reply);
    skb_out = nlmsg_new(reply_size, 0);
    if (!skb_out) {
        printk(KERN_ERR "Failed to allocate new skb\n");
        return;
    }

    nlh = nlmsg_put(skb_out, 0, 0, NLMSG_DONE, reply_size, 0);
    strncpy(nlmsg_data(nlh), reply, reply_size);

    res = nlmsg_unicast(nl_sk, skb_out, pid);
    if (res < 0)
        printk(KERN_INFO "Error while sending back to user\n");
}

int nl_send_msg_to_user(char *msg) {
    struct sk_buff *skb_out;
    struct nlmsghdr *nlh;
    int msg_size = strlen(msg);
    int res;

    skb_out = nlmsg_new(msg_size, 0); 
    if (!skb_out) {
        printk(KERN_ERR "Failed to allocate new skb\n");
        return -ENOMEM;
    }

    nlh = nlmsg_put(skb_out, 0, 0, NLMSG_DONE, msg_size, 0);
    strncpy(nlmsg_data(nlh), msg, msg_size);

    res = nlmsg_unicast(nl_sk, skb_out, pid);
    if (res < 0) {
        printk(KERN_ERR "Error while sending to user: %d\n", res);
        kfree_skb(skb_out);
        return res;
    }

    return 0;
}

void netlink_init(void) {
    struct netlink_kernel_cfg cfg = {
        .input = nl_recv_msg,
    };
    nl_sk = netlink_kernel_create(&init_net,NETLINK_USER,&cfg);
    if (!nl_sk)
    {
        printk(KERN_ALERT "Error creating socket.\n");
        return;
    }

    printk(KERN_INFO "BeginningVM Netlink module started.\n");
    
}

void netlink_exit(void) {
    printk(KERN_INFO "BeginningVM Netlink module exited\n");
    netlink_kernel_release(nl_sk);
}
EXPORT_SYMBOL(netlink_init);
EXPORT_SYMBOL(netlink_exit);
EXPORT_SYMBOL(nl_send_msg_to_user);