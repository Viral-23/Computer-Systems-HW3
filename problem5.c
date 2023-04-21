#include <pthread.h>
#include <stdio.h>
#include <signal.h>

#define NUM_THREADS 3
void *compute_sum(void *threadstruct);

struct thread_data {
    pthread_t id;
    int sum;
    int threadNum;
};

struct thread_data thread[NUM_THREADS];

void sig_handler(int sigNum) {
    printf("Signal %d received in thread\n", sigNum);
    sigset_t mask;
    sigemptyset(&mask);
    if (sigNum == SIGFPE) {
        sigaddset(&mask, SIGSTOP);
        sigaddset(&mask, SIGSEGV);
    }
    else if (sigNum == SIGSTOP) {
        sigaddset(&mask, SIGFPE);
        sigaddset(&mask, SIGSEGV);
    }
    else if (sigNum == SIGSEGV) {
        sigaddset(&mask, SIGFPE);
        sigaddset(&mask, SIGSTOP);
    }
    sigprocmask(SIG_BLOCK, &mask, NULL);
    signal(sigNum, sig_handler);

}

void *compute_sum(void *threadstruct) {
    signal(SIGFPE, sig_handler);
    signal(SIGSTOP, sig_handler);
    signal(SIGSEGV, sig_handler);

    struct thread_data *data = (struct thread_data*) threadstruct;
    long id = (long) data->id;
    for (int i = 1; i < 1000 * id; i++) {
        data->sum += i;
    }

    pthread_exit(0);
}

int main() {
    signal(SIGFPE, SIG_IGN);
    signal(SIGSTOP, SIG_IGN);
    signal(SIGSEGV, SIG_IGN);

    int sum = 0;
    for (int i = 0; i < NUM_THREADS; i++) {
        thread[i].sum = 0;
        thread[i].threadNum = i;
        pthread_create(&thread[i].id, NULL, compute_sum, &thread[i]);
    }

    int totalSum = 0;
    for (int i = 0; i < NUM_THREADS; i++) {
        void *status;
        pthread_join(thread[i].id, &status);
        totalSum += thread[i].sum;
    }
    printf("Total sum = %d\n", totalSum);
    
    signal(SIGFPE, SIG_DFL);
    signal(SIGSTOP, SIG_DFL);
    signal(SIGSEGV, SIG_DFL);

    return 0;
}

