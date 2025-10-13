#include "mem.h"
#include "uart.h"
#include "log.h"

void meminit(void)
{
  info("Initializing memory manager...");

  void *page_table_start = (void *)_PAGE_TABLE_START;
  unsigned int count = 1;
  for (page_table_start; page_table_start < (void *)_PAGE_TABLE_END; page_table_start += sizeof(struct pte))
  {
    struct pte *entry = (struct pte *)page_table_start;
    entry->used = 0;
    entry->addr = (void *)(_PAGE_TABLE_END + 1) + (count * _PAGE_SIZE);
    count++;
  }

  info("Memory manager initialized.");
}

void *get_page(void)
{
  void *page_table_start = (void *)_PAGE_TABLE_START;
  for (page_table_start; page_table_start < (void *)_PAGE_TABLE_END; page_table_start += sizeof(struct pte))
  {
    struct pte *entry = (struct pte *)page_table_start;

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
  void *page_table_start = (void *)_PAGE_TABLE_START;
  for (page_table_start; page_table_start < (void *)_PAGE_TABLE_END; page_table_start += sizeof(struct pte))
  {
    struct pte *entry = (struct pte *)page_table_start;
    if (entry->addr == ptr)
    {
      for (void *byte = (void *)entry->addr; byte < (void *)(entry->addr + _PAGE_SIZE); byte++)
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
  for (void *byte = addr; byte < (void *)(addr + _PAGE_SIZE); byte++)
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
  for (void *page_table_start = (void *)_PAGE_TABLE_START; page_table_start < (void *)_PAGE_TABLE_END; page_table_start += sizeof(struct pte))
  {
    struct pte *entry = (struct pte *)page_table_start;
    uart_print_int(c++);
    uart_print("    ");
    uart_print_hex((unsigned int)entry->addr);
    uart_print("    ");
    uart_print_hex((unsigned int)entry->addr + _PAGE_SIZE - 1);
    uart_print("   ");
    uart_print(entry->used ? "Yes" : "No");
    uart_print("\r\n");
  }
}