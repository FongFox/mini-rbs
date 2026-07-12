#include <string.h>
#include "protocol.h"

int process_message(struct UE *ue, const char *buffer, char *response) {
    if (strcmp(buffer, "ATTACH_REQUEST") == 0) {
        if (ue->state == IDLE) {
            strcpy(response, "ATTACH_ACCEPT\n");
            ue->state = ATTACHED;
        } else {
            strcpy(response, "ATTACH_REJECT\n");
        }
        return 0;
    } else if (strcmp(buffer, "PING") == 0) {
        if (ue->state == ATTACHED) {
            strcpy(response, "PONG\n");
        } else {
            strcpy(response, "PING_REJECT\n");
        }
        return 0;
    } else if (strcmp(buffer, "DETACH") == 0) {
        if (ue->state == ATTACHED) {
            strcpy(response, "DETACH_ACCEPT\n");
            ue->state = IDLE;
            return 1;
        } else {
            strcpy(response, "DETACH_REJECT\n");
            return 0;
        }
    } else {
        strcpy(response, "UNKNOWN_MESSAGE\n");
        return 0;
    }
}