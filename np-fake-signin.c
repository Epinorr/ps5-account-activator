/*
 * EPINOR Combined PS5 Payload: Account Activator + NP Fake Signin
 *
 * Based on earthonion/np-fake-signin and etaHEN / OffAct
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "account_activator.h"
#include "notification.h"
#include "hmac_md5.h"

/* Generated headers from upstream templates */
#include "auth_dat.h"
#include "config_dat.h"

/* User & System Service declarations */
typedef struct { uint32_t priority; } OrbisUserServiceInitializeParams;

extern int32_t sceUserServiceInitialize(OrbisUserServiceInitializeParams *params);
extern int32_t sceUserServiceTerminate(void);
extern int32_t sceUserServiceGetForegroundUser(int32_t *userId);
extern int32_t sceSystemServiceParamGetInt(int32_t paramId, int32_t *value);

/* Registry manager declarations */
extern int32_t sceRegMgrSetInt(uint32_t key, int32_t value);
extern int32_t sceRegMgrSetStr(uint32_t key, const char *value, size_t size);
extern int32_t sceRegMgrSetBin(uint32_t key, const void *value, size_t size);
extern int32_t sceRegMgrGetStr(uint32_t key, char *value, size_t size);
extern int32_t sceRegMgrGetInt(uint32_t key, int32_t *value);
extern int32_t sceRegMgrGetBin(uint32_t key, void *value, size_t size);

/* Registry keys from config.dat */
#define REG_KEY_USERNAME      125829632   /* offset 0x04, str 17 */
#define REG_KEY_ACCOUNT_ID    125830400   /* offset 0x100, bin 8 */
#define REG_KEY_EMAIL         125830656   /* offset 0x108, str 65 */
#define REG_KEY_NP_ENV        125874183   /* offset 0x177, str 17 */
#define REG_KEY_ONLINE_ID     125874188   /* offset 0x1AD, str 17 */
#define REG_KEY_COUNTRY       125874190   /* offset 0x1BE, str 3 */
#define REG_KEY_LANGUAGE      125874191   /* offset 0x1C1, str 6 */
#define REG_KEY_LOCALE        125874192   /* offset 0x1C7, str 36 */
#define REG_KEY_FLAG_1        125830144   /* offset 0x48, int */
#define REG_KEY_FLAG_2        125831168   /* offset 0x50, int */
#define REG_KEY_FLAG_3        125831424   /* offset 0x4C, int */
#define REG_KEY_FLAG_5C       125832960   /* offset 0x5C, int */
#define REG_KEY_FIELD_1F4     125874194   /* offset 0x1F4, int */
#define REG_KEY_SIGNIN_FLAG   125874185   /* offset 0x1F8, int - CRITICAL */
#define REG_KEY_FIELD_1FC     125874186   /* offset 0x1FC, int */
#define REG_KEY_NP_0xA4       125830912   /* offset 0xA4, int */
#define REG_KEY_NP_0xB4       125831936   /* offset 0xB4, int */
#define REG_KEY_NP_0xD0       125832704   /* offset 0xD0, int */
#define REG_KEY_NP_0xD4       125882625   /* offset 0xD4, int */
#define REG_KEY_NP_0xDC       125854723   /* offset 0xDC, int */
#define REG_KEY_NP_0xF4       125833216   /* offset 0xF4, int */
#define REG_KEY_EXT_0x1100    125874189   /* offset 0x1100, str 65 */
#define REG_KEY_EXT_0x1141    125874193   /* offset 0x1141, str 11 */
#define REG_KEY_EXT_0x114C    125874195   /* offset 0x114C, str 65 */

/* Runtime buffers */
static unsigned char cfg[8192];
static unsigned char acct[224];

/* HMAC-MD5 key for dat files */
static const uint8_t dat_hmac_key[16] = {
    0x28, 0x63, 0x2F, 0xA9, 0x6C, 0xCC, 0x59, 0x5E,
    0x20, 0x1F, 0x20, 0xCD, 0x77, 0xA1, 0x84, 0x45
};

static void patch_str(unsigned char *buf, int offset, const char *str, int max_len) {
    memset(buf + offset, 0, max_len);
    size_t len = strlen(str);
    if (len >= (size_t)max_len) len = max_len - 1;
    memcpy(buf + offset, str, len);
}

static void build_config(const char *username, const uint8_t *account_id) {
    patch_str(cfg, 0x04, username, 17);
    memcpy(cfg + 0x100, account_id, 8);
    patch_str(cfg, 0x1AD, username, 17);

    char country[3] = {0};
    memcpy(country, cfg + 0x1BE, 2);
    if (country[0] == 0) strcpy(country, "us");

    char np_email[65] = {0};
    snprintf(np_email, sizeof(np_email), "%.16s@a8.%.2s.np.playstation.net", username, country);

    patch_str(cfg, 0x108, np_email, 65);
    patch_str(cfg, 0x1100, np_email, 65);
}

static void build_account(const char *online_id, const uint8_t *account_id,
                          const char *country, const char *lang, const char *locale) {
    memset(acct, 0, sizeof(acct));
    uint32_t ver = 1;
    memcpy(acct + 0x00, &ver, 4);
    memcpy(acct + 0x08, account_id, 8);
    memset(acct + 0x10, '0', 64);
    acct[0x50] = 0x00;
    patch_str(acct, 0x51, online_id, 15);
    patch_str(acct, 0x65, "np", 8);
    acct[0x6A] = 0x01;
    patch_str(acct, 0x75, country, 2);
    patch_str(acct, 0x80, lang, 2);
    patch_str(acct, 0x88, locale, 8);
    memcpy(acct + 0xB8, &ver, 4);

    char hex_hash[33] = {0};
    hmac_md5_hex(dat_hmac_key, sizeof(dat_hmac_key), acct, 0xB8, hex_hash);
    memcpy(acct + 0xC0, hex_hash, 32);
}

static int write_file(const char *path, const unsigned char *data, size_t len) {
    int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) return -1;
    ssize_t written = write(fd, data, len);
    close(fd);
    return (written == (ssize_t)len) ? 0 : -1;
}

static int write_np_file(uint32_t userId, const char *fmt,
                         const unsigned char *data, size_t len) {
    char path[256];
    snprintf(path, sizeof(path), fmt, userId);
    return write_file(path, data, len);
}

static void write_np_files(uint32_t userId) {
    char dir[256];
    snprintf(dir, sizeof(dir), "/system_data/priv/home/%x/np", userId);
    mkdir(dir, 0755);

    write_np_file(userId, "/system_data/priv/home/%x/config.dat", cfg, sizeof(cfg));
    write_np_file(userId, "/system_data/priv/home/%x/np/account.dat", acct, sizeof(acct));
    write_np_file(userId, "/system_data/priv/home/%x/np/auth.dat", auth_dat, sizeof(auth_dat));
}

static void write_registry_state(uint32_t off) {
    sceRegMgrSetStr(REG_KEY_EMAIL + off, (const char *)(cfg + 0x108), 65);
    sceRegMgrSetStr(REG_KEY_NP_ENV + off, (const char *)(cfg + 0x177), 17);
    sceRegMgrSetStr(REG_KEY_ONLINE_ID + off, (const char *)(cfg + 0x1AD), 17);
    sceRegMgrSetStr(REG_KEY_COUNTRY + off, (const char *)(cfg + 0x1BE), 3);
    sceRegMgrSetStr(REG_KEY_LANGUAGE + off, (const char *)(cfg + 0x1C1), 6);
    sceRegMgrSetStr(REG_KEY_LOCALE + off, (const char *)(cfg + 0x1C7), 36);

    int32_t val;
    memcpy(&val, cfg + 0x48, 4); sceRegMgrSetInt(REG_KEY_FLAG_1 + off, val);
    memcpy(&val, cfg + 0x50, 4); sceRegMgrSetInt(REG_KEY_FLAG_2 + off, val);
    memcpy(&val, cfg + 0x4C, 4); sceRegMgrSetInt(REG_KEY_FLAG_3 + off, val);
    memcpy(&val, cfg + 0x5C, 4); sceRegMgrSetInt(REG_KEY_FLAG_5C + off, val);
    memcpy(&val, cfg + 0x1F4, 4); sceRegMgrSetInt(REG_KEY_FIELD_1F4 + off, val);
    memcpy(&val, cfg + 0x1F8, 4); sceRegMgrSetInt(REG_KEY_SIGNIN_FLAG + off, val);
    memcpy(&val, cfg + 0x1FC, 4); sceRegMgrSetInt(REG_KEY_FIELD_1FC + off, val);

    memcpy(&val, cfg + 0xA4, 4); sceRegMgrSetInt(REG_KEY_NP_0xA4 + off, val);
    memcpy(&val, cfg + 0xB4, 4); sceRegMgrSetInt(REG_KEY_NP_0xB4 + off, val);
    memcpy(&val, cfg + 0xD0, 4); sceRegMgrSetInt(REG_KEY_NP_0xD0 + off, val);
    memcpy(&val, cfg + 0xD4, 4); sceRegMgrSetInt(REG_KEY_NP_0xD4 + off, val);
    memcpy(&val, cfg + 0xDC, 4); sceRegMgrSetInt(REG_KEY_NP_0xDC + off, val);
    memcpy(&val, cfg + 0xF4, 4); sceRegMgrSetInt(REG_KEY_NP_0xF4 + off, val);

    sceRegMgrSetStr(REG_KEY_EXT_0x1100 + off, (const char *)(cfg + 0x1100), 65);
    sceRegMgrSetStr(REG_KEY_EXT_0x1141 + off, (const char *)(cfg + 0x1141), 11);
    sceRegMgrSetStr(REG_KEY_EXT_0x114C + off, (const char *)(cfg + 0x114C), 65);
}

static void set_registry_from_config(int account_numb) {
    uint32_t slot_off = (uint32_t)(account_numb - 1) * 0x10000;
    write_registry_state(0);
    write_registry_state(slot_off);
}

static int is_ps5_fake_signed_in(int account_numb, uint64_t account_id) {
    if (account_id == 0) return 0;

    uint32_t slot_off = (uint32_t)(account_numb - 1) * 0x10000;
    int32_t signin_flag = 0;
    char online_id[17] = {0};

    if (sceRegMgrGetInt(REG_KEY_SIGNIN_FLAG + slot_off, &signin_flag) != 0) {
        sceRegMgrGetInt(REG_KEY_SIGNIN_FLAG, &signin_flag);
    }
    if (sceRegMgrGetStr(REG_KEY_ONLINE_ID + slot_off, online_id, sizeof(online_id)) != 0 || online_id[0] == '\0') {
        sceRegMgrGetStr(REG_KEY_ONLINE_ID, online_id, sizeof(online_id));
    }

    return (signin_flag == 0x02 && online_id[0] != '\0');
}

int main(void) {
    OrbisUserServiceInitializeParams params = { .priority = 0x2bc };
    sceUserServiceInitialize(&params);

    char username[USERNAME_MAX] = {0};
    int activated_changed = 0;
    int error_code = 0;
    int32_t user_id = 0;
    int account_numb = 0;
    uint64_t account_id = 0;

    if (sceUserServiceGetForegroundUser(&user_id) != 0) {
        notify_epinor("Failed to get foreground user❌");
        sceUserServiceTerminate();
        return 1;
    }

    /* Step 1: Ensure foreground account is activated */
    if (account_activator_run(username, &activated_changed, &error_code,
                              &account_numb, &account_id) != 0) {
        char err_msg[128];
        snprintf(err_msg, sizeof(err_msg), "Activation Failed (Err: %d)❌", error_code);
        notify_epinor(err_msg);
        sceUserServiceTerminate();
        return 1;
    }

    /* Step 2: Check if user is already signed in */
    if (is_ps5_fake_signed_in(account_numb, account_id)) {
        if (activated_changed) {
            notify_epinor("Activated + Signed in🐈");
        } else {
            notify_epinor("Already Activated + Signed in🐈");
        }
        sceUserServiceTerminate();
        return 0;
    }

    /* Step 3: Detect region from system language */
    struct { const char *country; const char *lang; const char *locale; } region = {"us", "en", "en-US"};
    int32_t sys_lang = 1;
    if (sceSystemServiceParamGetInt(1 /* LANG */, &sys_lang) == 0) {
        switch (sys_lang) {
            case  0: region = (typeof(region)){"jp","ja","ja-JP"}; break;
            case  1: region = (typeof(region)){"us","en","en-US"}; break;
            case  2: region = (typeof(region)){"fr","fr","fr-FR"}; break;
            case  3: region = (typeof(region)){"es","es","es-ES"}; break;
            case  4: region = (typeof(region)){"de","de","de-DE"}; break;
            case  5: region = (typeof(region)){"it","it","it-IT"}; break;
            case  6: region = (typeof(region)){"nl","nl","nl-NL"}; break;
            case  7: region = (typeof(region)){"pt","pt","pt-PT"}; break;
            case  8: region = (typeof(region)){"ru","ru","ru-RU"}; break;
            case  9: region = (typeof(region)){"kr","ko","ko-KR"}; break;
            case 10: region = (typeof(region)){"tw","zh","zh-TW"}; break;
            case 11: region = (typeof(region)){"cn","zh","zh-CN"}; break;
            case 12: region = (typeof(region)){"fi","fi","fi-FI"}; break;
            case 13: region = (typeof(region)){"se","sv","sv-SE"}; break;
            case 14: region = (typeof(region)){"dk","da","da-DK"}; break;
            case 15: region = (typeof(region)){"no","no","no-NO"}; break;
            case 16: region = (typeof(region)){"pl","pl","pl-PL"}; break;
            case 17: region = (typeof(region)){"br","pt","pt-BR"}; break;
            case 18: region = (typeof(region)){"gb","en","en-GB"}; break;
            case 19: region = (typeof(region)){"tr","tr","tr-TR"}; break;
            case 20: region = (typeof(region)){"mx","es","es-MX"}; break;
            case 21: region = (typeof(region)){"sa","ar","ar-SA"}; break;
            case 22: region = (typeof(region)){"ca","fr","fr-CA"}; break;
            case 23: region = (typeof(region)){"cz","cs","cs-CZ"}; break;
            case 24: region = (typeof(region)){"hu","hu","hu-HU"}; break;
            case 25: region = (typeof(region)){"gr","el","el-GR"}; break;
            case 26: region = (typeof(region)){"ro","ro","ro-RO"}; break;
            case 27: region = (typeof(region)){"th","th","th-TH"}; break;
            case 28: region = (typeof(region)){"vn","vi","vi-VN"}; break;
            case 29: region = (typeof(region)){"id","id","id-ID"}; break;
        }
    }

    /* Step 4: Build & Write NP files and Registry */
    memcpy(cfg, config_dat, sizeof(cfg));
    patch_str(cfg, 0x1BE, region.country, 3);
    patch_str(cfg, 0x1C1, region.lang, 6);
    patch_str(cfg, 0x1C7, region.locale, 16);

    build_config(username, (const uint8_t *)&account_id);
    build_account(username, (const uint8_t *)&account_id, region.country, region.lang, region.locale);

    write_np_files((uint32_t)user_id);
    set_registry_from_config(account_numb);

    /* Step 5: Report clean notification */
    if (activated_changed) {
        notify_epinor("Activated + Signed in🐈");
    } else {
        notify_epinor("Signed in🐈");
    }

    sceUserServiceTerminate();
    return 0;
}