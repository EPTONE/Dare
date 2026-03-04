#include "dare.h" 

void *dare_init(darray_config *dare_conf) {
    assert(dare_conf);

    size_t size_bytes = dare_conf->size * dare_conf->type_offset;
    darray *dare = DARE_ALLOC(NULL, sizeof(darray) + size_bytes);
    if(dare == NULL) {
        assert(dare);
        return NULL;
    }

    dare->f_code = F_NULL;

    dare->elements = 0;
    dare->push = 0, dare->pull = 0;
  
    dare->load = dare_conf->load;
    dare->expander = dare_conf->expander; 
     
    dare->size = dare_conf->size;
    dare->size_bytes = size_bytes;

    void *data_offset = dare + 1;
    memset(data_offset, 0, size_bytes); 

    return data_offset;
}

void *dare_resize(void *arrptr, size_t expander) {
   assert(arrptr);
    
    darray *head = DARE_GET_HEADER(arrptr);
    head = DARE_REALLOC(head, head->size_bytes * expander);
    if(head == NULL) {
        assert(head);
        return NULL;
    }
    
    return head + 1;
}

// turn the rest of these into macros
// make fucking tests before you beat yourself with everything not working

/* removes the data at position copy data to dest if not NULL
 *
 * dare : the array structure to use | pos : the position in the array |
 * dest : the type to contain the data in before it is removed | return : void
 * */

/*
void dare_merge(darray *src, darray *dst, size_t offset) {
    dst->f_code = F_NULL;

    offset = WRAP(offset, dst->size);
    if(src->type_offset != dst->type_offset) {
        src->f_code = F_TNOMATCH, dst->f_code = F_TNOMATCH; 
        return;
    }
    
    while(IS_OVERLOADED(dst->load, src->size + offset, dst->size)) {
       dare_resize(dst, dst->expander);
       if(dst->f_code == F_NOALLOC) return;
    }

    dst->data += (offset * dst->type_offset);
    mempcpy(dst->data, src->data, src->size_bytes);
}
*/
