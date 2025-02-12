#include <stdio.h>
#include <stdlib.h>
#include "bank_interface.h"

typedef struct account {
    unsigned int account;
    int balance;
    struct account *next;
} account_t;

typedef struct ledger {
    account_t *head;
    account_t *tail;
} ledger_t;

int create_account(unsigned int account, void *ledger) {
    ledger_t *ledger_ptr = (ledger_t *)ledger;

    // Check if account already exists
    account_t *current = ledger_ptr->head;
    while (current != NULL) {
        if (current->account == account) {
            return -1; // Account already exists
        }
        current = current->next;
    }

    // Create new account
    account_t *new_account = (account_t *)malloc(sizeof(account_t));
    if (new_account == NULL) {
        perror("Failed to allocate memory for new account");
        return -1;
    }
    new_account->account = account;
    new_account->balance = 0; // Initial balance
    new_account->next = NULL;

    // Add the new account to the ledger
    if (ledger_ptr->head == NULL) {
        ledger_ptr->head = new_account;
        ledger_ptr->tail = new_account;
    } else {
        ledger_ptr->tail->next = new_account;
        ledger_ptr->tail = new_account;
    }

    return 0; // Success
}

void list_accounts(void *ledger) {
    ledger_t *ledger_ptr = (ledger_t *)ledger;

    account_t *current = ledger_ptr->head;
    while (current != NULL) {
        printf("Account: %u, Balance: %d\n", current->account, current->balance);
        current = current->next;
    }
}

int modify_balance(unsigned int account, int amount, void *ledger) {
    ledger_t *ledger_ptr = (ledger_t *)ledger;

    // Find the account
    account_t *current = ledger_ptr->head;
    while (current != NULL) {
        if (current->account == account) {
            current->balance += amount; // Modify balance
            return current->balance;   // Return updated balance
        }
        current = current->next;
    }

    return -1; // Account not found
}

int main() {
    ledger_t ledger;
    ledger.head = NULL;
    ledger.tail = NULL;

    // Test the functions
    create_account(0, &ledger);
    create_account(1, &ledger);
    create_account(2, &ledger);

    list_accounts(&ledger);
    
    modify_balance(1, 10, &ledger);
    modify_balance(0, -100, &ledger);
    modify_balance(123, -100, &ledger);

    list_accounts(&ledger);

    return 0;
}
