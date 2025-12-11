#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "parser.h"
#include "stats.h"
#include "database.h"

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket failed");
        exit(1);
    }
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(9000);
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        exit(1);
    }
    if (listen(server_fd, 5) < 0) {
    perror("listen failed");
    exit(1);
    }
    printf("Server is listening on port 9000...\n");

    if (!db_init("games.db")) {
        fprintf(stderr, "Failed to open database. Exiting.\n");
        return 1;
    }

    int client_fd = accept(server_fd, NULL, NULL);
    if (client_fd < 0) {
        perror("accept failed");
        exit(1);
    }
    printf("Client connected!\n");

    char buffer[1024];
    while (1) {
        int bytes = recv(client_fd, buffer, sizeof(buffer) - 1, 0);

        if (bytes < 0) {
            perror("recv failed");
            break;
        }
        if (bytes == 0) {
            printf("Client disconnected.\n");
            break;
        }

        buffer[bytes] = '\0';
        printf("Received: %s\n", buffer);
        
        parse_message(buffer);

        send(client_fd, "ACK\n", 4, 0);
    }

    close(client_fd);
    close(server_fd);
    db_close();
    
    return 0;
}
