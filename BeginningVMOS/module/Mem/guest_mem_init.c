#include "../BeginningVMOS.h"
#include <linux/gfp.h>
#include <linux/mm.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("KaliGo");
MODULE_VERSION("0.1");

u64 ept_pointer;
u64 guest_memory_pa;

int allocate_guest_memory(unsigned long guest_memory_size,
                          unsigned char *guest_bin, size_t guest_bin_len,
                          unsigned long *guest_memory_out,
                          phys_addr_t *guest_memory_pa_out) {
  unsigned long num_pages = (guest_memory_size + PAGE_SIZE - 1) / PAGE_SIZE;
  unsigned long i;
  struct page *pages;
  void *guest_memory;
  pages =
      alloc_pages(GFP_KERNEL | __GFP_ZERO, get_order(num_pages * PAGE_SIZE));
  if (!pages) {
    printk(KERN_ERR "Failed to allocate guest memory\n");
    return -ENOMEM;
  }
  guest_memory = page_address(pages);
  *guest_memory_pa = page_to_phys(pages);
  for (i = 0; i < guest_bin_len; i++) {
    ((u8 *)guest_memory)[i] = guest_bin[i];
  }
  return 0;
}