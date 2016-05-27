#include<stdlib.h>
#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<string.h>
#include<unistd.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include"common.h"

/*
服务器端多线程，可以接受多个客户端的连接
*/

int main()
{
	int listenfd;
	listenfd = socket(PF_INET , SOCK_STREAM, 0);
	if(listenfd <0)
	{
		err_exit("socket");
	}

	struct sockaddr_in addr;
	addr.sin_family = PF_INET;
	addr.sin_port = htons(5888);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	//设置地址可重复利用
	int on = 1;
	setsockopt(listenfd, SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));	

	int ret = bind(listenfd , (const struct sockaddr *)&addr,sizeof(addr));
	if(ret == -1)
	{
		err_exit("bind");
	}
	
	ret = listen(listenfd , SOMAXCONN);
	if(ret < 0)
	{
		err_exit("listen");
	}

	struct sockaddr_in peerAddr ;
	socklen_t peerlen = sizeof(peerAddr);//peerlen一定要有初始值
	
	int conn;
	while(1)
	{
		conn = accept (listenfd ,(struct sockaddr *)&peerAddr,&peerlen);
		if(conn <0)
		{
			err_exit("accept");
		}
	
		pid_t pid ;
		pid  = fork();
		if(pid == 0)  //子进程负责进行与客户端交互
		{
			close(listenfd); //子进程不需要监听套接字
			printf("客户端的IP地址是：%s,端口号是：%d\n",
			inet_ntoa(peerAddr.sin_addr),ntohs(peerAddr.sin_port));
	
			//定义一个应用层的缓冲区，用于接收数据
			char buf[512];
			while(1)
			{
				memset(buf,0,sizeof(buf));
				int readBytes = read(conn , buf , sizeof(buf));
				if(readBytes==0) //说明客户端已经关闭
				{
					printf("clinent close\n");
					break;	
				}
				fputs(buf,stdout);
				write ( conn , buf, sizeof(buf));
			}
			//能够执行到这里说明客户端已经关闭
			exit(EXIT_SUCCESS);//退出进程
		}	
		close(conn);
	}
	
	return 0;
}
