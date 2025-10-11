#include "mem.h"
#include "uart.h"

void meminit(void)
{
  uart_print("Initializing memory...\r\n");

  for (unsigned int *i = (unsigned int *)_PHYS_MEM_START; i < (unsigned int *)_PHYS_MEM_END; i++)
  {
    mem.next = (struct mem_block *)i;
    mem = *mem.next;
  }
}

void *bmalloc(void)
{
  if (mem.next == 0)
  {
    return 0;
  }
  struct mem_block *allocated_block = &mem;
  mem = *mem.next;
  return (void *)allocated_block;
}

void bfree(void *ptr)
{
  struct mem_block *block = (struct mem_block *)ptr;
  block->next = mem.next;
  mem.next = block;
}