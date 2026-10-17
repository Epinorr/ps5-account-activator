#include "notification.h"
#include <stdio.h>
#include <string.h>

extern int sceKernelSendNotificationRequest(int device,
notify_request_t *req,
size_t size, int blocking);

int send_notification(const char *message) {
notify_request_t req;
memset(&req, 0, sizeof(req));
strncpy(req.message, message, sizeof(req.message) - 1);
return sceKernelSendNotificationRequest(0, &req, sizeof(req), 0);
}

void notify_epinor(const char *status_line) {
char msg[512];
snprintf(msg, sizeof(msg), "%s\n\nCoded by EPINOR", status_line);
send_notification(msg);
}