#include "dare.h"
#include "stdio.h"

int main(int argc, char **argv) {
   char *id_stk = dare_stk_init(1000, sizeof(char), 0.75f, 1.5);

   char id = 'c';
   for(int i = 0; i < 1000; i++) {
        dare_stk_push(PCONV(void)&id_stk, &id);
    }
   
   char c = HCONV(char)dare_stk_pull(id_stk);

   for(int i = 0; i < 1000; i++) {
        char c = HCONV(char)dare_stk_pop(id_stk);
        printf("%c", c);
   }
    
   printf("\n%ld", DARE_GET_HEADER(id_stk, d_stack)->head);
   dare_stk_deinit(id_stk);
   return 0;
}
