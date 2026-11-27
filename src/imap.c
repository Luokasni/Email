#include "email.h"

int update_read_status(const char *username, int message_id) {
    char filename[100];
    FILE *fp;
    Email e;
    int found = 0;

    mailbox_filename(username, "mailbox", filename, sizeof(filename));
    fp = fopen(filename, "rb+");
    if (!fp) return 0;

    while (fread(&e, sizeof(Email), 1, fp) == 1) {
        if (e.id == message_id) {
            e.is_read = 1;
            if (fseek(fp, -(long)sizeof(Email), SEEK_CUR) == 0) {
                fwrite(&e, sizeof(Email), 1, fp);
                found = 1;
            }
            break;
        }
    }
    fclose(fp);
    return found;
}

void imap_menu(const char *username) {
    int choice, id;
    char line[32];
    MailNode *head, *p;
    printf("\n========== IMAP SESSION ==========\n");
    printf("C: LOGIN %s ********\n", username);
    printf("S: OK LOGIN successful\n");
    printf("C: SELECT INBOX\n");
    printf("S: OK INBOX selected\n");

    do {
        printf("\n1. LIST messages (IMAP SEARCH/UID overview)\n");
        printf("2. FETCH message by ID\n");
        printf("3. Mark message as read (IMAP STORE)\n");
        printf("0. LOGOUT\n");
        choice = read_choice();

        if (choice == 1) {
            printf("C: SEARCH ALL\nS: ");
            display_mailbox(username, "mailbox");
        } else if (choice == 2) {
            read_line("Message ID: ", line, sizeof(line));
            id = atoi(line);
            head = load_mail_list(username, "mailbox");
            p = head;
            while (p && p->email.id != id) p = p->next;
            if (p) {
                printf("C: FETCH %d BODY[]\nS: OK FETCH completed\n", id);
                display_email(&p->email);
            } else printf("Message not found.\n");
            free_mail_list(head);
        } else if (choice == 3) {
            read_line("Message ID to mark read: ", line, sizeof(line));
            id = atoi(line);
            printf("C: STORE %d +FLAGS (\\Seen)\n", id);
            if (update_read_status(username, id))
                printf("S: OK Message marked as read.\n");
            else printf("S: NO Message not found.\n");
        }
    } while (choice != 0);

    printf("C: LOGOUT\nS: OK LOGOUT completed\n");
}

/* ---------- POP3 simulation ---------- */

