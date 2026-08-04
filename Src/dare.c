#include "dare.h" 
#include <assert.h>

/*  initializes array with the provided config
 *  
 *  dare_conf : the config to initialize the array with | returns (void *) the
 *  memory address to the array, NULL on error
 */
void *dare_init(darray_config *dare_conf) {
    assert(dare_conf);

    size_t size_bytes = dare_conf->size * dare_conf->type_offset;
    darray *head = DARE_ALLOC(NULL, size_bytes + sizeof(darray));
    if(head == NULL) return NULL; 

    head->type_offset = dare_conf->type_offset; 

    head->size = dare_conf->size;
    head->elements = 0;

    head->load = dare_conf->load;
    head->expander = dare_conf->expander;

    head->push = 0;
    head->pull = 0;   

    memset(DARE_GET_ARRPTR(head, darray), 0, head->size * head->type_offset); 
    return DARE_GET_ARRPTR(head, darray);
}

/* frees the array from memory
 * 
 * arrptr : the array to be dealloced | returns : (void)
 */
void dare_deinit(void *arrptr) {
    assert(arrptr);

    darray *head = DARE_GET_HEADER(arrptr, darray); 
    if(head) DARE_FREE(head);
}

/* resizes the array by size * expander
 *
 * arrptr : the array to expand | expander : the multiplier to expand the array
 * by | returns : (void *) the address to the new array
 */
void *dare_expand(void *arrptr, double expander) {
    assert(arrptr);
    darray *head = DARE_GET_HEADER(arrptr, darray);
    size_t old_sz = head->size;

    size_t n_sz = head->size * expander;
    head = DARE_ALLOC(head, (n_sz * head->type_offset) + sizeof(darray));
    if(head == NULL) assert(head);
    void *n_arrptr = DARE_GET_ARRPTR(head, darray); 
    head->size = n_sz;

    // for some people they might not want to memset so lets give them and option not to
//    if(old_sz < n_sz) {
//        void *point = n_arrptr + (old_sz * head->type_offset);
//        size_t set_len = (n_sz * head->type_offset) - (old_sz * head->type_offset);
//        memset(point, 0, set_len);
//    }
  
    return DARE_GET_ARRPTR(head, darray);
}

/*  resizes array to the size of sz
 *
 *  arrptr : the array to resize | sz : what size the array should be set to |
 *  returns : (void *) the memory address to the new array
 */
void *dare_resize(void *arrptr, size_t sz) {
    assert(arrptr);

    darray *head = DARE_GET_HEADER(arrptr, darray);
    size_t old_sz = head->size;

    size_t n_sz = sz * head->type_offset;
    head = DARE_REALLOC(head, n_sz + sizeof(darray));
    if(head == NULL) assert(head);
    void *n_arrptr = DARE_GET_ARRPTR(head, darray); 
    head->size = sz;

    if(old_sz < n_sz) {
        void *point = n_arrptr + (old_sz * head->type_offset);
        size_t set_len = n_sz - (old_sz * head->type_offset);
        memset(point, 0, set_len);
    }

    return n_arrptr;
}

/* Merges source array to destination array at offset expanding destination as 
 * needed 
 *
 * src : source array to be used | vp_dst : double pointer that points to the
 * value that holds the array | offset : the spot in the array to append src |
 * returns : (void) 
 */
void dare_merge(void *src, void **vp_dst, size_t offset) {
    assert(src);
    assert(vp_dst);
    assert(*vp_dst);

    void *dst = *vp_dst;
    darray *src_head = DARE_GET_HEADER(src, darray);
    darray *dst_head = DARE_GET_HEADER(dst, darray);

    offset = WRAP(offset, dst_head->size);
    
    if(dst_head->size < src_head->size + offset) {
       *vp_dst = dare_resize(dst, (dst_head->size + src_head->size) * dst_head->expander);
        if(*vp_dst == NULL) return;
        dst = *vp_dst;
        dst_head = DARE_GET_HEADER(dst, darray);
    }

    void *point = dst + (offset * dst_head->type_offset);
    memcpy(point, src, src_head->size * src_head->type_offset);

    dst_head->elements += src_head->elements;
}

/* Pushes elements into alloc'd memory and increments to next free position,
 * wrapping if index goes past size 
 *
 * dare : array of memory to preform operations on | item : the pointer to the 
 * item to copy into memory | return : size_t insertion point of item 
 */
size_t dare_push(void **vp_arrptr, void *item) {
    assert(vp_arrptr);
    assert(*vp_arrptr);

    void *arrptr = *vp_arrptr;
    darray *head = DARE_GET_HEADER(arrptr, darray);

    if(IS_OVERLOADED(head->load, head->elements, head->size)) {
        *vp_arrptr = dare_expand(arrptr, head->expander);
        if(*vp_arrptr == NULL) return 0;
        arrptr = *vp_arrptr;
        head = DARE_GET_HEADER(arrptr, darray);
    }

    size_t push = WRAP(head->push, head->size);

    void *point = arrptr + (push * head->type_offset);
    memcpy(point, item, head->type_offset);

    head->elements++;

    head->push = push;
    head->push++;
    return push;
}

/* pulls and item from the array and returns it
 *
 * arrptr : the array to extract the item from | returns : (void *) the memory 
 * address of the item in the array 
 */
void *dare_pull(void *arrptr) {
    assert(arrptr);

    darray *head = DARE_GET_HEADER(arrptr, darray);

    size_t pull = WRAP(head->pull, head->size);
    void *point = arrptr + (pull * head->type_offset);

    head->pull = pull;
    head->pull++;
    return point; 
}

/* inserts item into array at specified position 
 *
 * vp_arrptr : the pointer to the value that contains the array | item : the 
 * variable that contains the item | pos : the position in which to insert the 
 * item | returns : (size_t) the position of where item was inserted 
 */
size_t dare_insert(void **vp_arrptr, void *item, size_t pos) {
    assert(vp_arrptr);
    assert(*vp_arrptr);

    void *arrptr = *vp_arrptr;
    darray *head = DARE_GET_HEADER(arrptr, darray); 

    if(IS_OVERLOADED(head->load, head->elements, head->size)) {
        *vp_arrptr = dare_expand(arrptr, head->expander);
        if(*vp_arrptr == NULL) return 0;
        arrptr = *vp_arrptr;
        head = DARE_GET_HEADER(arrptr, darray);
    }

    size_t n_pos = WRAP(pos, head->size);
    void *point = arrptr + (n_pos * head->type_offset);
    memcpy(point, item, head->type_offset);

    head->elements++;

    return n_pos;
}

/* gets an item at pos from the supplied array
 *
 * arrptr : the array to extract the item from | pos : the position in the array
 * to get the item | returns : (void *) the memory address of the item
 */
void *dare_get(void *arrptr, size_t pos) {
    assert(arrptr);

    darray *head = DARE_GET_HEADER(arrptr, darray);

    size_t n_pos = WRAP(pos, head->size);
    void *point = arrptr + (n_pos * head->type_offset);
    
    return point;
}

/* pops the newest inserted item from the array and returns it 
 *
 * arrptr : the array to use | returns : (void *) the memory address of the 
 * pointer
 */
void *dare_pop(void *arrptr) {
    assert(arrptr);

    darray *head = DARE_GET_HEADER(arrptr, darray);

    size_t n_pos = WRAP(head->push - 1, head->size);
    void *point = arrptr + (n_pos * head->type_offset);

    head->elements--;
    head->push--;

    return point;
}

/* removes the data at position copy data to dest if not NULL
 *
 * dare : the array structure to use | pos : the position in the array |
 * dest : the type to contain the data in before it is removed | return : void
 */
void *dare_remove(void *arrptr, size_t pos) {
    assert(arrptr);

    darray *head = DARE_GET_HEADER(arrptr, darray);

    size_t n_pos = WRAP(pos, head->size);
    void *point = arrptr + (n_pos * head->type_offset);

    head->elements--;

    return point;
}

/* Takes a pointer to an array and copys the specified array at pos in 
 * into the vp_arrptr increaseing array size if necessary
 *
 * vp_arrptr : pointer to value which hold array address | list : pointer to
 * array of values | pos : position to insert inside array | cp_size : size to 
 * copy over form list | returns : (size_t) the position of where items where 
 * inserted. */
size_t dare_insert_list(void **vp_arrptr, void *list, size_t pos, size_t cp_size) {
    assert(vp_arrptr);
    assert(*vp_arrptr);
    assert(list);

    void *arrptr = *vp_arrptr;
    darray *head = DARE_GET_HEADER(arrptr, darray);

    size_t n_pos = WRAP(pos, head->size);
    if(head->size < cp_size + n_pos) {
        *vp_arrptr = dare_resize(arrptr, (cp_size + head->size) * head->expander);
        if(*vp_arrptr == NULL) return 0;
        arrptr = *vp_arrptr;
        head = DARE_GET_HEADER(arrptr, darray);
    }

    void *point = arrptr + (n_pos * head->type_offset);
    memcpy(point, list, (cp_size * head->type_offset));

    head->elements += cp_size;

    return n_pos;
}

/* Copys data to the given list, pos specifying the position of the data in
 * arrptr and cp_size specifying length. 
 *
 * arrptr : the pointer to the array to copy from | list : the point be copied 
 * to | pos : where to start the copy | cp_size : length of the copy |
 * returns : (void). */
void dare_get_list(void *arrptr, void *list, size_t pos, size_t cp_size) {
    assert(arrptr);
    assert(list);

    darray *head = DARE_GET_HEADER(arrptr, darray);
    
    size_t n_pos = WRAP(pos, head->size);
    void *point = arrptr + (n_pos * head->type_offset);
    memcpy(list, point, cp_size * head->type_offset);
}

// the header always needs to be in front of the array the wrap macro is just
// there in the event we go out of bound however, we shouldn't really off 
// of wrap especially when resizing the array, so we need to always be up front

/* START OF STACK */

void *dare_stk_conf_init(darray_config *dare_conf) {
    if(dare_conf == NULL) return NULL;
    size_t n_sz_bytes = dare_conf->size * dare_conf->type_offset;
    
    d_stack *stk = DARE_REALLOC(NULL, sizeof(d_stack) + n_sz_bytes);
    if(stk == NULL) return NULL;

    stk->size = dare_conf->size;
    stk->type_offset = dare_conf->type_offset;
    stk->load = dare_conf->load;
    stk->expander = dare_conf->expander;

    stk->elements = 0;
    stk->head = 0;
    stk->tail = 0;

    memset(DARE_GET_ARRPTR(stk, d_stack), 0, n_sz_bytes);
    return DARE_GET_ARRPTR(stk, d_stack);
}

/* size : the size to set the array to | type_offset : how large each element in
 * the stack is going to be | load : the amount of elements that the stack can
 * handle before needing to reallocate | expander : the amount to expand the stack by.
 *
 * Description: similar to dare_stk_conf_init except it dosn't use a config
 * in order to initialize itself. Allocates a block of memory to be used by
 * the stack. */
void *dare_stk_init(size_t size, size_t type_offset, float load, double expander) {
    size_t n_sz_bytes = size * type_offset;
    d_stack *stk = DARE_REALLOC(NULL, sizeof(d_stack) + n_sz_bytes);
    if(stk == NULL) return NULL;

    stk->size = size;
    stk->type_offset = type_offset;
    stk->load = load;
    stk->expander = expander;

    stk->elements = 0;
    stk->head = 0;
    stk->tail = 0;

    memset(DARE_GET_ARRPTR(stk, d_stack), 0, n_sz_bytes);
    return DARE_GET_ARRPTR(stk, d_stack);
}

/* arrptr : the pointer to the array of the stack | returns : void
 * Description: This frees the stack in it's entirety. */
void dare_stk_deinit(void *arrptr) {
    if(arrptr == NULL) return;
    d_stack *stk_headptr = DARE_GET_HEADER(arrptr, d_stack);
    free(stk_headptr);
}

/* arrptr : the pointer to the array of the stack | expander : the decimal number
 * to expand the array by | returns : the new memory address to the array of the stack. */
void *dare_stk_expand(void *arrptr, double expander) {
    d_stack *stk_headptr = DARE_GET_HEADER(arrptr, d_stack);
    size_t old_sz = stk_headptr->size; 
    size_t new_sz = stk_headptr->size * expander;
   
    stk_headptr = DARE_ALLOC(stk_headptr, (new_sz * stk_headptr->type_offset) + sizeof(d_stack));
    if(stk_headptr == NULL) return NULL;

    stk_headptr->size = new_sz; 
    void *stk_arrptr = DARE_GET_ARRPTR(stk_headptr, d_stack);

//    if(old_sz < new_sz) {
//        void *point = stk_arrptr + (old_sz * stk_headptr->type_offset);
//        size_t set_len = (new_sz * stk_headptr->type_offset) - (old_sz * stk_headptr->type_offset);
//        memset(point, 0, set_len);
//    }

    return stk_arrptr;
}

// WIP
void *dare_stk_resize(void *stk, size_t size) {
    return 0;
}

/* arrptr : the pointer to the variable that holds the stacks array | item :
 * the pointer to the item to be copied into the stack | returns : -1 on error
 * or position of head.
 *
 * Description: pushes item onto the stack if error returns -1 if not returns 
 * position of the head. */
int dare_stk_push(void **arrptr, void *item) {
    void *stk_arrptr = *arrptr;
    d_stack *stk_headptr = DARE_GET_HEADER(stk_arrptr, d_stack);
    
    if(IS_OVERLOADED(stk_headptr->load, stk_headptr->elements, stk_headptr->size) 
       && stk_headptr->expander > 1.0) {
        *arrptr = dare_stk_expand(stk_arrptr, stk_headptr->expander); 
        if(*arrptr == NULL) return -1;

        stk_arrptr = *arrptr;
        stk_headptr = DARE_GET_HEADER(stk_arrptr, d_stack);
    } 

    if(stk_headptr->tail == stk_headptr->head && stk_headptr->elements != 0) {
        stk_headptr->head++;
    } else stk_headptr->elements++; 

    size_t new_pos = WRAP(stk_headptr->tail, stk_headptr->size); // ensure that where always in the array via wrapping.
    void *point = stk_arrptr + (new_pos * stk_headptr->type_offset); // move to point.
    memcpy(point, item, stk_headptr->type_offset); // copy data into array.

    stk_headptr->tail = new_pos + 1; // move to the next index.                                                                           
    return new_pos;
}

/* arrptr : the pointer to the array of the stack | returns the memory address 
 * of head.
 *
 * Description: returns the top of the stack as a memory address. */
void *dare_stk_peek(void *arrptr) {
    d_stack *stk_headptr = DARE_GET_HEADER(arrptr, d_stack);

    size_t new_pos = WRAP(stk_headptr->head, stk_headptr->size);
    void *point = arrptr + (new_pos * stk_headptr->type_offset);
    
    return point;
}

/* arrptr : the pointer to the array of the stack | returns : the memory address
 * of head or NULL on failure.
 *
 * Description: returns the memory address of head and moves it back by one*/
void *dare_stk_pop(void *arrptr) {     
    if(arrptr == NULL) return NULL; 
    d_stack *stk_headptr = DARE_GET_HEADER(arrptr, d_stack);

    size_t new_pos = WRAP(stk_headptr->head, stk_headptr->size);
    void *point = arrptr + (new_pos * stk_headptr->type_offset);
  
    if(stk_headptr->head != stk_headptr->tail) {
        stk_headptr->tail = new_pos - 1;
        stk_headptr->elements--;
    }

    return point;
}

/* START OF QUEUE */

void *dare_queue_init_conf(darray_config *conf) {
    if(conf == NULL) return NULL; 
    size_t size_bytes = conf->size * conf->type_offset;

    d_queue *queue = DARE_ALLOC(NULL, sizeof(d_queue) + size_bytes);
    if(queue == NULL) return NULL;
   
    queue->size = conf->size;
    queue->type_offset = conf->type_offset;
    queue->load = conf->load;
    queue->expander = conf->expander;

    queue->elements = 0;
    queue->head = 0;
    queue->tail = 0;

    return DARE_GET_ARRPTR(queue, d_queue);
}

void *dare_queue_init(size_t size, size_t type_offset, float load, double expander) {
    size_t size_bytes = size * type_offset;

    d_queue *queue = DARE_ALLOC(NULL, sizeof(d_queue) + size_bytes);
    if(queue == NULL) return NULL;
   
    queue->size = size;
    queue->type_offset = type_offset;
    queue->load = load;
    queue->expander = expander;

    queue->elements = 0;
    queue->head = 0;
    queue->tail = 0;

    return DARE_GET_ARRPTR(queue, d_queue);
}

signed int dare_queue_deinit(void *queue_arrptr) {
    if(queue_arrptr == NULL) return -1;
    free(DARE_GET_HEADER(queue_arrptr, d_queue));
    return 0;
}

void *dare_queue_expand(void *arrptr, size_t expander) {
    d_queue *queue_headptr = DARE_GET_HEADER(arrptr, d_queue);
    size_t old_sz = queue_headptr->size; 
    size_t new_sz = queue_headptr->size * expander;
   
    queue_headptr = DARE_ALLOC(queue_headptr, (new_sz * queue_headptr->type_offset) + sizeof(d_queue));
    if(queue_headptr == NULL) return NULL;

    queue_headptr->size = new_sz; 
    return DARE_GET_ARRPTR(queue_headptr, d_queue);
}

void *dare_queue_resize() {
    return NULL;
}

/* arrptr : the pointer to the variable that holds the address to the queue's array |
 * item : the data to be copied into the queue | returns : where the item was index or
 * -1 on failure. */
signed long int dare_queue_push(void **arrptr, void *item) {
    if(arrptr == NULL) return -1; 
    void *queue_arrptr = *arrptr;
    if(queue_arrptr == NULL) return -1;
    d_queue *queue_headptr = DARE_GET_HEADER(queue_arrptr, d_queue);
    if(queue_headptr == NULL) return -1;

    if(IS_OVERLOADED(queue_headptr->load, queue_headptr->elements, queue_headptr->size) && queue_headptr->expander > 1.0) {
        *arrptr = dare_queue_expand(queue_arrptr, queue_headptr->expander);
        if(*arrptr == NULL) return -1;

        queue_arrptr = *arrptr;
        queue_headptr = DARE_GET_HEADER(queue_arrptr, d_queue);
    }

    if(queue_headptr->tail == queue_headptr->head && queue_headptr->elements != 0) {
        queue_headptr->head++;
    } else queue_headptr->elements++;
    
    size_t new_pos = WRAP(queue_headptr->tail, queue_headptr->size);
    void *point = queue_arrptr + new_pos * queue_headptr->type_offset;
    memcpy(point, item, queue_headptr->type_offset);
    
    queue_headptr->tail = new_pos + 1;
    return new_pos;
}

/* arrptr : The pointer to the array of the queue | back : what face of the queue
 * to return front or back | returns : The front or the back of pointer of the queue. 
 *
 * Description: Returns the memory address of the front or back of the queue 
 * based on what true/false value back is provided. */
void *dare_queue_get_face(void *arrptr, bool back) {
    if(arrptr == NULL) return NULL;
    d_queue *queue_headptr = DARE_GET_HEADER(arrptr, d_queue);

    size_t new_pos;
    if(back == false) new_pos = WRAP(queue_headptr->head, queue_headptr->size);
    else new_pos = WRAP(queue_headptr->tail, queue_headptr->size);
    
    void *point = arrptr + new_pos * queue_headptr->type_offset; 
    return point;
}

/* arrptr : The pointer to the array of the queue | returns : the pointer to the
 * front of the queue or NULL on failure
 *
 * Description: Returns the address that head points to and increments by one 
 * returning NULL in the event that arrptr or queue_headptr is NULL. */
void *dare_queue_consume(void *arrptr) {
    if(arrptr == NULL) return NULL;
    d_queue *queue_headptr = DARE_GET_HEADER(arrptr, d_queue);
    if(queue_headptr == NULL) return NULL;

    size_t new_pos = WRAP(queue_headptr->head, queue_headptr->size);
    void *point = arrptr + new_pos * queue_headptr->type_offset;

    if(queue_headptr->head != queue_headptr->tail) {
        queue_headptr->head = new_pos + 1;
        queue_headptr->elements--; 
    }

    return point;
}

#undef DARE_ALLOC
#undef DARE_REALLOC
#undef DARE_FREE
