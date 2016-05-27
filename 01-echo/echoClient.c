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
	
	char buf[512];
	while(fgets(buf,sizeof(buf),stdin)!=NULL)
	{
		write(sock , buf , sizeof(buf));	
		memset(buf,0,sizeof(buf));
		int readBytes = read(sock , buf , sizeof(buf));
		if(readBytes>0)
		{
			printf("echo:");
			fputs(buf,stdout);
		}
		memset(buf,0,sizeof(buf));
	}
	close(sock);
	return 0;
}
