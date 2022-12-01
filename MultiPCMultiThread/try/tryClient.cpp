#define CLIENT
#include "mySrvClt.hpp"

int main(int argc, char const *argv[])
{
    /* ------------------------------
     *
     * 网络设置（Client）
     *  
     * -----------------------------*/

    // Client socket描述符
    int client_fd;
    // My address, remote address
    struct sockaddr_in remote_addr;
    // 一些网络收发相关变量
    int ret, recv_len, send_len;

    // 设置地址
    memset(&remote_addr,0,sizeof(remote_addr));         // reset
    remote_addr.sin_family = AF_INET;                   // IPV4
    remote_addr.sin_addr.s_addr = inet_addr(SERVER_IP); // Server IP
    remote_addr.sin_port = htons(PORT);                 // Port

    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("create client_fd failed...\r\n");
        return -1;
    }

    if ((ret = connect(client_fd, (struct sockaddr*)&remote_addr, sizeof(remote_addr))) < 0)
    {
        printf("connect failed...\r\n");
        return -1;
    }
    printf("Connect to server!\r\n");
    
    if ((recv_len = recv(client_fd, buf, BUF_LEN, 0)) < 0)
    {
        printf("client recv failed...\r\n");
        return -1;
    }
    printf("# Received: %s\r\n", buf);

    return 0;
}
