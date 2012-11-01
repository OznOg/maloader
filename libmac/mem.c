// Copyright 2011 Sebastien Gonzalve. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
//   1. Redistributions of source code must retain the above copyright
//      notice, this list of  conditions and the following disclaimer.
//
//   2. Redistributions in binary form must reproduce the above
//      copyright notice, this list of conditions and the following
//      disclaimer in the documentation and/or other materials
//      provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY Sebastien Gonzalve ``AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL Sebastien Gonzalve OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
// USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
// OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
// SUCH DAMAGE.

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

/**
 * BSD provides extra memory manipulation functions such as malloc_size which
 * need to know the way memory is handled by the libC. For now, I do not want
 * to hack directly internal structures of the libC, so I wrote those little
 * wrappers in order to do the trick.
 * So here, we allocate a little bit more memory to put our own header which
 * holds the size to the buffer and a magic. The magic is just for debug
 * purpose, the size is actually used in malloc_size()
 */
#define MALOADER_MEM_MAGIC 0x5EB15BAD

typedef struct {
  size_t size;
  int magic;
  char data[];
} mblock_t;


/* Macro to get the */
#define DATA_TO_MBLOCK(ptr) ((mblock_t *) (((char *)ptr) \
                                            - sizeof(size_t) \
                                            - sizeof(int)))

/* For this wrapper, good size is the size itself */
size_t malloc_good_size(size_t size) {
  return size;
}

/* Pickup the size field in the header hidden before buffer */
size_t malloc_size(const void *ptr) {
 return DATA_TO_MBLOCK(ptr)->size;
}

void *__darwin_malloc(size_t size) {
  mblock_t *block = malloc(size + sizeof(*block));
  block->size = size;
  block->magic = MALOADER_MEM_MAGIC;
  return block->data;
}

void *__darwin_realloc(void *ptr, size_t size) {
  mblock_t *block = malloc(size + sizeof(*block));
  mblock_t *old_block = DATA_TO_MBLOCK(ptr);

  block->size = size;
  block->magic = MALOADER_MEM_MAGIC;

  if (ptr == NULL)
    return block->data;

  assert(old_block->magic == MALOADER_MEM_MAGIC);

  if (size >= old_block->size)
    memcpy(block->data, old_block->data, old_block->size);
  else
    memcpy(block->data, old_block->data, size);

  free(old_block);

  return block->data;
}

void *__darwin_calloc(size_t nmemb, size_t size){
  void *data = __darwin_malloc(size * nmemb);

  memset(data, 0, size * nmemb);
  return data;
}

char *__darwin_strdup(const char *s)
{
  void *data = __darwin_calloc(1, strlen(s) + 1 /* for '\0' */);

  memcpy(data, s, strlen(s));

  return data;
}

int __darwin_vasprintf(char **strp, const char *fmt, va_list ap)
{
  va_list aq;
  va_copy(aq, ap);
  /* snprintf has the ability to tell how many byte WOULD HAVE BEEN written
   * if the buffer was not too small. Thus here, I use this trick to know the
   * amount of memory that is needed to be allocated.
   * Passing 0 for size afford that no byte will be written */
  int len = vsnprintf(NULL, 0, fmt, aq);
  va_end(aq);

  /* Alloc enougth space */
  *strp = __darwin_malloc(len + 1);

  return vsnprintf(*strp, len + 1, fmt, ap);
}

int __darwin_asprintf(char **strp, const char *fmt, ...) {
  int err;
  va_list argp;
  va_start(argp, fmt);
  err = __darwin_vasprintf(strp, fmt, argp);
  va_end(argp);
  return err;
}

void __darwin_free(void *ptr) {
  mblock_t *block = DATA_TO_MBLOCK(ptr);

  if (ptr == NULL)
    return;

  assert(block->magic == MALOADER_MEM_MAGIC);

  free(block);
}

