#ifndef MEM_H
#define MEM_H

#include <stdint.h>

#define _PHYS_MEM_START   0x0100
#define _PHYS_MEM_END     0x08FF
#define _STACK_SIZE       128
#define _STACK_GUARD      _PHYS_MEM_END - _STACK_SIZE
#define _PAGE_SIZE        128
#define _PAGE_TABLE_SIZE  256
#define _PAGE_TABLE_START _STACK_GUARD - _PAGE_TABLE_SIZE

struct __attribute__((packed, aligned(1))) pte {
  uint8_t used;
  void * addr;
};

void meminit(void);
void * get_page(void);
void free_page(void *ptr);
void dump_page(void *addr);
void dump_page_table(void);

#endif // MEM_H