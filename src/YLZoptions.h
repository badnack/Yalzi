/**
 * @file YLZoptions.h
 *
 * @author Nilo Redini
 * @author Francesco Disperati
 * @author Davide Pellegrino
 *
 * Options variables for YALZI
*/

#ifndef _YLZOPTIONS_H
#define _YLZOPTIONS_H

#define MAX_CHAR_VRB 255

int verbose_flag;

/**
   Print a line if versbose flasg is set.

   @param str String to write
*/
inline void print_verbose(char* str, ...);

#endif
