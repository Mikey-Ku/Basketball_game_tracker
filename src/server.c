#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "parser.h"
#include "stats.h"
#include "database.h"

#define PORT 9000
#define LISTEN_BACKLOG 5
#define BUFFER_SIZE 1024


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
    server_addr.sin_port = htons(PORT);
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        exit(1);
    }
    if (listen(server_fd, LISTEN_BACKLOG) < 0) {
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

    char buffer[BUFFER_SIZE];
    while (1) {
        int bytes = recv(client_fd, buffer, sizeof(buffer) - 1, 0);

        if (bytes < 0) {
            perror("recv failed");
            break;
        }
        if (bytes == 0) {
            break;
        }

        buffer[bytes] = '\0';
        
        char *response = handle_message(buffer);

        if (response) {
            send(client_fd, response, strlen(response), 0);
            free(response);
        }
    }

    close(client_fd);
    close(server_fd);
    db_close();
    
    return 0;
}
