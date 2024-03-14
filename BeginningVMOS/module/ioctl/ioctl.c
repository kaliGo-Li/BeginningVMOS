#include <linux/module.h>
#include "../BeginningVMOS.h"
#include "../VMX/vmx.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("KaliGo");
MODULE_VERSION("0.1");

long BeginningVM_ioctl(struct file *file,
			unsigned int cmd,
			unsigned long data) {
    switch(cmd) {
        case BEGINNINGVM_PROBE:
         vmx_init();
         break;
    }
    return 0;
}
EXPORT_SYMBOL(BeginningVM_ioctl);