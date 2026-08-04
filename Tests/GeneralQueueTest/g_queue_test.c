#include "dare.h"
#include <stdio.h>
#include <string.h>

typedef struct job {
    int priority;
    char name[8];
} job;

int main(int argc, char **argv) {
    job *job_queue = dare_queue_init(25, sizeof(job), 0.75f, 1.5);

    job j = {5, "DESKJOB"};
    dare_queue_push(PCONV(void)&job_queue, &j);

    strcpy(j.name, "NEWJOB");
    dare_queue_push(PCONV(void)&job_queue, &j);

    for(int i = 0; i < 2; i++) {
        job *active_job = dare_queue_consume(job_queue);
        printf("|Assignment: %s | Priority: %d\n", active_job->name, active_job->priority);
    }

    printf("%ld", DARE_GET_ELEMENTS(job_queue, d_queue));

    dare_queue_deinit(job_queue);
    return 0;
}
