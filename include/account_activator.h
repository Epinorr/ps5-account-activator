/*
 * EPINOR PS5 Account Activator
 *
 * Based on the account activation logic in etaHEN / AccountActivator
 * and PS5Dev / OffAct. Licensed under GPL-3.0-or-later.
 */
#pragma once

#include <stdint.h>

#define USERNAME_ENTITY_NUMBER       0x7800200
#define USERNAME_ENTITY_NUMBER_2     0x7940200
#define ACCOUNT_ID_ENTITY_NUMBER     0x7800500
#define ACCOUNT_ID_ENTITY_NUMBER_2   0x7940500
#define ACCOUNT_TYPE_ENTITY_NUMBER   0x780b007
#define ACCOUNT_TYPE_ENTITY_NUMBER_2 0x794b007
#define ACCOUNT_FLAGS_ENTITY_NUMBER  0x7800800
#define ACCOUNT_FLAGS_ENTITY_NUMBER_2 0x7940800

#define ACCOUNT_TYPE_MAX 17
#define ACCOUNT_FLAGS_VALUE 4098
#define MAX_ACCOUNT_SLOTS 16
#define USERNAME_MAX 100

enum account_activator_error {
    ACCOUNT_ACTIVATOR_OK = 0,
    ACCOUNT_ACTIVATOR_ERR_ARGUMENT = -1000,
    ACCOUNT_ACTIVATOR_ERR_USERNAME = -1001,
    ACCOUNT_ACTIVATOR_ERR_USER_REGISTRY = -1002,
    ACCOUNT_ACTIVATOR_ERR_ACCOUNT_ID = -1003,
    ACCOUNT_ACTIVATOR_ERR_GENERATED_ID = -1004,
};

/* Returns 0 on success; changed is 1 only if registry values were written. */
int account_activator_run(char username[USERNAME_MAX], int *changed, int *error_code,
                          int *account_number, uint64_t *account_id);