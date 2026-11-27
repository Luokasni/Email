#include "email.h"
#include <sys/stat.h>
#include <errno.h>

void ensure_data_directories(void) {
    if (mkdir("data", 0777) != 0 && errno != EEXIST) {
        perror("mkdir data");
    }
    if (mkdir("data/local", 0777) != 0 && errno != EEXIST) {
        perror("mkdir data/local");
    }
}

void trim_newline(char *s) {
    size_t n = strlen(s);
    while (n > 0 && (s[n - 1] == '\n' || s[n - 1] == '\r')) {
        s[--n] = '\0';
    }
}

void read_line(const char *prompt, char *buffer, size_t size) {
    int ch;
    printf("%s", prompt);
    if (fgets(buffer, (int)size, stdin) == NULL) {
        buffer[0] = '\0';
        return;
    }
    if (strchr(buffer, '\n') == NULL) {
        while ((ch = getchar()) != '\n' && ch != EOF) { }
    }
    trim_newline(buffer);
}

int read_choice(void) {
    char line[32];
    read_line("Choose: ", line, sizeof(line));
    return atoi(line);
}

void make_address(const char *username, char *address, size_t size) {
    snprintf(address, size, "%s@%s", username, DOMAIN);
}

int valid_username(const char *s) {
    size_t i, n = strlen(s);
    if (n == 0 || n >= NAME_LEN) return 0;
    for (i = 0; i < n; i++) {
        if (!(isalnum((unsigned char)s[i]) || s[i] == '_' || s[i] == '.'))
            return 0;
    }
    return 1;
}

int valid_address(const char *address) {
    char expected_suffix[32];
    size_t n = strlen(address), suffix_len;
    snprintf(expected_suffix, sizeof(expected_suffix), "@%s", DOMAIN);
    suffix_len = strlen(expected_suffix);
    if (n <= suffix_len) return 0;
    if (strcmp(address + n - suffix_len, expected_suffix) != 0) return 0;
    {
        char username[NAME_LEN];
        size_t user_len = n - suffix_len;
        if (user_len >= sizeof(username)) return 0;
        memcpy(username, address, user_len);
        username[user_len] = '\0';
        return valid_username(username);
    }
}

void username_from_address(const char *address, char *username, size_t size) {
    const char *at = strchr(address, '@');
    size_t n;
    if (!at) {
        username[0] = '\0';
        return;
    }
    n = (size_t)(at - address);
    if (n >= size) n = size - 1;
    memcpy(username, address, n);
    username[n] = '\0';
}

void mailbox_filename(const char *username, const char *kind,
                             char *filename, size_t size) {
    if (strcmp(kind, "local") == 0)
        snprintf(filename, size, "data/local/%s_downloads.dat", username);
    else if (strcmp(kind, "mailbox") == 0)
        snprintf(filename, size, "data/%s_inbox.dat", username);
    else if (strcmp(kind, "sent") == 0)
        snprintf(filename, size, "data/%s_sent.dat", username);
    else
        snprintf(filename, size, "data/%s_%s.dat", username, kind);
}

void current_timestamp(char *buffer, size_t size) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    if (tm_info) strftime(buffer, size, "%Y-%m-%d %H:%M:%S", tm_info);
    else snprintf(buffer, size, "unknown-time");
}

int user_index(const char *username) {
    int i;
    for (i = 0; i < user_count; i++) {
        if (strcmp(users[i].username, username) == 0) return i;
    }
    return -1;
}

int address_exists(const char *address) {
    char username[NAME_LEN];
    if (!valid_address(address)) return 0;
    username_from_address(address, username, sizeof(username));
    return user_index(username) >= 0;
}

