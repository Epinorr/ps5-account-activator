/*
 * EPINOR PS5 Account Activator
 *
 * Derived from the AccountActivator implementation in etaHEN and the
 * registry constants/algorithm used by PS5Dev/OffAct.
 */
#include "account_activator.h"

#include <stddef.h>
#include <stdint.h>
#include <string.h>

extern int sceUserServiceGetForegroundUser(int *user_id);
extern int sceUserServiceGetUserName(int user_id, char *username, size_t size);

extern int sceRegMgrGetStr(int entity, char *value, size_t size);
extern int sceRegMgrGetInt(int entity, int *value);
extern int sceRegMgrGetBin(int entity, void *value, size_t size);
extern int sceRegMgrSetInt(int entity, int value);
extern int sceRegMgrSetBin(int entity, const void *value, size_t size);
extern int sceRegMgrSetStr(int entity, const char *value, size_t size);

static int get_entity_number(int account_number, int base_entity, int fallback_entity)
{
    if (account_number < 1 || account_number > MAX_ACCOUNT_SLOTS)
        return fallback_entity;

    return (account_number - 1) * 65536 + base_entity;
}

static int find_account_slot(const char *username)
{
    char registry_username[USERNAME_MAX];

    if (username == NULL || username[0] == '\0')
        return -1;

    for (int account_number = 1; account_number <= MAX_ACCOUNT_SLOTS; ++account_number) {
        const int entity = get_entity_number(account_number,
                                             USERNAME_ENTITY_NUMBER,
                                             USERNAME_ENTITY_NUMBER_2);
        memset(registry_username, 0, sizeof(registry_username));

        const int ret = sceRegMgrGetStr(entity,
                                        registry_username,
                                        sizeof(registry_username));
        if (ret != 0)
            continue;

        registry_username[sizeof(registry_username) - 1] = '\0';
        if (strcmp(username, registry_username) == 0)
            return account_number;
    }

    return -1;
}

static uint64_t generate_account_id(const char *username)
{
    uint64_t base = 0x5EAF00DULL / 0xCA7F00DULL;

    if (username != NULL && *username != '\0') {
        do {
            base = 0x100000001B3ULL *
                   (base ^ (uint8_t)*username++);
        } while (*username != '\0');
    }

    return base;
}

int account_activator_run(char username[USERNAME_MAX], int *changed, int *error_code,
                           int *account_number_out, uint64_t *account_id_out)
{
    int user_id = -1;
    int account_number;
    int ret;
    uint64_t account_id = 0;
    uint64_t old_account_id = 0;
    char old_account_type[ACCOUNT_TYPE_MAX] = {0};
    int old_flags = 0;
    int have_old_type = 0;
    int have_old_flags = 0;

    if (username == NULL || changed == NULL || error_code == NULL ||
        account_number_out == NULL || account_id_out == NULL) {
        return ACCOUNT_ACTIVATOR_ERR_ARGUMENT;
    }

    username[0] = '\0';
    *changed = 0;
    *error_code = 0;
    *account_number_out = -1;
    *account_id_out = 0;

    ret = sceUserServiceGetForegroundUser(&user_id);
    if (ret != 0) {
        *error_code = ret;
        return ret;
    }

    ret = sceUserServiceGetUserName(user_id, username, USERNAME_MAX);
    if (ret != 0) {
        *error_code = ACCOUNT_ACTIVATOR_ERR_USERNAME;
        return ACCOUNT_ACTIVATOR_ERR_USERNAME;
    }
    username[USERNAME_MAX - 1] = '\0';

    account_number = find_account_slot(username);
    if (account_number < 0) {
        *error_code = ACCOUNT_ACTIVATOR_ERR_USER_REGISTRY;
        return ACCOUNT_ACTIVATOR_ERR_USER_REGISTRY;
    }

    const int id_entity = get_entity_number(account_number,
                                            ACCOUNT_ID_ENTITY_NUMBER,
                                            ACCOUNT_ID_ENTITY_NUMBER_2);
    const int type_entity = get_entity_number(account_number,
                                              ACCOUNT_TYPE_ENTITY_NUMBER,
                                              ACCOUNT_TYPE_ENTITY_NUMBER_2);
    const int flags_entity = get_entity_number(account_number,
                                               ACCOUNT_FLAGS_ENTITY_NUMBER,
                                               ACCOUNT_FLAGS_ENTITY_NUMBER_2);

    ret = sceRegMgrGetBin(id_entity, &old_account_id, sizeof(old_account_id));
    if (ret != 0) {
        *error_code = ret;
        return ret;
    }

    if (old_account_id != 0) {
        *account_number_out = account_number;
        *account_id_out = old_account_id;
        return ACCOUNT_ACTIVATOR_OK;
    }

    /* Read all values that may need restoring before the first write. */
    ret = sceRegMgrGetStr(type_entity, old_account_type, sizeof(old_account_type));
    if (ret != 0) {
        *error_code = ret;
        return ret;
    }
    have_old_type = 1;

    ret = sceRegMgrGetInt(flags_entity, &old_flags);
    if (ret != 0) {
        *error_code = ret;
        return ret;
    }
    have_old_flags = 1;

    account_id = generate_account_id(username);
    if (account_id == 0) {
        *error_code = ACCOUNT_ACTIVATOR_ERR_GENERATED_ID;
        return ACCOUNT_ACTIVATOR_ERR_GENERATED_ID;
    }

    static const char account_type[ACCOUNT_TYPE_MAX] = "np";

    ret = sceRegMgrSetBin(id_entity, &account_id, sizeof(account_id));
    if (ret != 0) {
        *error_code = ret;
        return ret;
    }

    ret = sceRegMgrSetStr(type_entity, account_type, sizeof(account_type));
    if (ret != 0) {
        if (have_old_type)
            (void)sceRegMgrSetStr(type_entity, old_account_type, sizeof(old_account_type));
        (void)sceRegMgrSetBin(id_entity, &old_account_id, sizeof(old_account_id));
        *error_code = ret;
        return ret;
    }

    ret = sceRegMgrSetInt(flags_entity, ACCOUNT_FLAGS_VALUE);
    if (ret != 0) {
        if (have_old_flags)
            (void)sceRegMgrSetInt(flags_entity, old_flags);
        if (have_old_type)
            (void)sceRegMgrSetStr(type_entity, old_account_type, sizeof(old_account_type));
        (void)sceRegMgrSetBin(id_entity, &old_account_id, sizeof(old_account_id));
        *error_code = ret;
        return ret;
    }

    *changed = 1;
    *account_number_out = account_number;
    *account_id_out = account_id;
    return ACCOUNT_ACTIVATOR_OK;
}