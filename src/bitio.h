/**
 * @file bitio.h
 *
 *
 * @author Nilo Redini
 * @author Francesco Disperati
 * @author Davide Pellegrino
 *
 * Variables and funcitons definition to manage Bit buffered files.
 *
 */

#ifndef _BITIO_H_
#define _BITIO_H_

#include <stdint.h>
#include <string.h>

/** GCC check */
#if __GNUC__
#if __x86_64__ || __ppc64__
#define ARCH 64
#define FULLMASK 0xFFFFFFFFFFFFFFFF
typedef uint64_t env_var;
#else
#define ARCH 32
#define FULLMASK 0xFFFFFFFF
typedef uint32_t env_var;
#endif
#endif

/** Buffer memory size */
#define BUFSIZE 4096
/** Cell buffer size */
#define CELLSIZE (sizeof(env_var) * 8)
/** Amount of cells in the buffer */
#define BUFCELLS (BUFSIZE / (sizeof(env_var)))
/** Buffer bytes size */
#define BUFBYTES (BUFCELLS * sizeof(env_var))


/** Bitfile structure */
typedef struct BITIO BITIO;

/**
   Representation of a buffered file.
*/
struct BITIO{
  /** File descriptor (UNIX std)*/
  int fd;
  /** First available bit file position (used to read) */
  uint32_t first;
  /** Fisrt free bit file position (used to write) */
  uint32_t empty;
  /** I/O mode */
  int mode;
  /** I/O buffer */
  env_var buf[BUFCELLS];
};

/**
   Opens a new file.

   @param name File name.
   @param mode Opening mode (O_WRONLY/O_RDONLY).
*/
BITIO* bitio_open(char* name, int mode);

/**
   Reads the content of a buffered file.

   @param p BITIO pointer.
   @param dst Pointer to a buffer to store the file content.
   @param dst_len Content length to read.
   @return number of bits read. On error, -1 is returned, and errno is set appropriately.
*/
int bitio_read(BITIO* bip, env_var* dst, size_t dst_len);

/**
   Writes in a buffered file.

   @param p BITIO pointer.
   @param src Data to write.
   @param src_len Data len.
   @return zero on success. On error, -1 is returned, and errno is set appropriately.
*/
int bitio_write(BITIO* bip, env_var* src, size_t src_len);

/**
   Closes a buffered file.

   @todo check if was opened.
   @param bip BITIO pointer.
   @return zero on success. On error, -1 is returned, and errno is set appropriately.
*/
int bitio_close(BITIO* bip);

/**
   Performs the buffer flush operation.

   @param bip BITIO pointer
*/
void bitio_flush(BITIO* bip);

#endif
