#include "dare.h"
#include <stdio.h>
#include <valgrind/memcheck.h>

darray_config carr_conf = {.type_offset = sizeof(char), .size = 35, .load = 0.05, .expander = 1.5};

int main(int argc, char **argv) {
    char *c_arr = dare_init(&carr_conf);

    char c = 'c';
    dare_insert(PCONV(void)&c_arr, &c, 0);
    c = 'a';
    dare_insert(PCONV(void)&c_arr, &c, 1);
    c = 't';
    dare_insert(PCONV(void)&c_arr, &c, 2);

    c = 'p';
    dare_insert(PCONV(void)&c_arr, &c, 10);
    dare_insert(PCONV(void)&c_arr, &c, 39);

    darray *c_arr_head = DARE_GET_HEADER(c_arr);
    for(size_t i = 0; i < c_arr_head->size; i++) {
        printf("%c", HCONV(char)dare_get(c_arr, i));
    }
    
    printf("%ld <- ARRAY_SIZE\n", c_arr_head->size);
    dare_deinit(c_arr);
    return 0;
}
