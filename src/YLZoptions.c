#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>

#include "YLZoptions.h"

inline void print_verbose(char* str)
{
  if(str == NULL)
    return;

  if(verbose_flag == 1)
    if((write(STDOUT_FILENO, str,  strnlen(str, MAX_CHAR_VRB)))){/*Shut up compiler*/}
}
