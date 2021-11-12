#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<assert.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>

int main()
{
	int sockfd = socket(AF_INET,SOCK_STREAM,0);
	assert(sockfd != -1);

	struct sockaddr_in saddr,caddr;
	memset(&saddr,0,sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(6000);
	saddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	int res = connect(sockfd,(struct sockaddr*)&saddr,sizeof(saddr));
	assert(res != -1);
	listen(sockfd,5);
	while(1)
	{
		printf("input:\n");
		//输入缓冲区
		char buff[128] = {0};
		fgets(buff,128,stdin);
	    
	    //如果输入的数据为"end"，表示要关闭客户端，此时跳出循环
  		if(strncmp(buff,"end",3 ) == 0)
		{ 
			break;
		}
		//否则就发送所输入的数据
		send(sockfd,buff,strlen(buff),0);
		//再将缓冲区清零
		memset(buff,0,128);
		//接收服务器端的回复
		recv(sockfd,buff,127,0);
		printf("buff = %s\n",buff);
	}
	//关闭该连接
    close(sockfd); 
    return 0;
}