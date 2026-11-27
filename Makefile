CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -pedantic -Iinclude
TARGET = email_poc
SOURCES = src/main.c src/utils.c src/users.c src/queue.c src/storage.c src/smtp.c src/imap.c src/pop3.c src/client.c

.PHONY: all clean
all: $(TARGET)

$(TARGET): $(SOURCES) include/email.h
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET)

clean:
	rm -f $(TARGET)
