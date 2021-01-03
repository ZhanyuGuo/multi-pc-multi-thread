#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main()
{
    struct sockaddr_in srvAddr;
    bzero(&srvAddr,sizeof(srvAddr));
    srvAddr.sin_family = AF_INET;
    srvAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    srvAddr.sin_port = htons(8888);
    int srvAddrLen = sizeof(srvAddr);
 
    int iSock = socket(AF_INET, SOCK_DGRAM, 0); //对于该参数我们仅需熟记AF_INET和AF_INET6即可, SOCK_DGRAM,SOCK_STREAM
    bind(iSock, (struct sockaddr *)&srvAddr, sizeof(srvAddr));
 
    while(1)
    {
        printf("press enter...");
		getchar(); // 卡住
		char szBuf[1024] = {0};
        recvfrom(iSock, szBuf, sizeof(szBuf) - 1, 0, (struct sockaddr *)&srvAddr, (socklen_t*)&srvAddrLen); // 注意: 这里用srvAddr是不好的，应该用cliAddr,表示对方地址
        printf("msg from client [%s]\n", szBuf);
    }
 
    close(iSock);
    return 0;
}
