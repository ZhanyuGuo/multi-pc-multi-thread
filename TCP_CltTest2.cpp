#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#define PORT 8888
#define SERVER_IP "127.0.0.1"
#define BUF_LEN 1024

int clientHandler(int client_fd)
{
    double a, rlt;
    int send_len, recv_len;
    while (1)
    {
        printf("input a: ");
        scanf("%lf", &a);
        send_len = send(client_fd, &a, sizeof(a), 0);
        if (send_len < 0)
        {
            printf("client send failed...\r\n");
            return -1;
        }

        recv_len = recv(client_fd, &rlt, sizeof(rlt), 0);
        if (recv_len < 0)
        {
            printf("client recv failed...\r\n");
            return -1;
        }
        printf("Anser of sin(%f) = %f\r\n", a, rlt);
    }
    return 0;
}

int main(int argc, char const *argv[])
{
    int client_fd;
    int ret;
    int send_len;
    int recv_len;
    char buf[BUF_LEN];
    struct sockaddr_in remote_addr;
    memset(&remote_addr,0,sizeof(remote_addr));
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    remote_addr.sin_port = htons(PORT);

    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0)
    {
        printf("create client_fd failed...\r\n");
        return -1;
    }

    ret = connect(client_fd, (struct sockaddr*)&remote_addr, sizeof(remote_addr));
    if (ret < 0)
    {
        printf("connect failed...\r\n");
        return -1;
    }

    printf("connect to server!\r\n");
    recv_len = recv(client_fd, buf, BUF_LEN, 0);
    if (recv_len < 0)
    {
        printf("client recv failed...\r\n");
        return -1;
    }
    buf[recv_len] = '\0';
    printf("received: %s\r\n", buf);

    clientHandler(client_fd);

    close(client_fd);
    return 0;
}
