#include "dare.h" 

/* Initializes the memory segment to be used by the header and the array
 * and then returns to the pointer at the end of said header
 *
 * *dare_conf : takes a pointer to a configuration struct values will be added
 * to the header | returns : (void *) a pointer to the beginning of the array
 */
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

    void *data_offset = dare + 1;
    memset(data_offset, 0, size_bytes); 

    return data_offset;
}
