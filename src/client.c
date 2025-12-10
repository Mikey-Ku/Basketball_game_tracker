#include <stdio.h>        // printf(), perror()
#include <stdlib.h>       // exit()
#include <string.h>       // memset(), strlen()
#include <unistd.h>       // close()
#include <arpa/inet.h>    // inet_addr(), htons(), sockaddr_in
#include <sys/socket.h>   // socket(), connect(), send(), recv()

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
    char message[] = "WHATTT UPPP";
    send(sock, message, strlen(message), 0);

    char buffer[1024];
    int bytes = recv(sock, buffer, sizeof(buffer), 0);

    buffer[bytes] = '\0';

    printf("Server says: %s\n", buffer);

    close(sock);

    return 0;
}
