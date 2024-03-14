#ifndef IOCTL_H
#define IOCTL_H
long BeginningVM_ioctl(struct file *file,
			unsigned int cmd,
			unsigned long data);
#endif