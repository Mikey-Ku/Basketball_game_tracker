#include "database.h"
#include "parser.h"
#include "stats.h"

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#define PORT 9000
#define LISTEN_BACKLOG 5
#define BUFFER_SIZE 256

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0); // Create socket
    if (server_fd < 0) {
        perror("socket failed");
        exit(1);
    }
    struct sockaddr_in server_addr; // Define server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET; //address family (IPv4)
    server_addr.sin_addr.s_addr = INADDR_ANY; //Any IP address
    server_addr.sin_port = htons(PORT); // Port 9000
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) { // Bind socket
        perror("bind failed");
        exit(1);
    }
    if (listen(server_fd, LISTEN_BACKLOG) < 0) { // Listen for connections
        perror("listen failed");
        exit(1);
    }
    printf("Server is listening on port 9000...\n");

    if (!db_init("games.db")) { //check database initialization
        fprintf(stderr, "Failed to open database. Exiting.\n");
        return 1;
    }

    int client_fd = accept(server_fd, NULL, NULL); // Accept a connection
    if (client_fd < 0) {
        perror("accept failed");
        exit(1);
    }

    char buffer[BUFFER_SIZE];
    while (1) {
        int bytes = recv(client_fd, buffer, sizeof(buffer) - 1, 0); // Receive data

        if (bytes < 0) {
            perror("recv failed");
            break;
        }
        if (bytes == 0) {
            break;
        }

        buffer[bytes] = '\0';

        time_t now = time(NULL);
        char *time_str = ctime(&now);
        time_str[strlen(time_str) - 1] = '\0'; // Remove trailing newline

        printf("[%s] Received command: %s\n", time_str, buffer);

        char *response = handle_message(buffer, now); // Process message

        if (response) {
            send(client_fd, response, strlen(response), 0);
            free(response);
        }
    }

    close(client_fd); // Close client connection
    close(server_fd); // Close server socket
    db_close(); // Close database

    return 0;
}
