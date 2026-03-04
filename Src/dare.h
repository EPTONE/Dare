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

#ifndef DARE_REALLOC
    #define DARE_REALLOC(ptr, size) realloc(ptr, size)
#endif /* DARE_REALLOC */

#ifndef DARE_FREE
    #define DARE_FREE(ptr) free(ptr)
#endif /* DARE_FREE */

#define INIT_DARE_CONF_DEFAULT(type) {sizeof(type), 25, 0.75f, 2.0f}

#define WRAP(pos, size) size ? (pos) % size : 0
#define IS_OVERLOADED(load, elements, size) load <= (float)elements / (float)size
#define DARE_GET_HEADER(arrptr) ((darray *)(arrptr)) - 1

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
void *dare_resize(void *arrptr, size_t expander);

#define DARE_DEINIT(arrptr)                                                    \
    do {                                                                       \
        assert(arrptr);                                                        \
        darray *head = DARE_GET_HEADER(arrptr);                                \
        DARE_FREE(head);                                                       \
        arrptr = NULL                                                          \
    } while(false)                                                             \

#define DARE_PUSH(arrptr, item)                                                \
    do {                                                                       \
        assert(arrptr);                                                        \
        darray *head = DARE_GET_HEADER(arrptr);                                \
        while(IS_OVERLOADED(head->load, head->elements, head->size)) {         \
            arrptr = dare_resize(arrptr, head->size * head->expander);         \
        }                                                                      \
                                                                               \
        size_t push = WRAP(head->push, head->size);                            \
        arrptr[push] = item;                                                   \
        head->push = push++; head->elements++;                                 \
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
            arrptr = dare_resize(arrptr, head->size * head->expander);         \
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
        size_t pull = WRAP(pos, item);                                         \
        item = arrptr[pull]                                                    \
    } while(false)                                                             \

#define DARE_PGET(arrptr, item, pos)                                           \
    do {                                                                       \
        assert(arrptr);                                                        \
        darray *head = DARE_GET_HEADER(arrptr);                                \
                                                                               \
        size_t pull = WRAP(pos, item);                                         \
        item = &arrptr[pull]                                                   \
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

#define DARE_HREMOVE(arrptr, item, pos)\
    do {                                                                       \
        darray *head = DARE_GET_HEADER(arrptr);                                \
                                                                               \
        size_t pull = WRAP(pos, head->size);                                   \
        item = arrptr[pull];                                                   \
        head->elements--;                                                      \
    } while(false)                                                             \

#define DARE_PREMOVE(arrptr, item, pos)                                        \
    do {                                                                       \
        darray *head = DARE_GET_HEADER(arrptr);                                \
                                                                               \
        size_t pull = WRAP(pos, head->size);                                   \
        item = &arrptr[pull];                                                  \
        head->elements--;                                                      \
    } while(false)                                                             \

void dare_merge(darray *src, darray *dst, size_t offset);

#endif /* DARE_H */
