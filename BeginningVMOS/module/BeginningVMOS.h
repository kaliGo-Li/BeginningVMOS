#ifndef __BEGINNINGVM__ 
#define __BEGINNINGVM__
#include <linux/ioctl.h>
#ifdef USERSPACE
   #include <stdint.h>
   #define u64 uint64_t
   #define u32 uint32_t
   #define u8 uint8_t
#else
   #include <linux/types.h>
#endif

#define BEGINNINGVM_COUNT 1
#define BEGINNINGVM_MAJOR 511
#define BEGINNINGVM_MINOR 0

#define BEGINNINGVM_MAGIC 'M'
#define BEGINNINGVM_PROBE _IOR(BEGINNINGVM_MAJOR, 0, u64)
#define BEGINNINGVM_RUN _IO(BEGINNINGVM_MAGIC, 1)

#endif