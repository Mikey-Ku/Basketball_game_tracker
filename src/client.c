#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 9000
#define BUFFER_SIZE_SEND 256
#define BUFFER_SIZE_RECEIVE 4096
#define SERVER_IP "127.0.0.1" //localhost

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0); // Create socket
    if (sock < 0) { //socket failure check
        perror("socket failed");
        exit(1);
    }
    struct sockaddr_in server_addr; // Define server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) { // Connect to server
        perror("connect failed");
        exit(1);
    }
    printf("Connected to the server!\n");

    while (1) { // Main loop
        char message[BUFFER_SIZE_SEND];
        printf("> ");
        fgets(message, sizeof(message), stdin);
        message[strcspn(message, "\n")] = '\0'; // newline -> null terminator
        if (strcmp(message, "quit") == 0) {
            break;
        }
        if (send(sock, message, strlen(message), 0) < 0) {
            perror("Send failed");
            continue;
        }

        char buffer[BUFFER_SIZE_RECEIVE];
        int bytes = recv(sock, buffer, sizeof(buffer) - 1, 0);
        buffer[bytes] = '\0';
        printf("Server: %s\n", buffer);
    }
    close(sock);
    return 0;
}
