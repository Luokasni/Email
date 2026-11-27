#include "email.h"

void pop3_download(const char *username) {
    char line[32], filename[100];
    FILE *fp;
    MailNode *head, *p;
    int id, downloaded = 0;

    printf("\n========== POP3 SESSION ==========\n");
    printf("C: USER %s\nS: +OK user accepted\n", username);
    printf("C: PASS ********\nS: +OK maildrop locked and ready\n");
    head = load_mail_list(username, "mailbox");
    printf("C: STAT\n");
    {
        int count = 0;
        for (p = head; p; p = p->next) count++;
        printf("S: +OK %d message(s)\n", count);
    }
    printf("C: LIST\n");
    for (p = head; p; p = p->next)
        printf("S: %d %lu bytes (message ID %d)\n",
               p->email.id, (unsigned long)sizeof(Email), p->email.id);

    read_line("Enter message ID to RETR (0 to cancel): ", line, sizeof(line));
    id = atoi(line);
    if (id == 0) {
        free_mail_list(head);
        printf("C: QUIT\nS: +OK POP3 session ended\n");
        return;
    }

    p = head;
    while (p && p->email.id != id) p = p->next;
    if (!p) {
        printf("S: -ERR No such message\n");
        free_mail_list(head);
        return;
    }

    mailbox_filename(username, "local", filename, sizeof(filename));
    fp = fopen(filename, "ab");
    if (!fp) {
        printf("Could not create local mailbox copy.\n");
        free_mail_list(head);
        return;
    }
    printf("C: RETR %d\nS: +OK message follows\n", id);
    fwrite(&p->email, sizeof(Email), 1, fp);
    fclose(fp);
    downloaded = 1;

    if (downloaded)
        printf("Local copy created successfully for message ID %d.\n", id);

    printf("C: QUIT\nS: +OK POP3 session ended\n");
    free_mail_list(head);
}

/* ---------- Inbox, sent and local copy ---------- */

