#include <stdio.h>     
#include <stdlib.h>     
#include <string.h>
#include <unistd.h>       
#include <arpa/inet.h> 
#include <sys/socket.h>  

int main() 
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket failed");
        exit(1);
    }
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(9000);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
    perror("connect failed");
    exit(1);
    }

    printf("Connected to the server!\n");

    while (1) {
    char message[1024];

    printf("> ");
    fgets(message, sizeof(message), stdin);

    message[strcspn(message, "\n")] = '\0';

    if (strcmp(message, "quit") == 0) {
        break;
    }

    send(sock, message, strlen(message), 0);

    char buffer[1024];
    int bytes = recv(sock, buffer, sizeof(buffer) - 1, 0);
    buffer[bytes] = '\0';

    printf("Server: %s\n", buffer);

    }

    close(sock);

    return 0;
}
