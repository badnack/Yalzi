/**
 * @file LZ78compressor.h
 *
 * @author Nilo Redini
 * @author Francesco Disperati
 * @author Davide Pellegrino
 * Variables and functions definitions for LZ78 compression
 *
*/

#ifndef _LZ78COMPRESSOR_H_
#define _LZ78COMPRESSOR_H_

#include "bitio.h"

#define BYTEBUFFERSIZE 4096

/**
   Compress a file.
   
   @param in_file Input file
   @param out_file Output file
   @return zero on success. On error, -1 is returned, and errno is set appropriately.
*/
int compress(BITIO* in_file, BITIO* out_file);

#endif
