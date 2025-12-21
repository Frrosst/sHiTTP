#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "parser.h"
#define PORT 8080
#define BUFFER_SIZE 1024

int main(void) {
    int server_fd, client_fd;
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    char buffer[BUFFER_SIZE];


    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }


    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    //!REMOVE LATER
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 10) < 0) {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

do{

    client_fd = accept(server_fd, (struct sockaddr *)&addr, &addr_len);
    if (client_fd < 0) {
        perror("accept");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    ssize_t bytes = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
    if (bytes < 0) {
        perror("recv");
    } else {
        buffer[bytes] = '\0';
        printf("Received:\n%s\n", buffer);
    }

    Request req;
    parse_req(buffer, &req);

  
    
    const char *response = "HTTP/1.1 200 OK \r\nContent-Type: text/plain\r\nContent-Length: 13\r\n\r\nHello World!\r\n";
    send(client_fd, response, strlen(response), 0);

    close(client_fd);
} while (1);


    close(server_fd);

    return 0;
}
