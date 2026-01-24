#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 6969
#define MTU_SIZE 1400

int main() {
    int server_fd;
    struct sockaddr_in address;
    int opt = 1;
    char buffer[MTU_SIZE];

    printf("🔫 ShankPit Infantry Server v95 (MTU Fixed) Starting...\n");

    if ((server_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket failed"); exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed"); exit(EXIT_FAILURE);
    }

    printf("📡 Listening on port %d...\n", PORT);
    
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);

    while(1) {
        int n = recvfrom(server_fd, buffer, MTU_SIZE, 0, (struct sockaddr*)&client_addr, &addr_len);
        if (n > 0) {
            // Process Infantry Input
            // Echo back state (simplified for restoration)
            sendto(server_fd, buffer, n, 0, (struct sockaddr*)&client_addr, addr_len);
        }
    }
    return 0;
}
