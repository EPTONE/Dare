#include <stdio.h>
#include <stdlib.h>
#include <sys/random.h>

#include "dare.h"
#include <valgrind/valgrind.h>

darray_config int_conf = {sizeof(int), 25, 0.75, 2.0};

int main(int argc, char **argv) {
    int *arr = dare_init(&int_conf);

    int rand_data = 0;
    for(size_t i = 0; i < 4096; i++) {
        getrandom(&rand_data, sizeof(int), GRND_RANDOM | GRND_INSECURE);
        dare_push(PCONV(void)&arr, &rand_data);      
    }

    darray *arr_head = DARE_GET_HEADER(arr);

    for(size_t i = 0; i < arr_head->size; i++) {
        printf("%d\n", HCONV(int)dare_get(arr, i));
    }

    printf("\n%ld <- HEADER SIZE", arr_head->size);
    VALGRIND_PRINTF("\n%ld <- HEADER SIZE", arr_head->size);
    dare_deinit(arr);
}
