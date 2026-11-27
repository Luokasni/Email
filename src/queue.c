#include "email.h"

void queue_init(EmailQueue *q) {
    q->front = q->rear = NULL;
}

int enqueue(EmailQueue *q, const Email *email) {
    QueueNode *node = (QueueNode *)malloc(sizeof(QueueNode));
    if (!node) return 0;
    node->email = *email;
    node->next = NULL;
    if (q->rear) q->rear->next = node;
    else q->front = node;
    q->rear = node;
    return 1;
}

int dequeue(EmailQueue *q, Email *email) {
    QueueNode *temp;
    if (!q->front) return 0;
    temp = q->front;
    *email = temp->email;
    q->front = temp->next;
    if (!q->front) q->rear = NULL;
    free(temp);
    return 1;
}

/* ---------- Linked list: mailbox display ---------- */
