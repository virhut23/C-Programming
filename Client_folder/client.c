#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]) {

    int sockFD, portNo, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[255];

    if (argc < 3) {
        fprintf(stderr, "Usage: %s hostname port\n", argv[0]);
        exit(1);
    }

    portNo = atoi(argv[2]);
    sockFD = socket(AF_INET, SOCK_STREAM, 0);
    if (sockFD < 0) {
        error("Error opening socket");
    }

    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr, "Error, no such host\n");
        exit(1);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    memcpy((char *) &serv_addr.sin_addr.s_addr, (char *) server->h_addr, server->h_length);
    serv_addr.sin_port = htons(portNo);

    if (connect(sockFD, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        error("Connection failed");
    }

    printf("Connected to the server. Start chatting!\n");

    while (1) {
        bzero(buffer, 255);
        printf("[You]: ");
        fgets(buffer, 255, stdin);
        buffer[strcspn(buffer, "\n")] = 0;

        if (strlen(buffer) >= 254) {
            printf("Input too long, try again.\n");
            continue;
        }

        strcat(buffer, "\n");

        n = write(sockFD, buffer, strlen(buffer));
        if (n < 0) {
            error("Error writing to socket");
        }

        bzero(buffer, 255);
        n = read(sockFD, buffer, 255);
        if (n < 0) {
            error("Error reading from socket");
        }

        printf("\n[Server]: %s\n", buffer);
        fflush(stdout);

        if (strncmp("bye", buffer, 3) == 0) {
            break;
        }
    }

    close(sockFD);
    return 0;
}
