#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h> // for socket(), bind(), listen(), accept(), etc.
#include <netinet/in.h> // for sockaddr_in, IPPROTO_TCP, and other network-related constants
#include <arpa/inet.h>  // for inet_addr() and inet_pton(), which help with IP address conversions
#include <unistd.h>     // for close() system call

#define SERVER_IP "127.0.0.1"
#define echoServPort 8080
#define MAXPENDING 5

void DieWithError(const char *errorMessage, int sockfd)
{
    perror(errorMessage);
    close(sockfd);
    exit(1);
}

int main()
{
    int servSock, clientSock;
    struct sockaddr_in echoServAddr, clientAddr;
    socklen_t clientlen;
    char buffer[1024];

    // Create server socket
    if ((servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
        perror("socket() failed");
        exit(1);
    }

    memset(&echoServAddr, 0, sizeof(echoServAddr));
    echoServAddr.sin_family = AF_INET;                /* Internet address family */
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    echoServAddr.sin_port = htons(echoServPort);      /* Local port */

    if (bind(servSock, (struct sockaddr *)&echoServAddr, sizeof(echoServAddr)) < 0)
    {
        DieWithError("bind() failed", servSock);
    }

    // Listen for incoming connections
    if (listen(servSock, MAXPENDING) < 0)
    {
        DieWithError("listen() failed", servSock);
    }

    printf("Waiting for connections on port %d...\n", echoServPort);

    while (1)
    {
        clientlen = sizeof(clientAddr);
        if ((clientSock = accept(servSock, (struct sockaddr *)&clientAddr, &clientlen)) < 0)
        {
            DieWithError("accept() failed", servSock);
        }

        printf("Connection established with client: %s:%d\n",
               inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

        ssize_t numBytes;
        while ((numBytes = recv(clientSock, buffer, sizeof(buffer) - 1, 0)) > 0)
        {
            buffer[numBytes] = '\0';
            printf("Received from client: %s\n", buffer);

            if (send(clientSock, buffer, numBytes, 0) < 0)
            {
                DieWithError("send() failed", clientSock);
            }
        }

        if (numBytes < 0)
        {
            DieWithError("recv() failed", clientSock);
        }
        else if (numBytes == 0)
        {
            printf("Client disconnected.\n");
        }

        close(clientSock);
    }

    close(servSock);
    return 0;
}
