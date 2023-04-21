#include <pthread.h>
#include <stdio.h>
#include <signal.h>

#define NUM_THREADS 3
void *compute_sum(void *s);

struct thread_data {
    pthread_t id;
    int sum;
};

struct thread_data thread[NUM_THREADS];

void *compute_sum(void *s) {
    struct thread_data *data = (struct thread_data*) thread;
    int id = thread->id;
    int sum = thread->sum;

    for (int i = 1; i < 1000 * id; i++) {
        sum += i;
    }

    pthread_exit(0);
}

int main() {
    signal(SIGFPE, SIG_IGN);
    signal(SIGSTOP, SIG_IGN);
    signal(SIGSEGV, SIG_IGN);

    int sum = 0;
    for (int i = 0; i < NUM_THREADS; i++) {
        if (i == 0)
            thread[i].id = pthread_create(&thread[i].id, thread_sigfpe, compute_sum, &thread[i]);
        else if (i == 1)
            thread[i].id = pthread_create(&thread[i].id, thread_sigstop, compute_sum, &thread[i]);
        else
            thread[i].id = pthread_create(&thread[i].id, thread_sigsegv, compute_sum, &thread[i]);
    }

    int totalSum = 0;
    for (int i = 0; i < NUM_THREADS; i++) {
        void *status;
        pthread_join(thread[i].id, &status);
    }
    printf("Total sum = %d\n", totalSum);
    
    signal(SIGFPE, SIG_DFL);
    signal(SIGSTOP, SIG_DFL);
    signal(SIGSEGV, SIG_DFL);

    return 0;
}

