#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <linux/netlink.h>
#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#define USERSPACE 1
#include "../module/BeginningVMOS.h"

#define NETLINK_USER 31
#define MAX_PAYLOAD 1024

int netlink_ready = 0;

// 线程函数声明
void *netlink_thread_func(void *arg);
void *device_thread_func(void *arg);

static int BeginningVM_dev_fd;

void *netlink_thread_func(void *arg) {
  int ret;
  struct sockaddr_nl src_addr, dest_addr;
  struct nlmsghdr *nlh = NULL;
  struct iovec iov;
  int sock_fd;
  struct msghdr msg;

  sock_fd = socket(PF_NETLINK, SOCK_RAW, NETLINK_USER);
  if (sock_fd < 0)
    return -1;

  memset(&src_addr, 0, sizeof(src_addr));
  src_addr.nl_family = AF_NETLINK;
  src_addr.nl_pid = getpid();

  bind(sock_fd, (struct sockaddr *)&src_addr, sizeof(src_addr));

  memset(&dest_addr, 0, sizeof(dest_addr));
  dest_addr.nl_family = AF_NETLINK;
  dest_addr.nl_pid = 0;   
  dest_addr.nl_groups = 0; 

  nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PAYLOAD));
  memset(nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));
  nlh->nlmsg_len = NLMSG_SPACE(MAX_PAYLOAD);
  nlh->nlmsg_pid = getpid();
  nlh->nlmsg_flags = 0;

  strcpy(NLMSG_DATA(nlh), "Hello");

  iov.iov_base = (void *)nlh;
  iov.iov_len = nlh->nlmsg_len;
  memset(&msg, 0, sizeof(msg));
  msg.msg_name = (void *)&dest_addr;
  msg.msg_namelen = sizeof(dest_addr);
  msg.msg_iov = &iov;
  msg.msg_iovlen = 1;

  sendmsg(sock_fd, &msg, 0);

  recvmsg(sock_fd, &msg, 0);
  printf("Received message payload: %s\n", NLMSG_DATA(nlh));
  printf("Waiting for messages from kernel...\n");

  while (1) {
    memset(nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));
    ret = recvmsg(sock_fd, &msg, 0);
    if (ret < 0) {
      printf("Error receiving message: %s\n", strerror(errno));
      break;
    }
    printf("Received message payload: %s\n", NLMSG_DATA(nlh));
    if (strcmp(NLMSG_DATA(nlh), "Goodbye") == 0) {
      printf("Received exit message from kernel.\n");
      break;
    }
  }
  close(sock_fd);
}
void *device_thread_func(void *arg) {

  int ret;
  cpu_set_t mask;
  CPU_ZERO(&mask);
  CPU_SET(1, &mask);
  if (-1 == sched_setaffinity(0, sizeof mask, &mask)) {
    printf("failed to set affinity\n");

    goto err0;
  }
  if ((BeginningVM_dev_fd = open("/dev/BeginningVM", O_RDWR)) < 0) {
    printf("failed to open BeginningVM device\n");

    goto err0;
  }

  if ((ret = ioctl(BeginningVM_dev_fd, BEGINNINGVM_PROBE)) < 0) {
    printf("failed to exec ioctl BeginningVM_PROBE\n");

    goto err1;
  }

err1:
  close(BeginningVM_dev_fd);

err0:
  return NULL;
}

int main() {
  pthread_t netlink_thread, device_thread;
  if (pthread_create(&netlink_thread, NULL, netlink_thread_func, NULL) != 0) {
    printf("Failed to create netlink thread\n");
    return -1;
  }
  usleep(100000);

  if (pthread_create(&device_thread, NULL, device_thread_func, NULL) != 0) {
    printf("Failed to create device thread\n");
    pthread_cancel(netlink_thread);
    return -1;
  }
  pthread_join(netlink_thread, NULL);
  pthread_join(device_thread, NULL);

  return 0;
}