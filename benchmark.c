#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/resource.h>
#include "bank_interface.h"

typedef struct {
    int num_threads;
    int num_accounts;
    int *accounts;
    int num_transactions;
    void *ledger;
} driver_t;

void *thread_entry_point(void * arg) {
    //printf("started thread %p\n", arg);
    driver_t *driver = (driver_t *) arg;
    void *ledger = driver->ledger;

    for (int i = 0; i < driver->num_accounts; i++) {
        create_account(driver->accounts[i], ledger);        
    }
    //list_accounts(ledger);
    
    time_t t;
    srand((unsigned) time(&t));
    for (int i = 0; i < driver->num_transactions; i++) {
        int account_index = rand() % driver->num_accounts;
        int account = driver->accounts[account_index];

        int amount = rand() % 100 - 50;
        int r = modify_balance(account, amount, ledger);
        // printf("%d", r);
    }

    // printf("done\n");
    return NULL;
}

double benchmark_driver(int num_threads, int num_transactions, int num_accounts, void *ledger) {
    struct rusage start;
    getrusage(RUSAGE_SELF, &start);

    driver_t driver;
    driver.num_accounts = num_accounts;
    driver.accounts = (int *) malloc(driver.num_accounts * sizeof(int));
    for (int i = 0; i < driver.num_accounts; i++) {
        driver.accounts[i] = i;
    }
    driver.num_transactions = num_transactions;
    driver.ledger = ledger;


    pthread_t threads[num_threads];
    for (int i = 0; i < num_threads; i++) {
        int r = pthread_create(&threads[i], NULL, thread_entry_point, &driver);
        if (r != 0) {
            printf("Failure to create thread\n");
            exit(-1);
        } 
    } 

    // list_accounts(&ledger);

    for (int i = 0; i < num_threads; i++) {
        int r = pthread_join(threads[i], NULL);
        if (r != 0) {
            printf("Failure to join on thread\n");
            exit(-1);
        }
    }

    struct rusage end;
    getrusage(RUSAGE_SELF, &end);

    struct timeval delta;
    delta.tv_sec = end.ru_stime.tv_sec - start.ru_stime.tv_sec;
    delta.tv_usec = end.ru_stime.tv_usec - start.ru_stime.tv_usec;

    // printf("s=%ld us=%ld\n", delta.tv_sec, delta.tv_usec);
    double time_taken = delta.tv_sec + (delta.tv_usec * 1E-6);
    return time_taken;
}
