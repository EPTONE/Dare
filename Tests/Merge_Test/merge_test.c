#include "dare.h"
#include <stdio.h>
#include <stdint.h>
#include <sys/random.h>

darray_config arr16_conf = {.type_offset = sizeof(uint16_t), 
                            .size = 15, 
                            .load = 0.85, 
                            .expander = 1.5};

int main(int argc, char **argv) {

    uint16_t *arr1 = dare_init(&arr16_conf);
    uint16_t *arr2 = dare_init(&arr16_conf);

    for(size_t i = 0; i < 655; i++) {
        uint16_t v_insert1, v_insert2;
        getrandom(&v_insert1, sizeof(uint16_t), GRND_RANDOM | GRND_INSECURE);
        getrandom(&v_insert2, sizeof(uint16_t), GRND_RANDOM | GRND_INSECURE);

        dare_push(PCONV(void)&arr1, &v_insert1);
        dare_push(PCONV(void)&arr2, &v_insert2); 
    }

    darray *head2 = DARE_GET_HEADER(arr2);
    dare_merge(arr1, PCONV(void)&arr2, head2->push);

    for(size_t i = 0; i < (DARE_GET_HEADER(arr2))->elements; i++) {
        printf("%d\n", HCONV(uint16_t)dare_get(arr2, i));
    }

    printf("arr1->size: %ld\n", (DARE_GET_HEADER(arr1))->size);
    printf("arr2->size: %ld\n", (DARE_GET_HEADER(arr2))->size);

    dare_deinit(arr1);
    dare_deinit(arr2);
    return 0;
}
