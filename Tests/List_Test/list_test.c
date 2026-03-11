#include "dare.h"
#include <stdio.h>
#include <sys/random.h>

darray_config arr_conf = {.type_offset = sizeof(int), .size = 5, .load = 0.75, .expander = 1.5};

int main(int argc, char **argv) {
    int *darr = dare_init(&arr_conf);
    int buff[15] = {0};
    
    getrandom(buff, sizeof(int) * 15, GRND_RANDOM | GRND_INSECURE);

    dare_insert_list(PCONV(void)&darr, buff, 0, 15);
    int buff2[15] = {0};

    dare_get_list(darr, buff2, 0, 15); 
    for(size_t i = 0; i < 15; i++) {
        printf("%d\n", buff2[i]);
    }
    dare_deinit(darr);
}
