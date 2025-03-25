#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/un.h>
#include <ctype.h>
#include <string.h>

#define SOCKET_PATH "/tmp/mysocket.sock"
#define BufferSize 1024

void DieWithError(const char *errorMessage, int sockfd)
{
    perror(errorMessage);
    close(sockfd);
    exit(1);
}

int main()
{
    int clientsocket;
    struct sockaddr_un serverAddr;
    char buffer[BufferSize];
    size_t numByte;

    if ((clientsocket = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
    {
        perror("socket() Failed");
        exit(1);
    }

    memset(&serverAddr, 0, sizeof(struct sockaddr_un));

    serverAddr.sun_family = AF_UNIX;

    strcpy(serverAddr.sun_path, SOCKET_PATH);

    if (connect(clientsocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        DieWithError("connect() failed", clientsocket);
    }

    while (1)
    {
        printf("Enter an msg to send to the server :  ");
        fgets(buffer, sizeof(buffer), stdin);

        buffer[strcspn(buffer, "\n")] = '\0';

        if (strcmp(buffer, "exit") == 0)
        {
            break;
        }

        if (send(clientsocket, buffer, strlen(buffer), 0) < 0)
        {
            DieWithError("send() failed", clientsocket);
        }

        if ((numByte = recv(clientsocket, buffer, sizeof(buffer) - 1, 0)) > 0)
        {
            buffer[numByte] = '\0';
            printf("Received from server: %s\n", buffer);
        }

        if (numByte < 0)
        {
            DieWithError("recv() failed", clientsocket);
        }
        else if (numByte == 0)
        {
            printf("Server closed the connection.\n");
            break;
        }
    }

    close(clientsocket);

    return 0;
}
