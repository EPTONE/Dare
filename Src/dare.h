#ifndef DARE_H
    #define DARE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>

#ifndef DARE_ALLOC // User defined allocation method
    #define DARE_ALLOC(ptr, size) realloc(ptr, size)
#endif /* DARE_ALLOC */

#ifndef DARE_REALLOC // User defined reallocation method
    #define DARE_REALLOC(ptr, size) realloc(ptr, size)
#endif /* DARE_REALLOC */

#ifndef DARE_FREE // User defined free method 
    #define DARE_FREE(ptr) free(ptr)
#endif /* DARE_FREE */

#define HCONV(type) *(type *)
#define PCONV(type)  (type *)

#define DARE_GET_ARRPTR(head) ((head) + 1)
#define DARE_GET_HEADER(arrptr) ((darray *)(arrptr)) - 1

// Default darray_config initialization
#define INIT_DARE_CONF_DEFAULT(type) {sizeof(type), 25, 0.75f, 2.0f}

#define WRAP(pos, size) size ? (pos) % size : 0
#define IS_OVERLOADED(load, elements, size) load <= (float)elements / (float)size

// we can probably get rid of this and replace it with another library
typedef enum f_code {
    
    F_NULL,
    F_NOALLOC,
    F_NOFREE,
    F_TNOMATCH,

} f_code;

typedef struct darray {
    int f_code;

    size_t type_offset;

    size_t elements;
    size_t size;
 
    size_t push, pull;
    // might not be bad to add some queue functionality in here,
    // like a head and tail

    float load;
    double expander;
} darray;

typedef struct darray_config { 
    size_t type_offset;
    size_t size;

    float load;
    double expander;
} darray_config;

void *dare_init(darray_config *dare_conf);
void dare_deinit(void *arrptr);

void *dare_expand(void *arrptr, double expander);
void dare_merge(void *src, void **vp_dst, size_t offset);

size_t dare_push(void **vp_arrptr, void *item);
void *dare_pull(void *arrptr);

size_t dare_insert(void **vp_arrptr, void *item, size_t pos);
void *dare_get(void *arrptr, size_t pos);

void *dare_pop(void *arrptr);
void *dare_remove(void *arrptr, size_t pos);

#endif /* DARE_H */
