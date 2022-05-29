#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char *argv[])
{
    struct sockaddr_in remote_addr; // 服务器端网络地址结构体
    int len, client_sockfd;
    // 数据初始化--清零
    memset(&remote_addr, 0, sizeof(remote_addr));
    remote_addr.sin_family = AF_INET;                         // 设置为IP通信
    remote_addr.sin_addr.s_addr = inet_addr("192.168.1.121"); // 服务器IP地址
    remote_addr.sin_port = htons(12522);                      // 服务器端口号
    // 创建客户端套接字--IPv4协议，面向连接通信，TCP协议
    if (0 > (client_sockfd = socket(PF_INET, SOCK_STREAM, 0)))
    {
        perror("socket");
        return 1;
    }
    // 将套接字绑定到服务器的网络地址上
    if (0 > connect(client_sockfd, (struct sockaddr *)&remote_addr, sizeof(struct sockaddr)))
    {
        perror("connect");
        return 1;
    }
    printf("connected to server\n");
    // 循环的发送接收信息并打印接收信息--recv返回接收到的字节数，send返回发送的字节数

    char buf[BUFSIZ]; // 数据传送的缓冲区
    char cmd[BUFSIZ];
    while (1)
    {
        printf("Enter cmd:");
        //scanf("%s", cmd);
        gets(cmd);
        //cmd[2]='\0';
        //int aa = atoi(cmd+2);
        //printf("aa=%s\n", aa);
        buf[0] = 0x7e;
        buf[1] = 5;
        //printf("cmd:%s, len:%s\n", cmd, strlen(cmd));
        if (cmd[0] == 'l')
        {
            buf[2] = 0x00;
            buf[3] = 0x00;
            int angle = atoi(cmd + 2);
            //printf("angle %s\n", angle);
            char tmp = (char) angle;
            buf[4] = (char) angle;
        }
        else if (cmd[0] == 'r')
        {
            buf[2] = 0x00;
            buf[3] = 0x01;
            int angle = atoi(cmd + 2);
            buf[4] = (char)angle;
        }
        else if (cmd[0] == 'f')
        {
            buf[2] = 0x01;
            buf[3] = 0x00;
            int speed = atoi(cmd + 2);
            if (speed > 10)
            {
                continue;
            }
            buf[4] = (char)speed;
        }
        else if (cmd[0] == 'b')
        {
            buf[2] = 0x01;
            buf[3] = 0x01;
            int speed = atoi(cmd + 2);
            if (speed > 10)
            {
                continue;
            }
            buf[4] = (char)speed;
        }

        len = send(client_sockfd, buf, 5, 0);
        // 接收服务器端信息
        len = recv(client_sockfd, buf, BUFSIZ, 0);
        printf("status:%d\n", buf[len-1]);
    }
    close(client_sockfd); // 关闭套接字
    return 0;
}
