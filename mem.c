#include "mem.h"
#include "uart.h"
#include "log.h"

void meminit(void)
{
  info("Initializing memory manager...");

  unsigned int count = 1;
  for (void *page_table = (void *)PAGE_TABLE_START; page_table < (void *)PAGE_TABLE_END; page_table += sizeof(struct pte))
  {
    void *start_addr = (void *) (PAGE_TABLE_END + 1) + (count * PAGE_SIZE);
    struct pte *entry = page_table;
    entry->used = 0;

    if ((start_addr + PAGE_SIZE) >= (void*) STACK_GUARD)
    {
      entry->addr = 0;
      continue;
    }

    entry->addr = start_addr;
    count++;
  }

  info("Memory manager initialized.");
}

void *get_page(void)
{

  for (void *page_table_start = (void *)PAGE_TABLE_START; page_table_start < (void *)STACK_GUARD; page_table_start += sizeof(struct pte))
  {
    struct pte *entry = page_table_start;

    if (entry->used == 0)
    {
      entry->used = 1;
      return entry->addr;
    }
  }

  return 0;
}

void free_page(void *ptr)
{

  for (void *page_table_start = (void *)PAGE_TABLE_START; page_table_start < (void *)PAGE_TABLE_END; page_table_start += sizeof(struct pte))
  {
    struct pte *entry = page_table_start;
    if (entry->addr == ptr)
    {
      for (void *byte = entry->addr; byte < (entry->addr + PAGE_SIZE); byte++)
      {
        *(char *)byte = 0;
      }
      entry->used = 0;
      return;
    }
  }
}

void dump_page(void *addr)
{
  for (void *byte = addr; byte < (addr + PAGE_SIZE); byte++)
  {
    uart_print_hex(*(unsigned char *)byte);
    uart_print(" ");
  }
  uart_print("\r\n");
}

void dump_page_table(void)
{
  uart_print("PAGE TABLE DUMP\r\n");

  uart_print("Page  Start     End      Used\r\n");

  unsigned int c = 0;
  for (void *page_table_start = (void *)PAGE_TABLE_START; page_table_start < (void *)PAGE_TABLE_END; page_table_start += sizeof(struct pte))
  {
    struct pte *entry = page_table_start;
    uart_print_int(c++);
    uart_print("    ");
    uart_print_hex((unsigned int)entry->addr);
    uart_print("    ");
    uart_print_hex((unsigned int)entry->addr + PAGE_SIZE - 1);
    uart_print("   ");
    uart_print(entry->used ? "Yes" : "No");
    uart_print("\r\n");
  }
}