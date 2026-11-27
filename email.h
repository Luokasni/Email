#ifndef EMAIL_H
#define EMAIL_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#define DOMAIN "inlook.com"
#define MAX_USERS 100
#define EMAIL_LEN 64
#define NAME_LEN 32
#define SUBJECT_LEN 120
#define BODY_LEN 700
#define LIST_LEN 240
#define PASSWORD_LEN 40

typedef struct { char username[NAME_LEN]; char password[PASSWORD_LEN]; } User;
typedef struct {
    int id;
    char sender[EMAIL_LEN], to[LIST_LEN], cc[LIST_LEN], bcc[LIST_LEN];
    char subject[SUBJECT_LEN], body[BODY_LEN], timestamp[32];
    int is_read, delivery_status;
} Email;
typedef struct MailNode { Email email; struct MailNode *next; } MailNode;
typedef struct QueueNode { Email email; struct QueueNode *next; } QueueNode;
typedef struct { QueueNode *front, *rear; } EmailQueue;

extern User users[MAX_USERS];
extern int user_count;
extern int next_email_id;

void ensure_data_directories(void);
void trim_newline(char *s);
void read_line(const char *prompt, char *buffer, size_t size);
int read_choice(void);
void make_address(const char *username, char *address, size_t size);
int valid_username(const char *s);
int valid_address(const char *address);
void username_from_address(const char *address, char *username, size_t size);
void mailbox_filename(const char *username, const char *kind, char *filename, size_t size);
void current_timestamp(char *buffer, size_t size);
int user_index(const char *username);
int address_exists(const char *address);
void load_users(void);
void save_user(const User *user);
void register_user(void);
void queue_init(EmailQueue *q);
int enqueue(EmailQueue *q, const Email *email);
int dequeue(EmailQueue *q, Email *email);
void free_mail_list(MailNode *head);
MailNode *load_mail_list(const char *username, const char *kind);
int append_email(const char *username, const char *kind, const Email *email);
void display_email(const Email *e);
void display_mailbox(const char *username, const char *kind);
int add_recipient_list(const char *list, char recipients[][EMAIL_LEN], int max_recipients);
int recipient_already_added(char recipients[][EMAIL_LEN], int count, const char *address);
void print_smtp_exchange(const Email *e);
int deliver_to_recipient(const Email *original, const char *address);
void compose_and_send(const char *current_user);
int update_read_status(const char *username, int message_id);
void imap_menu(const char *username);
void pop3_download(const char *username);
void view_local_mail(const char *username);
void view_one_inbox_message(const char *username);
void user_menu(const char *username);
void login_user(void);
#endif
