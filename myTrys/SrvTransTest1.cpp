#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#define PORT 8888
#define BUF_LEN 8000*4
#define RECV_LEN 8000
double recvBuf[BUF_LEN];

int main(int argc, char const *argv[])
{
    printf("Server inited...\r\n");

    int server_fd;
    int client_fd;
    int ret;
    int sin_size;
    int recv_len;

    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;

    memset(&server_addr,0,sizeof(server_addr));
    server_addr.sin_family = AF_INET;  // IPV4
    server_addr.sin_addr.s_addr = htons(INADDR_ANY); // local ip
    server_addr.sin_port = htons(PORT);  // port

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("create server_fd failed...\r\n");
        return -1;
    }

    if ((ret = bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr))) < 0)
    {
        printf("bind server_fd failed...\r\n");
        return -1;
    }

    printf("Server inited succeed.\r\n");
    listen(server_fd, 5);

    sin_size = sizeof(struct sockaddr_in);
    if ((client_fd = accept(server_fd, (struct sockaddr*)&client_addr, (socklen_t *)&sin_size)) < 0)
    {
        printf("accept connection failed...\r\n");
        return -1;
    }

    printf("accept client %s\r\n", inet_ntoa(client_addr.sin_addr));

    // if ((recv_len = recv(client_fd, recvBuf, BUF_LEN*sizeof(double), 0)) < 0)
    // {
    //     printf("recv failed...\r\n");
    //     return -1;
    // }
    double *p = recvBuf;
    for (int i = 0; i < BUF_LEN/RECV_LEN; i++)
    {
        if ((recv_len = recv(client_fd, p, RECV_LEN*sizeof(double), 0)) < 0)
        {
            printf("recv failed...\r\n");
            return -1;
        }
        printf("%d, %lf\r\n", recv_len, *p);
        p += RECV_LEN;
    }
    printf("recived succeed...\r\n");
    
    // for (int i = 0; i < BUF_LEN; i++)
    // {
    //     printf("%lf ", recvBuf[i]);
    //     if (i % 5 == 0)
    //     {
    //         printf("\r\n");
    //     }
        
    // }
    
    return 0;
}
