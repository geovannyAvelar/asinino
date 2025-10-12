#ifndef MEM_H
#define MEM_H

#define _PHYS_MEM_START   0x0100
#define _PHYS_MEM_END     0x08FF
#define _PAGE_SIZE        128
#define _PAGE_TABLE_SIZE  40
#define _PAGE_TABLE_START_OFFSET 512
#define _PAGE_TABLE_START _PHYS_MEM_START + _PAGE_TABLE_START_OFFSET
#define _PAGE_TABLE_END   _PAGE_TABLE_START + _PAGE_TABLE_SIZE

struct pte {
  unsigned int used;
  void * addr;
};

void meminit(void);
void * get_page(void);
void free_page(void *ptr);
void dump_page(void *addr);
void dump_page_table(void);

#endif // MEM_H