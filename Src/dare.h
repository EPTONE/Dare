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

#define DARE_GET_ARRPTR(head, type) (((type *)(head)) + 1)
#define DARE_GET_HEADER(arrptr, type) (((type *)(arrptr)) - 1)

#define DARE_GET_TYPE_OFFSET(arrptr, type) DARE_GET_HEADER(arrptr, type)->type_offset

#define DARE_GET_ELEMENTS(arrptr, type) DARE_GET_HEADER(arrptr, type)->elements
#define DARE_GET_SIZE(arrptr, type) DARE_GET_HEADER(arrptr, type)->size

#define DARE_GET_LOAD(arrptr, type) DARE_GET_HEADER(arrptr, type)->load
#define DARE_GET_EXPANDER(arrptr, type) DARE_GET_HEADER(arrptr)->expander

#define DARE_GET_PUSH(arrptr, type) DARE_GET_HEADER(arrptr, type)->push
#define DARE_GET_PULL(arrptr, type) DARE_GET_HEADER(arrptr, type)->pull

// Default darray_config initialization
#define INIT_DARE_CONF_DEFAULT(type) {sizeof(type), 25, 0.85f, 1.5f}

#define WRAP(pos, size) ((size) ? (pos) % (size) : 0)
#define IS_OVERLOADED(load, ele, sz) load <= (double)ele / (double)sz

typedef struct darray { // this is kinda a do everything I think it would be 
                        // good to also have some more specific data structures
    size_t type_offset;

    size_t elements;
    size_t size;
 
    size_t push, pull;
    // size_t head, tail;
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

size_t dare_insert_list(void **vp_arrptr, void *list, size_t pos, size_t cp_size);
void dare_get_list(void *arrptr, void *list, size_t pos, size_t cp_size);

/* DARE STACK IMPLEMENTATION */

typedef struct d_stack {
    size_t type_offset; // the offset of the type stored

    size_t elements; // the total elements in the stack.
    size_t size; // the total size of the array.

    size_t head; // the value at the end of the stack.

    float load; // the load limit on the stack.
    double expander; // how much the stakc should automatically expand by.
} d_stack;

void *dare_stk_conf_init(darray_config *dare_conf);
void *dare_stk_init(size_t size, size_t type_offset, float load, double expander);

void dare_stk_deinit(void *stkptr);

void *dare_stk_expand(void *stkptr, double expander);
void *dare_stk_resize(void *stkptr, size_t size);

int dare_stk_push(void **vp_stkptr, void *item);
void *dare_stk_pull(void *arrptr);
void *dare_stk_pop(void *stkptr);

void dare_stk_reset(void *stkptr);

/* DARE QUEUE IMPLEMENTATION */

typedef struct d_queue {
    size_t type_offset;

    size_t elements;
    size_t size;

    size_t head, tail;

    float load;
    double expander;
} d_queue;

/*
void *dare_queue_init_conf();
void *dare_queue_init(size_t size, size_t type_offset, float load, double expander);

signed long int dare_queue_push();
void *dare_queue_consume();
*/

// -------------------------------------------------------------------------- //

#endif /* DARE_H */
