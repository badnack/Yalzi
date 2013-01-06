/**
 * @file bitio.c
 *
 *
 * @author Francesco Disperati
 * @author Davide Pellegrino
 * @author Nilo Redini
 *  Funcrtions to manage buffered operations on a file.
 */

#include "bitio.h"
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>


inline static int
min(int x1, int x2)
{
  return (x1<=x2)?x1:x2;
}

static int
store_buffer(BITIO* bip, size_t count)
{
  size_t n = count / sizeof(env_var) + ((count % sizeof(env_var)) > 0);
  ssize_t written_bytes = 0;
  uint8_t* buffer;

  if(bip == NULL || count > BUFBYTES || !count){
    errno = EINVAL;
    return -1;
  }

  buffer = (uint8_t*)bip->buf;
  for(; n--;) /* Portability */
    if(ARCH == 32){
      bip->buf[n] = htole32(bip->buf[n]);
    } else {
      bip->buf[n] = htole64(bip->buf[n]);
    }


  n = 0;
  while(n < count){
    written_bytes = write(bip->fd, (uint8_t*)(buffer + n), count - n);
    if(written_bytes == -1)
      return -1;

    n += written_bytes;

  }
  return n;
}

static int
load_buffer(BITIO* bip, size_t count)
{
  size_t n = 0;
  ssize_t read_bytes = 0;
  uint8_t* buffer;
  int offset;

  if(bip == NULL || bip->fd == -1 || !count || count > BUFBYTES){
    errno = EINVAL;
    return -1;
  }
  buffer = (uint8_t*)bip->buf;

  while(n < count){
    read_bytes = read(bip->fd, buffer + n, count - n);
    if(read_bytes == -1)
      return -1;
    if(read_bytes == 0)
      break;
    n += read_bytes;
  }

  read_bytes = n;
  offset = n % sizeof(env_var);
  n = n / sizeof(env_var);

  if(offset != 0){
    n++;
    bip->buf[n] &= ((((env_var)1) << offset * 8) - 1);
  }

  for(; n--;) /* Portability */
    if(ARCH == 32){
      bip->buf[n] = le32toh(bip->buf[n]);
    } else {
      bip->buf[n] = le64toh(bip->buf[n]);
    }

  return read_bytes;
}

static int
fix_write(BITIO* bip, env_var src, size_t src_len)
{
  int index;
  int offset;
  int bits_to_write;
  env_var mask;

  if(src_len > CELLSIZE){
      errno = EINVAL;
      return -1;
  }

  while(src_len > 0){
    index = (bip->empty) / CELLSIZE;
    offset = (bip->empty) % CELLSIZE;
    bits_to_write = min(CELLSIZE - offset, src_len);
    mask = (bits_to_write == CELLSIZE) ? FULLMASK : ((((env_var)1) << bits_to_write) - 1);
    bip->buf[index] &= /* (bits_to_write == CELLSIZE) ? 0 : */ (((env_var)1) << offset) - 1;
    bip->buf[index] |= (src & mask) << offset;
    src_len -= bits_to_write;
    bip->empty += bits_to_write;
    if(bip->empty >= BUFCELLS * CELLSIZE){
      if(store_buffer(bip, BUFBYTES) == -1)
        return -1;
      bip->empty = 0;
    }
    src >>= bits_to_write;
  }

  return 0;

}

static int
fix_read(BITIO* bip, env_var* dst, size_t dst_len)
{
  int err;
  int bits_to_read;
  int read_bits = 0;
  int index;
  int offset;
  env_var mask;

  if(dst_len > CELLSIZE){
    errno = EINVAL;
    return -1;
  }

  while(dst_len > 0){
    if(bip->first >= BUFCELLS * CELLSIZE || bip->empty <= bip->first){
      if((err = load_buffer(bip, BUFBYTES)) == -1)
        return -1;
      bip->first = 0;
      bip->empty = err * 8;
      if(bip->empty == 0)
        return read_bits;
    }
    index = (bip->first) / CELLSIZE;
    offset = (bip->first) % CELLSIZE;
    bits_to_read = min(CELLSIZE - offset, min(dst_len, bip->empty - bip->first));
    mask = (bits_to_read == CELLSIZE) ? FULLMASK : (((((env_var)1) << bits_to_read) - 1) << offset);
    *dst &= /* (bits_to_read == CELLSIZE) ? 0 : */ ~((((env_var)1 << bits_to_read) - 1) << read_bits);
    *dst |= (offset - read_bits > 0) ? ((env_var)(bip->buf[index] & mask) >> (offset - read_bits)) : ((env_var)(bip->buf[index] & mask) << (read_bits - offset));
    dst_len -= bits_to_read;
    bip->first += bits_to_read;
    read_bits += bits_to_read;
  }
  return read_bits;
}

BITIO*
bitio_open(char* name, int mode)
{
  int fd;
  BITIO* bf;

  if(mode != O_RDONLY && mode != O_WRONLY){
    errno = EINVAL;
    return NULL;
  }

  if((fd = open(name, (mode == O_RDONLY ? O_RDONLY:(O_WRONLY | O_CREAT | O_TRUNC)), 0644)) < 0)
    return NULL;

  if((bf = (BITIO*) calloc(1, sizeof(BITIO))) == NULL){
    close(fd);
    errno = ENOMEM;
    return NULL;
  }

  memset(bf, 0, sizeof(BITIO));
  bf->fd = fd;
  bf->mode = mode;

  return bf;
}

int
bitio_write(BITIO* bip, void* src, size_t src_len)
{
  int b_steps = src_len / (sizeof(env_var) * 8) + ((src_len % (sizeof(env_var) * 8)) > 0);
  int i,size;
  env_var* data = (env_var*) src;

  if(bip == NULL || data == NULL || src_len <= 0){
    errno = EINVAL;
    return -1;
  }
  if(bip->mode != O_WRONLY){
    errno = EBADF;
    return -1;
  }

  for(i = 0; i < b_steps; i++){
    size = (src_len > (sizeof(env_var) * 8)) ? ((sizeof(env_var) * 8)) : src_len;
    if(fix_write(bip, data[i], size) == -1)
      return -1;
    src_len -= size;
  }
  return 0;
}

int
bitio_read(BITIO* bip, void* dst, size_t dst_len)
{
  int read_bits = 0;
  int b_steps = dst_len / (sizeof(env_var) * 8) + ((dst_len % (sizeof(env_var) * 8)) > 0);
  int i,size;
  env_var* data = (env_var*) dst;

  if(dst == NULL || bip == NULL || dst_len <= 0){
    errno = EINVAL;
    return -1;
  }
  if(bip->mode != O_RDONLY){
    errno = EBADF;
    return -1;
  }

  for(i = 0; i < b_steps; i++){
    size = (dst_len > (sizeof(env_var) * 8)) ? (sizeof(env_var) * 8) : dst_len;
    if((read_bits += fix_read(bip, &data[i], size)) <= 0)
      return -1;
    dst_len -= size;
  }

  return read_bits;
}


int
bitio_close(BITIO* bip)
{
  int err, index, offset;

  if(bip == NULL){
    errno = EINVAL;
    return -1;
  }

  err = 0;
  if(bip->mode == O_WRONLY && bip->empty > 0){
    index = bip->empty / CELLSIZE;
    if((offset = (bip->empty % CELLSIZE)))
      bip->buf[index] &= ((env_var)1 << offset) - 1;
    store_buffer(bip,  index*sizeof(env_var) + (offset / 8) + ((offset % 8) > 0));
  }

  err = close(bip->fd);
  memset(bip, 0, sizeof(BITIO));
  free(bip);

  return err;
}

void 
bitio_flush(BITIO* bip)
{
  int index, offset;
  
  if(bip == NULL)
    return;

  if(bip->mode == O_WRONLY && bip->empty > 0){
    index = bip->empty / CELLSIZE;
    if((offset = (bip->empty % CELLSIZE)))
      bip->buf[index] &= ((env_var)1 << offset) - 1;
    store_buffer(bip,  index*sizeof(env_var) + (offset / 8) + ((offset % 8) > 0));
  }

  memset(bip->buf, 0, BUFBYTES);
}
