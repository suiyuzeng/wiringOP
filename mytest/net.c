#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "controller.h"

int server_sockfd;
int thread;
int runFlag = 1;
int port;

void *run(void *tid)
{
    struct sockaddr_in my_addr;
    memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_addr.s_addr = INADDR_ANY;
    my_addr.sin_port = htons(port);

    if (0 > (server_sockfd = socket(PF_INET, SOCK_STREAM, 0)))
    {
        perror("socket");
        return 1;
    }

    if (0 > bind(server_sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)))
    {
        perror("bind error");
        return 1;
    }

    listen(server_sockfd, 1);

    int client_sockfd;
    struct sockaddr_in remote_addr;
    int sin_size = sizeof(struct sockaddr_in);
    while (runFlag)
    {
        if (0 > (client_sockfd = accept(server_sockfd, (struct sockaddr *)&remote_addr, &sin_size)))
        {
            perror("accept");
            return 1;
        }

        int len;
        char bufIn[BUFSIZ];
        char bufOut[BUFSIZ];
        while (runFlag)
        {
            len = recv(client_sockfd, bufIn, BUFSIZ, 0);
            if (len <= 0)
                break;

            int outputLen = procCmd(bufIn, len, bufOut, BUFSIZ);
            if (outputLen > 0)
            {
                if (send(client_sockfd, bufOut, outputLen, 0) < 0)
                {
                    perror("send failed");
                    break;
                }
            }
        }

        close(client_sockfd);
    }
    return NULL;
}

int initNet(int srvPort)
{
    port = srvPort;
    pthread_create(&thread, NULL, run, NULL);
    return 0;
}

void shutdownNet()
{
    runFlag = 0;
    close(server_sockfd);
}
