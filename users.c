#include "email.h"

void load_users(void) {
    FILE *fp = fopen("data/users.dat", "rb");
    if (!fp) return;
    user_count = (int)fread(users, sizeof(User), MAX_USERS, fp);
    fclose(fp);
}

void save_user(const User *user) {
    FILE *fp = fopen("data/users.dat", "ab");
    if (!fp) {
        printf("ERROR: Could not save user file.\n");
        return;
    }
    fwrite(user, sizeof(User), 1, fp);
    fclose(fp);
}

void register_user(void) {
    User u;
    char confirm[PASSWORD_LEN];
    char address[EMAIL_LEN];

    if (user_count >= MAX_USERS) {
        printf("User limit reached.\n");
        return;
    }

    read_line("Choose username (e.g. alice): ", u.username, sizeof(u.username));
    if (!valid_username(u.username)) {
        printf("Invalid username. Use letters, numbers, dot or underscore.\n");
        return;
    }
    if (user_index(u.username) >= 0) {
        printf("Username already exists.\n");
        return;
    }

    read_line("Choose password: ", u.password, sizeof(u.password));
    read_line("Confirm password: ", confirm, sizeof(confirm));
    if (u.password[0] == '\0' || strcmp(u.password, confirm) != 0) {
        printf("Password is empty or confirmation does not match.\n");
        return;
    }

    users[user_count++] = u;
    save_user(&u);
    make_address(u.username, address, sizeof(address));
    printf("Account created successfully: %s\n", address);
}
