#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_FD 10 //设定最多监听的文件描述符的数量为10
int fds[MAX_FD]; //存储多个文件描述符的数组

//初始化文件描述符数组
void fds_init()
{
	int i = 0;
	for(;i<MAX_FD;i++)
	{
		fds[i] = -1; //将所有的元素初始化为-1
	}
}

//在文件描述符数组中寻找第一个值为-1的元素，即此元素存放新添加的文件描述符
void fds_add(int fd)
{
	int i = 0;
	for(;i<MAX_FD;i++)
	{
		if(fds[i] == -1)
		{
			fds[i] = fd;
			break;
		}
	}
}

//在文件描述符数组中寻找指定值的元素，将其删除，即将该元素置为-1
void fds_del(int fd)
{
	int i = 0;
	for(;i<MAX_FD;i++)
	{
		if(fds[i] == fd)
		{
			fds[i] = -1;
			break;
		}
	}
}

int main()
{
	int sockfd = socket(AF_INET,SOCK_STREAM,0);
	assert(sockfd != -1);

	struct sockaddr_in saddr,caddr;
	memset(&saddr,0,sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(6000);
	saddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	int res = bind(sockfd,(struct sockaddr *)&saddr,sizeof(saddr));
	assert(res != -1);

	listen(sockfd,5);
	
	//这里只对读事件进行验证，所以只定义一个fd_set结构
	fd_set fdset;
	//初始化存放文件描述符的数组
	fds_init();

	fds_add(sockfd); //将socket也作为要监听的文件描述符
	
	//服务器开始进入while循环
	while(1)
	{
		//因为调用select后，有可能改变文件描述符在fdset位图中的标志，所以每一次开始监听之前都要重新轻灵标志
		FD_ZERO(&fdset);
		int maxfd = -1; //记录当前要监听的文件描述符的最大值

		int i = 0;
		for(; i < MAX_FD; ++i)
		{
			if(fds[i] != -1) //如果文件描述符数组中的元素不为-1，就将此元素设置到fds位图中
			{
				FD_SET(fds[i], &fdset);
				if(fds[i] > maxfd)
				{
					maxfd = fds[i];
				}
			}
		}
		//设置浅睡眠时间为5毫秒整
		struct timeval tv = {5,0};
		
		//返回值为一共就绪事件的总和
		int n = select(maxfd + 1, &fdset, NULL, NULL, &tv);
		if(n == -1) //如果返回值为-1，表示出错
		{
			perror("select error");
			continue;
		}
		else if(n == 0) //如果返回值为0，表示睡眠期间没有事件就绪，继续循环监听
		{
			printf("time out\n");
			continue;
		}
		else
		{
			int i = 0;
			//再一次遍历文件描述符数组，查看哪一个是就绪事件
			for(; i < MAX_FD; ++i)
			{
				if(fds[i] == -1)
				{
					continue;
				}
				//测试此文件描述符元素在位图中对应的位是否被设置
				if(FD_ISSET(fds[i], &fdset))
				{
					//如果就绪的是sockfd，表示有了新连接
					if(sockfd == fds[i])
					{
						int len = sizeof(caddr);
						int c = accept(sockfd, (struct sockaddr *)&caddr, &len);

						if(c < 0)
						{
							continue;
						}
						fds_add(c);
					}
					else //否则就读取就绪事件的数据
					{
						char buff[128] = {0};
						//如果返回值小于等于0，表示连接中断或出错
						if(recv(fds[i], buff, 127, 0) <= 0)
						{
							//此时需要关闭该连接
							close(fds[i]);
							//并从文件描述符数组中删除此元素
							fds_del(fds[i]);
							printf("one client over\n");
							continue;
						}
						printf("read:%s\n",buff);
						//读取全部数据后，向客户端发送"ok"
						send(fds[i], "ok", 2, 0);
					}
				}
			}
		}
	}

	return 0;
}