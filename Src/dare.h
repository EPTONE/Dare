#ifndef DARE_H
    #define DARE_H

#include <stddef.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#ifndef DARE_ALLOC
    #define DARE_ALLOC(ptr, size) realloc(ptr, size)
#endif /* DARE_ALLOC */

#ifndef DARE_FREE
    #define DARE_FREE(ptr) free(ptr)
#endif /* DARE_FREE */

#define WRAP(pos, size) size ? pos % size : 0
#define IS_OVERLOADED(load, elements, size) load <= (float)elements / (float)size

typedef enum f_code {
    
    F_NULL,
    F_NOALLOC,
    F_NOFREE,

} f_code;

typedef struct darray {
    int f_code;

    size_t push, pull;
    size_t type_offset;
    
    float load;
    size_t expander;

    size_t elements;
    size_t size, size_bytes;

    void *data;

} darray;

typedef struct darray_config {
    
    size_t type_offset;
    size_t size;

    float load;
    size_t expander;

} darray_config;

void dare_init(darray *dare, darray_config *dare_conf);
void dare_deinit(darray *dare);

size_t dare_push(darray *dare, void *item);
size_t dare_insert(darray *dare, void *item, size_t pos);

void *dare_pull(darray *dare);
void *dare_get(darray *dare, size_t pos);

void *dare_pop(darray *dare);
void *dare_remove(darray *dare, size_t pos);

#endif /* DARE_H */
