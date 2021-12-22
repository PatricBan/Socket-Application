#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAXLINE 4096    /*max text line length*/
#define SERV_PORT 22521 /*port*/
#define LISTENQ 8       /*maximum number of client connections */

void connectToIPv6Server(int sock_desc, struct sockaddr_in6 serv)
{

    inet_pton(AF_INET6, "2a00:1450:4001:828::2004", &serv.sin6_addr); //IPv6 addresses from text to binary form

    serv.sin6_family = AF_INET6;
    serv.sin6_port = htons(80);

    printf("before connection\n ");

    //Connect
    if (connect(sock_desc, (struct sockaddr *)&serv, sizeof(serv)) < 0)
    {
        puts("connect error");
    }

    puts("Connected");
}

void sendAndReceive(int sock_desc)
{

    //opening the html file for writing
    int num;
    FILE *fptr;

    fptr = fopen("/home/g5s2e1/final/google.html", "w");

    if (fptr == NULL)
    {
        printf("Error opening the file!");
        exit(1);
    }

    // make the request to IPv6 server
    char *command = "GET / HTTP/1.0\r\n\r\n";
    send(sock_desc, command, strlen(command), 0);
    printf("command sent");

    int n = 0;
    char recvBuff[2048];

    while ((n = recv(sock_desc, recvBuff, sizeof(recvBuff) - 1, 0)) > 0) //receive data on the socket
    {
        recvBuff[n] = 0;

        fprintf(fptr, recvBuff); //write in the file
        printf(recvBuff);        // display in the console
    }

    printf("\n\n\n");

    fclose(fptr);
}

/*
Realizing the IPv4 server. Make connections and send commands from
ClientIPv4_win_2021.exe 
*/
void createIpv4Server()
{
    int listenfd, connfd, n;
    socklen_t clilen;
    char buf[MAXLINE];
    struct sockaddr_in cliaddr, servaddr;

    //creation of the socket
    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    //preparation of the socket address
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    // bind: assigns the address specified by servaddr to the socket referred to
    // by the file descriptor listenfd.
    bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));


    //listen for connections on a socket
    listen(listenfd, LISTENQ);

    printf("%s\n", "Server running...waiting for connections.");
    fflush(stdout);
    for (;;)
    {
        printf(" starting of for\n");
        fflush(stdout);
        clilen = sizeof(cliaddr);

       // accept a connection on a socket
        connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);
        printf("%s\n", "Received request...");
        fflush(stdout);

        while ((n = recv(connfd, buf, MAXLINE, 0)) > 0)
        {
            printf("%s\n", "String received from and resent to the client:");
            fflush(stdout);

            //if the command received is 01#
            if (strcmp(buf, "01#") == 0)
            {
                puts("Command #01 implemented!");

                //send confirmation
                send(connfd, "Command #01 implemented!", 30, 0);

                /*
                    reading from the file and sending line by line to the server
                */
                FILE *fptr;
                fptr = fopen("/home/g5s2e1/final/google.html", "r");
                char line[2048];

                if (fptr == NULL)
                {
                    printf("Error opening the file!");
                    exit(1);
                }

                //read until eof
                while (fgets(line, sizeof(line), fptr))
                {

                    //  printf("%s", line);
                    // send the html code
                    send(connfd, line, sizeof(line), 0);
                }

                fclose(fptr);

                fflush(stdout);
            }
            else
            {
                puts("Command not implemented!");
                send(connfd, "Command not implemented!", 30, 0);
                fflush(stdout);
            }
            puts(buf);
            fflush(stdout);
        }

        if (n < 0)
        {
            perror("Read error");
            exit(1);
        }
        close(connfd);
    }
    //close listening socket
    close(listenfd);
}

int main(int argc, char const *argv[])
{

    //Create socket
    int sock_desc;
    struct sockaddr_in6 serv;
    sock_desc = socket(AF_INET6, SOCK_STREAM, 0);

    if ((sock_desc < 0))
    {
        printf("\n Socket creation error \n");
        return -1;
    }
    printf("socket created\n");

    connectToIPv6Server(sock_desc, serv);
    sendAndReceive(sock_desc);
    createIpv4Server();

    return 0;
}
