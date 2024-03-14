#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include "../BeginningVMOS.h"
#include "../netlink/netlink_kern.h"
#include "../ioctl/ioctl.h"
MODULE_LICENSE("GPL");
MODULE_AUTHOR("KaliGo");
MODULE_VERSION("0.1");

static dev_t BeginningVM_dev;
static struct cdev BeginningVM_cdev;
static struct file_operations BeginningVM_fops = {
	.owner = THIS_MODULE,
	.unlocked_ioctl = BeginningVM_ioctl,
};
int Beginning_register_device(void) {
    printk(KERN_INFO "Beginning device module started.\n"); 
    BeginningVM_dev = MKDEV(BEGINNINGVM_MAJOR,BEGINNINGVM_MINOR);
    if(0 < register_chrdev_region(BeginningVM_dev, BEGINNINGVM_COUNT,"BeginningVM")){
        printk(KERN_ERR "Beginning device module: register_chrdev_region error\n");
        // nl_send_msg_to_user(user_pid,"Beginning device module: register_chrdev_region error\n");
        goto err0;
    }

    cdev_init(&BeginningVM_cdev, &BeginningVM_fops);
    BeginningVM_cdev.owner = THIS_MODULE;

    if(0 < cdev_add(&BeginningVM_cdev,BeginningVM_dev,1)) {
        printk("Beginning device module:cdev_add error\n");
        // nl_send_msg_to_user(user_pid,"Beginning device module:cdev_add error\n");
        goto err1;
    }

    err1:
    unregister_chrdev_region(BeginningVM_dev,1);
    err0:
    
    return -ENODEV;

}

void BeginningVM_device_exit(void) {
    printk("Beginning device module EXIT.\n");
    cdev_del(&BeginningVM_cdev);
    unregister_chrdev_region(BeginningVM_dev,1);
    return;
}

EXPORT_SYMBOL(Beginning_register_device);
EXPORT_SYMBOL(BeginningVM_device_exit);