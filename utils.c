#include "utils.h"

int str_ncpy(char* dest, const char* src, int length) {
  int i = 0;
  while (i < length) {
    if (src[i] == ' ') return i;
    dest[i] = src[i];
    i++;
  }
  return i;
}

