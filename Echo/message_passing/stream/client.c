#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h> // for socket(), bind(), listen(), accept(), etc.
#include <netinet/in.h> // for sockaddr_in, IPPROTO_TCP, and other network-related constants
#include <arpa/inet.h>  // for inet_addr() and inet_pton(), which help with IP address conversions
#include <unistd.h>     // for close() system call

#define SERVER_IP "127.0.0.1"
#define echoServPort 8080

void DieWithError(const char *errorMessage, int sockfd)
{
    perror(errorMessage);
    close(sockfd);
    exit(1);
}

int main()
{
    int clientSock;
    struct sockaddr_in echoServAddr;
    char buffer[1024];

    if ((clientSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
        perror("socket() Failed");
        exit(1);
    }

    memset(&echoServAddr, 0, sizeof(echoServAddr));
    echoServAddr.sin_family = AF_INET;
    echoServAddr.sin_addr.s_addr = inet_addr(SERVER_IP); // Server IP
    echoServAddr.sin_port = htons(echoServPort);

    if (connect(clientSock, (struct sockaddr *)&echoServAddr, sizeof(echoServAddr)) < 0)
    {
        DieWithError("connect() failed", clientSock);
    }

    while (1)
    {
        printf("Enter message to send to server (or 'exit' to quit): ");
        fgets(buffer, sizeof(buffer), stdin);

        buffer[strcspn(buffer, "\n")] = '\0';

        if (strcmp(buffer, "exit") == 0)
        {
            break;
        }

        if (send(clientSock, buffer, strlen(buffer), 0) < 0)
        {
            DieWithError("send() failed", clientSock);
        }

        ssize_t numBytes = recv(clientSock, buffer, sizeof(buffer) - 1, 0);
        if (numBytes < 0)
        {
            DieWithError("recv() failed", clientSock);
        }
        else if (numBytes == 0)
        {
            printf("Server closed the connection.\n");
            break;
        }

        buffer[numBytes] = '\0';
        printf("Received from server: %s\n", buffer);
        
    }

    close(clientSock);
    return 0;
}
