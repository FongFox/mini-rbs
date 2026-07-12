#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

enum UEState {
    IDLE,
    ATTACHED
};

struct UE {
    int id;
    // int state; // 0 = IDE; 1 = CONNECTED; 2 = ATTACHED
    enum UEState state;
};

int main() {
    printf("MiniRBS server starting...\n");

    // 1. socket() - tạo sockfd, duy nhất, duy trì suốt đời trong chương trình
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // 2. bind() - gán port 8080 cho sockfd
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = INADDR_ANY;
    bind(sockfd, (struct sockaddr *)&addr, sizeof(addr));

    // 3. listen() - bật chế độ chờ, tối đa 5 kết nối xếp hàng
    listen(sockfd, 5);
    printf("MiniRBS server listening on port 8080...\n");

    while (1) {
        // 4. accept() - sockfd vẫn y nguyên, chỉ sinh ra client_fd MỚI mỗi vòng lặp
        int client_fd = accept(sockfd, NULL, NULL);

        struct UE ue;
        ue.id = 1;
        ue.state = IDLE;

        while (1) {
            // 5. recv()/send() - nói chuyện qua lại, dùng client_fd (không dùng sockfd)
            char buffer[1024];
            int bytes = recv(client_fd, buffer, sizeof(buffer), 0);

            if (bytes <= 0) {
                // UE đã ngắt kết nối (đóng nc, mất mạng...)
                break;
            }

            buffer[bytes] = '\0';
            buffer[strcspn(buffer, "\n")] = '\0'; // Cắt bỏ \n nếu có (tương đương: buffer[14] = '\0';)
            printf("Received: %s\n", buffer);

            // LOGIC XỬ LÝ GIAO THỨC
            if (strcmp(buffer, "ATTACH_REQUEST") == 0) {
                if (ue.state == IDLE) {
                    send(client_fd, "ATTACH_ACCEPT\n", strlen("ATTACH_ACCEPT\n"), 0);
                    ue.state = ATTACHED;
                } else {
                    send(client_fd, "ATTACH_REJECT\n", strlen("ATTACH_REJECT\n"), 0);
                }
            } else if (strcmp(buffer, "PING") == 0) {
                if (ue.state == ATTACHED) {
                    send(client_fd, "PONG\n", strlen("PONG\n"), 0);
                } else {
                    send(client_fd, "PING_REJECT\n", strlen("PING_REJECT\n"), 0);
                }
            } else if (strcmp(buffer, "DETACH") == 0) {
                if (ue.state == ATTACHED) {
                    send(client_fd, "DETACH_ACCEPT\n", strlen("DETACH_ACCEPT\n"), 0);
                    ue.state = IDLE;
                    break;
                } else {
                    send(client_fd, "DETACH_REJECT\n", strlen("DETACH_REJECT\n"), 0);
                }
            } else {
                send(client_fd, "UNKNOWN_MESSAGE\n", strlen("UNKNOWN_MESSAGE\n"), 0);
            }
        }

        // đóng riêng UE (client_fd) này — sockfd vẫn sống, vòng lặp quay lại accept() tiếp
        close(client_fd);
    }

    return 0;
}