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

// Default darray_config initialization
#define INIT_DARE_CONF_DEFAULT(type) {sizeof(type), 25, 0.75f, 2.0f}

#define WRAP(pos, size) size ? (pos) % size : 0
#define IS_OVERLOADED(load, elements, size) load <= (float)elements / (float)size
#define DARE_GET_HEADER(arrptr) ((darray *)(arrptr)) - 1

// we can probably get rid of this and replace it with another library
typedef enum f_code {
    
    F_NULL,
    F_NOALLOC,
    F_NOFREE,
    F_TNOMATCH,

} f_code;

typedef struct darray {
    int f_code;

    size_t elements;
    size_t size;
 
    size_t push, pull;
        
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
void *dare_resize(void *arrptr, double expander);

/* Deinitalizes the array setting the provided pointer to NULL
 *
 * *arrptr : the pointer to the provided array | returns : (void) 
 */
#define DARE_DEINIT(arrptr)                                                    \
    do {                                                                       \
        assert(arrptr);                                                        \
        darray *head = DARE_GET_HEADER(arrptr);                                \
        DARE_FREE(head);                                                       \
        arrptr = NULL;                                                         \
    } while(false)                                                             \

/* Resizes the array to the specified size returning a pointer to the new array
 *
 * *arrptr : the pointer to the array | expander : how much to increase or 
 * decrease the array | returns : (void *) a pointer to the beginning of the 
 * new array
 */
#define DARE_EXPAND(arrptr, expander)                                          \
    do {                                                                       \
        assert(arrptr);                                                        \
                                                                               \
        darray *head = DARE_GET_HEADER(arrptr);                                \
                                                                               \
        size_t n_size = head->size * expander;                                 \
        size_t byte_size = n_size * sizeof(*arrptr);                           \
        head = DARE_REALLOC(head, byte_size + sizeof(darray));                 \
        if(head == NULL) {                                                     \
            assert(head);                                                      \
            arrptr = NULL;                                                     \
        }                                                                      \
                                                                               \
        arrptr = (void *)head + 1;                                             \
        head->size = n_size;                                                   \
                                                                               \
    } while(false)                                                             \

#define DARE_PUSH(arrptr, item)                                                \
    do {                                                                       \
        assert(arrptr);                                                        \
        darray *head = DARE_GET_HEADER(arrptr);                                \
        while(IS_OVERLOADED(head->load, head->elements, head->size)) {         \
            DARE_EXPAND(arrptr, head->expander);                               \
            head = DARE_GET_HEADER(arrptr);                                    \
        }                                                                      \
                                                                               \
        size_t push = WRAP(head->push, head->size);                            \
        arrptr[push] = item;                                                   \
        push++;                                                                \
        head->push = push;                                                     \
        head->elements++;                                                      \
    } while(false)                                                             \

#define DARE_HPULL(arrptr, item)                                               \
    do {                                                                       \
        darray *head = DARE_GET_HEADER(arrptr);                                \
        size_t pull = WRAP(head->pull, head->size);                            \
        item = arrptr[pull];                                                   \
        head->pull = pull++;                                                   \
    } while(false)                                                             \

#define DARE_PPULL(arrptr, item)                                               \
    do {                                                                       \
        darray *head = DARE_GET_HEADER(arrptr);                                \
        size_t pull = WRAP(arrptr, head->pull, head->size);                    \
        item = &arrptr[pull];                                                  \
        head->pull = pull++;                                                   \
    } while(false)                                                             \

#define DARE_INSERT(arrptr, item, pos)                                         \
    do {                                                                       \
        assert(arrptr);                                                        \
        darray *head = DARE_GET_HEADER(arrptr);                                \
        while(IS_OVERLOADED(head->load, head->elements, head->size)) {         \
            DARE_EXPAND(arrptr, head->expander);                               \
            head = DARE_GET_HEADER(arrptr);                                    \
        }                                                                      \
                                                                               \
        size_t push = WRAP(pos, head->size);                                   \
        arrptr[push] = item;                                                   \
        head->elements++;                                                      \
    } while(false)                                                             \

#define DARE_HGET(arrptr, item, pos)                                           \
    do {                                                                       \
        assert(arrptr);                                                        \
        darray *head = DARE_GET_HEADER(arrptr);                                \
                                                                               \
        size_t pull = WRAP(pos, head->size);                                   \
        item = arrptr[pull];                                                   \
    } while(false)                                                             \

#define DARE_PGET(arrptr, item, pos)                                           \
    do {                                                                       \
        assert(arrptr);                                                        \
        darray *head = DARE_GET_HEADER(arrptr);                                \
                                                                               \
        size_t pull = WRAP(pos, head->size);                                   \
        item = &arrptr[pull];                                                  \
    } while(false)                                                             \

#define DARE_HPOP(arrptr, item)                                                \
    do {                                                                       \
        darray *head = DARE_GET_HEADER(arrptr);                                \
                                                                               \
        size_t pull = WRAP(head->push--, head->size);                          \
        item = arrptr[pull];                                                   \
        head->elements--;                                                      \
    } while(false)                                                             \

#define DARE_PPOP(arrptr, item)                                                \
    do {                                                                       \
        darray *head = DARE_GET_HEADER(arrptr);                                \
                                                                               \
        size_t pull = WRAP(head->push--, head->size);                          \
        item = &arrptr[pull];                                                  \
        head->elements--;                                                      \
    } while(false)                                                             \

#define DARE_HREMOVE(arrptr, item, pos)                                        \
    do {                                                                       \
        assert(arrptr);                                                        \
        darray *head = DARE_GET_HEADER(arrptr);                                \
                                                                               \
        size_t pull = WRAP(pos, head->size);                                   \
        item = arrptr[pull];                                                   \
        head->elements--;                                                      \
    } while(false)                                                             \

#define DARE_PREMOVE(arrptr, item, pos)                                        \
    do {                                                                       \
        assert(arrptr);                                                        \
        darray *head = DARE_GET_HEADER(arrptr);                                \
                                                                               \
        size_t pull = WRAP(pos, head->size);                                   \
        item = &arrptr[pull];                                                  \
        head->elements--;                                                      \
    } while(false)                                                             \

#define DARE_MERGE(src, dst, offset)                                           \
    do {                                                                       \
        assert(src), assert(dst);                                              \
        darray *src_head = DARE_GET_HEADER(src);                               \
        darray *dst_head = DARE_GET_HEADER(dst);                               \
                                                                               \
        offset = WRAP(offset, dst_head->size);                                 \
                                                                               \
        while(IS_OVERLOADED(dst_head->load,                                    \
                            src_head->size + (offset),                         \
                            dst_head->size)) {                                 \
           dst = dare_resize(dst_head, dst_head->expander);                    \
        }                                                                      \
                                                                               \
        void *dst_offset = &dst[offset];                                       \
        mempcpy(dst_offset, src, src_head->size_bytes);                        \
    } while(false)                                                             \

#endif /* DARE_H */
