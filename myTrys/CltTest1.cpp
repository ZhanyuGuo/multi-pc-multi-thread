
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
    bzero(&srvAddr, sizeof(srvAddr));
    srvAddr.sin_family = AF_INET;
    srvAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    srvAddr.sin_port = htons(8888);
 
    int iSock = socket(AF_INET, SOCK_DGRAM, 0); // udp
	int i = 0;
    while(1)
    {
		printf("press enter to send data\n");
		getchar(); // 卡住
		char szBuf[32] = {0};
        snprintf(szBuf, sizeof(szBuf), "hello %d", ++i);
        sendto(iSock, szBuf, strlen(szBuf) + 1, 0, (struct sockaddr *)&srvAddr, sizeof(srvAddr));
    }
 
	close(iSock);
    return 0;
}