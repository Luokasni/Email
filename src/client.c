#include "email.h"

void view_local_mail(const char *username) {
    MailNode *head = load_mail_list(username, "local");
    MailNode *p = head;
    if (!p) {
        printf("No locally downloaded messages.\n");
        return;
    }
    printf("\n--- Local downloaded messages ---\n");
    while (p) {
        display_email(&p->email);
        p = p->next;
    }
    free_mail_list(head);
}

void view_one_inbox_message(const char *username) {
    char line[32];
    int id;
    MailNode *head, *p;
    read_line("Enter message ID: ", line, sizeof(line));
    id = atoi(line);
    head = load_mail_list(username, "mailbox");
    p = head;
    while (p && p->email.id != id) p = p->next;
    if (p) display_email(&p->email);
    else printf("Message not found.\n");
    free_mail_list(head);
}

/* ---------- Main application ---------- */

void user_menu(const char *username) {
    int choice;
    do {
        printf("\n========== INLOOK MAIL ==========\n");
        printf("Logged in as: %s@%s\n", username, DOMAIN);
        printf("1. Compose and send email (SMTP)\n");
        printf("2. View inbox\n");
        printf("3. View sent emails\n");
        printf("4. Read inbox email\n");
        printf("5. Retrieve/download email (POP3)\n");
        printf("6. Access/manage server mailbox (IMAP)\n");
        printf("7. View local downloaded emails\n");
        printf("0. Logout\n");
        choice = read_choice();

        switch (choice) {
            case 1: compose_and_send(username); break;
            case 2: display_mailbox(username, "mailbox"); break;
            case 3: display_mailbox(username, "sent"); break;
            case 4: view_one_inbox_message(username); break;
            case 5: pop3_download(username); break;
            case 6: imap_menu(username); break;
            case 7: view_local_mail(username); break;
            case 0: printf("Logged out.\n"); break;
            default: printf("Invalid choice.\n"); break;
        }
    } while (choice != 0);
}

void login_user(void) {
    char username[NAME_LEN], password[PASSWORD_LEN], address[EMAIL_LEN];
    int index;

    read_line("Username: ", username, sizeof(username));
    read_line("Password: ", password, sizeof(password));

    index = user_index(username);
    if (index < 0 || strcmp(users[index].password, password) != 0) {
        printf("Authentication failed.\n");
        return;
    }
    make_address(username, address, sizeof(address));
    printf("Authentication successful. Welcome %s\n", address);
    user_menu(username);
}

