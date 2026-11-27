#include "email.h"

User users[MAX_USERS];
int user_count = 0;
int next_email_id = 1;

int main(void) {
    int choice;
    ensure_data_directories();
    load_users();

    printf("====================================\n");
    printf("   INLOOK EMAIL CLIENT-SERVER POC\n");
    printf("   Custom domain: %s\n", DOMAIN);
    printf("====================================\n");

    do {
        printf("\n1. Register user\n");
        printf("2. Login\n");
        printf("3. List registered users (demo/admin)\n");
        printf("0. Exit\n");
        choice = read_choice();

        switch (choice) {
            case 1:
                register_user();
                break;
            case 2:
                login_user();
                break;
            case 3: {
                int i;
                if (user_count == 0) printf("No registered users.\n");
                for (i = 0; i < user_count; i++) {
                    char address[EMAIL_LEN];
                    make_address(users[i].username, address, sizeof(address));
                    printf("%d. %s\n", i + 1, address);
                }
                break;
            }
            case 0:
                printf("Exiting Inlook Email POC.\n");
                break;
            default:
                printf("Invalid choice.\n");
        }
    } while (choice != 0);

    return 0;
}
