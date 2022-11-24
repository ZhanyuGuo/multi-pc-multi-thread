#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8888
#define BUF_LEN 8000*4
#define SEND_LEN 8000
double sendBuf[BUF_LEN];

int main(int argc, char const *argv[])
{
    int client_fd;
    int ret;
    int send_len;
    struct sockaddr_in server_addr;

    for (int i = 0; i < BUF_LEN; i++)
    {
        sendBuf[i] = double(i);
    }
    

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(SERVER_PORT);

    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("create client_fd failed...\r\n");
        return -1;
    }

    
    if ((ret = connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr))) < 0)
    {
        printf("connect failed...\r\n");
        return -1;
    }

    printf("connect to server!\r\n");

    double *p = sendBuf;
    for (int i = 0; i < BUF_LEN/SEND_LEN; i++)
    {
        if ((send_len = send(client_fd, p, SEND_LEN*sizeof(double), 0)) < 0)
        {
            printf("send failed...\r\n");
            return -1;
        }
        printf("%d, %lf\r\n", send_len, *p);
        p += SEND_LEN;
    }

    // if ((send_len = send(client_fd, sendBuf, BUF_LEN*sizeof(double), 0)) < 0)
    // {
    //     printf("send failed...\r\n");
    //     return -1;
    // }
    
    
    
    close(client_fd);

    return 0;
}
