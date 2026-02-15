#include "dare.h" 
#include <string.h>
void dare_init(darray *dare, darray_config *dare_conf) {
    assert(dare);
    assert(dare_conf);

    dare->f_code = F_NULL;
   
    dare->elements = 0;
    dare->push = 0, dare->pull = 0;
    dare->type_offset = dare_conf->type_offset;
  
    dare->load = dare_conf->load;
    dare->expander = dare_conf->expander;

    size_t size_bytes = dare_conf->size * dare_conf->type_offset;
    dare->data = DARE_ALLOC(NULL, size_bytes);
    
    if(dare->data) {
   
        dare->size = dare_conf->size;
        dare->size_bytes = size_bytes;
        memset(dare->data, 0, dare->size_bytes);

    } else {
       
        dare->size = 0;
        dare->size_bytes = 0;

        // process failure throw at user if not in debug
        dare->f_code = F_NOALLOC;
        assert(!dare->data);
    
    }

}

void dare_deinit(darray *dare) {
    assert(dare);
    dare->f_code = F_NULL;

    if(dare->data) DARE_FREE(dare->data);
    else dare->f_code = F_NOFREE, assert(!dare->data);

    dare->data = NULL;
    
    dare->size = 0, dare->size_bytes = 0;
    dare->elements = 0;
    
    dare->push = 0, dare->pull = 0;
    dare->type_offset = 0;
}

void dare_resize(darray *dare, size_t expander) {
    assert(dare);

    size_t n_size_bytes = dare->size_bytes * expander;
    dare->data = DARE_ALLOC(dare->data, n_size_bytes);
    
    if(dare->data) {
    
        dare->size = n_size_bytes / dare->type_offset;
        dare->size_bytes = n_size_bytes;
    
    } else {
        
        dare->f_code = F_NOALLOC;
        dare->size = 0, dare->size_bytes = 0;
        assert(!dare->data);
    
    }
}

/* Pushes elements into alloc'd memory and increments to next free position,
 * wrapping if index goes past size 
 *
 * dare : array of memory to preform operations on | item : the pointer to the 
 * item to copy into memory | return : size_t insertion point of item */
size_t dare_push(darray *dare, void *item) {
    assert(dare);
    dare->f_code = F_NULL;

    if(IS_OVERLOADED(dare->load, dare->elements, dare->size)) {
        dare_resize(dare, dare->expander);
    }

    size_t push = WRAP(dare->push, dare->size);

    void *point = dare->data + (push * dare->type_offset);
    memcpy(point, item, dare->type_offset);

    dare->elements++;

    dare->push = push;
    dare->push++;
    return push;
}

void *dare_pull(darray *dare) {
    assert(dare);
    dare->f_code = F_NULL;

    size_t pull = WRAP(dare->pull, dare->size);

    void *point = dare->data + (pull * dare->type_offset);

    dare->pull = pull;
    dare->pull++;
    return point; 
}

size_t dare_insert(darray *dare , void *item, size_t pos) {
   
    if(IS_OVERLOADED(dare->load, dare->elements, dare->size)) {
        dare_resize(dare, dare->expander);
    }

    size_t n_pos = WRAP(pos, dare->size);
    void *point = dare->data + (n_pos * dare->type_offset);
    memcpy(point, item, dare->type_offset);

    dare->elements++;

    return n_pos;
}

void *dare_get(darray *dare, size_t pos) {
    size_t n_pos = WRAP(pos, dare->size);
    void *point = dare->data + (n_pos * dare->type_offset);
    
    return point;
}

void *dare_pop(darray *dare) {

    if(IS_OVERLOADED(dare->load, dare->elements, dare->size)) {
        dare_resize(dare, dare->expander);
    }

    size_t n_pos = WRAP(dare->push - 1, dare->size);
    void *point = dare->data + (n_pos * dare->type_offset);

    dare->elements--;
    dare->push--;

    return point;
}

/* removes the data at position copy data to dest if not NULL
 *
 * dare : the array structure to use | pos : the position in the array |
 * dest : the type to contain the data in before it is removed | return : void
 * */
void dare_remove(darray *dare, size_t pos, void *dest) {

    if(IS_OVERLOADED(dare->load, dare->elements, dare->size)) {
        dare_resize(dare, dare->expander);
    }

    size_t n_pos = WRAP(pos, dare->size);
    void *point = dare->data + (n_pos * dare->type_offset);
    
    if(dest) memcpy(dest, point, dare->type_offset);
    memset(point, 0, dare->type_offset);

    dare->elements--;
}
