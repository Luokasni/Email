#include "email.h"

void free_mail_list(MailNode *head) {
    while (head) {
        MailNode *next = head->next;
        free(head);
        head = next;
    }
}

MailNode *load_mail_list(const char *username, const char *kind) {
    char filename[100];
    FILE *fp;
    MailNode *head = NULL, *tail = NULL;
    Email e;

    mailbox_filename(username, kind, filename, sizeof(filename));
    fp = fopen(filename, "rb");
    if (!fp) return NULL;

    while (fread(&e, sizeof(Email), 1, fp) == 1) {
        MailNode *node = (MailNode *)malloc(sizeof(MailNode));
        if (!node) break;
        node->email = e;
        node->next = NULL;
        if (!head) head = node;
        else tail->next = node;
        tail = node;
    }
    fclose(fp);
    return head;
}

int append_email(const char *username, const char *kind, const Email *email) {
    char filename[100];
    FILE *fp;
    mailbox_filename(username, kind, filename, sizeof(filename));
    fp = fopen(filename, "ab");
    if (!fp) {
        printf("ERROR: Cannot open %s for writing.\n", filename);
        return 0;
    }
    if (fwrite(email, sizeof(Email), 1, fp) != 1) {
        fclose(fp);
        return 0;
    }
    fclose(fp);
    return 1;
}

void display_email(const Email *e) {
    printf("\n----------------------------------------\n");
    printf("Message ID : %d\n", e->id);
    printf("From       : %s\n", e->sender);
    printf("To         : %s\n", e->to);
    printf("CC         : %s\n", e->cc[0] ? e->cc : "(none)");
    printf("BCC        : %s\n", e->bcc[0] ? "(hidden from other recipients)" : "(none)");
    printf("Subject    : %s\n", e->subject);
    printf("Date       : %s\n", e->timestamp);
    printf("Status     : %s\n", e->is_read ? "READ" : "UNREAD");
    printf("Body       : %s\n", e->body);
    printf("----------------------------------------\n");
}

void display_mailbox(const char *username, const char *kind) {
    MailNode *head = load_mail_list(username, kind), *p = head;
    int count = 0;
    if (!p) {
        printf("No messages found.\n");
        return;
    }
    while (p) {
        printf("ID: %-5d | From: %-25s | Subject: %s | %s\n",
               p->email.id, p->email.sender, p->email.subject,
               p->email.is_read ? "READ" : "UNREAD");
        count++;
        p = p->next;
    }
    printf("Total messages: %d\n", count);
    free_mail_list(head);
}

/* ---------- Recipient list parsing ---------- */
