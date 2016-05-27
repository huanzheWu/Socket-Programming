#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netinet/in.h>
#include<arpa/inet.h>
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
	
	char readBuf[512];
	char writeBuf[512];
	pid_t pid ;
	pid = fork();
	if(pid == 0)//子进程复制读取
	{
		
                while(1)
                {

                        memset(writeBuf,0,sizeof(writeBuf));
                        fgets(writeBuf,sizeof(writeBuf),stdin);
                        write(sock,writeBuf,sizeof(writeBuf));
                }	
	}
	else if( pid >0)//父进程
	{
		 while(1)
                {
                        
                        memset(readBuf,0,sizeof(readBuf));
                        int readBytes = read(sock,readBuf,sizeof(readBuf));
                        if(readBytes == 0)
                        {       
                                printf("client close\n");
                                break;
                        }
                        if(readBytes<0)
                        {       
                                err_exit("read");
                        }
                        fputs(readBuf,stdout);
                }
                exit(EXIT_SUCCESS);	
	}
	
	close(sock);
	return 0;
}
