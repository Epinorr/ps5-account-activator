#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <stddef.h>
#include <stdint.h>

/* Same 0xC30-byte request layout used by proven PS5 payloads. */
typedef struct notify_request {
char useless1[45];
char message[3075];
} notify_request_t;

int send_notification(const char *message);
void notify_epinor(const char *status_line);

#endif /* NOTIFICATION_H */