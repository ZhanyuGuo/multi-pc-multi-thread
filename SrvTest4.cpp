#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#define SERVER_PORT 8888
#define BUF_LEN 1024

void severMsgRecevier(int fd)
{
    char buf[BUF_LEN];  // buf, 1024 bytes
    socklen_t len;
    struct sockaddr_in client_addr;
    double a, rlt;
    while(1)
    {
        len = sizeof(client_addr);
        recvfrom(fd, &a, sizeof(a), 0, (struct sockaddr*)&client_addr, &len);  // recvfrom spinning, wait for msg
        rlt = sin(a);
        printf("recevie from client:%f\n", a);  // print msg from client
        memset(buf, 0, BUF_LEN);
        sprintf(buf, "The answer of sin(a) is %f\n", rlt);  // reply to client
        printf("send to client:%s\n", buf);  // print reply
        sendto(fd, buf, BUF_LEN, 0, (struct sockaddr*)&client_addr, len);  // send to client
    }
}

int main(int argc, char* argv[])
{
    printf("Hello, I'm server!\r\n");

    int server_fd, ret;
    struct sockaddr_in server_addr;

    server_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(server_fd < 0)
    {
        printf("create socket fail!\n");
        return -1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(SERVER_PORT);

    ret = bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if(ret < 0)
    {
        printf("socket bind fail!\n");
        return -1;
    }

    severMsgRecevier(server_fd);

    close(server_fd);
    return 0;
}

/* -------------------------------------------------
 * int socket(int domain, int type, int protocol);
 * 
 * @param: domain: AF_INET      -> IPV4
 *                 AF_INET6     -> IPV6
 * 
 * @param: type: SOCK_DGRAM     -> UDP
 *               SOCK_STREAM    -> TCP
 * 
 * @param: protocol: set to 0 is OK.
 * 
 * @return: suc:    an int
 *          fail:   -1
 * ------------------------------------------------*/


/*---------------------------------------------------
 * ssize_t sendto(int sockfd, const void *buf, size_t len, int flags,
 *                  const struct sockaddr *dest_addr, socklen_t addrlen);
 * 
 * @param: sockfd:  an int return from socket.
 * @param: buf: buffer.
 * @param: len: length of bytes to send.
 * @param: flags: set to 0 is ok.
 * @param: dest_addr: destination address.
 * @param: addrlen: length of destination address.
 * 
 * @return: suc:    len
 *          fail:   -1
 * ------------------------------------------------*/ 

/*---------------------------------------------------
 * ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags,
 *                    struct sockaddr *src_addr, socklen_t *addrlen);
 * 
 * @param: sockfd:  an int return from socket.
 * @param: buf: buffer.
 * @param: len: length of bytes to recv.
 * @param: flags: set to 0 is ok.
 * @param: src_addr: source address.
 * @param: addrlen: length of source address.
 * 
 * @return: suc:    len
 *          fail:   -1
 * ------------------------------------------------*/ 


/*---------------------------------------------------
 * int bind(int sockfd, const struct sockaddr* my_addr, socklen_t addrlen);
 * 
 * @param: sockfd:  an int return from socket.
 * @param: my_addr: ip and port.
 * @param: addrlen: size of my_addr.
 * 
 * @return: suc:    0
 *          fail:   -1
 * ------------------------------------------------*/ 


/*---------------------------------------------------
 * int close(int fd);
 * 
 * @param: fd: an int return from socket.
 * -------------------------------------------------*/