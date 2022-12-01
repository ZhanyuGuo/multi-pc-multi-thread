#define SERVER
#include "mySrvClt.hpp"

int main(int argc, char const *argv[])
{
    /* ------------------------------
     *
     * 网络设置（Server）
     *
     * -----------------------------*/

    // Server, Client socket描述符
    int server_fd, client_fd;
    // My address, remote address
    struct sockaddr_in my_addr, remote_addr;
    // 一些网络收发相关变量
    int ret, recv_len, send_len, sin_size;

    // 设置地址
    memset(&my_addr, 0, sizeof(my_addr)); // reset
    my_addr.sin_family = AF_INET;         // IPV4
    my_addr.sin_addr.s_addr = INADDR_ANY; // Local IP
    my_addr.sin_port = htons(PORT);       // Port

    if ((server_fd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("create server_fd failed...\r\n");
        return -1;
    }

    if ((ret = bind(server_fd, (struct sockaddr *)&my_addr, sizeof(my_addr))) < 0)
    {
        printf("bind server_fd failed...\r\n");
        return -1;
    }

    // 等待连接
    listen(server_fd, 5);

    // 只针对一个client的情况
    sin_size = sizeof(struct sockaddr_in);
    if ((client_fd = accept(server_fd, (struct sockaddr *)&remote_addr, (socklen_t *)&sin_size)) < 0)
    {
        printf("accept connection failed...\r\n");
        return -1;
    }
    printf("# Accept client %s\r\n", inet_ntoa(remote_addr.sin_addr));

    // 对Client的应答
    memset(buf, 0, BUF_LEN);
    sprintf(buf, "Hello Client!");
    if ((send_len = send(client_fd, buf, strlen(buf), 0)) < 0)
    {
        printf("server send failed...\r\n");
        return -1;
    }
    printf("# Send: Hello Client!\r\n");

    return 0;
}
