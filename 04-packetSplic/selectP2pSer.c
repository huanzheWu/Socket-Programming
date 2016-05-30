
/*
单进程服务器
实现简单点到点的聊天功能
使用select函数来处理IO
创建日期：2016/5/30
*/
#include<stdlib.h>
#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<string.h>
#include<unistd.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<signal.h>
#include <sys/select.h>
#include <sys/time.h>
#include"common.h"

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
	conn = accept (listenfd ,(struct sockaddr *)&peerAddr,&peerlen);
	if(conn <0)
	{
		err_exit("accept");
	}
	
	printf("客户端的IP地址是：%s,端口号是：%d\n",
		inet_ntoa(peerAddr.sin_addr),ntohs(peerAddr.sin_port));
	

	fd_set fds ;
	int maxfds ;
	if(STDIN_FILENO >conn)	
		maxfds = STDIN_FILENO;
	else 
		maxfds = conn;

	while(1)
	{
		FD_ZERO(&fds);
        	FD_SET(STDIN_FILENO,&fds);
        	FD_SET(conn,&fds); 
		int  nready = select(maxfds,&fds, NULL,NULL,NULL);
		if(nready == -1)
			err_exit("select");
		else if(nready == 0)
			continue;
		
		if(FD_ISSET(STDIN_FILENO,&fds))//键盘有数据可读
		{
			printf("xxxxxxxxxxxxxxx");
		
			struct packet writebuf; 
			memset(&writebuf,0,sizeof(writebuf));
			fgets(writebuf.data,sizeof(writebuf.data),stdin);
			int n = strlen(writebuf.data);
			writebuf.msgLen =htonl(n);
			writen(conn,&writebuf,4+n);//发送
			memset(&writebuf,0,sizeof(writebuf));
					
		}
		if(FD_ISSET(conn,&fds)) //有数据读
		{
			printf("有数据读");
			struct packet readbuf ;
			memset(&readbuf,0,sizeof(readbuf));
                        int ret = readn(conn,&readbuf.msgLen,4); //先读取四个字节，确定后续数据的长度
                        if(ret == -1)
                        {
                                err_exit("readn");
                        }
                        else if(ret == 0)
                        {
                                printf("peer1 close\n");
                                break;
                        }
                        
                        int dataBytes = ntohl(readbuf.msgLen); //字节序的转换
                        int readBytes = readn(conn,readbuf.data,dataBytes); //读取出后续的数据
 			 if(readBytes<0)
                        {
                                err_exit("read");
                        }
                        fputs(readbuf.data,stdout);
                }                              
                       						
	}	
	close(listenfd);
	close(conn);
	return 0;
}

