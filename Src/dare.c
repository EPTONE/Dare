#include "dare.h" 
#include <valgrind/memcheck.h>

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

    memset(DARE_GET_ARRPTR(head), 0, head->size * head->type_offset); 
    return DARE_GET_ARRPTR(head);
}

/* frees the array from memory
 * 
 * arrptr : the array to be dealloced | returns : (void)
 */
void dare_deinit(void *arrptr) {
    assert(arrptr);

    darray *head = DARE_GET_HEADER(arrptr); 
    if(head) DARE_FREE(head);
}

/* resizes the array by size * expander
 *
 * arrptr : the array to expand | expander : the multiplier to expand the array
 * by | returns : (void *) the address to the new array
 */
void *dare_expand(void *arrptr, double expander) {
    assert(arrptr);
    darray *head = DARE_GET_HEADER(arrptr);
    size_t old_sz = head->size;

    size_t n_sz = head->size * expander;
    head = DARE_ALLOC(head, (n_sz * head->type_offset) + sizeof(darray));
    if(head == NULL) assert(head);
    void *n_arrptr = DARE_GET_ARRPTR(head); 
    head->size = n_sz;

    if(old_sz < n_sz) {
        void *point = n_arrptr + (old_sz * head->type_offset);
        size_t set_len = (n_sz * head->type_offset) - (old_sz * head->type_offset);
        memset(point, 0, set_len);
    }
  
    return DARE_GET_ARRPTR(head);
}

/*  resizes array to the size of sz
 *
 *  arrptr : the array to resize | sz : what size the array should be set to |
 *  returns : (void *) the memory address to the new array
 */
void *dare_resize(void *arrptr, size_t sz) {
    assert(arrptr);

    darray *head = DARE_GET_HEADER(arrptr);
    size_t old_sz = head->size;

    size_t n_sz = sz * head->type_offset;
    head = DARE_REALLOC(head, sz + sizeof(darray));
    if(head == NULL) assert(head);
    void *n_arrptr = DARE_GET_ARRPTR(head); 
    head->size = sz;

    if(old_sz < n_sz) {
        void *point = n_arrptr + (old_sz * head->type_offset);
        size_t set_len = (n_sz * head->type_offset) - (old_sz * head->type_offset);
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
    darray *src_head = DARE_GET_HEADER(src);
    darray *dst_head = DARE_GET_HEADER(dst);

    offset = WRAP(offset, dst_head->size);
    
    while(src_head->size + offset > dst_head->size) {
       *vp_dst = dare_expand(dst, dst_head->expander);
        if(*vp_dst == NULL) return;
        dst = *vp_dst;
        dst_head = DARE_GET_HEADER(dst);
    }

    void *point = dst + (offset * dst_head->type_offset);
    mempcpy(point, src, src_head->size * src_head->type_offset);

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
    darray *head = DARE_GET_HEADER(arrptr);

    while(IS_OVERLOADED(head->load, head->elements, head->size)) {
        *vp_arrptr = dare_expand(arrptr, head->expander);
        if(*vp_arrptr == NULL) return 0;
        arrptr = *vp_arrptr;
        head = DARE_GET_HEADER(arrptr);
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

    darray *head = DARE_GET_HEADER(arrptr);

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
    darray *head = DARE_GET_HEADER(arrptr); 

    while(IS_OVERLOADED(head->load, head->elements, head->size)) {
        *vp_arrptr = dare_expand(arrptr, head->expander);
        if(*vp_arrptr == NULL) return 0;
        arrptr = *vp_arrptr;
        head = DARE_GET_HEADER(arrptr);
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

    darray *head = DARE_GET_HEADER(arrptr);

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

    darray *head = DARE_GET_HEADER(arrptr);

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

    darray *head = DARE_GET_HEADER(arrptr);

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
    darray *head = DARE_GET_HEADER(arrptr);

    size_t n_pos = WRAP(pos, head->size);
    if(cp_size + n_pos > head->size) {
        *vp_arrptr = dare_resize(arrptr, (cp_size + head->size) * head->expander);
        if(*vp_arrptr) return 0;
        arrptr = *vp_arrptr;
        head = DARE_GET_HEADER(arrptr);
    }
    
    void *point = arrptr + (n_pos * head->type_offset);
    memcpy(point, list, cp_size * head->type_offset);

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

    darray *head = DARE_GET_HEADER(arrptr);
    
    size_t n_pos = WRAP(pos, head->size);
    void *point = arrptr + (n_pos * head->type_offset);
    memcpy(list, point, cp_size * head->type_offset);
}

