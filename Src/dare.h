#ifndef DARE_H
    #define DARE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>

#ifndef DARE_ALLOC
    #define DARE_ALLOC(ptr, size) realloc(ptr, size)
#endif /* DARE_ALLOC */

#ifndef DARE_FREE
    #define DARE_FREE(ptr) free(ptr)
#endif /* DARE_FREE */

#define WRAP(pos, size) size ? pos % size : 0
#define IS_OVERLOADED(load, elements, size) load <= (float)elements / (float)size

#define DARE_HTCONV(type, func) *(type *)func
#define DARE_TCONV(type, func) (type *) func

typedef enum f_code {
    
    F_NULL,
    F_NOALLOC,
    F_NOFREE,
    F_TNOMATCH,

} f_code;

typedef struct darray {
    int f_code;

    size_t elements;
    size_t size, size_bytes;
    size_t type_offset;
 
    size_t push, pull;
        
    float load;
    double expander;

    void *data; // we could just make this one big field of data
} darray;

typedef struct darray_config {
    
    size_t type_offset;
    size_t size;

    float load;
    double expander;

} darray_config;

void dare_init(darray *dare, darray_config *dare_conf);
void dare_deinit(darray *dare);

size_t dare_push(darray *dare, void *item);
void *dare_pull(darray *dare);

size_t dare_insert(darray *dare, void *item, size_t pos);
void *dare_get(darray *dare, size_t pos);

void *dare_pop(darray *dare);
void dare_hremove(darray *dare, size_t pos, void *dest);

void dare_merge(darray *src, darray *dst, size_t offset);

#endif /* DARE_H */
