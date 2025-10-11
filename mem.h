#define _PHYS_MEM_START 0x0100
#define _PHYS_MEM_END   0x08FF

struct mem_block {
  struct mem_block *next;
} mem;

void meminit(void);
void *bmalloc(void);
void bfree(void *ptr);