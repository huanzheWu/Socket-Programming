#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<signal.h>
#include"common.h"
int main()
{
	int sock ;
	sock = socket(PF_INET,SOCK_STREAM,0);
	if(sock <0)
	{
		err_exit("socket");		
	}
	
	struct sockaddr_in addr;
	memset(&addr,0,sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(5888);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	
	int ret ;
	ret = connect (sock , (struct sockaddr*)&addr,sizeof(addr));
	if(ret < 0)
	{
		err_exit("connect");
	} 
	
	//char readBuf[512];
	//char writeBuf[512];
	pid_t pid ;
	pid = fork();
	if(pid == 0)//子进程从键盘输入
	{
		struct packet writeBuf;

		
                while(fgets(writeBuf.data,sizeof(writeBuf.data),stdin)!=NULL)
                {
			int n = strlen(writeBuf.data);
			writeBuf . msgLen  =htonl(n);
			
                        writen(sock,&writeBuf,4+n);
                        memset(&writeBuf,0,sizeof(writeBuf));
                }
		//子进程结束输入，此时应该通知父进程也结束，使用信号
		kill(getppid(),SIGUSR1);	
		exit(EXIT_SUCCESS);	
	}	
	else if( pid >0)//父进程
	{
		signal (SIGUSR1,handler);  //信号，收到SIGUSR1信号后调用handler处理函数
		 while(1)
                {
 			struct packet readBuf;
                       
                        memset(&readBuf,0,sizeof(readBuf));
                     	int ret = readn(sock,&readBuf.msgLen ,4); //先读取前4个字节 ，获得data长度
			if(ret == -1)
			{
				err_exit("readn");
			}
			else if(ret == 0)
			{
				printf("peer close\n");
				break;
			}

			int dataBytes = ntohl(readBuf.msgLen);  //注意字节序的转换
			int readBytes = readn(sock,readBuf.data,dataBytes); //读取data

                        if(readBytes == 0)//对方关闭，则应该退出该进程
                        {       
                                printf("peer close\n");
                                break;
                        }
                        if(readBytes<0)
                        {       
                                err_exit("read");
                        }
                        fputs(readBuf.data,stdout);
                }
	}	
	close(sock);
	return 0;
}
