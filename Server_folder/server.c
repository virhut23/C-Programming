#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]) {

    if (argc < 2) {
        fprintf(stderr, "Port number is not provided\n");
        exit(1);
    }

    int sockFD, newSockFD, portNo, n;
    char buffer[255];
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t clilen;

    sockFD = socket(AF_INET, SOCK_STREAM, 0);
    if (sockFD < 0) {
        error("Error opening socket");
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    portNo = atoi(argv[1]);

    if (portNo <= 0) {
        fprintf(stderr, "Invalid port number.\n");
        exit(1);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portNo);

    if (bind(sockFD, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        error("Binding failed");
    }

    listen(sockFD, 5);
    clilen = sizeof(cli_addr);
    newSockFD = accept(sockFD, (struct sockaddr *) &cli_addr, &clilen);
    if (newSockFD < 0) {
        error("Error on accept");
    }

    printf("Client connected. Start chatting!\n");

    while (1) {
        bzero(buffer, 255);
        n = read(newSockFD, buffer, 255);
        if (n < 0) {
            error("Error on reading");
        }

        printf("\n[Client]: %s\n", buffer);
        fflush(stdout);

        if (strncmp("bye", buffer, 3) == 0)
            break;

        bzero(buffer, 255);
        printf("[You]: ");
        fgets(buffer, 255, stdin);
        buffer[strcspn(buffer, "\n")] = 0;

        strcat(buffer, "\n");

        n = write(newSockFD, buffer, strlen(buffer));
        if (n < 0) {
            error("Error on writing");
        }

        if (strncmp("bye", buffer, 3) == 0)
            break;
    }

    close(newSockFD);
    close(sockFD);
    return 0;
}
