#ifndef NETLINK_KERN_H
#define NETLINK_KERN_H

void netlink_init(void);
void netlink_exit(void);
int nl_send_msg_to_user(char *msg);
#endif