#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#define PORT 8888
#define BUF_LEN 1024

char buf[BUF_LEN];

int serverHandler(int client_fd)
{
    int recv_len, send_len;
    double a, rlt;
    while (1)
    {
        // memset(buf, 0, BUF_LEN);
        recv_len = recv(client_fd, &a, sizeof(a), 0);
        if (recv_len < 0)
        {
            printf("server recv failed...\r\n");
            return -1;
        }
        // buf[recv_len] = '\0';
        printf("Received: %f\r\n", a);
        rlt = sin(a);

        send_len = send(client_fd, &rlt, sizeof(rlt), 0);
        if (send_len < 0)
        {
            printf("server send failed...\r\n");
            return -1;
        }
        printf("Send: %f\r\n", rlt);
    }
    return 0;
}

int main(int argc, char const *argv[])
{
    int server_fd;
    int client_fd;
    int ret;
    int addr_len;
    int send_len;
    int sin_size;
    
    struct sockaddr_in my_addr;
    struct sockaddr_in remote_addr;

    memset(&my_addr,0,sizeof(my_addr));
    my_addr.sin_family = AF_INET;  // IPV4
    my_addr.sin_addr.s_addr = htons(INADDR_ANY); // local ip
    my_addr.sin_port = htons(PORT);  // port

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        printf("create server_fd failed...\r\n");
        return -1;
    }

    ret = bind(server_fd, (struct sockaddr*)&my_addr, sizeof(my_addr));
    if (ret < 0)
    {
        printf("bind server_fd failed...\r\n");
        return -1;
    }

    listen(server_fd, 5);

    sin_size = sizeof(struct sockaddr_in);
    client_fd = accept(server_fd, (struct sockaddr*)&remote_addr, (socklen_t *)&sin_size);
    if (client_fd < 0)
    {
        printf("accept connection failed...\r\n");
        return -1;
    }

    printf("accept client %s\r\n", inet_ntoa(remote_addr.sin_addr));
    memset(buf, 0, BUF_LEN);
    sprintf(buf, "Welcome Client!\r\n");
    send_len = send(client_fd, buf, sizeof(buf), 0);
    if (send_len < 0)
    {
        printf("server send failed...\r\n");
        return -1;
    }

    serverHandler(client_fd);

    close(client_fd);
    close(server_fd);
    return 0;
}
