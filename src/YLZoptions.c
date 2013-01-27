#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <stdarg.h>

#include "YLZoptions.h"

inline void print_verbose(char* str, ...)
{
  char buf[MAX_CHAR_VRB];
  va_list argptr;
  int len;

  if (str == NULL || verbose_flag != 1)
    return;

  va_start(argptr, str);
  vsnprintf(buf, MAX_CHAR_VRB, str, argptr);
  va_end(argptr);
  len = strnlen(buf, MAX_CHAR_VRB);

  if ((write(STDOUT_FILENO, buf,  len))){
    /*Shut up compiler*/
  }
}
