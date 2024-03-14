#include "../netlink/netlink_kern.h"
#include <linux/module.h>
#define NUM_MSRS 10
#define BUFFER_SIZE 256

MODULE_LICENSE("GPL");
MODULE_AUTHOR("KaliGo");
MODULE_VERSION("0.1");

struct msr_entry {
  u32 address;
  char *name;
};

static struct msr_entry msr_entries[] = {
    {0x480, "IA32_VMX_BASIC"},
    {0x486, "IA32_VMX_CR0_FIXED0"},
    {0x487, "IA32_VMX_CR0_FIXED1"},
    {0x488, "IA32_VMX_CR4_FIXED0"},
    {0x489, "IA32_VMX_CR4_FIXED1"},
    {0x48D, "IA32_VMX_TRUE_PINBASED_CTLS"},
    {0x48E, "IA32_VMX_TRUE_PROCBASED_CTLS"},
    {0x48B, "IA32_VMX_PROCBASED_CTLS2"},
    {0x48F, "IA32_VMX_TRUE_EXIT_CTLS"},
    {0x490, "IA32_VMX_TRUE_ENTRY_CTLS"},
};
static char buffer[BUFFER_SIZE];
static void netlink_print(char *name, u32 address, u32 eax, u32 edx) {

  int ret;
  ret = snprintf(buffer, BUFFER_SIZE, "%s (MSR 0x%X) = 0x%08X%08X\n", name,
                 address, edx, eax);

  if (ret >= BUFFER_SIZE) {

    printk(KERN_WARNING "The buffer was too small for the entire message.\n");
  } else if (ret < 0) {

    printk(KERN_WARNING "An error occurred while formatting the string.\n");
  } else {

    nl_send_msg_to_user(buffer);
  }
}

static void read_msr(struct msr_entry entry) {
  u32 eax, edx;
  asm volatile("rdmsr\n\t" : "=a"(eax), "=d"(edx) : "c"(entry.address));
  printk(KERN_INFO "%s (MSR 0x%X) = 0x%08X%08X\n", entry.name, entry.address,
         edx, eax);
  netlink_print(entry.name, entry.address, edx, eax);
}

static void BeginningVM_probe(void) {
  printk(KERN_INFO "Beginning PROBE\n");

  for (int i = 0; i < ARRAY_SIZE(msr_entries); i++) {
    read_msr(msr_entries[i]);
  }
}

int vmx_init(void) {
  BeginningVM_probe();
  return 0;
}

EXPORT_SYMBOL(vmx_init);