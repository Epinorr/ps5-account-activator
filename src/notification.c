#include "notification.h"

#include <string.h>

extern int sceKernelSendNotificationRequest(int device,
                                            notify_request_t *req,
                                            size_t size,
                                            int blocking);

int send_notification(const char *message)
{
    notify_request_t req;
    memset(&req, 0, sizeof(req));

    if (message != NULL)
        strncpy(req.message, message, sizeof(req.message) - 1);

    return sceKernelSendNotificationRequest(0, &req, sizeof(req), 0);
}