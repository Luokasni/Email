#include "email.h"

int add_recipient_list(const char *list, char recipients[][EMAIL_LEN],
                              int max_recipients) {
    char copy[LIST_LEN];
    char *token;
    int count = 0;
    size_t i;

    if (!list[0]) return 0;
    snprintf(copy, sizeof(copy), "%s", list);
    token = strtok(copy, ",");
    while (token && count < max_recipients) {
        while (*token == ' ') token++;
        for (i = strlen(token); i > 0 && token[i - 1] == ' '; i--)
            token[i - 1] = '\0';
        if (token[0]) {
            if (!valid_address(token) || !address_exists(token)) return -1;
            snprintf(recipients[count], EMAIL_LEN, "%s", token);
            count++;
        }
        token = strtok(NULL, ",");
    }
    return count;
}

int recipient_already_added(char recipients[][EMAIL_LEN], int count,
                                   const char *address) {
    int i;
    for (i = 0; i < count; i++)
        if (strcmp(recipients[i], address) == 0) return 1;
    return 0;
}

/* ---------- SMTP simulation and delivery ---------- */

void print_smtp_exchange(const Email *e) {
    printf("\n========== SMTP SESSION ==========\n");
    printf("C: EHLO %s\n", DOMAIN);
    printf("S: 250 Hello %s\n", DOMAIN);
    printf("C: MAIL FROM:<%s>\n", e->sender);
    printf("S: 250 Sender accepted\n");
    printf("C: RCPT TO:<%s>\n", e->to);
    printf("S: 250 Recipient accepted\n");
    printf("C: DATA\n");
    printf("S: 354 End data with <CRLF>.<CRLF>\n");
    printf("C: Subject: %s\n", e->subject);
    printf("C: %s\n", e->body);
    printf("C: .\n");
    printf("S: 250 Message accepted for delivery\n");
    printf("C: QUIT\n");
    printf("S: 221 Bye\n");
    printf("==================================\n");
}

int deliver_to_recipient(const Email *original, const char *address) {
    Email copy = *original;
    char username[NAME_LEN];

    username_from_address(address, username, sizeof(username));
    if (!address_exists(address)) return 0;

    /* Each recipient gets an individual mailbox copy. */
    snprintf(copy.to, sizeof(copy.to), "%s", address);
    copy.delivery_status = 1;
    copy.is_read = 0;

    if (!append_email(username, "mailbox", &copy)) return 0;
    printf("Delivery ACK: %s accepted message ID %d\n", address, copy.id);
    return 1;
}

void compose_and_send(const char *current_user) {
    Email email;
    char sender[EMAIL_LEN];
    char recipients[10][EMAIL_LEN];
    int count = 0, i, delivered = 0, n;
    EmailQueue queue;
    Email queued;

    memset(&email, 0, sizeof(email));
    make_address(current_user, sender, sizeof(sender));
    snprintf(email.sender, sizeof(email.sender), "%s", sender);

    printf("\n--- Compose Email ---\n");
    read_line("To (comma-separated addresses): ", email.to, sizeof(email.to));
    read_line("CC (optional, comma-separated): ", email.cc, sizeof(email.cc));
    read_line("BCC (optional, comma-separated): ", email.bcc, sizeof(email.bcc));
    read_line("Subject: ", email.subject, sizeof(email.subject));
    read_line("Body (single line for this POC): ", email.body, sizeof(email.body));

    if (!email.to[0] || !email.subject[0] || !email.body[0]) {
        printf("To, subject and body are required.\n");
        return;
    }

    n = add_recipient_list(email.to, recipients, 10);
    if (n <= 0) {
        printf("Invalid or unregistered To recipient.\n");
        return;
    }
    count = n;

    n = add_recipient_list(email.cc, recipients + count, 10 - count);
    if (n < 0) {
        printf("Invalid or unregistered CC recipient.\n");
        return;
    }
    count += n;

    n = add_recipient_list(email.bcc, recipients + count, 10 - count);
    if (n < 0) {
        printf("Invalid or unregistered BCC recipient.\n");
        return;
    }
    count += n;

    /* Remove duplicate recipients before delivery. */
    {
        char unique[10][EMAIL_LEN];
        int unique_count = 0;
        for (i = 0; i < count; i++) {
            if (!recipient_already_added(unique, unique_count, recipients[i])) {
                snprintf(unique[unique_count], EMAIL_LEN, "%s", recipients[i]);
                unique_count++;
            }
        }
        count = unique_count;
        for (i = 0; i < count; i++)
            snprintf(recipients[i], EMAIL_LEN, "%s", unique[i]);
    }

    email.id = next_email_id++;
    current_timestamp(email.timestamp, sizeof(email.timestamp));
    email.is_read = 0;
    email.delivery_status = 0;

    queue_init(&queue);
    if (!enqueue(&queue, &email)) {
        printf("Could not add message to outgoing queue.\n");
        return;
    }

    print_smtp_exchange(&email);

    while (dequeue(&queue, &queued)) {
        for (i = 0; i < count; i++) {
            if (deliver_to_recipient(&queued, recipients[i])) delivered++;
            else printf("Delivery failed: %s\n", recipients[i]);
        }
        queued.delivery_status = (delivered == count) ? 1 : 0;
        append_email(current_user, "sent", &queued);
    }

    if (delivered == count) {
        printf("\nSubmission ACK: server accepted the message.\n");
        printf("Delivery status: DELIVERED to %d recipient(s).\n", delivered);
    } else {
        printf("Delivery status: PARTIAL/FAILED (%d of %d recipients).\n",
               delivered, count);
    }
}

/* ---------- IMAP simulation ---------- */
