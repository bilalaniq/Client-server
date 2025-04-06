# Echo using stream socket

- Client

  1.  Create a TCP socket
  2.  Establish connection
  3.  Communicate
  4.  Close the connection

- Server

  1.  Create a TCP socket
  2.  Assign a port to socket
  3.  Set socket to listen
  4.  Repeatedly:
      - Accept new connection
      - Communicate
      - Close the connection

<br>
<br>

# STEPS

The server starts by getting ready to receive client connections…

1. Create a TCP socket

```c
/* Create socket for incoming connections */
if ((servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
DieWithError("socket() failed");
```

2. Assign a port to socket

```c
echoServAddr.sin_family = AF_INET; /* Internet address family */
echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
echoServAddr.sin_port = htons(echoServPort); /* Local port */

if (bind(servSock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
DieWithError("bind() failed");
```

3. Set socket to listen

```c
/* Mark the socket so it will listen for incoming connections */
if (listen(servSock, MAXPENDING) < 0)
DieWithError("listen() failed");
```

4.  Accept new connection

```c
for (;;) /* Run forever */
{
clntLen = sizeof(echoClntAddr);
if ((clientSock=accept(servSock,(struct sockaddr *)&echoClntAddr,&clntLen))<0)
DieWithError("accept() failed");
...

}
```

Server is now blocked waiting for connection from a client
A client decides to talk to the server

5. Create a TCP socket (client side)

```c
/* Create a reliable, stream socket using TCP */
if ((clientSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
DieWithError("socket() failed");
```

6. Establish connection (client side)

```c
echoServAddr.sin_family = AF_INET; /* Internet address family */
echoServAddr.sin_addr.s_addr = inet_addr(echoservIP); /* Server IP address*/
echoServAddr.sin_port = htons(echoServPort); /* Server port */

if (connect(clientSock, (struct sockaddr *) &echoServAddr,
sizeof(echoServAddr)) < 0)
DieWithError("connect() failed");
```

Server’s accept procedure in now unblocked and returns client’s socket

7. Communicate (client side)

```c
echoStringLen = strlen(echoString); /* Determine input length */
/* Send the string to the server */
if (send(clientSock, echoString, echoStringLen, 0) != echoStringLen)
DieWithError("send() sent a different number of bytes than expected");
```

8. Communicate (server side)

```c
/* Receive message from client */
if ((recvMsgSize = recv(clntSocket, echoBuffer, RCVBUFSIZE, 0)) < 0)
DieWithError("recv() failed");
/* Send received string and receive again until end of transmission */
while (recvMsgSize > 0) { /* zero indicates end of transmission */
if (send(clientSocket, echobuffer, recvMsgSize, 0) != recvMsgSize)
DieWithError(“send() failed”);
if ((recvMsgSize = recv(clientSocket, echoBuffer, RECVBUFSIZE, 0)) < 0)
DieWithError(“recv() failed”);
}
```

Similarly, the client receives the data from the server

9. Close the connection

```c
close(clientSock);
```

Server is now blocked waiting for connection from a client...

and same is repeated

Repeatedly:
a. Accept new connection
b. Communicate
c. Close the connection
