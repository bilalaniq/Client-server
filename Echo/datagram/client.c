#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <ctype.h>
#include <string.h>
#include <arpa/inet.h>

#define BufferSize 1024
#define ServPort 8080
#define SERVER_IP "127.0.0.1"
// 127.0.0.1 (also called localhost) is a loopback address that refers to the same machine.
// This works only if both the server and client run on the same computer

void DieWithError(const char *errorMessage, int sockfd)
{
    perror(errorMessage);
    close(sockfd);
    exit(1);
}

int main()
{
    int clientsocket;
    struct sockaddr_in serverAddr;
    char buffer[BufferSize];
    size_t numByte;

    if ((clientsocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket() Failed");
        exit(1);
    }


    memset(&serverAddr, 0, sizeof(serverAddr));

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
    serverAddr.sin_port = htons(8080);

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
            if (strcmp(buffer, "DISCONNECT") == 0)
            {
                printf("server has been disconnected\n");
                exit(1);
            }
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
