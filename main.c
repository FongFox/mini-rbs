#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>

#include "protocol.h"

#define MAX_CLIENTS 5

int main() {
    printf("MiniRBS server starting...\n");

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = INADDR_ANY;
    bind(sockfd, (struct sockaddr *) &addr, sizeof(addr));

    struct UE clients[MAX_CLIENTS];
    for (int i = 0; i < MAX_CLIENTS; i++) {
        clients[i].socket = -1;
    }

    listen(sockfd, 5);
    printf("MiniRBS server listening on port 8080...\n");

    while (1) {
        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(sockfd, &read_fds);

        int max_fd = sockfd;
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i].socket != -1) {
                FD_SET(clients[i].socket, &read_fds);
                if (clients[i].socket > max_fd) {
                    max_fd = clients[i].socket;
                }
            }
        }

        select(max_fd + 1, &read_fds, NULL, NULL, NULL);

        if (FD_ISSET(sockfd, &read_fds)) {
            int new_fd = accept(sockfd, NULL, NULL);

            int placed = 0;
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (clients[i].socket == -1) {
                    clients[i].socket = new_fd;
                    clients[i].id = i + 1;
                    clients[i].state = IDLE;
                    placed = 1;
                    printf("UE %d connected (fd=%d)\n", clients[i].id, new_fd);
                    break;
                }
            }

            if (!placed) {
                printf("Server full, rejecting new connection\n");
                send(new_fd, "SERVER_FULL\n", strlen("SERVER_FULL\n"), 0);
                close(new_fd);
            } else {
                char *log_message = malloc(100);
                sprintf(log_message, "Log for UE connected at fd=%d", new_fd);
                printf("%s\n", log_message);
                free(log_message);
            }
        }

        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i].socket != -1 && FD_ISSET(clients[i].socket, &read_fds)) {
                char buffer[1024];
                int bytes = recv(clients[i].socket, buffer, sizeof(buffer), 0);

                if (bytes <= 0) {
                    close(clients[i].socket);
                    clients[i].socket = -1;
                    continue;
                }

                buffer[bytes] = '\0';
                buffer[strcspn(buffer, "\n")] = '\0';
                printf("UE %d received: %s\n", clients[i].id, buffer);

                char response[1024];
                int should_close = process_message(&clients[i], buffer, response);
                send(clients[i].socket, response, strlen(response), 0);

                if (should_close) {
                    close(clients[i].socket);
                    clients[i].socket = -1;
                }
            }
        }
    }

    return 0;
}
