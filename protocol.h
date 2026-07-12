#ifndef PROTOCOL_H
#define PROTOCOL_H

enum UEState {
    IDLE,
    ATTACHED
};

struct UE {
    int socket;
    int id;
    enum UEState state;
};

int process_message(struct UE *ue, const char *buffer, char *response);

#endif