#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

struct UE {
    int id;
    int state; // 0 = IDE; 1 = CONNECTED; 2 = ATTACHED
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

        // 5. recv()/send() - nói chuyện qua lại, dùng client_fd (không dùng sockfd)
        char buffer[1024];
        int bytes = recv(client_fd, buffer, sizeof(buffer), 0);
        buffer[bytes] = '\0';
        printf("Received: %s\n", buffer);

        // echo lại y hệt những gì nhận được
        send(client_fd, buffer, bytes, 0);

        // đóng riêng client_fd này — sockfd vẫn sống, vòng lặp quay lại accept() tiếp
        close(client_fd);
    }

    return 0;
}