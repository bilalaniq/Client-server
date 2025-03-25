#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <ctype.h>
#include <string.h>
#include <signal.h>

#define MAX_CLIENTS_Served 3
#define BufferSize 1024
#define ServPort 8080

int Servsock;
struct sockaddr_in ServerAddr, ClientAddr;
socklen_t clientLen = sizeof(ClientAddr);
char Buffer[BufferSize];

void DieWithError(const char *errorMessage, int sockfd)
{
    perror(errorMessage);
    close(sockfd);
    exit(1);
}

void convertToUpper(char *str)
{
    while (*str)
    {
        *str = toupper(*str);
        str++;
    }
}

int main()
{

    if ((Servsock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket() failed");
        exit(1);
    }

    memset(&ServerAddr, 0, sizeof(ServerAddr));

    ServerAddr.sin_family = AF_INET;

    ServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    ServerAddr.sin_port = htons(ServPort);

    if (bind(Servsock, (struct sockaddr *)&ServerAddr, sizeof(ServerAddr)) < 0)
    {
        DieWithError("bind() failed", Servsock);
    }

    printf("Socket bound successfully to %d\n", ServPort);

    while (1)
    {
        ssize_t numByte = recvfrom(Servsock, Buffer, sizeof(Buffer) - 1, 0,
                                   (struct sockaddr *)&ClientAddr, &clientLen);

        if (numByte < 0)
        {
            DieWithError("recvfrom() failed", Servsock);
        }

        Buffer[numByte] = '\0';

        printf("Received from client :  %s\n ", Buffer);

        convertToUpper(Buffer);

        if (sendto(Servsock, Buffer, strlen(Buffer), 0,
                   (struct sockaddr *)&ClientAddr, clientLen) < 0)
        {
            DieWithError("sendto() failed", Servsock);
        }
    }

    close(Servsock);

    return 0;
}
