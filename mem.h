#ifndef MEM_H
#define MEM_H

#include <stdint.h>

#define PHYS_MEM_START   (0x0100 + 512)
#define PHYS_MEM_END     0x08FF
#define STACK_SIZE       128
#define STACK_GUARD      (PHYS_MEM_END - STACK_SIZE)
#define PAGE_SIZE        128
#define PAGE_COUNT       (((PHYS_MEM_END - STACK_SIZE - PHYS_MEM_START) / PAGE_SIZE) - 1)
#define PAGE_TABLE_SIZE  (PAGE_COUNT * sizeof(struct pte))
#define PAGE_TABLE_START (PHYS_MEM_START + 1)
#define PAGE_TABLE_END   (PAGE_TABLE_START + PAGE_TABLE_SIZE)

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