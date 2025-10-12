#include "asi_string.h"

int strcmp(const char *s1, const char *s2)
{
  while (*s1 && (*s1 == *s2)) {
    s1++;
    s2++;
  }
  return *(const unsigned char *)s1 - *(const unsigned char *)s2;
}

void asi_strcpy(char *dest, const char *src)
{
  while ((*dest++ = *src++));
}

int strsplit(const char *str, char sep, unsigned int index, char *output)
{
  unsigned int current_index = 0;
  while (*str) {
    const char *start = str;
    while (*str && *str != sep) {
      str++;
    }
    if (current_index == index) {
      while (start < str) {
        *output++ = *start++;
      }
      *output = '\0';
      return 0;
    }
    current_index++;
    if (*str) {
      str++;
    }
  }
  *output = '\0';

  return -1;
}