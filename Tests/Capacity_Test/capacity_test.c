#include <stdio.h>
#include <sys/random.h>

#include "dare.h"

darray_config int_conf = {sizeof(int), 25, 0.75, 2.0};

int main(int argc, char **argv) {
    int *arr = dare_init(&int_conf);

    int rand_data = 0;
    for(size_t i = 0; i < 65; i++) {
        getrandom(&rand_data, sizeof(int), GRND_RANDOM | GRND_INSECURE);
        DARE_PUSH(arr, rand_data);      
        darray *arr_head = DARE_GET_HEADER(arr);
    }
   
    /*
    for(size_t i = 0; i < arr_head->size; i++) {
        int val;
        DARE_HGET(arr, val, i);
        printf("%d\n", val);
    }*/

    DARE_DEINIT(arr);
}
