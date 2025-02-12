#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "benchmark.h"

typedef struct account {
    unsigned int account;
    int balance;
    struct account *next;
    pthread_mutex_t lock;
} account_t;

typedef struct ledger {
    account_t *head;
    account_t *tail;
    pthread_mutex_t lock; // Global lock for the ledger
} ledger_t;

void run_benchmark(const char *filename, ledger_t *ledger) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        perror("Failed to open file for writing");
        exit(EXIT_FAILURE);
    }

    fprintf(fp, "Threads,Accounts,Time (s)\n");

    int threads[] = {1, 10, 100, 1000};
    int accounts[] = {1, 10, 100};

    for (int i = 0; i < 4; i++) { // Iterate over thread counts
        for (int j = 0; j < 3; j++) { // Iterate over account counts
            double time_taken = benchmark_driver(threads[i], 10000, accounts[j], ledger); // Use 0 to indicate global lock
            fprintf(fp, "%d,%d,%f\n", threads[i], accounts[j], time_taken);
        }
    }

    fclose(fp);
    printf("Benchmark results written to %s\n", filename);
}

void create_account(ledger_t *l, unsigned int account) {
    pthread_mutex_lock(&l->lock);

    account_t *current = l->head;
    while (current != NULL) {
        if (current->account == account) {
            printf("Account %u already exists.\n", account);
            pthread_mutex_unlock(&l->lock);
            return;
        }
        current = current->next;
    }

    account_t *new_account = (account_t *)malloc(sizeof(account_t));
    new_account->account = account;
    new_account->balance = 0;
    new_account->next = NULL;

    if (l->head == NULL) {
        l->head = new_account;
        l->tail = new_account;
    } else {
        l->tail->next = new_account;
        l->tail = new_account;
    }

    pthread_mutex_unlock(&l->lock);
    printf("Account %u created.\n", account);
}

void list_accounts(ledger_t *l) {
    pthread_mutex_lock(&l->lock);

    account_t *current = l->head;
    while (current != NULL) {
        printf("Account: %u, Balance: %d\n", current->account, current->balance);
        current = current->next;
    }

    pthread_mutex_unlock(&l->lock);
}

void modify_balance(ledger_t *l, unsigned int account, int amount) {
    pthread_mutex_lock(&l->lock);

    account_t *current = l->head;
    while (current != NULL) {
        if (current->account == account) {
            current->balance += amount;
            printf("Account %u new balance: %d\n", account, current->balance);
            pthread_mutex_unlock(&l->lock);
            return;
        }
        current = current->next;
    }

    pthread_mutex_unlock(&l->lock);
    printf("Account %u not found.\n", account);
}

int main() {
    ledger_t ledger;
    ledger.head = NULL;
    ledger.tail = NULL;
    pthread_mutex_init(&ledger.lock, NULL);

    int choice, account, amount;

    while (1) {
        printf("\nOptions:\n");
        printf("1. Create account\n");
        printf("2. List accounts\n");
        printf("3. Modify balance\n");
        printf("4. Run benchmark\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("Enter account number: ");
                scanf("%d", &account);
                create_account(&ledger, account);
                break;

            case 2:
                list_accounts(&ledger);
                break;

            case 3:
                printf("Enter account number: ");
                scanf("%d", &account);
                printf("Enter amount to modify: ");
                scanf("%d", &amount);
                modify_balance(&ledger, account, amount);
                break;

            case 4:
                run_benchmark("results_global_lock.csv", &ledger);

                break;

            case 5:
                printf("Exiting...\n");
                return 0;

            default:
                printf("Invalid choice.\n");
        }
    }

    return 0;
}
