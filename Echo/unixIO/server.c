#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/un.h>
#include <pthread.h>
#include <ctype.h>
#include <string.h>

#define MAX_CLIENTS_Served 3
#define BufferSize 1024
#define SOCKET_PATH "/tmp/mysocket.sock"

int active_clients = 0;
pthread_mutex_t client_mutex = PTHREAD_MUTEX_INITIALIZER;

struct thread_args
{
    int clientSocket;
};

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

void *manage_client(void *args)
{
    struct thread_args *args_s = (struct thread_args *)args;
    char buffer[BufferSize];
    ssize_t numBytes;
    pid_t clientPID;

    numBytes = recv(args_s->clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (numBytes < 0)
    {
        perror("recv() failed");
        close(args_s->clientSocket);
        free(args_s);

        pthread_mutex_lock(&client_mutex);
        active_clients--;
        pthread_mutex_unlock(&client_mutex);

        return NULL;
    }

    clientPID = atoi(buffer);
    buffer[numBytes] = '\0';
    printf("Client connected with PID: %s\n", buffer);

    while ((numBytes = recv(args_s->clientSocket, buffer, sizeof(buffer) - 1, 0)) > 0)
    {
        buffer[numBytes] = '\0';
        printf("Received from client<%d>: %s\n", clientPID, buffer);
        convertToUpper(buffer);

        if (send(args_s->clientSocket, buffer, numBytes, 0) < 0)
        {
            perror("send() failed");
            break;
        }
    }

    if (numBytes == 0)
    {
        printf("Client disconnected.\n");
    }
    else if (numBytes < 0)
    {
        perror("recv() failed");
    }

    close(args_s->clientSocket);
    free(args_s);

    pthread_mutex_lock(&client_mutex);
    active_clients--;
    pthread_mutex_unlock(&client_mutex);

    return NULL;
}

int main()
{
    int Servsock;
    struct sockaddr_un ServerAddr, ClientAddr;
    pthread_t threadID;
    socklen_t clientLen = sizeof(ClientAddr);

    if ((Servsock = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
    {
        perror("socket() failed");
        exit(1);
    }

    memset(&ServerAddr, 0, sizeof(ServerAddr));
    ServerAddr.sun_family = AF_UNIX;
    strcpy(ServerAddr.sun_path, SOCKET_PATH);

    unlink(SOCKET_PATH);

    if (bind(Servsock, (struct sockaddr *)&ServerAddr, sizeof(ServerAddr)) < 0)
    {
        DieWithError("bind() failed", Servsock);
    }

    printf("Socket bound successfully to %s\n", SOCKET_PATH);

    if (listen(Servsock, MAX_CLIENTS_Served) < 0)
    {
        DieWithError("listen() failed", Servsock);
    }

    printf("Waiting for connections...\n");

    while (1)
    {
        struct thread_args *args = (struct thread_args *)malloc(sizeof(struct thread_args));
        if (!args)
        {
            perror("malloc() failed");
            continue;
        }

        int new_client;
        if ((new_client = accept(Servsock, (struct sockaddr *)&ClientAddr, &clientLen)) < 0)
        {
            perror("accept() failed");
            free(args);
            continue;
        }

        pthread_mutex_lock(&client_mutex);
        if (active_clients >= MAX_CLIENTS_Served)
        {
            pthread_mutex_unlock(&client_mutex);
            printf("Server full. Rejecting client.\n");
            close(new_client);
            free(args);
            continue;
        }

        active_clients++;
        pthread_mutex_unlock(&client_mutex);

        printf("New connection accepted\n");

        args->clientSocket = new_client;

        if (pthread_create(&threadID, NULL, manage_client, (void *)args) != 0)
        {
            perror("pthread_create() failed");
            close(new_client);
            free(args);

            pthread_mutex_lock(&client_mutex);
            active_clients--;
            pthread_mutex_unlock(&client_mutex);
            continue;
        }

        pthread_detach(threadID);
    }

    close(Servsock);
    unlink(SOCKET_PATH);
    return 0;
}
