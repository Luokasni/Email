# Inlook Email Client-Server POC

A modular C11 teaching POC with simulated SMTP, POP3 and IMAP command exchanges, a FIFO outgoing queue, linked-list mailbox loading, file persistence and acknowledgments.

## Directory layout

```text
Email_Project/
├── include/email.h
├── src/                  # C modules
├── data/                 # Persistent server-side records
│   └── local/            # POP3-downloaded local copies
├── tests/                # Manual test plan
├── Makefile
└── README.md
```

## Build and run

From the project root:

```sh
make clean
make
./email_poc
```

Or compile directly:

```sh
gcc -std=c11 -Wall -Wextra -pedantic -Iinclude src/*.c -o email_poc
./email_poc
```

The program creates `data/` and `data/local/` if they do not exist. Runtime files:

- `data/users.dat` — registered users
- `data/<username>_inbox.dat` — server-side inbox
- `data/<username>_sent.dat` — sender's sent messages
- `data/local/<username>_downloads.dat` — POP3 local copies

## Demo

1. Register `bob` and `alice`.
2. Log in as `bob` and send to `alice@inlook.com`.
3. Log in as `alice`, view inbox, then retrieve using POP3.
4. View local downloads; use IMAP to fetch and mark a message read.

## Scope and limitations

This is a single-process, file-backed educational simulation. SMTP/POP3/IMAP commands and responses are demonstrated by application logic; this is not a TCP server and is not fully RFC-compliant. User credentials are stored in plain text and must not be used for real accounts. Email records are stored as binary C structures, so they are intended to be read by this program rather than edited as text.
