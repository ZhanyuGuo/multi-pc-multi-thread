#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8888
#define BUF_LEN 512

void clientMsgSender(int fd, struct sockaddr* dst)
{
    socklen_t len;
    struct sockaddr_in src;
    while(1)
    {
        char buf[BUF_LEN] = "TEST UDP MSG!\n";
        len = sizeof(*dst);
        printf("client:%s\n", buf);  // print msg to send
        sendto(fd, buf, BUF_LEN, 0, dst, len);
        memset(buf, 0, BUF_LEN);
        recvfrom(fd, buf, BUF_LEN, 0, (struct sockaddr*)&src, &len);  // receive from server
        printf("server:%s\n", buf);
        sleep(1);  // send per second
    }
}

int main(int argc, char* argv[])
{
    printf("Hello, I'm client!\r\n");

    int client_fd;
    struct sockaddr_in server_addr;

    client_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(client_fd < 0)
    {
        printf("create socket fail!\n");
        return -1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    // server_addr.sin_addr.s_addr = htonl(INADDR_ANY);  //注意网络序转换
    server_addr.sin_port = htons(SERVER_PORT);  //注意网络序转换

    clientMsgSender(client_fd, (struct sockaddr*)&server_addr);

    close(client_fd);

    return 0;
}
