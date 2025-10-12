#ifndef ASI_STRING_H
#define ASI_STRING_H

int strcmp(const char *s1, const char *s2);
void asi_strcpy(char *dest, const char *src);
int strsplit(const char *str, char sep, unsigned int index, char *output);

#endif // ASI_STRING_H