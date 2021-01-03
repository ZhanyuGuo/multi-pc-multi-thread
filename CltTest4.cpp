#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <iostream>
using namespace std;

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8888
#define BUF_LEN 1024

void clientMsgSender(int fd, struct sockaddr* server_addr)
{
    char buf[BUF_LEN];
    socklen_t len;
    struct sockaddr_in src;
    double a, rlt;
    while(1)
    {
        cout << "input a:";
        cin >> a;
        len = sizeof(*server_addr);
        printf("send to server:%f\n", a);  // print msg to send
        sendto(fd, &a, sizeof(a), 0, server_addr, len);

        memset(&buf, 0, sizeof(buf));
        recvfrom(fd, buf, BUF_LEN, 0, (struct sockaddr*)&src, &len);  // receive from server
        printf("receive from server:%s\n", buf);
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
    // server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(SERVER_PORT);

    clientMsgSender(client_fd, (struct sockaddr*)&server_addr);

    close(client_fd);

    return 0;
}
